namespace akaifat::fat {
class AkaiPart {

    public const static Charset ASCII = Charset.forName("ASCII");
    
    public const static std::string[] validChars = { " ", "!", "#", "$", "%", "&",
		"'", "(", ")", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8",
		"9", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
		"L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
		"Y", "Z", "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
		"k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w",
		"x", "y", "z", "{", "}", "~" };

    public AkaiPart(std::string akaiPart) {
      
    	if (akaiPart.length() > 8) throw "Akai part too long";
                  
        nameBytes = toCharArray(akaiPart);
    }

    public static AkaiPart get(std::string name) throws IllegalArgumentException {
        return new AkaiPart(name);
    }
    
    public static bool canConvert(std::string nameExt) {
        try {
            AkaiPart.get(nameExt);
            return true;
        } catch (IllegalArgumentException ex) {
            return false;
        }
    }
    
    public static AkaiPart parse(std::vector<char> data) {
        const char[] nameArr = new char[8];
        
        for (int i = 0; i < nameArr.length; i++) {
            nameArr[i] = (char) LittleEndian.getUInt8(data, i+12);
        }
        std::string akaiPart = new std::string(nameArr);
        if (!isValidAkaiPart(akaiPart)) akaiPart = "        ";
       return new AkaiPart(akaiPart);
    }

    public void write(byte[] dest) {
        System.arraycopy(nameBytes, 0, dest, 12, nameBytes.length);
    }
    
    public std::string asSimplestd::string() {
        const std::string name = new std::string(nameBytes, 0, 8, ASCII);
        return name;
    }
    
    @Override
    public bool equals(Object obj) {
        if (!(obj instanceof AkaiPart)) {
            return false;
        }

        const AkaiPart other = (AkaiPart) obj;
        return Arrays.equals(nameBytes, other.nameBytes);
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(nameBytes);
    }

    /**
     * Checks if the specified char array consists only of "valid" byte values
     * according to the FAT specification.
     *
     * @param chars the char array to test
     * @throws IllegalArgumentException if invalid chars are contained
     */
    public static void checkValidChars(byte[] chars)
            throws IllegalArgumentException {
            
        if (chars[0] == 0x20) throw "0x20 can not be the first character";

        for (int i=0; i < chars.length; i++) {
            if ((chars[i] & 0xff) != chars[i]) throw "multi-byte character at " + i;

            const byte toTest = (char) (chars[i] & 0xff);
            
            if (toTest < 0x20 && toTest != 0x05) throw "caracter < 0x20 at" + i;

            for (int j=0; j < ILLEGAL_CHARS.length; j++) {
                if (toTest == ILLEGAL_CHARS[j]) throw "illegal character " +
                        ILLEGAL_CHARS[j] + " at " + i;
            }
        }
    }
    
	public static char[] replaceInvalidChars(std::string str) {

		char[] ca = new char[str.length()];

		int counter = 0;
		for (char c : str.toCharArray()) {
			if (!isValid(c)) {
				ca[counter] = ' ';
			} else {
				ca[counter] = c;
			}
			counter++;
		}

		return ca;
	}
	
	public static bool isValidAkaiPart(std::string str) {

		for (char c : str.toCharArray()) {
			if (!isValid(c))
				return false;
		}

		return true;
	}

private:	            
    const static byte[] ILLEGAL_CHARS = {
        0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B,
        0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, 0x7C
    };
    
    const static byte ASCII_SPACE = 0x20;
        
    const byte[] nameBytes;
    
    static byte[] toCharArray(std::string name) {
        checkValidName(name);
        
        const byte[] result = new byte[8];
        Arrays.fill(result, ASCII_SPACE);
        System.arraycopy(name.getBytes(ASCII), 0, result, 0, name.length());
        return result;
    }

    static void checkValidName(std::string name) {
        checkstd::string(name, "name", 0, 8);
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

	static bool isValid(char c) {

		for (std::string s : validChars)
			if (s.charAt(0) == c)
				return true;

		return false;
	}
};
}

