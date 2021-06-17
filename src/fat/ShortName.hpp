#pragma once

#include <vector>

namespace akaifat::fat {
class ShortName {
private:
    static std::vector<char>& ILLEGAL_CHARS() {
        static std::vector<char> result{
            0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B,
            0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, 0x7C
        };
        return result;
    }
    
    static const char ASCII_SPACE = 0x20;
    
    std::vector<char> nameBytes;
    
    static std::vector<char> toCharArray(std::string& name, std::string& ext) {
        checkValidName(name);
        checkValidExt(ext);
        
        std::vector<char> result (11);
        //Arrays.fill(result, ASCII_SPACE);
        //System.arraycopy(name.getBytes(ASCII), 0, result, 0, name.length());
        //System.arraycopy(ext.getBytes(ASCII), 0, result, 8, ext.length());
        
        return result;
    }

    static void checkValidName(std::string name) {
        checkString(name, "name", 1, 8);
    }

    static void checkValidExt(std::string ext) {
        checkString(ext, "extension", 0, 3);
    }

    static void checkString(std::string str, std::string strType,
            int minLength, int maxLength) {

        if (str.length() < minLength)
            throw strType +
                    " must have at least " + std::to_string(minLength) +
                    " characters: " + str;
        if (str.length() > maxLength)
            throw strType +
                    " has more than " + std::to_string(maxLength) +
                    " characters: " + str;
    }

public:
//    static Charset ASCII = Charset.forName("ASCII");
    
//    static ShortName DOT = ShortName(".", "");
//
//    static ShortName DOT_DOT = ShortName("..", "");

    ShortName(){}
    
    ShortName(std::string nameExt) {
        if (nameExt.length() > 12) throw "name too long";
        
        auto i = nameExt.find_last_of('.');
        
        std::string name;
        std::string ext;
        
        if (i < 0) {
//            name = nameExt.toUpperCase(Locale.ROOT);
            ext = "";
        } else {
//            name = nameExt.substring(0, i).toUpperCase(Locale.ROOT);
//            ext = nameExt.substring(i + 1).toUpperCase(Locale.ROOT);
        }
        
//        nameBytes = toCharArray(namestd::string, extstd::string);
//        checkValidChars(nameBytes);
    }
    
    ShortName(std::string name, std::string ext) {
//        nameBytes = toCharArray(name, ext);
    }
    
    char checkSum() {
//        byte[] dest = new byte[11];
//        for (int i = 0; i < 11; i++)
//            dest[i] = (char) nameBytes[i];
//
//        int sum = dest[0];
//        for (int i = 1; i < 11; i++) {
//            sum = dest[i] + (((sum & 1) << 7) + ((sum & 0xfe) >> 1));
//        }
//
//        return (char) (sum & 0xff);
        return 0;
    }

    static ShortName get(std::string name) {
//        if (name.equals(".")) return DOT;
//        else if (name.equals("..")) return DOT_DOT;
//        else return new ShortName(name);
        return ShortName();
    }
    
    static bool canConvert(std::string nameExt) {
        try {
            ShortName::get(nameExt);
            return true;
        } catch (std::exception&) {
            return false;
        }
    }
    
    static ShortName parse(std::vector<char>& data) {
//        char[] nameArr = new char[8];
//
//        for (int i = 0; i < nameArr.length; i++) {
//            nameArr[i] = (char) LittleEndian.getUInt8(data, i);
//        }
//
//        if (LittleEndian.getUInt8(data, 0) == 0x05) {
//            nameArr[0] = (char) 0xe5;
//        }
//
//        char[] extArr = new char[3];
//        for (int i = 0; i < extArr.length; i++) {
//            extArr[i] = (char) LittleEndian.getUInt8(data, 0x08 + i);
//        }
//
//        return new ShortName(
//                new std::string(nameArr).trim(),
//                new std::string(extArr).trim());
        return ShortName();
    }

    void write(std::vector<char>& dest) {
//        System.arraycopy(nameBytes, 0, dest, 0, nameBytes.length);
    }
    
    std::string asSimpleString() {
//        std::string name = new std::string(nameBytes, 0, 8, ASCII).trim();
//        std::string ext = new std::string(nameBytes, 8, 3, ASCII).trim();
//
//        return ext.isEmpty() ? name : name + "." + ext;
        return "";
    }
    
    int hashCode() {
//        return Arrays.hashCode(nameBytes);
        return 0;
    }

    static void checkValidChars(std::vector<char>& chars) {
            
        if (chars[0] == 0x20) throw "0x20 can not be the first character";

        for (int i=0; i < chars.size(); i++) {
            if ((chars[i] & 0xff) != chars[i]) throw "multi-byte character at " + std::to_string(i);

            auto toTest = (char) (chars[i] & 0xff);
            
            if (toTest < 0x20 && toTest != 0x05) throw "character < 0x20 at" + std::to_string(i);

            for (int j=0; j < ILLEGAL_CHARS().size(); j++) {
                if (toTest == ILLEGAL_CHARS()[j]) throw "illegal character " +
                        std::to_string(ILLEGAL_CHARS()[j]) + " at " + std::to_string(i);
            }
        }
    }
};
}
