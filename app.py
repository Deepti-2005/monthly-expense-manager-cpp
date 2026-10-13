import streamlit as st
import pandas as pd
from datetime import date
from collections import defaultdict

st.set_page_config(page_title="Monthly Expense Manager", page_icon="💸", layout="wide")

st.markdown("""
<style>
.block-container {padding-top: 2rem; padding-bottom: 2rem;}
[data-testid="stMetric"] {background: #f5f7fb; padding: 16px; border-radius: 12px;}
</style>
""", unsafe_allow_html=True)

st.title("💸 Monthly Expense Manager")
st.caption("Track income, manage expenses, and keep your monthly budget on track.")

if "transactions" not in st.session_state:
    st.session_state.transactions = [
        {"Date": date.today().isoformat(), "Type": "Income", "Category": "Salary", "Description": "Sample income", "Amount": 25000.0, "Payment": "Bank Transfer"},
        {"Date": date.today().isoformat(), "Type": "Expense", "Category": "Food", "Description": "Lunch", "Amount": 250.0, "Payment": "UPI"},
        {"Date": date.today().isoformat(), "Type": "Expense", "Category": "Travel", "Description": "Bus fare", "Amount": 80.0, "Payment": "Cash"},
    ]
if "budgets" not in st.session_state:
    st.session_state.budgets = {}

transactions = st.session_state.transactions
today = date.today()
month_key = today.strftime("%Y-%m")

with st.sidebar:
    st.header("Navigation")
    page = st.radio("Choose a section", ["Dashboard", "Add Transaction", "All Transactions", "Monthly Budget"])
    st.divider()
    st.caption("Demo note: this hosted demo stores data in the current app session. Refreshing/restarting may reset it.")

def current_month_rows():
    return [t for t in transactions if str(t["Date"]).startswith(month_key)]

month_rows = current_month_rows()
income_total = sum(t["Amount"] for t in month_rows if t["Type"] == "Income")
expense_total = sum(t["Amount"] for t in month_rows if t["Type"] == "Expense")
balance = income_total - expense_total
budget = float(st.session_state.budgets.get(month_key, 10000.0))
remaining = budget - expense_total
usage = (expense_total / budget * 100) if budget > 0 else 0

if page == "Dashboard":
    st.subheader(f"Overview · {today.strftime('%B %Y')}")
    a, b, c, d = st.columns(4)
    a.metric("Total Income", f"₹{income_total:,.2f}")
    b.metric("Total Expenses", f"₹{expense_total:,.2f}")
    c.metric("Balance", f"₹{balance:,.2f}")
    d.metric("Monthly Budget", f"₹{budget:,.2f}")
    st.subheader("Budget status")
    st.progress(min(max(usage / 100, 0.0), 1.0))
    st.write(f"**{usage:.1f}% used** · ₹{max(remaining, 0):,.2f} remaining")
    if usage >= 100:
        st.error("Budget exceeded! Review your spending.")
    elif usage >= 80:
        st.warning("You have used at least 80% of your budget.")
    else:
        st.success("You are within your budget.")
    left, right = st.columns(2)
    with left:
        st.subheader("Expense by category")
        category_totals = defaultdict(float)
        for t in month_rows:
            if t["Type"] == "Expense":
                category_totals[t["Category"]] += t["Amount"]
        if category_totals:
            chart_df = pd.DataFrame({"Category": list(category_totals.keys()), "Amount": list(category_totals.values())}).set_index("Category")
            st.bar_chart(chart_df)
        else:
            st.info("Add an expense to see category analytics.")
    with right:
        st.subheader("Recent transactions")
        if transactions:
            st.dataframe(pd.DataFrame(transactions).tail(5).iloc[::-1], use_container_width=True, hide_index=True)
        else:
            st.info("No transactions yet.")

elif page == "Add Transaction":
    st.subheader("Add a transaction")
    with st.form("transaction_form", clear_on_submit=True):
        kind = st.radio("Transaction type", ["Expense", "Income"], horizontal=True)
        trans_date = st.date_input("Date", value=today)
        if kind == "Expense":
            category_options = ["Food", "Travel", "Shopping", "Bills", "Health", "Education", "Entertainment", "Other"]
        else:
            category_options = ["Salary", "Freelance", "Business", "Gift", "Other"]
        category = st.selectbox("Category / Source", category_options)
        amount = st.number_input("Amount (₹)", min_value=1.0, step=50.0)
        description = st.text_input("Description", placeholder="e.g. Groceries, monthly salary")
        payment = st.selectbox("Payment method", ["UPI", "Cash", "Debit/Credit Card", "Bank Transfer", "Other"])
        submitted = st.form_submit_button("Save transaction", type="primary")
        if submitted:
            transactions.append({
                "Date": trans_date.isoformat(), "Type": kind, "Category": category,
                "Description": description.strip() or category, "Amount": float(amount), "Payment": payment
            })
            st.success(f"{kind} of ₹{amount:,.2f} added successfully.")
            st.rerun()

elif page == "All Transactions":
    st.subheader("All transactions")
    if transactions:
        df = pd.DataFrame(transactions)
        f1, f2 = st.columns(2)
        with f1:
            type_filter = st.selectbox("Type", ["All", "Expense", "Income"])
        with f2:
            categories = ["All"] + sorted(df["Category"].dropna().unique().tolist())
            category_filter = st.selectbox("Category", categories)
        if type_filter != "All":
            df = df[df["Type"] == type_filter]
        if category_filter != "All":
            df = df[df["Category"] == category_filter]
        st.dataframe(df.sort_values("Date", ascending=False), use_container_width=True, hide_index=True)
        st.download_button("Download transactions as CSV", df.to_csv(index=False).encode("utf-8"), "transactions.csv", "text/csv")
    else:
        st.info("No transactions recorded yet.")

elif page == "Monthly Budget":
    st.subheader("Set monthly budget")
    st.write(f"Current month: **{today.strftime('%B %Y')}**")
    new_budget = st.number_input("Budget amount (₹)", min_value=0.0, value=float(budget), step=500.0)
    if st.button("Save budget", type="primary"):
        st.session_state.budgets[month_key] = float(new_budget)
        st.success("Monthly budget updated.")
        st.rerun()
    st.divider()
    st.metric("Spent this month", f"₹{expense_total:,.2f}")
    st.metric("Budget remaining", f"₹{new_budget - expense_total:,.2f}")
    if new_budget > 0:
        st.progress(min(max(expense_total / new_budget, 0.0), 1.0))
        st.caption(f"{expense_total / new_budget * 100:.1f}% of budget used")

st.divider()
st.caption("Academic project demo · Monthly Expense Manager · C++ project companion web interface")
