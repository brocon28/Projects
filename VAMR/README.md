# Volatility-Adjusted Mean Reversion (VAMR) — Pairs Trading Backtester

## 📌 Overview
This project implements a **Volatility-Adjusted Mean Reversion (VAMR)** strategy in **C++**.  
Originally designed as a single-asset mean-reversion system on SPY, the strategy evolved into a **pairs trading framework** using **Coca-Cola (KO)** and **Pepsi (PEP)**.

Pairs trading exploits the mean-reverting relationship between two correlated assets, generating signals when the spread between them deviates significantly from its historical average.

---

## ⚙️ Strategy Logic
- **Step 1:** Model KO as a function of PEP using regression (KO = α + β·PEP).
- **Step 2:** Compute the spread = KO – (α + β·PEP).
- **Step 3:** Standardize spread into a z-score.
- **Step 4:** Enter trades:
    - **Long Spread (Buy KO, Short PEP)** when z ≤ –2.
    - **Short Spread (Short KO, Buy PEP)** when z ≥ +2.
- **Step 5:** Exit when z returns near 0.
- **Risk Controls:**
    - Max 2% of capital per trade.
    - Volatility filter to avoid unstable regimes.
    - Max 60-day hold period.

---

## 📊 Outputs
- Console log: Debug info (dates, spreads, z-scores, volatility checks).
- CSV export: `trades_pairs.csv` with entry/exit prices, position sizes, and PnL.
- Summary stats: Total trades, win/loss ratio, cumulative profit/loss.

---

## 🛠️ Technical Details
- **Language:** C++
- **Inputs:** `A.csv` (KO prices), `B.csv` (PEP prices).
- **Dependencies:** Standard Library only (no external libraries required).

---

## 🚀 Next Steps
- Extend backtests with 5+ years of KO/PEP data.
- Add transaction cost modeling.
- Experiment with additional pairs (XOM/CVX, JPM/BAC, etc.).
- Compare results to vanilla mean reversion on SPY.

---

## 👤 Author
Developed by Brady O’Connor as part of an application project for the **AlgoGators Quant Fund**.  
I’m new to trading but excited about algorithmic finance and building strategies from scratch.

---
