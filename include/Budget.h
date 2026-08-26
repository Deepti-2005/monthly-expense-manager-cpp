#ifndef BUDGET_H
#define BUDGET_H

#include <string>

// Represents a monthly spending limit set by the user.
class Budget {
private:
    int month;   // 1-12
    int year;    // e.g. 2026
    double limit;

public:
    Budget();
    Budget(int month, int year, double limit);

    int getMonth() const;
    int getYear() const;
    double getLimit() const;

    void setLimit(double newLimit);

    std::string toFileString() const;
    static Budget fromFileString(const std::string& line, bool& ok);
};

#endif // BUDGET_H
