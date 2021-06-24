#include "FatLfnDirectoryEntry.hpp"

#include "FatDirectoryEntry.hpp"

using namespace akaifat::fat;

FatLfnDirectoryEntry::FatLfnDirectoryEntry(std::string name, ShortName sn,
                                           FatLfnDirectory* _parent, bool directory)
: akaifat::AbstractFsObject(false), parent (_parent) {
    fileName = name;
    
    realEntry = FatDirectoryEntry::create(
                                          parent->getFat()->getFatType(), directory);
    realEntry->setShortName(sn);
}

FatLfnDirectoryEntry::FatLfnDirectoryEntry(FatLfnDirectory* _parent,
                                           FatDirectoryEntry* _realEntry, std::string _fileName)
: akaifat::AbstractFsObject (_parent->isReadOnly()), parent (_parent), realEntry (_realEntry), fileName (_fileName)
{
}

FatLfnDirectoryEntry* FatLfnDirectoryEntry::extract(
                                                    FatLfnDirectory* dir, int offset, int len) {
    
    auto realEntry = dir->dir->getEntry(offset + len - 1);
    auto fileName = realEntry->getShortName().asSimpleString();
    return new FatLfnDirectoryEntry(dir, realEntry, fileName);
}

bool FatLfnDirectoryEntry::isHiddenFlag() {
    return realEntry->isHiddenFlag();
}

void FatLfnDirectoryEntry::setHiddenFlag(bool hidden) {
    checkWritable();
    
    realEntry->setHiddenFlag(hidden);
}

bool FatLfnDirectoryEntry::isSystemFlag() {
    return realEntry->isSystemFlag();
}

void FatLfnDirectoryEntry::setSystemFlag(bool systemEntry) {
    checkWritable();
    
    realEntry->setSystemFlag(systemEntry);
}

bool FatLfnDirectoryEntry::isReadOnlyFlag() {
    return realEntry->isReadonlyFlag();
}

void FatLfnDirectoryEntry::setReadOnlyFlag(bool readOnly) {
    checkWritable();
    
    realEntry->setReadonlyFlag(readOnly);
}

bool FatLfnDirectoryEntry::isArchiveFlag() {
    return realEntry->isArchiveFlag();
}

void FatLfnDirectoryEntry::setArchiveFlag(bool archive) {
    checkWritable();
    
    realEntry->setArchiveFlag(archive);
}

std::vector<FatDirectoryEntry*> FatLfnDirectoryEntry::compactForm() {
    return std::vector<FatDirectoryEntry*>{realEntry};
}

std::string FatLfnDirectoryEntry::getName() {
    checkValid();
    
    return fileName;
}

std::string FatLfnDirectoryEntry::getAkaiPartString() {
    if (isDirectory()) return "";
    return AkaiPart::parse(realEntry->data).asSimpleString();
}

void FatLfnDirectoryEntry::setAkaiPartString(std::string& s) {
    if (isDirectory()) return;
    AkaiPart ap(s);
    ap.write(realEntry->data);
}

FsDirectory* FatLfnDirectoryEntry::getParent() {
    checkValid();
    
    return parent;
}

void FatLfnDirectoryEntry::setName(std::string newName) {
    checkWritable();
    
    if (!parent->isFreeName(newName)) {
        throw "the name \"" + newName + "\" is already in use";
    }
    
    parent->unlinkEntry(this);
    fileName = newName;
    parent->linkEntry(this);
}

void FatLfnDirectoryEntry::moveTo(FatLfnDirectory* target, std::string& newName) {
    checkWritable();
    
    if (!target->isFreeName(newName)) {
        throw "the name \"" + newName + "\" is already in use";
    }
    
    parent->unlinkEntry(this);
    parent = target;
    fileName = newName;
    parent->linkEntry(this);
}

FatFile* FatLfnDirectoryEntry::getFile() {
    return parent->getFile(realEntry);
}

FatLfnDirectory* FatLfnDirectoryEntry::getDirectory() {
    return parent->getDirectory(realEntry);
}

bool FatLfnDirectoryEntry::isFile() {
    return realEntry->isFile();
}

bool FatLfnDirectoryEntry::isDirectory() {
    return realEntry->isDirectory();
}

bool FatLfnDirectoryEntry::isDirty() {
    return realEntry->isDirty();
}

bool FatLfnDirectoryEntry::isValid() {
    return realEntry->isValid();
}

bool FatLfnDirectoryEntry::isReadOnly() {
    return realEntry->isReadOnly();
}
