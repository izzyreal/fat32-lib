namespace akaifat::fat {
class LittleEndian {
private:
    LittleEndian(){}
    
public:
    static int getUInt8(byte[] src, int offset) {
        return src[offset] & 0xFF;
    }

    static int getUInt16(byte[] src, int offset) {
        const int v0 = src[offset + 0] & 0xFF;
        const int v1 = src[offset + 1] & 0xFF;
        return ((v1 << 8) | v0);
    }

    static long getUInt32(byte[] src, int offset) {
        const long v0 = src[offset + 0] & 0xFF;
        const long v1 = src[offset + 1] & 0xFF;
        const long v2 = src[offset + 2] & 0xFF;
        const long v3 = src[offset + 3] & 0xFF;
        return ((v3 << 24) | (v2 << 16) | (v1 << 8) | v0);
    }

    static void setInt8(byte[] dst, int offset, int value) {
        assert (value & 0xff) == value : "value out of range";
        
        dst[offset] = (byte) value;
    }

    static void setInt16(byte[] dst, int offset, int value) {
        assert (value & 0xffff) == value : "value out of range";
        
        dst[offset + 0] = (byte) (value & 0xFF);
        dst[offset + 1] = (byte) ((value >>> 8) & 0xFF);
    }
    
    static void setInt32(byte[] dst, int offset, long value)
            throws IllegalArgumentException {
        
        assert value <= Integer.MAX_VALUE : "value out of range";
        
        dst[offset + 0] = (byte) (value & 0xFF);
        dst[offset + 1] = (byte) ((value >>> 8) & 0xFF);
        dst[offset + 2] = (byte) ((value >>> 16) & 0xFF);
        dst[offset + 3] = (byte) ((value >>> 24) & 0xFF);
    }
    
};
}
