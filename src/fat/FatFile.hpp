#include "../AbstractFsObject.hpp"
#include "../FsFile.hpp"

namespace akaifat::fat {
class FatFile : public akaifat::AbstractFsObject, public akaifat::FsFile {
private:
    const FatDirectoryEntry entry;
    const ClusterChain chain;
    
    FatFile(FatDirectoryEntry myEntry, ClusterChain _chain)
    : akaifat::AbstractFsObject(myEntry.isReadOnly()), entry (myEntry), chain (_chain)
    {
    }
    
public:
    static FatFile get(Fat fat, FatDirectoryEntry entry)
            throw (std::exception) {
        
        if (entry.isDirectory())
            throw new IllegalArgumentException(entry + " is a directory");
            
        const ClusterChain cc = new ClusterChain(
                fat, entry.getStartCluster(), entry.isReadonlyFlag());
                
        if (entry.getLength() > cc.getLengthOnDisk()) throw new std::exception(
                "entry (" + entry.getLength() +
                ") is larger than associated cluster chain ("
                + cc.getLengthOnDisk() + ")");
                
        return new FatFile(entry, cc);
    }
    
     long getLength() override {
        checkValid();
        
        return entry.getLength();
    }
    
     void setLength(long length) throw (ReadOnlyException, IOException) override {
        checkWritable();
        
        if (getLength() == length) return;
        
        chain.setSize(length);
        
        entry.setStartCluster(chain.getStartCluster());
        entry.setLength(length);
    }
    
    void read(long offset, ByteBuffer dest) throw (std::exception) override {
        checkValid();
        
        const int len = dest.remaining();
        
        if (len == 0) return;
        
        if (offset + len > getLength()) {
            throw new EOFException();
        }
        
        chain.readData(offset, dest);
    }

    void write(long offset, ByteBuffer srcBuf)
            throw (ReadOnlyException, std::exception) override {

        checkWritable();
        
        const long lastByte = offset + srcBuf.remaining();

        if (lastByte > getLength()) {
            setLength(lastByte);
        }
        
        chain.writeData(offset, srcBuf);
    }
    
    /**
     * Has no effect besides possibly throwing an {@code ReadOnlyException}. To
     * make sure that all data is written out to disk use the
     * {@link FatFileSystem#flush()} method.
     *
     * @throws ReadOnlyException if this {@code FatFile} is read-only
     */
    void flush() throw (std::exception) override {
        checkWritable();
    }
    
    ClusterChain getChain() {
        checkValid();
        
        return chain;
    }
    
    std::string tostd::string() override {
        return getClass().getSimpleName() + " [length=" + getLength() +
                ", first cluster=" + chain.getStartCluster() + "]";
    }
    
};
}
