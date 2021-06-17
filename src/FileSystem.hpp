#pragma once
#include <exception>

namespace akaifat {

class FsDirectory;

class FileSystem {
public:
    virtual FsDirectory* getRoot() = 0;

    virtual bool isReadOnly() = 0;

    virtual void close() = 0;
    
    virtual bool isClosed() = 0;

    virtual long getTotalSpace() = 0;

    virtual long getFreeSpace() = 0;

    virtual long getUsableSpace() = 0;

    virtual void flush() = 0;
};
}
