#pragma once
#include <string>
#include <map>
#include <filesystem>
#include <vector>
#include "../../include/types.hpp"

class IFileAnalyzer
{
private:
    virtual std::optional<DependencyReference> reference(const std::string &line) = 0;
    virtual std::optional<DockerRequirement> requirement(const std::string &line) = 0;
    virtual std::string CleanLine(const std::string &line) = 0;

public : virtual ~IFileAnalyzer() = default;
    virtual FileAnalysis analyze(const FileInfo &info) = 0;
};