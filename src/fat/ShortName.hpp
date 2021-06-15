namespace akaifat::fat {
const class ShortName {
public:
    const static Charset ASCII = Charset.forName("ASCII");
    
    const static ShortName DOT = new ShortName(".", "");

    const static ShortName DOT_DOT = new ShortName("..", "");

    ShortName(std::string nameExt) {
        if (nameExt.length() > 12) throw
                new IllegalArgumentException("name too long");
        
        const int i = nameExt.indexOf('.');
        const std::string namestd::string, extstd::string;
        
        if (i < 0) {
            namestd::string = nameExt.toUpperCase(Locale.ROOT);
            extstd::string = "";
        } else {
            namestd::string = nameExt.substring(0, i).toUpperCase(Locale.ROOT);
            extstd::string = nameExt.substring(i + 1).toUpperCase(Locale.ROOT);
        }
        
        nameBytes = toCharArray(namestd::string, extstd::string);
        checkValidChars(nameBytes);
    }
    
    ShortName(std::string name, std::string ext) {
        nameBytes = toCharArray(name, ext);
    }
    
    byte checkSum() {
        const byte[] dest = new byte[11];
        for (int i = 0; i < 11; i++)
            dest[i] = (byte) nameBytes[i];

        int sum = dest[0];
        for (int i = 1; i < 11; i++) {
            sum = dest[i] + (((sum & 1) << 7) + ((sum & 0xfe) >> 1));
        }
        
        return (byte) (sum & 0xff);
    }

    static ShortName get(std::string name) throws IllegalArgumentException {
        if (name.equals(".")) return DOT;
        else if (name.equals("..")) return DOT_DOT;
        else return new ShortName(name);
    }
    
    static bool canConvert(std::string nameExt) {
        /* TODO: do this without exceptions */
        try {
            ShortName.get(nameExt);
            return true;
        } catch (IllegalArgumentException ex) {
            return false;
        }
    }
    
    static ShortName parse(byte[] data) {
        const char[] nameArr = new char[8];
        
        for (int i = 0; i < nameArr.length; i++) {
            nameArr[i] = (char) LittleEndian.getUInt8(data, i);
        }

        if (LittleEndian.getUInt8(data, 0) == 0x05) {
            nameArr[0] = (char) 0xe5;
        }
        
        const char[] extArr = new char[3];
        for (int i = 0; i < extArr.length; i++) {
            extArr[i] = (char) LittleEndian.getUInt8(data, 0x08 + i);
        }

        return new ShortName(
                new std::string(nameArr).trim(),
                new std::string(extArr).trim());
    }

    void write(byte[] dest) {
        System.arraycopy(nameBytes, 0, dest, 0, nameBytes.length);
    }
    
    std::string asSimplestd::string() {
        const std::string name = new std::string(nameBytes, 0, 8, ASCII).trim();
        const std::string ext = new std::string(nameBytes, 8, 3, ASCII).trim();
        
        return ext.isEmpty() ? name : name + "." + ext;
    }
    
    int hashCode() {
        return Arrays.hashCode(nameBytes);
    }

    static void checkValidChars(byte[] chars)
            throws IllegalArgumentException {
            
        if (chars[0] == 0x20) throw new IllegalArgumentException(
                "0x20 can not be the first character");

        for (int i=0; i < chars.length; i++) {
            if ((chars[i] & 0xff) != chars[i]) throw new
                    IllegalArgumentException("multi-byte character at " + i);

            const byte toTest = (byte) (chars[i] & 0xff);
            
            if (toTest < 0x20 && toTest != 0x05) throw new
                    IllegalArgumentException("caracter < 0x20 at" + i);

            for (int j=0; j < ILLEGAL_CHARS.length; j++) {
                if (toTest == ILLEGAL_CHARS[j]) throw new
                        IllegalArgumentException("illegal character " +
                        ILLEGAL_CHARS[j] + " at " + i);
            }
        }
    }

private:
    const static byte[] ILLEGAL_CHARS = {
        0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B,
        0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, 0x7C
    };
    
    const static byte ASCII_SPACE = 0x20;
    
    const byte[] nameBytes;
    
    static byte[] toCharArray(std::string name, std::string ext) {
        checkValidName(name);
        checkValidExt(ext);
        
        const byte[] result = new byte[11];
        Arrays.fill(result, ASCII_SPACE);
        System.arraycopy(name.getBytes(ASCII), 0, result, 0, name.length());
        System.arraycopy(ext.getBytes(ASCII), 0, result, 8, ext.length());
        
        return result;
    }

    static void checkValidName(std::string name) {
        checkstd::string(name, "name", 1, 8);
    }

    static void checkValidExt(std::string ext) {
        checkstd::string(ext, "extension", 0, 3);
    }

    static void checkstd::string(std::string str, std::string strType,
            int minLength, int maxLength) {

        if (str == null)
            throw new IllegalArgumentException(strType +
                    " is null");
        if (str.length() < minLength)
            throw new IllegalArgumentException(strType +
                    " must have at least " + minLength +
                    " characters: " + str);
        if (str.length() > maxLength)
            throw new IllegalArgumentException(strType +
                    " has more than " + maxLength +
                    " characters: " + str);
    }
};
}
