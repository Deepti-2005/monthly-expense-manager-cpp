#include "Validation.h"
#include <iostream>
#include <sstream>
#include <limits>
#include <cctype>
#include <algorithm>
#include <cmath>

namespace Validation {

bool isValidDate(const std::string& date) {
    // Strict format check: YYYY-MM-DD (10 characters, dashes at 4 and 7)
    if (date.size() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i]))) return false;
    }

    int year, month, day;
    try {
        year = std::stoi(date.substr(0, 4));
        month = std::stoi(date.substr(5, 2));
        day = std::stoi(date.substr(8, 2));
    } catch (const std::exception&) {
        return false;
    }

    if (year < 1900 || year > 2200) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;

    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];

    // Leap year check for February
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap) maxDay = 29;
    }

    return day <= maxDay;
}

bool isValidAmount(double amount) {
    return amount > 0.0 && std::isfinite(amount);
}

bool isNonEmpty(const std::string& text) {
    for (char c : text) {
        if (!std::isspace(static_cast<unsigned char>(c))) return true;
    }
    return false;
}

void clearInputStream() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readIntInRange(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            // Stream failed (e.g. EOF) - clear and retry to keep app alive
            std::cin.clear();
            continue;
        }

        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) {
            std::cout << "[ERROR] Please enter a number.\n";
            continue;
        }
        line = line.substr(start, end - start + 1);

        std::istringstream iss(line);
        int value;
        char extra;
        if ((iss >> value) && !(iss >> extra)) {
            if (value >= minValue && value <= maxValue) {
                return value;
            }
            std::cout << "[ERROR] Please enter a value between "
                      << minValue << " and " << maxValue << ".\n";
        } else {
            std::cout << "[ERROR] Invalid input. Please enter a whole number.\n";
        }
    }
}

double readPositiveAmount(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            continue;
        }

        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) {
            std::cout << "[ERROR] Please enter a number.\n";
            continue;
        }
        line = line.substr(start, end - start + 1);

        std::istringstream iss(line);
        double value;
        char extra;
        if ((iss >> value) && !(iss >> extra)) {
            if (isValidAmount(value)) {
                return value;
            }
            std::cout << "[ERROR] Invalid amount. Please enter a positive number.\n";
        } else {
            std::cout << "[ERROR] Invalid input. Please enter a numeric amount.\n";
        }
    }
}

std::string readNonEmptyLine(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);

        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) {
            std::cout << "[ERROR] This field cannot be empty.\n";
            continue;
        }
        return line.substr(start, end - start + 1);
    }
}

std::string readValidDate(const std::string& prompt) {
    while (true) {
        std::string date = readNonEmptyLine(prompt);
        if (isValidDate(date)) {
            return date;
        }
        std::cout << "[ERROR] Invalid date. Please use format YYYY-MM-DD (e.g. 2026-08-26).\n";
    }
}

bool readConfirmation(const std::string& prompt) {
    while (true) {
        std::string line = readNonEmptyLine(prompt);
        if (line.size() == 1) {
            char c = std::tolower(static_cast<unsigned char>(line[0]));
            if (c == 'y') return true;
            if (c == 'n') return false;
        }
        std::cout << "[ERROR] Please enter Y or N.\n";
    }
}

} // namespace Validation
