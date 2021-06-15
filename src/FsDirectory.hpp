#pragma once

#include "FsObject.hpp"

#include "FsDirectoryEntry.hpp"

#include <algorithm>
#include <exception>

namespace akaifat {
class FsDirectory : public FsObject {
public:
//    virtual std::iterator<FsDirectoryEntry> iterator() = 0;

    virtual FsDirectoryEntry getEntry(std::string name) = 0;

    virtual FsDirectoryEntry addFile(std::string name) = 0;

    virtual FsDirectoryEntry addDirectory(std::string name) = 0;

    virtual void remove(std::string name) = 0;

    virtual void flush() = 0;
    
};
}
