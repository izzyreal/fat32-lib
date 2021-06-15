#include "AbstractDirectory.hpp"

#include "FatType.hpp"
#include "FatDirectoryEntry.hpp"
#include "DirectoryFullException.hpp"

using namespace akaifat;

AbstractDirectory::AbstractDirectory(
    FatType _type,
    int _capacity,
    bool _readOnly,
    bool _isRoot
) : type (_type), capacity (_capacity), readOnly (_readOnly), isRoot (_isRoot)
{
}

void AbstractDirectory::setEntries(std::vector<FatDirectoryEntry>& newEntries)
{
    if (newEntries.size() > capacity)
        throw "too many entries";
    
    entries = newEntries;
}

void AbstractDirectory::sizeChanged(const long newSize)
{
    const long newCount = newSize / FatDirectoryEntry.SIZE;
    
    if (newCount > MAX_INT)
        "directory too large";
    
    capacity = (int) newCount;
}

void AbstractDirectory::read()
{
    const std::vector<char> data(getCapacity() * FatDirectoryEntry.SIZE);
    
    for (int i=0; i < getCapacity(); i++)
    {
        const std::shared_ptr<FatDirectoryEntry> e =
                FatDirectoryEntry::read(type, data, isReadOnly());
        
        if (!e) break;
        
        if (e->isVolumeLabel())
        {
            if (!isRoot)
                throw "volume label in non-root directory";
            
            volumeLabel = e->getVolumeLabel();
        }
        else
        {
            entries.push_back(e);
        }
    }
}

const std::shared_ptr<FatDirectoryEntry> AbstractDirectory::getEntry(int idx)
{
    return entries.get(idx);
}

const int AbstractDirectory::getCapacity()
{
    return capacity;
}

const int AbstractDirectory::getEntryCount()
{
    return entries.size();
}

bool AbstractDirectory::isReadOnly()
{
    return readOnly;
}

const bool AbstractDirectory::isRoot()
{
    return isRoot;
}

int AbstractDirectory::getSize()
{
    return entries.size() + ((volumeLabel != null) ? 1 : 0);
}

void AbstractDirectory::flush()
{    
    const std::vector<char> data(
            getCapacity() * FatDirectoryEntry.SIZE + (volumeLabel != null ? FatDirectoryEntry.SIZE : 0));
    
    for (FatDirectoryEntry entry : entries)
    {
        if (entry != null)
            entry.write(data);
    }
            
    if (volumeLabel != null)
    {
        const FatDirectoryEntry labelEntry =
                FatDirectoryEntry.createVolumeLabel(type, volumeLabel);

        labelEntry.write(data);
    }
    
    if (data.hasRemaining())
        FatDirectoryEntry.writeNullEntry(data);
    
    write(data);
}

void AbstractDirectory::addEntry(FatDirectoryEntry e)
{
    assert (e != null);
    
    if (getSize() == getCapacity())
        changeSize(getCapacity() + 1);

    entries.add(e);
}

void AbstractDirectory::addEntries(FatDirectoryEntry[] entries)
{    
    if (getSize() + entries.length > getCapacity())
        changeSize(getSize() + entries.length);

    entries.addAll(Arrays.asList(entries));
}

void AbstractDirectory::removeEntry(FatDirectoryEntry entry)
{
    assert (entry != null);
    
    entries.remove(entry);
    changeSize(getSize());
}

std::string& AbstractDirectory::getLabel()
{
    checkRoot();
    
    return volumeLabel;
}

FatDirectoryEntry AbstractDirectory::createSub(Fat fat)
{
    const ClusterChain chain = new ClusterChain(fat, false);
    chain.setChainLength(1);

    const FatDirectoryEntry entry = FatDirectoryEntry.create(type, true);
    entry.setStartCluster(chain.getStartCluster());
    
    const ClusterChainDirectory dir =
            new ClusterChainDirectory(chain, false);

    const FatDirectoryEntry dot = FatDirectoryEntry.create(type, true);
    dot.setShortName(ShortName.DOT);
    dot.setStartCluster(dir.getStorageCluster());
    dir.addEntry(dot);

    const FatDirectoryEntry dotDot = FatDirectoryEntry.create(type, true);
    dotDot.setShortName(ShortName.DOT_DOT);
    dotDot.setStartCluster(getStorageCluster());
    dir.addEntry(dotDot);

    dir.flush();

    return entry;
}

void AbstractDirectory::setLabel(std::string& label)
{
    checkRoot();

    if (label != null && label.length() > MAX_LABEL_LENGTH) throw "label too long";

    if (volumeLabel != null)
    {
        if (label == null)
        {
            changeSize(getSize() - 1);
            volumeLabel = null;
        }
        else
        {
            ShortName.checkValidChars(label.getBytes(ShortName.ASCII));
            volumeLabel = label;
        }
    }
    else
    {
        if (label != null)
        {
            changeSize(getSize() + 1);
            ShortName.checkValidChars(label.getBytes(ShortName.ASCII));
            volumeLabel = label;
        }
    }
}

void AbstractDirectory::checkRoot() {
    if (!isRoot())
    {
        throw "only supported on root directories";
    }
}
