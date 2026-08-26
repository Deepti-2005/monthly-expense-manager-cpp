#ifndef VALIDATION_H
#define VALIDATION_H

#include <string>

// Collection of reusable, stateless validation helpers.
// Grouped in a namespace rather than a class since they hold no state.
namespace Validation {

    // Validates a date string in strict YYYY-MM-DD format, including
    // month range, day range, and leap-year awareness.
    bool isValidDate(const std::string& date);

    // True if amount is a finite number strictly greater than zero.
    bool isValidAmount(double amount);

    // True if the string contains at least one non-whitespace character.
    bool isNonEmpty(const std::string& text);

    // Reads an integer from std::cin within [minValue, maxValue].
    // Reprompts on invalid input; never throws or crashes on bad input.
    int readIntInRange(const std::string& prompt, int minValue, int maxValue);

    // Reads a positive double amount from std::cin (rejects <= 0).
    double readPositiveAmount(const std::string& prompt);

    // Reads a non-empty line of text from std::cin.
    std::string readNonEmptyLine(const std::string& prompt);

    // Reads and validates a YYYY-MM-DD date from std::cin.
    std::string readValidDate(const std::string& prompt);

    // Reads a Y/N confirmation; returns true only for 'Y'/'y'.
    bool readConfirmation(const std::string& prompt);

    // Clears cin's fail state and discards the rest of the current line.
    void clearInputStream();
}

#endif // VALIDATION_H
