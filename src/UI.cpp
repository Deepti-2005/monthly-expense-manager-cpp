#include "UI.h"
#include "Validation.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

namespace {
    std::string csvEscape(const std::string& value) {
        bool needsQuotes = value.find(',') != std::string::npos ||
                           value.find('"') != std::string::npos ||
                           value.find('\n') != std::string::npos ||
                           value.find('\r') != std::string::npos;
        if (!needsQuotes) return value;

        std::string escaped;
        escaped.reserve(value.size() + 2);
        escaped += '"';
        for (char ch : value) {
            if (ch == '"') {
                escaped += '"';
            }
            escaped += ch;
        }
        escaped += '"';
        return escaped;
    }
}

UI::UI(ExpenseManager& mgr) : manager(mgr) {}

void UI::printHeader(const std::string& title) const {
    std::cout << "\n========================================\n";
    std::cout << "   " << title << "\n";
    std::cout << "========================================\n";
}

void UI::printSeparator() const {
    std::cout << std::string(80, '-') << "\n";
}

void UI::pause() const {
    std::cout << "\nPress Enter to return to the menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void UI::persist() {
    if (!manager.saveAll()) {
        std::cout << "[ERROR] Failed to save data to disk. Changes may be lost.\n";
    }
}

std::string UI::chooseCategory() const {
    const auto& cats = ExpenseConstants::categories();
    std::cout << "\nSelect Category:\n";
    for (size_t i = 0; i < cats.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << cats[i] << "\n";
    }
    int choice = Validation::readIntInRange("Enter choice: ", 1, static_cast<int>(cats.size()));
    return cats[choice - 1];
}

std::string UI::choosePaymentMethod() const {
    const auto& methods = ExpenseConstants::paymentMethods();
    std::cout << "\nSelect Payment Method:\n";
    for (size_t i = 0; i < methods.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << methods[i] << "\n";
    }
    int choice = Validation::readIntInRange("Enter choice: ", 1, static_cast<int>(methods.size()));
    return methods[choice - 1];
}

void UI::printExpenseTable(const std::vector<Expense>& list) const {
    if (list.empty()) {
        std::cout << "\nNo expenses found.\n";
        return;
    }

    std::cout << "\n";
    printSeparator();
    std::cout << std::left
               << std::setw(6) << "ID"
               << std::setw(14) << "Date"
               << std::setw(16) << "Category"
               << std::right << std::setw(10) << "Amount"
               << "  " << std::left << std::setw(14) << "Payment"
               << "Description" << "\n";
    printSeparator();

    for (const auto& e : list) {
        e.printRow();
    }
    printSeparator();

    double total = manager.totalExpenseAmount(list);
    std::cout << "Total Expenses: Rs. " << std::fixed << std::setprecision(2) << total << "\n";
}

void UI::printIncomeTable(const std::vector<Income>& list) const {
    if (list.empty()) {
        std::cout << "\nNo income records found.\n";
        return;
    }

    std::cout << "\n";
    printSeparator();
    std::cout << std::left
               << std::setw(6) << "ID"
               << std::setw(14) << "Date"
               << std::setw(18) << "Source"
               << std::right << std::setw(10) << "Amount"
               << "  " << std::left << "Description" << "\n";
    printSeparator();

    for (const auto& i : list) {
        i.printRow();
    }
    printSeparator();

    double total = manager.totalIncomeAmount(list);
    std::cout << "Total Income: Rs. " << std::fixed << std::setprecision(2) << total << "\n";
}

void UI::showBudgetWarning(double usedPercent) const {
    if (usedPercent >= 100.0) {
        std::cout << "\n[ALERT] You have exceeded your monthly budget.\n";
    } else if (usedPercent >= 80.0) {
        std::cout << "\n[WARNING] You have used more than 80% of your monthly budget.\n";
    }
}

// ---------------- Menu handlers ----------------

