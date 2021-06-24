#include "AbstractDirectory.hpp"

#include "FatType.hpp"
#include "FatDirectoryEntry.hpp"

using namespace akaifat::fat;

AbstractDirectory::AbstractDirectory(
    FatType* _type,
    int _capacity,
    bool _readOnly,
    bool __isRoot
) : type (_type), capacity (_capacity), readOnly (_readOnly), _isRoot (__isRoot)
{
}

//void AbstractDirectory::setEntries(std::vector<FatDirectoryEntry>& newEntries)
//{
//    if (newEntries.size() > capacity)
//        throw "too many entries";
//    
//    entries = newEntries;
//}

void AbstractDirectory::sizeChanged(long newSize)
{
    long newCount = newSize / FatDirectoryEntry::SIZE;
    
    if (newCount > INT_MAX) throw "directory too large";
    
    capacity = (int) newCount;
}

void AbstractDirectory::read()
{
    auto capacity = getCapacity();
    
    ByteBuffer data(capacity * FatDirectoryEntry::SIZE);
    
    read(data);
    data.flip();

    for (int i = 0; i < capacity; i++)
    {
        auto e = FatDirectoryEntry::read(type, data, readOnly);
        
        printf("AbstractDirectory read entry with name: %s\n", e->getShortName().asSimpleString().c_str());
        
        if (!e) break;
        
        if (e->isVolumeLabel())
        {
            if (!_isRoot)
                throw "volume label in non-root directory";
            
            volumeLabel = e->getVolumeLabel();
        }
        else
        {
            entries.push_back(e);
        }
    }
}

FatDirectoryEntry* AbstractDirectory::getEntry(int idx)
{
    return entries[idx];
}

int AbstractDirectory::getCapacity()
{
    return capacity;
}

int AbstractDirectory::getEntryCount()
{
    return (int) entries.size();
}

bool AbstractDirectory::isDirReadOnly()
{
    return readOnly;
}

bool AbstractDirectory::isRoot()
{
    return _isRoot;
}

int AbstractDirectory::getSize()
{
    return (int) entries.size() + ((volumeLabel.length() != 0) ? 1 : 0);
}

void AbstractDirectory::flush()
{
    std::vector<char> data(getCapacity() * FatDirectoryEntry::SIZE + (volumeLabel.length() != 0 ? FatDirectoryEntry::SIZE : 0));
    
    for (auto entry : entries)
    {
        if (entry != nullptr)
            entry->write(data);
    }
            
    if (volumeLabel.length() != 0)
    {
        auto labelEntry =
                FatDirectoryEntry::createVolumeLabel(type, volumeLabel);

        labelEntry->write(data);
    }
    
//    if (data.hasRemaining())
//        FatDirectoryEntry::writeNullEntry(data);
    
//    write(data);
}

void AbstractDirectory::addEntry(FatDirectoryEntry* e)
{
    assert (e != nullptr);
    
    if (getSize() == getCapacity())
        changeSize(getCapacity() + 1);

    entries.push_back(e);
}

void AbstractDirectory::addEntries(std::vector<FatDirectoryEntry*>& newEntries)
{    
    if (getSize() + newEntries.size() > getCapacity())
        changeSize( (int) (getSize() + newEntries.size()) );

    for (auto& e : newEntries)
        entries.push_back(e);
}

void AbstractDirectory::removeEntry(FatDirectoryEntry* entry)
{
    assert (entry != nullptr);
    
//    entries.remove(entry);
    changeSize(getSize());
}

std::string& AbstractDirectory::getLabel()
{
    checkRoot();
    
    return volumeLabel;
}

FatDirectoryEntry* AbstractDirectory::createSub(Fat* fat)
{
//    auto chain = new ClusterChain(fat, false);
//    chain.setChainLength(1);
//
//    auto entry = FatDirectoryEntry::create(type, true);
//    entry.setStartCluster(chain.getStartCluster());
//
//    ClusterChainDirectory dir =
//            new ClusterChainDirectory(chain, false);
//
//    auto dot = FatDirectoryEntry::create(type, true);
//    dot.setShortName(ShortName.DOT);
//    dot.setStartCluster(dir.getStorageCluster());
//    dir.addEntry(dot);
//
//    FatDirectoryEntry dotDot = FatDirectoryEntry.create(type, true);
//    dotDot.setShortName(ShortName.DOT_DOT);
//    dotDot.setStartCluster(getStorageCluster());
//    dir.addEntry(dotDot);
//
//    dir.flush();
//
//    return entry;
    return nullptr;
}

void AbstractDirectory::setLabel(std::string& label)
{
    checkRoot();

    if (label.length() > MAX_LABEL_LENGTH) throw "label too long";

//    if (volumeLabel != null)
//    {
//        if (label == null)
//        {
//            changeSize(getSize() - 1);
//            volumeLabel = null;
//        }
//        else
//        {
//            ShortName.checkValidChars(label.getBytes(ShortName.ASCII));
//            volumeLabel = label;
//        }
//    }
//    else
//    {
//        if (label != null)
//        {
//            changeSize(getSize() + 1);
//            ShortName.checkValidChars(label.getBytes(ShortName.ASCII));
//            volumeLabel = label;
//        }
//    }
}

void AbstractDirectory::checkRoot() {
    if (!isRoot())
    {
        throw "only supported on root directories";
    }
}

std::vector<FatDirectoryEntry*>& AbstractDirectory::getEntries()
{
    return entries;
}
