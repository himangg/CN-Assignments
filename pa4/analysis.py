import subprocess
import csv
from datetime import datetime

def extract_data(pcap_file):
    """
    Use tshark to extract necessary fields from a pcap file.
    """
    cmd = [
        "tshark",
        "-r", pcap_file,
        "-T", "fields",
        "-e", "frame.time_epoch",
        "-e", "tcp.len"
    ]
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    # print(result)
    
    if result.returncode != 0:
        print("Error running tshark:", result.stderr)
        return []
    
    return result.stdout.strip().split("\n")

def calculate_throughput(data):
    """
    Calculate throughput based on the extracted data.
    """
    timestamps = []
    total_bytes = 0

    # count=0
    for row in data:
        try:
            # Split by whitespace to extract fields
            fields = row.split("\t")
            # if (count%1==0):
            #     print(fields)
            # count+=1
            if len(fields) < 2 or not fields[0] or not fields[1]:
                continue
            
            timestamp = float(fields[0])  # Frame timestamp in seconds
            tcp_len = int(fields[1]) if fields[1].isdigit() else 0  # TCP payload length
            
            timestamps.append(timestamp)
            total_bytes += tcp_len
        except ValueError:
            continue

    if not timestamps:
        print("No valid data extracted.")
        return 0

    # Calculate the duration of the capture
    duration = max(timestamps) - min(timestamps)

    if duration <= 0:
        print("Invalid duration detected.")
        return 0

    # Throughput = Total bytes / Duration (Bytes per second)
    throughput = total_bytes / duration
    return throughput

def main():
    pcap_file = "/home//kartikeya//ns-allinone-3.42//ns-3.42//tcp-example-2-0.pcap"  # Replace with your .pcap file path
    extracted_data = extract_data(pcap_file)

    if not extracted_data:
        print("No data extracted from pcap file.")
        return

    throughput = calculate_throughput(extracted_data)
    print(f"Average Throughput: {throughput:.2f} Bytes/Second of the file {pcap_file}")

if __name__ == "__main__":
    main()


# Average Throughput: 311868.03 Bytes/Second
# Average Throughput: 315526.98 Bytes/Second
# Average Throughput: 316159.24 Bytes/Second

