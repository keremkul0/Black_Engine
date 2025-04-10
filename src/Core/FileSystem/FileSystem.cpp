/**
 * @file FileSystem.cpp
 * @brief Implementation of utility functions for file system operations
 *
 * This file contains the implementation of the FileSystem class, which provides
 * a cross-platform interface for file and directory operations. It uses std::filesystem
 * for C++17 compliant and platform-independent operations.
 *
 * @see FileSystem.h
 */

// Include our header first
#include "FileSystem.h"

// Then include other required headers
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iterator>
#include "Core/Logger/LogMacros.h"

// Using std::filesystem with a shorter alias
namespace fs = std::filesystem;

bool FileSystem::BE_Initialize()
{
    // Subsystem initialization message
    BE_CAT_INFO("FileSystem", "FileSystem subsystem initialized");
    return true;
}

bool FileSystem::BE_File_Exists(const std::string &path)
{
    return fs::exists(path) && fs::is_regular_file(path);
}

bool FileSystem::BE_Create_File(const std::string &path)
{
    try {
        // Create parent directories if they don't exist
        const fs::path filePath(path);

        if (const fs::path parentPath = filePath.parent_path(); !parentPath.empty() && !fs::exists(parentPath)) {
            BE_CAT_DEBUG_FMT("FileSystem", "Creating parent directory: {}", parentPath.string());
            fs::create_directories(parentPath);
        }
        
        std::ofstream file(path);

        if (const bool result = file.is_open(); !result) {
            BE_CAT_ERROR_FMT("FileSystem", "Failed to create file: {}", path);
            return false;
        }

        BE_CAT_DEBUG_FMT("FileSystem", "File created successfully: {}", path);
        return true;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error creating file: {} - {}", path, e.what());
        return false;
    }
}

bool FileSystem::BE_Delete_File(const std::string &path)
{
    try {
        if (!BE_File_Exists(path)) {
            BE_CAT_WARN_FMT("FileSystem", "File not found for deletion: {}", path);
            return false;
        }

        bool result = fs::remove(path);

        if (result) {
            BE_CAT_DEBUG_FMT("FileSystem", "File deleted successfully: {}", path);
        } else {
            BE_CAT_ERROR_FMT("FileSystem", "Failed to delete file: {}", path);
        }

        return result;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error deleting file: {} - {}", path, e.what());
        return false;
    }
}

std::string FileSystem::BE_Read_Text_File(const std::string &path)
{
    try {
        if (!BE_File_Exists(path)) {
            BE_CAT_WARN_FMT("FileSystem", "File not found for reading: {}", path);
            return "";
        }

        const std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();

        BE_CAT_DEBUG_FMT("FileSystem", "File read successfully: {}", path);
        return buffer.str();
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error reading file: {} - {}", path, e.what());
        return "";
    }
}

bool FileSystem::BE_Write_Text_File(const std::string &path, const std::string &content)
{
    try {
        // Create parent directories if they don't exist
        fs::path filePath(path);
        fs::path parentPath = filePath.parent_path();
        
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            BE_CAT_DEBUG_FMT("FileSystem", "Creating parent directory: {}", parentPath.string());
            fs::create_directories(parentPath);
        }
        
        std::ofstream file(path);
        if (!file.is_open()) {
            BE_CAT_ERROR_FMT("FileSystem", "Failed to open file for writing: {}", path);
            return false;
        }

        file << content;

        if (file.fail()) {
            BE_CAT_ERROR_FMT("FileSystem", "Failed to write to file: {}", path);
            return false;
        }

        BE_CAT_DEBUG_FMT("FileSystem", "File written successfully: {}", path);
        return true;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error writing to file: {} - {}", path, e.what());
        return false;
    }
}

std::vector<uint8_t> FileSystem::BE_Read_Binary_File(const std::string &path)
{
    try {
        if (!BE_File_Exists(path)) {
            BE_CAT_WARN_FMT("FileSystem", "Binary file not found for reading: {}", path);
            return {};
        }

        std::ifstream file(path, std::ios::binary);
        file.unsetf(std::ios::skipws);  // To not skip whitespaces

        file.seekg(0, std::ios::end);
        const std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data;
        data.reserve(fileSize);

        // Read the file byte by byte into the vector
        data.insert(
            data.begin(),
            std::istream_iterator<uint8_t>(file),
            std::istream_iterator<uint8_t>()
        );

        BE_CAT_DEBUG_FMT("FileSystem", "Binary file read successfully: {} - {} bytes", path, data.size());
        return data;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error reading binary file: {} - {}", path, e.what());
        return {};
    }
}

