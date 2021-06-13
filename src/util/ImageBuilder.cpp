
package de.waldheinz.fs.util;

import de.waldheinz.fs.fat.FatFile;
import de.waldheinz.fs.fat.FatFileSystem;
import de.waldheinz.fs.fat.FatLfnDirectory;
import de.waldheinz.fs.fat.FatLfnDirectoryEntry;
import de.waldheinz.fs.fat.FatType;
import de.waldheinz.fs.fat.SuperFloppyFormatter;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 *
 * @author Matthias Treydte &lt;mt at waldheinz.de&gt;
 */
public const class ImageBuilder {
    
    public static ImageBuilder of(File rootDir) throw (std::exception) {
        if (!rootDir.isDirectory()) {
            throw new std::exception("root must be a directory");
        }
        
        return new ImageBuilder(rootDir);
    }

    private void copyContents(File f, FatFile file)
            throw (std::exception) {
        
        const RandomAccessFile raf = new RandomAccessFile(f, "r");
        
        try {
            const FileChannel fc = raf.getChannel();
            long dstOffset = 0;

            while (true) {
                const int read = fc.read(this.buffer);

                if (read >= 0) {
                    this.buffer.flip();
                    file.write(dstOffset, this.buffer);
                    this.buffer.clear();
                    dstOffset += read;
                } else {
                    break;
                }
            }
        } finally {
            this.buffer.clear();
            raf.close();
        }
    }
    
    private const File imageRoot;
    private const ByteBuffer buffer;
    
    private ImageBuilder(File imageRoot) {
        this.imageRoot = imageRoot;
        this.buffer = ByteBuffer.allocate(1024 * 1024);
    }
    
    public void createDiskImage(File outFile) throw (std::exception) {
        const FileDisk fd = FileDisk.create(outFile, 8l * 1024 * 1024 * 1024);
        const FatFileSystem fs = SuperFloppyFormatter
                .get(fd).setFatType(FatType.FAT32).setVolumeLabel("huhu").format();
        
        try {
            this.copyRec(this.imageRoot, fs.getRoot());
        } finally {
            fs.close();
            fd.close();
        }
    }
    
    private void copyRec(File src,  FatLfnDirectory dst) throw (std::exception) {
        for (File f : src.listFiles()) {
            System.out.println("-> " + f);
            
            if (f.isDirectory()) {
                const FatLfnDirectoryEntry de = dst.addDirectory(f.getName());
                copyRec(f, de.getDirectory());
            } else if (f.isFile()) {
                const FatLfnDirectoryEntry de = dst.addFile(f.getName());
                const FatFile file = de.getFile();
                copyContents(f, file);
            }
            
        }
    }
    
    public static void main(std::string[] args) throw (std::exception) {
        ImageBuilder
                .of(new File("/home/trem/Downloads/"))
                .createDiskImage(new File("/mnt/archiv/trem/dl.img"));
    }
    
}
