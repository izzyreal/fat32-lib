#pragma once

#include "FsObject.hpp"

#include "FsDirectory.hpp"
#include "FsFile.hpp"

#include <exception>

namespace akaifat {
class FsDirectoryEntry : public FsObject {
public:
//    const static Comparator<FsDirectoryEntry> DIRECTORY_ENTRY_COMPARATOR =
//            new Comparator<FsDirectoryEntry>() {
//
//        @Override
//        int compare(FsDirectoryEntry e1, FsDirectoryEntry e2) {
//            if (e2.isDirectory() == e1.isDirectory()) {
//                /* compare names */
//                return e1.getName().compareTo(e2.getName());
//            } else {
//                if (e2.isDirectory()) return 1;
//                else return -1;
//            }
//        }
//    };
    
    virtual std::string getName() = 0;

    virtual FsDirectory getParent() = 0;

    virtual bool isFile() = 0;

    virtual bool isDirectory() = 0;

    virtual void setName(std::string newName) = 0;

    virtual FsFile getFile() = 0;

    virtual FsDirectory getDirectory() = 0;

    virtual bool isDirty() = 0;
};
}
