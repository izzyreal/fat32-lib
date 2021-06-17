#pragma once

#include "../AbstractFsObject.hpp"
#include "../FsDirectoryEntry.hpp"

#include "FatLfnDirectory.hpp"
#include "FatDirectoryEntry.hpp"

#include <string>

namespace akaifat::fat {
class FatLfnDirectoryEntry
        : public akaifat::AbstractFsObject, public akaifat::FsDirectoryEntry {
private:
    FatLfnDirectory* parent;
    std::string fileName;
    
public:
    FatDirectoryEntry* realEntry;
        
    FatLfnDirectoryEntry(std::string name, ShortName sn,
            FatLfnDirectory* _parent, bool directory)
            : akaifat::AbstractFsObject(false), parent (_parent) {
        fileName = name;
        
//        realEntry = FatDirectoryEntry.create(
//                parent->getFat().getFatType(), directory);
//        realEntry->setShortName(sn);
    }

    FatLfnDirectoryEntry(FatLfnDirectory* _parent,
            FatDirectoryEntry* _realEntry, std::string _fileName)
            : akaifat::AbstractFsObject (_parent->isReadOnly()), parent (_parent), realEntry (_realEntry), fileName (_fileName)
    {
    }
    
    static FatLfnDirectoryEntry* extract(
            FatLfnDirectory* dir, int offset, int len) {
            
        auto realEntry = dir->dir->getEntry(offset + len - 1);
        std::string fileName;
        
        if (len == 1) {
            /* this is just an old plain 8.3 entry */
            fileName = realEntry->getShortName().asSimpleString();
        } else {
            /* stored in reverse order */
//            std::stringBuilder name = new std::stringBuilder(13 * (len - 1));
            
            for (int i = len - 2; i >= 0; i--) {
                auto entry = dir->dir->getEntry(i + offset);
//                name.append(entry.getLfnPart());
            }
            
//            fileName = name.to_string().trim();
        }
        
//        return new FatLfnDirectoryEntry(dir, realEntry, fileName);
        return nullptr;
    }
    
    bool isHiddenFlag() {
        return realEntry->isHiddenFlag();
    }
    
    void setHiddenFlag(bool hidden) {
        checkWritable();
        
        realEntry->setHiddenFlag(hidden);
    }
    
    bool isSystemFlag() {
        return realEntry->isSystemFlag();
    }
    
    void setSystemFlag(bool systemEntry) {
        checkWritable();
        
        realEntry->setSystemFlag(systemEntry);
    }

    bool isReadOnlyFlag() {
        return realEntry->isReadonlyFlag();
    }

    void setReadOnlyFlag(bool readOnly) {
        checkWritable();
        
        realEntry->setReadonlyFlag(readOnly);
    }

    bool isArchiveFlag() {
        return realEntry->isArchiveFlag();
    }

    void setArchiveFlag(bool archive) {
        checkWritable();

        realEntry->setArchiveFlag(archive);
    }
    
//    FatDirectoryEntry[] compactForm() {
//            return new FatDirectoryEntry[]{realEntry};
//    }

    std::string getName() override {
        checkValid();
        
        return fileName;
    }
    
    std::string getAkaiPartString() {
    	if (isDirectory()) return "";
    	return AkaiPart::parse(realEntry->data).asSimpleString();
    }
    
    void setAkaiPartString(std::string& s) {
    	if (isDirectory()) return;
    	AkaiPart ap(s);
    	ap.write(realEntry->data);
    }
    
    FsDirectory* getParent() override {
        checkValid();
        
        return parent;
    }
    
    void setName(std::string newName) override {
        checkWritable();
        
        if (!parent->isFreeName(newName)) {
            throw "the name \"" + newName + "\" is already in use";
        }
        
        parent->unlinkEntry(this);
        fileName = newName;
        parent->linkEntry(this);
    }
    
    void moveTo(FatLfnDirectory* target, std::string& newName) {
        checkWritable();

        if (!target->isFreeName(newName)) {
            throw "the name \"" + newName + "\" is already in use";
        }
        
        parent->unlinkEntry(this);
        parent = target;
        fileName = newName;
        parent->linkEntry(this);
    }
        
    FatFile* getFile() override {
        return parent->getFile(realEntry);
    }
    
    FatLfnDirectory* getDirectory() override {
        return parent->getDirectory(realEntry);
    }
    
    bool isFile() override {
        return realEntry->isFile();
    }

    bool isDirectory() override {
        return realEntry->isDirectory();
    }

    bool isDirty() override {
        return realEntry->isDirty();
    }
    
};
}
