#include "AbstractDirectory.hpp"

#include "DirectoryFullException.hpp"

namespace akaifat::fat {
class ClusterChainDirectory : public AbstractDirectory {
protected:
    ClusterChainDirectory(ClusterChain& _chain, bool isRoot)
    : AbstractDirectory(
                chain.getFat().getFatType(),
                (int)(chain.getLengthOnDisk() / FatDirectoryEntry.SIZE),
                chain.isReadOnly(), isRoot), chain (_chain)
    {
    }


    
    const void read(ByteBuffer data) throw (std::exception) override {
        chain.readData(0, data);
    }

    const void write(ByteBuffer data) throw (std::exception) override {
        const int toWrite = data.remaining();
        chain.writeData(0, data);
        const long trueSize = chain.getLengthOnDisk();
        
        if (trueSize > toWrite) {
            const int rest = (int) (trueSize - toWrite);
            const ByteBuffer fill = ByteBuffer.allocate(rest);
            chain.writeData(toWrite, fill);
        }
    }

    const long getStorageCluster() override {
        return isRoot() ? 0 : chain.getStartCluster();
    }
    
    const void changeSize(int entryCount)
            throw (std::exception, std::illegal_argument) override {

        assert (entryCount >= 0);

        const int size = entryCount * FatDirectoryEntry.SIZE;

        if (size > MAX_SIZE) throw new DirectoryFullException(
                "directory would grow beyond " + MAX_SIZE + " bytes",
                getCapacity(), entryCount);
        
        sizeChanged(chain.setSize(Math.max(size, chain.getClusterSize())));
    }
    
public:
    const static int MAX_SIZE = 65536 * 32;

    const ClusterChain& chain;
    
    static ClusterChainDirectory readRoot(
            ClusterChain chain) throw (std::exception) {
        
        const ClusterChainDirectory result =
                new ClusterChainDirectory(chain, true);
        
        result.read();
        return result;
    }
    
    static ClusterChainDirectory createRoot(Fat& fat) throw (std::exception) {

        if (fat.getFatType() != FatType.FAT32) {
            throw new IllegalArgumentException(
                    "only FAT32 stores root directory in a cluster chain");
        }

        const Fat32BootSector bs = (Fat32BootSector) fat.getBootSector();
        const ClusterChain cc = new ClusterChain(fat, false);
        cc.setChainLength(1);
        
        bs.setRootDirFirstCluster(cc.getStartCluster());
        
        const ClusterChainDirectory result =
                new ClusterChainDirectory(cc, true);
        
        result.flush();
        return result;
    }
    
    const void delete() throw (std::exception) {
        chain.setChainLength(0);
    }
};
}
