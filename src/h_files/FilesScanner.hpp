#pragma once
#include <string>
#include <map>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include <regex>
#include "../../include/types.hpp"

/**
 * @class FilesScanner
 *
 * @details This class is designed for high-performance file scanning. It uses a hybrid approach
 * for filtering: exact names and extensions are checked in O(1) time using hash sets, while
 * complex wildcard patterns fallback to a regex engine. It automatically respects files like
 * .gitignore and provides built-in defaults for common junk files.
 */
class FilesScanner
{
private:
    std::filesystem::path RootPath;
    std::vector<FileInfo> Files;

    // O(1) lookup sets for exact matches (fast path)
    std::unordered_set<std::string> ExactNames;
    std::unordered_set<std::string> ExactExtensions;

    // Fallback list for complex glob patterns that require regex (slow path)
    std::vector<std::regex> PatternRules;

    /**
     * @details Opens the specified file from the root directory, reads it line by line,
     * strips out whitespaces and comments (lines starting with '#'), and passes every valid
     * pattern to the GlobToRegex() method for classification.
     *
     * @param FileName The name of the ignore file to load (e.g., ".gitignore", ".dockerignore").
     */
    void LoadIgnoreFile(std::string &FileName);

    /**
     * @details This function acts as a dispatcher. It analyzes the given pattern:
     * - If it's a simple extension (e.g., "*.cpp"), it strips the "*." and adds it to ExactExtensions.
     * - If it's an exact name (e.g., "build/"), it adds it to ExactNames.
     * - If it contains complex wildcards (e.g., "src/*_test.cpp"), it translates the glob syntax
     *   into standard regex syntax and stores it in PatternRules.
     *
     * @param pattern A single rule line extracted from an ignore file or default fallbacks.
     */
    void GlobToRegex(const std::string &pattern);

    /**
     * @details Populates the ignore engine with universally ignored patterns (like .git,
     * node_modules) and compiled binaries (*.o, *.exe). This acts as a safety net, ensuring
     * the scanner doesn't waste time on junk files even if a .gitignore file is missing.
     */
    void InitDefaultFallbacks();

    /**
     * @details The function first checks the file's stem and extension against the O(1)
     * unordered_sets for maximum performance. If no exact match is found, it calculates the
     * relative path and checks it against the slower regex patterns as a last resort.
     *
     * @param path The directory entry (file or folder) currently being evaluated.
     * @return true if the path matches any ignore rule, false if it's safe to scan.
     */
    bool IsIgnoredPath(const std::filesystem::directory_entry &path);

public:
    /**
     * @details Initializes the root path. It will throw an exception (throw 1) if the
     * provided path does not exist on the disk or is not a valid directory.
     *
     * @param path The absolute or relative path to the root directory to be scanned.
     */
    explicit FilesScanner(const std::string &path);

    /**
     * @details Iterates through all files and directories starting from RootPath.
     * It uses is_ignored_path() to prune branches (like ignored folders) early.
     * Valid files are parsed into FileInfo structs and saved internally.
     */
    void scan();

    /**
     * @details Provides read-only access to the internal vector of FileInfo structures
     * that was populated during the scan() operation.
     *
     * @return A constant vector containing metadata for all valid files found.
     */
    const std::vector<FileInfo> getFiles();
};