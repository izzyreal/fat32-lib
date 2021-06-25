#pragma once

#include <string>

namespace akaifat {

    class StrUtil {
    public:
        // trim from left
        static inline std::string &ltrim(std::string &s, const char *t = " \t\n\r\f\v") {
            s.erase(0, s.find_first_not_of(t));
            return s;
        }

        // trim from right
        static inline std::string &rtrim(std::string &s, const char *t = " \t\n\r\f\v") {
            s.erase(s.find_last_not_of(t) + 1);
            return s;
        }

//    // trim from left & right
//    static inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
//    {
//        return ltrim(rtrim(s, t), t);
//    }

        // copying versions
        static inline std::string ltrim_copy(std::string s, const char *t = " \t\n\r\f\v") {
            return ltrim(s, t);
        }

        static inline std::string rtrim_copy(std::string s, const char *t = " \t\n\r\f\v") {
            return rtrim(s, t);
        }

        static inline std::string trim_copy(std::string s, const char *t = " \t\n\r\f\v") {
            return trim(s, t);
        }

        static std::string trim(const std::string &str,
                                const std::string &whitespace = " \t\0") {
            const auto strBegin = str.find_first_not_of(whitespace);
            if (strBegin == std::string::npos)
                return ""; // no content

            const auto strEnd = str.find_last_not_of(whitespace);
            const auto strRange = strEnd - strBegin + 1;

            return str.substr(strBegin, strRange);
        }

        static inline void to_upper(std::string &s) {
            for (int i = 0; i < s.length(); i++)
                s[i] = toupper(s[i]);

        }

        static inline std::string to_upper_copy(std::string s) {
            for (int i = 0; i < s.length(); i++)
                s[i] = toupper(s[i]);
            return s;
        }

        static inline std::string to_lower_copy(std::string s) {
            for (int i = 0; i < s.length(); i++)
                s[i] = tolower(s[i]);
            return s;
        }
    };

}
