#include "AkaiFatFileSystem.hpp"

using namespace akaifat;

AkaiFatFileSystem::AkaiFatFileSystem(
    BlockDevice device,
    bool readOnly,
    bool ignoreFatDifferences
) : akaifat::AbstractFileSystem (readOnly),
    bs (BootSector::read(device))
{                
    if (bs.getNrFats() <= 0) 
            throw "boot sector says there are no FATs";
    
    filesOffset = bs.getFilesOffset();
    fatType = bs.getFatType();
    fat = Fat.read(bs, 0);

    if (!ignoreFatDifferences)
    {
        for (int i=1; i < bs.getNrFats(); i++)
        {
            const Fat tmpFat = Fat.read(bs, i);
        
            if (!fat.equals(tmpFat))
                throw "FAT " + i + " differs from FAT 0";
        }
    }
    
    if (fatType == FatType.FAT32)
    {
        const Fat32BootSector f32bs = (Fat32BootSector) bs;
        const ClusterChain rootChain = new ClusterChain(fat,
                f32bs.getRootDirFirstCluster(), isReadOnly());
    
        rootDirStore = ClusterChainDirectory.readRoot(rootChain);
        fsiSector = FsInfoSector.read(f32bs);
        
        if (fsiSector.getFreeClusterCount() < fat.getFreeClusterCount())
        {
            "free cluster count mismatch - fat: " +
                    fat.getFreeClusterCount() + " - fsinfo: " +
                    fsiSector.getFreeClusterCount());
        }
    }
    else
    {
        rootDirStore =
                Fat16RootDirectory.read((Fat16BootSector) bs,readOnly);
        fsiSector = null;
    }

    rootDir = new AkaiFatLfnDirectory(rootDirStore, fat, isReadOnly());
        
}

std::shared_ptr<AkaiFatFileSystem> AkaiFatFileSystem::read(std::shared_ptr<BlockDevice> device, bool readOnly)
{    
    return std::make_shared<AkaiFatFileSystem>(device, readOnly);
}

long AkaiFatFileSystem::getFilesOffset()
{
    checkClosed();
    
    return filesOffset;
}

FatType AkaiFatFileSystem::getFatType()
{
    checkClosed();

    return fatType;
}

std::string AkaiFatFileSystem::getVolumeLabel()
{
    checkClosed();
    
    const std::string fromDir = rootDirStore.getLabel();
    
    if (fromDir == null && fatType != FatType.FAT32) {
        return ((Fat16BootSector)bs).getVolumeLabel();
    } else {
        return fromDir;
    }
}

void AkaiFatFileSystem::setVolumeLabel(std::string label)
{
    
    checkClosed();
    checkReadOnly();

    rootDirStore.setLabel(label);
    
    if (fatType != FatType.FAT32)
        dynamic_cast<Fat16BootSector*>(bs)->setVolumeLabel(label);
}

AbstractDirectory& AkaiFatFileSystem::getRootDirStore()
{
    checkClosed();
    
    return rootDirStore;
}

void AkaiFatFileSystem::flush()
{
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

AkaiFatLfnDirectory& AkaiFatFileSystem::getRoot()
{
    checkClosed();
    
    return rootDir;
}

Fat AkaiFatFileSystem::getFat()
{
    return fat;
}

BootSector& AkaiFatFileSystem::getBootSector()
{
    checkClosed();
    
    return bs;
}

long AkaiFatFileSystem::getFreeSpace()
{
    checkClosed();

    return fat.getFreeClusterCount() * bs.getBytesPerCluster();
}

long AkaiFatFileSystem::getTotalSpace()
{
    checkClosed();

    if (fatType == FatType.FAT32) {
        return bs.getNrTotalSectors() * bs.getBytesPerSector();
    }

    return -1;
}

long AkaiFatFileSystem::getUsableSpace()
{
    checkClosed();

    return bs.getDataClusterCount() * bs.getBytesPerCluster();
}
