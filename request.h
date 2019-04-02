#ifndef REQUEST_H
#define REQUEST_H

#include "common_includes.h"


enum Method {GET=1, POST=2};

class Request {
    using Header = std::vector<std::pair<String, String>>;

    String rawRequestPath_;
    Method method_;
    Header header_;
    String content_;
public:
    HashMap<String, String> parserArgs(const String& argStr) const;

protected:
    void parse(const String& request);

public:
    Request(const String& request);

    void assign_content(const String& cons);

    Method method() const;

    bool have_args() const;

    String request_path() const;

    const String& content() const;

    String operator[](const String& key) const;

    bool have_key(const String& key) const;

    HashMap<String, String> post_dict() const;

    String raw_post() const;

    String raw_get() const;

    HashMap<String, String> get_dict() const;

    String clean_path() const;

    String request_filename() const;

    String raw_post_values() const;
};

#endif // REQUEST_H
