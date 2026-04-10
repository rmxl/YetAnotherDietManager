#ifndef CALORIE_CALCULATOR_H
#define CALORIE_CALCULATOR_H

#include "../model/UserProfile.h"
#include <memory>
#include <string>
#include <vector>

/**
 * Interface for different calorie calculation methods
 */
class CalorieCalculator {
public:
    virtual ~CalorieCalculator() = default;
    virtual double calculateDailyCalories(const UserProfile& profile, const Date& date) const = 0;
    virtual std::string getName() const = 0;
};

/**
 * Harris-Benedict equation for calorie calculation
 */
class HarrisBenedictCalculator : public CalorieCalculator {
public:
    double calculateDailyCalories(const UserProfile& profile, const Date& date) const override;
    std::string getName() const override;
};

/**
 * Mifflin-St Jeor equation for calorie calculation
 */
class MifflinStJeorCalculator : public CalorieCalculator {
public:
    double calculateDailyCalories(const UserProfile& profile, const Date& date) const override;
    std::string getName() const override;
};

/**
 * Factory for creating calorie calculators
 */
class CalorieCalculatorFactory {
public:
    static std::shared_ptr<CalorieCalculator> createCalculator(CalorieCalculationMethod method);
    static std::vector<std::shared_ptr<CalorieCalculator>> getAllCalculators();
};

#endif // CALORIE_CALCULATOR_H
