// Small helper to keep API calls in one place.
async function request(url, options = {}) {
    const response = await fetch(url, options);
    if (!response.ok) throw new Error("Request failed. Check the form and try again.");
    return response.json();
}

// Indian rupee formatting for amounts.
function money(value) {
    return "₹" + Number(value).toLocaleString("en-IN", {
        minimumFractionDigits: 2,
        maximumFractionDigits: 2
    });
}

async function loadSummary() {
    const data = await request("/api/summary");
    document.getElementById("income").textContent = money(data.income);
    document.getElementById("expenses").textContent = money(data.expenses);
    document.getElementById("balance").textContent = money(data.balance);
    document.getElementById("remaining").textContent = money(data.remaining);
    document.getElementById("budget-label").textContent = "Budget: " + money(data.budget);
    document.getElementById("budget-input").value = data.budget;
}

async function loadTransactions() {
    const items = await request("/api/transactions");
    const list = document.getElementById("transaction-list");
    document.getElementById("transaction-count").textContent =
        items.length + (items.length === 1 ? " entry" : " entries");

    if (items.length === 0) {
        list.innerHTML = '<tr><td colspan="4" class="empty">No transactions yet. Add your first entry above.</td></tr>';
        return;
    }

    // Newest added item appears first. Text is inserted safely using textContent.
    list.innerHTML = "";
    items.slice().reverse().forEach(item => {
        const row = document.createElement("tr");
        const title = document.createElement("td");
        title.textContent = item.title;
        const categoryCell = document.createElement("td");
        const tag = document.createElement("span");
        tag.className = "category";
        tag.textContent = item.category;
        categoryCell.appendChild(tag);
        const date = document.createElement("td");
        date.textContent = item.date;
        const amount = document.createElement("td");
        amount.className = item.type === "income" ? "income-amount" : "expense-amount";
        amount.textContent = (item.type === "income" ? "+" : "−") + money(item.amount);
        row.append(title, categoryCell, date, amount);
        list.appendChild(row);
    });
}

async function refreshPage() {
    await Promise.all([loadSummary(), loadTransactions()]);
}

// Use today's date as the default transaction date.
document.getElementById("date").value = new Date().toISOString().slice(0, 10);

document.getElementById("transaction-form").addEventListener("submit", async event => {
    event.preventDefault();
    const form = event.currentTarget;
    const message = document.getElementById("form-message");
    const values = new URLSearchParams(new FormData(form));
    try {
        await request("/api/transaction", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: values.toString()
        });
        message.textContent = "Transaction saved successfully.";
        form.reset();
        document.getElementById("date").value = new Date().toISOString().slice(0, 10);
        await refreshPage();
    } catch (error) {
        message.textContent = error.message;
    }
});

document.getElementById("budget-form").addEventListener("submit", async event => {
    event.preventDefault();
    const message = document.getElementById("budget-message");
    const values = new URLSearchParams(new FormData(event.currentTarget));
    try {
        await request("/api/budget", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: values.toString()
        });
        message.textContent = "Budget updated.";
        await loadSummary();
    } catch (error) {
        message.textContent = error.message;
    }
});

// Load saved data when the page opens.
refreshPage().catch(() => {
    document.getElementById("form-message").textContent =
        "Cannot connect to C++ backend. Start budget_app.exe in the VS Code terminal.";
});
