#include <gtest/gtest.h>
#include "Core/FileSystem/FileSystem.h"
#include "Core/Logger/LogManager.h"
#include <fstream>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

class FileSystemTest : public testing::Test {
protected:
    // Use temp directory and random folder name for tests
    std::filesystem::path tempBasePath = std::filesystem::temp_directory_path() / "black_engine_test";
    std::string testDir;
    std::string testFile;
    std::string testContent = "Hello, Black Engine!";
    std::string testBinaryFile;
    std::vector<uint8_t> testBinaryContent = {0x42, 0x6C, 0x61, 0x63, 0x6B, 0x20, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65};
    std::string nestedDir;    void SetUp() override {
        // Create a unique test directory for each test
        const std::string uniqueSuffix = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        
        // Make sure the base path exists - using standard filesystem API instead of our own FileSystem class
        // to avoid circular dependencies in testing
        try {
            create_directories(tempBasePath);
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Failed to create temp directory: " << e.what() << std::endl;
            // Fallback to using just the system temp directory
            tempBasePath = std::filesystem::temp_directory_path();
        }
        
        // Define test paths
        testDir = (tempBasePath / ("test_dir_" + uniqueSuffix)).string();
        testFile = (tempBasePath / ("test_file_" + uniqueSuffix + ".txt")).string();
        testBinaryFile = (tempBasePath / ("test_binary_" + uniqueSuffix + ".bin")).string();
        nestedDir = (std::filesystem::path(testDir) / "nested_dir").string();
        
        // Clean up any leftover files/dirs from previous runs
        CleanUp();
    }

    void TearDown() override {
        // Clean up test files and directories after each test
        CleanUp();
    }

    void CleanUp() const {
        // Remove test files and directories
        if (FileSystem::BE_File_Exists(testFile)) {
            FileSystem::BE_Delete_File(testFile);
        }
        if (FileSystem::BE_File_Exists(testBinaryFile)) {
            FileSystem::BE_Delete_File(testBinaryFile);
        }
        if (FileSystem::BE_Directory_Exists(testDir)) {
            FileSystem::BE_Delete_Directory(testDir, true);
        }
        
        // Make sure the base temp directory is cleaned if empty
        try {
            if (exists(tempBasePath) && is_empty(tempBasePath)) {
                std::filesystem::remove(tempBasePath);
            }
        } catch (const std::exception&) {
            // Ignore errors when trying to clean up temp dir
        }
    }
};

// Initialization tests
TEST_F(FileSystemTest, Initialize) {
    EXPECT_TRUE(FileSystem::BE_Initialize());
}

// File existence tests
TEST_F(FileSystemTest, FileExists) {
    EXPECT_FALSE(FileSystem::BE_File_Exists(testFile));
    
    // Create the file manually using standard C++ file operations
    // Make sure the directory exists for the file first
    const std::filesystem::path filePath(testFile);
    create_directories(filePath.parent_path());
    
    std::ofstream file(testFile);
    ASSERT_TRUE(file.is_open()) << "Failed to open file for writing: " << testFile;
    file << testContent;
    file.close();
    
    EXPECT_TRUE(FileSystem::BE_File_Exists(testFile));
}

// File creation tests
TEST_F(FileSystemTest, CreateFile) {
    EXPECT_TRUE(FileSystem::BE_Create_File(testFile));
    EXPECT_TRUE(FileSystem::BE_File_Exists(testFile));
    
    // Clean up
    EXPECT_TRUE(FileSystem::BE_Delete_File(testFile));
}

// Text file reading and writing tests
TEST_F(FileSystemTest, ReadWriteTextFile) {
    // Write to a text file
    EXPECT_TRUE(FileSystem::BE_Write_Text_File(testFile, testContent));
    EXPECT_TRUE(FileSystem::BE_File_Exists(testFile));
    
    // Read from the text file
    const std::string content = FileSystem::BE_Read_Text_File(testFile);
    EXPECT_EQ(content, testContent);
    
    // Read from a non-existent file should return empty string
    EXPECT_TRUE(FileSystem::BE_Read_Text_File("nonexistent_file.txt").empty());
}

// Binary file reading and writing tests
TEST_F(FileSystemTest, ReadWriteBinaryFile) {
    // Write to a binary file
    EXPECT_TRUE(FileSystem::BE_Write_Binary_File(testBinaryFile, testBinaryContent));
    EXPECT_TRUE(FileSystem::BE_File_Exists(testBinaryFile));
    
    // Read from the binary file
    const std::vector<uint8_t> content = FileSystem::BE_Read_Binary_File(testBinaryFile);
    EXPECT_EQ(content.size(), testBinaryContent.size());
    for (size_t i = 0; i < content.size(); i++) {
        EXPECT_EQ(content[i], testBinaryContent[i]);
    }
    
    // Read from a non-existent file should return empty vector
    EXPECT_TRUE(FileSystem::BE_Read_Binary_File("nonexistent_binary_file.bin").empty());
}

// Directory existence tests
TEST_F(FileSystemTest, DirectoryExists) {
    EXPECT_FALSE(FileSystem::BE_Directory_Exists(testDir));
    
    // Create the directory manually with create_directories to ensure parent directories are created too
    fs::create_directories(testDir);
    
    EXPECT_TRUE(FileSystem::BE_Directory_Exists(testDir));
}

// Directory creation tests
TEST_F(FileSystemTest, CreateDirectory) {
    EXPECT_TRUE(FileSystem::BE_Create_Directory(testDir));
    EXPECT_TRUE(FileSystem::BE_Directory_Exists(testDir));
    
    // Test creating nested directories
    EXPECT_TRUE(FileSystem::BE_Create_Directory(nestedDir));
    EXPECT_TRUE(FileSystem::BE_Directory_Exists(nestedDir));
}

