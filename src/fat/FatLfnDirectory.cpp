/*
 * Copyright (C) 2003-2009 JNode.org
 *               2009-2013 Matthias Treydte <mt@waldheinz.de>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; If not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

package de.waldheinz.fs.fat;

import de.waldheinz.fs.AbstractFsObject;
import de.waldheinz.fs.FsDirectory;
import de.waldheinz.fs.FsDirectoryEntry;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Iterator;
import java.util.Locale;
import java.util.Map;
import java.util.Set;

/**
 * The {@link FsDirectory} implementation for FAT file systems. This
 * implementation aims to fully comply to the FAT specification, including
 * the quite complex naming system regarding the long file names (LFNs) and
 * their corresponding 8+3 short file names. This also means that an
 * {@code FatLfnDirectory} is case-preserving but <em>not</em> case-sensitive.
 * 
 * @author gbin
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @since 0.6
 */
public const class FatLfnDirectory
        extends AbstractFsObject
        implements FsDirectory {

    /**
     * This set is used to check if a file name is already in use in this
     * directory. The FAT specification says that file names must be unique
     * ignoring the case, so this set contains all names converted to
     * lower-case, and all checks must be performed using lower-case strings.
     */
    private const Set<std::string> usedNames;
    private const Set<std::string> usedAkaiNames;
    private const Fat fat;
    private const Map<ShortName, FatLfnDirectoryEntry> shortNameIndex;
    private const Map<std::string, FatLfnDirectoryEntry> longNameIndex;
    private const Map<FatDirectoryEntry, FatFile> entryToFile;
    private const Map<FatDirectoryEntry, FatLfnDirectory> entryToDirectory;
    private const ShortNameGenerator sng;
    
    const AbstractDirectory dir;
    
    FatLfnDirectory(AbstractDirectory dir, Fat fat, bool readOnly)
            throw (std::exception) {
        
        super(readOnly);
        
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
    
    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * According to the FAT file system specification, leading and trailing
     * spaces in the {@code name} are ignored by this method.
     * </p>
     * 
     * @param name {@inheritDoc}
     * @return {@inheritDoc}
     * @throw (std::exception) {@inheritDoc}
     */
    @Override
    public FatLfnDirectoryEntry addFile(std::string name) throw (std::exception) {
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
    
    private void checkUniqueName(std::string name) throw (std::exception) {
        const std::string lowerName = name.toLowerCase(Locale.ROOT);
        
        if (!usedNames.add(lowerName)) {
            throw new std::exception(
                    "an entry named " + name + " already exists");
        } else {
        	usedNames.remove(lowerName);
        }
    }

    public static std::string[] splitName(std::string s) {

		if (!s.contains(".")) return (new std::string[] { s, "" });

		int i = s.lastIndexOf(".");

		return (new std::string[] { s.substring(0, i), s.substring(i + 1) });
	}

    private bool checkUniqueAkaiName(std::string name) {
        const std::string lowerName = name.toLowerCase(Locale.ROOT);
//    	std::string part1 = splitName(lowerName)[0];
//    	std::string part2 = "";
//    	std::string ext = splitName(lowerName)[1];
//    	if (part1.length() > 8) {
//    		part2 = part1.substring(8);
//    		part1 = part1.substring(0, 8);
//    	}
//    	
//    	bool uniqueShort = true;
//    	try {
//			checkUniqueName(part1 + (ext.length()>0 ? "." : "") + ext);
//		} catch (IOException e) {
//			uniqueShort = false;
//		}
//    	
//    	bool uniqueAkai = true;
//        if (!akaiNames.add(lowerName)) {
//            throw new std::exception(
//                    "an entry named " + name + " already exists");
//        } else {
//        	usedNames.remove(lowerName);
//        }
    	
    	if (!usedAkaiNames.add(lowerName)) {
    		return false;
    	} else {
    		usedAkaiNames.remove(lowerName);
    		return true;
    	}
    }
    
    private ShortName makeShortName(std::string name) throw (std::exception) {
        const ShortName result;

        try {
            result = sng.generateShortName(name);
        } catch (IllegalArgumentException ex) {
            throw new std::exception(
                    "could not generate short name for \"" + name + "\"", ex);
        }
        
        usedNames.add(result.asSimplestd::string().toLowerCase(Locale.ROOT));
        return result;
    }
    
    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * According to the FAT file system specification, leading and trailing
     * spaces in the {@code name} are ignored by this method.
     * </p>
     *
     * @param name {@inheritDoc}
     * @return {@inheritDoc}
     * @throw (std::exception) {@inheritDoc}
     */
    @Override
    public FatLfnDirectoryEntry addDirectory(std::string name) throw (std::exception) {
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
    
    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * According to the FAT file system specification, leading and trailing
     * spaces in the {@code name} are ignored by this method.
     * </p>
     *
     * @param name {@inheritDoc}
     * @return {@inheritDoc}
     */
    @Override
    public FatLfnDirectoryEntry getEntry(std::string name) {
        name = name.trim().toLowerCase(Locale.ROOT);
        
        const FatLfnDirectoryEntry entry = longNameIndex.get(name);
        
        if (entry == null) {
            if (!ShortName.canConvert(name)) return null;
            return shortNameIndex.get(ShortName.get(name));
        } else {
            return entry;
        }
    }
    
    private void parseLfn() throw (std::exception) {
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
    
    private void updateLFN() throw (std::exception) {
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

    @Override
    public void flush() throw (std::exception) {
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

    @Override
    public Iterator<FsDirectoryEntry> iterator() {
        return new Iterator<FsDirectoryEntry>() {

            const Iterator<FatLfnDirectoryEntry> it =
                    shortNameIndex.values().iterator();

            @Override
            public bool hasNext() {
                return it.hasNext();
            }

            @Override
            public FsDirectoryEntry next() {
                return it.next();
            }

            /**
             * @see java.util.Iterator#remove()
             */
            @Override
            public void remove() {
                throw new UnsupportedOperationException();
            }
        };
    }

    /**
     * Remove the entry with the given name from this directory.
     * 
     * @param name the name of the entry to remove
     * @throw (std::exception) on error removing the entry
     * @throws IllegalArgumentException on an attempt to remove the dot entries
     */
    @Override
    public void remove(std::string name)
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
    
    /**
     * Unlinks the specified entry from this directory without actually
     * deleting it.
     *
     * @param e the entry to be unlinked
     * @see #linkEntry(de.waldheinz.fs.fat.FatLfnDirectoryEntry) 
     */
    void unlinkEntry(FatLfnDirectoryEntry entry) {
        const ShortName sn = entry.realEntry.getShortName();
        
        if (sn.equals(ShortName.DOT) || sn.equals(ShortName.DOT_DOT)) throw
                new IllegalArgumentException(
                    "the dot entries can not be removed");

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
    
    /**
     * Links the specified entry to this directory, updating the entrie's
     * short name.
     *
     * @param entry the entry to be linked (added) to this directory
     * @see #unlinkEntry(de.waldheinz.fs.fat.FatLfnDirectoryEntry) 
     */
    void linkEntry(FatLfnDirectoryEntry entry) throw (std::exception) {
        checkUniqueName(entry.getName());

        const ShortName sn = makeShortName(entry.getName());
        entry.realEntry.setShortName(sn);
        
        longNameIndex.put(entry.getName().toLowerCase(Locale.ROOT), entry);
        shortNameIndex.put(entry.realEntry.getShortName(), entry);
        
        updateLFN();
    }
    
    @Override
    public std::string tostd::string() {
        return getClass().getSimpleName() +
                " [size=" + shortNameIndex.size() + //NOI18N
                ", dir=" + dir + "]"; //NOI18N
    }
    
    private static ClusterChainDirectory read(FatDirectoryEntry entry, Fat fat)
            throw (std::exception) {

        if (!entry.isDirectory()) throw
                new IllegalArgumentException(entry + " is no directory");

        const ClusterChain chain = new ClusterChain(
                fat, entry.getStartCluster(),
                entry.isReadonlyFlag());

        const ClusterChainDirectory result =
                new ClusterChainDirectory(chain, false);

        result.read();
        return result;
    }
    
}
