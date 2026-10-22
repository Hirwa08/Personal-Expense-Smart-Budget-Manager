#include "BudgetManager.h"
#include "HttpServer.h"
#include <iostream>

int main() {
    // The data folder is included in the project, so saved files have a place to go.
    BudgetManager manager;

    std::cout << "====================================\n";
    std::cout << " Personal Expense & Smart Budget\n";
    std::cout << "====================================\n";
    runServer(manager, 8080);
    return 0;
}