// File listing tests
TEST_F(FileSystemTest, GetFilesInDirectory) {
    // Set up test directory with files
    EXPECT_TRUE(FileSystem::BE_Create_Directory(testDir));

    const std::string file1 = testDir + "/file1.txt";
    const std::string file2 = testDir + "/file2.txt";
    const std::string file3 = testDir + "/file3.dat";
    
    EXPECT_TRUE(FileSystem::BE_Write_Text_File(file1, "file1"));
    EXPECT_TRUE(FileSystem::BE_Write_Text_File(file2, "file2"));
    EXPECT_TRUE(FileSystem::BE_Write_Text_File(file3, "file3"));
    
    // Test getting all files in directory
    const auto files = FileSystem::BE_Get_Files_In_Directory(testDir);
    EXPECT_EQ(files.size(), 3);
    
    // Test getting files with specific extension
    const auto txtFiles = FileSystem::BE_Get_Files_In_Directory(testDir, ".txt");
    EXPECT_EQ(txtFiles.size(), 2);

    const auto datFiles = FileSystem::BE_Get_Files_In_Directory(testDir, ".dat");
    EXPECT_EQ(datFiles.size(), 1);
    
    // Test with non-existent directory
    const auto emptyFiles = FileSystem::BE_Get_Files_In_Directory("nonexistent_directory");
    EXPECT_TRUE(emptyFiles.empty());
}

// Directory listing tests
TEST_F(FileSystemTest, GetDirectoriesInDirectory) {
    // Set up test directory with subdirectories
    EXPECT_TRUE(FileSystem::BE_Create_Directory(testDir));

    const std::string subDir1 = testDir + "/subdir1";
    const std::string subDir2 = testDir + "/subdir2";
    
    EXPECT_TRUE(FileSystem::BE_Create_Directory(subDir1));
    EXPECT_TRUE(FileSystem::BE_Create_Directory(subDir2));
    
    // Also create a file to make sure it's not counted as directory
    EXPECT_TRUE(FileSystem::BE_Write_Text_File(testDir + "/somefile.txt", "content"));
    
    // Test getting directories
    const auto dirs = FileSystem::BE_Get_Directories_In_Directory(testDir);
    EXPECT_EQ(dirs.size(), 2);
    
    // Test with non-existent directory
    const auto emptyDirs = FileSystem::BE_Get_Directories_In_Directory("nonexistent_directory");
    EXPECT_TRUE(emptyDirs.empty());
}

// Path utility tests
TEST_F(FileSystemTest, GetFileName) {
    EXPECT_EQ(FileSystem::BE_Get_File_Name("path/to/file.txt"), "file.txt");
    EXPECT_EQ(FileSystem::BE_Get_File_Name("file.txt"), "file.txt");
    EXPECT_EQ(FileSystem::BE_Get_File_Name("/root/file"), "file");
}

TEST_F(FileSystemTest, GetFileExtension) {
    EXPECT_EQ(FileSystem::BE_Get_File_Extension("path/to/file.txt"), ".txt");
    EXPECT_EQ(FileSystem::BE_Get_File_Extension("file.dat"), ".dat");
    EXPECT_EQ(FileSystem::BE_Get_File_Extension("file"), "");
    EXPECT_EQ(FileSystem::BE_Get_File_Extension("file."), ".");
}

TEST_F(FileSystemTest, GetFileNameWithoutExtension) {
    EXPECT_EQ(FileSystem::BE_Get_File_Name_Without_Extension("path/to/file.txt"), "file");
    EXPECT_EQ(FileSystem::BE_Get_File_Name_Without_Extension("file.dat"), "file");
    EXPECT_EQ(FileSystem::BE_Get_File_Name_Without_Extension("file"), "file");
}

TEST_F(FileSystemTest, GetDirectoryPath) {
    EXPECT_EQ(FileSystem::BE_Get_Directory_Path("path/to/file.txt"), "path/to");
    EXPECT_EQ(FileSystem::BE_Get_Directory_Path("file.txt"), "");
    EXPECT_EQ(FileSystem::BE_Get_Directory_Path("/root/file"), "/root");
}

TEST_F(FileSystemTest, NormalizePath) {
    // Note: The exact result of NormalizePath depends on current working directory
    // so we'll just check that it returns a non-empty string
    EXPECT_FALSE(FileSystem::BE_Normalize_Path("path/to/file.txt").empty());
    EXPECT_FALSE(FileSystem::BE_Normalize_Path("./file.txt").empty());
}

TEST_F(FileSystemTest, CombinePaths) {
    // Since path separators can be different across platforms (/ on Unix, \ on Windows)
    // we should normalize the expected and actual results for comparison

    const std::string result1 = FileSystem::BE_Combine_Paths("path/to", "file.txt");
    // Normalize by converting Windows backslashes to forward slashes if needed
    std::string normalized1 = result1;
    std::replace(normalized1.begin(), normalized1.end(), '\\', '/');
    EXPECT_EQ(normalized1, "path/to/file.txt");

    const std::string result2 = FileSystem::BE_Combine_Paths("path/to/", "file.txt");
    std::string normalized2 = result2;
    std::replace(normalized2.begin(), normalized2.end(), '\\', '/');
    EXPECT_EQ(normalized2, "path/to/file.txt");

    const std::string result3 = FileSystem::BE_Combine_Paths("", "file.txt");
    std::string normalized3 = result3;
    std::replace(normalized3.begin(), normalized3.end(), '\\', '/');
    EXPECT_EQ(normalized3, "file.txt");
}
