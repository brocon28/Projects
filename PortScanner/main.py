import argparse
import sys
import ipaddress
from scanner import scan_port
from logger import save_results
from utils import parse_ports, valid_ip


def build_parser():
    parser = argparse.ArgumentParser(prog='portscanner', description='Simple TCP port scanner')

    parser.add_argument('--ip', '-i',
                        required=True,
                        type=valid_ip,
                        help='Target IP address (IPv4 or IPv6)')

    parser.add_argument('--ports', '-p',
                        type=parse_ports,
                        default='1-1024',   # note: parse_ports expects a string, argparse will call it
                        help="Ports to scan. Examples: '22', '20-80', '22,80,443', '1-1024,8080-8090'. Default: 1-1024")

    parser.add_argument('--timeout', '-t',
                        type=float,
                        default=1.0,
                        help='Socket timeout in seconds for each connection attempt (default: 1.0)')

    parser.add_argument('--threads', '-T',
                        type=int,
                        default=100,
                        help='Number of worker threads for parallel scanning (default: 100)')

    parser.add_argument('--log', '-l',
                        type=str,
                        default=None,
                        help='Optional path to save scan results (e.g. results.txt)')

    parser.add_argument('--scan', '-s',
                        choices=['tcp'],
                        default='tcp',
                        help='Scan type (only tcp supported in this project)')

    return parser


if __name__ == '__main__':
    parser = build_parser()
    args = parser.parse_args()

    if isinstance(args.ports, str):
        args.ports = parse_ports(args.ports)

    print("Parsed arguments:")
    print("  IP:      ", args.ip)
    print("  Ports:   ", f"{len(args.ports)} ports (showing up to first 20):", args.ports[:20])
    print("  Timeout: ", args.timeout)
    print("  Threads: ", args.threads)
    print("  Log file:", args.log)
    print("  Scan type:", args.scan)

    print("\nStarting scan...\n")
    open_ports = []

    for port in args.ports:
        is_open = scan_port(args.ip, port, args.timeout)
        if is_open:
            print(f"Port {port}: OPEN")
            open_ports.append(port)
        else:
            print(f"Port {port}: CLOSED")

    print("\nScan complete.")
    print("Open ports:", open_ports)

    if args.log:
        save_results(args.ip, args.ports, open_ports, args.log)
