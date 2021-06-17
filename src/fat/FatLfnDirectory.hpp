#pragma once

#include "../AbstractFsObject.hpp"
#include "../FsDirectory.hpp"

#include "AkaiFatLfnDirectoryEntry.hpp"
#include "Fat.hpp"

#include <set>
#include <string>

namespace akaifat::fat {
class FatLfnDirectory
        : public akaifat::AbstractFsObject, public akaifat::FsDirectory {
private:
    std::set<std::string> usedNames;
    std::set<std::string> usedAkaiNames;
    Fat* fat;
//    std::map<ShortName, FatLfnDirectoryEntry> shortNameIndex;
//    std::map<std::string, FatLfnDirectoryEntry> longNameIndex;
//    std::map<FatDirectoryEntry, FatFile> entryToFile;
//    std::map<FatDirectoryEntry, FatLfnDirectory> entryToDirectory;
//    ShortNameGenerator sng;
        
public:
    AbstractDirectory* dir;

    FatLfnDirectory(AbstractDirectory* _dir, Fat* _fat, bool readOnly)
    : akaifat::AbstractFsObject(readOnly)
    {
        if ((_dir == nullptr) || (_fat == nullptr)) throw "dir or fat == nullptr";
        
        fat = _fat;
        dir = _dir;
        
//        shortNameIndex =
//                new LinkedHashstd::map<ShortName, FatLfnDirectoryEntry>();
//
//        longNameIndex =
//                new LinkedHashstd::map<std::string, FatLfnDirectoryEntry>();
//
//        entryToFile =
//                new LinkedHashstd::map<FatDirectoryEntry, FatFile>();
//
//        entryToDirectory =
//                new LinkedHashstd::map<FatDirectoryEntry, FatLfnDirectory>();
//
//        usedNames = new Hashstd::set<std::string>();
//        usedAkaiNames = new Hashstd::set<std::string>();
//        sng = new ShortNameGenerator(usedNames);
        
//        parseLfn();
    }

    Fat* getFat() {
        return fat;
    }
    
    FatFile* getFile(FatDirectoryEntry* entry) {
//        FatFile* file = entryToFile.get(entry);
//
//        if (file == null) {
//            file = FatFile.get(fat, entry);
//            entryToFile.put(entry, file);
//        }
//
//        return file;
        return nullptr;
    }
    
    FatLfnDirectory* getDirectory(FatDirectoryEntry* entry) {
//        FatLfnDirectory result = entryToDirectory.get(entry);
//
//        if (result == null) {
//            ClusterChainDirectory storage = read(entry, fat);
//            result = new FatLfnDirectory(storage, fat, isReadOnly());
//            entryToDirectory.put(entry, result);
//        }
//
//        return result;
        return nullptr;
    }
    
    FsDirectoryEntry* addFile(std::string& name) override {
        checkWritable();
//        checkUniqueName(name);
//
//        name = name.trim();
//        ShortName sn = makeShortName(name);
//
//        FatLfnDirectoryEntry entry =
//                new FatLfnDirectoryEntry(name, sn, this, false);
//
//        dir.addEntries(entry.compactForm());
//
//        shortNameIndex.put(sn, entry);
//        longNameIndex.put(name.toLowerCase(Locale.ROOT), entry);
//
//        getFile(entry.realEntry);
//
//        return entry;
        return nullptr;
    }
    
    bool isFreeName(std::string name) {
//        return !usedNames.contains(name.toLowerCase(Locale.ROOT));
        return true;
    }
    
    static std::vector<std::string> splitName(std::string& s) {
//		if (!s.contains(".")) return (new std::string[] { s, "" });
//
//		int i = s.lastIndexOf(".");
//
//		return (new std::string[] { s.substring(0, i), s.substring(i + 1) });
        return {};
	}

    FsDirectoryEntry* addDirectory(std::string& name) override {
//        checkWritable();
//        checkUniqueName(name);
//        name = name.trim();
//        ShortName sn = makeShortName(name);
//        FatDirectoryEntry real = dir.createSub(fat);
//        real.setShortName(sn);
//        FatLfnDirectoryEntry e =
//                new FatLfnDirectoryEntry(this, real, name);
//
//        try {
//            dir.addEntries(e.compactForm());
//        } catch (IOException ex) {
//            ClusterChain cc =
//                    new ClusterChain(fat, real.getStartCluster(), false);
//            cc.setChainLength(0);
//            dir.removeEntry(real);
//            throw ex;
//        }
//
//        shortNameIndex.put(sn, e);
//        longNameIndex.put(name.toLowerCase(Locale.ROOT), e);
//
//        getDirectory(real);
//
//        flush();
//        return e;
        return nullptr;
    }
    
    FsDirectoryEntry* getEntry(std::string& name) override {
//        name = name.trim().toLowerCase(Locale.ROOT);
//
//        FatLfnDirectoryEntry entry = longNameIndex.get(name);
//
//        if (entry == null) {
//            if (!ShortName.canConvert(name)) return null;
//            return shortNameIndex.get(ShortName.get(name));
//        } else {
//            return entry;
//        }
        return nullptr;
    }
    

    void flush() override {
        checkWritable();
        
//        for (FatFile f : entryToFile.values()) {
//            f.flush();
//        }
//
//        for (FatLfnDirectory d : entryToDirectory.values()) {
//            d.flush();
//        }
//
        updateLFN();
        dir->flush();
    }

//    Iterator<FsDirectoryEntry> iterator() override {
//        return new Iterator<FsDirectoryEntry>() {
//
//            Iterator<FatLfnDirectoryEntry> it =
//                    shortNameIndex.values().iterator();
//
//
//            bool hasNext() override {
//                return it.hasNext();
//            }
//
//
//            FsDirectoryEntry next() override {
//                return it.next();
//            }
//
//            /**
//             * @see java.util.Iterator#remove()
//             */
//
//            void remove() override {
//                throw new UnsupportedOperationException();
//            }
//        };
//    }

    void remove(std::string& name) override {
        
        checkWritable();
        
//        FatLfnDirectoryEntry entry = getEntry(name);
//        if (entry == null) return;
//
//        unlinkEntry(entry);
//
//        ClusterChain cc = new ClusterChain(
//                fat, entry.realEntry.getStartCluster(), false);
//
//        cc.setChainLength(0);
        
        updateLFN();
    }
    
    void unlinkEntry(FsDirectoryEntry* entry) {
//        ShortName sn = entry.realEntry.getShortName();
//
//        if (sn.equals(ShortName.DOT) || sn.equals(ShortName.DOT_DOT)) throw "the dot entries can not be removed";
//
//        std::string lowerName = entry.getName().toLowerCase(Locale.ROOT);
//
//        assert (longNameIndex.containsKey(lowerName));
//        longNameIndex.remove(lowerName);
//
//        assert (shortNameIndex.containsKey(sn));
//        shortNameIndex.remove(sn);
//        usedNames.remove(sn.asSimpleString().toLowerCase(Locale.ROOT));
//
//        assert (usedNames.contains(lowerName));
//        usedNames.remove(lowerName);
//
//        if (entry.isFile()) {
//            entryToFile.remove(entry.realEntry);
//        } else {
//            entryToDirectory.remove(entry.realEntry);
//        }
    }
    
    void linkEntry(FsDirectoryEntry* entry) {
        auto name = entry->getName();
        checkUniqueName(name);

//        ShortName sn = makeShortName(entry->getName());
//        entry.realEntry.setShortName(sn);
//
//        longNameIndex.put(entry.getName().toLowerCase(Locale.ROOT), entry);
//        shortNameIndex.put(entry.realEntry.getShortName(), entry);
//
        updateLFN();
    }
    
private:
    void checkUniqueName(std::string& name) {
//        std::string lowerName = name.toLowerCase(Locale.ROOT);
//
//        if (!usedNames.add(lowerName)) {
//            throw "an entry named " + name + " already exists";
//        } else {
//            usedNames.remove(lowerName);
//        }
    }

    bool checkUniqueAkaiName(std::string name) {
//        std::string lowerName = name.toLowerCase(Locale.ROOT);
//        if (!usedAkaiNames.add(lowerName)) {
//            return false;
//        } else {
//            usedAkaiNames.remove(lowerName);
//            return true;
//        }
        return true;
    }
    
    ShortName makeShortName(std::string name) {
        ShortName result;

//        try {
//            result = sng.generateShortName(name);
//        } catch (std::exception&) {
//            throw "could not generate short name for \"" + name + "\"";
//        }
//
//        usedNames.add(result.asSimpleString().toLowerCase(Locale.ROOT));
        return result;
    }
        
    void parseLfn() {
        int i = 0;
        int size = dir->getEntryCount();
        
        while (i < size) {
            // jump over empty entries
            while (i < size && dir->getEntry(i) == nullptr) {
                i++;
            }

            if (i >= size) {
                break;
            }

            int offset = i; // beginning of the entry
            // check when we reach a real entry
            while (dir->getEntry(i)->isLfnEntry()) {
                i++;
                if (i >= size) {
                    // This is a cutted entry, forgive it
                    break;
                }
            }
            
            if (i >= size) {
                // This is a cutted entry, forgive it
                break;
            }
            
//            FatLfnDirectoryEntry current =
//                    FatLfnDirectoryEntry.extract(this, offset, ++i - offset);
//
//            if (!current.realEntry.isDeleted() && current.isValid()) {
//                checkUniqueName(current.getName());
//                usedNames.add(current.realEntry.getShortName().asSimpleString().toLowerCase(Locale.ROOT));
//
//                shortNameIndex.put(current.realEntry.getShortName(), current);
//                longNameIndex.put(current
//                        .getName()
//                        .toLowerCase(Locale.ROOT), current);
//            }
        }
    }
    
    void updateLFN() {
//        ArrayList<FatDirectoryEntry> dest =
//                new ArrayList<FatDirectoryEntry>();
//
//        for (FatLfnDirectoryEntry currentEntry : shortNameIndex.values()) {
//            FatDirectoryEntry[] encoded = currentEntry.compactForm();
//            dest.addAll(Arrays.asList(encoded));
//        }
//
//        int size = dest.size();
//
//        dir.changeSize(size);
//        dir.setEntries(dest);
    }

    static ClusterChainDirectory* read(FatDirectoryEntry entry, Fat fat) {
        
        if (!entry.isDirectory()) throw entry.getShortName().asSimpleString() + " is no directory";

//        ClusterChain chain = new ClusterChain(
//                fat, entry.getStartCluster(),
//                entry.isReadonlyFlag());
//
//        ClusterChainDirectory result =
//                new ClusterChainDirectory(chain, false);
//
//        result.read();
//        return result;
        return nullptr;
    }
    
};
}
