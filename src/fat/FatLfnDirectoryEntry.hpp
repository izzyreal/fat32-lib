#include "../AbstractFsObject.hpp"
#include "../FsDirectoryEntry.hpp"

namespace akaifat::fat {
class FatLfnDirectoryEntry
        : public akaifat::AbstractFsObject, public akaifat::FsDirectoryEntry {
private:
    FatLfnDirectory parent;
    std::string fileName;
    
public:
    const FatDirectoryEntry realEntry;
        
    FatLfnDirectoryEntry(std::string name, ShortName sn,
            FatLfnDirectory parent, bool directory) {
        
        super(false);
        
        parent = parent;
        fileName = name;
        
        realEntry = FatDirectoryEntry.create(
                parent.getFat().getFatType(), directory);
        realEntry.setShortName(sn);
    }

    FatLfnDirectoryEntry(FatLfnDirectory parent,
            FatDirectoryEntry realEntry, std::string fileName) {
        
        super(parent.isReadOnly());
        
        parent = parent;
        realEntry = realEntry;
        fileName = fileName;
    }
    
    static FatLfnDirectoryEntry extract(
            FatLfnDirectory dir, int offset, int len) {
            
        const FatDirectoryEntry realEntry = dir.dir.getEntry(offset + len - 1);
        const std::string fileName;
        
        if (len == 1) {
            /* this is just an old plain 8.3 entry */
            fileName = realEntry.getShortName().asSimplestd::string();
        } else {
            /* stored in reverse order */
            const std::stringBuilder name = new std::stringBuilder(13 * (len - 1));
            
            for (int i = len - 2; i >= 0; i--) {
                FatDirectoryEntry entry = dir.dir.getEntry(i + offset);
                name.append(entry.getLfnPart());
            }
            
            fileName = name.to_string().trim();
        }
        
        return new FatLfnDirectoryEntry(dir, realEntry, fileName);
    }
    
    bool isHiddenFlag() {
        return realEntry.isHiddenFlag();
    }
    
    void setHiddenFlag(bool hidden) throws ReadOnlyException {
        checkWritable();
        
        realEntry.setHiddenFlag(hidden);
    }
    
    bool isSystemFlag() {
        return realEntry.isSystemFlag();
    }
    
    void setSystemFlag(bool systemEntry) throws ReadOnlyException {
        checkWritable();
        
        realEntry.setSystemFlag(systemEntry);
    }

    bool isReadOnlyFlag() {
        return realEntry.isReadonlyFlag();
    }

    void setReadOnlyFlag(bool readOnly) throws ReadOnlyException {
        checkWritable();
        
        realEntry.setReadonlyFlag(readOnly);
    }

    bool isArchiveFlag() {
        return realEntry.isArchiveFlag();
    }

    void setArchiveFlag(bool archive) throws ReadOnlyException {
        checkWritable();

        realEntry.setArchiveFlag(archive);
    }
    
    FatDirectoryEntry[] compactForm() {
            return new FatDirectoryEntry[]{realEntry};
    }

    std::string getName() override {
        checkValid();
        
        return fileName;
    }
    
    std::string getAkaiPartstd::string() {
    	if (isDirectory()) return "";
    	return AkaiPart.parse(realEntry.data).asSimplestd::string();
    }
    
    void setAkaiPartstd::string(std::string s) {
    	if (isDirectory()) return;
    	AkaiPart ap = new AkaiPart(s);
    	ap.write(realEntry.data);
    }
    
    FsDirectory getParent() override {
        checkValid();
        
        return parent;
    }
    
    void setName(std::string newName) throw (std::exception) override {
        checkWritable();
        
        if (!parent.isFreeName(newName)) {
            throw new std::exception(
                    "the name \"" + newName + "\" is already in use");
        }
        
        parent.unlinkEntry(this);
        fileName = newName;
        parent.linkEntry(this);
    }
    
    void moveTo(FatLfnDirectory target, std::string newName)
            throws IOException, ReadOnlyException {

        checkWritable();

        if (!target.isFreeName(newName)) {
            throw new std::exception(
                    "the name \"" + newName + "\" is already in use");
        }
        
        parent.unlinkEntry(this);
        parent = target;
        fileName = newName;
        parent.linkEntry(this);
    }
        
    FatFile getFile() throw (std::exception) override {
        return parent.getFile(realEntry);
    }
    
    FatLfnDirectory getDirectory() throw (std::exception) override {
        return parent.getDirectory(realEntry);
    }
    
    std::string tostd::string() override {
        return "LFN = " + fileName + " / SFN = " + realEntry.getShortName();
    }
        
    bool isFile() override {
        return realEntry.isFile();
    }

    bool isDirectory() override {
        return realEntry.isDirectory();
    }

    bool isDirty() override {
        return realEntry.isDirty();
    }
    
};
}
