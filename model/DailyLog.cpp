#include "DailyLog.h"
#include "../util/FileHandler.h"
#include "../util/StringUtils.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

DailyLog::DailyLog(const Date& date) : date(date) {}

Date DailyLog::getDate() const {
    return date;
}

std::vector<LogEntryPtr> DailyLog::getEntries() const {
    return entries;
}

double DailyLog::getTotalCalories() const {
    double total = 0.0;
    for (const auto& entry : entries) {
        total += entry->getCalories();
    }
    return total;
}

void DailyLog::addEntry(const LogEntryPtr& entry) {
    entries.push_back(entry);
}

void DailyLog::removeEntry(int index) {
    if (index >= 0 && index < static_cast<int>(entries.size())) {
        entries.erase(entries.begin() + index);
    }
}

std::string DailyLog::toString() const {
    std::stringstream ss;
    ss << "Daily Log for " << date.toString() << "\n";
    ss << "----------------------------------------\n";
    
    if (entries.empty()) {
        ss << "No food entries for this day.\n";
    } else {
        for (size_t i = 0; i < entries.size(); ++i) {
            ss << i + 1 << ". " << entries[i]->toString() << "\n";
        }
        ss << "----------------------------------------\n";
        ss << "Total Calories: " << std::fixed << std::setprecision(1) << getTotalCalories() << "\n";
    }
    
    return ss.str();
}

std::vector<std::string> DailyLog::toFileStrings() const {
    std::vector<std::string> lines;
    
    // First line is the date
    lines.push_back("DATE|" + date.toString());
    
    // Rest of the lines are entries
    for (const auto& entry : entries) {
        lines.push_back("ENTRY|" + entry->toFileString());
    }
    
    return lines;
}

std::map<Date, DailyLogPtr> DailyLog::loadFromFile(
    const std::string& filename, const FoodDatabasePtr& foodDb) {
    
    std::map<Date, DailyLogPtr> logs;
    
    std::vector<std::string> lines;
    if (!FileHandler::readLines(filename, lines)) {
        std::cerr << "Warning: Could not read log file: " << filename << std::endl;
        return logs;
    }
    
    DailyLogPtr currentLog = nullptr;
    
    for (const auto& line : lines) {
        if (line.empty() || line[0] == '#') {
            continue;  // Skip comments and empty lines
        }
        
        std::vector<std::string> parts = StringUtils::split(line, '|');
        if (parts.size() < 2) {
            continue;  // Invalid line format
        }
        
        if (parts[0] == "DATE") {
            try {
                Date date(parts[1]);
                currentLog = std::make_shared<DailyLog>(date);
                logs[date] = currentLog;
            } catch (const std::exception& e) {
                std::cerr << "Error parsing date from log: " << e.what() << std::endl;
                continue;
            }
        }
        else if (parts[0] == "ENTRY" && currentLog && parts.size() >= 2) {
            std::vector<std::string> entryParts = StringUtils::split(parts[1], '|');
            if (entryParts.size() >= 2) {
                std::string foodId = entryParts[0];
                double servings = std::stod(entryParts[1]);
                
                FoodPtr food = foodDb->getFoodById(foodId);
                if (food) {
                    LogEntryPtr entry = std::make_shared<LogEntry>(food, servings);
                    currentLog->addEntry(entry);
                }
                else {
                    std::cerr << "Warning: Food not found in database: " << foodId << std::endl;
                }
            }
        }
    }
    
    return logs;
}

bool DailyLog::saveToFile(
    const std::string& filename, 
    const std::map<Date, DailyLogPtr>& logs) {
    
    std::vector<std::string> lines;
    
    // Header
    lines.push_back("# Daily Food Log");
    lines.push_back("# Format: DATE|YYYY-MM-DD");
    lines.push_back("# Format: ENTRY|FoodID|Servings");
    
    // Sort logs by date
    std::vector<Date> dates;
    for (const auto& pair : logs) {
        dates.push_back(pair.first);
    }
    
    std::sort(dates.begin(), dates.end());
    
    // Add each log to the file
    for (const auto& date : dates) {
        const auto& log = logs.at(date);
        
        std::vector<std::string> logLines = log->toFileStrings();
        lines.insert(lines.end(), logLines.begin(), logLines.end());
        
        // Add a blank line between logs
        lines.push_back("");
    }
    
    return FileHandler::writeLines(filename, lines);
}
