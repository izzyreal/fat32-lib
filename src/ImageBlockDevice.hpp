#pragma once

#include "BlockDevice.hpp"

#include "ByteBuffer.hpp"

#include <exception>
#include <fstream>

namespace akaifat {
class ImageBlockDevice : public BlockDevice {
private:
    std::ifstream& img;
    
public:
    ImageBlockDevice(std::ifstream& _img) : img (_img) {}
    
    bool isClosed() { return false; }

    long getSize() { return 0; }

    void read(long devOffset, ByteBuffer& dest) {
        if (isClosed()) throw "device closed";
        
        auto toRead = dest.remaining();
        if ((devOffset + toRead) > getSize()) throw "reading past end of device";
        
        img.seekg(devOffset);
        std::vector<char>& buf = dest.getBuffer();
        img.read(&buf[0], dest.getCapacity());
    }

    void write(long devOffset, ByteBuffer& src) {
        
    }
            
    void flush() {}

    int getSectorSize() {
        return 512;
    }

    void close() {}
    
    bool isReadOnly() { return false; }
    
};
}
