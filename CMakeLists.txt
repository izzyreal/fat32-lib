cmake_minimum_required(VERSION 3.19)
project(fat32_lib_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(fat32_lib_cpp

        src/main.cpp

        src/fat/AbstractDirectory.cpp
        src/fat/AkaiFatFileSystem.cpp
        src/fat/AkaiFatLfnDirectory.cpp
        src/fat/BootSector.cpp
        src/fat/FatLfnDirectoryEntry.cpp
        )