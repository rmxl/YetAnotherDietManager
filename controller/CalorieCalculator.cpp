#include "CalorieCalculator.h"

double HarrisBenedictCalculator::calculateDailyCalories(const UserProfile& profile, const Date& date) const {
    int age = profile.getAge(date);
    double weight = profile.getWeight(date);
    int height = profile.getHeightCm();
    Gender gender = profile.getGender();
    ActivityLevel activity = profile.getActivityLevel(date);
    
    double bmr = 0.0;
    
    if (gender == Gender::Male) {
        bmr = 88.362 + (13.397 * weight) + (4.799 * height) - (5.677 * age);
    } else {
        bmr = 447.593 + (9.247 * weight) + (3.098 * height) - (4.330 * age);
    }
    
    double activityMultiplier = 1.55; // Default: moderately active
    
    switch (activity) {
        case ActivityLevel::Sedentary:
            activityMultiplier = 1.2;
            break;
        case ActivityLevel::LightlyActive:
            activityMultiplier = 1.375;
            break;
        case ActivityLevel::ModeratelyActive:
            activityMultiplier = 1.55;
            break;
        case ActivityLevel::VeryActive:
            activityMultiplier = 1.725;
            break;
        case ActivityLevel::ExtremelyActive:
            activityMultiplier = 1.9;
            break;
    }
    
    return bmr * activityMultiplier;
}

std::string HarrisBenedictCalculator::getName() const {
    return "Harris-Benedict Equation";
}

double MifflinStJeorCalculator::calculateDailyCalories(const UserProfile& profile, const Date& date) const {
    int age = profile.getAge(date);
    double weight = profile.getWeight(date);
    int height = profile.getHeightCm();
    Gender gender = profile.getGender();
    ActivityLevel activity = profile.getActivityLevel(date);
    
    double bmr = 0.0;
    
    if (gender == Gender::Male) {
        bmr = (10 * weight) + (6.25 * height) - (5 * age) + 5;
    } else {
        bmr = (10 * weight) + (6.25 * height) - (5 * age) - 161;
    }
    
    double activityMultiplier = 1.55; // Default: moderately active
    
    switch (activity) {
        case ActivityLevel::Sedentary:
            activityMultiplier = 1.2;
            break;
        case ActivityLevel::LightlyActive:
            activityMultiplier = 1.375;
            break;
        case ActivityLevel::ModeratelyActive:
            activityMultiplier = 1.55;
            break;
        case ActivityLevel::VeryActive:
            activityMultiplier = 1.725;
            break;
        case ActivityLevel::ExtremelyActive:
            activityMultiplier = 1.9;
            break;
    }
    
    return bmr * activityMultiplier;
}

std::string MifflinStJeorCalculator::getName() const {
    return "Mifflin-St Jeor Equation";
}

std::shared_ptr<CalorieCalculator> CalorieCalculatorFactory::createCalculator(CalorieCalculationMethod method) {
    switch (method) {
        case CalorieCalculationMethod::Harris_Benedict:
            return std::make_shared<HarrisBenedictCalculator>();
        case CalorieCalculationMethod::Mifflin_St_Jeor:
            return std::make_shared<MifflinStJeorCalculator>();
        default:
            return std::make_shared<HarrisBenedictCalculator>();
    }
}

std::vector<std::shared_ptr<CalorieCalculator>> CalorieCalculatorFactory::getAllCalculators() {
    std::vector<std::shared_ptr<CalorieCalculator>> calculators;
    calculators.push_back(std::make_shared<HarrisBenedictCalculator>());
    calculators.push_back(std::make_shared<MifflinStJeorCalculator>());
    return calculators;
}
