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
        
        auto toReadTotal = dest.remaining();

        if ((devOffset + toReadTotal) > getSize())
            throw std::runtime_error("reading past end of device");
        
        img.seekg(devOffset);
        std::vector<char>& buf = dest.getBuffer();

        auto toRead = dest.limit() - dest.position();
        img.read(&buf[0] + dest.position(), toRead);
        dest.position(dest.position() + toRead);
    }

    void write(long devOffset, ByteBuffer& src) override {
        if (isClosed()) throw std::runtime_error("device closed");

        auto toWriteTotal = src.remaining();
        if ((devOffset + toWriteTotal) > getSize()) throw std::runtime_error("writing past end of device");

        img.seekp(devOffset);
        std::vector<char>& buf = src.getBuffer();
        auto toWrite = src.limit() - src.position();
        img.write(&buf[0] + src.position(), toWrite);
        src.position(src.position() + toWrite);
    }
            
    void flush() {
    }

    int getSectorSize() {
        return 512;
    }

    void close() {}
    
    bool isReadOnly() { return false; }
    
};
}
