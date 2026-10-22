# Personal Expense & Smart Budget Manager

A beginner-friendly C++ OOP project for tracking income, expenses, categories and budgets.

## What it does
- Add income and expenses
- View recent transactions
- Set a monthly spending budget
- See total income, expenses, balance and remaining budget
- Save transactions and budget to local files

## Project structure
- `backend/` — C++ application logic and local HTTP server
- `frontend/` — simple web interface
- `data/` — local data files (created automatically when needed)
- `docs/` — project notes

## Important note
The backend is written in C++. It uses a small built-in HTTP server with standard socket APIs, so no external C++ libraries are needed. The included Windows build instructions use MinGW-w64. On Linux/macOS, compile with the corresponding compiler.

## Run on Windows (VS Code + MinGW-w64)
1. Install a C++ compiler such as MinGW-w64 and make sure `g++` is available in the VS Code terminal.
2. Open this project folder in VS Code.
3. In the terminal, run:
   ```powershell
   g++ backend/main.cpp backend/BudgetManager.cpp backend/HttpServer.cpp -std=c++17 -o budget_app.exe -lws2_32
   .\budget_app.exe
   ```
4. Open `http://localhost:8080` in your browser.

If your compiler doesn't support `-lws2_32`, check that you are using the Windows MinGW compiler. On Linux/macOS use:
```bash
g++ backend/main.cpp backend/BudgetManager.cpp backend/HttpServer.cpp -std=c++17 -o budget_app
./budget_app
```
5. Keep the terminal open while using the app. Press `Ctrl+C` to stop it.

## Data
The program creates `data/transactions.csv` and `data/budget.txt` in the project folder. Keep the folder structure unchanged.

## GitHub
Create a new empty repository on GitHub, then run:
```bash
git init
git add .
git commit -m "Initial Personal Expense and Smart Budget Manager"
git branch -M main
git remote add origin YOUR_GITHUB_REPOSITORY_URL
git push -u origin main
```

## Learning note
The code uses classes, objects, encapsulation, vectors, file handling, and basic HTTP request handling. Read the comments and try changing a small feature yourself before submission.
