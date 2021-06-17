#pragma once

#include "../AbstractFsObject.hpp"
#include "../FsFile.hpp"

#include "Fat.hpp"
#include "ClusterChain.hpp"
#include "FatDirectoryEntry.hpp"

#include <exception>

namespace akaifat::fat {
class FatFile : public akaifat::AbstractFsObject, public akaifat::FsFile {
private:
    FatDirectoryEntry* entry;
    ClusterChain chain;
    
    FatFile(FatDirectoryEntry* myEntry, ClusterChain _chain)
    : akaifat::AbstractFsObject(myEntry->isReadOnly()), entry (myEntry), chain (_chain)
    {
    }
    
public:
    static FatFile* get(Fat* fat, FatDirectoryEntry* entry) {
        
        if (entry->isDirectory())
            throw entry->getShortName().asSimpleString() + " is a directory";
            
//        ClusterChain cc = new ClusterChain(
//                fat, entry.getStartCluster(), entry.isReadonlyFlag());
//
//        if (entry.getLength() > cc.getLengthOnDisk())
//                throw "entry (" + std::to_string(entry.getLength()) +
//                ") is larger than associated cluster chain ("
//                + std::to_string(cc.getLengthOnDisk()) + ")";
//
//        return new FatFile(entry, cc);
        return nullptr;
    }
    
     long getLength() override {
        checkValid();
        
        return entry->getLength();
    }
    
     void setLength(long length) override {
        checkWritable();
        
        if (getLength() == length) return;
        
        chain.setSize(length);
        
        entry->setStartCluster(chain.getStartCluster());
        entry->setLength(length);
    }
    
    void read(long offset, ByteBuffer& dest) override {
        checkValid();
        
        auto len = dest.remaining();
        
        if (len == 0) return;
        
        if (offset + len > getLength()) {
            throw "EOF";
        }
        
        chain.readData(offset, dest);
    }

    void write(long offset, ByteBuffer& srcBuf) override {

        checkWritable();
        
        long lastByte = offset + srcBuf.remaining();

        if (lastByte > getLength()) {
            setLength(lastByte);
        }
        
        chain.writeData(offset, srcBuf);
    }
    
    void flush() override {
        checkWritable();
    }
    
    ClusterChain getChain() {
        checkValid();
        
        return chain;
    }
};
}
