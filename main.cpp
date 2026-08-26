#include "ExpenseManager.h"
#include "FileManager.h"
#include "UI.h"
#include <iostream>

int main() {
    try {
        FileManager fileManager("data/expenses.txt", "data/income.txt", "data/budgets.txt");
        ExpenseManager manager(fileManager);
        manager.loadAll();

        UI ui(manager);
        ui.run();
    } catch (const std::exception& ex) {
        std::cerr << "[FATAL ERROR] " << ex.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "[FATAL ERROR] An unknown error occurred.\n";
        return 1;
    }

    return 0;
}
