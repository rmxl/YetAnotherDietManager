#include "FileHandler.h"
#include <fstream>
#include <iostream>
#include <filesystem>

bool FileHandler::readLines(const std::string& filename, std::vector<std::string>& lines) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return false;
    }
    
    lines.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    file.close();
    return true;
}

bool FileHandler::writeLines(const std::string& filename, const std::vector<std::string>& lines) {
    // Backup the file first if it exists
    if (fileExists(filename)) {
        backupFile(filename);
    }
    
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    
    for (const auto& line : lines) {
        file << line << std::endl;
    }
    
    file.close();
    return true;
}

bool FileHandler::fileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

bool FileHandler::backupFile(const std::string& filename) {
    if (!fileExists(filename)) {
        return false;
    }
    
    std::string backupFilename = filename + ".bak";
    
    try {
        std::filesystem::copy_file(filename, backupFilename, 
                                 std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating backup: " << e.what() << std::endl;
        return false;
    }
}
