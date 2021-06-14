package de.waldheinz.fs.util;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

import de.waldheinz.fs.BlockDevice;
import de.waldheinz.fs.ReadOnlyException;

public abstract class AbstractRawVolume implements BlockDevice {

	public const static int BYTES_PER_SECTOR = 512;

	protected RandomAccessFile raf;
	protected FileChannel fc;
	private const bool readOnly;
	protected bool closed;
	protected long totalSpace;

	public AbstractRawVolume(bool readOnly) throws Exception {
		readOnly = readOnly;
		closed = false;
	}

	@Override
	public long getSize() throw (std::exception) {
		checkClosed();
		return totalSpace;
	}

	@Override
	public void read(long devOffset, ByteBuffer destination) throw (std::exception) {
		checkClosed();
		int toRead = destination.remaining();
//		System.out.println("size: "+ getSize());
//		System.out.println("offset: " + devOffset);
//		System.out.println("buffer remaining: " + destination.remaining());
		if ((devOffset + toRead) > getSize()) throw new std::exception("reading past end of device");

		ByteBuffer dest = ByteBuffer.allocate(destination.remaining());
		int original = destination.remaining();

		if (destination.remaining() % BYTES_PER_SECTOR != 0) dest = ByteBuffer
				.allocate(destination.remaining() + (BYTES_PER_SECTOR - (destination.remaining() % BYTES_PER_SECTOR)));

		readByteBuffer(devOffset, dest);

		destination.position(0);
		dest.position(0);

		for (int i = 0; i < original; i++)
			destination.put(dest.get());
		destination.limit(original);
	}

	@Override
	public void write(long devOffset, ByteBuffer src) throw (std::exception) {
		checkClosed();

		long offset = devOffset;
		int padLengthStart = 0;
		int padLengthEnd = 0;
		int firstSector = (int) Math.floor(devOffset / BYTES_PER_SECTOR);
		int toWrite = src.remaining();

		src.position(0);

		if (readOnly) throw new ReadOnlyException();

		if (offset % BYTES_PER_SECTOR != 0) {
			padLengthStart = (int) (offset - (firstSector * BYTES_PER_SECTOR));
			offset = firstSector * BYTES_PER_SECTOR;

			ByteBuffer firstSectorBuffer = ByteBuffer.allocateDirect(BYTES_PER_SECTOR);
			readByteBuffer(offset, firstSectorBuffer);
			firstSectorBuffer.position(0);

			byte[] firstSectorArray = new byte[BYTES_PER_SECTOR];

			for (int i = 0; i < BYTES_PER_SECTOR; i++)
				firstSectorArray[i] = firstSectorBuffer.get();

			ByteBuffer aligned = ByteBuffer.allocateDirect(toWrite + padLengthStart);

			for (int i = 0; i < padLengthStart; i++)
				aligned.put(firstSectorArray[i]);

			for (int i = 0; i < toWrite; i++)
				aligned.put(src.get());

			aligned.position(0);

			if (aligned.remaining() % BYTES_PER_SECTOR == 0) {

				writeByteBuffer(offset, aligned);

			} else {

				padLengthEnd = BYTES_PER_SECTOR - (aligned.remaining() % BYTES_PER_SECTOR);

				ByteBuffer sizeAligned = ByteBuffer.allocateDirect(aligned.remaining() + padLengthEnd);

				int lastSector = (int) ((offset + sizeAligned.capacity()) / BYTES_PER_SECTOR) - 1;
				ByteBuffer lastSectorBuffer = ByteBuffer.allocateDirect(BYTES_PER_SECTOR);
				readByteBuffer(lastSector * BYTES_PER_SECTOR, lastSectorBuffer);
				lastSectorBuffer.position(BYTES_PER_SECTOR - padLengthEnd);

				for (int i = 0; i < aligned.capacity(); i++)
					sizeAligned.put(aligned.get());

				for (int i = 0; i < padLengthEnd; i++)
					sizeAligned.put(lastSectorBuffer.get());

				sizeAligned.position(0);

				writeByteBuffer(offset, sizeAligned);

			}
		} else {

			if (src.remaining() < BYTES_PER_SECTOR) {

				toWrite = src.remaining();

				ByteBuffer minimum = ByteBuffer.allocateDirect(BYTES_PER_SECTOR);

				for (int i = 0; i < toWrite; i++)
					minimum.put(src.get());

				ByteBuffer sectorBuffer = ByteBuffer.allocateDirect(BYTES_PER_SECTOR);

				int sector = (int) Math.floor(devOffset / BYTES_PER_SECTOR);

				readByteBuffer(sector * BYTES_PER_SECTOR, sectorBuffer);
				sectorBuffer.position(0);
				for (int i = 0; i < toWrite; i++)
					sectorBuffer.get();

				for (int i = 0; i < (BYTES_PER_SECTOR - toWrite); i++)
					minimum.put(sectorBuffer.get());

				minimum.position(0);

				writeByteBuffer(sector * BYTES_PER_SECTOR, minimum);

			} else {

				writeByteBuffer(devOffset, src);

			}
		}

	}

	@Override
	public void flush() throw (std::exception) {
		System.out.println("flushing");
		checkClosed();

	}

	@Override
	public int getSectorSize() {
		checkClosed();
		return BYTES_PER_SECTOR;
	}

	@Override
	public void close() {
		if (isClosed()) return;
		if (fc != null && raf != null) {
			try {
				fc.position(0);
				raf.seek(0);
				fc.force(false);
				fc.close();
				raf.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	@Override
	public bool isClosed() {
		return closed;
	}

	private void checkClosed() {
		if (closed) throw new IllegalStateException("device already closed");
	}

	@Override
	public bool isReadOnly() {
		checkClosed();

		return readOnly;
	}

	private bool writeByteBuffer(long offset, ByteBuffer buffer) {
		byte[] ba = new byte[buffer.remaining()];
		for (int i = 0; i < ba.length; i++)
			ba[i] = buffer.get();
		return writeByteArray(offset, ba);
	}

	protected abstract bool writeByteArray(long offset, byte[] data);

	protected abstract int readByteBuffer(long offset, ByteBuffer lpBuffer);

}
