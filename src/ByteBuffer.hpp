#pragma once

#include <vector>

namespace akaifat {
class ByteBuffer {
private:
    std::vector<char> buf;
    long pos = 0;
    
public:
    ByteBuffer(long size)
    : buf (std::vector<char>(size)) {}
    ByteBuffer(std::vector<char>& data) : buf (data) {}

    void get(std::vector<char>& dest) {
        for (int i = 0; i < dest.size(); i++)
        {
            if (i >= buf.size()) throw "invalid bytebuffer read";
            dest[i] = get();
        }
        pos += dest.size();
    }
    
    char get() { return buf[pos++]; }
    
    short getShort() {
        short result = (buf[pos + 1] << 8) | (buf[pos] & 0xff);
        pos += 2;
        return result;
    }
    
    long position() { return pos; }
    long remaining() { return buf.size() - pos; }
    
    void put(char c) { buf[pos++] = c; }
    void put(std::vector<char>& data) {
        for (int i = 0; i < data.size(); i++) {
            if (i >= buf.size()) throw "invalid bytebuffer write";
            buf[i] = data[i];
        }
        pos += data.size();
    }
    
    std::vector<char>& getBuffer() { return buf; }
    long getCapacity() { return buf.size(); }
    
};
}
