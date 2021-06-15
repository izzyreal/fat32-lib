#include "../AbstractFsObject.hpp"

#include "../LittleEndian.hpp"

#include <vector>

namespace akaifat::fat {
class FatDirectoryEntry : public AbstractFsObject {

private:
    const FatType& type;
    bool dirty;
    static const int OFFSET_ATTRIBUTES = 0x0b;
    
    static const int OFFSET_FILE_SIZE = 0x1c;
    
    static const int F_READONLY = 0x01;
    static const int F_HIDDEN = 0x02;
    static const int F_SYSTEM = 0x04;
    static const int F_VOLUME_ID = 0x08;
    static const int F_DIRECTORY = 0x10;
    static const int F_ARCHIVE = 0x20;

public:
    FatDirectoryEntry(FatType& fs)
    : FatDirectoryEntry(fs, std::vector<char>(SIZE), false)
    {
    }

    FatDirectoryEntry(FatType& fs, std::vector<char> _data, bool readOnly)
    : AbstractFsObject(readOnly), type (fs), data (_data)
    {
    }
        
    void setFlag(int mask, bool set) {
        const int oldFlags = getFlags();

        if (((oldFlags & mask) != 0) == set) return;
        
        if (set) {
            setFlags(oldFlags | mask);
        } else {
            setFlags(oldFlags & ~mask);
        }

        dirty = true;
    }
    
    int getFlags() {
        return LittleEndian::getUInt8(data, OFFSET_ATTRIBUTES);
    }
    
    void setFlags(int flags) {
        LittleEndian::setInt8(data, OFFSET_ATTRIBUTES, flags);
    }

    const std::vector<char> data;
    
    const static int SIZE = 32;

    static const int ENTRY_DELETED_MAGIC = 0xe5;
    
    static FatDirectoryEntry read(
            FatType type, ByteBuffer buff, bool readOnly) {
        
        assert (buff.remaining() >= SIZE);

        if (buff.get(buff.position()) == 0) return null;

        const std::vector<char> data(SIZE);
        buff.get(data);
        return new FatDirectoryEntry(type, data, readOnly);
    }

    static void writeNullEntry(ByteBuffer buff) {
        for (int i=0; i < SIZE; i++) {
            buff.put((char) 0);
        }
    }
    
    bool isVolumeLabel() {
        if (isLfnEntry()) return false;
        else return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == F_VOLUME_ID);
    }

    bool isSystemFlag() {
        return ((getFlags() & F_SYSTEM) != 0);
    }

    void setSystemFlag(bool isSystem) {
        setFlag(F_SYSTEM, isSystem);
    }

    bool isArchiveFlag() {
        return ((getFlags() & F_ARCHIVE) != 0);
    }

    void setArchiveFlag(bool isArchive) {
        setFlag(F_ARCHIVE, isArchive);
    }
    
    bool isHiddenFlag() {
        return ((getFlags() & F_HIDDEN) != 0);
    }

    void setHiddenFlag(bool isHidden) {
        setFlag(F_HIDDEN, isHidden);
    }
    
    bool isVolumeIdFlag() {
        return ((getFlags() & F_VOLUME_ID) != 0);
    }
    
    bool isLfnEntry() {
        return isReadonlyFlag() && isSystemFlag() &&
                isHiddenFlag() && isVolumeIdFlag();
    }
    
    bool isDirty() {
        return dirty;
    }

