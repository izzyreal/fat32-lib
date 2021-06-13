#include <vector>
#include <string>

namespace akaifat::fat {

class FatDirectoryEntry;
class FatType;
class DirectoryFullException;

class AbstractDirectory {

public:
    static const int MAX_LABEL_LENGTH = 11;
    
    void setEntries(std::vector<FatDirectoryEntry>& newEntries);
    
    const FatDirectoryEntry getEntry(int idx);
    
    const int getCapacity();

    const int getEntryCount();
    
    bool isReadOnly();

    const bool isRoot();

    int getSize();

    void flush() throw (std::exception);

    void addEntry(FatDirectoryEntry e) throw (std::exception);
    
    void addEntries(FatDirectoryEntry[] entries)
            throw (std::exception);
    void removeEntry(FatDirectoryEntry entry) throw (std::exception);

    std::string& getLabel() throw (std::exception);

    FatDirectoryEntry createSub(Fat fat) throw (std::exception);
    
    void setLabel(std::string& label) throw (std::illegal_argument,
            UnsupportedOperationException, std::exception);

private:
    const std::vector<FatDirectoryEntry> entries;
    const bool readOnly;
    const bool isRoot;
    const FatType type;

    int capacity;
    std::string volumeLabel;

    void checkRoot() throw (std::exception);

protected:
    AbstractDirectory(
        FatType type,
        int capacity,
        bool readOnly,
        bool isRoot
    );

    
    virtual void read(ByteBuffer data) throw (std::exception) = 0;

    virtual void write(ByteBuffer data) throw (std::exception) = 0;

    virtual long getStorageCluster() = 0;

    virtual void changeSize(const int entryCount)
            throw (DirectoryFullException, std::exception) = 0;
            
    void sizeChanged(const long newSize) throw (std::exception);
            
    void read() throw (std::exception);

};
}