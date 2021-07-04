#pragma once

#include "../AbstractFileSystem.hpp"

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
    long filesOffset;
            
public:
    AkaiFatFileSystem(BlockDevice* device, bool readOnly,
            bool ignoreFatDifferences);
    
    AkaiFatFileSystem(BlockDevice* api, bool readOnly)
    // Should ignoreFatDifferences be false?
    : AkaiFatFileSystem (api, readOnly, true) {}

    static AkaiFatFileSystem* read(BlockDevice* device, bool readOnly);

    ~AkaiFatFileSystem() {
        delete fat;
        delete bs;
        delete rootDir;
        delete rootDirStore;
    }

    long getFilesOffset();

    std::string getVolumeLabel();

    void setVolumeLabel(std::string label);

    AbstractDirectory* getRootDirStore();
    
    void flush() override;
    
    AkaiFatLfnDirectory* getRoot() override;

    BootSector* getBootSector();

    long getFreeSpace() override;

    long getTotalSpace() override;

    long getUsableSpace() override;
};
}
