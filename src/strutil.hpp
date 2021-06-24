#pragma once

#include <string>

namespace akaifat {

class StrUtil {
public:
    // trim from left
    static inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from right
    static inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from left & right
    static inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
    {
        return ltrim(rtrim(s, t), t);
    }

    // copying versions
    static inline std::string ltrim_copy(std::string s, const char* t = " \t\n\r\f\v")
    {
        return ltrim(s, t);
    }

    static inline std::string rtrim_copy(std::string s, const char* t = " \t\n\r\f\v")
    {
        return rtrim(s, t);
    }

    static inline std::string trim_copy(std::string s, const char* t = " \t\n\r\f\v")
    {
        return trim(s, t);
    }
    
    static inline void to_upper(std::string& s)
    {
        for (int i = 0; i < s.length(); i++)
            s[i] = toupper(s[i]);
        
    }

    static inline std::string to_upper_copy(std::string s)
    {
        for (int i = 0; i < s.length(); i++)
            s[i] = toupper(s[i]);
        return s;
    }

};

}