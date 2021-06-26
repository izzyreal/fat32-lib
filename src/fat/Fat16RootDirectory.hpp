#pragma once

#include "AbstractDirectory.hpp"

namespace akaifat::fat {
    class Fat16RootDirectory : public AbstractDirectory {
    private:
        BlockDevice *device;
        long deviceOffset;

        Fat16RootDirectory(Fat16BootSector *bs, bool readOnly)
                : AbstractDirectory(bs->getFatType(), bs->getRootDirEntryCount(), readOnly, true) {
            if (bs->getRootDirEntryCount() <= 0)
                throw std::runtime_error("root directory size is " + std::to_string(bs->getRootDirEntryCount()));

            deviceOffset = bs->getRootDirOffset();
            device = bs->getDevice();
        }

    protected:
        void read(ByteBuffer &data) override {
            device->read(deviceOffset, data);
        }

        void read() override { AbstractDirectory::read(); }

        void write(ByteBuffer &data) override {
            device->write(deviceOffset, data);
        }


        long getStorageCluster() override {
            return 0;
        }

        void changeSize(int entryCount) override {
            if (getCapacity() < entryCount) {
                throw std::runtime_error("directory full");
            }
        }

    public:
        static Fat16RootDirectory *read(
                Fat16BootSector *bs, bool readOnly) {
            auto result = new Fat16RootDirectory(bs, readOnly);
            result->read();
            return result;
        }

        static Fat16RootDirectory *create(
                Fat16BootSector *bs) {
            auto result = new Fat16RootDirectory(bs, false);
            result->flush();
            return result;
        }
    };
}
