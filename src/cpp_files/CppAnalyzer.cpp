#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include <cctype>
#include <sstream>
#include "../h_files/CppAnalyzer.hpp"
#include "../../include/types.hpp"

#include <string_view>
#include <unordered_set>

inline const std::unordered_set<std::string_view> CppStandardHeaders = {
    "cfloat", "climits", "compare", "contracts", "coroutine", "csetjmp", "csignal", "cstdarg",
    "cstddef", "cstdint", "cstdlib", "exception", "initializer_list", "limits", "new", "source_location",
    "stdfloat", "typeinfo", "version", "concepts", "cassert", "cerrno", "debugging", "stacktrace",
    "stdexcept", "system_error", "memory", "memory_resource", "scoped_allocator", "meta", "ratio",
    "type_traits", "any", "bitset", "bit", "charconv", "expected", "format", "functional", "optional",
    "tuple", "utility", "variant", "array", "deque", "flat_map", "flat_set", "forward_list", "hive",
    "inplace_vector", "list", "map", "mdspan", "queue", "set", "span", "stack", "unordered_map", "unordered_set",
    "vector", "iterator", "generator", "ranges", "algorithm", "numeric", "cctype", "cstring", "cuchar",
    "cwchar", "cwctype", "string_view", "string", "clocale", "codecvt", "locale", "regex", "text_encoding",
    "cfenv", "cmath", "complex", "linalg", "numbers", "random", "simd", "stdckdint.h", "valarray",
    "chrono", "ctime", "cinttypes", "cstdio", "filesystem", "fstream", "iomanip", "iosfwd", "iostream",
    "ios", "istream", "ostream", "print", "spanstream", "sstream", "streambuf", "strstream", "syncstream",
    "atomic", "barrier", "condition_variable", "future", "hazard_pointer", "latch", "mutex", "rcu",
    "semaphore", "shared_mutex", "stdatomic.h", "stop_token", "thread", "execution"};

FileAnalysis CppAnalyzer::analyze(const FileInfo &info)
{
    FileAnalysis analysis;
    analysis.fileId = info.id;
    relativeLocation = std::filesystem::relative(info.path, absoluteLocation).string();
    std::ifstream file(info.path);
    if (!file.is_open())
    {
        return analysis;
    }
    std::string line;
    while (getline(file, line))
    {
        line = CleanLine(line);
        auto result1 = reference(line);
        if (result1)
        {
            analysis.dependencies.push_back(result1);
        }
        auto result2 = requirement(line);
        if (result2)
        {
            analysis.dockerRequirements.push_back(result2);
        }
    }
}

std::string CppAnalyzer::CleanLine(const std::string &line)
{
    std::string result;
    int size = line.size();
    result.reserve(size);
    bool in_single_comment = false;
    bool in_multi_comment = false;
    char in_string = '\0';
    bool in_whitespace = false;

    for (size_t i = 0; i < size; i++)
    {
        char c = line[i];
        if (in_single_comment)
        {
            if (c == '\n')
            {
                in_single_comment = false;
            }
            continue;
        }
        if (in_multi_comment)
        {
            if (c == '*' && i + 1 < line.size() && line[i + 1] == '/')
            {
                in_multi_comment = false;
                ++i;
            }
            continue;
        }
        if (in_string != '\0')
        {
            result += c;
            if (c == in_string && line[i - 1] != '\\')
                in_string = '\0';
            continue;
        }

        if (c == '"' || c == '\'')
        {
            in_string = c;
            result += c;
            continue;
        }
        if (c == '/' && i + 1 < size)
        {
            if (line[i + 1] == '/')
            {
                in_single_comment = true;
                i++;
                continue;
            }
            if (line[i + 1] == '*')
            {
                in_multi_comment = true;
                i++;
                continue;
            }
        }
        if (std::isspace(static_cast<unsigned char>(c)))
        {
            if (!in_whitespace && !result.empty())
            {
                result += ' ';
                in_whitespace = true;
            }
            continue;
        }
        in_whitespace = false;
        result += c;
    }

    return result;
}

std::optional<DependencyReference> CppAnalyzer::reference(const std::string &line)
{
    DependencyReference result;
    size_t StartInclude = line.find("include");
    size_t StartImport = line.find("import");
    if (StartInclude == 1)
    {
        size_t StartQuote = line.find('"', StartInclude + 6);
        if (StartQuote != std::string::npos)
        {
            size_t EndQuote = line.find('"', StartQuote + 1);
            if (EndQuote != std::string::npos)
            {
                result.value = convertPathToId(line.substr(StartQuote, EndQuote - StartQuote + 1));
                result.kind = DependencyKind::LocalFile;
                return result;
            }
            // need to throw error
        }
        size_t StartAngle = line.find('<', StartInclude);
        if (StartAngle != std::string::npos)
        {
            size_t EndAngle = line.find('>', StartInclude + 1);
            if (EndAngle != std::string::npos)
            {
                result.value = line.substr(StartAngle, EndAngle - StartAngle + 1);
                if (CppStandardHeaders.find(result.value) != CppStandardHeaders.end())
                {
                    result.kind = DependencyKind::SystemHeader;
                    return result;
                }
                result.kind = DependencyKind::ExternalPackage;
                return result;
            }
        }
    }
    else if (StartImport == 0 || StartImport == 7)
    {
        size_t StartName = line.find_first_not_of(" \t", StartImport + 6);
        size_t EndName = line.find(';', StartName);

        if (StartName != std::string::npos && EndName != std::string::npos)
        {
            std::string ModuleName = line.substr(StartName, EndName - StartName + 1);

            result.value = ModuleName;
            if (line[StartName] == '<')
            {
                result.value = convertPathToId(ModuleName.substr(1, ModuleName.size() - 2));
                result.kind = DependencyKind::LocalFile;
                return result;
            }
            else
            {
                if (CppStandardHeaders.find(result.value) != CppStandardHeaders.end())
                {
                    result.kind = DependencyKind::SystemHeader;
                    return result;
                }
                result.kind = DependencyKind::ExternalPackage;
                return result;
            }
        }
    }
    return std::nullopt;
}

std::optional<DockerRequirement> CppAnalyzer::requirement(const std::string &line)
{
}

int CppAnalyzer::convertPathToId(const std::string &line)
{
    std::filesystem::path full = std::filesystem::path(relativeLocation) / line;
    std::string normalizedPath = resolverLocalFile(line);
    auto it = PathToId.find(normalizedPath);
    if (it != PathToId.end())
    {
        return it->second;
    }
    return -1; // Return -1 if the path is not found
}

std::string CppAnalyzer::resolverLocalFile(const std::string &line)
{
    std::filesystem::path full = std::filesystem::path(relativeLocation) / line;
    return full.lexically_normal().string();
}