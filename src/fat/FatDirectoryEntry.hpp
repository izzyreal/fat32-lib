#pragma once

#include "../AbstractFsObject.hpp"

#include "AbstractDirectory.hpp"
#include "../ByteBuffer.hpp"
#include "LittleEndian.hpp"
#include "ShortName.hpp"
#include "FatType.hpp"

#include <utility>
#include <vector>

using namespace akaifat;

namespace akaifat::fat {
    class FatDirectoryEntry : public AbstractFsObject {

    private:
        FatType *type;
        bool dirty{};
        static const int OFFSET_ATTRIBUTES = 0x0b;
        static const int OFFSET_FILE_SIZE = 0x1c;
        static const int F_READONLY = 0x01;
        static const int F_HIDDEN = 0x02;
        static const int F_SYSTEM = 0x04;
        static const int F_VOLUME_ID = 0x08;
        static const int F_DIRECTORY = 0x10;
        static const int F_ARCHIVE = 0x20;

    public:
        explicit FatDirectoryEntry(FatType *fs)
                : FatDirectoryEntry(fs, std::vector<char>(SIZE), false) {
        }

        FatDirectoryEntry(FatType *fs, std::vector<char> _data, bool readOnly)
                : AbstractFsObject(readOnly), type(fs), data(std::move(_data)) {
        }

        void setFlag(int mask, bool set) {
            int oldFlags = getFlags();

            if (((oldFlags & mask) != 0) == set) return;

            if (set) {
                setFlags(oldFlags | mask);
            } else {
                setFlags(oldFlags & ~mask);
            }

            dirty = true;
        }

        int getFlags() {
            return LittleEndian::getUInt8(data, OFFSET_ATTRIBUTES);
        }

        void setFlags(int flags) {
            LittleEndian::setInt8(data, OFFSET_ATTRIBUTES, flags);
        }

        std::vector<char> data;

        static int const SIZE = 32;
        static int const ENTRY_DELETED_MAGIC = 0xe5;

        static FatDirectoryEntry *read(
                FatType *type, ByteBuffer &buff, bool readOnly) {

            assert (buff.remaining() >= SIZE);

            std::vector<char> data(SIZE);
            buff.get(data);
            return new FatDirectoryEntry(type, data, readOnly);
        }

        static void writeNullEntry(ByteBuffer buff) {
            for (int i = 0; i < SIZE; i++) {
                buff.put((char) 0);
            }
        }

        bool isVolumeLabel() {
            if (isLfnEntry()) return false;
            else return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == F_VOLUME_ID);
        }

        bool isSystemFlag() {
            return ((getFlags() & F_SYSTEM) != 0);
        }

        void setSystemFlag(bool isSystem) {
            setFlag(F_SYSTEM, isSystem);
        }

        bool isArchiveFlag() {
            return ((getFlags() & F_ARCHIVE) != 0);
        }

        void setArchiveFlag(bool isArchive) {
            setFlag(F_ARCHIVE, isArchive);
        }

        bool isHiddenFlag() {
            return ((getFlags() & F_HIDDEN) != 0);
        }

        void setHiddenFlag(bool isHidden) {
            setFlag(F_HIDDEN, isHidden);
        }

        bool isVolumeIdFlag() {
            return ((getFlags() & F_VOLUME_ID) != 0);
        }

        bool isReadonlyFlag() {
            return ((getFlags() & F_READONLY) != 0);
        }

        bool isLfnEntry() {
            return isReadonlyFlag() && isSystemFlag() &&
                   isHiddenFlag() && isVolumeIdFlag();
        }

        bool isDirty() {
            return dirty;
        }

        bool isDirectory() {
            return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == F_DIRECTORY);
        }

        static FatDirectoryEntry *create(FatType *type, bool directory) {
            auto result = new FatDirectoryEntry(type);

            if (directory) {
                result->setFlags(F_DIRECTORY);
            }

            return result;
        }

        static FatDirectoryEntry *createVolumeLabel(
                FatType *type, const std::string &volumeLabel) {

            assert(volumeLabel.length() != 0);

            std::vector<char> data(SIZE);

            for (int i = 0; i < volumeLabel.length(); i++)
                data[i] = volumeLabel[i];

            auto result = new FatDirectoryEntry(type, data, false);
            result->setFlags(FatDirectoryEntry::F_VOLUME_ID);
            return result;
        }

        std::string getVolumeLabel() {
            if (!isVolumeLabel())
                throw std::runtime_error("not a volume label");

            std::string result;

            for (int i = 0; i < AbstractDirectory::MAX_LABEL_LENGTH; i++) {
                auto b = data[i];

                if (b != 0) {
                    result.push_back((char) b);
                } else {
                    break;
                }
            }

            return result;
        }

        bool isDeleted() {
            return (LittleEndian::getUInt8(data, 0) == ENTRY_DELETED_MAGIC);
        }

        long getLength() {
            return LittleEndian::getUInt32(data, OFFSET_FILE_SIZE);
        }

        void setLength(long length) {
            LittleEndian::setInt32(data, OFFSET_FILE_SIZE, length);
        }

        ShortName getShortName() {
            if (data[0] == 0) {
                throw std::runtime_error("data[0] == 0 so there's no ShortName");
            } else {
                return ShortName::parse(data);
            }
        }

        bool isFile() {
            return ((getFlags() & (F_DIRECTORY | F_VOLUME_ID)) == 0);
        }

        void setShortName(ShortName &sn) {
            if (sn.equals(getShortName())) return;

            sn.write(data);
            dirty = true;
        }

        void setAkaiName(std::string s) {
//    	std::string part1 = AkaiFatLfnDirectory::splitName(s)[0];
//    	std::string part2 = "        ";
//    	std::string ext = AkaiFatLfnDirectory::splitName(s)[1];
//    	if (part1.length() > 8) {
//    		part2 = part1.substr(8);
//    		part1 = part1.substr(0, 8);
//    	}
//    	if (ext.length() > 0) ext = "."+ ext;
//    	ShortName sn = new ShortName(part1 + ext);
//    	sn.write(data);
//    	AkaiPart ap = new AkaiPart(part2);
//    	ap.write(data);
        }

        long getStartCluster() {
            return LittleEndian::getUInt16(data, 0x1a);
        }

        void setStartCluster(long startCluster) {
            if (startCluster > INT_MAX)
                throw std::runtime_error("startCluster too big");

            LittleEndian::setInt16(data, 0x1a, (int) startCluster);
        }

        void write(ByteBuffer &buff) {
            buff.put(data);
            dirty = false;
        }

        void setReadonlyFlag(bool isReadonly) {
            setFlag(F_READONLY, isReadonly);
        }
    };
}
