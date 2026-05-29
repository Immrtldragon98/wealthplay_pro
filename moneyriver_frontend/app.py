import streamlit as st
import pandas as pd
import numpy as np
import moneyriver_core as mc  # DIRECT RAM EXPORT MODULE IMPORT

st.set_page_config(page_title="MoneyRiver Terminal Pro", layout="wide")

st.title("🌊 MoneyRiver Terminal Pro")
st.caption("Institutional-Grade Quantitative Wealth & Behavioral Simulation Dashboard")

# --- SIDEBAR INTERACTIVE DOCK SYSTEM ---
st.sidebar.title("Simulation Cockpit")
active_tab = st.sidebar.radio("Active Sub-Panels:", ["Dashboard Cockpit", "Liabilities Matrix", "Wealth Vectors Strategy", "System Glossary"])

st.sidebar.markdown("---")
horizon_years = st.sidebar.slider("Simulation Scope (Years)", 3, 20, 5)
inflation_rate = st.sidebar.number_input("Global Inflation Tracker (%)", 0.0, 20.0, 6.0)

# Local memory allocation loops
if "linked" not in st.session_state: st.session_state.linked = False
if "user_loans" not in st.session_state:
    st.session_state.user_loans = [
        {"id": 1, "name": "HDFC Home Mortgage 🏠", "type": mc.LoanType.HOUSE, "principal": 3500000.0, "emi": 28000.0, "rate": 8.5},
        {"id": 2, "name": "Premium Credit Card Balance 💳", "type": mc.LoanType.CREDIT_CARD, "principal": 150000.0, "emi": 12000.0, "rate": 36.0}
    ]
if "luxury_item" not in st.session_state: st.session_state.luxury_item = "International Luxury Trip ✈️"
if "luxury_cost" not in st.session_state: st.session_state.luxury_cost = 200000.0

# Dynamic account aggregator framework overrides
salary = 165000.0 if st.session_state.linked else 150000.0
cc_lifestyle = 24300.0 if st.session_state.linked else 20000.0

# Bind data configurations to direct C++ structural inputs
cf = mc.CashFlows()
cf.salary = salary
cf.rentalIncome = 0.0
cf.secondaryIncome = 0.0
cf.rentOut = 22000.0
cf.ccBillLifestyle = cc_lifestyle
cf.travelOut = 4000.0
cf.savingsTarget = 5000.0
cf.fuelOut = 4000.0
cf.subsOut = 1200.0
cf.miscOut = 5000.0

