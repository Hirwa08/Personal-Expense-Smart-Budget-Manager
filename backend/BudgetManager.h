#ifndef BUDGET_MANAGER_H
#define BUDGET_MANAGER_H

#include <string>
#include <vector>

// One transaction means one income or expense entry.
struct Transaction {
    int id;
    std::string type;       // "income" or "expense"
    std::string title;
    std::string category;
    double amount;
    std::string date;
};

class BudgetManager {
private:
    std::vector<Transaction> transactions;
    double monthlyBudget;
    int nextId;

    void loadTransactions();
    void saveTransactions() const;

public:
    BudgetManager();

    bool addTransaction(const std::string& type, const std::string& title,
                        const std::string& category, double amount,
                        const std::string& date);
    void setBudget(double amount);
    double getBudget() const;
    double totalIncome() const;
    double totalExpenses() const;
    double balance() const;
    double remainingBudget() const;
    const std::vector<Transaction>& getTransactions() const;
    std::string transactionsAsJson() const;
    std::string summaryAsJson() const;
};

#endif
