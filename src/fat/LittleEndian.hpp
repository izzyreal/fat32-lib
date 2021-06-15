#pragma once

#include <vector>

namespace akaifat::fat {
class LittleEndian {
private:
    LittleEndian(){}
    
public:
    static int getUInt8(std::vector<char>& src, int offset) {
        return src[offset] & 0xFF;
    }

    static int getUInt16(std::vector<char>& src, int offset) {
        const int v0 = src[offset + 0] & 0xFF;
        const int v1 = src[offset + 1] & 0xFF;
        return ((v1 << 8) | v0);
    }

    static long getUInt32(std::vector<char>& src, int offset) {
        const long v0 = src[offset + 0] & 0xFF;
        const long v1 = src[offset + 1] & 0xFF;
        const long v2 = src[offset + 2] & 0xFF;
        const long v3 = src[offset + 3] & 0xFF;
        return ((v3 << 24) | (v2 << 16) | (v1 << 8) | v0);
    }

    static void setInt8(std::vector<char>& dst, int offset, int value) {
        if ( (value & 0xff) != value) throw "value out of range";
        
        dst[offset] = (char) value;
    }

    static void setInt16(std::vector<char>& dst, int offset, int value) {
        if ( (value & 0xffff) != value) throw "value out of range";
        
        dst[offset + 0] = (char) (value & 0xFF);
        dst[offset + 1] = (char) ((value >> 8) & 0xFF);
    }
    
    static void setInt32(std::vector<char>& dst, int offset, long value){
        
        if (value > INT_MAX) throw "value out of range";
        
        dst[offset + 0] = (char) (value & 0xFF);
        dst[offset + 1] = (char) ((value >> 8) & 0xFF);
        dst[offset + 2] = (char) ((value >> 16) & 0xFF);
        dst[offset + 3] = (char) ((value >> 24) & 0xFF);
    }
    
};
}
