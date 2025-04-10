#pragma once

#include <string>
#include <vector>
#include <cstdint>

/**
 * @class FileSystem
 * @brief Provides utility functions for file system operations
 * 
 * This class offers a platform-independent interface for file and directory operations.
 * All functions are static and start with the BE_ prefix.
 */
class FileSystem
{
public:
    /**
     * @brief Initializes the FileSystem subsystem
     * @return True if successful, false otherwise
     */
    static bool BE_Initialize();

    // File operations
    /**
     * @brief Checks if a file exists at the specified path
     * @param path The path to check
     * @return True if file exists, false otherwise
     */
    static bool BE_File_Exists(const std::string& path);
    
    /**
     * @brief Creates an empty file at the specified path
     * @param path The path where the file should be created
     * @return True if successful, false otherwise
     */
    static bool BE_Create_File(const std::string& path);
    
    /**
     * @brief Deletes the specified file
     * @param path The path of the file to delete
     * @return True if successful, false otherwise
     */
    static bool BE_Delete_File(const std::string& path);

    // File reading/writing operations
    /**
     * @brief Reads a text file and returns its content as a string
     * @param path The path of the file to read
     * @return File content or empty string if file not found
     */
    static std::string BE_Read_Text_File(const std::string& path);
    
    /**
     * @brief Writes content to a text file
     * @param path The path of the file to write to
     * @param content The text content to write
     * @return True if successful, false otherwise
     */
    static bool BE_Write_Text_File(const std::string& path, const std::string& content);

    /**
     * @brief Reads a binary file and returns its content as a byte array
     * @param path The path of the file to read
     * @return File content or empty vector if file not found
     */
    static std::vector<uint8_t> BE_Read_Binary_File(const std::string& path);
    
    /**
     * @brief Writes data to a binary file
     * @param path The path of the file to write to
     * @param data The binary data to write
     * @return True if successful, false otherwise
     */
    static bool BE_Write_Binary_File(const std::string& path, const std::vector<uint8_t>& data);

    // Directory operations
    /**
     * @brief Checks if a directory exists at the specified path
     * @param path The path to check
     * @return True if directory exists, false otherwise
     */
    static bool BE_Directory_Exists(const std::string& path);
    
    /**
     * @brief Creates a directory at the specified path (creates parent directories if needed)
     * @param path The path where the directory should be created
     * @return True if successful, false otherwise
     */
    static bool BE_Create_Directory(const std::string& path);
    
    /**
     * @brief Deletes the specified directory
     * @param path The path of the directory to delete
     * @param recursive If true, also deletes files/directories inside
     * @return True if successful, false otherwise
     */
    static bool BE_Delete_Directory(const std::string& path, bool recursive = false);

    // Directory listing operations
    /**
     * @brief Lists files in the specified directory
     * @param path The path of the directory to list
     * @param extension Filter for files with a specific extension (e.g. ".txt")
     * @return List of file paths
     */
    static std::vector<std::string> BE_Get_Files_In_Directory(const std::string& path, const std::string& extension = "");
    
    /**
     * @brief Lists subdirectories in the specified directory
     * @param path The path of the directory to list
     * @return List of subdirectory paths
     */
    static std::vector<std::string> BE_Get_Directories_In_Directory(const std::string& path);

    // Path operations
    /**
     * @brief Extracts the file name from a path
     * @param path The file path
     * @return File name (including extension)
     */
    static std::string BE_Get_File_Name(const std::string& path);
    
    /**
     * @brief Returns the file extension
     * @param path The file path
     * @return File extension (e.g. ".txt")
     */
    static std::string BE_Get_File_Extension(const std::string& path);
    
    /**
     * @brief Returns the file name without extension
     * @param path The file path
     * @return File name without extension
     */
    static std::string BE_Get_File_Name_Without_Extension(const std::string& path);
    
    /**
     * @brief Returns the parent directory path of a file or directory
     * @param path The file or directory path
     * @return Parent directory path
     */
    static std::string BE_Get_Directory_Path(const std::string& path);
    
    /**
     * @brief Normalizes a path (converts to absolute path)
     * @param path The path to normalize
     * @return Normalized path
     */
    static std::string BE_Normalize_Path(const std::string& path);
    
    /**
     * @brief Combines two paths
     * @param path1 First path
     * @param path2 Second path
     * @return Combined path
     */
    static std::string BE_Combine_Paths(const std::string& path1, const std::string& path2);
};
