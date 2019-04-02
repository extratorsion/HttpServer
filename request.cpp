#include "request.h"


HashMap<String, String> Request::parserArgs(const String& argStr) const {
    HashMap<String, String> dict;
    if (argStr.size() == 0)
        return dict;

    std::vector<String> varValues = argStr.split('&');
    for(String& varValue: varValues) {
        dict.insert(varValue.divide('='));
    }
    return dict;
}

void Request::parse(const String& request) {
    auto [header, content] = request.divide("\r\n\r\n");
            content_ = content;
            std::vector<String> lines =  header.split("\r\n");
            std::vector<String> first_line = lines[0].split(' ');
            if (first_line[0] == "GET")
            method_ = GET;
            else if (first_line[0] == "POST")
            method_ = POST;

            rawRequestPath_ = first_line[1];

            size_t i = 1;
            for (; i < lines.size(); ++i) {
        if (lines[i].size()) {
            header_.push_back(lines[i].divide(": "));
        } else {
            break;
        }
    }
}

Request::Request(const String& request) {
    parse(request);
}

void Request::assign_content(const String& cons) {
    content_ = cons;
}

Method Request::method() const {
    return method_;
}

bool Request::have_args() const {
    return -1 != rawRequestPath_.find("?");
}

String Request::request_path() const {
    if (have_args()) {
        return rawRequestPath_[Slice(0, rawRequestPath_.find("?"))];
    } else {
        return rawRequestPath_;
    }
}

const String& Request::content() const {
    return content_;
}

String Request::operator[](const String& key) const{
    for (size_t i = 0; i < header_.size(); ++i) {
        if (key == header_[i].first)
            return header_[i].second;
    }
    return String();
}

bool Request::have_key(const String& key) const {
    for (size_t i = 0; i < header_.size(); ++i) {
        if (key == header_[i].first)
            return true;
    }
    return false;
}

HashMap<String, String> Request::post_dict() const {
    return parserArgs(content_);
}

HashMap<String, String> Request::get_dict() const {
    HashMap<String, String> gdict;
    if (not have_args())
        return gdict;
    String argStr = rawRequestPath_[Slice(rawRequestPath_.rfind("?") + 1)];
    return parserArgs(argStr);
}

String Request::clean_path() const {
    String cleanPath;
    if (rawRequestPath_.startswith("/")){
        if (have_args())
            cleanPath = rawRequestPath_[Slice(1, rawRequestPath_.rfind('?'))];
        else
            cleanPath = rawRequestPath_[Slice(1)];
    } else {
        if (have_args())
            cleanPath =  rawRequestPath_[Slice(0, rawRequestPath_.rfind('?'))];
        else
            cleanPath = rawRequestPath_;
    }
    return cleanPath;
}

String Request::request_filename() const {
    String path = clean_path();
    if (path.endswith("/") or path.length() == 0) {
        path += "index.html";
        return path;
    } else {
        return path;
    }
}

String Request::raw_post_values() const {
   String build;
   HashMap<String, String> dict = post_dict();
   for(auto [key, value]: dict)
       build += (value + " ");
   return build[Slice(0, build.size()-1)];
}

String Request::raw_post() const {
    return content_;
}

String Request::raw_get() const {
    if (not have_args())
        return "";
    return rawRequestPath_[Slice(rawRequestPath_.find('?')+1)];
}

