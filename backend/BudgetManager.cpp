#include "BudgetManager.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

namespace {
    // Keep this project simple by escaping the few characters used in JSON.
    std::string escapeJson(const std::string& value) {
        std::string result;
        for (char c : value) {
            if (c == '"' || c == '\\') result += '\\';
            if (c == '\n' || c == '\r') continue;
            result += c;
        }
        return result;
    }

    // CSV fields in this beginner version should not contain commas.
    std::string cleanCsv(std::string value) {
        for (char& c : value) if (c == ',' || c == '\n' || c == '\r') c = ' ';
        return value;
    }
}

BudgetManager::BudgetManager() : monthlyBudget(5000.0), nextId(1) {
    // Create data folder manually once; files are created automatically.
    std::ifstream budgetFile("data/budget.txt");
    if (budgetFile) budgetFile >> monthlyBudget;
    loadTransactions();
}

void BudgetManager::loadTransactions() {
    std::ifstream file("data/transactions.csv");
    if (!file) return;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream row(line);
        Transaction t;
        std::string amountText;
        if (std::getline(row, amountText, ',') &&
            std::getline(row, t.type, ',') &&
            std::getline(row, t.title, ',') &&
            std::getline(row, t.category, ',') &&
            std::getline(row, amountText, ',') &&
            std::getline(row, t.date, ',')) {
            // The first field is the ID; read it separately for clarity.
        }
        // Parse the complete row in its actual column order.
        std::stringstream actual(line);
        std::string idText, type, title, category, amount, date;
        if (std::getline(actual,idText,',') && std::getline(actual,type,',') &&
            std::getline(actual,title,',') && std::getline(actual,category,',') &&
            std::getline(actual,amount,',') && std::getline(actual,date,',')) {
            try {
                t.id = std::stoi(idText);
                t.type = type; t.title = title; t.category = category;
                t.amount = std::stod(amount); t.date = date;
                transactions.push_back(t);
                if (t.id >= nextId) nextId = t.id + 1;
            } catch (...) {
                // Ignore a damaged line instead of crashing the whole app.
            }
        }
    }
}

void BudgetManager::saveTransactions() const {
    std::ofstream file("data/transactions.csv");
    for (const auto& t : transactions) {
        file << t.id << ',' << cleanCsv(t.type) << ',' << cleanCsv(t.title)
             << ',' << cleanCsv(t.category) << ',' << t.amount << ','
             << cleanCsv(t.date) << '\n';
    }
}

bool BudgetManager::addTransaction(const std::string& type, const std::string& title,
                                  const std::string& category, double amount,
                                  const std::string& date) {
    if ((type != "income" && type != "expense") || title.empty() ||
        category.empty() || amount <= 0 || date.empty()) return false;

    Transaction t{nextId++, type, title, category, amount, date};
    transactions.push_back(t);
    saveTransactions();
    return true;
}

void BudgetManager::setBudget(double amount) {
    if (amount < 0) return;
    monthlyBudget = amount;
    std::ofstream file("data/budget.txt");
    file << monthlyBudget;
}

double BudgetManager::getBudget() const { return monthlyBudget; }

double BudgetManager::totalIncome() const {
    double total = 0;
    for (const auto& t : transactions) if (t.type == "income") total += t.amount;
    return total;
}

double BudgetManager::totalExpenses() const {
    double total = 0;
    for (const auto& t : transactions) if (t.type == "expense") total += t.amount;
    return total;
}

double BudgetManager::balance() const { return totalIncome() - totalExpenses(); }
double BudgetManager::remainingBudget() const { return monthlyBudget - totalExpenses(); }
const std::vector<Transaction>& BudgetManager::getTransactions() const { return transactions; }

std::string BudgetManager::transactionsAsJson() const {
    std::ostringstream out;
    out << '[';
    for (size_t i = 0; i < transactions.size(); ++i) {
        const auto& t = transactions[i];
        if (i) out << ',';
        out << "{\"id\":" << t.id << ",\"type\":\"" << escapeJson(t.type)
            << "\",\"title\":\"" << escapeJson(t.title) << "\",\"category\":\""
            << escapeJson(t.category) << "\",\"amount\":" << t.amount
            << ",\"date\":\"" << escapeJson(t.date) << "\"}";
    }
    out << ']';
    return out.str();
}

std::string BudgetManager::summaryAsJson() const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2)
        << "{\"income\":" << totalIncome()
        << ",\"expenses\":" << totalExpenses()
        << ",\"balance\":" << balance()
        << ",\"budget\":" << monthlyBudget
        << ",\"remaining\":" << remainingBudget() << '}';
    return out.str();
}