void UI::handleAddExpense() {
    printHeader("ADD EXPENSE");

    std::string date = Validation::readValidDate("Enter date (YYYY-MM-DD): ");
    std::string category = chooseCategory();
    double amount = Validation::readPositiveAmount("Enter amount: Rs. ");
    std::string description = Validation::readNonEmptyLine("Enter description: ");
    std::string paymentMethod = choosePaymentMethod();

    int id = manager.addExpense(date, category, amount, description, paymentMethod);
    persist();

    std::cout << "\n[SUCCESS] Expense added successfully.\n";
    std::cout << "Expense ID: " << id << "\n";

    int month = std::stoi(date.substr(5, 2));
    int year = std::stoi(date.substr(0, 4));
    const Budget* b = manager.getBudget(month, year);
    if (b != nullptr) {
        double spent = manager.totalExpenseAmount(manager.filterByMonth(month, year));
        double usedPercent = (b->getLimit() > 0.0) ? (spent / b->getLimit()) * 100.0 : 0.0;
        showBudgetWarning(usedPercent);
    }

    pause();
}

void UI::handleAddIncome() {
    printHeader("ADD INCOME");

    std::string date = Validation::readValidDate("Enter date (YYYY-MM-DD): ");
    std::string source = Validation::readNonEmptyLine("Enter source: ");
    double amount = Validation::readPositiveAmount("Enter amount: Rs. ");
    std::string description = Validation::readNonEmptyLine("Enter description: ");

    int id = manager.addIncome(date, source, amount, description);
    persist();

    std::cout << "\n[SUCCESS] Income added successfully.\n";
    std::cout << "Income ID: " << id << "\n";
    pause();
}

void UI::handleViewExpenses() {
    printHeader("ALL EXPENSES");
    printExpenseTable(manager.getAllExpenses());
    pause();
}

void UI::handleViewIncome() {
    printHeader("ALL INCOME");
    printIncomeTable(manager.getAllIncome());
    pause();
}

void UI::handleSearch() {
    printHeader("SEARCH TRANSACTIONS");
    std::cout << "1. By ID\n2. By Category\n3. By Date\n4. By Description\n5. By Payment Method\n";
    int choice = Validation::readIntInRange("Enter choice: ", 1, 5);

    SearchField field;
    std::string prompt;
    switch (choice) {
        case 1: field = SearchField::Id; prompt = "Enter ID: "; break;
        case 2: field = SearchField::Category; prompt = "Enter category: "; break;
        case 3: field = SearchField::Date; prompt = "Enter date (YYYY-MM-DD): "; break;
        case 4: field = SearchField::Description; prompt = "Enter keyword: "; break;
        default: field = SearchField::PaymentMethod; prompt = "Enter payment method: "; break;
    }

    std::string query = Validation::readNonEmptyLine(prompt);
    std::vector<Expense> results = manager.searchExpenses(field, query);

    if (results.empty()) {
        std::cout << "\nNo matching transactions found.\n";
    } else {
        printExpenseTable(results);
    }
    pause();
}

void UI::handleUpdateExpense() {
    printHeader("UPDATE EXPENSE");
    int id = Validation::readIntInRange("Enter expense ID to update: ", 1, std::numeric_limits<int>::max());

    const Expense* existing = manager.findExpenseById(id);
    if (existing == nullptr) {
        std::cout << "\n[ERROR] No expense found with ID " << id << ".\n";
        pause();
        return;
    }

    std::cout << "\nCurrent expense details:\n";
    printExpenseTable({*existing});

    if (!Validation::readConfirmation("\nAre you sure you want to update this expense? (Y/N): ")) {
        std::cout << "\n[INFO] Update cancelled.\n";
        pause();
        return;
    }

    std::string date = Validation::readValidDate("Enter new date (YYYY-MM-DD): ");
    std::string category = chooseCategory();
    double amount = Validation::readPositiveAmount("Enter new amount: Rs. ");
    std::string description = Validation::readNonEmptyLine("Enter new description: ");
    std::string paymentMethod = choosePaymentMethod();

    manager.updateExpense(id, date, category, amount, description, paymentMethod);
    persist();

    std::cout << "\n[SUCCESS] Expense updated successfully.\n";
    pause();
}

