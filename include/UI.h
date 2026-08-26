#ifndef UI_H
#define UI_H

#include "ExpenseManager.h"

// Handles all console interaction: menus, prompts, and formatted
// output. Delegates every actual data operation to ExpenseManager,
// keeping presentation separate from business logic.
class UI {
private:
    ExpenseManager& manager;

    void printHeader(const std::string& title) const;
    void printSeparator() const;
    void pause() const;

    std::string chooseCategory() const;
    std::string choosePaymentMethod() const;

    void printExpenseTable(const std::vector<Expense>& list) const;
    void printIncomeTable(const std::vector<Income>& list) const;

    void showBudgetWarning(double usedPercent) const;

    // Menu handlers - one per main menu option
    void handleAddExpense();
    void handleAddIncome();
    void handleViewExpenses();
    void handleViewIncome();
    void handleSearch();
    void handleUpdateExpense();
    void handleDeleteExpense();
    void handleFilterExpenses();
    void handleSortExpenses();
    void handleSetBudget();
    void handleMonthlySummary();
    void handleCategoryWise();
    void handleBudgetStatus();
    void handleExportCsv();

    void persist(); // saves all data and reports failure if it happens

public:
    explicit UI(ExpenseManager& mgr);
    void run(); // main application loop
};

#endif // UI_H
