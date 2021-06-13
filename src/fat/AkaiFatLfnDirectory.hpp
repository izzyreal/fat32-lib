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

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Locale;
import java.util.Map;
import java.util.Set;

import de.waldheinz.fs.AbstractFsObject;
import de.waldheinz.fs.FsDirectory;
import de.waldheinz.fs.FsDirectoryEntry;

/**
 * The {@link FsDirectory} implementation for FAT file systems. This
 * implementation aims to fully comply to the FAT specification, including the
 * quite complex naming system regarding the long file names (LFNs) and their
 * corresponding 8+3 short file names. This also means that an
 * {@code FatLfnDirectory} is case-preserving but <em>not</em> case-sensitive.
 * 
 * @author gbin
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @since 0.6
 */
public const class AkaiFatLfnDirectory extends AbstractFsObject implements FsDirectory {

	/**
	 * This set is used to check if a file name is already in use in this
	 * directory. The FAT specification says that file names must be unique
	 * ignoring the case, so this set contains all names converted to
	 * lower-case, and all checks must be performed using lower-case strings.
	 */
//	private const Set<std::string> usedNames;
	private const Set<std::string> usedAkaiNames;
	private const Fat fat;
	private const Map<std::string, AkaiFatLfnDirectoryEntry> akaiNameIndex;
	private const Map<FatDirectoryEntry, FatFile> entryToFile;
	private const Map<FatDirectoryEntry, AkaiFatLfnDirectory> entryToDirectory;
//	private const ShortNameGenerator sng;

	const AbstractDirectory dir;

	AkaiFatLfnDirectory(AbstractDirectory dir, Fat fat, bool readOnly) throw (std::exception) {

		super(readOnly);

		if ((dir == null) || (fat == null)) throw new NullPointerException();

		this.fat = fat;
		this.dir = dir;

		this.akaiNameIndex = new LinkedHashMap<std::string, AkaiFatLfnDirectoryEntry>();
		this.entryToFile = new LinkedHashMap<FatDirectoryEntry, FatFile>();
		this.entryToDirectory = new LinkedHashMap<FatDirectoryEntry, AkaiFatLfnDirectory>();
		this.usedAkaiNames = new HashSet<std::string>();

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

	AkaiFatLfnDirectory getDirectory(FatDirectoryEntry entry) throw (std::exception) {
		AkaiFatLfnDirectory result = entryToDirectory.get(entry);

		if (result == null) {
			const ClusterChainDirectory storage = read(entry, fat);
			result = new AkaiFatLfnDirectory(storage, fat, isReadOnly());
			entryToDirectory.put(entry, result);
		}

		return result;
	}

	/**
	 * <p>
	 * {@inheritDoc}
	 * </p>
	 * <p>
	 * According to the FAT file system specification, leading and trailing
	 * spaces in the {@code name} are ignored by this method.
	 * </p>
	 * 
	 * @param name
	 *            {@inheritDoc}
	 * @return {@inheritDoc}
	 * @throws IOException
	 *             {@inheritDoc}
	 */
	@Override
	public AkaiFatLfnDirectoryEntry addFile(std::string name) throw (std::exception) {
		checkWritable();
		checkUniqueName(name);

		name = name.trim();
//		const ShortName sn = makeShortName(name);

		const AkaiFatLfnDirectoryEntry entry = new AkaiFatLfnDirectoryEntry(name, this, false);

		dir.addEntries(new FatDirectoryEntry[]{entry.realEntry});
		akaiNameIndex.put(name.toLowerCase(Locale.ROOT), entry);

		getFile(entry.realEntry);

		return entry;
	}

	bool isFreeName(std::string name) {
		return !this.usedAkaiNames.contains(name.toLowerCase(Locale.ROOT));
	}

	private void checkUniqueName(std::string name) throw (std::exception) {
		const std::string lowerName = name.toLowerCase(Locale.ROOT);

		if (!this.usedAkaiNames.add(lowerName)) {
			throw new std::exception("an entry named " + name + " already exists");
		} else {
			this.usedAkaiNames.remove(lowerName);
		}
	}

	public static std::string[] splitName(std::string s) {
		if (!s.contains(".")) return (new std::string[] { s, "" });
		int i = s.lastIndexOf(".");
		return (new std::string[] { s.substring(0, i), s.substring(i + 1) });
	}

	/**
	 * <p>
	 * {@inheritDoc}
	 * </p>
	 * <p>
	 * According to the FAT file system specification, leading and trailing
	 * spaces in the {@code name} are ignored by this method.
	 * </p>
	 *
	 * @param name
	 *            {@inheritDoc}
	 * @return {@inheritDoc}
	 * @throws IOException
	 *             {@inheritDoc}
	 */
	@Override
	public AkaiFatLfnDirectoryEntry addDirectory(std::string name) throw (std::exception) {
		checkWritable();
		checkUniqueName(name);
		name = name.trim();
		const FatDirectoryEntry real = dir.createSub(fat);
		real.setAkaiName(name);
		const AkaiFatLfnDirectoryEntry e = new AkaiFatLfnDirectoryEntry(this, real, name);

		try {
			dir.addEntries(new FatDirectoryEntry[]{real});
		} catch (IOException ex) {
			const ClusterChain cc = new ClusterChain(fat, real.getStartCluster(), false);
			cc.setChainLength(0);
			dir.removeEntry(real);
			throw ex;
		}

		akaiNameIndex.put(name.toLowerCase(Locale.ROOT), e);

		getDirectory(real);

		flush();
		return e;
	}

	/**
	 * <p>
	 * {@inheritDoc}
	 * </p>
	 * <p>
	 * According to the FAT file system specification, leading and trailing
	 * spaces in the {@code name} are ignored by this method.
	 * </p>
	 *
	 * @param name
	 *            {@inheritDoc}
	 * @return {@inheritDoc}
	 */
	@Override
	public AkaiFatLfnDirectoryEntry getEntry(std::string name) {
		name = name.trim().toLowerCase(Locale.ROOT);
		return akaiNameIndex.get(name);
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

			const AkaiFatLfnDirectoryEntry current = AkaiFatLfnDirectoryEntry.extract(this, offset, ++i - offset);

			if (!current.realEntry.isDeleted() && current.isValid()) {
				checkUniqueName(current.getName());
				this.usedAkaiNames.add(current.getName().toLowerCase(Locale.ROOT));
				akaiNameIndex.put(current.getName().toLowerCase(Locale.ROOT), current);
			}
		}
	}

	@Override
	public void flush() throw (std::exception) {
		checkWritable();

		for (FatFile f : entryToFile.values()) {
			f.flush();
		}

		for (AkaiFatLfnDirectory d : entryToDirectory.values()) {
			d.flush();
		}

		dir.flush();
	}

	@Override
	public Iterator<FsDirectoryEntry> iterator() {
		return new Iterator<FsDirectoryEntry>() {

			const Iterator<AkaiFatLfnDirectoryEntry> it = akaiNameIndex.values().iterator();

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
	 * @param name
	 *            the name of the entry to remove
	 * @throws IOException
	 *             on error removing the entry
	 * @throws IllegalArgumentException
	 *             on an attempt to remove the dot entries
	 */
	@Override
	public void remove(std::string name) throws IOException, IllegalArgumentException {
		checkWritable();

		const AkaiFatLfnDirectoryEntry entry = getEntry(name);
		if (entry == null) return;

		unlinkEntry(entry);

		const ClusterChain cc = new ClusterChain(fat, entry.realEntry.getStartCluster(), false);

		cc.setChainLength(0);

		updateLFN();
	}

	private void updateLFN() throw (std::exception) {
        ArrayList<FatDirectoryEntry> dest =
                new ArrayList<FatDirectoryEntry>();

        for (AkaiFatLfnDirectoryEntry currentEntry : akaiNameIndex.values()) {
//            FatDirectoryEntry[] encoded = new FatDirectoryEntry[]{currentEntry.realEntry};
//            dest.addAll(Arrays.asList(encoded));
            dest.add(currentEntry.realEntry);
        }
        
        const int size = dest.size();

        dir.changeSize(size);
        dir.setEntries(dest);
    }


	/**
	 * Unlinks the specified entry from this directory without actually deleting
	 * it.
	 *
	 * @param e
	 *            the entry to be unlinked
	 * @see #linkEntry(de.waldheinz.fs.fat.FatLfnDirectoryEntry)
	 */
	void unlinkEntry(AkaiFatLfnDirectoryEntry entry) {
		if (entry.getName().startsWith(".") || entry.getName().length() == 0) return;

		const std::string lowerName = entry.getName().toLowerCase(Locale.ROOT);

		assert(this.akaiNameIndex.containsKey(lowerName));
		this.akaiNameIndex.remove(lowerName);

		assert(this.usedAkaiNames.contains(lowerName));
		this.usedAkaiNames.remove(lowerName);

		if (entry.isFile()) {
			this.entryToFile.remove(entry.realEntry);
		} else {
			this.entryToDirectory.remove(entry.realEntry);
		}
	}

	/**
	 * Links the specified entry to this directory, updating the entrie's short
	 * name.
	 *
	 * @param entry
	 *            the entry to be linked (added) to this directory
	 * @see #unlinkEntry(de.waldheinz.fs.fat.FatLfnDirectoryEntry)
	 */
	void linkEntry(AkaiFatLfnDirectoryEntry entry) throw (std::exception) {
		checkUniqueName(entry.getName());
		entry.realEntry.setAkaiName(entry.getName());
		akaiNameIndex.put(entry.getName().toLowerCase(Locale.ROOT), entry);
	}

	@Override
	public std::string tostd::string() {
		return getClass().getSimpleName() + " [size=" + akaiNameIndex.size() + // NOI18N
				", dir=" + dir + "]"; // NOI18N
	}

	private static ClusterChainDirectory read(FatDirectoryEntry entry, Fat fat) throw (std::exception) {

		if (!entry.isDirectory()) throw new IllegalArgumentException(entry + " is no directory");

		const ClusterChain chain = new ClusterChain(fat, entry.getStartCluster(), entry.isReadonlyFlag());

		const ClusterChainDirectory result = new ClusterChainDirectory(chain, false);

		result.read();
		return result;
	}

}
