/**
 * @file FileSystem.cpp
 * @brief Implementation of filesystem utilities for cross-platform file operations
 *
 * This file provides implementation for various filesystem operations including:
 * - File manipulation (create, delete, read, write)
 * - Directory operations (create, delete, listing)
 * - Path utilities (normalization, combining, extraction)
 *
 * The implementation uses the C++17 std::filesystem library for cross-platform
 * filesystem operations, abstracting platform-specific details away from client code.
 *
 * @note All operations are designed to be safe, returning empty results or false
 *       instead of throwing exceptions when operations fail.
 *
 * @see FileSystem.h for the interface definitions
 */

#include "FileSystem.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iterator>
#include "Core/Logger/LogMacros.h"

namespace fs = std::filesystem;

bool FileSystem::Initialize()
{
    // Log initialization for this subsystem.
    BE_CAT_INFO("FileSystem", "FileSystem module initialized");
    return true;
}

bool FileSystem::FileExists(const std::string &path)
{
    return fs::exists(path) && fs::is_regular_file(path);
}

bool FileSystem::CreateFile(const std::string &path)
{
    std::ofstream file(path);
    return file.is_open();
}

bool FileSystem::DeleteFile(const std::string &path)
{
    if (!FileExists(path))
        return false;

    return fs::remove(path);
}

std::string FileSystem::ReadTextFile(const std::string &path)
{
    if (!FileExists(path))
    {
        BE_CAT_WARN_FORMAT("FileSystem", "File does not exist: {}", path);
        return "";
    }

    const std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystem::WriteTextFile(const std::string &path, const std::string &content)
{
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << content;
    return true;
}

std::vector<uint8_t> FileSystem::ReadBinaryFile(const std::string &path)
{
    if (!FileExists(path))
        return {};

    std::ifstream file(path, std::ios::binary);
    file.unsetf(std::ios::skipws);

    file.seekg(0, std::ios::end);
    const std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data;
    data.reserve(static_cast<size_t>(fileSize));
    data.insert(
        data.begin(),
        std::istream_iterator<uint8_t>(file),
        std::istream_iterator<uint8_t>()
    );

    return data;
}

bool FileSystem::WriteBinaryFile(const std::string &path, const std::vector<uint8_t> &data)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;

    file.write(
        reinterpret_cast<const char *>(data.data()),
        static_cast<std::streamsize>(data.size())
    );
    return true;
}

bool FileSystem::DirectoryExists(const std::string &path)
{
    return fs::exists(path) && fs::is_directory(path);
}

bool FileSystem::CreateDirectory(const std::string &path)
{
    BE_CAT_DEBUG_FORMAT("FileSystem", "Creating directory: {}", path);
    const bool result = fs::create_directories(path);
    if (!result)
    {
        BE_CAT_ERROR_FORMAT("FileSystem", "Failed to create directory: {}", path);
    }
    return result;
}

bool FileSystem::DeleteDirectory(const std::string &path, bool recursive)
{
    if (!DirectoryExists(path))
        return false;

    if (recursive)
        return fs::remove_all(path) > 0;

    return fs::remove(path);
}

std::vector<std::string> FileSystem::GetFilesInDirectory(
    const std::string &path,
    const std::string &extension
)
{
    std::vector<std::string> files;

    if (!DirectoryExists(path))
        return files;

    for (const auto &entry : fs::directory_iterator(path))
    {
        if (entry.is_regular_file())
        {
            if (extension.empty() || entry.path().extension() == extension)
            {
                files.push_back(entry.path().string());
            }
        }
    }
    return files;
}

std::vector<std::string> FileSystem::GetDirectoriesInDirectory(const std::string &path)
{
    std::vector<std::string> directories;

    if (!DirectoryExists(path))
    {
        BE_CAT_WARN_FORMAT("FileSystem", "Directory does not exist: {}", path);
        return directories;
    }

    for (const auto &entry : fs::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            directories.push_back(entry.path().string());
        }
    }
    return directories;
}

std::string FileSystem::GetFileName(const std::string &path)
{
    return fs::path(path).filename().string();
}

std::string FileSystem::GetFileExtension(const std::string &path)
{
    return fs::path(path).extension().string();
}

std::string FileSystem::GetFileNameWithoutExtension(const std::string &path)
{
    return fs::path(path).stem().string();
}

std::string FileSystem::GetDirectoryPath(const std::string &path)
{
    return fs::path(path).parent_path().string();
}

std::string FileSystem::NormalizePath(const std::string &path)
{
    return fs::absolute(fs::path(path)).string();
}

std::string FileSystem::CombinePaths(const std::string &path1, const std::string &path2)
{
    return (fs::path(path1) / fs::path(path2)).string();
}