void UI::handleDeleteExpense() {
    printHeader("DELETE EXPENSE");
    int id = Validation::readIntInRange("Enter expense ID to delete: ", 1, std::numeric_limits<int>::max());

    const Expense* existing = manager.findExpenseById(id);
    if (existing == nullptr) {
        std::cout << "\n[ERROR] No expense found with ID " << id << ".\n";
        pause();
        return;
    }

    std::cout << "\nExpense to be deleted:\n";
    printExpenseTable({*existing});

    if (!Validation::readConfirmation("\nAre you sure you want to delete this expense? (Y/N): ")) {
        std::cout << "\n[INFO] Deletion cancelled.\n";
        pause();
        return;
    }

    manager.deleteExpense(id);
    persist();

    std::cout << "\n[SUCCESS] Expense deleted successfully.\n";
    pause();
}

void UI::handleFilterExpenses() {
    printHeader("FILTER EXPENSES");
    std::cout << "1. By Month\n2. By Category\n3. By Payment Method\n4. By Date Range\n";
    int choice = Validation::readIntInRange("Enter choice: ", 1, 4);

    std::vector<Expense> results;

    if (choice == 1) {
        int month = Validation::readIntInRange("Enter month (1-12): ", 1, 12);
        int year = Validation::readIntInRange("Enter year: ", 1900, 2200);
        results = manager.filterByMonth(month, year);
    } else if (choice == 2) {
        std::string category = chooseCategory();
        results = manager.filterByCategory(category);
    } else if (choice == 3) {
        std::string method = choosePaymentMethod();
        results = manager.filterByPaymentMethod(method);
    } else {
        std::string start = Validation::readValidDate("Enter start date (YYYY-MM-DD): ");
        std::string end = Validation::readValidDate("Enter end date (YYYY-MM-DD): ");
        results = manager.filterByDateRange(start, end);
    }

    printExpenseTable(results);
    pause();
}

void UI::handleSortExpenses() {
    printHeader("SORT EXPENSES");
    std::cout << "1. Amount - Low to High\n";
    std::cout << "2. Amount - High to Low\n";
    std::cout << "3. Date - Oldest to Newest\n";
    std::cout << "4. Date - Newest to Oldest\n";
    std::cout << "5. Category - A to Z\n";
    int choice = Validation::readIntInRange("Enter choice: ", 1, 5);

    SortField field;
    switch (choice) {
        case 1: field = SortField::AmountAsc; break;
        case 2: field = SortField::AmountDesc; break;
        case 3: field = SortField::DateAsc; break;
        case 4: field = SortField::DateDesc; break;
        default: field = SortField::CategoryAsc; break;
    }

    std::vector<Expense> sorted = manager.sortExpenses(manager.getAllExpenses(), field);
    printExpenseTable(sorted);
    pause();
}

void UI::handleSetBudget() {
    printHeader("SET MONTHLY BUDGET");
    int month = Validation::readIntInRange("Enter month (1-12): ", 1, 12);
    int year = Validation::readIntInRange("Enter year: ", 1900, 2200);

    const Budget* existing = manager.getBudget(month, year);
    if (existing != nullptr) {
        std::cout << "\n[INFO] A budget of Rs. " << std::fixed << std::setprecision(2)
                  << existing->getLimit() << " already exists for this month.\n";
        std::cout << "Entering a new value will update it.\n";
    }

    double limit = Validation::readPositiveAmount("Enter monthly budget: Rs. ");
    manager.setBudget(month, year, limit);
    persist();

    std::cout << "\n[SUCCESS] Budget set successfully for " << month << "/" << year << ".\n";
    pause();
}

