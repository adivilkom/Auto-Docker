#include "../h_files/FilesScanner.hpp"
#include <fstream>
#include <string>
#include <map>
#include <filesystem>
#include <vector>

#include <iostream>

#include "../../include/types.hpp"

FilesScanner::FilesScanner(const std::string &Path)
{
    RootPath = std::filesystem::path(Path);
    if (!std::filesystem::exists(RootPath) || !std::filesystem::is_directory(RootPath) || std::filesystem::is_block_file(RootPath))
    {
        throw(1);
    }
    InitDefaultFallbacks();
    std::string gitignore = ".gitignore";
    LoadIgnoreFile(gitignore);
    std::string dockerignore = ".dockerignore";
    LoadIgnoreFile(dockerignore);
}

void FilesScanner::scan()
{
    for (const auto &entry : std::filesystem::recursive_directory_iterator(RootPath))
    {
        if (IsIgnoredPath(entry))
        {
            continue;
        }

        FileInfo Info;
        Info.path = entry.path().string();
        Info.extension = entry.path().extension().string();
        Info.stem = entry.path().stem().string();
        Files.push_back(Info);
    }
}

bool FilesScanner::IsIgnoredPath(const std::filesystem::directory_entry &path)
{
    if (!path.exists() || !path.is_regular_file() && path.is_directory())
    {
        return true;
    }

    std::string rel_path = std::filesystem::relative(path.path(), RootPath).generic_string();
    std::string name = path.path().filename().string();
    std::string extension = path.path().extension().string();

    // 1. Check if the exact name (file or directory) matches O(1) sets
    if (ExactNames.find(name) != ExactNames.end())
    {
        return true;
    }

    // 2. If it's a regular file, check its extension in O(1) time
    if (path.is_regular_file() && !path.is_block_file() && !path.is_symlink())
    {
        if (ExactExtensions.find(extension) != ExactExtensions.end())
        {
            return true;
        }
    }

    // 3. Check against all regex pattern rules (works for both files and directories)
    for (const auto &pattern : PatternRules)
    {
        if (std::regex_match(rel_path, pattern))
        {
            return true;
        }
    }

    return false;
}

void FilesScanner::InitDefaultFallbacks()
{
    static const std::vector<std::string> defaults = {
        "*.git*", "*node_modules*", "*build*", "*dist*",
        "*.o", "*.so", "*.dylib", "*.exe", "*.bin", "*.zip"};
    for (const auto &pattern : defaults)
    {
        GlobToRegex(pattern);
    }
}

void FilesScanner::LoadIgnoreFile(std::string &FileName)
{
    auto IgnorPath = RootPath / FileName;
    if (!std::filesystem::exists(IgnorPath) || std::filesystem::is_block_file(IgnorPath))
    {
        return;
    }
    std::ifstream file(IgnorPath);
    std::string line;
    while (getline(file, line))
    {
        // Strip leading and trailing whitespace and ignore comments
        size_t first = line.find_first_not_of(" \t\r\n");
        // If the line is empty or a comment, skip it
        if (first == std::string::npos || line[first] == '#')
        {
            continue;
        }
        // Strip trailing whitespace
        size_t last = line.find_last_not_of(" \t\r\n");
        std::string pattern = line.substr(first, last - first + 1);
        GlobToRegex(pattern);
    }
}

void FilesScanner::GlobToRegex(const std::string &pattern)
{
    // Check for exact extension match for patterns like "*.ext" and add to ExactExtensions
    if (pattern.rfind("*.", 0) == 0 && pattern.find_first_of("*?/", 2) == std::string::npos)
    {
        std::string ext = pattern.substr(1);
        ExactExtensions.insert(ext);
        return;
    }
    // Check for exact name match for patterns without wildcards and add to ExactNames, patterns like "filename".
    if (pattern.find_first_of("*?[]/") == std::string::npos)
    {
        std::string clean_name = pattern;
        if (!clean_name.empty() && clean_name.back() == '/')
        {
            clean_name.pop_back();
        }
        ExactNames.insert(clean_name);
        return;
    }

    std::string regex_str = "^";
    for (char c : pattern)
    {
        switch (c)
        {
        case '*':
            regex_str += ".*";
            break;
        case '?':
            regex_str += ".";
            break;
        case '.':
            regex_str += "\\.";
            break;
        case '/':
            regex_str += "[/\\\\]";
            break;
        default:
            if (std::isalnum(c) || c == '_' || c == '-')
            {
                regex_str += c;
            }
            else
            {
                regex_str += "\\";
                regex_str += c;
            }
        }
    }
    if (pattern.back() == '/')
    {
        regex_str += ".*";
    }
    else
    {
        regex_str += "$";
    }
    PatternRules.push_back(std::regex(regex_str, std::regex::icase));
}

const std::vector<FileInfo> FilesScanner::getFiles()
{
    return Files;
}
