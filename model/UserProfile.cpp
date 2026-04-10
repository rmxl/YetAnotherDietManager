#include "UserProfile.h"
#include "../util/FileHandler.h"
#include "../util/StringUtils.h"
#include <fstream>
#include <sstream>
#include <iostream>

UserProfile::UserProfile() 
    : name("User"), gender(Gender::Male), heightCm(170), 
      calorieMethod(CalorieCalculationMethod::Harris_Benedict), fixedCalorieGoal(2000.0) {
    
    // Default values for today
    Date today = Date::today();
    ageByDate[today] = 30;
    weightByDate[today] = 70.0;
    activityLevelByDate[today] = ActivityLevel::ModeratelyActive;
}

UserProfile::UserProfile(const std::string& name, Gender gender, int heightCm)
    : name(name), gender(gender), heightCm(heightCm), 
      calorieMethod(CalorieCalculationMethod::Harris_Benedict), fixedCalorieGoal(2000.0) {
    
    // Default values for today
    Date today = Date::today();
    ageByDate[today] = 30;
    weightByDate[today] = 70.0;
    activityLevelByDate[today] = ActivityLevel::ModeratelyActive;
}

std::string UserProfile::getName() const {
    return name;
}

Gender UserProfile::getGender() const {
    return gender;
}

int UserProfile::getHeightCm() const {
    return heightCm;
}

CalorieCalculationMethod UserProfile::getCalorieMethod() const {
    return calorieMethod;
}

double UserProfile::getFixedCalorieGoal() const {
    return fixedCalorieGoal;
}

void UserProfile::setName(const std::string& name) {
    this->name = name;
}

void UserProfile::setGender(Gender gender) {
    this->gender = gender;
}

void UserProfile::setHeightCm(int heightCm) {
    this->heightCm = heightCm;
}

void UserProfile::setCalorieMethod(CalorieCalculationMethod method) {
    this->calorieMethod = method;
}

void UserProfile::setFixedCalorieGoal(double calories) {
    this->fixedCalorieGoal = calories;
}

int UserProfile::getAge(const Date& date) const {
    // Find the nearest date not after the requested date
    auto it = ageByDate.upper_bound(date);
    if (it == ageByDate.begin()) {
        // No entries before this date, return default
        return 30;
    }
    --it; // Get the last entry not after date
    return it->second;
}

double UserProfile::getWeight(const Date& date) const {
    // Find the nearest date not after the requested date
    auto it = weightByDate.upper_bound(date);
    if (it == weightByDate.begin()) {
        // No entries before this date, return default
        return 70.0;
    }
    --it; // Get the last entry not after date
    return it->second;
}

ActivityLevel UserProfile::getActivityLevel(const Date& date) const {
    // Find the nearest date not after the requested date
    auto it = activityLevelByDate.upper_bound(date);
    if (it == activityLevelByDate.begin()) {
        // No entries before this date, return default
        return ActivityLevel::ModeratelyActive;
    }
    --it; // Get the last entry not after date
    return it->second;
}

void UserProfile::setAge(const Date& date, int age) {
    ageByDate[date] = age;
}

void UserProfile::setWeight(const Date& date, double weight) {
    weightByDate[date] = weight;
}

void UserProfile::setActivityLevel(const Date& date, ActivityLevel level) {
    activityLevelByDate[date] = level;
}

double UserProfile::getTargetCalories(const Date& date) const {
    int age = getAge(date);
    double weight = getWeight(date);
    ActivityLevel level = getActivityLevel(date);
    
    switch (calorieMethod) {
        case CalorieCalculationMethod::Harris_Benedict:
            return calculateHarrisBenedict(gender, age, weight, heightCm, level);
        case CalorieCalculationMethod::Mifflin_St_Jeor:
            return calculateMifflinStJeor(gender, age, weight, heightCm, level);
        case CalorieCalculationMethod::FixedGoal:
            return fixedCalorieGoal;
        default:
            return calculateHarrisBenedict(gender, age, weight, heightCm, level);
    }
}

double UserProfile::calculateHarrisBenedict(Gender gender, int age, double weightKg, int heightCm, ActivityLevel level) const {
    double bmr = 0.0;
    
    if (gender == Gender::Male) {
        bmr = 88.362 + (13.397 * weightKg) + (4.799 * heightCm) - (5.677 * age);
    } else {
        bmr = 447.593 + (9.247 * weightKg) + (3.098 * heightCm) - (4.330 * age);
    }
    
    double activityMultiplier = getActivityMultiplier(level);
    return bmr * activityMultiplier;
}

double UserProfile::calculateMifflinStJeor(Gender gender, int age, double weightKg, int heightCm, ActivityLevel level) const {
    double bmr = 0.0;
    
    if (gender == Gender::Male) {
        bmr = (10 * weightKg) + (6.25 * heightCm) - (5 * age) + 5;
    } else {
        bmr = (10 * weightKg) + (6.25 * heightCm) - (5 * age) - 161;
    }
    
    double activityMultiplier = getActivityMultiplier(level);
    return bmr * activityMultiplier;
}

double UserProfile::getActivityMultiplier(ActivityLevel level) const {
    switch (level) {
        case ActivityLevel::Sedentary:
            return 1.2;
        case ActivityLevel::LightlyActive:
            return 1.375;
        case ActivityLevel::ModeratelyActive:
            return 1.55;
        case ActivityLevel::VeryActive:
            return 1.725;
        case ActivityLevel::ExtremelyActive:
            return 1.9;
        default:
            return 1.55;
    }
}

