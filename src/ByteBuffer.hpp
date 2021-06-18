#pragma once

#include <vector>

namespace akaifat {
class ByteBuffer {
private:
    std::vector<char> buf;
    long pos = 0;
    long limit_ = 0;
    
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
    char get(int index) { return buf[index]; }
    
    short getShort() {
        short result = getShort(pos);
        pos += 2;
        return result;
    }
    
    short getShort(long index) {
        short result = (buf[index] & 0xff) | (buf[index + 1] << 8);
        return result;
    }
    
    long position() { return pos; }
    long remaining() { return buf.size() - pos; }
    void rewind() { pos = 0; }
    void limit(long newLimit) { limit_ = 0; }
    
    void put(char c) { buf[pos++] = c; }
    void put(std::vector<char>& data) {
        for (int i = 0; i < data.size(); i++) {
            if (i >= buf.size()) throw "invalid bytebuffer write";
            buf[i] = data[i];
        }
        pos += data.size();
    }
    
    std::vector<char>& getBuffer() { return buf; }
    long capacity() { return buf.size(); }
    
};
}
