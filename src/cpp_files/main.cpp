#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <functional>
#include "../h_files/ProjectScanner.hpp"
#include "../h_files/DependencyResolverC_Cpp.hpp"
#include "../h_files/DependencyExtractorC_Cpp.hpp"
#include "../include/types.hpp"
#include <chrono>

int main(int argc, char *argv[])
{
    auto start_time = std::chrono::high_resolution_clock::now();
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <path_to_scan>" << std::endl;
        return 1;
    }
    absoluteLocation = argv[1];

    ProjectScanner scanner(argv[1]);
    DependencyExtractorC_Cpp Extractor;
    DependencyResolverC_Cpp Resolver;

    std::cout << "Initialized successfully" << std::endl;
    scanner.scan();

    std::cout << "Now Extracting..." << std::endl;
    std::vector<RawDependency> dep;
    for (const auto &fileInfo : IdToInfo)
    {
        std::vector<RawDependency> helper = Extractor.extract(fileInfo);
        dep.insert(dep.end(), helper.begin(), helper.end());
    }

    std::cout << "Now Resolving..." << std::endl;
    std::vector<Dependency> resolvedDeps = Resolver.resolver(dep);

    std::cout << "\n=== File Dependencies Report ===" << std::endl;
    for (const auto &fileInfo : IdToInfo)
    {
        std::cout << "File Path: " << fileInfo.path
                  << " | ID: " << fileInfo.id << std::endl;
        std::cout << "Points to dependencies:" << std::endl;

        bool hasDeps = false;
        for (const auto &d : resolvedDeps)
        {
            if (d.SourceFileId == fileInfo.id)
            {
                std::cout << "  -> " << d.value
                          << " (Type: " << static_cast<int>(d.type) << ")" << std::endl;
                hasDeps = true;
            }
        }

        if (!hasDeps)
        {
            std::cout << "  (None)" << std::endl;
        }
        std::cout << "-----------------------------------" << std::endl;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "\nTotal execution time: " << duration_ms << " ms" << std::endl;

    return 0;
}