void UI::handleMonthlySummary() {
    printHeader("MONTHLY SUMMARY");
    int month = Validation::readIntInRange("Enter month (1-12): ", 1, 12);
    int year = Validation::readIntInRange("Enter year: ", 1900, 2200);

    MonthlySummary s = manager.getMonthlySummary(month, year);

    static const char* monthNames[] = {
        "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE",
        "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"
    };

    if (!s.hasData) {
        std::cout << "\n[INFO] No transactions found for " << monthNames[month - 1] << " " << year << ".\n";
        pause();
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "   " << monthNames[month - 1] << " " << year << " SUMMARY\n";
    std::cout << "========================================\n\n";

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Income       : Rs. " << s.totalIncome << "\n";
    std::cout << "Total Expenses     : Rs. " << s.totalExpenses << "\n";
    std::cout << "Remaining Balance  : Rs. " << s.remainingBalance << "\n\n";

    if (s.hasBudget) {
        std::cout << "Budget             : Rs. " << s.budgetLimit << "\n";
        std::cout << "Budget Used        : " << s.budgetUsedPercent << "%\n";
        std::cout << "Budget Remaining   : Rs. " << s.budgetRemaining << "\n\n";
    } else {
        std::cout << "Budget             : Not set\n\n";
    }

    std::cout << "Number of Expenses : " << s.expenseCount << "\n";
    std::cout << "Number of Income   : " << s.incomeCount << "\n\n";

    if (s.expenseCount > 0) {
        std::cout << "Highest Expense    : Rs. " << s.highestExpense << "\n";
        std::cout << "Average Expense    : Rs. " << s.averageExpense << "\n";
    }

    std::cout << "========================================\n";

    if (s.hasBudget) {
        showBudgetWarning(s.budgetUsedPercent);
    }

    pause();
}

void UI::handleCategoryWise() {
    printHeader("CATEGORY-WISE EXPENSES");

    auto totals = manager.categoryWiseTotals(manager.getAllExpenses());

    if (totals.empty()) {
        std::cout << "\n[INFO] No expenses recorded yet.\n";
        pause();
        return;
    }

    std::cout << "\n";
    printSeparator();
    double grandTotal = 0.0;
    std::string highestCategory;
    double highestAmount = -1.0;

    for (const auto& [category, amount] : totals) {
        std::cout << std::left << std::setw(18) << category
                  << "Rs. " << std::fixed << std::setprecision(2) << std::right << std::setw(12) << amount << "\n";
        grandTotal += amount;
        if (amount > highestAmount) {
            highestAmount = amount;
            highestCategory = category;
        }
    }
    printSeparator();
    std::cout << std::left << std::setw(18) << "Total"
              << "Rs. " << std::fixed << std::setprecision(2) << std::right << std::setw(12) << grandTotal << "\n";
    printSeparator();

    std::cout << "\nHighest Spending Category: " << highestCategory << "\n";
    pause();
}

void UI::handleBudgetStatus() {
    printHeader("BUDGET STATUS");
    int month = Validation::readIntInRange("Enter month (1-12): ", 1, 12);
    int year = Validation::readIntInRange("Enter year: ", 1900, 2200);

    const Budget* b = manager.getBudget(month, year);
    if (b == nullptr) {
        std::cout << "\n[INFO] No budget has been set for " << month << "/" << year << ".\n";
        pause();
        return;
    }

    double spent = manager.totalExpenseAmount(manager.filterByMonth(month, year));
    double remaining = b->getLimit() - spent;
    double usedPercent = (b->getLimit() > 0.0) ? (spent / b->getLimit()) * 100.0 : 0.0;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nMonthly Budget: Rs. " << b->getLimit() << "\n";
    std::cout << "Total Spent:    Rs. " << spent << "\n";
    std::cout << "Remaining:      Rs. " << remaining << "\n";
    std::cout << "Used:           " << usedPercent << "%\n";

    showBudgetWarning(usedPercent);
    pause();
}

void UI::handleExportCsv() {
    printHeader("EXPORT CSV REPORT");

    std::string fileName = Validation::readNonEmptyLine("Enter export file name (e.g. report.csv): ");
    std::string targetPath = fileName;
    if (fileName.find('/') == std::string::npos && fileName.find('\\') == std::string::npos) {
        targetPath = "exports/" + fileName;
    }

    size_t slashPos = targetPath.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        std::string dirPath = targetPath.substr(0, slashPos);
        if (!dirPath.empty()) {
            #if defined(_WIN32)
                _mkdir(dirPath.c_str());
            #else
                mkdir(dirPath.c_str(), 0755);
            #endif
        }
    }

    std::ofstream out(targetPath, std::ios::trunc);
    if (!out.is_open()) {
        std::cout << "\n[ERROR] Failed to create export file: " << targetPath << "\n";
        pause();
        return;
    }

    out << "Type,ID,Date,Category,Source,Amount,Description,Payment Method\n";

    for (const auto& expense : manager.getAllExpenses()) {
        out << "Expense," << expense.getId() << ","
            << csvEscape(expense.getDate()) << ","
            << csvEscape(expense.getCategory()) << ","
            << "" << ","
            << std::fixed << std::setprecision(2) << expense.getAmount() << ","
            << csvEscape(expense.getDescription()) << ","
            << csvEscape(expense.getPaymentMethod()) << "\n";
    }

    for (const auto& income : manager.getAllIncome()) {
        out << "Income," << income.getId() << ","
            << csvEscape(income.getDate()) << ","
            << "" << ","
            << csvEscape(income.getSource()) << ","
            << std::fixed << std::setprecision(2) << income.getAmount() << ","
            << csvEscape(income.getDescription()) << ","
            << "" << "\n";
    }

    std::cout << "\n[SUCCESS] CSV report exported to " << targetPath << "\n";
    pause();
}

