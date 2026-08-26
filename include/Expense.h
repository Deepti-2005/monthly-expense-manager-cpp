#ifndef EXPENSE_H
#define EXPENSE_H

#include <string>
#include <vector>

// Represents a single expense transaction.
// Encapsulates all data related to one expense entry and exposes
// controlled access through getters/setters.
class Expense {
private:
    int id;
    std::string date;           // format: YYYY-MM-DD
    std::string category;
    double amount;
    std::string description;
    std::string paymentMethod;

public:
    Expense();
    Expense(int id,
            const std::string& date,
            const std::string& category,
            double amount,
            const std::string& description,
            const std::string& paymentMethod);

    // Getters
    int getId() const;
    std::string getDate() const;
    std::string getCategory() const;
    double getAmount() const;
    std::string getDescription() const;
    std::string getPaymentMethod() const;

    // Setters
    void setDate(const std::string& newDate);
    void setCategory(const std::string& newCategory);
    void setAmount(double newAmount);
    void setDescription(const std::string& newDescription);
    void setPaymentMethod(const std::string& newPaymentMethod);

    // Serialization helpers
    std::string toFileString() const;               // pipe-delimited line for file storage
    static Expense fromFileString(const std::string& line, bool& ok);

    // Display helpers
    void printRow() const;                            // one formatted table row
};

// Lists of valid categories and payment methods used across the app.
namespace ExpenseConstants {
    const std::vector<std::string>& categories();
    const std::vector<std::string>& paymentMethods();
    bool isValidCategory(const std::string& category);
    bool isValidPaymentMethod(const std::string& method);
}

#endif // EXPENSE_H
