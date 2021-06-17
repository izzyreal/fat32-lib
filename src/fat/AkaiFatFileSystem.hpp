#pragma once

#include "../AbstractFileSystem.hpp"

#include "FatType.hpp"
#include "AkaiFatLfnDirectory.hpp"
#include "Fat.hpp"
#include "BootSector.hpp"

#include <memory>

namespace akaifat::fat {
class AkaiFatFileSystem : public akaifat::AbstractFileSystem
{
private:
    Fat* fat;
    BootSector* bs;
    AkaiFatLfnDirectory* rootDir;
    AbstractDirectory* rootDirStore;
    FatType* fatType;
    long filesOffset;
  
    AkaiFatFileSystem(BlockDevice* device, bool readOnly,
            bool ignoreFatDifferences);
            
public:
    static AkaiFatFileSystem* read(BlockDevice* device, bool readOnly);

    AkaiFatFileSystem(BlockDevice* api, bool readOnly)
    : AkaiFatFileSystem (api, readOnly, false) {}

    long getFilesOffset();

    FatType* getFatType();

    std::string getVolumeLabel();
    
    void setVolumeLabel(std::string label);

    AbstractDirectory getRootDirStore();
    
    void flush() override;
    
    AkaiFatLfnDirectory* getRoot() override;
    
    Fat* getFat();

    BootSector getBootSector();

    long getFreeSpace() override;

    long getTotalSpace() override;

    long getUsableSpace() override;
};
}
