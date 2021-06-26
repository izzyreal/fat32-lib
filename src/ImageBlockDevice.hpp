#pragma once

#include "BlockDevice.hpp"

#include "ByteBuffer.hpp"

#include <exception>
#include <fstream>

namespace akaifat {
class ImageBlockDevice : public BlockDevice {
private:
    std::fstream& img;
    
public:
    ImageBlockDevice(std::fstream& _img) : img (_img) {}
    
    bool isClosed() { return false; }

    long getSize() {
        const auto begin = img.tellg();
        img.seekg (0, std::ios::end);
        const auto end = img.tellg();
        const auto fsize = (end-begin);
        return fsize;
    }

    void read(long devOffset, ByteBuffer& dest) {
        if (isClosed()) throw std::runtime_error("device closed");
        
        auto toRead = dest.remaining();
        if ((devOffset + toRead) > getSize()) throw std::runtime_error("reading past end of device");
        
        img.seekg(devOffset);
        std::vector<char>& buf = dest.getBuffer();
        img.read(&buf[0], dest.capacity());
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
