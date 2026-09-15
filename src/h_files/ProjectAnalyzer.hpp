#pragma once
#include <string>
#include <map>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include <regex>
#include "../../include/types.hpp"
#include "IFileAnalyzer.hpp"

class ProjectAnalyzer
{
private:
    std::vector<FileInfo> files;
    std::vector<std::unique_ptr<IFileAnalyzer>> analyzers;
    std::vector<FileAnalysis> analysisResults;

public:
    ProjectAnalyzer(std::vector<FileInfo> &incomingFiles);
    void analyzeAllFiles();
    FileAnalysis &getAnalysis(int id);
    std::vector<FileAnalysis> getAllAnalysis();
};
