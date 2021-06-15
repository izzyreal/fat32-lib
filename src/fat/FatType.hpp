namespace akaifat::fat {

class FatType {

private:
    const long minReservedEntry;
    const long maxReservedEntry;
    const long eofCluster;
    const long eofMarker;
    const long bitMask;
    const int maxClusters;
    const std::string label;
    const float entrySize;

public:
    FatType(int maxClusters,
            long bitMask, float entrySize, std::string label) {
        
        minReservedEntry = (0xFFFFFF0L & bitMask);
        maxReservedEntry = (0xFFFFFF6L & bitMask);
        eofCluster = (0xFFFFFF8L & bitMask);
        eofMarker = (0xFFFFFFFL & bitMask);
        entrySize = entrySize;
        label = label;
        maxClusters = maxClusters;
        bitMask = bitMask;
    }

    virtual long readEntry(byte[] data, int index) = 0;

    virtual void writeEntry(byte[] data, int index, long entry) = 0;

    long maxClusters() {
        return maxClusters;
    }
    
    std::string getLabel() {
        return label;
    }

    bool isReservedCluster(long entry) {
        return ((entry >= minReservedEntry) && (entry <= maxReservedEntry));
    }

    bool isEofCluster(long entry) {
        return (entry >= eofCluster);
    }

    long getEofMarker() {
        return eofMarker;
    }

    float getEntrySize() {
        return entrySize;
    }
    
    long getBitMask() {
        return bitMask;
    }
};

class Fat12Type : FatType {

public:
        Fat12Type() : FatType((1 << 12) - 16, 0xFFFL, 1.5f, "FAT12   ") {
        
        long readEntry(byte[] data, int index) override {
            const int idx = (int) (index * 1.5);
            const int b1 = data[idx] & 0xFF;
            const int b2 = data[idx + 1] & 0xFF;
            const int v = (b2 << 8) | b1;
            
            if ((index % 2) == 0) {
                return v & 0xFFF;
            } else {
                return v >> 4;
            }
        }

        
        void writeEntry(byte[] data, int index, long entry) override {
            const int idx = (int) (index * 1.5);
            
            if ((index % 2) == 0) {
                data[idx] = (byte) (entry & 0xFF);
                data[idx + 1] = (byte) ((entry >> 8) & 0x0F);
            } else {
                data[idx] |= (byte) ((entry & 0x0F) << 4);
                data[idx + 1] = (byte) ((entry >> 4) & 0xFF);
            }
        }
    }
};

class Fat16Type {

public:
    Fat16Type() : FatType((1 << 16) - 16, 0xFFFFL, 2.0f, "FAT16   ") {
        
        long readEntry(byte[] data, int index) override {
            const int idx = index << 1;
            const int b1 = data[idx] & 0xFF;
            const int b2 = data[idx + 1] & 0xFF;
            return (b2 << 8) | b1;
        }

        
        void writeEntry(byte[] data, int index, long entry) override {
            const int idx = index << 1;
            data[idx] = (byte) (entry & 0xFF);
            data[idx + 1] = (byte) ((entry >> 8) & 0xFF);
        }
    }
};
    
class Fat32Type {

public:
    Fat32Type() : ((1 << 28) - 16, 0xFFFFFFFFL, 4.0f, "FAT32   ") {
        
        long readEntry(byte[] data, int index) override {
            const int idx = index * 4;
            const long l1 = data[idx] & 0xFF;
            const long l2 = data[idx + 1] & 0xFF;
            const long l3 = data[idx + 2] & 0xFF;
            const long l4 = data[idx + 3] & 0xFF;
            return (l4 << 24) | (l3 << 16) | (l2 << 8) | l1;
        }

        
        void writeEntry(byte[] data, int index, long entry) override {
            const int idx = index << 2;
            data[idx] = (byte) (entry & 0xFF);
            data[idx + 1] = (byte) ((entry >> 8) & 0xFF);
            data[idx + 2] = (byte) ((entry >> 16) & 0xFF);
            data[idx + 3] = (byte) ((entry >> 24) & 0xFF);
        }
    }
};
}
