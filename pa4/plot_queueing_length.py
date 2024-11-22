import matplotlib.pyplot as plt

# Initialize variables
time = []  # List to store timestamps
queue_length = []  # List to store queue lengths
current_queue_length = 0  # Variable to track queue length over time

# Parse the trace file
with open("tcp-example.tr", "r") as file:
    for line in file:
        parts = line.strip().split()
        
        # Skip lines that don't contain "Enqueue" or "Dequeue"
        if not ("Enqueue" in line or "Dequeue" in line):
            continue
        
        try:
            # Extract the time (second field in verbose format)
            event_time = float(parts[1])
            
            # Determine the event type
            if "Enqueue" in line:
                current_queue_length += 1  # Increment queue length
            elif "Dequeue" in line:
                current_queue_length -= 1  # Decrement queue length

            # Ensure the queue length never goes negative
            current_queue_length = max(0, current_queue_length)

            # Store the time and queue length
            time.append(event_time)
            queue_length.append(current_queue_length)
        
        except ValueError:
            # Skip lines with unexpected formatting
            print(f"Skipping invalid line: {line.strip()}")

# Plot the graph
plt.figure(figsize=(12, 8))
plt.plot(time, queue_length, color='blue', linestyle='-', linewidth=1)
plt.title("Queue Length Over Time", fontsize=14)
plt.xlabel("Time (seconds)", fontsize=12)
plt.ylabel("Queue Length (Packets)", fontsize=12)
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.tight_layout()

# Save and show the graph
plt.savefig("queue_length_plot.png", dpi=300)
plt.show()
