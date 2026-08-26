# Monthly Expense Manager

A fully functional, console-based personal finance tracker written in modern C++17. Track income and expenses, set monthly budgets, and generate spending reports — all backed by simple, human-readable local file storage. No database, no internet connection, no external dependencies.

## Description

Monthly Expense Manager is a command-line application that lets a user record income and expense transactions, categorize spending, set monthly budget limits, and generate financial summaries. All data is persisted to plain text files on disk and reloaded automatically the next time the program starts, so nothing is lost between sessions.

The project was built as a demonstration of core C++ and CS fundamentals: object-oriented design, the STL, file I/O, exception handling, and clean modular architecture — the kind of project a CSE student can confidently walk through in a placement interview.

## Features

- **Add Expense** — record an expense with date, category, amount, description, and payment method (auto-generated unique ID, full input validation)
- **Add Income** — record income with date, source, amount, and description
- **View All Expenses / View All Income** — clean, aligned table output with running totals
- **Search Transactions** — search expenses by ID, category, date, description (substring match), or payment method
- **Update Expense** — edit an existing expense by ID, with a preview and Y/N confirmation before saving
- **Delete Expense** — remove an expense by ID, with a preview and Y/N confirmation before deleting
- **Filter Expenses** — by month/year, category, payment method, or a date range
- **Sort Expenses** — by amount (asc/desc), date (asc/desc), or category (A–Z), using `std::sort`
- **Set Monthly Budget** — define or update a spending limit for any month/year
- **View Monthly Summary** — total income, total expenses, remaining balance, budget usage, highest/average expense for a chosen month
- **Category-wise Expenses** — spending broken down by category with the highest-spending category highlighted
- **View Budget Status** — spent vs. remaining vs. budget %, with automatic 80% warning and 100% overspend alert
- **Persistent Storage** — all data automatically saved to disk after every change, and reloaded on startup
- **Robust Input Validation** — the program never crashes on bad input (letters where numbers are expected, negative amounts, malformed dates, missing/corrupted data files, etc.)

## Tech Stack

- **Language:** C++17
- **Standard Library only:** `<vector>`, `<map>`, `<string>`, `<algorithm>`, `<fstream>`, `<sstream>`, `<iomanip>`
- **Persistence:** plain delimiter-separated text files (no database)
- **Paradigm:** Object-Oriented Programming with a modular, layered architecture

## Project Structure

```text
monthly-expense-manager/
│
├── include/                 # Header files (class declarations / interfaces)
│   ├── Expense.h
│   ├── Income.h
│   ├── Budget.h
│   ├── ExpenseManager.h
│   ├── FileManager.h
│   ├── Validation.h
│   └── UI.h
│
├── src/                      # Implementation files
│   ├── Expense.cpp
│   ├── Income.cpp
│   ├── Budget.cpp
│   ├── ExpenseManager.cpp
│   ├── FileManager.cpp
│   ├── Validation.cpp
│   └── UI.cpp
│
├── data/                     # Persistent storage (plain text, pipe-delimited)
│   ├── expenses.txt
│   ├── income.txt
│   └── budgets.txt
│
├── main.cpp                  # Application entry point
├── README.md
└── .gitignore
```

### Layer responsibilities

| Layer | File(s) | Responsibility |
|---|---|---|
| **Data models** | `Expense`, `Income`, `Budget` | Encapsulated storage of one transaction/budget's data, plus serialization to/from a file line |
| **Persistence** | `FileManager` | Reads and writes the three data files; creates them if missing; skips malformed lines |
| **Business logic** | `ExpenseManager` | Owns all in-memory data; provides add/update/delete/search/filter/sort/summary operations; never touches `std::cin`/`std::cout` |
| **Validation** | `Validation` (namespace) | Reusable input readers and validators — dates, positive amounts, non-empty strings, confirmations |
| **Presentation** | `UI` | Menu loop, prompts, and formatted table/report output; calls into `ExpenseManager` for all real work |
| **Entry point** | `main.cpp` | Wires the layers together and starts the app inside a top-level `try/catch` |

## How to Compile

### Linux / macOS

```bash
g++ -std=c++17 -Wall -Wextra main.cpp src/*.cpp -Iinclude -o expense_manager
```

### Windows (MinGW)

```bash
g++ -std=c++17 -Wall -Wextra main.cpp src\Expense.cpp src\Income.cpp src\Budget.cpp src\ExpenseManager.cpp src\FileManager.cpp src\Validation.cpp src\UI.cpp -Iinclude -o expense_manager.exe
```

## How to Run

### Linux / macOS

```bash
./expense_manager
```

### Windows

```bash
expense_manager.exe
```

Run the executable from the project's root directory so it can find (or create) the `data/` folder.

## Example Usage

```text
==================================================
              MONTHLY EXPENSE MANAGER
==================================================

1.  Add Expense
2.  Add Income
...
14. Exit

==================================================
Enter your choice: 1

========================================
   ADD EXPENSE
========================================
Enter date (YYYY-MM-DD): 2026-08-20

Select Category:
  1. Food
  2. Travel
  ...
Enter choice: 1
Enter amount: Rs. 350
Enter description: Lunch with friends

Select Payment Method:
  1. Cash
  2. UPI
  ...
Enter choice: 2

[SUCCESS] Expense added successfully.
Expense ID: 101
```

## Concepts Demonstrated

- **Object-Oriented Programming** — encapsulation (private data members with getters/setters), composition (`ExpenseManager` owns collections of `Expense`/`Income`/`Budget`), and separation of concerns across five distinct classes
- **STL** — `std::vector`, `std::map`, `std::sort`, `std::find_if`, `std::remove_if`, `std::copy_if`
- **File Handling** — `fstream`-based persistence with a custom pipe-delimited format, directory/file auto-creation, and safe re-loading
- **Algorithms** — custom comparators for multi-criteria sorting, linear search variants for lookup/filtering
- **Exception Handling** — `try/catch` around numeric parsing (`std::stoi`/`std::stod`) and a top-level guard in `main()`
- **Input Validation** — a dedicated `Validation` namespace that guarantees the program never crashes on malformed input
- **Modular Design** — clean header/implementation separation, single-responsibility classes, no business logic inside the UI layer

## Future Improvements

- Graphical user interface (Qt or a web front end)
- SQLite (or another embedded database) instead of flat files
- Visual charts for spending trends (bar/pie charts)
- Multi-user login system with per-user data
- Cloud synchronization across devices
- A companion mobile application

## Design Note: Why `double` for Currency

This project stores monetary values as `double`, formatted to two decimal places on both display and file output. For a student/portfolio project this keeps the code readable and matches how the amounts are typed in and displayed. The trade-off is that `double` can accumulate tiny floating-point rounding errors over many operations. A production-grade financial system would instead store amounts as integer paise (i.e. `amount * 100` as an `int64_t`) to guarantee exact arithmetic, converting to rupees only for display. That approach is noted here as a deliberate, known simplification rather than an oversight.
