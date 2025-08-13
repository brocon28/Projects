# System Monitor

A Python-based system monitoring tool that displays real-time CPU, memory, disk, and network statistics directly in the terminal.  
Built using the `psutil` library for cross-platform compatibility.

## Features

- **CPU Monitoring**
  - Overall CPU usage
  - Per-core utilization
- **Memory Monitoring**
  - Total, used, free memory (in MB)
  - Memory usage percentage
- **Disk Monitoring**
  - Total, used, free disk space (in MB)
  - Disk usage percentage
- **Network Monitoring**
  - Total bytes sent and received
  - Upload and download speeds (per second)

## Installation
   git clone https://github.com/YOUR-USERNAME/system-monitor.git
   cd system-monitor
   pip install psutil
## Output
python3 main.py:
=== CPU Usage Monitor ===
Overall CPU Usage: 12.5%
  Core 0: 10.0%
  Core 1: 15.0%
  Core 2: 8.0%
  Core 3: 17.0%

=== Memory Usage Monitor ===
Total: 15831.5 MB
Used: 472.0 MB
Free: 15227.4 MB
Percent: 4.4%

=== Disk Usage Monitor ===
Total: 1031018.4 MB
Used: 2271.5 MB
Free: 976302.1 MB
Percent: 0.2%

=== Network Usage Monitor ===
Total Bytes Sent: 1.2 MB
Total Bytes Received: 33.0 MB
Upload Speed: 6.0 B/s
Download Speed: 6.0 B/s




