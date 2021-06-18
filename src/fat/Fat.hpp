#pragma once

#include "BootSector.hpp"
#include "FatType.hpp"

#include <memory>

namespace akaifat::fat {
class Fat {
private:
    std::vector<long> entries;
    FatType* fatType;
    BootSector* bs;
    long offset;
    int lastClusterIndex;
    int sectorCount;
    int sectorSize;
    BlockDevice* device;
    
    int lastAllocatedCluster;

    Fat(BootSector* _bs, long _offset)
    : bs (_bs), offset (_offset)
    {
        device = bs->getDevice();
        fatType = bs->getFatType();
        
        if (bs->getSectorsPerFat() > INT_MAX)
            throw "FAT too large";

        if (bs->getSectorsPerFat() <= 0)
                throw "boot sector says there are " + std::to_string(bs->getSectorsPerFat()) +
                " sectors per FAT";

        if (bs->getBytesPerSector() <= 0)
                throw "boot sector says there are " + std::to_string(bs->getBytesPerSector()) +
                " bytes per sector";

        sectorCount = (int) bs->getSectorsPerFat();
        sectorSize = bs->getBytesPerSector();
        lastAllocatedCluster = FIRST_CLUSTER;
        
        if (bs->getDataClusterCount() > INT_MAX) throw
            "too many data clusters";
        
        if (bs->getDataClusterCount() == 0) throw
            "no data clusters";
        
        lastClusterIndex = (int) bs->getDataClusterCount() + FIRST_CLUSTER;

        entries = std::vector<long>((sectorCount * sectorSize) /
                fatType->getEntrySize());
                
        if (lastClusterIndex > entries.size())
            throw std::runtime_error("file system has " + std::to_string(lastClusterIndex) +
            " clusters but only " + std::to_string(entries.size()) + " FAT entries");
    }
    

    void init(int mediumDescriptor) {
        entries[0] =
                (mediumDescriptor & 0xFF) |
                (0xFFFFF00L & fatType->getBitMask());
        entries[1] = fatType->getEofMarker();
    }
    
    void read() {
        std::vector<char> data(sectorCount * sectorSize);
        ByteBuffer bb(data);
        device->read(offset, bb);

        for (int i = 0; i < entries.size(); i++)
            entries[i] = fatType->readEntry(data, i);
    }
    
public:
    static const int FIRST_CLUSTER = 2;
    
    static Fat* read(BootSector* bs, int fatNr) {
        
        if (fatNr > bs->getNrFats()) {
            throw "boot sector says there are only " + std::to_string(bs->getNrFats()) +
                    " FATs when reading FAT #" + std::to_string(fatNr);
        }
        
        long fatOffset = bs->getFatOffset(fatNr);
        auto result = new Fat(bs, fatOffset);
//        result.read();
        return result;
    }
    
    static Fat* create(BootSector* bs, int fatNr) {
        
        if (fatNr > bs->getNrFats()) {
            throw "boot sector says there are only " + std::to_string(bs->getNrFats()) +
                    " FATs when creating FAT #" + std::to_string(fatNr);
        }
        
        long fatOffset = bs->getFatOffset(fatNr);
        auto result = new Fat(bs, fatOffset);

        if (bs->getDataClusterCount() > result->entries.size())
            throw "FAT too small for device";
            
        result->init(bs->getMediumDescriptor());
        result->write();
        return result;
    }

    FatType* getFatType() {
        return fatType;
    }
    
    BootSector* getBootSector() {
        return bs;
    }

    BlockDevice* getDevice() {
        return device;
    }
   
    void write() {
        writeCopy(offset);
    }
    
    void writeCopy(long offset) {
        std::vector<char> data(sectorCount * sectorSize);
        
        for (int index = 0; index < entries.size(); index++) {
            fatType->writeEntry(data, index, entries[index]);
        }
        
        auto bb = ByteBuffer(data);
        device->write(offset, bb);
    }
    
    int getMediumDescriptor() {
        return (int) (entries[0] & 0xFF);
    }
    
    long getEntry(int index) {
        return entries[index];
    }

    int getLastFreeCluster() {
        return lastAllocatedCluster;
    }
    
