#include "Budget.h"
#include <sstream>
#include <iomanip>
#include <vector>

Budget::Budget() : month(0), year(0), limit(0.0) {}

Budget::Budget(int month, int year, double limit)
    : month(month), year(year), limit(limit) {}

int Budget::getMonth() const { return month; }
int Budget::getYear() const { return year; }
double Budget::getLimit() const { return limit; }

void Budget::setLimit(double newLimit) { limit = newLimit; }

std::string Budget::toFileString() const {
    std::ostringstream oss;
    oss << month << "|" << year << "|" << std::fixed << std::setprecision(2) << limit;
    return oss.str();
}

static std::vector<std::string> splitPipeBudget(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, '|')) {
        fields.push_back(field);
    }
    return fields;
}

Budget Budget::fromFileString(const std::string& line, bool& ok) {
    ok = false;
    std::vector<std::string> f = splitPipeBudget(line);
    if (f.size() != 3) {
        return Budget();
    }
    try {
        int m = std::stoi(f[0]);
        int y = std::stoi(f[1]);
        double l = std::stod(f[2]);
        ok = true;
        return Budget(m, y, l);
    } catch (const std::exception&) {
        ok = false;
        return Budget();
    }
}
