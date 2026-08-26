#include "Expense.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

Expense::Expense()
    : id(0), date(""), category(""), amount(0.0), description(""), paymentMethod("") {}

Expense::Expense(int id,
                  const std::string& date,
                  const std::string& category,
                  double amount,
                  const std::string& description,
                  const std::string& paymentMethod)
    : id(id), date(date), category(category), amount(amount),
      description(description), paymentMethod(paymentMethod) {}

int Expense::getId() const { return id; }
std::string Expense::getDate() const { return date; }
std::string Expense::getCategory() const { return category; }
double Expense::getAmount() const { return amount; }
std::string Expense::getDescription() const { return description; }
std::string Expense::getPaymentMethod() const { return paymentMethod; }

void Expense::setDate(const std::string& newDate) { date = newDate; }
void Expense::setCategory(const std::string& newCategory) { category = newCategory; }
void Expense::setAmount(double newAmount) { amount = newAmount; }
void Expense::setDescription(const std::string& newDescription) { description = newDescription; }
void Expense::setPaymentMethod(const std::string& newPaymentMethod) { paymentMethod = newPaymentMethod; }

std::string Expense::toFileString() const {
    std::ostringstream oss;
    oss << id << "|" << date << "|" << category << "|"
        << std::fixed << std::setprecision(2) << amount << "|"
        << description << "|" << paymentMethod;
    return oss.str();
}

// Splits a pipe-delimited line into fields, preserving empty fields.
static std::vector<std::string> splitPipe(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, '|')) {
        fields.push_back(field);
    }
    return fields;
}

Expense Expense::fromFileString(const std::string& line, bool& ok) {
    ok = false;
    std::vector<std::string> f = splitPipe(line);
    if (f.size() != 6) {
        return Expense();
    }
    try {
        int parsedId = std::stoi(f[0]);
        double parsedAmount = std::stod(f[3]);
        ok = true;
        return Expense(parsedId, f[1], f[2], parsedAmount, f[4], f[5]);
    } catch (const std::exception&) {
        ok = false;
        return Expense();
    }
}

void Expense::printRow() const {
    std::cout << std::left
               << std::setw(6) << id
               << std::setw(14) << date
               << std::setw(16) << category
               << std::right << std::setw(10) << std::fixed << std::setprecision(2) << amount
               << "  " << std::left << std::setw(14) << paymentMethod
               << description
               << "\n";
}

namespace ExpenseConstants {

const std::vector<std::string>& categories() {
    static const std::vector<std::string> cats = {
        "Food", "Travel", "Shopping", "Bills", "Education",
        "Entertainment", "Health", "Rent", "Groceries", "Other"
    };
    return cats;
}

const std::vector<std::string>& paymentMethods() {
    static const std::vector<std::string> methods = {
        "Cash", "UPI", "Debit Card", "Credit Card", "Bank Transfer", "Other"
    };
    return methods;
}

static std::string toLowerCopy(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return out;
}

bool isValidCategory(const std::string& category) {
    std::string lower = toLowerCopy(category);
    for (const auto& c : categories()) {
        if (toLowerCopy(c) == lower) return true;
    }
    return false;
}

bool isValidPaymentMethod(const std::string& method) {
    std::string lower = toLowerCopy(method);
    for (const auto& m : paymentMethods()) {
        if (toLowerCopy(m) == lower) return true;
    }
    return false;
}

} // namespace ExpenseConstants
