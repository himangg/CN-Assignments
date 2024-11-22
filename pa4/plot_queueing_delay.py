import matplotlib.pyplot as plt

# Initialize data structures
enqueue_times = {}  # Dictionary to store enqueue times for each packet
dequeue_events = []  # List to store (time, delay) tuples

# Parse the trace file
with open("tcp-example.tr", "r") as file:
    for line in file:
        parts = line.strip().split()
        
        # Skip lines that don't contain "Enqueue" or "Dequeue"
        if not ("Enqueue" in line or "Dequeue" in line):
            continue

        try:
            # Extract the event time and packet ID
            event_time = float(parts[1])
            packet_id = parts[-1]  # Assume the last field is the packet ID (adjust if needed)

            if "Enqueue" in line:
                # Store the enqueue time for the packet
                enqueue_times[packet_id] = event_time

            elif "Dequeue" in line:
                if packet_id in enqueue_times:
                    # Calculate queueing delay
                    enqueue_time = enqueue_times.pop(packet_id)
                    delay = event_time - enqueue_time
                    dequeue_events.append((event_time, delay))

        except ValueError:
            # Skip lines with unexpected formatting
            print(f"Skipping invalid line: {line.strip()}")

# Separate the dequeue events into time and delay for plotting
time = [event[0] for event in dequeue_events]
delay = [event[1] for event in dequeue_events]

# Plot the graph
plt.figure(figsize=(12, 8))
plt.scatter(time, delay, color='green', alpha=0.6, label="Queueing Delay")
plt.title("Queueing Delay Over Time", fontsize=14)
plt.xlabel("Time (seconds)", fontsize=12)
plt.ylabel("Queueing Delay (seconds)", fontsize=12)
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.legend(fontsize=12)
plt.tight_layout()

# Save and show the graph
plt.savefig("queueing_delay_plot_q3.png", dpi=300)
plt.show()
