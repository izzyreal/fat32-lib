namespace akaifat::fat {
class Fat {
public:
    const static int FIRST_CLUSTER = 2;
    
    static Fat read(BootSector bs, int fatNr)
            throws IOException, IllegalArgumentException {
        
        if (fatNr > bs.getNrFats()) {
            throw new IllegalArgumentException(
                    "boot sector says there are only " + bs.getNrFats() +
                    " FATs when reading FAT #" + fatNr);
        }
        
        const long fatOffset = bs.getFatOffset(fatNr);
        const Fat result = new Fat(bs, fatOffset);
        result.read();
        return result;
    }
    
    static Fat create(BootSector bs, int fatNr)
            throws IOException, IllegalArgumentException {
        
        if (fatNr > bs.getNrFats()) {
            throw new IllegalArgumentException(
                    "boot sector says there are only " + bs.getNrFats() +
                    " FATs when creating FAT #" + fatNr);
        }
        
        const long fatOffset = bs.getFatOffset(fatNr);
        const Fat result = new Fat(bs, fatOffset);

        if (bs.getDataClusterCount() > result.entries.length)
            throw "FAT too small for device";
            
        result.init(bs.getMediumDescriptor());
        result.write();
        return result;
    }

    FatType getFatType() {
        return fatType;
    }
    
    BootSector getBootSector() {
        return bs;
    }

    BlockDevice getDevice() {
        return device;
    }
   
    void write() throw (std::exception) {
        writeCopy(offset);
    }
    
    void writeCopy(long offset) throw (std::exception) {
        const std::vector<char> data = new byte[sectorCount * sectorSize];
        
        for (int index = 0; index < entries.length; index++) {
            fatType.writeEntry(data, index, entries[index]);
        }
        
        device.write(offset, ByteBuffer.wrap(data));
    }
    
    int getMediumDescriptor() {
        return (int) (entries[0] & 0xFF);
    }
    
    long getEntry(int index) {
        return entries[index];
    }

    int getLastFreeCluster() {
        return lastAllocatedCluster;
    }
    
    long[] getChain(long startCluster) {
        testCluster(startCluster);
        // Count the chain first
        int count = 1;
        long cluster = startCluster;
        while (!isEofCluster(entries[(int) cluster])) {
            count++;
            cluster = entries[(int) cluster];
        }
        // Now create the chain
        long[] chain = new long[count];
        chain[0] = startCluster;
        cluster = startCluster;
        int i = 0;
        while (!isEofCluster(entries[(int) cluster])) {
            cluster = entries[(int) cluster];
            chain[++i] = cluster;
        }
        return chain;
    }

    long getNextCluster(long cluster) {
        testCluster(cluster);
        long entry = entries[(int) cluster];
        if (isEofCluster(entry)) {
            return -1;
        } else {
            return entry;
        }
    }

    long allocNew() throw (std::exception) {

        int i;
        int entryIndex = -1;

        for (i = lastAllocatedCluster; i < lastClusterIndex; i++) {
            if (isFreeCluster(i)) {
                entryIndex = i;
                break;
            }
        }
        
        if (entryIndex < 0) {
            for (i = FIRST_CLUSTER; i < lastAllocatedCluster; i++) {
                if (isFreeCluster(i)) {
                    entryIndex = i;
                    break;
                }
            }
        }
        
        if (entryIndex < 0) {
            throw "FAT Full (" + (lastClusterIndex - FIRST_CLUSTER)
                    + ", " + i + ")";
        }
        
        entries[entryIndex] = fatType.getEofMarker();
        lastAllocatedCluster = entryIndex % lastClusterIndex;
        if (lastAllocatedCluster < FIRST_CLUSTER)
            lastAllocatedCluster = FIRST_CLUSTER;
        
        return entryIndex;
    }
    
    int getFreeClusterCount() {
        int result = 0;

        for (int i=FIRST_CLUSTER; i < lastClusterIndex; i++) {
            if (isFreeCluster(i)) result++;
        }

        return result;
    }

    int getLastAllocatedCluster() {
        return lastAllocatedCluster;
    }
    
    long[] allocNew(int nrClusters) throw (std::exception) {
        const long rc[] = new long[nrClusters];
        
        rc[0] = allocNew();
        for (int i = 1; i < nrClusters; i++) {
            rc[i] = allocAppend(rc[i - 1]);
        }
        
        return rc;
    }
    
    long allocAppend(long cluster)
            throw (std::exception) {
        
        testCluster(cluster);
        
        while (!isEofCluster(entries[(int) cluster])) {
            cluster = entries[(int) cluster];
        }
        
        long newCluster = allocNew();
        entries[(int) cluster] = newCluster;

        return newCluster;
    }

    void setEof(long cluster) {
        testCluster(cluster);
        entries[(int) cluster] = fatType.getEofMarker();
    }

    void setFree(long cluster) {
        testCluster(cluster);
        entries[(int) cluster] = 0;
    }
    
    bool equals(Fat& other) override {
        if (fatType != other.fatType) return false;
        if (sectorCount != other.sectorCount) return false;
        if (sectorSize != other.sectorSize) return false;
        if (lastClusterIndex != other.lastClusterIndex) return false;
        if (!Arrays.equals(entries, other.entries)) return false;
        
        return (getMediumDescriptor() == other.getMediumDescriptor());
    }
    
    int hashCode() override {
        int hash = 7;
        hash = 23 * hash + Arrays.hashCode(entries);
        hash = 23 * hash + fatType.hashCode();
        hash = 23 * hash + sectorCount;
        hash = 23 * hash + sectorSize;
        hash = 23 * hash + lastClusterIndex;
        return hash;
    }

protected:
    bool isFreeCluster(long entry) {
        if (entry > Integer.MAX_VALUE) throw new IllegalArgumentException();
        return (entries[(int) entry] == 0);
    }
    
    bool isReservedCluster(long entry) {
        return fatType.isReservedCluster(entry);
    }

    bool isEofCluster(long entry) {
        return fatType.isEofCluster(entry);
    }
    
    void testCluster(long cluster) throws IllegalArgumentException {
        if ((cluster < FIRST_CLUSTER) || (cluster >= entries.length)) {
            throw new IllegalArgumentException(
                    "invalid cluster value " + cluster);
        }
    }

private:
    const long[] entries;
    const FatType fatType;
    const int sectorCount;
    const int sectorSize;
    const BlockDevice device;
    const BootSector bs;
    const long offset;
    const int lastClusterIndex;
    
    int lastAllocatedCluster;

    Fat(BootSector& _bs, long offset) throw (std::exception) {
        bs = _bs;
        fatType = bs.getFatType();
        if (bs.getSectorsPerFat() > Integer.MAX_VALUE)
            throw "FAT too large";

        if (bs.getSectorsPerFat() <= 0) 
                throw "boot sector says there are " + bs.getSectorsPerFat() +
                " sectors per FAT";

        if (bs.getBytesPerSector() <= 0) 
                throw "boot sector says there are " + bs.getBytesPerSector() +
                " bytes per sector";

        sectorCount = (int) bs.getSectorsPerFat();
        sectorSize = bs.getBytesPerSector();
        device = bs.getDevice();
        offset = offset;
        lastAllocatedCluster = FIRST_CLUSTER;
        
        if (bs.getDataClusterCount() > Integer.MAX_VALUE) throw
                "too many data clusters";
        
        if (bs.getDataClusterCount() == 0) throw
                "no data clusters";
        
        lastClusterIndex = (int) bs.getDataClusterCount() + FIRST_CLUSTER;

        entries = new long[(int) ((sectorCount * sectorSize) /
                fatType.getEntrySize())];
                
        if (lastClusterIndex > entries.length) 
            throw "file system has " + lastClusterIndex +
            "clusters but only " + entries.length + " FAT entries";
    }
    

    void init(int mediumDescriptor) {
        entries[0] = 
                (mediumDescriptor & 0xFF) |
                (0xFFFFF00L & fatType.getBitMask());
        entries[1] = fatType.getEofMarker();
    }
    
    void read() throw (std::exception) {
        const std::vector<char> data = new byte[sectorCount * sectorSize];
        device.read(offset, ByteBuffer.wrap(data));

        for (int i = 0; i < entries.length; i++)
            entries[i] = fatType.readEntry(data, i);
    }
 
};
}
