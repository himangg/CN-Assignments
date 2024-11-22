import matplotlib.pyplot as plt

# Read data from the cwnd trace file
time = []
new_cwnd = []

with open("tcp-example.cwnd", "r") as file:
    for line in file:
        values = line.strip().split()
        if len(values) == 3:  # Ensure the line has three columns
            time.append(float(values[0]))       # Time column
            new_cwnd.append(int(values[2]))     # New CWND column

# Plot the graph
plt.figure(figsize=(12, 8))

# Plot new CWND
plt.plot(time, new_cwnd, color='red', linestyle='-', linewidth=2, label="New CWND")

# Graph formatting
plt.title("Congestion Window (CWND) Evolution Over Time", fontsize=14)
plt.xlabel("Time (seconds)", fontsize=12)
plt.ylabel("Congestion Window (Bytes)", fontsize=12)
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.legend(fontsize=12)
plt.tight_layout()

# Save and show the graph
plt.savefig("new_cwnd_plot_only_new_q3.png", dpi=300)
plt.show()
