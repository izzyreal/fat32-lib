#pragma once
#include "FsDirectory.hpp"

#include <exception>

namespace akaifat {
class FileSystem {
public:
    virtual FsDirectory getRoot() = 0;

    virtual const bool isReadOnly() = 0;

    virtual void close() = 0;
    
    virtual const bool isClosed() = 0;

    virtual long getTotalSpace() = 0;

    virtual long getFreeSpace() = 0;

    virtual long getUsableSpace() = 0;

    virtual void flush() = 0;
};
}
