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
    auto fs = dynamic_cast<AkaiFatFileSystem*>(FileSystemFactory::createAkai(&device, false));

    auto root = fs->getRoot();

    if (root != nullptr) {
        auto entries = root->akaiNameIndex;
        for (auto& e : entries)
            printf("Name: %s\n", e.first.c_str());
        auto test1 = entries["test1"];
        if (test1->isDirectory()) {
            printf("test1 is a directory\n");
        } else {
            printf("test1 is NOT a directory\n");
        }

        auto dir = test1->getDirectory();
//        auto dirEntries = dir->akaiNameIndex;

//        for (auto& e : dirEntries)
//            printf("Name1: %s\n", e.second->getName().c_str());

    }
    else
    {
        printf("root is nullptr!\n");
    }

    img.close();

    return 0;
}
