#pragma once

#include "AbstractDirectory.hpp"

namespace akaifat::fat {
class Fat16RootDirectory : public AbstractDirectory {
private:
    BlockDevice* device;
    long deviceOffset;

    Fat16RootDirectory(Fat16BootSector* bs, bool readOnly)
    : AbstractDirectory(bs->getFatType(), bs->getRootDirEntryCount(), readOnly, true)
    {
        if (bs->getRootDirEntryCount() <= 0) throw "root directory size is " + std::to_string(bs->getRootDirEntryCount());
        
        deviceOffset = bs->getRootDirOffset();
        device = bs->getDevice();
    }

protected:    
    void read(ByteBuffer& data) override {
        device->read(deviceOffset, data);
    }

    
    void write(ByteBuffer& data) override {
        device->write(deviceOffset, data);
    }

    
    long getStorageCluster() override {
        return 0;
    }

    void changeSize(int entryCount) override {
        if (getCapacity() < entryCount) {
            throw "directory full";
        }
    }

public:
    static Fat16RootDirectory* read(
            Fat16BootSector* bs, bool readOnly) {
//        Fat16RootDirectory result(bs, readOnly);
//        result.read();
//        return result;
        return nullptr;
    }

    static Fat16RootDirectory* create(
            Fat16BootSector* bs) {
//        Fat16RootDirectory result(bs, false);
//        result.flush();
//        return result;
        return nullptr;
    }    
};
}
