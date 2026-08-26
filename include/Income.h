#ifndef INCOME_H
#define INCOME_H

#include <string>

// Represents a single income transaction.
class Income {
private:
    int id;
    std::string date;      // format: YYYY-MM-DD
    std::string source;
    double amount;
    std::string description;

public:
    Income();
    Income(int id,
           const std::string& date,
           const std::string& source,
           double amount,
           const std::string& description);

    // Getters
    int getId() const;
    std::string getDate() const;
    std::string getSource() const;
    double getAmount() const;
    std::string getDescription() const;

    // Setters
    void setDate(const std::string& newDate);
    void setSource(const std::string& newSource);
    void setAmount(double newAmount);
    void setDescription(const std::string& newDescription);

    // Serialization helpers
    std::string toFileString() const;
    static Income fromFileString(const std::string& line, bool& ok);

    // Display helper
    void printRow() const;
};

#endif // INCOME_H
