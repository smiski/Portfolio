# 💸 Monte Carlo Retirement Simulator

**Description:**  
This Python project simulates retirement portfolio outcomes using Monte Carlo methods. It models the growth and depletion of individual or joint investment portfolios under stochastic market conditions, accounting for income, expenses, annual savings, inflation, and investment returns. The simulator generates probabilistic projections to help users plan and evaluate retirement readiness.  

**Highlights:**  
- **Stochastic Monte Carlo Simulation:** Run thousands of scenarios to assess portfolio longevity and risk under realistic market volatility.  
- **Customizable Inputs:** Supports multiple individuals, joint accounts, pre- and post-retirement expenses, and detailed investment allocations.  
- **Interactive Visualization & Reporting:** Generates plots of portfolio trajectories by age and provides success rates, ending balances, and other key statistics.

![Monte Carlo Simulation Screenshot - Married](./screenshots/married.png)

![Monte Carlo Simulation Screenshot - Single](./screenshots/single.png)

---

## Getting Started

### Requirements
- Python 3.8+  
- Packages: `numpy`, `matplotlib`  

Install the required packages using pip:  
```
pip install numpy matplotlib
```

### Running the Simulator

1.	Place the JSON input file in the same directory as the scripts. Example files provided:
	•	married_retirement_input.json – for joint individuals
	•	single.json – for a single individual

2.	Run the simulation and visualize results:
```
python visualization.py
```

3.	View the outputs:
	•	Interactive plot showing portfolio balances by age for all simulations
	•	Text summary including:
  	•	Success Rate (% of simulations ending with positive funds)
  	•	Average Ending Balance
  	•	Ending Balance ± 1 Standard Deviation
  	•	Average Age Funds Last

### Editing the JSON Inputs

You can customize scenarios by modifying the input JSON file:

Individual Information
	•	"name": Individual’s name or "Joint" for shared accounts
	•	"current_age": Current age in years
	•	"retirement_age": Age at retirement
	•	"planning_horizon": Age to simulate until (life expectancy)
	•	"pre_retirement_income": Annual income before retirement (salary, bonuses, etc.)
	•	"post_retirement_income": Annual income after retirement (Social Security, pensions, annuities, etc.)
	•	"portfolio": Current balances in each bucket
  	•	cash, taxable, tax_deferred, tax_free
	•	"annual_savings": Contributions per year to each bucket

Expenses
	•	"pre_retirement_expenses": Expected annual expenses before retirement
	•	"post_retirement_expenses": Expected annual expenses after retirement

Market Returns
	•	"returns": Expected market returns and volatility for each bucket
	•	mean – Expected average annual return (decimal, e.g., 0.06 = 6%)
	•	volatility – Standard deviation of annual returns (decimal)

Inflation and Simulations
	•	"inflation": Annual inflation rate (decimal)
	•	"num_simulations": Number of Monte Carlo simulations to run

### Tips ###

•	To simulate a joint portfolio, include an individual with "name": "Joint" and specify the shared portfolio and savings.
•	Modify "returns" and "inflation" to test different market conditions.
•	Increase "num_simulations" for more robust statistics (slower) or decrease for faster results (less precise).
