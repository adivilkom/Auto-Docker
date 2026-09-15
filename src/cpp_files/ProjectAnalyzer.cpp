#include "../h_files/ProjectAnalyzer.hpp"
#include <fstream>
#include <string>
#include <map>
#include <filesystem>
#include <vector>
#include "../../include/types.hpp"

ProjectAnalyzer::ProjectAnalyzer(std::vector<FileInfo> &incomingFiles)
{
    files = incomingFiles;
    // creat instans of the language analyzer
    // then insert them in to analyzers
}

void ProjectAnalyzer::analyzeAllFiles()
{
    for (int i = 0; i < files.size(); i++)
    {
        FileInfo file = files[i];
        if (file.kind == FileKind::Source || file.kind == FileKind::Header)
        {
            switch (file.language)
            {
            case Language::Cpp:
            case Language::C:
                analyzers[0];
                break;
            // add more language

            default:
            break;
            }
        }
        
    }
    
}
