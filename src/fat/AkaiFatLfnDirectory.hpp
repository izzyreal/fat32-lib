namespace akaifat::fat {
class AkaiFatLfnDirectory : public AbstractFsObject, public FsDirectory
{
public:
	const AbstractDirectory dir;

	AkaiFatLfnDirectory(AbstractDirectory dir, Fat fat, bool readOnly);

	Fat getFat();

	FatFile getFile(FatDirectoryEntry entry) throw (std::exception);

	AkaiFatLfnDirectory getDirectory(FatDirectoryEntry entry) throw (std::exception);

	AkaiFatLfnDirectoryEntry addFile(std::string name) throw (std::exception) override;

	bool isFreeName(std::string name);

	static std::string[] splitName(std::string s);

	AkaiFatLfnDirectoryEntry addDirectory(std::string name) throw (std::exception) override;

	AkaiFatLfnDirectoryEntry getEntry(std::string name) override;

	void flush() throw (std::exception) override;

	Iterator<FsDirectoryEntry> iterator() override;

	void remove(std::string name) throw (std::exception, std::illegal_argument) override;

	void unlinkEntry(AkaiFatLfnDirectoryEntry entry);

	void linkEntry(AkaiFatLfnDirectoryEntry entry) throw (std::exception);

private:
	const std::set<std::string> usedAkaiNames;
	const Fat& fat;
	const Map<std::string, AkaiFatLfnDirectoryEntry> akaiNameIndex;
	const Map<FatDirectoryEntry, FatFile> entryToFile;
	const Map<FatDirectoryEntry, AkaiFatLfnDirectory> entryToDirectory;

	void checkUniqueName(std::string name) throw (std::exception);

	void parseLfn() throw (std::exception);

	void updateLFN() throw (std::exception);

	static std::shared_ptr<ClusterChainDirectory> read(FatDirectoryEntry entry, Fat fat) throw (std::exception);
};
}
