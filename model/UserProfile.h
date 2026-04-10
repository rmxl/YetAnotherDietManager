#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include "Date.h"
#include <string>
#include <map>
#include <memory>

enum class Gender { Male, Female };
enum class ActivityLevel { Sedentary, LightlyActive, ModeratelyActive, VeryActive, ExtremelyActive };
enum class CalorieCalculationMethod { Harris_Benedict, Mifflin_St_Jeor, FixedGoal };

/**
 * Represents the user's profile with personal information and daily metrics
 */
class UserProfile {
public:
    UserProfile();
    UserProfile(const std::string& name, Gender gender, int heightCm);
    
    // Basic profile accessors
    std::string getName() const;
    Gender getGender() const;
    int getHeightCm() const;
    CalorieCalculationMethod getCalorieMethod() const;
    double getFixedCalorieGoal() const;
    
    void setName(const std::string& name);
    void setGender(Gender gender);
    void setHeightCm(int heightCm);
    void setCalorieMethod(CalorieCalculationMethod method);
    void setFixedCalorieGoal(double calories);
    
    // Daily metric accessors
    int getAge(const Date& date) const;
    double getWeight(const Date& date) const;
    ActivityLevel getActivityLevel(const Date& date) const;
    
    // Daily metric modifiers
    void setAge(const Date& date, int age);
    void setWeight(const Date& date, double weight);
    void setActivityLevel(const Date& date, ActivityLevel level);
    
    // Get target calories based on the current method
    double getTargetCalories(const Date& date) const;
    
    // Load and save to file
    static std::shared_ptr<UserProfile> loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // String conversion for enums
    static std::string genderToString(Gender gender);
    static Gender stringToGender(const std::string& str);
    
    static std::string activityLevelToString(ActivityLevel level);
    static ActivityLevel stringToActivityLevel(const std::string& str);
    
    static std::string calorieMethodToString(CalorieCalculationMethod method);
    static CalorieCalculationMethod stringToCalorieMethod(const std::string& str);
    
private:
    // Basic profile information
    std::string name;
    Gender gender;
    int heightCm;
    CalorieCalculationMethod calorieMethod;
    double fixedCalorieGoal;
    
    // Daily metrics (mapped by date)
    std::map<Date, int> ageByDate;
    std::map<Date, double> weightByDate;
    std::map<Date, ActivityLevel> activityLevelByDate;
    
    // Helper methods for calculating target calories
    double calculateHarrisBenedict(Gender gender, int age, double weightKg, int heightCm, ActivityLevel level) const;
    double calculateMifflinStJeor(Gender gender, int age, double weightKg, int heightCm, ActivityLevel level) const;
    double getActivityMultiplier(ActivityLevel level) const;
};

typedef std::shared_ptr<UserProfile> UserProfilePtr;

#endif // USER_PROFILE_H
