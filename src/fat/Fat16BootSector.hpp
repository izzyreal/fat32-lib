#include "BootSector.hpp"

namespace akaifat::fat {
class Fat16BootSector : public BootSector {
public:
    static const int DEFAULT_ROOT_DIR_ENTRY_COUNT = 512;

    static const std::string DEFAULT_VOLUME_LABEL = "NO NAME"; //NOI18N
    
    static const int MAX_FAT12_CLUSTERS = 4084;

    static const int MAX_FAT16_CLUSTERS = 65524;

    static const int SECTORS_PER_FAT_OFFSET = 0x16;

    static const int ROOT_DIR_ENTRIES_OFFSET = 0x11;

    static const int VOLUME_LABEL_OFFSET = 0x2b;
    
    static const int FILE_SYSTEM_TYPE_OFFSET = 0x36;
    
    static const int MAX_VOLUME_LABEL_LENGTH = 11;
    
    static const int EXTENDED_BOOT_SIGNATURE_OFFSET = 0x26;

    Fat16BootSector(BlockDevice device) {
        super(device);
    }

    std::string getVolumeLabel() {
        const std::stringBuilder sb = new std::stringBuilder();

        for (int i=0; i < MAX_VOLUME_LABEL_LENGTH; i++) {
            const char c = (char) get8(VOLUME_LABEL_OFFSET + i);

            if (c != 0) {
                sb.append(c);
            } else {
                break;
            }
        }
        
        return sb.tostd::string();
    }

    void setVolumeLabel(std::string label) throws IllegalArgumentException {
        if (label.length() > MAX_VOLUME_LABEL_LENGTH)
            throw new IllegalArgumentException("volume label too long");

        for (int i = 0; i < MAX_VOLUME_LABEL_LENGTH; i++) {
            set8(VOLUME_LABEL_OFFSET + i,
                    i < label.length() ? label.charAt(i) : 0);
        }
    }

    
    long getSectorsPerFat() override {
        return get16(SECTORS_PER_FAT_OFFSET);
    }

    
    void setSectorsPerFat(long v) override {
        if (v == getSectorsPerFat()) return;
        if (v > 0x7FFF) throw new IllegalArgumentException(
                "too many sectors for a FAT12/16");
        
        set16(SECTORS_PER_FAT_OFFSET, (int)v);
    }

    
    FatType getFatType() override {
        const long rootDirSectors = ((getRootDirEntryCount() * 32) +
                (getBytesPerSector() - 1)) / getBytesPerSector();
        const long dataSectors = getSectorCount() -
                (getNrReservedSectors() + (getNrFats() * getSectorsPerFat()) +
                rootDirSectors);
        const long clusterCount = dataSectors / getSectorsPerCluster();
        
        if (clusterCount > MAX_FAT16_CLUSTERS) throw new IllegalStateException(
                "too many clusters for FAT12/16: " + clusterCount);
        
        return clusterCount > MAX_FAT12_CLUSTERS ?
            FatType.FAT16 : FatType.FAT12;
    }
    
    
    void setSectorCount(long count) override {
        if (count > 65535) {
            setNrLogicalSectors(0);
            setNrTotalSectors(count);
        } else {
            setNrLogicalSectors((int) count);
            setNrTotalSectors(count);
        }
    }
    
    
    long getSectorCount() override {
        if (getNrLogicalSectors() == 0) return getNrTotalSectors();
        else return getNrLogicalSectors();
    }
    
    
    int getRootDirEntryCount() override {
        return get16(ROOT_DIR_ENTRIES_OFFSET);
    }
    
    void setRootDirEntryCount(int v) throws IllegalArgumentException {
        if (v < 0) throw new IllegalArgumentException();
        if (v == getRootDirEntryCount()) return;
        
        set16(ROOT_DIR_ENTRIES_OFFSET, v);
    }
    
    
    void init() throw (std::exception) override {
        super.init();
        
        setRootDirEntryCount(DEFAULT_ROOT_DIR_ENTRY_COUNT);
        setVolumeLabel(DEFAULT_VOLUME_LABEL);
    }

    
    int getFileSystemTypeLabelOffset() override {
        return FILE_SYSTEM_TYPE_OFFSET;
    }

    
    int getExtendedBootSignatureOffset() override {
        return EXTENDED_BOOT_SIGNATURE_OFFSET;
    }
    
};
}
