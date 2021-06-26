#pragma once

#include <exception>
#include <string>

namespace akaifat {

class FsDirectory;
class FsFile;

class FsDirectoryEntry {
public:
    virtual std::string getName() = 0;

    virtual FsDirectory* getParent() = 0;

    virtual bool isFile() = 0;

    virtual bool isDirectory() = 0;

    virtual void setName(std::string newName) = 0;

    virtual FsFile* getFile() = 0;

    virtual FsDirectory* getDirectory() = 0;

    virtual bool isDirty() = 0;
    
    virtual bool isValid() = 0;
    
    virtual bool isReadOnly() = 0;
};
}
