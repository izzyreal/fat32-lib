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
        if (e.second->getName().length() == 0 || e.second->getName().length() == 1) {
            printf("");
        }
        printf("Name: %s\n", e.second->getName().c_str());
    }

    img.close();

    return 0;
}
