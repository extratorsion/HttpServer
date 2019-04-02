#ifndef RESPONSE_H
#define RESPONSE_H

#include "common_includes.h"
#include "request.h"

#define GBUF_SIZE 102400

class Response {

public:
    static HashMap<int, String> statusMap;

    static HashMap<String, String> typeMap;

    static char* gbuf;

    static String makeResponse(const String& status, const std::vector<std::pair<String, String>>& key_value, const String& content);

    static String makeResponse(const String& status, const HashMap<String, String>& key_value, const String& content);

    static String makeResponse(int statusCode, const std::vector<std::pair<String, String>>& key_value, const String& content);

    static String makeResponse(int statusCode, const HashMap<String, String>& key_value, const String& content);

    static String makeResponseHeader(int statusCode, const HashMap<String, String>& key_value);

    static size_t getFileSize(const String& path);

    static String getContentType(const String& path);

    static void sendFileEntity(const String& path, int fd);

    static void ProcessResponse(const String& request, int fd);

public:
    enum class ResponseType {File, Content};

public:
    String responseHeader_;
    String responseBody_;
    String filename_;
    ResponseType responseType_;
    int fd_;

    Response(const Request& req, int fd);

    void reply();

    void replyFile();

    void replyContent();
};

#endif // RESPONSE_H