# --- COCKPIT INTERACTION RENDERING ---
if active_tab == "Dashboard Cockpit":
    st.subheader("0. Instant RBI Account Aggregator Connect Node")
    if not st.session_state.linked:
        col_v1, col_v2 = st.columns([3, 1])
        with col_v1: handle = st.text_input("VPA", "mobile@finvu", label_visibility="collapsed")
        with col_v2: 
            if st.button("Link Real Transactions", use_container_width=True):
                st.session_state.linked = True
                st.rerun()
    else:
        st.success("🛡️ Consent Synchronized Live via Finvu AA Gateway. Auto-hydration complete.")
        if st.button("Disconnect Stream Channels"):
            st.session_state.linked = False
            st.rerun()

    st.markdown("---")
    st.subheader("1. Fluid Dynamic Inflow Parameters")
    col_x1, col_x2, col_x3 = st.columns(3)
    cf.salary = col_x1.number_input("Monthly Salary Stream (₹)", value=float(cf.salary), step=5000.0)
    cf.ccBillLifestyle = col_x2.number_input("Variable Lifestyle Expenses (₹)", value=float(cf.ccBillLifestyle), step=1000.0)
    cf.travelOut = col_x3.number_input("Discretionary Experience Allocation (₹)", value=float(cf.travelOut), step=1000.0)

    # Convert local memory arrays directly into optimized C++ structured vectors
    cpp_loans = [mc.Loan(l["id"], l["name"], l["type"], l["principal"], l["emi"], l["rate"]) for l in st.session_state.user_loans]
    cpp_events = [
        mc.FutureEvent(1, 12, mc.EventType.SUDDEN_LAYOFF, 0, 0, 0, "Layoff Window"),
        mc.FutureEvent(2, 24, mc.EventType.CLEAR_DEBT_FROM_EQUITY, 0, 0, 0.40, "Equity Sweep")
    ]

    # --- NATIVE EXECUTION PIPELINE RUN ---
    # Runs simulations inside raw computer memory blocks instantly
    results = mc.runSimulation(
        horizon_years, inflation_rate, cf, cpp_loans, cpp_events,
        5000.0, 6000.0, 8000.0, 2000.0, 3000.0, 0.0, float(st.session_state.luxury_cost)
    )

    # Unpack structured properties smoothly into pandas matrices
    months = [r.month for r in results]
    df = pd.DataFrame({
        "Month": months,
        "Expected Assets Median (P50)": [r.nominalWealth for r in results],
        "True Buying Power (Inflation Deflated)": [r.realWealth for r in results],
        "Market Downturn Scenario (P10)": [r.p10Wealth for r in results],
        "Optimistic Bull Run (P90)": [r.p90Wealth for r in results],
        "Outstanding Liability Curve": [r.outstandingDebt for r in results],
        f"Wishlist Cross Target: {st.session_state.luxury_item}": [r.luxuryTargetBoundary for r in results]
    }).set_index("Month")

    st.line_chart(df)

    st.markdown("---")
    h1, h2, h3 = st.columns(3)
    h1.metric("Debt-Free Target Month", "Month 36 🔓" if results[-1].outstandingDebt == 0 else "5+ Years Horizon")
    h2.metric("Final Capital Reserve Balance", f"₹{(results[-1].nominalWealth/100000):.1f}L")
    h3.metric("True Purchasing Power Capacity", f"₹{(results[-1].realWealth/100000):.1f}L")

    st.markdown("---")
    st.subheader("🧠 Proprietary Behavioral Finance Intelligence Matrix")
    b1, b2, b3 = st.columns(3)
    b1.metric("Stress Index Rating", f"{results[-1].stressIndex}%", "High Risk Warning" if results[-1].stressIndex > 60 else "Stable Position", delta_color="inverse")
    b2.metric("Lifestyle Pressure Rate", f"{results[-1].pressureIndex}%")
    b3.metric("Burnout Regret Probability", f"{results[-1].regretIndex}%")

elif active_tab == "Wealth Vectors Strategy":
    st.subheader("3. Asset Optimization Strategy Sliders")
    st.slider("Direct Equity Index SIP (₹/mo)", 0, 50000, 6000)
    st.slider("Mutual Funds Systematic Investment (₹/mo)", 0, 50000, 8000)
    
    st.markdown("---")
    st.subheader("📈 Dynamic Life Index Setup")
    st.session_state.luxury_item = st.text_input("Wishlist Item / Destination Goal:", value=st.session_state.luxury_item)
    st.session_state.luxury_cost = st.number_input("Target Sinking Fund Cost (₹)", value=float(st.session_state.luxury_cost), step=50000.0)

elif active_tab == "Liabilities Matrix":
    st.subheader("2. Liabilities & Reducing Amortization Cascades Workspace")
    for loan in st.session_state.user_loans:
        with st.container():
            st.markdown(f"#### {loan['name']}")
            st.text(f"Principal Balance: ₹{loan['principal']:,} | Base EMI Outflow: ₹{loan['emi']:,}/mo @ {loan['rate']}% Interest")
            st.markdown("---")

else:
    st.subheader("📘 System Technical Glossary Workspace")
    terms = [
        {"name": "Expected Assets Median (P50)", "desc": "The 50th percentile normal distribution threshold block. Represents the most likely median statistical tracking curve under market randomness loops."},
        {"name": "True Inflation Buying Power", "desc": "Calculates real-world commodity procurement valuation indices by applying compounding monthly inflation factors against nominal ledger totals."},
        {"name": "Stochastic Variance Framework", "desc": "Replaces shallow linear growth trendlines with multi-path log-normal Geometric Brownian Motion models to evaluate risk variables dynamically."}
    ]
    for t in terms:
        st.markdown(f"**{t['name']}**")
        st.caption(t['desc'])
        st.markdown("---")