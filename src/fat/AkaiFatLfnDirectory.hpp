#pragma once

#include "../AbstractFsObject.hpp"
#include "../FsDirectory.hpp"

#include "FatDirectoryEntry.hpp"
#include "ClusterChainDirectory.hpp"
#include "Fat.hpp"
#include "FatFile.hpp"
#include "AbstractDirectory.hpp"

#include <set>

namespace akaifat { class FsDirectoryEntry; }

namespace akaifat::fat {
class AkaiFatLfnDirectoryEntry;
class AkaiFatLfnDirectory : public akaifat::AbstractFsObject, public akaifat::FsDirectory
{
public:
	AbstractDirectory* dir;
    std::map<std::string, AkaiFatLfnDirectoryEntry*> akaiNameIndex;

	AkaiFatLfnDirectory(AbstractDirectory* dir, Fat* fat, bool readOnly);

	Fat* getFat();

	FatFile* getFile(FatDirectoryEntry* entry);

    AkaiFatLfnDirectory* getDirectory(FatDirectoryEntry* entry);

    akaifat::FsDirectoryEntry* addFile(std::string& name) override;

	bool isFreeName(std::string& name);

	static std::vector<std::string> splitName(std::string& s);

    akaifat::FsDirectoryEntry* addDirectory(std::string& name) override;

    akaifat::FsDirectoryEntry* getEntry(std::string& name) override;

	void flush() override;

	void remove(std::string& name) override;

	void unlinkEntry(AkaiFatLfnDirectoryEntry* entry);

	void linkEntry(AkaiFatLfnDirectoryEntry* entry);

private:
	std::set<std::string> usedAkaiNames;
	Fat* fat;
	std::map<FatDirectoryEntry*, FatFile*> entryToFile;
	std::map<FatDirectoryEntry*, AkaiFatLfnDirectory*> entryToDirectory;

	void checkUniqueName(std::string& name);

	void parseLfn();

	void updateLFN();

	static ClusterChainDirectory* read(FatDirectoryEntry*, Fat*);

    bool isDirValid() override;

    bool isDirReadOnly() override;
};
}
