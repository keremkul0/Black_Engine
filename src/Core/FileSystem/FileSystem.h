#pragma once

#include <string>
#include <vector>
#include <cstdint>

class FileSystem
{
public:
    static bool Initialize();

    static bool FileExists(const std::string& path);
    static bool CreateFile(const std::string& path);
    static bool DeleteFile(const std::string& path);

    static std::string ReadTextFile(const std::string& path);
    static bool WriteTextFile(const std::string& path, const std::string& content);

    static std::vector<uint8_t> ReadBinaryFile(const std::string& path);
    static bool WriteBinaryFile(const std::string &path, const std::vector<uint8_t> &data);

    static bool DirectoryExists(const std::string& path);
    static bool CreateDirectory(const std::string& path);
    static bool DeleteDirectory(const std::string& path, bool recursive = false);

    static std::vector<std::string> GetFilesInDirectory(const std::string& path, const std::string& extension = "");
    static std::vector<std::string> GetDirectoriesInDirectory(const std::string& path);

    static std::string GetFileName(const std::string& path);
    static std::string GetFileExtension(const std::string& path);
    static std::string GetFileNameWithoutExtension(const std::string& path);
    static std::string GetDirectoryPath(const std::string& path);
    static std::string NormalizePath(const std::string& path);
    static std::string CombinePaths(const std::string& path1, const std::string& path2);
};
