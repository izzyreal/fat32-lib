#include "AkaiFatFileSystem.hpp"

#include "BlockDevice.hpp"
#include "../AbstractFileSystem.hpp"
#include

using namespace akaifat;

AkaiFatFileSystem::AkaiFatFileSystem(
    BlockDevice* device,
    bool readOnly,
    bool ignoreFatDifferences
) : akaifat::AbstractFileSystem (readOnly),
    bs (BootSector::read(device))
{                
    if (bs.getNrFats() <= 0) 
            throw "boot sector says there are no FATs";
    
//    filesOffset = bs->getFilesOffset();
//    fatType = bs->getFatType();
//    fat = Fat::read(bs, 0);
//
//    if (!ignoreFatDifferences)
//    {
//        for (int i=1; i < bs.getNrFats(); i++)
//        {
//            Fat tmpFat = Fat.read(bs, i);
//        
//            if (!fat.equals(tmpFat))
//                throw "FAT " + i + " differs from FAT 0";
//        }
//    }
//
//        rootDirStore =
//                Fat16RootDirectory.read((Fat16BootSector) bs,readOnly);
//
//    rootDir = new AkaiFatLfnDirectory(rootDirStore, fat, isReadOnly());
        
}

AkaiFatFileSystem* AkaiFatFileSystem::read(BlockDevice* device, bool readOnly)
{    
    return new AkaiFatFileSystem(device, readOnly);
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
    
    std::string fromDir = rootDirStore.getLabel();
    
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
}

AkaiFatLfnDirectory& AkaiFatFileSystem::getRoot()
{
    checkClosed();
    
    return rootDir;
}

Fat* AkaiFatFileSystem::getFat()
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
