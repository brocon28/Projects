import datetime

def save_results(ip, ports_scanned, open_ports, log_path):
    with open(log_path, 'a') as log_file:
        log_file.write(f"Scan Timestamp: {datetime.datetime.now()}\n")
        log_file.write(f"Target IP: {ip}\n")
        log_file.write(f"Ports scanned: {ports_scanned}\n")
        log_file.write(f"Open ports: {open_ports}\n")
        log_file.write("-" * 40 + "\n")
