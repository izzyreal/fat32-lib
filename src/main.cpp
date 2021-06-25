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
        auto fat = root->getFat();
        auto file = root->iterator();
        auto name = file->second->getName();
        printf("Name: %s\n", name.c_str());
    }
    else
    {
        printf("root is nullptr!\n");
    }

    img.close();

    return 0;
}
