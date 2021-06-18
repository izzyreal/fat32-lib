#pragma once

#include "../AbstractFileSystem.hpp"

#include "FatType.hpp"
#include "AkaiFatLfnDirectory.hpp"
#include "Fat.hpp"
#include "Fat16BootSector.hpp"

#include <memory>

namespace akaifat::fat {
class AkaiFatFileSystem : public akaifat::AbstractFileSystem
{
private:
    Fat* fat;
    Fat16BootSector* bs;
    AkaiFatLfnDirectory* rootDir;
    AbstractDirectory* rootDirStore;
    FatType* fatType;
    long filesOffset;
            
public:
    AkaiFatFileSystem(BlockDevice* device, bool readOnly,
            bool ignoreFatDifferences);
    
    AkaiFatFileSystem(BlockDevice* api, bool readOnly)
    // Should ignoreFatDifferences be false?
    : AkaiFatFileSystem (api, readOnly, true) {}

    static AkaiFatFileSystem* read(BlockDevice* device, bool readOnly);

    ~AkaiFatFileSystem() {
        if (fat != nullptr) delete fat;
        if (bs != nullptr) delete bs;
        if (rootDir != nullptr) delete rootDir;
        if (rootDirStore != nullptr) delete rootDirStore;
        if (fatType != nullptr) delete fatType;
    }
    
    long getFilesOffset();

    FatType* getFatType();

    std::string getVolumeLabel();
    
    void setVolumeLabel(std::string label);

    AbstractDirectory* getRootDirStore();
    
    void flush() override;
    
    AkaiFatLfnDirectory* getRoot() override;
    
    Fat* getFat();

    BootSector* getBootSector();

    long getFreeSpace() override;

    long getTotalSpace() override;

    long getUsableSpace() override;
};
}
