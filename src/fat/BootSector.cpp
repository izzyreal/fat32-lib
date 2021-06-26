#include "BootSector.hpp"

#include "Fat16BootSector.hpp"

using namespace akaifat;
using namespace akaifat::fat;

BootSector* BootSector::read(BlockDevice* device) {
    ByteBuffer bb(512);
    //        bb.order(ByteOrder.LITTLE_ENDIAN);
    device->read(0, bb);
    
    if ((bb.get(510) & 0xff) != 0x55 ||
        (bb.get(511) & 0xff) != 0xaa)
        throw std::runtime_error("missing boot sector signature");
    
    int sectorsPerCluster = bb.get(SECTORS_PER_CLUSTER_OFFSET);
    
    if (sectorsPerCluster <= 0)
        throw std::runtime_error("suspicious sectors per cluster count " + std::to_string(sectorsPerCluster));
                
    auto result = new Fat16BootSector(device);
    result->read_();
    return result;
}
