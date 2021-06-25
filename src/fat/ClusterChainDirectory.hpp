#pragma once

#include "AbstractDirectory.hpp"
#include "ClusterChain.hpp"

namespace akaifat::fat {
class ClusterChainDirectory : public AbstractDirectory {
protected:


    void read(ByteBuffer& data) override {
        chain.readData(0, data);
    }

    void write(ByteBuffer& data) override {
        auto toWrite = data.remaining();
        chain.writeData(0, data);
        long trueSize = chain.getLengthOnDisk();
        
        if (trueSize > toWrite) {
            int rest = (int) (trueSize - toWrite);
            ByteBuffer fill(rest);
            chain.writeData(toWrite, fill);
        }
    }

    void changeSize(int entryCount) override {

        assert (entryCount >= 0);

//        int size = entryCount * FatDirectoryEntry.SIZE;
        int size = entryCount * 32;
        
        if (size > MAX_SIZE) throw "directory would grow beyond " + std::to_string(MAX_SIZE) + " bytes";
        
//        sizeChanged(chain.setSize(Math.max(size, chain.getClusterSize())));
    }
    
public:
    static const int MAX_SIZE = 65536 * 32;

    ClusterChain& chain;
    
    static ClusterChainDirectory readRoot(
            ClusterChain& chain) {
        
        ClusterChainDirectory result(chain, true);
        
//        result.read();
        return result;
    }
        
    void delete_() {
        chain.setChainLength(0);
    }

    ClusterChainDirectory(ClusterChain& _chain, bool isRoot)
    : AbstractDirectory(
                chain.getFat()->getFatType(),
//                (int)(chain.getLengthOnDisk() / FatDirectoryEntry::SIZE),
                (int)(_chain.getLengthOnDisk() / 32),
                _chain.isReadOnly(), isRoot), chain (_chain)
    {
    }

    long getStorageCluster() override {
        return isRoot() ? 0 : chain.getStartCluster();
    }
};
}
