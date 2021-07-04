#include <iostream>

#include "ImageBlockDevice.hpp"
#include "FileSystemFactory.hpp"

#include "fat/AkaiFatLfnDirectoryEntry.hpp"

using namespace akaifat;
using namespace akaifat::fat;

int main() {
    std::fstream img;
    img.open("/Users/izmar/Desktop/fat16.img", std::ios_base::in | std::ios_base::out);

    img.seekg(std::ios::beg);

    ImageBlockDevice device(img);
    auto fs = dynamic_cast<AkaiFatFileSystem *>(FileSystemFactory::createAkai(&device, false));

    auto root = fs->getRoot();

    auto bs = dynamic_cast<Fat16BootSector *>(fs->getBootSector());

    auto volumeLabel = bs->getVolumeLabel();

    auto entries = root->akaiNameIndex;

    printf("- %s ROOT LISTING -\n", StrUtil::trim_copy(volumeLabel).c_str());

    for (auto &e : entries)
        printf("Name: %s\n", e.first.c_str());

    auto test1 = entries["test1"];

    auto dir = dynamic_cast<AkaiFatLfnDirectory *>(test1->getDirectory());
    printf("- TEST1 LISTING -\n");
    auto dirEntries = dir->akaiNameIndex;

    for (auto &e : dirEntries) {

        printf("Name: %s\n", e.second->getName().c_str());
        if (e.second->isFile())
        {
            auto length = e.second->getFile()->getLength();
            printf("Length: %li\n", length);
        }
        if (e.second->getName() == "SNARE4.SND") {
//            ByteBuffer newData(100);
//            for (int i = 0; i < newData.capacity(); i++)
//                newData.put((char)(i));
//            newData.flip();
//            e.second->getFile()->setLength(100);
//            e.second->getFile()->write(0, newData);
//            e.second->getFile()->flush();
            printf("SNARE4.SND data:");
            ByteBuffer buf(e.second->getFile()->getLength());
            e.second->getFile()->read(0, buf);
            buf.rewind();
            for (int i = 0; i < 100; i++)
                printf("%c", buf.get());
            printf("\n");

//            std::fstream output;
//            output.open("/Users/izmar/Desktop/SNARE5.SND", std::ios_base::out | std::ios_base::binary);
//            auto buf_ = buf.getBuffer();
//            output.write(&buf_[0], buf.capacity());
//            output.close();
        }
    }

//    std::string newDirName = "aaaa";
//    root->addDirectory(newDirName);


//    std::string toRemove = "snare4.snd";
//    dir->remove(toRemove);
//    std::string newFileName = "SNARE4.SND";
//    dir->addFile(newFileName);

    fs->flush();

    img.close();

    return 0;
}
