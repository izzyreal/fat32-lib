#include "../AbstractFileSystem.hpp"

#include <memory>

namespace akaifat::fat {
class AkaiFatFileSystem : public akaifat::AbstractFileSystem
{

public:
    AkaiFatFileSystem(BlockDevice api, bool readOnly) throw (std::exception)
        : AkaiFatFileSystem (api, readyOnly, false);

    long getFilesOffset();

    FatType getFatType();

    std::string getVolumeLabel();
    
    void setVolumeLabel(std::string label)
            throw (std::exception);

    AbstractDirectory getRootDirStore();
    
    void flush() throw (std::exception) override;
    
    AkaiFatLfnDirectory getRoot();
    
    Fat getFat();

    BootSector getBootSector();

    long getFreeSpace() override;

    long getTotalSpace() override;

    long getUsableSpace() override;

private:
    const Fat fat;
    const FsInfoSector fsiSector;
    const BootSector bs;
    const AkaiFatLfnDirectory rootDir;
    const AbstractDirectory rootDirStore;
    const FatType fatType;
    const long filesOffset;
  
    AkaiFatFileSystem(BlockDevice device, bool readOnly,
            bool ignoreFatDifferences)
            throw (std::exception);
        
    static AkaiFatFileSystem read(std::shared_ptr<BlockDevice> device, bool readOnly);
};
}
