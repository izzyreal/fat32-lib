#pragma once

#include "BlockDevice.hpp"

#include "ByteBuffer.hpp"

#include <exception>

namespace akaifat {
class DummyBlockDevice : public BlockDevice {
public:
    long getSize() { return 0; }

    void read(long devOffset, ByteBuffer dest) {}

    void write(long devOffset, ByteBuffer src) {}
            
    void flush() {}

    int getSectorSize() { return 0; }

    void close() {}

    bool isClosed() { return false; }
    
    bool isReadOnly() { return false; }
    
};
}
