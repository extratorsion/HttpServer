#include "mstring.h"

Slice::Slice(int beg, int end)
    :beg(beg), end(end)
{}

String::String(std::string& str) {
    *this = str;
}

bool String::endswith(const std::string& str) const {
    if (str.size() > this->size())
        return false;
    else
        return this->substr(this->size()-str.size(), str.size()) == str;
}

bool String::startswith(const std::string& str) const {
    if (str.size() > this->size())
        return false;
    else
        return this->substr(0, str.size()) == str;
}

char String::operator[](size_t pos) const {
    return this->at(pos);
}

String String::operator[](const Slice& slice) const {
    auto str = this->substr(slice.beg, slice.end - slice.beg);
    return String(str.begin(), str.end());
}

String String::replace(const std::string& old, const std::string& nw) const {
    String build;
    for(size_t i = 0; i < this->size(); ++i) {
        if (this->at(i)==old[0]) {
            if (i + old.size() <= this->size() and this->substr(i, old.size()) == old) {
                build += nw;
                i += old.size();
                --i;
            }
        }else {
            build.push_back(this->at(i));
        }
    }
    return build;
}

std::vector<String> String::split(char ch) const {
    String tmp;
    std::vector<String> splits;
    for (uint i = 0; i < this->size(); ++i) {
        if (this->at(i) != ch) {
            tmp.push_back(this->at(i));
        }
        else {
            splits.push_back(tmp);
            tmp.clear();
        }
    }
    if (tmp.size()) {
        splits.push_back(tmp);
    }
    return splits;
}

std::vector<String> String::split(const String& str) const {
    String tmp;
    std::vector<String> splits;
    for (uint i = 0; i < this->size(); ++i) {
        if (this->at(i) != str[0]) {
            tmp.push_back(this->at(i));
        }
        else if (this->operator[](Slice(i, i+str.size())) == str){
            splits.push_back(tmp);
            tmp.clear();
            i += str.size();
            --i;
        } else {
            tmp.push_back(this->at(i));
        }
    }
    if (tmp.size()) {
        splits.push_back(tmp);
    }
    return splits;

}

std::pair<String, String> String::divide(char ch) const {
    String first, second;
    size_t i = 0;
    for (; i < this->size(); ++i) {
        if (this->at(i) == ch)
            break;
    }
    if (i != this->size())
        return std::make_pair(String(this->begin(), this->begin()+i), String(this->begin()+i+1, this->end()));
    else {
        return std::make_pair(*this, String());
    }
}

std::pair<String, String> String::divide(const String& str) const {
    String first, second;
    size_t i = 0;
    for (; i < this->size(); ++i) {
        if (!(this->at(i) == str[0])) {
            continue;
        } else {
            if(i + str.size() < this->size() and this->operator[](Slice(i, i+str.size())) == str)
                break;
        }
    }
    if (i != this->size())
        return std::make_pair(String(this->begin(), this->begin()+i), String(this->begin()+i+str.size(), this->end()));
    else {
        return std::make_pair(String(this->begin(), this->begin()+i), String());
    }
}

String String::format(const std::vector<String>& vec) {
    /*
    uint total_size = this->size();
    for (const String& str: vec)
        total_size += str.size();
    total_size -= (2 * vec.size());
    clear_gBuffer(total_size+1);
    uint ind = 0;
    int buf_ind = 0;
    for (uint i = 0; i < this->size(); ++i)
        if (this->at(i) != '{')
            gBuffer_[buf_ind++]=(this->at(i));
        else if (i + 1 < this->size() and this->at(i+1)=='}') {
            for (int j = 0; j < vec[ind].size(); ++j)
                gBuffer_[buf_ind++] = vec[ind][j];
            ++ind;
            ++i;
        } else {
            gBuffer_[buf_ind++]=(this->at(i));
        }
    return String(gBuffer_, gBuffer_+total_size);
    */
    String build;
    uint ind = 0;
    for (uint i = 0; i < this->size(); ++i)
        if (this->at(i) != '{')
            build.push_back(this->at(i));
        else if (i + 1 < this->size() and this->at(i+1)=='}') {
            build += vec[ind++];
            ++i;
        } else {
            build.push_back(this->at(i));
        }
    return build;
}

String String::operator+(const basic_string<char>& str) const {
    String ret(*this);
    ret += str;
    return ret;
}

String String::operator+(const char* str) const {
    String ret(*this);
    ret += str;
    return ret;
}

String String::itos(int num) {
    String s;
    if (num < 0) {
        s += '-';
        uint32_t tmp;
        *(reinterpret_cast<int*>(&tmp)) = num;
        tmp = (~tmp) + 1;
        while (tmp > 0) {
            s.insert(s.begin()+1,  (tmp % 10) + '0');
            tmp /= 10;
        }
    } else if (num == 0) {
        return String("0");
    } else {
        while (num > 0) {
            s.insert(s.begin(),  (num % 10) + '0');
            num /= 10;
        }
    }
    return s;
}

String String::lltos(long long num) {

    String s;
    if (num < 0) {
        s += '-';
        uint64_t tmp;
        *(reinterpret_cast<long long*>(&tmp)) = num;
        tmp = (~tmp) + 1;
        while (tmp > 0) {
            s.insert(s.begin()+1,  (tmp % 10) + '0');
            tmp /= 10;
        }
    } else if (num == 0) {
        return String("0");
    } else {
        while (num > 0) {
            s.insert(s.begin(),  (num % 10) + '0');
            num /= 10;
        }
    }
    return s;
}

String String::itos(size_t num) {
    if (num == 0)
        return "0";
    String s;
    while (num > 0) {
        s.insert(s.begin(),  (num % 10) + '0');
        num /= 10;
    }
    return s;
}

int String::stoi(const String& numstr) {
    if (numstr.size() == 0)
        return 0;
    bool neg = false, symbol = false;
    if (numstr[0] == '-')
        neg = true, symbol=true;
    if (numstr[0] == '+')
        neg = false, symbol=true;

    int total = 0;
    uint num_len = uint(numstr.size());
    for (uint i = (symbol ? 1 : 0); i < numstr.size(); ++i)
        total += int((numstr[i] - '0') * std::pow(10, num_len-i-1));
    if (not neg and total <= INT_MAX)
        return total;
    else if(neg)
        return -total;
    return 0;
}

long long String::stoll(const String& numstr) {
    if (numstr.size() == 0)
        return 0;
    bool neg = false, symbol = false;
    if (numstr[0] == '-')
        neg = true, symbol=true;
    if (numstr[0] == '+')
        neg = false, symbol=true;

    long long total = 0;
    uint num_len = uint(numstr.size());
    for (uint i = (symbol ? 1 : 0); i < numstr.size(); ++i)
        total += int64_t((numstr[i] - '0') * std::pow(10, num_len-i-1));
    if (not neg and total <= LONG_LONG_MAX)
        return total;
    if (neg)
        return -total;
    return 0;
}

String operator+(const char* str1, const String& str2) {
    String str(str1);
    str += str2;
    return str;
}

namespace std {
size_t hash<String>::operator() (const String& str) const {
    return std::hash<std::basic_string<char>>()(str);
}
}
