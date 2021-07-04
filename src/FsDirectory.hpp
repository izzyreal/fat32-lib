#pragma once

#include "FsDirectoryEntry.hpp"

#include <memory>
#include <algorithm>
#include <exception>
#include <map>
#include <iterator>
#include <string>

namespace akaifat {
class FsDirectory {
public:
    virtual FsDirectoryEntry * getEntry(std::string& name) = 0;

    virtual FsDirectoryEntry * addFile(std::string& name) = 0;

    virtual FsDirectoryEntry * addDirectory(std::string& name) = 0;

    virtual void remove(std::string& name) = 0;

    virtual void flush() = 0;

    virtual bool isDirValid() = 0;
    
    virtual bool isDirReadOnly() = 0;
    
};
}
