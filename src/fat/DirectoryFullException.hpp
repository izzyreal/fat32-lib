namespace akaifat::fat {
class DirectoryFullException : std::exception {
private:
    const int currentCapacity;
    const int requestedCapacity;
    
    DirectoryFullException(int currentCapacity, int requestedCapacity)
    : DirectoryFullException("directory is full", currentCapacity, requestedCapacity)
    {
    }
    
    DirectoryFullException(std::string& message, int _currentCapacity, int _requestedCapacity)
    : std::exception (message), currentCapacity (_currentCapacity), requestedCapacity (_requestedCapacity)
    {
    }
    
    public int getCurrentCapacity() {
        return currentCapacity;
    }

    public int getRequestedCapacity() {
        return requestedCapacity;
    }
    
};
}
