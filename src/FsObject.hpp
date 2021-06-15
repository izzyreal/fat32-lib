#pragma once

namespace akaifat {
class FsObject {
public:
    virtual bool isValid() = 0;
    
    virtual bool isReadOnly() = 0;
};
}
