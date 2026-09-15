#pragma once
#include <string>
#include <map>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include "IFileAnalyzer.hpp"
#include "../../include/types.hpp"

class CppAnalyzer : public IFileAnalyzer
{
private:
    std::string relativeLocation;
    std::optional<DependencyReference> reference(const std::string &line) override;
    std::optional<DockerRequirement> requirement(const std::string &line) override;
    std::string CppAnalyzer::resolverLocalFile(const std::string &line);
    int convertPathToId(const std::string &line);
    std::string CleanLine(const std::string &line) override;

public:
    FileAnalysis analyze(const FileInfo &info) override;
};
