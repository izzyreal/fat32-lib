#pragma once

#include "../AbstractFsObject.hpp"
#include "../FsDirectoryEntry.hpp"

#include "ShortName.hpp"
#include "FatFile.hpp"
#include "FatLfnDirectory.hpp"

#include <string>

namespace akaifat::fat {

class FatLfnDirectory;

class FatLfnDirectoryEntry
: public akaifat::AbstractFsObject, public akaifat::FsDirectoryEntry {

private:
    FatLfnDirectory* parent;
    std::string fileName;
    
public:
    FatDirectoryEntry* realEntry;
    
    FatLfnDirectoryEntry(std::string name, ShortName sn,
                         FatLfnDirectory* _parent, bool directory);
    
    FatLfnDirectoryEntry(FatLfnDirectory* _parent,
                         FatDirectoryEntry* _realEntry, std::string _fileName);
    
    static FatLfnDirectoryEntry* extract(FatLfnDirectory* dir, int offset, int len);
    
    bool isHiddenFlag();
    
    void setHiddenFlag(bool hidden);
    
    bool isSystemFlag();
    
    void setSystemFlag(bool systemEntry);
    
    bool isReadOnlyFlag();
    
    void setReadOnlyFlag(bool readOnly);
    
    bool isArchiveFlag();
    
    void setArchiveFlag(bool archive);
    
    std::vector<FatDirectoryEntry*> compactForm();
    
    std::string getName() override;
    
    std::string getAkaiPartString();
    
    void setAkaiPartString(std::string& s);
    
    FsDirectory* getParent() override;
    
    void setName(std::string newName) override;
    
    void moveTo(FatLfnDirectory* target, std::string& newName);
    
    FatFile* getFile() override;
    
    FatLfnDirectory* getDirectory() override;
    
    bool isFile() override;
    
    bool isDirectory() override;
    
    bool isDirty() override;
    
    bool isValid() override;
    
    bool isReadOnly() override;
    
};
}
