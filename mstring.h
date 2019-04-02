#ifndef MSTRING_H
#define MSTRING_H

#include <string>
#include <functional>
#include <climits>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

class String: public std::basic_string<char> {
    using std::basic_string<char>::basic_string;

public:
    struct Slice {
        int beg;
        int end;
    public:
        Slice(int beg, int end=INT_MAX);
    };

    String(std::string& str);

    bool endswith(const std::string& str) const;

    bool startswith(const std::string& str) const;

    char operator[](size_t pos) const;

    String operator[](const Slice& slice) const;

    String operator+(const basic_string<char>& str) const;

    String operator+(const char* str) const;

    String replace(const std::string& old, const std::string& nw) const;

    std::vector<String> split(char ch) const;

    std::vector<String> split(const String& str) const;

    std::pair<String, String> divide(char ch) const;

    std::pair<String, String> divide(const String& str) const;

    String format(const std::vector<String>& vec);

public:
    static  String itos(int num);

    static String itos(size_t num);

    static String lltos(long long num);

    static int stoi(const String& numstr);

    static long long stoll(const String& numstr);
};

String operator+(const char* str1, const String& str2);

namespace std {
    template <>
struct hash<String> {
        size_t operator() (const String& str) const;
};
}

using Slice = String::Slice;


#endif // STRING_H
