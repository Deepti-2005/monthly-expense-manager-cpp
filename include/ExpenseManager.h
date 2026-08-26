#ifndef EXPENSEMANAGER_H
#define EXPENSEMANAGER_H

#include <vector>
#include <string>
#include <map>
#include "Expense.h"
#include "Income.h"
#include "Budget.h"
#include "FileManager.h"

// Summary of a single month's financial activity, computed on demand.
struct MonthlySummary {
    bool hasData = false;
    double totalIncome = 0.0;
    double totalExpenses = 0.0;
    double remainingBalance = 0.0;
    bool hasBudget = false;
    double budgetLimit = 0.0;
    double budgetUsedPercent = 0.0;
    double budgetRemaining = 0.0;
    int expenseCount = 0;
    int incomeCount = 0;
    double highestExpense = 0.0;
    double averageExpense = 0.0;
};

// Which field to search transactions by.
enum class SearchField { Id, Category, Date, Description, PaymentMethod };

// Which field to sort expenses by.
enum class SortField { AmountAsc, AmountDesc, DateAsc, DateDesc, CategoryAsc };

// Central business-logic class. Owns the in-memory collections of
// expenses, income, and budgets, and provides all operations for
// manipulating and analyzing them. Delegates persistence to FileManager
// and never talks to std::cin/std::cout directly (that is UI's job).
class ExpenseManager {
private:
    std::vector<Expense> expenses;
    std::vector<Income> incomeRecords;
    std::vector<Budget> budgets;

    FileManager fileManager;

    int nextExpenseId;
    int nextIncomeId;

    int computeNextId(const std::vector<Expense>& items) const;
    int computeNextIncomeId(const std::vector<Income>& items) const;

    static std::string toLower(const std::string& s);
    static bool sameMonthYear(const std::string& date, int month, int year);

public:
    explicit ExpenseManager(const FileManager& fm);

    // Loads all persisted data into memory. Call once at startup.
    void loadAll();

    // Persists all in-memory data to disk. Call after every mutation
    // and again before the program exits.
    bool saveAll() const;

    // ---- Expense operations ----
    int addExpense(const std::string& date, const std::string& category,
                   double amount, const std::string& description,
                   const std::string& paymentMethod);

    bool updateExpense(int id, const std::string& date, const std::string& category,
                        double amount, const std::string& description,
                        const std::string& paymentMethod);

    bool deleteExpense(int id);

    const Expense* findExpenseById(int id) const;

    const std::vector<Expense>& getAllExpenses() const;

    std::vector<Expense> searchExpenses(SearchField field, const std::string& query) const;

    std::vector<Expense> filterByMonth(int month, int year) const;
    std::vector<Expense> filterByCategory(const std::string& category) const;
    std::vector<Expense> filterByPaymentMethod(const std::string& method) const;
    std::vector<Expense> filterByDateRange(const std::string& startDate, const std::string& endDate) const;

    std::vector<Expense> sortExpenses(const std::vector<Expense>& source, SortField field) const;

    double totalExpenseAmount(const std::vector<Expense>& source) const;

    // ---- Income operations ----
    int addIncome(const std::string& date, const std::string& source,
                  double amount, const std::string& description);

    const std::vector<Income>& getAllIncome() const;

    double totalIncomeAmount(const std::vector<Income>& source) const;
    std::vector<Income> incomeForMonth(int month, int year) const;

    // ---- Budget operations ----
    void setBudget(int month, int year, double limit);
    const Budget* getBudget(int month, int year) const;

    // ---- Reporting ----
    MonthlySummary getMonthlySummary(int month, int year) const;
    std::map<std::string, double> categoryWiseTotals(const std::vector<Expense>& source) const;
};

#endif // EXPENSEMANAGER_H
