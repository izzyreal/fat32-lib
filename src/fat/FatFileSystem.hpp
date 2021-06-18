#pragma once

#include "../AbstractFileSystem.hpp"

#include "Fat.hpp"
#include "FatLfnDirectory.hpp"
#include "AbstractDirectory.hpp"
#include "FatType.hpp"

namespace akaifat::fat {
class FatFileSystem : public akaifat::AbstractFileSystem {
private:
    Fat* fat;
    BootSector* bs;
    FatLfnDirectory* rootDir;
    AbstractDirectory* rootDirStore;
    FatType* fatType;
    long filesOffset;
    
    FatFileSystem(BlockDevice* device, bool readOnly,
            bool ignoreFatDifferences)
           
    : akaifat::AbstractFileSystem(readOnly)
    {
        bs = BootSector::read(device);
        
        if (bs->getNrFats() <= 0)
               throw "boot sector says there are no FATs";
        
        filesOffset = bs->getFilesOffset();
        fatType = bs->getFatType();
        fat = Fat::read(bs, 0);

        if (!ignoreFatDifferences) {
            for (int i=1; i < bs->getNrFats(); i++) {
                auto tmpFat = Fat::read(bs, i);
                if (!fat->equals(tmpFat)) {
                    throw "FAT " + std::to_string(i) + " differs from FAT 0";
                }
            }
        }
        
        rootDirStore = Fat16RootDirectory::read(dynamic_cast<Fat16BootSector*>(bs), readOnly);
        rootDir = new FatLfnDirectory(rootDirStore, fat, isReadOnly());
    }

public:
    FatFileSystem(BlockDevice* api, bool readOnly)
    : FatFileSystem(api, readOnly, false)
    {
    }

    static FatFileSystem read(BlockDevice* device, bool readOnly) {
        return FatFileSystem(device, readOnly);
    }

    long getFilesOffset() {
        checkClosed();
        
        return filesOffset;
    }

    FatType* getFatType() {
        checkClosed();

        return fatType;
    }

    std::string getVolumeLabel() {
        checkClosed();
        
        std::string fromDir = rootDirStore->getLabel();
        
//        if (fromDir == null && fatType != FatType.FAT32) {
//            return ((Fat16BootSector)bs).getVolumeLabel();
//        } else {
//            return fromDir;
//        }
        return "";
    }
    
    void setVolumeLabel(std::string label) {
        
        checkClosed();
        checkReadOnly();

        rootDirStore->setLabel(label);
        
//        if (fatType != FatType.FAT32) {
//            ((Fat16BootSector)bs).setVolumeLabel(label);
//        }
    }

    AbstractDirectory* getRootDirStore() {
        checkClosed();
        
        return rootDirStore;
    }
    
    void flush() override {
        checkClosed();
        
//        if (bs->isDirty()) {
//            bs->write();
//        }
//
//        for (int i = 0; i < bs->getNrFats(); i++) {
//            fat.writeCopy(bs->getFatOffset(i));
//        }
//
//        rootDir.flush();
//
//        if (fsiSector != null) {
//            fsiSector.setFreeClusterCount(fat.getFreeClusterCount());
//            fsiSector.setLastAllocatedCluster(fat.getLastAllocatedCluster());
//            fsiSector.write();
//        }
    }
    
    FatLfnDirectory* getRoot() override {
        checkClosed();
        
        return rootDir;
    }
    
    Fat* getFat() {
        return fat;
    }

    BootSector* getBootSector() {
        checkClosed();
        
        return bs;
    }

    long getFreeSpace() override {
        checkClosed();

        return fat->getFreeClusterCount() * bs->getBytesPerCluster();
    }

    long getTotalSpace() override {
        checkClosed();

//        if (fatType == FatType.FAT32) {
//            return bs->getNrTotalSectors() * bs->getBytesPerSector();
//        }

        return -1;
    }

    long getUsableSpace() override {
        checkClosed();

        return bs->getDataClusterCount() * bs->getBytesPerCluster();
    }
};
}
