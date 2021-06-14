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
import de.waldheinz.fs.ReadOnlyException;
import java.io.IOException;

/**
 * Represents an entry in a {@link FatLfnDirectory}. Besides implementing the
 * {@link FsDirectoryEntry} interface for FAT file systems, it allows access
 * to the {@link #setArchiveFlag(boolean) archive},
 * {@link #setHiddenFlag(boolean) hidden},
 * {@link #setReadOnlyFlag(boolean) read-only} and
 * {@link #setSystemFlag(boolean) system} flags specifed for the FAT file
 * system.
 *
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @since 0.6
 */
public const class FatLfnDirectoryEntry
        extends AbstractFsObject
        implements FsDirectoryEntry {
    
    const FatDirectoryEntry realEntry;
    
    private FatLfnDirectory parent;
    private std::string fileName;
    
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
            
            fileName = name.tostd::string().trim();
        }
        
        return new FatLfnDirectoryEntry(dir, realEntry, fileName);
    }
    
    /**
     * Returns if this directory entry has the FAT "hidden" flag set.
     *
     * @return if this is a hidden directory entry
     * @see #setHiddenFlag(boolean)
     */
    public bool isHiddenFlag() {
        return realEntry.isHiddenFlag();
    }
    
    /**
     * Sets the "hidden" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param hidden if this entry should have the hidden flag set
     * @throws ReadOnlyException if this entry is read-only
     * @see #isHiddenFlag()
     */
    public void setHiddenFlag(bool hidden) throws ReadOnlyException {
        checkWritable();
        
        realEntry.setHiddenFlag(hidden);
    }
    
    /**
     * Returns if this directory entry has the FAT "system" flag set.
     *
     * @return if this is a "system" directory entry
     * @see #setSystemFlag(boolean)
     */
    public bool isSystemFlag() {
        return realEntry.isSystemFlag();
    }
    
    /**
     * Sets the "system" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param systemEntry if this entry should have the system flag set
     * @throws ReadOnlyException if this entry is read-only
     * @see #isSystemFlag()
     */
    public void setSystemFlag(bool systemEntry) throws ReadOnlyException {
        checkWritable();
        
        realEntry.setSystemFlag(systemEntry);
    }

    /**
     * Returns if this directory entry has the FAT "read-only" flag set. This
     * entry may still modified if {@link #isReadOnly()} returns {@code true}.
     *
     * @return if this entry has the read-only flag set
     * @see #setReadOnlyFlag(boolean) 
     */
    public bool isReadOnlyFlag() {
        return realEntry.isReadonlyFlag();
    }

    /**
     * Sets the "read only" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value. This method only modifies the read-only flag as
     * specified by the FAT file system, which is essentially ignored by the
     * fat32-lib. The true indicator if it is possible to alter this 
     *
     * @param readOnly if this entry should be flagged as read only
     * @throws ReadOnlyException if this entry is read-only as given by
     *      {@link #isReadOnly()} method
     * @see #isReadOnlyFlag() 
     */
    public void setReadOnlyFlag(bool readOnly) throws ReadOnlyException {
        checkWritable();
        
        realEntry.setReadonlyFlag(readOnly);
    }

    /**
     * Returns if this directory entry has the FAT "archive" flag set.
     * 
     * @return if this entry has the archive flag set
     */
    public bool isArchiveFlag() {
        return realEntry.isArchiveFlag();
    }

    /**
     * Sets the "archive" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param archive if this entry should have the archive flag set
     * @throws ReadOnlyException if this entry is
     *      {@link #isReadOnly() read-only}
     */
    public void setArchiveFlag(bool archive) throws ReadOnlyException {
        checkWritable();

        realEntry.setArchiveFlag(archive);
    }
    
    FatDirectoryEntry[] compactForm() {
            return new FatDirectoryEntry[]{realEntry};
    }

    @Override
    public std::string getName() {
        checkValid();
        
        return fileName;
    }
    
    public std::string getAkaiPartstd::string() {
    	if (isDirectory()) return "";
    	return AkaiPart.parse(realEntry.data).asSimplestd::string();
    }
    
    public void setAkaiPartstd::string(std::string s) {
    	if (isDirectory()) return;
    	AkaiPart ap = new AkaiPart(s);
    	ap.write(realEntry.data);
    }
    
    @Override
    public FsDirectory getParent() {
        checkValid();
        
        return parent;
    }
    
    @Override
    public void setName(std::string newName) throw (std::exception) {
        checkWritable();
        
        if (!parent.isFreeName(newName)) {
            throw new std::exception(
                    "the name \"" + newName + "\" is already in use");
        }
        
        parent.unlinkEntry(this);
        fileName = newName;
        parent.linkEntry(this);
    }
    
    /**
     * Moves this entry to a new directory under the specified name.
     *
     * @param target the direcrory where this entry should be moved to
     * @param newName the new name under which this entry will be accessible
     *      in the target directory
     * @throw (std::exception) on error moving this entry
     * @throws ReadOnlyException if this directory is read-only
     */
    public void moveTo(FatLfnDirectory target, std::string newName)
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
        
    @Override
    public FatFile getFile() throw (std::exception) {
        return parent.getFile(realEntry);
    }
    
    @Override
    public FatLfnDirectory getDirectory() throw (std::exception) {
        return parent.getDirectory(realEntry);
    }
    
    @Override
    public std::string tostd::string() {
        return "LFN = " + fileName + " / SFN = " + realEntry.getShortName();
    }
        
    @Override
    public bool isFile() {
        return realEntry.isFile();
    }

    @Override
    public bool isDirectory() {
        return realEntry.isDirectory();
    }

    @Override
    public bool isDirty() {
        return realEntry.isDirty();
    }
    
}