    std::vector<long> getChain(long startCluster) {
        testCluster(startCluster);
        // Count the chain first
        int count = 1;
        long cluster = startCluster;
        while (!isEofCluster(entries[(int) cluster])) {
            count++;
            cluster = entries[(int) cluster];
        }
        // Now create the chain
        std::vector<long> chain(count);
        chain[0] = startCluster;
        cluster = startCluster;
        int i = 0;
        while (!isEofCluster(entries[(int) cluster])) {
            cluster = entries[(int) cluster];
            chain[++i] = cluster;
        }
        return chain;
    }

    long getNextCluster(long cluster) {
        testCluster(cluster);
        long entry = entries[(int) cluster];
        if (isEofCluster(entry)) {
            return -1;
        } else {
            return entry;
        }
    }

    long allocNew() {

        int i;
        int entryIndex = -1;

        for (i = lastAllocatedCluster; i < lastClusterIndex; i++) {
            if (isFreeCluster(i)) {
                entryIndex = i;
                break;
            }
        }
        
        if (entryIndex < 0) {
            for (i = FIRST_CLUSTER; i < lastAllocatedCluster; i++) {
                if (isFreeCluster(i)) {
                    entryIndex = i;
                    break;
                }
            }
        }
        
        if (entryIndex < 0) {
            throw "FAT Full (" + std::to_string(lastClusterIndex - FIRST_CLUSTER)
                    + ", " + std::to_string(i) + ")";
        }
        
        entries[entryIndex] = fatType->getEofMarker();
        lastAllocatedCluster = entryIndex % lastClusterIndex;
        if (lastAllocatedCluster < FIRST_CLUSTER)
            lastAllocatedCluster = FIRST_CLUSTER;
        
        return entryIndex;
    }
    
    int getFreeClusterCount() {
        int result = 0;

        for (int i=FIRST_CLUSTER; i < lastClusterIndex; i++) {
            if (isFreeCluster(i)) result++;
        }

        return result;
    }

    int getLastAllocatedCluster() {
        return lastAllocatedCluster;
    }
    
    std::vector<long> allocNew(int nrClusters) {
        std::vector<long> rc(nrClusters);
        
        rc[0] = allocNew();
        for (int i = 1; i < nrClusters; i++) {
            rc[i] = allocAppend(rc[i - 1]);
        }
        
        return rc;
    }
    
    long allocAppend(long cluster) {
        
        testCluster(cluster);
        
        while (!isEofCluster(entries[(int) cluster])) {
            cluster = entries[(int) cluster];
        }
        
        long newCluster = allocNew();
        entries[(int) cluster] = newCluster;

        return newCluster;
    }

    void setEof(long cluster) {
        testCluster(cluster);
        entries[(int) cluster] = fatType->getEofMarker();
    }

    void setFree(long cluster) {
        testCluster(cluster);
        entries[(int) cluster] = 0;
    }
    
    bool equals(Fat* other) {
        if (fatType != other->fatType) return false;
        if (sectorCount != other->sectorCount) return false;
        if (sectorSize != other->sectorSize) return false;
        if (lastClusterIndex != other->lastClusterIndex) return false;
//        if (!Arrays.equals(entries, other.entries)) return false;
        
        return (getMediumDescriptor() == other->getMediumDescriptor());
    }
    
    int hashCode() {
        int hash = 7;
//        hash = 23 * hash + Arrays.hashCode(entries);
        hash = 23 * hash + (int) fatType->hashCode();
        hash = 23 * hash + sectorCount;
        hash = 23 * hash + sectorSize;
        hash = 23 * hash + lastClusterIndex;
        return hash;
    }

    // Can be protected?
    void testCluster(long cluster) {
        if ((cluster < FIRST_CLUSTER) || (cluster >= entries.size())) {
            throw "invalid cluster value " + std::to_string(cluster);
        }
    }

    bool isFreeCluster(long entry) {
        if (entry > INT_MAX) throw "entry is bigger than INT_MAX";
        return (entries[(int) entry] == 0);
    }
    
    bool isReservedCluster(long entry) {
        return fatType->isReservedCluster(entry);
    }

    bool isEofCluster(long entry) {
        return fatType->isEofCluster(entry);
    }

};
}
