namespace akaifat::fat {
class AkaiFatLfnDirectoryEntry : public AbstractFsObject, public FsDirectoryEntry
{
private:
	const FatDirectoryEntry realEntry;

	AkaiFatLfnDirectory parent;
	std::string fileName;

public:
	AkaiFatLfnDirectoryEntry(std::string name, AkaiFatLfnDirectory akaiFatLfnDirectory, bool directory)
  : AbstractFsObject (false), fileName (name), parent (akaiFatLfnDirectory)
  {
		realEntry = FatDirectoryEntry.create(akaiFatLfnDirectory.getFat().getFatType(), directory);
		realEntry.setAkaiName(name);
	}

	AkaiFatLfnDirectoryEntry(AkaiFatLfnDirectory akaiFatLfnDirectory, FatDirectoryEntry _realEntry, std::string _fileName)
  : AbstractFsObject(akaiFatLfnDirectory.isReadOnly()), parent (akaiFatLfnDirectory), realEntry (_realEntry), fileName (_fileName)
  {
	}

	static AkaiFatLfnDirectoryEntry extract(AkaiFatLfnDirectory& dir, int offset, int len)
  {
		const FatDirectoryEntry realEntry = dir.dir.getEntry(offset + len - 1);
		const std::string shortName = realEntry.getShortName().asSimplestd::string();
		const std::string akaiPart = AkaiPart.parse(realEntry.data).asSimplestd::string().trim();
		std::string part1 = AkaiFatLfnDirectory.splitName(shortName)[0].trim();
		std::string ext = AkaiFatLfnDirectory.splitName(shortName)[1].trim();
	
  	if (ext.length() > 0) ext = "." + ext;
	
  	std::string akaiFileName = part1 + akaiPart + ext;
	
  	return AkaiFatLfnDirectoryEntry(dir, realEntry, akaiFileName);
	}

	bool isHiddenFlag()
  {
		return realEntry.isHiddenFlag();
	}

	void setHiddenFlag(bool hidden)
  {
		checkWritable();
		realEntry.setHiddenFlag(hidden);
	}

	bool isSystemFlag()
  {
		return realEntry.isSystemFlag();
	}

	void setSystemFlag(bool systemEntry)
  {
		checkWritable();
		realEntry.setSystemFlag(systemEntry);
	}

	bool isReadOnlyFlag()
  {
		return realEntry.isReadonlyFlag();
	}

	void setReadOnlyFlag(bool readOnly)
  {
		checkWritable();
		realEntry.setReadonlyFlag(readOnly);
	}

	bool isArchiveFlag()
  {
		return realEntry.isArchiveFlag();
	}

  void setArchiveFlag(bool archive)
  {
		checkWritable();
		realEntry.setArchiveFlag(archive);
	}

	std::string getName() override
  {
		checkValid();

		return fileName;
	}

	std::string getAkaiPart()
  {
		if (isDirectory()) return "";
		return AkaiPart.parse(realEntry.data).asSimpleString();
	}

	void setAkaiPart(std::string s)
  {
		if (isDirectory()) return;
		AkaiPart ap = new AkaiPart(s);
		ap.write(realEntry.data);
	}

	FsDirectory getParent() override
  {
		checkValid();
		return parent;
	}

	void setName(std::string newName) throw (std::exception) override {
		checkWritable();

		if (!parent.isFreeName(newName)) {
			throw "the name \"" + newName + "\" is already in use";
		}

		parent.unlinkEntry(this);
		fileName = newName;
		parent.linkEntry(this);
	}

	void moveTo(AkaiFatLfnDirectory target, std::string newName) {

		checkWritable();

		if (!target.isFreeName(newName)) {
			throw "the name \"" + newName + "\" is already in use";
		}

		parent.unlinkEntry(this);
		parent = target;
		fileName = newName;
		parent.linkEntry(this);
	}

	FatFile getFile() throw (std::exception) override {
		return parent.getFile(realEntry);
	}

	AkaiFatLfnDirectory getDirectory() throw (std::exception) override {
		return parent.getDirectory(realEntry);
	}

	bool isFile() override {
		return realEntry.isFile();
	}

	bool isDirectory() override {
		return realEntry.isDirectory();
	}

	bool isDirty() override {
		return realEntry.isDirty();
	}

};
}
