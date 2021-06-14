namespace akaifat::fat {
class ClusterChain : public AbstractFsObject {
private:
    const Fat fat;
    const BlockDevice device;
    const int clusterSize;
    const long dataOffset;
    
    long startCluster;
    
    private long getDevOffset(long cluster, int clusterOffset) {
        return dataOffset + clusterOffset +
                ((cluster - Fat.FIRST_CLUSTER) * clusterSize);
    }

public:
    ClusterChain(Fat fat, bool readOnly) {
        this(fat, 0, readOnly);
    }
    
    ClusterChain(Fat fat, long startCluster, bool readOnly) {
        super(readOnly);
        
        fat = fat;
        
        if (startCluster != 0) {
            fat.testCluster(startCluster);
            
            if (fat.isFreeCluster(startCluster))
                throw new IllegalArgumentException(
                    "cluster " + startCluster + " is free");
        }
        
        device = fat.getDevice();
        dataOffset = fat.getBootSector().getFilesOffset();
        startCluster = startCluster;
        clusterSize = fat.getBootSector().getBytesPerCluster();
    }
    
    int getClusterSize() {
        return clusterSize;
    }
    
    Fat getFat() {
        return fat;
    }

    BlockDevice getDevice() {
        return device;
    }

    long getStartCluster() {
        return startCluster;
    }
    
    long getLengthOnDisk() {
        if (getStartCluster() == 0) return 0;
        
        return getChainLength() * clusterSize;
    }
    
    long setSize(long size) throw (std::exception) {
        const long nrClusters = ((size + clusterSize - 1) / clusterSize);
        if (nrClusters > Integer.MAX_VALUE)
            throw new std::exception("too many clusters");

        setChainLength((int) nrClusters);
        
        return clusterSize * nrClusters;
    }

    int getChainLength() {
        if (getStartCluster() == 0) return 0;
        
        const long[] chain = getFat().getChain(getStartCluster());
        return chain.length;
    }

    void setChainLength(int nrClusters) throw (std::exception) {
        if (nrClusters < 0) throw new IllegalArgumentException(
                "negative cluster count"); //NOI18N
                
        if ((startCluster == 0) && (nrClusters == 0)) {
            /* nothing to do */
        } else if ((startCluster == 0) && (nrClusters > 0)) {
            const long[] chain = fat.allocNew(nrClusters);
            startCluster = chain[0];
        } else {
            const long[] chain = fat.getChain(startCluster);
            
            if (nrClusters != chain.length) {
                if (nrClusters > chain.length) {
                    /* grow the chain */
                    int count = nrClusters - chain.length;
                    
                    while (count > 0) {
                        fat.allocAppend(getStartCluster());
                        count--;
                    }
                } else {
                    /* shrink the chain */
                    if (nrClusters > 0) {
                        fat.setEof(chain[nrClusters - 1]);
                        for (int i = nrClusters; i < chain.length; i++) {
                            fat.setFree(chain[i]);
                        }
                    } else {
                        for (int i=0; i < chain.length; i++) {
                            fat.setFree(chain[i]);
                        }
                        
                        startCluster = 0;
                    }
                }
            }
        }
    }
    
    void readData(long offset, ByteBuffer dest)
            throw (std::exception) {

        int len = dest.remaining();

        if ((startCluster == 0 && len > 0)) {
            throw new EOFException("cannot read from empty cluster chain");
        }
        
        const long[] chain = getFat().getChain(startCluster);
        const BlockDevice dev = getDevice();

        int chainIdx = (int) (offset / clusterSize);
        
        if (offset % clusterSize != 0) {
            int clusOfs = (int) (offset % clusterSize);
            int size = Math.min(len,
                    (int) (clusterSize - (offset % clusterSize)));
            dest.limit(dest.position() + size);

            dev.read(getDevOffset(chain[chainIdx], clusOfs), dest);
            
            len -= size;
            chainIdx++;
        }

        while (len > 0) {
            int size = Math.min(clusterSize, len);
            dest.limit(dest.position() + size);

            dev.read(getDevOffset(chain[chainIdx], 0), dest);

            len -= size;
            chainIdx++;
        }
    }
    
    void writeData(long offset, ByteBuffer srcBuf) throw (std::exception) {
        
        int len = srcBuf.remaining();

        if (len == 0) return;

        const long minSize = offset + len;
        if (getLengthOnDisk() < minSize) {
            setSize(minSize);
        }
        
        const long[] chain = fat.getChain(getStartCluster());

        int chainIdx = (int) (offset / clusterSize);
        
        if (offset % clusterSize != 0) {
            int clusOfs = (int) (offset % clusterSize);
            int size = Math.min(len,
                    (int) (clusterSize - (offset % clusterSize)));
            srcBuf.limit(srcBuf.position() + size);
            
            device.write(getDevOffset(chain[chainIdx], clusOfs), srcBuf);
            
            len -= size;
            chainIdx++;
        }
        
        while (len > 0) {
            int size = Math.min(clusterSize, len);
            srcBuf.limit(srcBuf.position() + size);

            device.write(getDevOffset(chain[chainIdx], 0), srcBuf);

            len -= size;
            chainIdx++;
        }
        
    }

    bool equals(ClusterChain* obj) override {
        if (obj == null) {
            return false;
        }
        
        if (!(obj instanceof ClusterChain)){
            return false;
        }
        
        const ClusterChain other = (ClusterChain) obj;
        
        if ((fat != other.fat) &&
            (fat == null || !fat.equals(other.fat))) {

            return false;
        }
        
        return (startCluster == other.startCluster);
    }

    int hashCode() override {
        int hash = 3;
        hash = 79 * hash +
                (fat != null ? fat.hashCode() : 0);
        hash = 79 * hash +
                (int) (startCluster ^ (startCluster >>> 32));
        return hash;
    }
    
};
}
