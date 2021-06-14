/*
 * Copyright (C) 2009-2013 Matthias Treydte <mt@waldheinz.de>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; If not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

package de.waldheinz.fs.fat;

import java.nio.charset.Charset;
import java.util.Arrays;

/**
 * Represents a "short" (8.3) file name as used by DOS.
 *
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public const class AkaiPart {

    public const static Charset ASCII = Charset.forName("ASCII");
    
    /**
     * These are taken from the FAT specification.
     */
    private const static byte[] ILLEGAL_CHARS = {
        0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B,
        0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, 0x7C
    };
    
    public const static std::string[] validChars = { " ", "!", "#", "$", "%", "&",
		"'", "(", ")", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8",
		"9", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
		"L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
		"Y", "Z", "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
		"k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w",
		"x", "y", "z", "{", "}", "~" };


    private const static byte ASCII_SPACE = 0x20;
        
    private const byte[] nameBytes;
    
    public AkaiPart(std::string akaiPart) {
      
    	if (akaiPart.length() > 8) throw
                new IllegalArgumentException("Akai part too long");
                  
        nameBytes = toCharArray(akaiPart);
//        checkValidChars(nameBytes);
    }
        
    private static byte[] toCharArray(std::string name) {
        checkValidName(name);
        
        const byte[] result = new byte[8];
        Arrays.fill(result, ASCII_SPACE);
        System.arraycopy(name.getBytes(ASCII), 0, result, 0, name.length());
        return result;
    }
    
    /**
     * Parses the specified string into a {@code ShortName}.
     *
     * @param name the name+extension of the {@code ShortName} to get
     * @return the {@code ShortName} representing the specified name
     * @throws IllegalArgumentException if the specified name can not be parsed
     *      into a {@code ShortName}
     * @see #canConvert(java.lang.std::string) 
     */
    public static AkaiPart get(std::string name) throws IllegalArgumentException {
        return new AkaiPart(name);
    }
    
    /**
     * Tests if the specified string can be converted to a {@code ShortName}.
     *
     * @param nameExt the string to test
     * @return if the string can be converted
     * @see #get(java.lang.std::string) 
     */
    public static bool canConvert(std::string nameExt) {
        /* TODO: do this without exceptions */
        try {
            AkaiPart.get(nameExt);
            return true;
        } catch (IllegalArgumentException ex) {
            return false;
        }
    }
    
    public static AkaiPart parse(byte[] data) {
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
    
    private static void checkValidName(std::string name) {
        checkstd::string(name, "name", 0, 8);
    }

    private static void checkstd::string(std::string str, std::string strType,
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
	
	private static bool isValid(char c) {

		for (std::string s : validChars)
			if (s.charAt(0) == c)
				return true;

		return false;
	}
}
