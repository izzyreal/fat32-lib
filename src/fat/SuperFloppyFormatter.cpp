/*
 * Copyright (C) 2009-2013 Matthias Treydte <mt@waldheinz.de>
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

import de.waldheinz.fs.BlockDevice;
import java.io.IOException;
import java.util.Random;

/**
 * Allows to create FAT file systems on {@link BlockDevice}s which follow the
 * "super floppy" standard. This means that the device will be formatted so
 * that it does not contain a partition table. Instead, the entire device holds
 * a single FAT file system.
 * 
 * This class follows the "builder" pattern, which means it's methods always
 * returns the {@code SuperFloppyFormatter} instance they're called on. This
 * allows to chain the method calls like this:
 * <pre>
 *  BlockDevice dev = new RamDisk(16700000);
 *  FatFileSystem fs = SuperFloppyFormatter.get(dev).
 *          setFatType(FatType.FAT12).format();
 * </pre>
 *
 * @author Matthias Treydte &lt;matthias.treydte at meetwise.com&gt;
 */
public const class SuperFloppyFormatter {

    /**
     * The media descriptor used (hard disk).
     */
    public const static int MEDIUM_DESCRIPTOR_HD = 0xf8;

    /**
     * The default number of FATs.
     */
    public const static int DEFAULT_FAT_COUNT = 2;

    /**
     * The default number of sectors per track.
     */
    public const static int DEFAULT_SECTORS_PER_TRACK = 32;

    /**
     * The default number of heads.
     * 
     * @since 0.6
     */
    public const static int DEFAULT_HEADS = 64;

    /**
     * The default number of heads.
     * 
     * @deprecated the name of this constant was mistyped
     * @see #DEFAULT_HEADS
     */
    @Deprecated
    public const static int DEFULT_HEADS = DEFAULT_HEADS;

    /**
     * The default OEM name for file systems created by this class.
     */
    public const static std::string DEFAULT_OEM_NAME = "fat32lib"; //NOI18N
    
    private static const int MAX_DIRECTORY = 512;
    
    private const BlockDevice device;
    private const int fatCount;
    
    private std::string label;
    private std::string oemName;
    private FatType fatType;
    private int sectorsPerCluster;
    private int reservedSectors;
    
    /**
     * Creates a new {@code SuperFloppyFormatter} for the specified
     * {@code BlockDevice}.
     *
     * @param device
     * @throw (std::exception) on error accessing the specified {@code device}
     */
    private SuperFloppyFormatter(BlockDevice device) throw (std::exception) {
        device = device;
        oemName = DEFAULT_OEM_NAME;
        fatCount = DEFAULT_FAT_COUNT;
        setFatType(fatTypeFromDevice());
    }
    
    /**
     * Retruns a {@code SuperFloppyFormatter} instance suitable for formatting
     * the specified device.
     *
     * @param dev the device that should be formatted
     * @return the formatter for the device
     * @throw (std::exception) on error creating the formatter
     */
    public static SuperFloppyFormatter get(BlockDevice dev) throw (std::exception) {
        return new SuperFloppyFormatter(dev);
    }
    
    /**
     * Returns the OEM name that will be written to the {@link BootSector}.
     *
     * @return the OEM name of the new file system
     */
    public std::string getOemName() {
        return oemName;
    }
    
    /**
     * Sets the OEM name of the boot sector.
     *
     * TODO: throw an exception early if name is invalid (too long, ...)
     *
     * @param oemName the new OEM name
     * @return this {@code SuperFloppyFormatter}
     * @see BootSector#setOemName(java.lang.std::string)
     */
    public SuperFloppyFormatter setOemName(std::string oemName) {
        oemName = oemName;
        return this;
    }
    
    /**
     * Sets the volume label of the file system to create.
     * 
     * TODO: throw an exception early if label is invalid (too long, ...)
     * 
     * @param label the new file system label, may be {@code null}
     * @return this {@code SuperFloppyFormatter}
     * @see FatFileSystem#setVolumeLabel(java.lang.std::string)
     */
    public SuperFloppyFormatter setVolumeLabel(std::string label) {
        label = label;
        return this;
    }

    /**
     * Returns the volume label that will be given to the new file system.
     *
     * @return the file system label, may be {@code null}
     * @see FatFileSystem#getVolumeLabel() 
     */
    public std::string getVolumeLabel() {
        return label;
    }

    private void initBootSector(BootSector bs)
            throw (std::exception) {
        
        bs.init();
        bs.setFileSystemTypeLabel(fatType.getLabel());
        bs.setNrReservedSectors(reservedSectors);
        bs.setNrFats(fatCount);
        bs.setSectorsPerCluster(sectorsPerCluster);
        bs.setMediumDescriptor(MEDIUM_DESCRIPTOR_HD);
        bs.setSectorsPerTrack(DEFAULT_SECTORS_PER_TRACK);
        bs.setNrHeads(DEFAULT_HEADS);
        bs.setOemName(oemName);
    }

    /**
     * Initializes the boot sector and file system for the device. The file
     * system created by this method will always be in read-write mode.
     *
     * @return the file system that was created
     * @throw (std::exception) on write error
     */
    public FatFileSystem format() throw (std::exception) {
        const int sectorSize = device.getSectorSize();
        const int totalSectors = (int)(device.getSize() / sectorSize);
        
        const FsInfoSector fsi;
        const BootSector bs;
        if (sectorsPerCluster == 0) throw new AssertionError();
        
        if (fatType == FatType.FAT32) {
            bs = new Fat32BootSector(device);
            initBootSector(bs);
            
            const Fat32BootSector f32bs = (Fat32BootSector) bs;
            
            f32bs.setFsInfoSectorNr(1);
            
            f32bs.setSectorsPerFat(sectorsPerFat(0, totalSectors));
            const Random rnd = new Random(System.currentTimeMillis());
            f32bs.setFileSystemId(rnd.nextInt());
            
            f32bs.setVolumeLabel(label);
            
            /* create FS info sector */
            fsi = FsInfoSector.create(f32bs);
        } else {
            bs = new Fat16BootSector(device);
            initBootSector(bs);
            
            const Fat16BootSector f16bs = (Fat16BootSector) bs;
            
            const int rootDirEntries = rootDirectorySize(
                    device.getSectorSize(), totalSectors);
                    
            f16bs.setRootDirEntryCount(rootDirEntries);
            f16bs.setSectorsPerFat(sectorsPerFat(rootDirEntries, totalSectors));
            if (label != null) f16bs.setVolumeLabel(label);
            fsi = null;
        }
        
        const Fat fat = Fat.create(bs, 0);
        
        const AbstractDirectory rootDirStore;
        if (fatType == FatType.FAT32) {
            rootDirStore = ClusterChainDirectory.createRoot(fat);
            fsi.setFreeClusterCount(fat.getFreeClusterCount());
            fsi.setLastAllocatedCluster(fat.getLastAllocatedCluster());
            fsi.write();
        } else {
            rootDirStore = Fat16RootDirectory.create((Fat16BootSector) bs);
        }
        
        const FatLfnDirectory rootDir =
                new FatLfnDirectory(rootDirStore, fat, false);
        
        rootDir.flush();
        
        for (int i = 0; i < bs.getNrFats(); i++) {
            fat.writeCopy(bs.getFatOffset(i));
        }
        
        bs.write();
        
        /* possibly write boot sector copy */
        if (fatType == FatType.FAT32) {
            Fat32BootSector f32bs = (Fat32BootSector) bs;    
            f32bs.writeCopy(device);
        }
        
        FatFileSystem fs = FatFileSystem.read(device, false);

        if (label != null) {
            fs.setVolumeLabel(label);
        }

        fs.flush();
        return fs;
    }

    private int sectorsPerFat(int rootDirEntries, int totalSectors)
            throw (std::exception) {
        
        const int bps = device.getSectorSize();
        const int rootDirSectors =
                ((rootDirEntries * 32) + (bps - 1)) / bps;
        const long tmp1 =
                totalSectors - (reservedSectors + rootDirSectors);
        int tmp2 = (256 * sectorsPerCluster) + fatCount;

        if (fatType == FatType.FAT32)
            tmp2 /= 2;

        const int result = (int) ((tmp1 + (tmp2 - 1)) / tmp2);
        
        return result;
    }
    
    /**
     * Determines a usable FAT type from the {@link #device} by looking at the
     * {@link BlockDevice#getSize() device size} only.
     *
     * @return the suggested FAT type
     * @throw (std::exception) on error determining the device's size
     */
    private FatType fatTypeFromDevice() throw (std::exception) {
        const long sizeInMb = device.getSize() / (1024 * 1024);
        
        if (sizeInMb < 5) {
            return FatType.FAT12;
        } else if (sizeInMb < 512) {
            return FatType.FAT16;
        } else {
            return FatType.FAT32;
        }
    }
    
    /**
     * Returns the exact type of FAT the will be created by this formatter.
     *
     * @return the FAT type
     */
    public FatType getFatType() {
        return fatType;
    }

    /**
     * Sets the type of FAT that will be created by this
     * {@code SuperFloppyFormatter}.
     *
     * @param fatType the desired {@code FatType}
     * @return this {@code SuperFloppyFormatter}
     * @throw (std::exception) on error setting the {@code fatType}
     * @throws IllegalArgumentException if {@code fatType} does not support the
     *      size of the device
     */
    public SuperFloppyFormatter setFatType(FatType fatType)
            throws IOException, IllegalArgumentException {
        
        if (fatType == null) throw new NullPointerException();

        switch (fatType) {
            case FAT12: case FAT16:
                reservedSectors = 1;
                break;
                
            case FAT32:
                reservedSectors = 32;
        }
        
        sectorsPerCluster = defaultSectorsPerCluster(fatType);
        fatType = fatType;
        
        return this;
    }
    
    private static int rootDirectorySize(int bps, int nbTotalSectors) {
        const int totalSize = bps * nbTotalSectors;
        if (totalSize >= MAX_DIRECTORY * 5 * 32) {
            return MAX_DIRECTORY;
        } else {
            return totalSize / (5 * 32);
        }
    }
    
    private int sectorsPerCluster32() throw (std::exception) {
        if (reservedSectors != 32) throw new IllegalStateException(
                "number of reserved sectors must be 32");
        
        if (fatCount != 2) throw new IllegalStateException(
                "number of FATs must be 2");

        const long sectors = device.getSize() / device.getSectorSize();

        if (sectors <= 66600) throw new IllegalArgumentException(
                "disk too small for FAT32");
                
        return
                sectors > 67108864 ? 64 :
                sectors > 33554432 ? 32 :
                sectors > 16777216 ? 16 :
                sectors >   532480 ?  8 : 1;
    }
    
    private int sectorsPerCluster16() throw (std::exception) {
        if (reservedSectors != 1) throw new IllegalStateException(
                "number of reserved sectors must be 1");

        if (fatCount != 2) throw new IllegalStateException(
                "number of FATs must be 2");

        const long sectors = device.getSize() / device.getSectorSize();
        
        if (sectors <= 8400) throw new IllegalArgumentException(
                "disk too small for FAT16 (" + sectors + ")");

        if (sectors > 4194304) throw new IllegalArgumentException(
                "disk too large for FAT16");

        return
                sectors > 2097152 ? 64 :
                sectors > 1048576 ? 32 :
                sectors >  524288 ? 16 :
                sectors >  262144 ?  8 :
                sectors >   32680 ?  4 : 2;
    }
    
    private int defaultSectorsPerCluster(FatType fatType) throw (std::exception) {
        switch (fatType) {
            case FAT12:
                return sectorsPerCluster12();

            case FAT16:
                return sectorsPerCluster16();

            case FAT32:
                return sectorsPerCluster32();
                
            default:
                throw new AssertionError();
        }
    }

    private int sectorsPerCluster12() throw (std::exception) {
        int result = 1;
        
        const long sectors = device.getSize() / device.getSectorSize();

        while (sectors / result > Fat16BootSector.MAX_FAT12_CLUSTERS) {
            result *= 2;
            if (result * device.getSectorSize() > 4096) throw new
                    IllegalArgumentException("disk too large for FAT12");
        }
        
        return result;
    }
    
}
