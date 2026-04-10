#ifndef DAILY_LOG_H
#define DAILY_LOG_H

#include "Date.h"
#include "LogEntry.h"
#include "FoodDatabase.h"
#include <vector>
#include <memory>
#include <string>
#include <map>

/**
 * Represents a daily food consumption log
 */
class DailyLog {
public:
    DailyLog(const Date& date);
    
    // Accessors
    Date getDate() const;
    std::vector<LogEntryPtr> getEntries() const;
    double getTotalCalories() const;
    
    // Modifiers
    void addEntry(const LogEntryPtr& entry);
    void removeEntry(int index);
    
    // String representation
    std::string toString() const;
    std::vector<std::string> toFileStrings() const;
    
    // Static methods for loading all logs from file
    static std::map<Date, std::shared_ptr<DailyLog>> loadFromFile(
        const std::string& filename, const FoodDatabasePtr& foodDb);
    
    // Static method to save all logs to file
    static bool saveToFile(
        const std::string& filename, 
        const std::map<Date, std::shared_ptr<DailyLog>>& logs);
    
private:
    Date date;
    std::vector<LogEntryPtr> entries;
};

typedef std::shared_ptr<DailyLog> DailyLogPtr;

#endif // DAILY_LOG_H
