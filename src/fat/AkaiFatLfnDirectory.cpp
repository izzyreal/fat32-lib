#include "AkaiFatLfnDirectory.hpp"

#include "AkaiFatLfnDirectoryEntry.hpp"

using namespace akaifat::fat;
using namespace akaifat;

AkaiFatLfnDirectory::AkaiFatLfnDirectory(AbstractDirectory *_dir, Fat *_fat, bool readOnly)
        : AbstractFsObject(readOnly), dir(_dir), fat(_fat) {
    parseLfn();
}

bool AkaiFatLfnDirectory::isDirReadOnly() {
    return AbstractFsObject::isReadOnly();
}

bool AkaiFatLfnDirectory::isDirValid() {
    return AbstractFsObject::isValid();
}

Fat *AkaiFatLfnDirectory::getFat() {
    return fat;
}

FatFile *AkaiFatLfnDirectory::getFile(FatDirectoryEntry *entry) {
    FatFile *file;

    if (entryToFile.find(entry) == end(entryToFile)) {
        file = FatFile::get(fat, entry);
        entryToFile[entry] = file;
    } else {
        file = entryToFile[entry];
    }

    return file;
}

AkaiFatLfnDirectory *AkaiFatLfnDirectory::getDirectory(FatDirectoryEntry *entry) {
    AkaiFatLfnDirectory *result;

    if (entryToDirectory.find(entry) == end(entryToDirectory)) {
        auto storage = read(entry, fat);
        result = new AkaiFatLfnDirectory(storage, fat, isReadOnly());
        entryToDirectory[entry] = result;
    } else {
        result = entryToDirectory[entry];
    }

    return result;
}

FsDirectoryEntry *AkaiFatLfnDirectory::addFile(std::string &name) {
//  checkWritable();
//  checkUniqueName(name);

//  name = name.trim();

//  auto entry = new AkaiFatLfnDirectoryEntry(name, this, false);

//  dir->addEntries(new FatDirectoryEntry[]{entry.realEntry});
//  akaiNameIndex.put(name.toLowerCase(Locale.ROOT), entry);

//  getFile(entry.realEntry);

//  return entry;
    return nullptr;
}

bool AkaiFatLfnDirectory::isFreeName(std::string &name) {
//  return !usedAkaiNames.contains(name.toLowerCase(Locale.ROOT));
    return true;
}


std::vector<std::string> AkaiFatLfnDirectory::splitName(std::string &s) {
//  if (!s.contains(".")) return (new std::string[] { s, "" });
//  int i = s.lastIndexOf(".");
//  return (new std::string[] { s.substring(0, i), s.substring(i + 1) });
    return {};
}

FsDirectoryEntry *AkaiFatLfnDirectory::addDirectory(std::string &name) {
//  checkWritable();
//  checkUniqueName(name);
//  name = name.trim();
//  FatDirectoryEntry real = dir.createSub(fat);
//  real.setAkaiName(name);
//  AkaiFatLfnDirectoryEntry e = new AkaiFatLfnDirectoryEntry(this, real, name);
//
//  try {
//    dir.addEntries(new FatDirectoryEntry[]{real});
//  } catch (std::exception&) {
//    ClusterChain cc = new ClusterChain(fat, real.getStartCluster(), false);
//    cc.setChainLength(0);
//    dir.removeEntry(real);
//    throw ex;
//  }
//
//  akaiNameIndex.put(name.toLowerCase(Locale.ROOT), e);
//
//  getDirectory(real);
//
//  flush();
//  return e;
    return nullptr;
}

FsDirectoryEntry *AkaiFatLfnDirectory::getEntry(std::string &name) {
    return akaiNameIndex[StrUtil::to_lower_copy(name)];
}

void AkaiFatLfnDirectory::flush() {
//  checkWritable();
//
//  for (FatFile f : entryToFile.values()) {
//    f.flush();
//  }
//
//  for (AkaiFatLfnDirectory d : entryToDirectory.values()) {
//    d.flush();
//  }
//
//  dir.flush();
}

void AkaiFatLfnDirectory::remove(std::string &name) {
//  checkWritable();
//
//  AkaiFatLfnDirectoryEntry entry = getEntry(name);
//  if (entry == null) return;
//
//  unlinkEntry(entry);
//
//  ClusterChain cc = new ClusterChain(fat, entry.realEntry.getStartCluster(), false);
//
//  cc.setChainLength(0);
//
    updateLFN();
}

void AkaiFatLfnDirectory::unlinkEntry(AkaiFatLfnDirectoryEntry *entry) {
//  if (entry.getName().startsWith(".") || entry.getName().length() == 0) return;
//
//  std::string lowerName = entry.getName().toLowerCase(Locale.ROOT);
//
//  assert(akaiNameIndex.containsKey(lowerName));
//  akaiNameIndex.remove(lowerName);
//
//  assert(usedAkaiNames.contains(lowerName));
//  usedAkaiNames.remove(lowerName);
//
//  if (entry.isFile()) {
//    entryToFile.remove(entry.realEntry);
//  } else {
//    entryToDirectory.remove(entry.realEntry);
//  }
}

void AkaiFatLfnDirectory::linkEntry(AkaiFatLfnDirectoryEntry *entry) {
//  checkUniqueName(entry.getName());
//  entry.realEntry.setAkaiName(entry.getName());
//  akaiNameIndex.put(entry.getName().toLowerCase(Locale.ROOT), entry);
}

void AkaiFatLfnDirectory::checkUniqueName(std::string &name) {
//  std::string lowerName = name.toLowerCase(Locale.ROOT);
//
//  if (!usedAkaiNames.add(lowerName))
//  {
//    throw "an entry named " + name + " already exists";
//  } else {
//    usedAkaiNames.remove(lowerName);
//  }
}

void AkaiFatLfnDirectory::parseLfn() {
//  int i = 0;
//  int size = dir.getEntryCount();
//
//  while (i < size) {
//    // jump over empty entries
//    while (i < size && dir.getEntry(i) == null) {
//      i++;
//    }
//
//    if (i >= size) {
//      break;
//    }
//
//    int offset = i;
//
//    while (dir.getEntry(i).isLfnEntry()) {
//      i++;
//      if (i >= size)
//        break;
//    }
//
//    if (i >= size)
//      break;
//
//    AkaiFatLfnDirectoryEntry current = AkaiFatLfnDirectoryEntry.extract(this, offset, ++i - offset);
//
//    if (!current.realEntry.isDeleted() && current.isValid()) {
//      checkUniqueName(current.getName());
//      usedAkaiNames.add(current.getName().toLowerCase(Locale.ROOT));
//      akaiNameIndex.put(current.getName().toLowerCase(Locale.ROOT), current);
//    }
//  }
}

void AkaiFatLfnDirectory::updateLFN() {
    std::vector<FatDirectoryEntry *> dest;

    for (auto &currentEntry : akaiNameIndex) {
        dest.push_back(currentEntry.second->realEntry);
    }

    dir->changeSize(static_cast<int>(dest.size()));
    dir->setEntries(dest);
}

ClusterChainDirectory *AkaiFatLfnDirectory::read(FatDirectoryEntry *entry, Fat *fat) {
    if (!entry->isDirectory()) throw std::runtime_error(entry->getShortName().asSimpleString() + " is no directory");

    ClusterChain chain(fat, entry->getStartCluster(), entry->isReadonlyFlag());

    auto result = new ClusterChainDirectory(chain, false);

    result->read();
}

std::map<std::string, AkaiFatLfnDirectoryEntry *>::iterator AkaiFatLfnDirectory::iterator() {
    return akaiNameIndex.begin();
}
