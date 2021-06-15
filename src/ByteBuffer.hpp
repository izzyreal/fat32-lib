#pragma once

#include <vector>

namespace akaifat {

class ByteBuffer {
private:
    std::vector<char> buf;
    
public:
    ByteBuffer(long size)
    : buf (std::vector<char>(size))
    {
    }
    
    void get(std::vector<char>& data) {}
    short getShort() { return 0; }
    long position() { return 0; }
    long remaining() { return 0; }
    void put(char c) { };
};

}
