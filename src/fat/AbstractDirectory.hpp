#pragma once

#include "FatDirectoryEntry.hpp"
#include "FatType.hpp"
#include "Fat.hpp"

#include <vector>
#include <string>

namespace akaifat::fat {

public:
    static const int MAX_LABEL_LENGTH = 11;
    
    void setEntries(std::vector<FatDirectoryEntry>& newEntries);
    
    const FatDirectoryEntry getEntry(int idx);
    
    const int getCapacity();

    const int getEntryCount();
    
    bool isReadOnly();

    const bool isRoot();

    int getSize();

    void flush();

    void addEntry(FatDirectoryEntry e);
    
    void addEntries(std::vector<FatDirectoryEntry> entries);
    void removeEntry(FatDirectoryEntry entry);

    std::string& getLabel();

    FatDirectoryEntry createSub(Fat fat);
    
    void setLabel(std::string& label);

private:
    const std::vector<FatDirectoryEntry> entries;
    const bool readOnly;
    const bool isRoot;
    const FatType type;

    int capacity;
    std::string volumeLabel;

    void checkRoot();

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
