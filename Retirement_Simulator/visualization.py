import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
from monte_carlo import run_simulation, simulate_portfolio
import json


def visualize_simulations(json_file, num_simulations=100):
    """
    Plot a line for each Monte Carlo simulation.
    X-axis = Age, Y-axis = Portfolio Balance ($).
    Display summary results as text below the graph.
    """
    with open(json_file, 'r') as f:
        data = json.load(f)

    individuals = [ind for ind in data["individuals"] if ind.get("name") != "Joint"]
    joint = next((ind for ind in data["individuals"] if ind.get("name") == "Joint"), None)

    pre_ret_expenses = data.get("pre_retirement_expenses", 60000)
    post_ret_expenses = data.get("post_retirement_expenses", 60000)
    returns = data.get("returns")
    inflation = data.get("inflation", 0.025)

    # Run summary results
    results = run_simulation(json_file, num_simulations=num_simulations)
    res = list(results.values())[0]  # "Combined"

    # Find axis limits
    min_age = min(ind["current_age"] for ind in individuals)
    max_age = max(ind["planning_horizon"] for ind in individuals)

    # Create figure with two rows: top = plot, bottom = text
    fig, ax = plt.subplots(
        figsize=(12, 8),
        nrows=2,
        gridspec_kw={"height_ratios": [4, 1]}
    )
    ax_plot, ax_text = ax

    # --- Plot simulations ---
    for _ in range(num_simulations):
        hist, _, ages_per_year = simulate_portfolio(
            individuals, joint, pre_ret_expenses, post_ret_expenses, returns, inflation
        )
        balances = [sum(year.values()) for year in hist]
        ages = [age_list[0] if age_list[0] != "XX" else None for age_list in ages_per_year]

        # Ensure alignment with min_age
        valid_points = [(a, b) for a, b in zip(ages, balances) if a is not None]
        if valid_points:
            x, y = zip(*valid_points)
            # If the first age > min_age, prepend a starting point at min_age
            if x[0] > min_age:
                x = (min_age,) + x
                y = (y[0],) + y
            ax_plot.plot(x, y, alpha=0.15, color="blue")

    # --- Format axes ---
    ax_plot.set_title("Monte Carlo Simulator")
    ax_plot.set_xlabel("Age")
    ax_plot.set_ylabel("Portfolio Balance ($)")
    ax_plot.grid(True, linestyle="--", alpha=0.6)

    # Force axes to start at exact bounds, no padding
    ax_plot.set_xlim(min_age, max_age)
    ax_plot.margins(x=0)  # remove extra horizontal padding
    ax_plot.set_ylim(0, None)  # y starts at $0

    # Format y-axis with commas and $
    ax_plot.yaxis.set_major_formatter(
        mtick.FuncFormatter(lambda x, _: f"${x:,.0f}")
    )

    # --- Add text summary below ---
    summary_text = (
        f"Success Rate: {res['success_rate_percent']:.2f}%\n"
        f"Average Ending Balance: ${res['average_ending_balance']:,.2f}\n"
        f"Ending Balance + 1 SD: ${res['ending_balance_plus_1sd']:,.2f}\n"
        f"Ending Balance - 1 SD: ${res['ending_balance_minus_1sd']:,.2f}\n"
        f"Average Age Funds Last To: {res['average_funds_last_age']:.2f}"
    )
    ax_text.axis("off")  # hide text subplot axes
    ax_text.text(0.5, 0.5, summary_text, ha="center", va="center", fontsize=10)

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    visualize_simulations("married_retirement_input.json", num_simulations=100)