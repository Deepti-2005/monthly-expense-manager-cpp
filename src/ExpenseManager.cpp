#include "ExpenseManager.h"
#include <algorithm>
#include <cctype>

ExpenseManager::ExpenseManager(const FileManager& fm)
    : fileManager(fm), nextExpenseId(1), nextIncomeId(1) {}

std::string ExpenseManager::toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return out;
}

bool ExpenseManager::sameMonthYear(const std::string& date, int month, int year) {
    // date format is guaranteed YYYY-MM-DD by validation at input time
    if (date.size() != 10) return false;
    try {
        int y = std::stoi(date.substr(0, 4));
        int m = std::stoi(date.substr(5, 2));
        return y == year && m == month;
    } catch (const std::exception&) {
        return false;
    }
}

int ExpenseManager::computeNextId(const std::vector<Expense>& items) const {
    int maxId = 100; // IDs start at 101 for readability
    for (const auto& e : items) {
        if (e.getId() > maxId) maxId = e.getId();
    }
    return maxId + 1;
}

int ExpenseManager::computeNextIncomeId(const std::vector<Income>& items) const {
    int maxId = 100;
    for (const auto& i : items) {
        if (i.getId() > maxId) maxId = i.getId();
    }
    return maxId + 1;
}

void ExpenseManager::loadAll() {
    expenses = fileManager.loadExpenses();
    incomeRecords = fileManager.loadIncome();
    budgets = fileManager.loadBudgets();

    nextExpenseId = computeNextId(expenses);
    nextIncomeId = computeNextIncomeId(incomeRecords);
}

bool ExpenseManager::saveAll() const {
    bool ok = true;
    ok = fileManager.saveExpenses(expenses) && ok;
    ok = fileManager.saveIncome(incomeRecords) && ok;
    ok = fileManager.saveBudgets(budgets) && ok;
    return ok;
}

// ---------------- Expense operations ----------------

int ExpenseManager::addExpense(const std::string& date, const std::string& category,
                                double amount, const std::string& description,
                                const std::string& paymentMethod) {
    int id = nextExpenseId++;
    expenses.emplace_back(id, date, category, amount, description, paymentMethod);
    return id;
}

bool ExpenseManager::updateExpense(int id, const std::string& date, const std::string& category,
                                    double amount, const std::string& description,
                                    const std::string& paymentMethod) {
    for (auto& e : expenses) {
        if (e.getId() == id) {
            e.setDate(date);
            e.setCategory(category);
            e.setAmount(amount);
            e.setDescription(description);
            e.setPaymentMethod(paymentMethod);
            return true;
        }
    }
    return false;
}

bool ExpenseManager::deleteExpense(int id) {
    auto it = std::remove_if(expenses.begin(), expenses.end(),
                              [id](const Expense& e) { return e.getId() == id; });
    if (it == expenses.end()) return false;
    expenses.erase(it, expenses.end());
    return true;
}

const Expense* ExpenseManager::findExpenseById(int id) const {
    auto it = std::find_if(expenses.begin(), expenses.end(),
                            [id](const Expense& e) { return e.getId() == id; });
    if (it == expenses.end()) return nullptr;
    return &(*it);
}

const std::vector<Expense>& ExpenseManager::getAllExpenses() const {
    return expenses;
}

std::vector<Expense> ExpenseManager::searchExpenses(SearchField field, const std::string& query) const {
    std::vector<Expense> results;
    std::string lowerQuery = toLower(query);

    for (const auto& e : expenses) {
        bool match = false;
        switch (field) {
            case SearchField::Id: {
                try {
                    match = (e.getId() == std::stoi(query));
                } catch (const std::exception&) {
                    match = false;
                }
                break;
            }
            case SearchField::Category:
                match = toLower(e.getCategory()) == lowerQuery;
                break;
            case SearchField::Date:
                match = e.getDate() == query;
                break;
            case SearchField::Description:
                match = toLower(e.getDescription()).find(lowerQuery) != std::string::npos;
                break;
            case SearchField::PaymentMethod:
                match = toLower(e.getPaymentMethod()) == lowerQuery;
                break;
        }
        if (match) results.push_back(e);
    }
    return results;
}

std::vector<Expense> ExpenseManager::filterByMonth(int month, int year) const {
    std::vector<Expense> results;
    std::copy_if(expenses.begin(), expenses.end(), std::back_inserter(results),
                 [&](const Expense& e) { return sameMonthYear(e.getDate(), month, year); });
    return results;
}

std::vector<Expense> ExpenseManager::filterByCategory(const std::string& category) const {
    std::vector<Expense> results;
    std::string lowerCat = toLower(category);
    std::copy_if(expenses.begin(), expenses.end(), std::back_inserter(results),
                 [&](const Expense& e) { return toLower(e.getCategory()) == lowerCat; });
    return results;
}

std::vector<Expense> ExpenseManager::filterByPaymentMethod(const std::string& method) const {
    std::vector<Expense> results;
    std::string lowerMethod = toLower(method);
    std::copy_if(expenses.begin(), expenses.end(), std::back_inserter(results),
                 [&](const Expense& e) { return toLower(e.getPaymentMethod()) == lowerMethod; });
    return results;
}

