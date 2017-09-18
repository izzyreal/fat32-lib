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
public final class FatLfnDirectoryEntry
        extends AbstractFsObject
        implements FsDirectoryEntry {
    
    final FatDirectoryEntry realEntry;
    
    private FatLfnDirectory parent;
    private String fileName;
    
    FatLfnDirectoryEntry(String name, ShortName sn,
            FatLfnDirectory parent, boolean directory) {
        
        super(false);
        
        this.parent = parent;
        this.fileName = name;
        
        this.realEntry = FatDirectoryEntry.create(
                parent.getFat().getFatType(), directory);
        this.realEntry.setShortName(sn);
    }

    FatLfnDirectoryEntry(FatLfnDirectory parent,
            FatDirectoryEntry realEntry, String fileName) {
        
        super(parent.isReadOnly());
        
        this.parent = parent;
        this.realEntry = realEntry;
        this.fileName = fileName;
    }
    
    static FatLfnDirectoryEntry extract(
            FatLfnDirectory dir, int offset, int len) {
            
        final FatDirectoryEntry realEntry = dir.dir.getEntry(offset + len - 1);
        final String fileName;
        
        if (len == 1) {
            /* this is just an old plain 8.3 entry */
            fileName = realEntry.getShortName().asSimpleString();
        } else {
            /* stored in reverse order */
            final StringBuilder name = new StringBuilder(13 * (len - 1));
            
            for (int i = len - 2; i >= 0; i--) {
                FatDirectoryEntry entry = dir.dir.getEntry(i + offset);
                name.append(entry.getLfnPart());
            }
            
            fileName = name.toString().trim();
        }
        
        return new FatLfnDirectoryEntry(dir, realEntry, fileName);
    }
    
    /**
     * Returns if this directory entry has the FAT "hidden" flag set.
     *
     * @return if this is a hidden directory entry
     * @see #setHiddenFlag(boolean)
     */
    public boolean isHiddenFlag() {
        return this.realEntry.isHiddenFlag();
    }
    
    /**
     * Sets the "hidden" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param hidden if this entry should have the hidden flag set
     * @throws ReadOnlyException if this entry is read-only
     * @see #isHiddenFlag()
     */
    public void setHiddenFlag(boolean hidden) throws ReadOnlyException {
        checkWritable();
        
        this.realEntry.setHiddenFlag(hidden);
    }
    
    /**
     * Returns if this directory entry has the FAT "system" flag set.
     *
     * @return if this is a "system" directory entry
     * @see #setSystemFlag(boolean)
     */
    public boolean isSystemFlag() {
        return this.realEntry.isSystemFlag();
    }
    
    /**
     * Sets the "system" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param systemEntry if this entry should have the system flag set
     * @throws ReadOnlyException if this entry is read-only
     * @see #isSystemFlag()
     */
    public void setSystemFlag(boolean systemEntry) throws ReadOnlyException {
        checkWritable();
        
        this.realEntry.setSystemFlag(systemEntry);
    }

    /**
     * Returns if this directory entry has the FAT "read-only" flag set. This
     * entry may still modified if {@link #isReadOnly()} returns {@code true}.
     *
     * @return if this entry has the read-only flag set
     * @see #setReadOnlyFlag(boolean) 
     */
    public boolean isReadOnlyFlag() {
        return this.realEntry.isReadonlyFlag();
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
    public void setReadOnlyFlag(boolean readOnly) throws ReadOnlyException {
        checkWritable();
        
        this.realEntry.setReadonlyFlag(readOnly);
    }

    /**
     * Returns if this directory entry has the FAT "archive" flag set.
     * 
     * @return if this entry has the archive flag set
     */
    public boolean isArchiveFlag() {
        return this.realEntry.isArchiveFlag();
    }

    /**
     * Sets the "archive" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param archive if this entry should have the archive flag set
     * @throws ReadOnlyException if this entry is
     *      {@link #isReadOnly() read-only}
     */
    public void setArchiveFlag(boolean archive) throws ReadOnlyException {
        checkWritable();

        this.realEntry.setArchiveFlag(archive);
    }
    
    FatDirectoryEntry[] compactForm() {
            return new FatDirectoryEntry[]{this.realEntry};
    }

    @Override
    public String getName() {
        checkValid();
        
        return fileName;
    }
    
    public String getAkaiPartString() {
    	if (this.isDirectory()) return "";
    	return AkaiPart.parse(this.realEntry.data).asSimpleString();
    }
    
    public void setAkaiPartString(String s) {
    	if (this.isDirectory()) return;
    	AkaiPart ap = new AkaiPart(s);
    	ap.write(this.realEntry.data);
    }
    
    @Override
    public FsDirectory getParent() {
        checkValid();
        
        return parent;
    }
    
    @Override
    public void setName(String newName) throws IOException {
        checkWritable();
        
        if (!this.parent.isFreeName(newName)) {
            throw new IOException(
                    "the name \"" + newName + "\" is already in use");
        }
        
        this.parent.unlinkEntry(this);
        this.fileName = newName;
        this.parent.linkEntry(this);
    }
    
    /**
     * Moves this entry to a new directory under the specified name.
     *
     * @param target the direcrory where this entry should be moved to
     * @param newName the new name under which this entry will be accessible
     *      in the target directory
     * @throws IOException on error moving this entry
     * @throws ReadOnlyException if this directory is read-only
     */
    public void moveTo(FatLfnDirectory target, String newName)
            throws IOException, ReadOnlyException {

        checkWritable();

        if (!target.isFreeName(newName)) {
            throw new IOException(
                    "the name \"" + newName + "\" is already in use");
        }
        
        this.parent.unlinkEntry(this);
        this.parent = target;
        this.fileName = newName;
        this.parent.linkEntry(this);
    }
        
    @Override
    public FatFile getFile() throws IOException {
        return parent.getFile(realEntry);
    }
    
    @Override
    public FatLfnDirectory getDirectory() throws IOException {
        return parent.getDirectory(realEntry);
    }
    
    @Override
    public String toString() {
        return "LFN = " + fileName + " / SFN = " + realEntry.getShortName();
    }
        
    @Override
    public boolean isFile() {
        return realEntry.isFile();
    }

    @Override
    public boolean isDirectory() {
        return realEntry.isDirectory();
    }

    @Override
    public boolean isDirty() {
        return realEntry.isDirty();
    }
    
}
