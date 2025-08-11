import argparse
import ipaddress

def valid_ip(s: str) -> str:
    try:
        ipaddress.ip_address(s)
        return s
    except ValueError:
        raise argparse.ArgumentTypeError(f"Invlaud Ip Address: {s}")

def parse_ports(s: str) -> list:
    parts = s.split(',')
    ports = set()
    for p in parts:
        p = p.strip()
        if not p:
            continue
        if '-' in p:
            try:
                start_str, end_str = p.split('-', 1)
                start = int(start_str)
                end = int(end_str)
            except ValueError:
                raise argparse.ArgumentTypeError(f"Invalid port range: {p}")
            if start < 1 or end > 65535 or start > end:
                raise argparse.ArgumentTypeError(f"Port range out of bounds: {p}")
            for port in range(start, end+1):
                ports.add(port)
        else:
            try:
                port = int(p)
            except ValueError:
                raise argparse.ArgumentTypeError(f"Invalid port: '{p}'")
            if port < 1 or port > 65535:
                raise argparse.ArgumentTypeError(f"Port out of bounds: {port}")
            ports.add(port)
    if not ports:
        raise argparse.ArgumentTypeError("No valid ports specified.")
    return sorted(ports)