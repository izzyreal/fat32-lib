#include "../AbstractFileSystem.hpp"

namespace akaifat::fat {
class FatFileSystem : public akaifat::AbstractFileSystem {
public:
    FatFileSystem(BlockDevice api, bool readOnly) throw (std::exception)
    : AkaiFatFileSystem(api, readOnly, false)
    {
    }

    static FatFileSystem read(BlockDevice device, bool readOnly)
            throw (std::exception) {
        
        return new FatFileSystem(device, readOnly);
    }

    long getFilesOffset() {
        checkClosed();
        
        return filesOffset;
    }

    FatType getFatType() {
        checkClosed();

        return fatType;
    }

    std::string getVolumeLabel() {
        checkClosed();
        
        const std::string fromDir = rootDirStore.getLabel();
        
        if (fromDir == null && fatType != FatType.FAT32) {
            return ((Fat16BootSector)bs).getVolumeLabel();
        } else {
            return fromDir;
        }
    }
    
    void setVolumeLabel(std::string label)
            throws ReadOnlyException, IOException {
        
        checkClosed();
        checkReadOnly();

        rootDirStore.setLabel(label);
        
        if (fatType != FatType.FAT32) {
            ((Fat16BootSector)bs).setVolumeLabel(label);
        }
    }

    AbstractDirectory getRootDirStore() {
        checkClosed();
        
        return rootDirStore;
    }
    
    void flush() throw (std::exception) override {
        checkClosed();
        
        if (bs.isDirty()) {
            bs.write();
        }
        
        for (int i = 0; i < bs.getNrFats(); i++) {
            fat.writeCopy(bs.getFatOffset(i));
        }
        
        rootDir.flush();
        
        if (fsiSector != null) {
            fsiSector.setFreeClusterCount(fat.getFreeClusterCount());
            fsiSector.setLastAllocatedCluster(fat.getLastAllocatedCluster());
            fsiSector.write();
        }
    }
    
    FatLfnDirectory getRoot() override {
        checkClosed();
        
        return rootDir;
    }
    
    Fat getFat() {
        return fat;
    }

    BootSector getBootSector() {
        checkClosed();
        
        return bs;
    }

    long getFreeSpace() override {
        checkClosed();

        return fat.getFreeClusterCount() * bs.getBytesPerCluster();
    }

    long getTotalSpace() override {
        checkClosed();

        if (fatType == FatType.FAT32) {
            return bs.getNrTotalSectors() * bs.getBytesPerSector();
        }

        return -1;
    }

    long getUsableSpace() override {
        checkClosed();

        return bs.getDataClusterCount() * bs.getBytesPerCluster();
    }
private:
    const Fat fat;
    const FsInfoSector fsiSector;
    const BootSector bs;
    const FatLfnDirectory rootDir;
    const AbstractDirectory rootDirStore;
    const FatType fatType;
    const long filesOffset;
    
    FatFileSystem(BlockDevice device, bool readOnly,
            bool ignoreFatDifferences)
            throw (std::exception)
    : akaifat::AbstractFileSystem(readOnly)
    {        
        bs = BootSector.read(device);
        
        if (bs.getNrFats() <= 0) throw new std::exception(
                "boot sector says there are no FATs");
        
        filesOffset = bs.getFilesOffset();
        fatType = bs.getFatType();
        fat = Fat.read(bs, 0);

        if (!ignoreFatDifferences) {
            for (int i=1; i < bs.getNrFats(); i++) {
                const Fat tmpFat = Fat.read(bs, i);
                if (!fat.equals(tmpFat)) {
                    throw new std::exception("FAT " + i + " differs from FAT 0");
                }
            }
        }
        
        if (fatType == FatType.FAT32) {
            const Fat32BootSector f32bs = (Fat32BootSector) bs;
            const ClusterChain rootChain = new ClusterChain(fat,
                    f32bs.getRootDirFirstCluster(), isReadOnly());
            rootDirStore = ClusterChainDirectory.readRoot(rootChain);
            fsiSector = FsInfoSector.read(f32bs);
            
            if (fsiSector.getFreeClusterCount() < fat.getFreeClusterCount()) {
                throw new std::exception("free cluster count mismatch - fat: " +
                        fat.getFreeClusterCount() + " - fsinfo: " +
                        fsiSector.getFreeClusterCount());
            }
        } else {
            rootDirStore =
                    Fat16RootDirectory.read((Fat16BootSector) bs,readOnly);
            fsiSector = null;
        }

        rootDir = new FatLfnDirectory(rootDirStore, fat, isReadOnly()); 
    }
};
}
