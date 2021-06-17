#pragma once

#include <string>
#include <vector>

namespace akaifat::fat {
class AkaiPart {
public:
//    static Charset ASCII = Charset.forName("ASCII");
    
    static std::vector<std::string>& validChars() { static std::vector<std::string> result { " ", "!", "#", "$", "%", "&",
		"'", "(", ")", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8",
		"9", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
		"L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
		"Y", "Z", "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
		"k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w",
		"x", "y", "z", "{", "}", "~" };
        return result;
    }

    AkaiPart(std::string akaiPart) {
      
    	if (akaiPart.length() > 8) throw "Akai part too long";
                  
//        nameBytes = toCharArray(akaiPart);
    }

    static AkaiPart get(std::string name) {
        return AkaiPart(name);
    }
    
    static bool canConvert(std::string nameExt) {
        try {
            AkaiPart::get(nameExt);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    static AkaiPart parse(std::vector<char>& data) {
//        char[] nameArr = new char[8];
//
//        for (int i = 0; i < nameArr.length; i++) {
//            nameArr[i] = (char) LittleEndian.getUInt8(data, i+12);
//        }
//        std::string akaiPart = new std::string(nameArr);
//        if (!isValidAkaiPart(akaiPart)) akaiPart = "        ";
//       return new AkaiPart(akaiPart);
        return AkaiPart("");
    }

    void write(std::vector<char>& dest) {
//        System.arraycopy(nameBytes, 0, dest, 12, nameBytes.length);
    }
    
    std::string asSimpleString() {
//        std::string name = new std::string(nameBytes, 0, 8, ASCII);
//        return name;
        return "";
    }
    
    bool equals(AkaiPart& akaiPart) {
//        if (!(obj instanceof AkaiPart)) {
//            return false;
//        }
//
//        AkaiPart other = (AkaiPart) obj;
//        return Arrays.equals(nameBytes, other.nameBytes);
        return false;
    }

    int hashCode() {
//        return Arrays.hashCode(nameBytes);
        return 0;
    }

    static void checkValidChars(const std::vector<char>& chars) {
            
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
    
	static std::vector<char> replaceInvalidChars(std::string& str) {

        std::vector<char> ca(str.length());

		int counter = 0;
        
		for (char c : str) {
			if (!isValid(c)) {
				ca[counter] = ' ';
			} else {
				ca[counter] = c;
			}
			counter++;
		}

		return ca;
	}
	
	static bool isValidAkaiPart(std::string str) {

		for (char c : str) {
			if (!isValid(c))
				return false;
		}

		return true;
	}

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

    static bool isValid(char c) {

        for (auto s : validChars())
            if (s[0] == c)
                return true;

        return false;
    }

    static std::vector<char> toCharArray(std::string name) {
        checkValidName(name);
        
        std::vector<char> result(8);
//        Arrays.fill(result, ASCII_SPACE);
//        System.arraycopy(name.getBytes(ASCII), 0, result, 0, name.length());
        return result;
    }

    static void checkValidName(std::string& name) {
        checkString(name, "name", 0, 8);
    }

    static void checkString(std::string& str, const std::string strType,
            int minLength, int maxLength) {
        if (str.length() < minLength)
            throw strType + " must have at least " + std::to_string(minLength) +
                    " characters: " + str;
        if (str.length() > maxLength)
            throw strType +
                    " has more than " + std::to_string(maxLength) +
                    " characters: " + str;
    }
};
}
