#include <iostream>

#include "ImageBlockDevice.hpp"
#include "FileSystemFactory.hpp"

using namespace akaifat;

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::fstream img;
    img.open("/Users/izmarverhage/Desktop/fat16.dmg", std::ios_base::in | std::ios_base::out | std::ios_base::app);

    img.seekg(std::ios::beg);

    ImageBlockDevice device(img);
    auto fs = FileSystemFactory::createAkai(&device, false);

    img.close();

    return 0;
}
