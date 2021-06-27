#include "AbstractDirectory.hpp"

#include "FatType.hpp"
#include "FatDirectoryEntry.hpp"
#include "ClusterChainDirectory.hpp"

using namespace akaifat::fat;

AbstractDirectory::AbstractDirectory(
        FatType *_type,
        int _capacity,
        bool _readOnly,
        bool _root
) : type(_type), capacity(_capacity), readOnly(_readOnly), _isRoot(_root) {
}

void AbstractDirectory::setEntries(std::vector<FatDirectoryEntry *> &newEntries) {
    if (newEntries.size() > capacity)
        throw std::runtime_error("too many entries");

    entries = newEntries;
}

void AbstractDirectory::sizeChanged(long newSize) {
    long newCount = newSize / FatDirectoryEntry::SIZE;

    if (newCount > INT_MAX)
        throw std::runtime_error("directory too large");

    capacity = (int) newCount;
}

void AbstractDirectory::read() {
    ByteBuffer data(capacity *FatDirectoryEntry::SIZE);

    read(data);
    data.flip();

    for (int i = 0; i < capacity; i++) {
        auto e = FatDirectoryEntry::read(type, data, readOnly);

        if (e == nullptr) continue;

        if (e->isVolumeLabel()) {
            if (!_isRoot)
                throw std::runtime_error("volume label in non-root directory");

            volumeLabel = e->getVolumeLabel();
        } else {
            entries.push_back(e);
        }
    }
}

FatDirectoryEntry *AbstractDirectory::getEntry(int idx) {
    return entries[idx];
}

int AbstractDirectory::getCapacity() const {
    return capacity;
}

int AbstractDirectory::getEntryCount() {
    return (int) entries.size();
}

bool AbstractDirectory::isDirReadOnly() {
    return readOnly;
}

bool AbstractDirectory::isRoot() const {
    return _isRoot;
}

int AbstractDirectory::getSize() {
    return (int) entries.size() + ((volumeLabel.length() != 0) ? 1 : 0);
}

void AbstractDirectory::flush() {
    ByteBuffer data(capacity *FatDirectoryEntry::SIZE
    +(volumeLabel.length() != 0 ? FatDirectoryEntry::SIZE : 0));

    for (auto entry : entries) {
        if (entry != nullptr)
            entry->write(data);
    }

    if (volumeLabel.length() != 0) {
        auto labelEntry =
                FatDirectoryEntry::createVolumeLabel(type, volumeLabel);

        labelEntry->write(data);
    }

    if (data.hasRemaining())
        FatDirectoryEntry::writeNullEntry(data);

    data.flip();

    write(data);
}

void AbstractDirectory::addEntry(FatDirectoryEntry *e) {
    assert (e != nullptr);

    if (getSize() == capacity)
        changeSize(capacity + 1);

    entries.push_back(e);
}

void AbstractDirectory::addEntries(std::vector<FatDirectoryEntry *> &newEntries) {
    if (getSize() + newEntries.size() > capacity)
        changeSize((int) (getSize() + newEntries.size()));

    for (auto &e : newEntries)
        entries.push_back(e);
}

void AbstractDirectory::removeEntry(FatDirectoryEntry *entry) {
    assert (entry != nullptr);

    auto it = find(begin(entries), end(entries), entry);

    if (it != end(entries))
        entries.erase(it);

    changeSize(getSize());
}

std::string &AbstractDirectory::getLabel() {
    checkRoot();

    return volumeLabel;
}

FatDirectoryEntry *AbstractDirectory::createSub(Fat *fat) {
    auto chain = new ClusterChain(fat, false);
    chain->setChainLength(1);

    auto entry = FatDirectoryEntry::create(type, true);
    entry->setStartCluster(chain->getStartCluster());

    auto dir = new ClusterChainDirectory(chain, false);

    auto dot = FatDirectoryEntry::create(type, true);
    auto sn_dot = ShortName::DOT();
    dot->setShortName(sn_dot);
    dot->setStartCluster(dir->getStorageCluster());
    dir->addEntry(dot);

    auto dotDot = FatDirectoryEntry::create(type, true);
    dotDot->setShortName(ShortName::DOT_DOT());
    dotDot->setStartCluster(getStorageCluster());
    dir->addEntry(dotDot);

    dir->flush();

    return entry;
}

void AbstractDirectory::setLabel(std::string &label) {
    checkRoot();

    if (label.length() > MAX_LABEL_LENGTH)
        throw std::runtime_error("label too long");

    volumeLabel = label;
}

void AbstractDirectory::checkRoot() const {
    if (!isRoot())
        throw std::runtime_error("only supported on root directories");
}
