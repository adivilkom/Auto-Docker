#pragma once
#include <string>
#include <vector>

enum class Languages
{
    Python,
    C,
    Cpp,
    Java
};

struct FileInfo
{
    std::string path;
    std::string extension;
    std::string stem;
    std::vector<std::string> dependencies;

};