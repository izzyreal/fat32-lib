#include "Sector.hpp"

namespace akaifat::fat {
class FsInfoSector : public Sector {
public:
    static const int FREE_CLUSTERS_OFFSET = 0x1e8;

    static const int LAST_ALLOCATED_OFFSET = 0x1ec;

    static const int SIGNATURE_OFFSET = 0x1fe;

    static FsInfoSector read(Fat32BootSector bs) throw (std::exception) {
        const FsInfoSector result =
                new FsInfoSector(bs.getDevice(), offset(bs));
        
        result.read();
        result.verify();
        return result;
    }

    static FsInfoSector create(Fat32BootSector bs) throw (std::exception) {
        const int offset = offset(bs);

        if (offset == 0) throw new std::exception(
                "creating a FS info sector at offset 0 is strange");
        
        const FsInfoSector result =
                new FsInfoSector(bs.getDevice(), offset(bs));
        
        result.init();
        result.write();
        return result;
    }

    void setFreeClusterCount(long value) {
        if (getFreeClusterCount() == value) return;
        
        set32(FREE_CLUSTERS_OFFSET, value);
    }
    
    long getFreeClusterCount() {
        return get32(FREE_CLUSTERS_OFFSET);
    }

    void setLastAllocatedCluster(long value) {
        if (getLastAllocatedCluster() == value) return;
        
        super.set32(LAST_ALLOCATED_OFFSET, value);
    }

    long getLastAllocatedCluster() {
        return super.get32(LAST_ALLOCATED_OFFSET);
    }

private:
    void init() {
        buffer.position(0x00);
        buffer.put((byte) 0x52);
        buffer.put((byte) 0x52);
        buffer.put((byte) 0x61);
        buffer.put((byte) 0x41);
        
        /* 480 reserved bytes */

        buffer.position(0x1e4);
        buffer.put((byte) 0x72);
        buffer.put((byte) 0x72);
        buffer.put((byte) 0x41);
        buffer.put((byte) 0x61);
        
        setFreeClusterCount(-1);
        setLastAllocatedCluster(Fat.FIRST_CLUSTER);

        buffer.position(SIGNATURE_OFFSET);
        buffer.put((byte) 0x55);
        buffer.put((byte) 0xaa);
        
        markDirty();
    }

    FsInfoSector(BlockDevice device, long offset) {
        super(device, offset, BootSector.SIZE);
    }

    static int offset(Fat32BootSector bs) {
        return bs.getFsInfoSectorNr() * bs.getBytesPerSector();
    }

    void verify() throw (std::exception) {
        if (!(get8(SIGNATURE_OFFSET) == 0x55) ||
                !(get8(SIGNATURE_OFFSET + 1) == 0xaa)) {

            throw new std::exception("invalid FS info sector signature");
        }
    }    
};
}
