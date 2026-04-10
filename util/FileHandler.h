#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>

/**
 * Utility class for handling file operations
 */
class FileHandler {
public:
    // Read all lines from a file
    static bool readLines(const std::string& filename, std::vector<std::string>& lines);
    
    // Write all lines to a file
    static bool writeLines(const std::string& filename, const std::vector<std::string>& lines);
    
    // Check if a file exists
    static bool fileExists(const std::string& filename);
    
    // Create a backup of a file
    static bool backupFile(const std::string& filename);
};

#endif // FILE_HANDLER_H
