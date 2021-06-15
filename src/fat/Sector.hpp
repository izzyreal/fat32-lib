namespace akaifat::fat {
class Sector {
private:
    const BlockDevice device;
    const long offset;
    
    bool dirty;
    
protected:
    const ByteBuffer buffer;

    Sector(BlockDevice device, long offset, int size)
    : buffer (ByteBuffer.allocate(size))
    {
        offset = offset;
        device = device;
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        dirty = true;
    }
    
    void read() throw (std::exception) {
        buffer.rewind();
        buffer.limit(buffer.capacity());
        device.read(offset, buffer);
        dirty = false;
    }
    
    const void markDirty() {
        dirty = true;
    }

    int get16(int offset) {
        return buffer.getShort(offset) & 0xffff;
    }

    long get32(int offset) {
        return buffer.getInt(offset);
    }
    
    int get8(int offset) {
        return buffer.get(offset) & 0xff;
    }
    
    void set16(int offset, int value) {
        buffer.putShort(offset, (short) (value & 0xffff));
        dirty = true;
    }

    void set32(int offset, long value) {
        buffer.putInt(offset, (int) (value & 0xffffffff));
        dirty = true;
    }

    void set8(int offset, int value) {
        if ((value & 0xff) != value) {
            throw new IllegalArgumentException(
                    value + " too big to be stored in a single octet");
        }
        
        buffer.put(offset, (byte) (value & 0xff));
        dirty = true;
    }
    
    long getOffset() {
        return offset;
    }

public:
    const bool isDirty() {
        return dirty;
    }
    
    BlockDevice getDevice() {
        return device;
    }

    const void write() throw (std::exception) {
        if (!isDirty()) return;
        
        buffer.position(0);
        buffer.limit(buffer.capacity());
        device.write(offset, buffer);
        dirty = false;
    }
};
}