bool FileSystem::BE_Write_Binary_File(const std::string &path, const std::vector<uint8_t> &data)
{
    try {
        // Create parent directories if they don't exist
        fs::path filePath(path);
        fs::path parentPath = filePath.parent_path();
        
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            BE_CAT_DEBUG_FMT("FileSystem", "Creating parent directory: {}", parentPath.string());
            fs::create_directories(parentPath);
        }
        
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            BE_CAT_ERROR_FMT("FileSystem", "Failed to open binary file: {}", path);
            return false;
        }

        file.write(
            reinterpret_cast<const char *>(data.data()),
            static_cast<std::streamsize>(data.size())
        );

        if (file.fail()) {
            BE_CAT_ERROR_FMT("FileSystem", "Failed to write to binary file: {}", path);
            return false;
        }

        BE_CAT_DEBUG_FMT("FileSystem", "Binary file written successfully: {} - {} bytes", path, data.size());
        return true;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error writing to binary file: {} - {}", path, e.what());
        return false;
    }
}

bool FileSystem::BE_Directory_Exists(const std::string &path)
{
    return fs::exists(path) && fs::is_directory(path);
}

bool FileSystem::BE_Create_Directory(const std::string &path)
{
    try {
        BE_CAT_DEBUG_FMT("FileSystem", "Creating directory: {}", path);
        const bool result = fs::create_directories(path);

        if (!result) {
            BE_CAT_ERROR_FMT("FileSystem", "Failed to create directory: {}", path);
        } else {
            BE_CAT_DEBUG_FMT("FileSystem", "Directory created successfully: {}", path);
        }

        return result;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error creating directory: {} - {}", path, e.what());
        return false;
    }
}

bool FileSystem::BE_Delete_Directory(const std::string &path, const bool recursive)
{
    try {
        if (!BE_Directory_Exists(path)) {
            BE_CAT_WARN_FMT("FileSystem", "Directory not found for deletion: {}", path);
            return false;
        }

        bool result;

        if (recursive) {
            result = fs::remove_all(path) > 0;
            BE_CAT_DEBUG_FMT("FileSystem", "Deleting directory and contents: {}", path);
        } else {
            result = fs::remove(path);
            BE_CAT_DEBUG_FMT("FileSystem", "Deleting empty directory: {}", path);
        }

        if (!result) {
            BE_CAT_ERROR_FMT("FileSystem", "Failed to delete directory: {}", path);
        }

        return result;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error deleting directory: {} - {}", path, e.what());
        return false;
    }
}

std::vector<std::string> FileSystem::BE_Get_Files_In_Directory(
    const std::string &path,
    const std::string &extension
)
{
    std::vector<std::string> files;

    try {
        if (!BE_Directory_Exists(path)) {
            BE_CAT_WARN_FMT("FileSystem", "Directory not found: {}", path);
            return files;
        }

        for (const auto &entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                if (extension.empty() || entry.path().extension() == extension) {
                    files.push_back(entry.path().string());
                }
            }
        }

        BE_CAT_DEBUG_FMT("FileSystem", "Found {} files in directory: {}", files.size(), path);
        return files;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error listing directory contents: {} - {}", path, e.what());
        return files;
    }
}

std::vector<std::string> FileSystem::BE_Get_Directories_In_Directory(const std::string &path)
{
    std::vector<std::string> directories;

    try {
        if (!BE_Directory_Exists(path)) {
            BE_CAT_WARN_FMT("FileSystem", "Directory not found: {}", path);
            return directories;
        }

        for (const auto &entry : fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }

        BE_CAT_DEBUG_FMT("FileSystem", "Found {} subdirectories in directory: {}", directories.size(), path);
        return directories;
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error listing subdirectories: {} - {}", path, e.what());
        return directories;
    }
}

std::string FileSystem::BE_Get_File_Name(const std::string &path)
{
    return fs::path(path).filename().string();
}

std::string FileSystem::BE_Get_File_Extension(const std::string &path)
{
    return fs::path(path).extension().string();
}

std::string FileSystem::BE_Get_File_Name_Without_Extension(const std::string &path)
{
    return fs::path(path).stem().string();
}

std::string FileSystem::BE_Get_Directory_Path(const std::string &path)
{
    return fs::path(path).parent_path().string();
}

std::string FileSystem::BE_Normalize_Path(const std::string &path)
{
    try {
        return absolute(fs::path(path)).string();
    }
    catch (const std::exception& e) {
        BE_CAT_ERROR_FMT("FileSystem", "Error normalizing path: {} - {}", path, e.what());
        return path; // Return original path in case of error
    }
}

std::string FileSystem::BE_Combine_Paths(const std::string &path1, const std::string &path2)
{
    return (fs::path(path1) / fs::path(path2)).string();
}
