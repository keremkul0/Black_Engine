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

// Define and register the FileSystem log category
BE_DEFINE_LOG_CATEGORY(FileSystemLog, "FileSystem");

bool FileSystem::BE_Initialize() {
    // Subsystem initialization message
    BE_LOG_INFO(FileSystemLog, "FileSystem subsystem initialized");
    return true;
}

bool FileSystem::BE_File_Exists(const std::string &path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

bool FileSystem::BE_Create_File(const std::string &path) {
    try {
        // Create parent directories if they don't exist
        const fs::path filePath(path);
        if (const fs::path parentPath = filePath.parent_path(); !parentPath.empty() && !fs::exists(parentPath)) {
            BE_LOG_DEBUG(FileSystemLog, "Creating parent directory: {}", parentPath.string());
            fs::create_directories(parentPath);
        }
        std::ofstream file(path);
        if (const bool result = file.is_open(); !result) {
            BE_LOG_ERROR(FileSystemLog, "Failed to create file: {}", path);
            return false;
        }

        BE_LOG_DEBUG(FileSystemLog, "File created successfully: {}", path);
        return true;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error creating file: {} - {}", path, e.what());
        return false;
    }
}

bool FileSystem::BE_Delete_File(const std::string &path) {
    try {
        if (!BE_File_Exists(path)) {
            BE_LOG_WARNING(FileSystemLog, "File not found for deletion: {}", path);
            return false;
        }

        const bool result = fs::remove(path);

        if (result) {
            BE_LOG_DEBUG(FileSystemLog, "File deleted successfully: {}", path);
        } else {
            BE_LOG_ERROR(FileSystemLog, "Failed to delete file: {}", path);
        }

        return result;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error deleting file: {} - {}", path, e.what());
        return false;
    }
}

std::string FileSystem::BE_Read_Text_File(const std::string &path) {
    try {
        if (!BE_File_Exists(path)) {
            BE_LOG_WARNING(FileSystemLog, "File not found for reading: {}", path);
            return "";
        }

        const std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();

        BE_LOG_DEBUG(FileSystemLog, "File read successfully: {}", path);
        return buffer.str();
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error reading file: {} - {}", path, e.what());
        return "";
    }
}

bool FileSystem::BE_Write_Text_File(const std::string &path, const std::string &content) {
    try {
        // Create parent directories if they don't exist
        fs::path filePath(path);
        fs::path parentPath = filePath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            BE_LOG_DEBUG(FileSystemLog, "Creating parent directory: {}", parentPath.string());
            fs::create_directories(parentPath);
        }

        std::ofstream file(path);
        if (!file.is_open()) {
            BE_LOG_ERROR(FileSystemLog, "Failed to open file for writing: {}", path);
            return false;
        }
        file << content;

        if (file.fail()) {
            BE_LOG_ERROR(FileSystemLog, "Failed to write to file: {}", path);
            return false;
        }

        BE_LOG_DEBUG(FileSystemLog, "File written successfully: {}", path);
        return true;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error writing to file: {} - {}", path, e.what());
        return false;
    }
}

std::vector<uint8_t> FileSystem::BE_Read_Binary_File(const std::string &path) {
    try {
        if (!BE_File_Exists(path)) {
            BE_LOG_WARNING(FileSystemLog, "Binary file not found for reading: {}", path);
            return {};
        }

        std::ifstream file(path, std::ios::binary);
        file.unsetf(std::ios::skipws); // To not skip whitespaces

        file.seekg(0, std::ios::end);
        const std::streampos fileSizeStream = file.tellg();
        if (fileSizeStream == std::streampos(-1)) {
            BE_LOG_ERROR(FileSystemLog, "Error getting file size: {}", path);
            return {};
        }
        const auto fileSize = static_cast<size_t>(fileSizeStream);
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data(fileSize);

        // Güvenli dönüşüm için kontrol eklendi
        if (fileSize > static_cast<size_t>(std::numeric_limits<std::streamsize>::max())) {
            BE_LOG_ERROR(FileSystemLog, "File size exceeds maximum allowed by std::streamsize: {}", path);
            return {};
        }

        const auto fileSizeAsStreamSize = static_cast<std::streamsize>(fileSize);
        file.read(reinterpret_cast<char *>(data.data()), fileSizeAsStreamSize);

        if (file.fail()) {
            BE_LOG_ERROR(FileSystemLog, "Error reading binary file: {}", path);
            return {};
        }

        BE_LOG_DEBUG(FileSystemLog, "Binary file read successfully: {} - {} bytes", path, data.size());
        return data;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error reading binary file: {} - {}", path, e.what());
        return {};
    }
}

