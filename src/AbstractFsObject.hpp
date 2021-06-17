#pragma once

namespace akaifat {
class AbstractFsObject  {
private:
    bool readOnly;
    bool valid;
    
public:
    bool isValid() {
        return valid;
    }

    bool isReadOnly() {
        return readOnly;
    }
    
protected:
    AbstractFsObject(bool _readOnly)
    : readOnly (_readOnly){
        valid = true;
    }
    
    virtual ~AbstractFsObject(){}

    void invalidate() {
        valid = false;
    }

    void checkValid() {
        if (!isValid()) throw "file system is not valid";
    }

    void checkWritable() {
        checkValid();

        if (isReadOnly()) {
            throw "file system is read only";
        }
    }
};
}
