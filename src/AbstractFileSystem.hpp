#pragma once

#include "FileSystem.hpp"

namespace akaifat {
class AbstractFileSystem : public FileSystem {
private:
    const bool readOnly;
    bool closed;
    
public:
    AbstractFileSystem(bool _readOnly)
    : readOnly (_readOnly){
        closed = false;
    }
    
    void close() override {
        if (!isClosed()) {
            if (!isReadOnly()) {
                flush();
            }
            
            closed = true;
        }
    }
    
    const bool isClosed() override {
        return closed;
    }
    
    const bool isReadOnly() override {
        return readOnly;
    }

protected:
    const void checkClosed() {
        if (isClosed()) {
            throw "file system was already closed";
        }
    }
    
    const void checkReadOnly() {
        if (isReadOnly()) {
            throw "file system is read only";
        }
    }
};
}