bool FileSystem::BE_Write_Binary_File(const std::string &path, const std::vector<uint8_t> &data) {
    try {
        // Create parent directories if they don't exist
        fs::path filePath(path);
        fs::path parentPath = filePath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            BE_LOG_DEBUG(FileSystemLog, "Creating parent directory: {}", parentPath.string());
            fs::create_directories(parentPath);
        }
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            BE_LOG_ERROR(FileSystemLog, "Failed to open binary file: {}", path);
            return false;
        }

        file.write(
            reinterpret_cast<const char *>(data.data()),
            static_cast<std::streamsize>(data.size())
        );
        if (file.fail()) {
            BE_LOG_ERROR(FileSystemLog, "Failed to write to binary file: {}", path);
            return false;
        }

        BE_LOG_DEBUG(FileSystemLog, "Binary file written successfully: {} - {} bytes", path, data.size());
        return true;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error writing to binary file: {} - {}", path, e.what());
        return false;
    }
}

bool FileSystem::BE_Directory_Exists(const std::string &path) {
    return fs::exists(path) && fs::is_directory(path);
}

bool FileSystem::BE_Create_Directory(const std::string &path) {
    try {
        BE_LOG_DEBUG(FileSystemLog, "Creating directory: {}", path);
        const bool result = fs::create_directories(path);

        if (!result) {
            BE_LOG_ERROR(FileSystemLog, "Failed to create directory: {}", path);
        } else {
            BE_LOG_DEBUG(FileSystemLog, "Directory created successfully: {}", path);
        }
        return result;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error creating directory: {} - {}", path, e.what());
        return false;
    }
}

bool FileSystem::BE_Delete_Directory(const std::string &path, const bool recursive) {
    try {
        if (!BE_Directory_Exists(path)) {
            BE_LOG_WARNING(FileSystemLog, "Directory not found for deletion: {}", path);
            return false;
        }

        bool result;

        if (recursive) {
            result = fs::remove_all(path) > 0;
            BE_LOG_DEBUG(FileSystemLog, "Deleting directory and contents: {}", path);
        } else {
            result = fs::remove(path);
            BE_LOG_DEBUG(FileSystemLog, "Deleting empty directory: {}", path);
        }

        if (!result) {
            BE_LOG_ERROR(FileSystemLog, "Failed to delete directory: {}", path);
        }
        return result;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error deleting directory: {} - {}", path, e.what());
        return false;
    }
}

std::vector<std::string> FileSystem::BE_Get_Files_In_Directory(
    const std::string &path,
    const std::string &extension
) {
    std::vector<std::string> files;
    try {
        if (!BE_Directory_Exists(path)) {
            BE_LOG_WARNING(FileSystemLog, "Directory not found: {}", path);
            return files;
        }

        for (const auto &entry: fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                if (extension.empty() || entry.path().extension() == extension) {
                    files.push_back(entry.path().string());
                }
            }
        }
        BE_LOG_DEBUG(FileSystemLog, "Found {} files in directory: {}", files.size(), path);
        return files;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error listing directory contents: {} - {}", path, e.what());
        return files;
    }
}

std::vector<std::string> FileSystem::BE_Get_Directories_In_Directory(const std::string &path) {
    std::vector<std::string> directories;
    try {
        if (!BE_Directory_Exists(path)) {
            BE_LOG_WARNING(FileSystemLog, "Directory not found: {}", path);
            return directories;
        }

        for (const auto &entry: fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }

        BE_LOG_DEBUG(FileSystemLog, "Found {} subdirectories in directory: {}", directories.size(), path);
        return directories;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error listing subdirectories: {} - {}", path, e.what());
        return directories;
    }
}

std::string FileSystem::BE_Get_File_Name(const std::string &path) {
    return fs::path(path).filename().string();
}

std::string FileSystem::BE_Get_File_Extension(const std::string &path) {
    return fs::path(path).extension().string();
}

std::string FileSystem::BE_Get_File_Name_Without_Extension(const std::string &path) {
    return fs::path(path).stem().string();
}

std::string FileSystem::BE_Get_Directory_Path(const std::string &path) {
    return fs::path(path).parent_path().string();
}

