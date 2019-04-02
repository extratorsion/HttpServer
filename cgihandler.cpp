#include "cgihandler.h"
#include <sys/wait.h>

HashSet<String> CgiHandler::cgiMap = {
     "add",
     "multiply"
};

String CgiHandler::getCgiProgrmaName(const String& str) {
    String name;
    if (str.startswith("/")) {
        name = ".";
        name += str; }
    else {
        name += "./";
        name += str;
    }
    return name;
}

String CgiHandler::processCgi(const String& cgiName, const String& inputArgs, char sep) {
    String programName = getCgiProgrmaName(cgiName);

    int ftoc[2];
    int ctof[2];
    if (-1 == pipe(ftoc) or -1 == pipe(ctof))
        throw CgiError(strerror(errno), "at pipe");

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        throw CgiError(strerror(errno), "at fork");
    } else if (pid == 0) {
        close(ftoc[1]);
        close(ctof[0]);
        char argbuf[4096] = {0};

        ssize_t buflen = read(ftoc[0], argbuf, sizeof(argbuf));
        std::vector<String> args = String(argbuf).split(sep);

        close(1);
        dup2(ctof[1], 1);

        char ** argVec = new char*[args.size() + 2];
        argVec[0] = const_cast<char*>(programName.c_str());
        int i = 1;
        for (; i <= args.size(); ++i)
            argVec[i] = const_cast<char*>(args[i-1].c_str());
        argVec[i] = nullptr;

        execv(programName.c_str(), argVec);
        delete []argVec;

        exit(0);

    } else {
        close(ftoc[0]);
        close(ctof[1]);

        write(ftoc[1], inputArgs.c_str(), inputArgs.size());

        char buf[4096] = {0};
        read(ctof[0], buf, sizeof(buf));

        int wstatus;
        do {
            wait(&wstatus);
        }while (not WIFEXITED(wstatus));

        return String(buf);
    }
}

bool CgiHandler::isCgi(const String& name) {
    return cgiMap.count(name) != 0;
}
