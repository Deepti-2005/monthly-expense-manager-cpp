#include "Income.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

Income::Income()
    : id(0), date(""), source(""), amount(0.0), description("") {}

Income::Income(int id,
               const std::string& date,
               const std::string& source,
               double amount,
               const std::string& description)
    : id(id), date(date), source(source), amount(amount), description(description) {}

int Income::getId() const { return id; }
std::string Income::getDate() const { return date; }
std::string Income::getSource() const { return source; }
double Income::getAmount() const { return amount; }
std::string Income::getDescription() const { return description; }

void Income::setDate(const std::string& newDate) { date = newDate; }
void Income::setSource(const std::string& newSource) { source = newSource; }
void Income::setAmount(double newAmount) { amount = newAmount; }
void Income::setDescription(const std::string& newDescription) { description = newDescription; }

std::string Income::toFileString() const {
    std::ostringstream oss;
    oss << id << "|" << date << "|" << source << "|"
        << std::fixed << std::setprecision(2) << amount << "|" << description;
    return oss.str();
}

static std::vector<std::string> splitPipeIncome(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, '|')) {
        fields.push_back(field);
    }
    return fields;
}

Income Income::fromFileString(const std::string& line, bool& ok) {
    ok = false;
    std::vector<std::string> f = splitPipeIncome(line);
    if (f.size() != 5) {
        return Income();
    }
    try {
        int parsedId = std::stoi(f[0]);
        double parsedAmount = std::stod(f[3]);
        ok = true;
        return Income(parsedId, f[1], f[2], parsedAmount, f[4]);
    } catch (const std::exception&) {
        ok = false;
        return Income();
    }
}

void Income::printRow() const {
    std::cout << std::left
               << std::setw(6) << id
               << std::setw(14) << date
               << std::setw(18) << source
               << std::right << std::setw(10) << std::fixed << std::setprecision(2) << amount
               << "  " << std::left << description
               << "\n";
}
