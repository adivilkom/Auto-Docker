#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <functional>

// ============================================================
// Language
// ============================================================
// Describes the programming language used by a source file.
enum class Language
{
    C,
    Cpp,
    Java,
    Python,
    JavaScript,
    TypeScript,
    Unknown
};

// ============================================================
// FileKind
// ============================================================
//
// Describes the role of a file inside the project.
enum class FileKind
{
    Source,
    Header,
    BuildConfiguration,
    DependencyConfiguration,
    DockerConfiguration,
    Unknown
};

// ============================================================
// DependencyKind
// ============================================================
//
// Describes what kind of dependency/reference was discovered.
enum class DependencyKind
{
    LocalFile,
    ExternalPackage,
    SystemHeader,
    Unknown
};

// ============================================================
// FileInfo
// ============================================================
//
// Contains basic metadata about a file discovered by ProjectScanner.
struct FileInfo
{
    int id;
    std::filesystem::path path;
    Language language;
    FileKind kind;
};

// RawDependency
// ============================================================
//
// Represents a dependency/reference exactly as it was extracted from the source file,
// before trying to resolve it to another file in the repository.
struct DependencyReference
{
    std::string value;
    DependencyKind kind;
};

// ============================================================
// DockerRequirementType
// ============================================================
//
// Describes a requirement that the application may need when running inside a container.
enum class DockerRequirementType
{
    Package,
    Port,
    EnvironmentVariable,
    NetworkCapability,
    RuntimeLibrary,
    Unknown
};

// ============================================================
// RuntimeRequirement
// ============================================================
//
// Represents one runtime requirement discovered during analysis.
struct DockerRequirement
{
    std::string value;
    DockerRequirementType type;    
};

// ============================================================
// FileAnalysis
// ============================================================
//
// Contains everything discovered by FileScanner AFTER reading one file.

struct FileAnalysis
{
    int fileId;
    std::vector<DependencyReference> dependencies;
    std::vector<DockerRequirement> dockerRequirements;
};

// ============================================================
// Dependency
// ============================================================
//
// Represents a FINAL local dependency relationship between two files after resolution.

inline std::vector<FileInfo> IdToInfo;
inline std::unordered_map<std::string, int> PathToId;
inline std::string absoluteLocation;

    struct PackageInfo
{
    int configFileId;

    std::filesystem::path rootPath;

    std::string name;

    std::vector<std::string> dependencies;
    std::vector<std::string> devDependencies;

    std::vector<std::string> scripts;

    std::string mainEntryPoint;
};

inline const std::unordered_map<std::string, Language> ExtensionToLanguage = {

    {".c", Language::C},
    {".h", Language::C},

    {".cpp", Language::Cpp},
    {".cc", Language::Cpp},
    {".cxx", Language::Cpp},
    {".hpp", Language::Cpp},
    {".hh", Language::Cpp},
    {".hxx", Language::Cpp},

    {".java", Language::Java},

    {".py", Language::Python},
    {".pyw", Language::Python},
    {".pyi", Language::Python},

    {".js", Language::JavaScript},
    {".mjs", Language::JavaScript},
    {".cjs", Language::JavaScript},

    {".ts", Language::TypeScript},
    {".tsx", Language::TypeScript}};