std::shared_ptr<UserProfile> UserProfile::loadFromFile(const std::string& filename) {
    std::vector<std::string> lines;
    if (!FileHandler::readLines(filename, lines)) {
        std::cerr << "Warning: Could not read profile file: " << filename << std::endl;
        return std::make_shared<UserProfile>();
    }
    
    auto profile = std::make_shared<UserProfile>();
    
    for (const auto& line : lines) {
        if (line.empty() || line[0] == '#') {
            continue;  // Skip comments and empty lines
        }
        
        std::vector<std::string> parts = StringUtils::split(line, '|');
        if (parts.size() < 2) {
            continue;
        }
        
        std::string key = parts[0];
        std::string value = parts[1];
        
        if (key == "NAME") {
            profile->setName(value);
        }
        else if (key == "GENDER") {
            profile->setGender(stringToGender(value));
        }
        else if (key == "HEIGHT") {
            profile->setHeightCm(std::stoi(value));
        }
        else if (key == "CALORIE_METHOD") {
            profile->setCalorieMethod(stringToCalorieMethod(value));
        }
        else if (key == "FIXED_CALORIE_GOAL") {
            profile->setFixedCalorieGoal(std::stod(value));
        }
        else if (key == "AGE" && parts.size() >= 3) {
            try {
                Date date(parts[1]);
                int age = std::stoi(parts[2]);
                profile->setAge(date, age);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing profile data: " << e.what() << std::endl;
            }
        }
        else if (key == "WEIGHT" && parts.size() >= 3) {
            try {
                Date date(parts[1]);
                double weight = std::stod(parts[2]);
                profile->setWeight(date, weight);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing profile data: " << e.what() << std::endl;
            }
        }
        else if (key == "ACTIVITY" && parts.size() >= 3) {
            try {
                Date date(parts[1]);
                ActivityLevel level = stringToActivityLevel(parts[2]);
                profile->setActivityLevel(date, level);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing profile data: " << e.what() << std::endl;
            }
        }
    }
    
    return profile;
}

bool UserProfile::saveToFile(const std::string& filename) const {
    std::vector<std::string> lines;
    
    // Header
    lines.push_back("# User Profile");
    
    // Basic information
    lines.push_back("NAME|" + name);
    lines.push_back("GENDER|" + genderToString(gender));
    lines.push_back("HEIGHT|" + std::to_string(heightCm));
    lines.push_back("CALORIE_METHOD|" + calorieMethodToString(calorieMethod));
    
    // Fixed calorie goal (only needed if method is FixedGoal)
    if (calorieMethod == CalorieCalculationMethod::FixedGoal) {
        lines.push_back("FIXED_CALORIE_GOAL|" + std::to_string(fixedCalorieGoal));
    }
    
    // Daily metrics
    for (const auto& pair : ageByDate) {
        lines.push_back("AGE|" + pair.first.toString() + "|" + std::to_string(pair.second));
    }
    
    for (const auto& pair : weightByDate) {
        std::stringstream ss;
        ss << "WEIGHT|" << pair.first.toString() << "|" << pair.second;
        lines.push_back(ss.str());
    }
    
    for (const auto& pair : activityLevelByDate) {
        lines.push_back("ACTIVITY|" + pair.first.toString() + "|" + activityLevelToString(pair.second));
    }
    
    return FileHandler::writeLines(filename, lines);
}

std::string UserProfile::genderToString(Gender gender) {
    switch (gender) {
        case Gender::Male:
            return "Male";
        case Gender::Female:
            return "Female";
        default:
            return "Unknown";
    }
}

Gender UserProfile::stringToGender(const std::string& str) {
    if (str == "Male") return Gender::Male;
    if (str == "Female") return Gender::Female;
    return Gender::Male;  // Default
}

std::string UserProfile::activityLevelToString(ActivityLevel level) {
    switch (level) {
        case ActivityLevel::Sedentary:
            return "Sedentary";
        case ActivityLevel::LightlyActive:
            return "LightlyActive";
        case ActivityLevel::ModeratelyActive:
            return "ModeratelyActive";
        case ActivityLevel::VeryActive:
            return "VeryActive";
        case ActivityLevel::ExtremelyActive:
            return "ExtremelyActive";
        default:
            return "ModeratelyActive";
    }
}

ActivityLevel UserProfile::stringToActivityLevel(const std::string& str) {
    if (str == "Sedentary") return ActivityLevel::Sedentary;
    if (str == "LightlyActive") return ActivityLevel::LightlyActive;
    if (str == "ModeratelyActive") return ActivityLevel::ModeratelyActive;
    if (str == "VeryActive") return ActivityLevel::VeryActive;
    if (str == "ExtremelyActive") return ActivityLevel::ExtremelyActive;
    return ActivityLevel::ModeratelyActive;  // Default
}

std::string UserProfile::calorieMethodToString(CalorieCalculationMethod method) {
    switch (method) {
        case CalorieCalculationMethod::Harris_Benedict:
            return "Harris_Benedict";
        case CalorieCalculationMethod::Mifflin_St_Jeor:
            return "Mifflin_St_Jeor";
        case CalorieCalculationMethod::FixedGoal:
            return "FixedGoal";
        default:
            return "Harris_Benedict";
    }
}

CalorieCalculationMethod UserProfile::stringToCalorieMethod(const std::string& str) {
    if (str == "Harris_Benedict") return CalorieCalculationMethod::Harris_Benedict;
    if (str == "Mifflin_St_Jeor") return CalorieCalculationMethod::Mifflin_St_Jeor;
    if (str == "FixedGoal") return CalorieCalculationMethod::FixedGoal;
    return CalorieCalculationMethod::Harris_Benedict;  // Default
}
