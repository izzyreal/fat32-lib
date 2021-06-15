#include "fat/FatFileSystem.hpp"
#include "fat/AkaiFatFileSystem.hpp"

namespace akaifat {
class FileSystemFactory {
private:
    FileSystemFactory() { }
    
public:
    static FileSystem create(BlockDevice device, bool readOnly)
            throw (std::exception) {
            
        return FatFileSystem.read(device, readOnly);
    }

    static FileSystem createAkai(BlockDevice device, bool readOnly)
            throw (std::exception) {
            
        return AkaiFatFileSystem::read(device, readOnly);
    }
    
};
}
