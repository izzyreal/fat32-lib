#include "BootSector.hpp"

namespace akaifat::fat {
class Fat32BootSector : public BootSector {
public:
    const static int ROOT_DIR_FIRST_CLUSTER_OFFSET = 0x2c;

    static const int SECTORS_PER_FAT_OFFSET = 0x24;

    static const int FILE_SYSTEM_TYPE_OFFSET = 0x52;
    
    static const int VERSION_OFFSET = 0x2a;
    static const int VERSION = 0;

    static const int FS_INFO_SECTOR_OFFSET = 0x30;
    static const int BOOT_SECTOR_COPY_OFFSET = 0x32;
    static const int EXTENDED_BOOT_SIGNATURE_OFFSET = 0x42;
    
    Fat32BootSector(BlockDevice device) throw (std::exception) {
        super(device);
    }
    
    void init() throw (std::exception) override {
        super.init();

        set16(VERSION_OFFSET, VERSION);

        setBootSectorCopySector(6); /* as suggested by M$ */
    }

    long getRootDirFirstCluster() {
        return get32(ROOT_DIR_FIRST_CLUSTER_OFFSET);
    }

    void setRootDirFirstCluster(long value) {
        if (getRootDirFirstCluster() == value) return;
        
        set32(ROOT_DIR_FIRST_CLUSTER_OFFSET, value);
    }

    void setBootSectorCopySector(int sectNr) {
        if (getBootSectorCopySector() == sectNr) return;
        if (sectNr < 0) throw "boot sector copy sector must be >= 0";
        
        set16(BOOT_SECTOR_COPY_OFFSET, sectNr);
    }
    
    int getBootSectorCopySector() {
        return get16(BOOT_SECTOR_COPY_OFFSET);
    }

    void setVolumeLabel(std::string label) {
        for (int i=0; i < 11; i++) {
            const byte c =
                    (label == null) ? 0 :
                    (label.length() > i) ? (char) label.charAt(i) : 0x20;

            set8(0x47 + i, c);
        }
    }

    int getFsInfoSectorNr() {
        return get16(FS_INFO_SECTOR_OFFSET);
    }

    void setFsInfoSectorNr(int offset) {
        if (getFsInfoSectorNr() == offset) return;

        set16(FS_INFO_SECTOR_OFFSET, offset);
    }
    
    void setSectorsPerFat(long v) override {
        if (getSectorsPerFat() == v) return;
        
        set32(SECTORS_PER_FAT_OFFSET, v);
    }
    
    long getSectorsPerFat() override {
        return get32(SECTORS_PER_FAT_OFFSET);
    }

    FatType getFatType() override {
        return FatType.FAT32;
    }

    void setSectorCount(long count) override {
        super.setNrTotalSectors(count);
    }

    long getSectorCount() override {
        return super.getNrTotalSectors();
    }

    Override
    int getRootDirEntryCount() {
        return 0;
    }
    
    void setFileSystemId(int id) {
        super.set32(0x43, id);
    }

    int getFileSystemId() {
        return (int) super.get32(0x43);
    }

    void writeCopy(BlockDevice device) throw (std::exception) {
        if (getBootSectorCopySector() > 0) {
            const long offset = (long)getBootSectorCopySector() * SIZE;
            buffer.rewind();
            buffer.limit(buffer.capacity());
            device.write(offset, buffer);
        }
    }

    int getFileSystemTypeLabelOffset() override {
        return FILE_SYSTEM_TYPE_OFFSET;
    }

    int getExtendedBootSignatureOffset() override {
        return EXTENDED_BOOT_SIGNATURE_OFFSET;
    }
};
}