// ---------------- Main loop ----------------

void UI::run() {
    while (true) {
        std::cout << "\n==================================================\n";
        std::cout << "              MONTHLY EXPENSE MANAGER\n";
        std::cout << "==================================================\n\n";
        std::cout << "1.  Add Expense\n";
        std::cout << "2.  Add Income\n";
        std::cout << "3.  View All Expenses\n";
        std::cout << "4.  View All Income\n";
        std::cout << "5.  Search Transactions\n";
        std::cout << "6.  Update Expense\n";
        std::cout << "7.  Delete Expense\n";
        std::cout << "8.  Filter Expenses\n";
        std::cout << "9.  Sort Expenses\n";
        std::cout << "10. Set Monthly Budget\n";
        std::cout << "11. View Monthly Summary\n";
        std::cout << "12. View Category-wise Expenses\n";
        std::cout << "13. View Budget Status\n";
        std::cout << "14. Export CSV Report\n";
        std::cout << "15. Exit\n";
        std::cout << "\n==================================================\n";

        int choice = Validation::readIntInRange("Enter your choice: ", 1, 15);

        switch (choice) {
            case 1: handleAddExpense(); break;
            case 2: handleAddIncome(); break;
            case 3: handleViewExpenses(); break;
            case 4: handleViewIncome(); break;
            case 5: handleSearch(); break;
            case 6: handleUpdateExpense(); break;
            case 7: handleDeleteExpense(); break;
            case 8: handleFilterExpenses(); break;
            case 9: handleSortExpenses(); break;
            case 10: handleSetBudget(); break;
            case 11: handleMonthlySummary(); break;
            case 12: handleCategoryWise(); break;
            case 13: handleBudgetStatus(); break;
            case 14: handleExportCsv(); break;
            case 15:
                persist();
                std::cout << "\nAll data saved. Goodbye!\n";
                return;
        }
    }
}
