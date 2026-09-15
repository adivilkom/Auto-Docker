cmake_minimum_required(VERSION 3.15)
project(Auto-Docker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(Auto-Docker
    src/cpp_files/main.cpp
    src/cpp_files/FilesScanner.cpp
    src/cpp_files/DependencyExtractorC_Cpp.cpp
    src/cpp_files/DependencyResolverC_Cpp.cpp
    include/types.hpp

)