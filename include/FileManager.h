#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include "Expense.h"
#include "Income.h"
#include "Budget.h"

// Responsible for all disk I/O: loading and saving expenses, income,
// and budgets to plain delimiter-based text files. Keeps file-format
// details out of the business logic layer.
class FileManager {
private:
    std::string expenseFilePath;
    std::string incomeFilePath;
    std::string budgetFilePath;

    // Ensures the parent directory of the given file path exists.
    void ensureDirectoryExists(const std::string& filePath) const;

    // Creates an empty file if it does not already exist.
    void ensureFileExists(const std::string& filePath) const;

public:
    FileManager(const std::string& expenseFile = "data/expenses.txt",
                const std::string& incomeFile = "data/income.txt",
                const std::string& budgetFile = "data/budgets.txt");

    // Loading. Malformed lines are skipped (not fatal). Returns true
    // even if the file was empty or missing (treated as no data).
    std::vector<Expense> loadExpenses() const;
    std::vector<Income> loadIncome() const;
    std::vector<Budget> loadBudgets() const;

    // Saving. Overwrites the file with the current in-memory state.
    // Returns false if the file could not be opened for writing.
    bool saveExpenses(const std::vector<Expense>& expenses) const;
    bool saveIncome(const std::vector<Income>& income) const;
    bool saveBudgets(const std::vector<Budget>& budgets) const;
};

#endif // FILEMANAGER_H
