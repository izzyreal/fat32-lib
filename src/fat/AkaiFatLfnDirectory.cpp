#include "AkaiFatLfnDirectory.hpp"

using namespace akaifat::fat;

AkaiFatLfnDirectory::AkaiFatLfnDirectory(AbstractDirectory& _dir, Fat& _fat, bool readOnly)
: AbstractFsObject (readOnly), dir (_dir)
{
  parseLfn();
}

Fat& AkaiFatLfnDirectory::getFat() {
  return fat;
}

FatFile AkaiFatLfnDirectory::getFile(FatDirectoryEntry entry)
{
  FatFile file = entryToFile.get(entry);

  if (file == null) {
    file = FatFile.get(fat, entry);
    entryToFile.put(entry, file);
  }

  return file;
}

AkaiFatLfnDirectory AkaiFatLfnDirectory::getDirectory(FatDirectoryEntry entry)
{
  AkaiFatLfnDirectory result = entryToDirectory.get(entry);

  if (result == null)
  {
    const ClusterChainDirectory storage = read(entry, fat);
    result = new AkaiFatLfnDirectory(storage, fat, isReadOnly());
    entryToDirectory.put(entry, result);
  }

  return result;
}

AkaiFatLfnDirectoryEntry AkaiFatLfnDirectory::addFile(std::string name)
{
  checkWritable();
  checkUniqueName(name);

  name = name.trim();

  const AkaiFatLfnDirectoryEntry entry = new AkaiFatLfnDirectoryEntry(name, this, false);

  dir.addEntries(new FatDirectoryEntry[]{entry.realEntry});
  akaiNameIndex.put(name.toLowerCase(Locale.ROOT), entry);

  getFile(entry.realEntry);

  return entry;
}

bool AkaiFatLfnDirectory::isFreeName(std::string name)
{
  return !usedAkaiNames.contains(name.toLowerCase(Locale.ROOT));
}


static std::string[] AkaiFatLfnDirectory::splitName(std::string s)
{
  if (!s.contains(".")) return (new std::string[] { s, "" });
  int i = s.lastIndexOf(".");
  return (new std::string[] { s.substring(0, i), s.substring(i + 1) });
}

AkaiFatLfnDirectoryEntry AkaiFatLfnDirectory::addDirectory(std::string name)
{
  checkWritable();
  checkUniqueName(name);
  name = name.trim();
  const FatDirectoryEntry real = dir.createSub(fat);
  real.setAkaiName(name);
  const AkaiFatLfnDirectoryEntry e = new AkaiFatLfnDirectoryEntry(this, real, name);

  try {
    dir.addEntries(new FatDirectoryEntry[]{real});
  } catch (IOException ex) {
    const ClusterChain cc = new ClusterChain(fat, real.getStartCluster(), false);
    cc.setChainLength(0);
    dir.removeEntry(real);
    throw ex;
  }

  akaiNameIndex.put(name.toLowerCase(Locale.ROOT), e);

  getDirectory(real);

  flush();
  return e;
}

AkaiFatLfnDirectoryEntry AkaiFatLfnDirectory::getEntry(std::string name)
{
  name = name.trim().toLowerCase(Locale.ROOT);
  return akaiNameIndex.get(name);
}

void AkaiFatLfnDirectory::flush()
{
  checkWritable();

  for (FatFile f : entryToFile.values()) {
    f.flush();
  }

  for (AkaiFatLfnDirectory d : entryToDirectory.values()) {
    d.flush();
  }

  dir.flush();
}

Iterator<FsDirectoryEntry> AkaiFatLfnDirectory::iterator()
{
  return new Iterator<FsDirectoryEntry>() {

    const Iterator<AkaiFatLfnDirectoryEntry> it = akaiNameIndex.values().iterator();

    @Override
    bool hasNext() {
      return it.hasNext();
    }

    @Override
    FsDirectoryEntry next() {
      return it.next();
    }

    @Override
    void remove() {
      throw new UnsupportedOperationException();
    }
  };
}

void AkaiFatLfnDirectory::remove(std::string name)
{
  checkWritable();

  const AkaiFatLfnDirectoryEntry entry = getEntry(name);
  if (entry == null) return;

  unlinkEntry(entry);

  const ClusterChain cc = new ClusterChain(fat, entry.realEntry.getStartCluster(), false);

  cc.setChainLength(0);

  updateLFN();
}

void AkaiFatLfnDirectory::unlinkEntry(AkaiFatLfnDirectoryEntry entry)
{
  if (entry.getName().startsWith(".") || entry.getName().length() == 0) return;

  const std::string lowerName = entry.getName().toLowerCase(Locale.ROOT);

  assert(akaiNameIndex.containsKey(lowerName));
  akaiNameIndex.remove(lowerName);

  assert(usedAkaiNames.contains(lowerName));
  usedAkaiNames.remove(lowerName);

  if (entry.isFile()) {
    entryToFile.remove(entry.realEntry);
  } else {
    entryToDirectory.remove(entry.realEntry);
  }
}

void AkaiFatLfnDirectory::linkEntry(AkaiFatLfnDirectoryEntry entry)
{
  checkUniqueName(entry.getName());
  entry.realEntry.setAkaiName(entry.getName());
  akaiNameIndex.put(entry.getName().toLowerCase(Locale.ROOT), entry);
}

void AkaiFatLfnDirectory::checkUniqueName(std::string name)
{
  const std::string lowerName = name.toLowerCase(Locale.ROOT);

  if (!usedAkaiNames.add(lowerName))
  {
    throw "an entry named " + name + " already exists";
  } else {
    usedAkaiNames.remove(lowerName);
  }
}

void AkaiFatLfnDirectory::parseLfn()
{
  int i = 0;
  const int size = dir.getEntryCount();

  while (i < size) {
    // jump over empty entries
    while (i < size && dir.getEntry(i) == null) {
      i++;
    }

    if (i >= size) {
      break;
    }

    int offset = i;

    while (dir.getEntry(i).isLfnEntry()) {
      i++;
      if (i >= size)
        break;
    }

    if (i >= size)
      break;

    const AkaiFatLfnDirectoryEntry current = AkaiFatLfnDirectoryEntry.extract(this, offset, ++i - offset);

    if (!current.realEntry.isDeleted() && current.isValid()) {
      checkUniqueName(current.getName());
      usedAkaiNames.add(current.getName().toLowerCase(Locale.ROOT));
      akaiNameIndex.put(current.getName().toLowerCase(Locale.ROOT), current);
    }
  }
}

void AkaiFatLfnDirectory::updateLFN()
{
      ArrayList<FatDirectoryEntry> dest =
              new ArrayList<FatDirectoryEntry>();

      for (AkaiFatLfnDirectoryEntry currentEntry : akaiNameIndex.values()) {
          dest.add(currentEntry.realEntry);
      }
      
      const int size = dest.size();

      dir.changeSize(size);
      dir.setEntries(dest);
  }

std::shared_ptr<ClusterChainDirectory> AkaiFatLfnDirectory::read(FatDirectoryEntry entry, Fat fat)
{
  if (!entry.isDirectory()) throw entry.getName() + " is no directory";

  const ClusterChain chain(fat, entry.getStartCluster(), entry.isReadonlyFlag());

  const ClusterChainDirectory result = std::make_shared<ClusterChainDirectory>(chain, false);

  result.read();
  return result;
}
