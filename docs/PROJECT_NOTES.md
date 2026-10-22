# Project Notes

## Problem statement
Students and other users may find it difficult to track daily expenses and manage a budget. This application records income and expenses, groups entries by category, calculates the balance, and shows whether spending is above the selected budget.

## Main features
1. Add income or expense records.
2. Store entries in a local CSV file.
3. Calculate total income, total expenses and current balance.
4. Set and save a budget.
5. Display a simple browser dashboard.
6. View transactions in a table.

## OOP concepts used
- **Class:** `BudgetManager` groups related operations.
- **Encapsulation:** transaction data and budget are private.
- **Objects:** `main.cpp` creates a `BudgetManager` object.
- **Structure:** `Transaction` keeps related transaction details together.
- **File handling:** `ifstream` and `ofstream` load and save records.
- **Vector:** stores the list of transactions.

## Architecture
The browser interface is in HTML, CSS and JavaScript. JavaScript sends simple HTTP requests to the local C++ server. The C++ server calls the `BudgetManager` class, which performs calculations and saves data in files.

## Scope / limitations
This is a learning project, not a banking product. It does not connect to a bank, handle multiple user accounts, or provide financial advice. The simple CSV format is intended for a student demonstration.
