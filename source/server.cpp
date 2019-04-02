#include "common_includes.h"
#include "response.h"
#include "cgihandler.h"
#include "request.h"
#include "threadpool.h"

/*
 * epoll server:
 *  1. create listen socket: serverFd
 *  2. create epoll listen file describe: epollFd
 *  3. epoll_wait
 *  4. use main thread handle accept
 *  5. mutiple threads handle i/o request
 *      5.1 if method is GET: then return file content
 *      5.2 if method is POST: then use CgiHandler handle cgi program and return the program's ouput.
 */

#pragma pack(2)
struct SocketInfo {
    uint32_t addr; /* ip 地址，网络序 */
    uint16_t port; /* 端口号, 网络序 */

public:
    String getIpaddress() {
        char buf[20] = {0};
        inet_ntop(AF_INET, &addr, buf, sizeof(sockaddr_in));
        return String(buf);
    }

    uint16_t getPort() {
        return  htons(port);
    }
};
#pragma pack()

String ReadAll(int fd) {
    String readBuf;
    ssize_t readSize;
    do {
        char buf[1024] = {0};
        if ((readSize = ::read(fd, buf, sizeof(buf))) < 0) {
            throw SocketError("readSize < 0 at read");
        }
        readBuf += buf;
    } while(readSize == 1024);
    return readBuf;
};

void SetNoneBlock(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int CreateServerSocket() {
    int serverFd = ::socket(AF_INET, SOCK_STREAM , 0);
    if (serverFd < 0)
        throw SocketError("server socket create error");
    int re_user_addr = 1;
    int set_ret = ::setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &re_user_addr, sizeof(re_user_addr));
    if (set_ret == -1)
        throw SocketError("server socket set reuse addr error");


    sockaddr_in addr = {
         AF_INET,
         htons(8000),
         {inet_addr("127.0.0.1")},
    };

    int bind_ret = ::bind(serverFd, reinterpret_cast<sockaddr*>(&addr), socklen_t(sizeof(addr)));

    if (bind_ret < 0)
        throw SocketError(strerror(errno), " at bind");

    int listen_ret = ::listen(serverFd, LISTEN_CNT);
    if (listen_ret < 0)
        throw SocketError("listen failure");
    return serverFd;
}

void HandleConnect(int serverFd, int epollFd, std::unordered_map<int, SocketInfo>& infoMap) {
    sockaddr_in clientAddr;
    socklen_t socklen;
    int conFd = ::accept(serverFd, (sockaddr*)(&clientAddr), &socklen);
    if (conFd < 0) {
        throw SocketError(strerror(errno), "at accept");
    }

    SetNoneBlock(conFd);
    /* 添加连接套接字对象 */

    infoMap[conFd] = SocketInfo{clientAddr.sin_addr.s_addr, clientAddr.sin_port};

    epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = conFd;

    /* 添加到epoll的监听列表中 */
    epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event);

    cout << "connect with " << infoMap[conFd].getIpaddress() << ":" << infoMap[conFd].getPort() << endl;

}

void HandleSigPipe(int signo) {}

std::mutex g_mutex;

int main() {
    try {

        chdir(RUN_DIR);

        signal(SIGPIPE, HandleSigPipe);

        CgiHandler::loadCgiPrograms();

        ThreadPool pool(4);

        std::unordered_map<int, SocketInfo> infoMap;

        auto processTask = [&infoMap](int epollFd, int conFd) {

            String request = ReadAll(conFd);

            /* 如果对端关闭 */
            if (request.length() == 0) {
                epoll_ctl(epollFd, EPOLL_CTL_DEL, conFd, nullptr);
                cout << "close " << infoMap[conFd].getIpaddress() << ":" << infoMap[conFd].getPort() << endl;
                std::unique_lock<std::mutex> lock(g_mutex);
                infoMap.erase(conFd);
                return;
            }

            /* handle request */
            cout << "request from# " << infoMap[conFd].getIpaddress() << ":" << infoMap[conFd].getPort() << endl;

#if defined(SHOW_REQUEST)
            cout << request;
#endif
            Response::ProcessResponse(request, conFd);
        };

        /* 创建监听套接字 */
        int serverFd = CreateServerSocket();

        /* 创建 epoll 对象 */
        int epollFd = epoll_create(50);

        epoll_event serverEpollEvent;
        serverEpollEvent.events = EPOLLIN | EPOLLET;
        serverEpollEvent.data.fd = serverFd;
        SetNoneBlock(serverFd);
        epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &serverEpollEvent);

        epoll_event evs[EVENTS_SIZE];

        while (1) {
            /* 阻塞等待事件 */
            int eventCnt = epoll_wait(epollFd, evs, EVENTS_SIZE, -1);
            if (eventCnt < 0) {
                throw SocketError("epoll error: epoll_wait return < 0");
            } else if (eventCnt == 0) {
                continue;
            } else {
                for (int i = 0; i < eventCnt; ++i) {
                    /* connect请求 */
                    if (evs[i].data.fd == serverFd and (evs[i].events & EPOLLIN)) {
                        HandleConnect(serverFd, epollFd, infoMap);

                    /* HTTP request handle */
                    } else if (evs[i].events & EPOLLIN) {
                        int conFd = evs[i].data.fd;
                        pool.commit(processTask, epollFd, conFd);
                    }
                }
            }
        }

    } catch (std::exception& e) {
        cerr << e.what() << endl;
    } catch (...) {
        cerr << "Error " << endl;
    }
    return 0;
}