    bool isDirectory() {
        return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == F_DIRECTORY);
    }
    
    static FatDirectoryEntry create(FatType type, bool directory) {
        const FatDirectoryEntry result = new FatDirectoryEntry(type);

        if (directory) {
            result.setFlags(F_DIRECTORY);
        }        
        return result;
    }
    
    static FatDirectoryEntry createVolumeLabel(
            FatType type, std::string volumeLabel) {
        
        assert(volumeLabel != null);
        
        const std::vector<char> data = new byte[SIZE];
        
        System.arraycopy(
                    volumeLabel.getBytes(ShortName.ASCII), 0,
                    data, 0,
                    volumeLabel.length());
        
        const FatDirectoryEntry result =
                new FatDirectoryEntry(type, data, false);
        result.setFlags(FatDirectoryEntry.F_VOLUME_ID);
        return result;
    }
    
    std::string getVolumeLabel() {
        if (!isVolumeLabel())
            throw "not a volume label";
            
        const std::stringBuilder sb = new std::stringBuilder();
        
        for (int i=0; i < AbstractDirectory.MAX_LABEL_LENGTH; i++) {
            const byte b = data[i];
            
            if (b != 0) {
                sb.append((char) b);
            } else {
                break;
            }
        }
        
        return sb.tostd::string();
    }

    bool isDeleted() {
        return  (LittleEndian.getUInt8(data, 0) == ENTRY_DELETED_MAGIC);
    }
    
    long getLength() {
        return LittleEndian.getUInt32(data, OFFSET_FILE_SIZE);
    }

    void setLength(long length) throws IllegalArgumentException {
        LittleEndian.setInt32(data, OFFSET_FILE_SIZE, length);
    }
    
    ShortName getShortName() {
        if (data[0] == 0) {
            return null;
        } else {
            return ShortName.parse(data);
        }
    }

    bool isFile() {
        return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == 0);
    }
    
    void setShortName(ShortName sn) {
        if (sn.equals(getShortName())) return;
        
        sn.write(data);
        dirty = true;
    }

    void setAkaiName(std::string s) {
    	System.out.println("string s " + s);
    	std::string part1 = AkaiFatLfnDirectory.splitName(s)[0];
    	std::string part2 = "        ";
    	std::string ext = AkaiFatLfnDirectory.splitName(s)[1];
    	if (part1.length() > 8) {
    		part2 = part1.substring(8);
    		part1 = part1.substring(0, 8);
    	}
    	if (ext.length() > 0) ext = "."+ ext;
    	const ShortName sn = new ShortName(part1 + ext);
    	sn.write(data);
    	System.out.println( "part 1 " + part1);
    	System.out.println( "part 2 " + part2);
    	const AkaiPart ap = new AkaiPart(part2);
    	ap.write(data);
    }
    
    long getStartCluster() {
        if (type == FatType.FAT32) {
            return
                    (LittleEndian.getUInt16(data, 0x14) << 16) |
                     LittleEndian.getUInt16(data, 0x1a);
        } else {
            return LittleEndian.getUInt16(data, 0x1a);
        }
    }
    
    void setStartCluster(long startCluster) {
        if (startCluster > Integer.MAX_VALUE) throw new AssertionError();

        if (type == FatType.FAT32) {
            LittleEndian.setInt16(data, 0x1a, (int) (startCluster & 0xffff));
            LittleEndian.setInt16(data, 0x14, (int) ((startCluster >> 16) & 0xffff));
        } else {
            LittleEndian.setInt16(data, 0x1a, (int) startCluster);
        }
    }
    
    void write(ByteBuffer buff) {
        buff.put(data);
        dirty = false;
    }

    bool isReadonlyFlag() {
        return ((getFlags() & F_READONLY) != 0);
    }
    
    void setReadonlyFlag(bool isReadonly) {
        setFlag(F_READONLY, isReadonly);
    }
    
    std::string getLfnPart() {
        const char[] unicodechar = new char[13];

        unicodechar[0] = (char) LittleEndian.getUInt16(data, 1);
        unicodechar[1] = (char) LittleEndian.getUInt16(data, 3);
        unicodechar[2] = (char) LittleEndian.getUInt16(data, 5);
        unicodechar[3] = (char) LittleEndian.getUInt16(data, 7);
        unicodechar[4] = (char) LittleEndian.getUInt16(data, 9);
        unicodechar[5] = (char) LittleEndian.getUInt16(data, 14);
        unicodechar[6] = (char) LittleEndian.getUInt16(data, 16);
        unicodechar[7] = (char) LittleEndian.getUInt16(data, 18);
        unicodechar[8] = (char) LittleEndian.getUInt16(data, 20);
        unicodechar[9] = (char) LittleEndian.getUInt16(data, 22);
        unicodechar[10] = (char) LittleEndian.getUInt16(data, 24);
        unicodechar[11] = (char) LittleEndian.getUInt16(data, 28);
        unicodechar[12] = (char) LittleEndian.getUInt16(data, 30);

        int end = 0;

        while ((end < 13) && (unicodechar[end] != '\0')) {
            end++;
        }
        
        return new std::string(unicodechar).substring(0, end);
    }

};
}
