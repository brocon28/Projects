# Python Port Scanner

A simple TCP port scanner written in Python. This tool scans specified ports on a given IP address to check if they are open or closed.

## Features

- Scan single ports, multiple ports, or port ranges.
- Supports IPv4 and IPv6 addresses.
- Configurable connection timeout.
- Optional logging of scan results.
- Basic TCP scanning.

## Requirements

- Python 3.6 or higher

## Usage

Run the scanner from the command line:

```bash
python3 main.py --ip <target_ip> --ports <ports> [options]

## Example Output
CLI: "python3 main.py --ip 127.0.0.1 --ports 22,80,443,8080,3306,5432 --log scan_results.txt"
Parsed arguments:
  IP:       127.0.0.1
  Ports:    6 ports (showing up to first 20): [22, 80, 443, 3306, 5432, 8080]
  Timeout:  1.0
  Threads:  100
  Log file: scan_results.txt
  Scan type: tcp

Starting scan...

Port 22: CLOSED
Port 80: CLOSED
Port 443: CLOSED
Port 3306: OPEN
Port 5432: CLOSED
Port 8080: OPEN

Scan complete.
Open ports: [3306, 8080]
