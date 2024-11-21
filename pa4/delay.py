input_file = "tcp-example.tr"  # Replace with your trace file name
output_file = "queue_delay.txt"

enqueue_times = {}  # Store enqueue times for packets
queue_delay_data = []

with open(input_file, "r") as infile:
    for line in infile:
        parts = line.strip().split()
        if len(parts) < 4:
            continue  # Skip invalid lines
        
        event = parts[0]  # First column: + or -
        timestamp = float(parts[1])  # Second column: time
        packet_id = parts[2]  # Third column: unique packet ID

        if event == "+":
            # Record enqueue time for the packet
            enqueue_times[packet_id] = timestamp
        elif event == "-":
            # Calculate queuing delay for the packet
            if packet_id in enqueue_times:
                delay = timestamp - enqueue_times[packet_id]
                queue_delay_data.append((timestamp, delay))
                del enqueue_times[packet_id]  # Remove processed packet

# Write queuing delay data to output file
with open(output_file, "w") as outfile:
    for time, delay in queue_delay_data:
        outfile.write(f"{time} {delay}\n")

print(f"Queue delay data written to {output_file}")
