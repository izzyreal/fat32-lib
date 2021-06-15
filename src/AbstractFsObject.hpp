#pragma once

#include "FsObject.hpp"

namespace akaifat {
class AbstractFsObject : public FsObject {
private:
    const bool readOnly;
    bool valid;
    
public:
    bool isValid() override {
        return valid;
    }

    bool isReadOnly() override {
        return readOnly;
    }
    
protected:
    AbstractFsObject(bool _readOnly)
    : readOnly (_readOnly){
        valid = true;
    }

    const void invalidate() {
        valid = false;
    }

    const void checkValid() {
        if (!isValid()) throw "file system is not valid";
    }

    const void checkWritable() {
        checkValid();

        if (isReadOnly()) {
            throw "file system is read only";
        }
    }
};
}
