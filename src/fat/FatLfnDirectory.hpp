#include "../AbstractFsObject.hpp"
#include "../FsDirectory.hpp"

namespace akaifat::fat {
class FatLfnDirectory
        : public akaifat::AbstractFsObject, public akaifat::FsDirectory {
private:
    const Set<std::string> usedNames;
    const Set<std::string> usedAkaiNames;
    const Fat fat;
    const Map<ShortName, FatLfnDirectoryEntry> shortNameIndex;
    const Map<std::string, FatLfnDirectoryEntry> longNameIndex;
    const Map<FatDirectoryEntry, FatFile> entryToFile;
    const Map<FatDirectoryEntry, FatLfnDirectory> entryToDirectory;
    const ShortNameGenerator sng;
    
    const AbstractDirectory dir;
    
public:
    FatLfnDirectory(AbstractDirectory dir, Fat fat, bool readOnly)
            throw (std::exception) : akaifat::AbstractFsObject(readOnly)
    {
        if ((dir == null) || (fat == null)) throw new NullPointerException();
        
        fat = fat;
        dir = dir;
        
        shortNameIndex =
                new LinkedHashMap<ShortName, FatLfnDirectoryEntry>();
                
        longNameIndex =
                new LinkedHashMap<std::string, FatLfnDirectoryEntry>();
                
        entryToFile =
                new LinkedHashMap<FatDirectoryEntry, FatFile>();
                
        entryToDirectory =
                new LinkedHashMap<FatDirectoryEntry, FatLfnDirectory>();
                
        usedNames = new HashSet<std::string>();
        usedAkaiNames = new HashSet<std::string>();
        sng = new ShortNameGenerator(usedNames);
        
        parseLfn();
    }

    Fat getFat() {
        return fat;
    }
    
    FatFile getFile(FatDirectoryEntry entry) throw (std::exception) {
        FatFile file = entryToFile.get(entry);

        if (file == null) {
            file = FatFile.get(fat, entry);
            entryToFile.put(entry, file);
        }
        
        return file;
    }
    
    FatLfnDirectory getDirectory(FatDirectoryEntry entry) throw (std::exception) {
        FatLfnDirectory result = entryToDirectory.get(entry);

        if (result == null) {
            const ClusterChainDirectory storage = read(entry, fat);
            result = new FatLfnDirectory(storage, fat, isReadOnly());
            entryToDirectory.put(entry, result);
        }
        
        return result;
    }
    
    FatLfnDirectoryEntry addFile(std::string name) throw (std::exception) override {
        checkWritable();
        checkUniqueName(name);
        
        name = name.trim();
        const ShortName sn = makeShortName(name);
        
        const FatLfnDirectoryEntry entry =
                new FatLfnDirectoryEntry(name, sn, this, false);

        dir.addEntries(entry.compactForm());
        
        shortNameIndex.put(sn, entry);
        longNameIndex.put(name.toLowerCase(Locale.ROOT), entry);

        getFile(entry.realEntry);
        
        return entry;
    }
    
    bool isFreeName(std::string name) {
        return !usedNames.contains(name.toLowerCase(Locale.ROOT));
    }
    
    static std::string[] splitName(std::string s) {

		if (!s.contains(".")) return (new std::string[] { s, "" });

		int i = s.lastIndexOf(".");

		return (new std::string[] { s.substring(0, i), s.substring(i + 1) });
	}

    FatLfnDirectoryEntry addDirectory(std::string name) throw (std::exception) override {
        checkWritable();
        checkUniqueName(name);
        name = name.trim();
        const ShortName sn = makeShortName(name);
        const FatDirectoryEntry real = dir.createSub(fat);
        real.setShortName(sn);
        const FatLfnDirectoryEntry e =
                new FatLfnDirectoryEntry(this, real, name);
        
        try {
            dir.addEntries(e.compactForm());
        } catch (IOException ex) {
            const ClusterChain cc =
                    new ClusterChain(fat, real.getStartCluster(), false);
            cc.setChainLength(0);
            dir.removeEntry(real);
            throw ex;
        }
        
        shortNameIndex.put(sn, e);
        longNameIndex.put(name.toLowerCase(Locale.ROOT), e);

        getDirectory(real);
        
        flush();
        return e;
    }
    
    FatLfnDirectoryEntry getEntry(std::string name) override {
        name = name.trim().toLowerCase(Locale.ROOT);
        
        const FatLfnDirectoryEntry entry = longNameIndex.get(name);
        
        if (entry == null) {
            if (!ShortName.canConvert(name)) return null;
            return shortNameIndex.get(ShortName.get(name));
        } else {
            return entry;
        }
    }
    

    void flush() throw (std::exception) override {
        checkWritable();
        
        for (FatFile f : entryToFile.values()) {
            f.flush();
        }
        
        for (FatLfnDirectory d : entryToDirectory.values()) {
            d.flush();
        }
        
        updateLFN();
        dir.flush();
    }

    Iterator<FsDirectoryEntry> iterator() override {
        return new Iterator<FsDirectoryEntry>() {

            const Iterator<FatLfnDirectoryEntry> it =
                    shortNameIndex.values().iterator();

    
            bool hasNext() override {
                return it.hasNext();
            }

    
            FsDirectoryEntry next() override {
                return it.next();
            }

            /**
             * @see java.util.Iterator#remove()
             */
    
            void remove() override {
                throw new UnsupportedOperationException();
            }
        };
    }

    void remove(std::string nameoverride )
            throws IOException, IllegalArgumentException {
        
        checkWritable();
        
        const FatLfnDirectoryEntry entry = getEntry(name);
        if (entry == null) return;
        
        unlinkEntry(entry);
        
        const ClusterChain cc = new ClusterChain(
                fat, entry.realEntry.getStartCluster(), false);

        cc.setChainLength(0);
        
        updateLFN();
    }
    
    void unlinkEntry(FatLfnDirectoryEntry entry) {
        const ShortName sn = entry.realEntry.getShortName();
        
        if (sn.equals(ShortName.DOT) || sn.equals(ShortName.DOT_DOT)) throw "the dot entries can not be removed";

        const std::string lowerName = entry.getName().toLowerCase(Locale.ROOT);

        assert (longNameIndex.containsKey(lowerName));
        longNameIndex.remove(lowerName);
        
        assert (shortNameIndex.containsKey(sn));
        shortNameIndex.remove(sn);
        usedNames.remove(sn.asSimplestd::string().toLowerCase(Locale.ROOT));
        
        assert (usedNames.contains(lowerName));
        usedNames.remove(lowerName);
        
        if (entry.isFile()) {
            entryToFile.remove(entry.realEntry);
        } else {
            entryToDirectory.remove(entry.realEntry);
        }
    }
    
    void linkEntry(FatLfnDirectoryEntry entry) throw (std::exception) {
        checkUniqueName(entry.getName());

        const ShortName sn = makeShortName(entry.getName());
        entry.realEntry.setShortName(sn);
        
        longNameIndex.put(entry.getName().toLowerCase(Locale.ROOT), entry);
        shortNameIndex.put(entry.realEntry.getShortName(), entry);
        
        updateLFN();
    }
    
private:
    void checkUniqueName(std::string name) throw (std::exception) {
        const std::string lowerName = name.toLowerCase(Locale.ROOT);
        
        if (!usedNames.add(lowerName)) {
            throw "an entry named " + name + " already exists";
        } else {
            usedNames.remove(lowerName);
        }
    }

    bool checkUniqueAkaiName(std::string name) {
        const std::string lowerName = name.toLowerCase(Locale.ROOT);        
        if (!usedAkaiNames.add(lowerName)) {
            return false;
        } else {
            usedAkaiNames.remove(lowerName);
            return true;
        }
    }
    
    ShortName makeShortName(std::string name) throw (std::exception) {
        const ShortName result;

        try {
            result = sng.generateShortName(name);
        } catch (const std::exception&) {
            throw "could not generate short name for \"" + name + "\"";
        }
        
        usedNames.add(result.asSimplestd::string().toLowerCase(Locale.ROOT));
        return result;
    }
        
    void parseLfn() throw (std::exception) {
        int i = 0;
        const int size = dir.getEntryCount();
        
        while (i < size) {
            // jump over empty entries
            while (i < size && dir.getEntry(i) == null) {
                i++;
            }

            if (i >= size) {
                break;
            }

            int offset = i; // beginning of the entry
            // check when we reach a real entry
            while (dir.getEntry(i).isLfnEntry()) {
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
            
            const FatLfnDirectoryEntry current =
                    FatLfnDirectoryEntry.extract(this, offset, ++i - offset);
            
            if (!current.realEntry.isDeleted() && current.isValid()) {
                checkUniqueName(current.getName());
                usedNames.add(current.realEntry.getShortName().asSimplestd::string().toLowerCase(Locale.ROOT));
                
                shortNameIndex.put(current.realEntry.getShortName(), current);
                longNameIndex.put(current
                        .getName()
                        .toLowerCase(Locale.ROOT), current);
            }
        }
    }
    
    void updateLFN() throw (std::exception) {
        ArrayList<FatDirectoryEntry> dest =
                new ArrayList<FatDirectoryEntry>();

        for (FatLfnDirectoryEntry currentEntry : shortNameIndex.values()) {
            FatDirectoryEntry[] encoded = currentEntry.compactForm();
            dest.addAll(Arrays.asList(encoded));
        }
        
        const int size = dest.size();

        dir.changeSize(size);
        dir.setEntries(dest);
    }

    static ClusterChainDirectory read(FatDirectoryEntry entry, Fat fat)
            throw (std::exception) {

        if (!entry.isDirectory()) entry + " is no directory";

        const ClusterChain chain = new ClusterChain(
                fat, entry.getStartCluster(),
                entry.isReadonlyFlag());

        const ClusterChainDirectory result =
                new ClusterChainDirectory(chain, false);

        result.read();
        return result;
    }
    
};
}
