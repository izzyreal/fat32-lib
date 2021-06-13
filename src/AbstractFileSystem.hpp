public abstract class AbstractFileSystem implements FileSystem {
    private const bool readOnly;
    private bool closed;
    
    /**
     * Creates a new {@code AbstractFileSystem}.
     * 
     * @param readOnly if the file system should be read-only
     */
    public AbstractFileSystem(bool readOnly) {
        this.closed = false;
        this.readOnly = readOnly;
    }
    
    @Override
    public void close() throw (std::exception) {
        if (!isClosed()) {
            if (!isReadOnly()) {
                flush();
            }
            
            closed = true;
        }
    }
    
    @Override
    public const bool isClosed() {
        return closed;
    }
    
    @Override
    public const bool isReadOnly() {
        return readOnly;
    }

    /**
     * Checks if this {@code FileSystem} was already closed, and throws an
     * exception if it was.
     *
     * @throws IllegalStateException if this {@code FileSystem} was
     *      already closed
     * @see #isClosed()
     * @see #close() 
     */
    protected const void checkClosed() throws IllegalStateException {
        if (isClosed()) {
            throw new IllegalStateException("file system was already closed");
        }
    }
    
    /**
     * Checks if this {@code FileSystem} is read-only, and throws an
     * exception if it is.
     *
     * @throws ReadOnlyException if this {@code FileSystem} is read-only
     * @see #isReadOnly() 
     */
    protected const void checkReadOnly() throws ReadOnlyException {
        if (isReadOnly()) {
            throw new ReadOnlyException();
        }
    }
}
