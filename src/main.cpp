#include <iostream>

#include "ImageBlockDevice.hpp"
#include "FileSystemFactory.hpp"

#include "fat/AkaiFatLfnDirectoryEntry.hpp"

using namespace akaifat;
using namespace akaifat::fat;

int main() {
    std::fstream img;
    img.open("/Users/izmar/Desktop/fat16.img", std::ios_base::in | std::ios_base::out | std::ios_base::app);

    img.seekg(std::ios::beg);

    ImageBlockDevice device(img);
    auto fs = dynamic_cast<AkaiFatFileSystem *>(FileSystemFactory::createAkai(&device, false));

    auto root = fs->getRoot();

    auto entries = root->akaiNameIndex;

    printf("- ROOT LISTING -\n");

    for (auto &e : entries)
        printf("Name: %s\n", e.first.c_str());

    auto test1 = entries["test1"];

    auto dir = dynamic_cast<AkaiFatLfnDirectory *>(test1->getDirectory());
    auto dirEntries = dir->akaiNameIndex;

    printf("- TEST1 LISTING -\n");

    for (auto &e : dirEntries)
    {
        printf("Name: %s\n", e.second->getName().c_str());
        if (e.second->getName() == "SNARE4.SND") {
            printf("SNARE4.SND data:");
            ByteBuffer buf(e.second->getFile()->getLength());
            e.second->getFile()->read(0, buf);
            buf.rewind();
            for (int i = 0; i < 500; i++)
                printf("%c", buf.get());
            printf("\n");
        }
    }

    img.close();

    return 0;
}
