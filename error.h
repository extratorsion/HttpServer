#ifndef ERROR_H
#define ERROR_H
#include <string>
#include <cstring>
#include <stdexcept>

class BaseError: public std::exception {
protected:
    char* err_msg_ = nullptr;
public:
    using std::exception::exception;

    BaseError(const char* err_msg) {
        err_msg_ = new char[strlen(err_msg) + 1];
        strcpy(err_msg_, err_msg);
    }

    BaseError(const std::string& err_str) {
        err_msg_ = new char[err_str.size()+ 1];
        strcpy(err_msg_, err_str.c_str());
    }

    virtual const char* what() const noexcept {
        return err_msg_;
    }

    virtual ~BaseError() {
        if (err_msg_)
            delete err_msg_;
    }
};

class FixedError : public BaseError {
protected:
    const char * common_output;
public:
    using BaseError::BaseError;
    FixedError(const char* err_msg, const char * at=nullptr) {
        common_output = "Error: ";
        construte(err_msg, at);
    }

    void construte(const char* err_msg, const char * at=nullptr) {
         if (at) {
            err_msg_ = new char[strlen(common_output) + strlen(err_msg) + strlen(at) + 2];
            strcpy(err_msg_, common_output);
            strcat(err_msg_, err_msg);
            strcat(err_msg_, " ");
            strcat(err_msg_, at);
        } else {
            err_msg_ = new char[strlen(common_output) + strlen(err_msg) + 1];
            strcpy(err_msg_, common_output);
            strcat(err_msg_, err_msg);
        }

    }
};


class SocketError : public FixedError {
public:
    SocketError(const char* err_msg, const char * at=nullptr) {
        common_output = "SocketError: ";
        construte(err_msg, at);
    }
};


class CgiError : public FixedError {
public:
    CgiError(const char* err_msg, const char * at=nullptr) {
        common_output = "CgiError: ";
        construte(err_msg, at);
    }
};

#endif // ERROR_H
