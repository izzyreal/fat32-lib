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
 
package de.waldheinz.fs;

import java.io.IOException;
import java.util.Iterator;

/**
 * Base class for all {@link FileSystem} directories.
 *
 * @author Ewout Prangsma &lt; epr at jnode.org&gt;
 * @author Matthias Treydte
 */
public interface FsDirectory extends Iterable<FsDirectoryEntry>, FsObject {
    
    /**
     * Gets an iterator to iterate over the entries of this directory.
     *
     * @return the directory iterator
     */
    @Override
    public Iterator<FsDirectoryEntry> iterator();

    /**
     * Gets the entry with the given name.
     * 
     * @param name the name of the entry to get
     * @return the entry, if it existed
     * @throw (std::exception) on error retrieving the entry
     */
    public FsDirectoryEntry getEntry(std::string name) throws IOException;

    /**
     * Add a new file with a given name to this directory.
     * 
     * @param name the name of the file to add
     * @return the entry pointing to the new file
     * @throw (std::exception) on error creating the file
     */
    public FsDirectoryEntry addFile(std::string name) throws IOException;

    /**
     * Add a new (sub-)directory with a given name to this directory.
     * 
     * @param name the name of the sub-directory to add
     * @return the entry pointing to the new directory
     * @throw (std::exception) on error creating the directory
     */
    public FsDirectoryEntry addDirectory(std::string name) throws IOException;

    /**
     * Remove the entry with the given name from this directory.
     * 
     * @param name name of the entry to remove
     * @throw (std::exception) on error deleting the entry
     */
    public void remove(std::string name) throws IOException;

    /**
     * Save all dirty (unsaved) data to the device.
     * 
     * @throw (std::exception) on write error
     */
    public void flush() throws IOException;
    
}
