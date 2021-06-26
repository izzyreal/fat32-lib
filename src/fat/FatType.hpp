#pragma once

#include <vector>
#include <string>

namespace akaifat::fat {
class FatType {

private:
    long minReservedEntry;
    long maxReservedEntry;
    long eofCluster;
    long eofMarker;
    long bitMask;
    int _maxClusters;
    std::string label;
    float entrySize;

public:
    FatType(int __maxClusters,
            long _bitMask, float _entrySize, std::string _label)
    : _maxClusters (__maxClusters), bitMask (_bitMask), entrySize (_entrySize), label (_label),
    minReservedEntry (0xFFFFFF0L & _bitMask), maxReservedEntry (0xFFFFFF6L & _bitMask),
    eofCluster (0xFFFFFF8L & _bitMask),
    eofMarker (0xFFFFFFFL & _bitMask)
    {
    }
    
    virtual ~FatType(){}
    
    virtual long readEntry(std::vector<char>& data, int index) = 0;

    virtual void writeEntry(std::vector<char>& data, int index, long entry) = 0;

    long maxClusters() {
        return _maxClusters;
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
    
    // TODO Is this cool?
    long hashCode() {
        return (long) this;
    }
};

class Fat12Type : FatType {

public:
    Fat12Type() : FatType((1 << 12) - 16, 0xFFFL, 1.5f, "FAT12   "){}
        
        long readEntry(std::vector<char>& data, int index) override {
            int idx = (int) (index * 1.5);
            int b1 = data[idx] & 0xFF;
            int b2 = data[idx + 1] & 0xFF;
            int v = (b2 << 8) | b1;
            
            if ((index % 2) == 0) {
                return v & 0xFFF;
            } else {
                return v >> 4;
            }
        }

        
        void writeEntry(std::vector<char>& data, int index, long entry) override {
            int idx = (int) (index * 1.5);
            
            if ((index % 2) == 0) {
                data[idx] = (char) (entry & 0xFF);
                data[idx + 1] = (char) ((entry >> 8) & 0x0F);
            } else {
                data[idx] |= (char) ((entry & 0x0F) << 4);
                data[idx + 1] = (char) ((entry >> 4) & 0xFF);
            }
        }
};

class Fat16Type : public FatType {

public:
    Fat16Type() : FatType((1 << 16) - 16, 0xFFFFL, 2.0f, "FAT16   "){}
        
        long readEntry(std::vector<char>& data, int index) override {
            int idx = index << 1;
            int b1 = data[idx] & 0xFF;
            int b2 = data[idx + 1] & 0xFF;
            auto result = (b2 << 8) | b1;
            return result;
        }

        
        void writeEntry(std::vector<char>& data, int index, long entry) override {
            int idx = index << 1;
            data[idx] = (char) (entry & 0xFF);
            data[idx + 1] = (char) ((entry >> 8) & 0xFF);
        }
};
    
class Fat32Type : FatType {

public:
    Fat32Type() : FatType((1 << 28) - 16, 0xFFFFFFFFL, 4.0f, "FAT32   ") {}
        long readEntry(std::vector<char>& data, int index) override {
            int idx = index * 4;
            long l1 = data[idx] & 0xFF;
            long l2 = data[idx + 1] & 0xFF;
            long l3 = data[idx + 2] & 0xFF;
            long l4 = data[idx + 3] & 0xFF;
            return (l4 << 24) | (l3 << 16) | (l2 << 8) | l1;
        }

        
        void writeEntry(std::vector<char>& data, int index, long entry) override {
            int idx = index << 2;
            data[idx] = (char) (entry & 0xFF);
            data[idx + 1] = (char) ((entry >> 8) & 0xFF);
            data[idx + 2] = (char) ((entry >> 16) & 0xFF);
            data[idx + 3] = (char) ((entry >> 24) & 0xFF);
        }
};
}
