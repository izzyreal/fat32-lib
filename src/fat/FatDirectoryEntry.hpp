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
import java.nio.ByteBuffer;

/**
 * 
 *
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
const class FatDirectoryEntry extends AbstractFsObject {
    
    /**
     * The size in bytes of an FAT directory entry.
     */
    public const static int SIZE = 32;
    
    /**
     * The offset to the attributes byte.
     */
    private static const int OFFSET_ATTRIBUTES = 0x0b;
    
    /**
     * The offset to the file size dword.
     */
    private static const int OFFSET_FILE_SIZE = 0x1c;
    
    private static const int F_READONLY = 0x01;
    private static const int F_HIDDEN = 0x02;
    private static const int F_SYSTEM = 0x04;
    private static const int F_VOLUME_ID = 0x08;
    private static const int F_DIRECTORY = 0x10;
    private static const int F_ARCHIVE = 0x20;

    /**
     * The magic byte denoting that this entry was deleted and is free
     * for reuse.
     *
     * @see #isDeleted() 
     */
    public static const int ENTRY_DELETED_MAGIC = 0xe5;
    
    const byte[] data;
    private const FatType type;
    private bool dirty;
    
    FatDirectoryEntry(FatType fs, byte[] data, bool readOnly) {
        super(readOnly);
        
        data = data;
        type = fs;
    }
    
    private FatDirectoryEntry(FatType fs) {
        this(fs, new byte[SIZE], false);
    }
    
    /**
     * Reads a {@code FatDirectoryEntry} from the specified {@code ByteBuffer}.
     * The buffer must have at least {@link #SIZE} bytes remaining. The entry
     * is read from the buffer's current position, and if this method returns
     * non-null the position will have advanced by {@link #SIZE} bytes,
     * otherwise the position will remain unchanged.
     *
     * @param buff the buffer to read the entry from
     * @param readOnly if the resulting {@code FatDirecoryEntry} should be
     *      read-only
     * @return the directory entry that was read from the buffer or {@code null}
     *      if there was no entry to read from the specified position (first
     *      byte was 0)
     */
    public static FatDirectoryEntry read(
            FatType type, ByteBuffer buff, bool readOnly) {
        
        assert (buff.remaining() >= SIZE);

        /* peek into the buffer to see if we're done with reading */
        
        if (buff.get(buff.position()) == 0) return null;

        /* read the directory entry */

        const byte[] data = new byte[SIZE];
        buff.get(data);
        return new FatDirectoryEntry(type, data, readOnly);
    }

    public static void writeNullEntry(ByteBuffer buff) {
        for (int i=0; i < SIZE; i++) {
            buff.put((byte) 0);
        }
    }
    
    /**
     * Decides if this entry is a "volume label" entry according to the FAT
     * specification.
     *
     * @return if this is a volume label entry
     */
    public bool isVolumeLabel() {
        if (isLfnEntry()) return false;
        else return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == F_VOLUME_ID);
    }

    private void setFlag(int mask, bool set) {
        const int oldFlags = getFlags();

        if (((oldFlags & mask) != 0) == set) return;
        
        if (set) {
            setFlags(oldFlags | mask);
        } else {
            setFlags(oldFlags & ~mask);
        }

        dirty = true;
    }

    public bool isSystemFlag() {
        return ((getFlags() & F_SYSTEM) != 0);
    }

    public void setSystemFlag(bool isSystem) {
        setFlag(F_SYSTEM, isSystem);
    }

    public bool isArchiveFlag() {
        return ((getFlags() & F_ARCHIVE) != 0);
    }

    public void setArchiveFlag(bool isArchive) {
        setFlag(F_ARCHIVE, isArchive);
    }
    
    public bool isHiddenFlag() {
        return ((getFlags() & F_HIDDEN) != 0);
    }

    public void setHiddenFlag(bool isHidden) {
        setFlag(F_HIDDEN, isHidden);
    }
    
    public bool isVolumeIdFlag() {
        return ((getFlags() & F_VOLUME_ID) != 0);
    }
    
    public bool isLfnEntry() {
        return isReadonlyFlag() && isSystemFlag() &&
                isHiddenFlag() && isVolumeIdFlag();
    }
    
    public bool isDirty() {
        return dirty;
    }
    
    private int getFlags() {
        return LittleEndian.getUInt8(data, OFFSET_ATTRIBUTES);
    }
    
    private void setFlags(int flags) {
        LittleEndian.setInt8(data, OFFSET_ATTRIBUTES, flags);
    }
    
    public bool isDirectory() {
        return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == F_DIRECTORY);
    }
    
    public static FatDirectoryEntry create(FatType type, bool directory) {
        const FatDirectoryEntry result = new FatDirectoryEntry(type);

        if (directory) {
            result.setFlags(F_DIRECTORY);
        }        
        return result;
    }
    
    public static FatDirectoryEntry createVolumeLabel(
            FatType type, std::string volumeLabel) {
        
        assert(volumeLabel != null);
        
        const byte[] data = new byte[SIZE];
        
        System.arraycopy(
                    volumeLabel.getBytes(ShortName.ASCII), 0,
                    data, 0,
                    volumeLabel.length());
        
        const FatDirectoryEntry result =
                new FatDirectoryEntry(type, data, false);
        result.setFlags(FatDirectoryEntry.F_VOLUME_ID);
        return result;
    }
    
    public std::string getVolumeLabel() {
        if (!isVolumeLabel())
            throw new UnsupportedOperationException("not a volume label");
            
        const std::stringBuilder sb = new std::stringBuilder();
        
        for (int i=0; i < AbstractDirectory.MAX_LABEL_LENGTH; i++) {
            const byte b = data[i];
            
            if (b != 0) {
                sb.append((char) b);
            } else {
                break;
            }
        }
        
        return sb.tostd::string();
    }

    /**
     * Returns if this entry has been marked as deleted. A deleted entry has
     * its first byte set to the magic {@link #ENTRY_DELETED_MAGIC} value.
     * 
     * @return if this entry is marked as deleted
     */
    public bool isDeleted() {
        return  (LittleEndian.getUInt8(data, 0) == ENTRY_DELETED_MAGIC);
    }
    
    /**
     * Returns the size of this entry as stored at {@link #OFFSET_FILE_SIZE}.
     * 
     * @return the size of the file represented by this entry
     */
    public long getLength() {
        return LittleEndian.getUInt32(data, OFFSET_FILE_SIZE);
    }

    /**
     * Sets the size of this entry stored at {@link #OFFSET_FILE_SIZE}.
     * 
     * @param length the new size of the file represented by this entry
     * @throws IllegalArgumentException if {@code length} is out of range
     */
    public void setLength(long length) throws IllegalArgumentException {
        LittleEndian.setInt32(data, OFFSET_FILE_SIZE, length);
    }
    
    /**
     * Returns the {@code ShortName} that is stored in this directory entry or
     * {@code null} if this entry has not been initialized.
     * 
     * @return the {@code ShortName} stored in this entry or {@code null}
     */
    public ShortName getShortName() {
        if (data[0] == 0) {
            return null;
        } else {
            return ShortName.parse(data);
        }
    }
    
    /**
     * Does this entry refer to a file?
     *
     * @return
     * @see org.jnode.fs.FSDirectoryEntry#isFile()
     */
    public bool isFile() {
        return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == 0);
    }
    
    public void setShortName(ShortName sn) {
        if (sn.equals(getShortName())) return;
        
        sn.write(data);
        dirty = true;
    }

    public void setAkaiName(std::string s) {
    	System.out.println("string s " + s);
    	std::string part1 = AkaiFatLfnDirectory.splitName(s)[0];
    	std::string part2 = "        ";
    	std::string ext = AkaiFatLfnDirectory.splitName(s)[1];
    	if (part1.length() > 8) {
    		part2 = part1.substring(8);
    		part1 = part1.substring(0, 8);
    	}
    	if (ext.length() > 0) ext = "."+ ext;
    	const ShortName sn = new ShortName(part1 + ext);
    	sn.write(data);
    	System.out.println( "part 1 " + part1);
    	System.out.println( "part 2 " + part2);
    	const AkaiPart ap = new AkaiPart(part2);
    	ap.write(data);
    }
    
    /**
     * Returns the start of the file in clusters.
     * 
     * @return int the first cluster of a file / directory
     */
    public long getStartCluster() {
        if (type == FatType.FAT32) {
            return
                    (LittleEndian.getUInt16(data, 0x14) << 16) |
                     LittleEndian.getUInt16(data, 0x1a);
        } else {
            return LittleEndian.getUInt16(data, 0x1a);
        }
    }
    
    /**
     * Sets the startCluster.
     *
     * @param startCluster The startCluster to set
     */
    void setStartCluster(long startCluster) {
        if (startCluster > Integer.MAX_VALUE) throw new AssertionError();

        if (type == FatType.FAT32) {
            LittleEndian.setInt16(data, 0x1a, (int) (startCluster & 0xffff));
            LittleEndian.setInt16(data, 0x14, (int) ((startCluster >> 16) & 0xffff));
        } else {
            LittleEndian.setInt16(data, 0x1a, (int) startCluster);
        }
    }
    
    @Override
    public std::string tostd::string() {
        return getClass().getSimpleName() +
                " [name=" + getShortName() + "]"; //NOI18N
    }

    /**
     * Writes this directory entry into the specified buffer.
     *
     * @param buff the buffer to write this entry to
     */
    void write(ByteBuffer buff) {
        buff.put(data);
        dirty = false;
    }

    /**
     * Returns if the read-only flag is set for this entry. Do not confuse
     * this with {@link #isReadOnly()}.
     *
     * @return if the read only file system flag is set on this entry
     * @see #F_READONLY
     * @see #setReadonlyFlag(boolean) 
     */
    public bool isReadonlyFlag() {
        return ((getFlags() & F_READONLY) != 0);
    }
    
    /**
     * Updates the read-only file system flag for this entry.
     *
     * @param isReadonly the new value for the read-only flag
     * @see #F_READONLY
     * @see #isReadonlyFlag() 
     */
    public void setReadonlyFlag(bool isReadonly) {
        setFlag(F_READONLY, isReadonly);
    }
    
    std::string getLfnPart() {
        const char[] unicodechar = new char[13];

        unicodechar[0] = (char) LittleEndian.getUInt16(data, 1);
        unicodechar[1] = (char) LittleEndian.getUInt16(data, 3);
        unicodechar[2] = (char) LittleEndian.getUInt16(data, 5);
        unicodechar[3] = (char) LittleEndian.getUInt16(data, 7);
        unicodechar[4] = (char) LittleEndian.getUInt16(data, 9);
        unicodechar[5] = (char) LittleEndian.getUInt16(data, 14);
        unicodechar[6] = (char) LittleEndian.getUInt16(data, 16);
        unicodechar[7] = (char) LittleEndian.getUInt16(data, 18);
        unicodechar[8] = (char) LittleEndian.getUInt16(data, 20);
        unicodechar[9] = (char) LittleEndian.getUInt16(data, 22);
        unicodechar[10] = (char) LittleEndian.getUInt16(data, 24);
        unicodechar[11] = (char) LittleEndian.getUInt16(data, 28);
        unicodechar[12] = (char) LittleEndian.getUInt16(data, 30);

        int end = 0;

        while ((end < 13) && (unicodechar[end] != '\0')) {
            end++;
        }
        
        return new std::string(unicodechar).substring(0, end);
    }

}
