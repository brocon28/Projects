import argparse
import os
from parser import parse_log_line
from collections import Counter

BOT_KEYWORDS = ['bot', 'crawler', 'spider', 'slurp', 'archiver', 'fetcher']

def is_bot(user_agent):
    return any(keyword in user_agent.lower() for keyword in BOT_KEYWORDS)

def analyze_log_file(filepath, save_to_file=False):
    if not os.path.exists(filepath):
        print(f"Error: File '{filepath}' does not exist.")
        return

    ip_counter = Counter()
    method_counter = Counter()
    endpoint_counter = Counter()
    status_counter = Counter()
    error_lines = []
    unique_ips = set()
    total_bytes = 0
    bot_counter = 0
    total_requests = 0

    with open(filepath, 'r') as file:
        for line in file:
            parsed = parse_log_line(line)
            if parsed:
                ip = parsed['ip']
                method = parsed['method']
                endpoint = parsed['endpoint']
                status = parsed['status']
                size = int(parsed['size'])

                ip_counter[ip] += 1
                method_counter[method] += 1
                endpoint_counter[endpoint] += 1
                status_counter[status] += 1
                total_bytes += size
                unique_ips.add(ip)
                total_requests += 1

                if status.startswith('4') or status.startswith('5'):
                    error_lines.append(line.strip())

                if 'bot' in line.lower():  # Simple bot detection
                    bot_counter += 1

    report = []

    report.append("\n--- Summary Report ---\n")
    report.append(f"Total Requests: {total_requests}")
    report.append(f"Total Unique IPs: {len(unique_ips)}")
    report.append(f"Total Data Transferred: {total_bytes / (1024 * 1024):.2f} MB")
    report.append(f"Estimated Bot Requests: {bot_counter}")

    report.append("\nTop 5 IPs:")
    for ip, count in ip_counter.most_common(5):
        report.append(f"{ip}: {count} requests")

    report.append("\nTop 5 Methods:")
    for method, count in method_counter.most_common(5):
        report.append(f"{method}: {count} requests")

    report.append("\nTop 5 Endpoints:")
    for endpoint, count in endpoint_counter.most_common(5):
        report.append(f"{endpoint}: {count} requests")

    report.append("\nStatus Code Counts:")
    for status, count in sorted(status_counter.items()):
        report.append(f"{status}: {count} responses")

    report.append(f"\nTotal Errors (4xx and 5xx): {len(error_lines)}")
    if error_lines:
        report.append("Example Error Line:")
        report.append(error_lines[0])

    # Print to terminal
    print("\n".join(report))

    # Optionally save to file
    if save_to_file:
        output_file = "log_summary.txt"
        with open(output_file, 'w') as f:
            f.write("\n".join(report))
        print(f"\nReport saved to {output_file}")

def main():
    parser = argparse.ArgumentParser(description="Apache Log Analyzer")
    parser.add_argument("logfile", help="Path to the log file (e.g., logs/test.log)")
    parser.add_argument("--save", action="store_true", help="Save the report to log_summary.txt")
    args = parser.parse_args()

    analyze_log_file(args.logfile, save_to_file=args.save)

if __name__ == "__main__":
    main()
