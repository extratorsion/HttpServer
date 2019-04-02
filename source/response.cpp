#include "response.h"
#include "cgihandler.h"

String Response::makeResponse(const String& status, const std::vector<std::pair<String, String>>& key_value, const String& content) {
    String build(status);
    build += "\r\n";
    for (auto [key, value]: key_value) {
        build += (key + ": " + value + "\r\n");
    }
    build += "\r\n";
    build += content;

    return build;
}

String Response::makeResponse(const String& status, const HashMap<String, String>& key_value, const String& content) {
    String build(status);
    build += "\r\n";
    for (auto [key, value]: key_value) {
        build += (key + ": " + value + "\r\n");
    }
    build += "\r\n";
    build += content;
    return build;
};

String Response::makeResponse(int statusCode, const std::vector<std::pair<String, String>>& key_value, const String& content) {
    return makeResponse(statusMap[statusCode], key_value, content);
}

String Response::makeResponse(int statusCode, const HashMap<String, String>& key_value, const String& content) {
    return makeResponse(statusMap[statusCode], key_value, content);
}

String Response::makeResponseHeader(int statusCode, const HashMap<String, String>& key_value) {
    String build(statusMap[statusCode]);
    build += "\r\n";
    for (auto [key, value]: key_value) {
        build += (key + ": " + value + "\r\n");
    }
    build += "\r\n";
    return build;
}

size_t Response::getFileSize(const String& path) {
    struct stat file_info;
    int ret = stat(path.c_str(), &file_info);
    if (ret != -1)
        return size_t(file_info.st_size);
    else
        return 0;
}

String Response::getContentType(const String& path) {
    size_t pointPos = path.rfind('.');
    String fileType = path[Slice(pointPos+1)];
    if (typeMap.count(fileType) != 0)
        return typeMap.at(fileType);
    else {
        return "text/css";
    }
}

void Response::sendFileEntity(const String& path, int fd) {
    char buf[2048] = {0};
    std::ifstream ifs(path, ios::in | ios::binary);
    if (not ifs) {
        ::send(fd, buf, 0, 0);
    } else {
        while (not ifs.eof()) {
            ifs.read(buf, 2048);
            ::send(fd, buf, size_t(ifs.gcount()), 0);
        }
    }
}

void Response::ProcessResponse(const String& request, int fd) {
    Response response(request, fd);
    response.reply();
}

Response::Response(const Request& req, int fd) {
    fd_ = fd;
    HashMap<String, String> headerMap {
        {"Connection", "close"},
        {"Server", "Test-Server"}
    };

    if (req.method() == GET) {
        int statusCode = 200;


        filename_ = req.request_filename();
        size_t contentLength = getFileSize(filename_);

        /* if file do not exitence */
        if (contentLength == 0) {
            statusCode = 404;
            filename_ = "404.html";
            contentLength = getFileSize(filename_);
        }

        headerMap["Content-Length"] = String::itos(contentLength);
        headerMap["Content-Type"] = getContentType(filename_);

        responseHeader_ = makeResponseHeader(statusCode, headerMap);

        responseType_ = ResponseType::File;

    } else if (req.method() == POST) {
        int statusCode = 204;
        if (CgiHandler::isCgi(req.clean_path())) {
            responseBody_ = CgiHandler::processCgi(req.clean_path(), req.raw_post_values());
            if (responseBody_.size())
                statusCode = 200;

            headerMap["Content-Length"] = String::itos(responseBody_.size());
            responseHeader_ = makeResponseHeader(statusCode, headerMap);

        } else {
            headerMap["Content-Length"] = 0;
            responseHeader_ = makeResponseHeader(204, headerMap);
            responseBody_ = "";
        }
        responseType_ = ResponseType::Content;
    }
}

void Response::reply() {
    switch (responseType_) {
    case ResponseType::File: replyFile(); break;
    case ResponseType::Content: replyContent(); break;
    }
}

void Response::replyFile() {
    ::send(fd_, responseHeader_.c_str(), responseHeader_.size(), 0);
    sendFileEntity(filename_, fd_);
}

void Response::replyContent() {
    ::send(fd_, responseHeader_.c_str(), responseHeader_.size(), 0);
    ::send(fd_, responseBody_.c_str(), responseBody_.size(), 0);
}


HashMap<int, String> Response::statusMap =  {
    {200, "HTTP/1.1 200 OK"},
    {204, "No Content"},
    {206, "Partical Content"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "HTTP/1.1 404 Not Found"},
    {500, "Internal Server Error"}
};

HashMap<String, String> Response::typeMap = {
    {"jpg", "image/jpg"},
    {"png", "image/png"},
    {"css", "text/css"},
    {"html", "text/html"},
    {"htm", "text/html"},
    {"js", "application/javascript; charset=utf-8"}

};

char* Response::gbuf = new char[GBUF_SIZE];
