cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_COMPILER C:/MinGW/mingw64/bin/g++.exe)

project(WGA_Texture_Editor
        VERSION 2.0.0
        LANGUAGES CXX)

include_directories("./vendor/OpenCL-SDK/include")
link_directories("./vendor/OpenCL-SDK/lib")
link_libraries("OpenCL")
add_subdirectory(src)



