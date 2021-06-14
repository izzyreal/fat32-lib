#include "AbstractDirectory.hpp"

#include "DirectoryFullException.hpp"

namespace akaifat::fat {
class Fat16RootDirectory : public AbstractDirectory {
private:
    const BlockDevice device;
    const long deviceOffset;

    Fat16RootDirectory(Fat16BootSector bs, bool readOnly)
    : AbstractDirectory(bs.getFatType(), bs.getRootDirEntryCount(), readOnly, true)
    {
        if (bs.getRootDirEntryCount() <= 0) throw new IllegalArgumentException(
                "root directory size is " + bs.getRootDirEntryCount());
        
        deviceOffset = bs.getRootDirOffset();
        device = bs.getDevice();
    }

protected:    
    void read(ByteBuffer data) throw (std::exception) override {
        device.read(deviceOffset, data);
    }

    
    void write(ByteBuffer data) throw (std::exception) override {
        device.write(deviceOffset, data);
    }

    
    long getStorageCluster() override {
        return 0;
    }

    void changeSize(int entryCount) throws DirectoryFullException override {
        if (getCapacity() < entryCount) {
            throw new DirectoryFullException(getCapacity(), entryCount);
        }
    }

public:
    static Fat16RootDirectory read(
            Fat16BootSector bs, bool readOnly) throw (std::exception) {
        
        const Fat16RootDirectory result = new Fat16RootDirectory(bs, readOnly);
        result.read();
        return result;
    }

    static Fat16RootDirectory create(
            Fat16BootSector bs) throw (std::exception) {
        
        const Fat16RootDirectory result = new Fat16RootDirectory(bs, false);
        result.flush();
        return result;
    }    
}
