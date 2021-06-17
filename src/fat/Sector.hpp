#pragma once

#include "../ByteBuffer.hpp"

namespace akaifat::fat {
class Sector {
private:
    BlockDevice* device;
    long offset;
    
    bool dirty;
    
protected:
    ByteBuffer buffer;

    Sector(BlockDevice* _device, long _offset, int size)
    : device (_device), offset (_offset), buffer (ByteBuffer(size)), dirty (true)
    {
//        buffer.order(ByteOrder.LITTLE_ENDIAN);
    }
    
    void read() {
//        buffer.rewind();
//        buffer.limit(buffer.capacity());
//        device.read(offset, buffer);
//        dirty = false;
    }
    
    void markDirty() {
        dirty = true;
    }

    int get16(int offset) {
//        return buffer.getShort(offset) & 0xffff;
        return 0;
    }

    long get32(int offset) {
//        return buffer.getInt(offset);
        return 0;
    }
    
    int get8(int offset) {
//        return buffer.get(offset) & 0xff;
        return 0;
    }
    
    void set16(int offset, int value) {
//        buffer.putShort(offset, (short) (value & 0xffff));
        dirty = true;
    }

    void set32(int offset, long value) {
//        buffer.putInt(offset, (int) (value & 0xffffffff));
        dirty = true;
    }

    void set8(int offset, int value) {
        if ((value & 0xff) != value) {
            throw std::to_string(value) + " too big to be stored in a single octet";
        }
        
//        buffer.put(offset, (char) (value & 0xff));
        dirty = true;
    }
    
    long getOffset() {
        return offset;
    }

public:
    bool isDirty() {
        return dirty;
    }
    
    BlockDevice* getDevice() {
        return device;
    }

    void write() {
        if (!isDirty()) return;
        
//        buffer.position(0);
//        buffer.limit(buffer.capacity());
//        device.write(offset, buffer);
        dirty = false;
    }
};
}
