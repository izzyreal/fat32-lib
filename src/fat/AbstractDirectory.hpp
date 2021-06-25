#pragma once

#include "FatType.hpp"
#include "Fat.hpp"

#include <vector>
#include <string>

using namespace akaifat;

namespace akaifat::fat {

class FatDirectoryEntry;

class AbstractDirectory {
public:
    virtual ~AbstractDirectory(){}
    static const int MAX_LABEL_LENGTH = 11;
    
    void setEntries(std::vector<FatDirectoryEntry*>& newEntries);
    
    FatDirectoryEntry* getEntry(int idx);
    
    int getCapacity() const;

    int getEntryCount();
    
    bool isDirReadOnly();

    bool isRoot() const;

    int getSize();

    void flush();

    void addEntry(FatDirectoryEntry*);
    
    void addEntries(std::vector<FatDirectoryEntry*>&);
    void removeEntry(FatDirectoryEntry*);

    std::string& getLabel();

    FatDirectoryEntry* createSub(Fat* fat);
    
    void setLabel(std::string& label);

    std::vector<FatDirectoryEntry*>& getEntries();

    virtual void changeSize(int entryCount) = 0;

private:
    std::vector<FatDirectoryEntry*> entries;
    bool readOnly;
    bool _isRoot;
    FatType* type;

    int capacity;
    std::string volumeLabel;

    void checkRoot() const;

protected:
    AbstractDirectory(
        FatType* type,
        int capacity,
        bool readOnly,
        bool isRoot
    );
        
    virtual void read(ByteBuffer& data) = 0;

    virtual void write(ByteBuffer& data) = 0;

    virtual long getStorageCluster() = 0;

    virtual void sizeChanged(long newSize);
            
    virtual void read();

};
}