std::vector<Expense> ExpenseManager::filterByDateRange(const std::string& startDate,
                                                         const std::string& endDate) const {
    std::vector<Expense> results;
    // Dates are in YYYY-MM-DD format, so lexicographic comparison equals
    // chronological comparison.
    std::copy_if(expenses.begin(), expenses.end(), std::back_inserter(results),
                 [&](const Expense& e) {
                     return e.getDate() >= startDate && e.getDate() <= endDate;
                 });
    return results;
}

std::vector<Expense> ExpenseManager::sortExpenses(const std::vector<Expense>& source, SortField field) const {
    std::vector<Expense> sorted = source;
    switch (field) {
        case SortField::AmountAsc:
            std::sort(sorted.begin(), sorted.end(),
                     [](const Expense& a, const Expense& b) { return a.getAmount() < b.getAmount(); });
            break;
        case SortField::AmountDesc:
            std::sort(sorted.begin(), sorted.end(),
                     [](const Expense& a, const Expense& b) { return a.getAmount() > b.getAmount(); });
            break;
        case SortField::DateAsc:
            std::sort(sorted.begin(), sorted.end(),
                     [](const Expense& a, const Expense& b) { return a.getDate() < b.getDate(); });
            break;
        case SortField::DateDesc:
            std::sort(sorted.begin(), sorted.end(),
                     [](const Expense& a, const Expense& b) { return a.getDate() > b.getDate(); });
            break;
        case SortField::CategoryAsc:
            std::sort(sorted.begin(), sorted.end(),
                     [](const Expense& a, const Expense& b) { return a.getCategory() < b.getCategory(); });
            break;
    }
    return sorted;
}

double ExpenseManager::totalExpenseAmount(const std::vector<Expense>& source) const {
    double total = 0.0;
    for (const auto& e : source) total += e.getAmount();
    return total;
}

// ---------------- Income operations ----------------

int ExpenseManager::addIncome(const std::string& date, const std::string& source,
                               double amount, const std::string& description) {
    int id = nextIncomeId++;
    incomeRecords.emplace_back(id, date, source, amount, description);
    return id;
}

const std::vector<Income>& ExpenseManager::getAllIncome() const {
    return incomeRecords;
}

double ExpenseManager::totalIncomeAmount(const std::vector<Income>& source) const {
    double total = 0.0;
    for (const auto& i : source) total += i.getAmount();
    return total;
}

std::vector<Income> ExpenseManager::incomeForMonth(int month, int year) const {
    std::vector<Income> results;
    std::copy_if(incomeRecords.begin(), incomeRecords.end(), std::back_inserter(results),
                 [&](const Income& i) { return sameMonthYear(i.getDate(), month, year); });
    return results;
}

// ---------------- Budget operations ----------------

void ExpenseManager::setBudget(int month, int year, double limit) {
    for (auto& b : budgets) {
        if (b.getMonth() == month && b.getYear() == year) {
            b.setLimit(limit);
            return;
        }
    }
    budgets.emplace_back(month, year, limit);
}

const Budget* ExpenseManager::getBudget(int month, int year) const {
    for (const auto& b : budgets) {
        if (b.getMonth() == month && b.getYear() == year) return &b;
    }
    return nullptr;
}

// ---------------- Reporting ----------------

MonthlySummary ExpenseManager::getMonthlySummary(int month, int year) const {
    MonthlySummary summary;

    std::vector<Expense> monthExpenses = filterByMonth(month, year);
    std::vector<Income> monthIncome = incomeForMonth(month, year);

    summary.expenseCount = static_cast<int>(monthExpenses.size());
    summary.incomeCount = static_cast<int>(monthIncome.size());
    summary.hasData = (summary.expenseCount > 0 || summary.incomeCount > 0);

    summary.totalExpenses = totalExpenseAmount(monthExpenses);
    summary.totalIncome = totalIncomeAmount(monthIncome);
    summary.remainingBalance = summary.totalIncome - summary.totalExpenses;

    if (!monthExpenses.empty()) {
        double highest = monthExpenses.front().getAmount();
        for (const auto& e : monthExpenses) {
            if (e.getAmount() > highest) highest = e.getAmount();
        }
        summary.highestExpense = highest;
        summary.averageExpense = summary.totalExpenses / summary.expenseCount;
    }

    const Budget* b = getBudget(month, year);
    if (b != nullptr) {
        summary.hasBudget = true;
        summary.budgetLimit = b->getLimit();
        summary.budgetUsedPercent = (b->getLimit() > 0.0)
            ? (summary.totalExpenses / b->getLimit()) * 100.0
            : 0.0;
        summary.budgetRemaining = b->getLimit() - summary.totalExpenses;
    }

    return summary;
}

std::map<std::string, double> ExpenseManager::categoryWiseTotals(const std::vector<Expense>& source) const {
    std::map<std::string, double> totals;
    for (const auto& e : source) {
        totals[e.getCategory()] += e.getAmount();
    }
    return totals;
}
