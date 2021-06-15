namespace akaifat::fat {
class BootSector extends Sector {
public:
    static const int FAT_COUNT_OFFSET = 16;
    static const int RESERVED_SECTORS_OFFSET = 14;    
    static const int TOTAL_SECTORS_16_OFFSET = 19;
    static const int TOTAL_SECTORS_32_OFFSET = 32;
    static const int FILE_SYSTEM_TYPE_LENGTH = 8;
    static const int SECTORS_PER_CLUSTER_OFFSET = 0x0d;
    static const int EXTENDED_BOOT_SIGNATURE = 0x29;
    const static int SIZE = 512;
    
        static BootSector read(BlockDevice device) throw (std::exception) {
        const ByteBuffer bb = ByteBuffer.allocate(512);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        device.read(0, bb);
        
        if ((bb.get(510) & 0xff) != 0x55 ||
                (bb.get(511) & 0xff) != 0xaa) 
                throw "missing boot sector signature";
                
        const byte sectorsPerCluster = bb.get(SECTORS_PER_CLUSTER_OFFSET);

        if (sectorsPerCluster <= 0) 
                throw "suspicious sectors per cluster count " + sectorsPerCluster;
                
        const int rootDirEntries = bb.getShort(
                Fat16BootSector.ROOT_DIR_ENTRIES_OFFSET);
        const int rootDirSectors = ((rootDirEntries * 32) +
                (device.getSectorSize() - 1)) / device.getSectorSize();

        const int total16 =
                bb.getShort(TOTAL_SECTORS_16_OFFSET) & 0xffff;
        const long total32 =
                bb.getInt(TOTAL_SECTORS_32_OFFSET) & 0xffffffffl;
        
        const long totalSectors = total16 == 0 ? total32 : total16;
        
        const int fatSz16 =
                bb.getShort(Fat16BootSector.SECTORS_PER_FAT_OFFSET)  & 0xffff;
        const long fatSz32 =
                bb.getInt(Fat32BootSector.SECTORS_PER_FAT_OFFSET) & 0xffffffffl;
                
        const long fatSz = fatSz16 == 0 ? fatSz32 : fatSz16;
        const int reservedSectors = bb.getShort(RESERVED_SECTORS_OFFSET);
        const int fatCount = bb.get(FAT_COUNT_OFFSET);
        const long dataSectors = totalSectors - (reservedSectors +
                (fatCount * fatSz) + rootDirSectors);
                
        const long clusterCount = dataSectors / sectorsPerCluster;
        
        const BootSector result =
                (clusterCount > Fat16BootSector.MAX_FAT16_CLUSTERS) ?
            new Fat32BootSector(device) : new Fat16BootSector(device);
            
        result.read();
        return result;
    }
    
    virtual FatType getFatType() = 0;
    
    virtual long getSectorsPerFat() = 0;
    
    virtual void setSectorsPerFat(long v) = 0;

    virtual void setSectorCount(long count) = 0;

    virtual int getRootDirEntryCount() = 0;
    
    virtual long getSectorCount() = 0;
    
    const long getFatOffset(int fatNr) {
        long sectSize = getBytesPerSector();
        long sectsPerFat = getSectorsPerFat();
        long resSects = getNrReservedSectors();

        long offset = resSects * sectSize;
        long fatSize = sectsPerFat * sectSize;

        offset += fatNr * fatSize;

        return offset;
    }

    const long getRootDirOffset() {
        long sectSize = getBytesPerSector();
        long sectsPerFat = getSectorsPerFat();
        int fats = getNrFats();

        long offset = getFatOffset(0);
        
        offset += fats * sectsPerFat * sectSize;

        return offset;
    }

    const long getFilesOffset() {
        long offset = getRootDirOffset();
        
        offset += getRootDirEntryCount() * 32l;
        
        return offset;
    }
    
    virtual int getFileSystemTypeLabelOffset() = 0;
    
    virtual int getExtendedBootSignatureOffset() = 0;
    
    void init() throw (std::exception) {
        setBytesPerSector(getDevice().getSectorSize());
        setSectorCount(getDevice().getSize() / getDevice().getSectorSize());
        set8(getExtendedBootSignatureOffset(), EXTENDED_BOOT_SIGNATURE);

        set8(0x00, 0xeb);
        set8(0x01, 0x3c);
        set8(0x02, 0x90);
        set8(0x1fe, 0x55);
        set8(0x1ff, 0xaa);
    }
    
    std::string getFileSystemTypeLabel() {
        const std::stringBuilder sb = new std::stringBuilder(FILE_SYSTEM_TYPE_LENGTH);

        for (int i=0; i < FILE_SYSTEM_TYPE_LENGTH; i++) {
            sb.append ((char) get8(getFileSystemTypeLabelOffset() + i));
        }

        return sb.tostd::string();
    }

    void setFileSystemTypeLabel(std::string fsType)
            throws IllegalArgumentException {

        if (fsType.length() != FILE_SYSTEM_TYPE_LENGTH) {
            throw new IllegalArgumentException();
        }

        for (int i=0; i < FILE_SYSTEM_TYPE_LENGTH; i++) {
            set8(getFileSystemTypeLabelOffset() + i, fsType.charAt(i));
        }
    }

    const long getDataClusterCount() {
        return getDataSize() / getBytesPerCluster();
    }

    std::string getOemName() {
        std::stringBuilder b = new std::stringBuilder(8);
        
        for (int i = 0; i < 8; i++) {
            int v = get8(0x3 + i);
            if (v == 0) break;
            b.append((char) v);
        }
        
        return b.tostd::string();
    }

    void setOemName(std::string name) {
        if (name.length() > 8) throw "only 8 characters are allowed";

        for (int i = 0; i < 8; i++) {
            char ch;
            if (i < name.length()) {
                ch = name.charAt(i);
            } else {
                ch = (char) 0;
            }

            set8(0x3 + i, ch);
        }
    }
    
    int getBytesPerSector() {
        return get16(0x0b);
    }

    void setBytesPerSector(int v) {
        if (v == getBytesPerSector()) return;

        switch (v) {
            case 512: case 1024: case 2048: case 4096:
                set16(0x0b, v);
                break;
                
            default:
                throw new IllegalArgumentException();
        }
    }

    int getBytesPerCluster() {
        return getSectorsPerCluster() * getBytesPerSector();
    }

   int getSectorsPerCluster() {
        return get8(SECTORS_PER_CLUSTER_OFFSET);
    }

    void setSectorsPerCluster(int v) {
        if (v == getSectorsPerCluster()) return;
        if (!isPowerOfTwo(v)) throw "value must be a power of two";
        
        set8(SECTORS_PER_CLUSTER_OFFSET, v);
    }
    
    int getNrReservedSectors() {
        return get16(RESERVED_SECTORS_OFFSET);
    }

    void setNrReservedSectors(int v) {
        if (v == getNrReservedSectors()) return;
        if (v < 1) throw "there must be >= 1 reserved sectors";
        set16(RESERVED_SECTORS_OFFSET, v);
    }

    const int getNrFats() {
        return get8(FAT_COUNT_OFFSET);
    }

    const void setNrFats(int v) {
        if (v == getNrFats()) return;
        
        set8(FAT_COUNT_OFFSET, v);
    }
    
    int getMediumDescriptor() {
        return get8(0x15);
    }

    void setMediumDescriptor(int v) {
        set8(0x15, v);
    }
    
    int getSectorsPerTrack() {
        return get16(0x18);
    }

    
    void setSectorsPerTrack(int v) {
        if (v == getSectorsPerTrack()) return;
        
        set16(0x18, v);
    }

    int getNrHeads() {
        return get16(0x1a);
    }

    void setNrHeads(int v) {
        if (v == getNrHeads()) return;
        
        set16(0x1a, v);
    }

    long getNrHiddenSectors() {
        return get32(0x1c);
    }

    void setNrHiddenSectors(long v) {
        if (v == getNrHiddenSectors()) return;
        
        set32(0x1c, v);
    }

protected:
    BootSector(BlockDevice device) {
        super(device, 0, SIZE);
        markDirty();
    }
    
    protected int getNrLogicalSectors() {
        return get16(TOTAL_SECTORS_16_OFFSET);
    }
    
    protected void setNrLogicalSectors(int v) {
        if (v == getNrLogicalSectors()) return;
        
        set16(TOTAL_SECTORS_16_OFFSET, v);
    }
    
    protected void setNrTotalSectors(long v) {
        set32(TOTAL_SECTORS_32_OFFSET, v);
    }
    
    protected long getNrTotalSectors() {
        return get32(TOTAL_SECTORS_32_OFFSET);
    }
    
private:
    static bool isPowerOfTwo(int n) {
        return ((n!=0) && (n&(n-1))==0);
    }

    long getDataSize() {
        return (getSectorCount() * getBytesPerSector()) -
                getFilesOffset();
    }    
};
}
