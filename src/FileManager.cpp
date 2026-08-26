#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

FileManager::FileManager(const std::string& expenseFile,
                          const std::string& incomeFile,
                          const std::string& budgetFile)
    : expenseFilePath(expenseFile), incomeFilePath(incomeFile), budgetFilePath(budgetFile) {
    ensureDirectoryExists(expenseFilePath);
    ensureDirectoryExists(incomeFilePath);
    ensureDirectoryExists(budgetFilePath);
    ensureFileExists(expenseFilePath);
    ensureFileExists(incomeFilePath);
    ensureFileExists(budgetFilePath);
}

void FileManager::ensureDirectoryExists(const std::string& filePath) const {
    size_t pos = filePath.find_last_of("/\\");
    if (pos == std::string::npos) return; // no directory component

    std::string dir = filePath.substr(0, pos);
    if (dir.empty()) return;

    // POSIX directory creation. Ignores errors such as "already exists".
    #if defined(_WIN32)
        _mkdir(dir.c_str());
    #else
        mkdir(dir.c_str(), 0755);
    #endif
}

void FileManager::ensureFileExists(const std::string& filePath) const {
    std::ifstream check(filePath);
    if (!check.good()) {
        std::ofstream create(filePath); // creates an empty file
    }
}

std::vector<Expense> FileManager::loadExpenses() const {
    std::vector<Expense> expenses;
    std::ifstream file(expenseFilePath);
    if (!file.is_open()) {
        return expenses; // treated as empty, not fatal
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        bool ok = false;
        Expense e = Expense::fromFileString(line, ok);
        if (ok) {
            expenses.push_back(e);
        }
        // malformed lines are silently skipped to avoid crashing
    }
    return expenses;
}

std::vector<Income> FileManager::loadIncome() const {
    std::vector<Income> incomeList;
    std::ifstream file(incomeFilePath);
    if (!file.is_open()) {
        return incomeList;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        bool ok = false;
        Income inc = Income::fromFileString(line, ok);
        if (ok) {
            incomeList.push_back(inc);
        }
    }
    return incomeList;
}

std::vector<Budget> FileManager::loadBudgets() const {
    std::vector<Budget> budgets;
    std::ifstream file(budgetFilePath);
    if (!file.is_open()) {
        return budgets;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        bool ok = false;
        Budget b = Budget::fromFileString(line, ok);
        if (ok) {
            budgets.push_back(b);
        }
    }
    return budgets;
}

bool FileManager::saveExpenses(const std::vector<Expense>& expenses) const {
    std::ofstream file(expenseFilePath, std::ios::trunc);
    if (!file.is_open()) return false;
    for (const auto& e : expenses) {
        file << e.toFileString() << "\n";
    }
    return true;
}

bool FileManager::saveIncome(const std::vector<Income>& income) const {
    std::ofstream file(incomeFilePath, std::ios::trunc);
    if (!file.is_open()) return false;
    for (const auto& i : income) {
        file << i.toFileString() << "\n";
    }
    return true;
}

bool FileManager::saveBudgets(const std::vector<Budget>& budgets) const {
    std::ofstream file(budgetFilePath, std::ios::trunc);
    if (!file.is_open()) return false;
    for (const auto& b : budgets) {
        file << b.toFileString() << "\n";
    }
    return true;
}