std::string FileSystem::BE_Normalize_Path(const std::string &path) {
    try {
        return absolute(fs::path(path)).string();
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error normalizing path: {} - {}", path, e.what());
        return path; // Return original path in case of error
    }
}

std::string FileSystem::BE_Combine_Paths(const std::string &path1, const std::string &path2) {
    return (fs::path(path1) / fs::path(path2)).string();
}

bool FileSystem::BE_Copy_File(const std::string &src, const std::string &dst) {
    try {
        namespace fs = std::filesystem;
        fs::path srcPath = fs::absolute(src);
        fs::path dstPath = fs::absolute(dst);

        // Eğer kaynak ve hedef aynı fiziksel dosya ise kopyalamaya gerek yok
        if (fs::equivalent(srcPath, dstPath)) {
            BE_LOG_DEBUG(FileSystemLog,
                         "Source and destination are the same file, skipping copy: {}",
                         srcPath.string());
            return true;
        }

        // Kaynak dosyanın varlığını doğrula
        if (!BE_File_Exists(srcPath.string())) {
            BE_LOG_ERROR(FileSystemLog, "Source file does not exist: {}", srcPath.string());
            return false;
        }

        // Hedef klasörü yoksa oluştur
        fs::path parentPath = dstPath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            BE_LOG_DEBUG(FileSystemLog, "Creating parent directory: {}", parentPath.string());
            fs::create_directories(parentPath);
        }

        // Hedef dosya zaten varsa sil (overwrite için)
        if (fs::exists(dstPath)) {
            BE_LOG_DEBUG(FileSystemLog, "Removing existing file before copy: {}", dstPath.string());
            if (!fs::remove(dstPath)) {
                BE_LOG_ERROR(FileSystemLog, "Failed to remove existing file: {}", dstPath.string());
                return false;
            }
        }

        // Şimdi temiz bir kopyalama yap
        fs::copy_file(srcPath, dstPath); // default seçenek: create only
        BE_LOG_DEBUG(FileSystemLog, "File copied from {} to {}",
                     srcPath.string(), dstPath.string());
        return true;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog,
                     "Error copying file from {} to {}: {}",
                     src, dst, e.what());
        return false;
    }
}


bool FileSystem::BE_Write_JSON(const std::string &path, const nlohmann::json &j) {
    try {
        // Create parent directories if they don't exist
        const fs::path filePath(path);
        if (const fs::path parentPath = filePath.parent_path(); !parentPath.empty() && !fs::exists(parentPath)) {
            BE_LOG_DEBUG(FileSystemLog, "Creating parent directory: {}", parentPath.string());
            fs::create_directories(parentPath);
        }

        // Open the file and write JSON with pretty-printing (indent = 4)
        std::ofstream file(path);
        if (!file.is_open()) {
            BE_LOG_ERROR(FileSystemLog, "Failed to open file for writing JSON: {}", path);
            return false;
        }

        // Write JSON with UTF-8 encoding and pretty-printing
        file << j.dump(4);

        BE_LOG_DEBUG(FileSystemLog, "JSON data written to {}", path);
        return true;
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error writing JSON to {}: {}", path, e.what());
        return false;
    }
}

nlohmann::json FileSystem::BE_Read_JSON(const std::string &path) {
    try {
        // Check if file exists
        if (!BE_File_Exists(path)) {
            BE_LOG_WARNING(FileSystemLog, "JSON file does not exist: {}", path);
            return nlohmann::json::object(); // Return empty JSON object
        }

        // Read the file content
        std::string content = BE_Read_Text_File(path);
        if (content.empty()) {
            BE_LOG_WARNING(FileSystemLog, "JSON file is empty: {}", path);
            return nlohmann::json::object();
        }

        // Parse the JSON content
        nlohmann::json j = nlohmann::json::parse(content);
        BE_LOG_DEBUG(FileSystemLog, "JSON data read from {}", path);
        return j;
    } catch (const nlohmann::json::parse_error &e) {
        BE_LOG_ERROR(FileSystemLog, "JSON parse error in {}: {}", path, e.what());
        return nlohmann::json::object(); // Return empty JSON object on parse error
    } catch (const std::exception &e) {
        BE_LOG_ERROR(FileSystemLog, "Error reading JSON from {}: {}", path, e.what());
        return nlohmann::json::object();
    }
}
