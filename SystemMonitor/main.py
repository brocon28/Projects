import time
import os
from cpu_monitor import get_cpu_usage, get_per_core_usage
from memory_monitor import get_memory_usage
from disk_monitor import get_disk_usage
from network_monitor import get_network_usage, format_bytes, format_speed


def clear_screen():
    os.system("clear" if os.name != "nt" else "cls")


def bytes_to_mb(bytes_val):
    """Convert bytes to megabytes."""
    return bytes_val / (1024 * 1024)


def print_cpu_usage():
    overall = get_cpu_usage()
    per_core = get_per_core_usage()

    print("=== CPU Usage Monitor ===")
    print(f"Overall CPU Usage: {overall:.1f}%")
    for i, usage in enumerate(per_core):
        print(f"  Core {i}: {usage:.1f}%")


def print_memory_usage():
    mem = get_memory_usage()

    print("=== Memory Usage Monitor ===")
    print(f"Total: {bytes_to_mb(mem.total):.1f} MB")
    print(f"Used: {bytes_to_mb(mem.used):.1f} MB")
    print(f"Free: {bytes_to_mb(mem.free):.1f} MB")
    print(f"Percent: {mem.percent:.1f}%")


def print_disk_usage():
    disk = get_disk_usage()

    print("=== Disk Usage Monitor ===")
    print(f"Total: {bytes_to_mb(disk.total):.1f} MB")
    print(f"Used: {bytes_to_mb(disk.used):.1f} MB")
    print(f"Free: {bytes_to_mb(disk.free):.1f} MB")
    print(f"Percent: {disk.percent:.1f}%")


def print_network_usage():
    net_stats = get_network_usage()

    print("=== Network Usage Monitor ===")
    print(f"Total Bytes Sent: {format_bytes(net_stats['bytes_sent'])}")
    print(f"Total Bytes Received: {format_bytes(net_stats['bytes_recv'])}")
    print(f"Upload Speed: {format_speed(net_stats['sent_per_sec'])}")
    print(f"Download Speed: {format_speed(net_stats['recv_per_sec'])}")

def main():
    try:
        while True:
            clear_screen()
            print_cpu_usage()
            print_memory_usage()
            print_disk_usage()
            print_network_usage()
            time.sleep(5)
    except KeyboardInterrupt:
        print("\nExiting System Monitor.")


if __name__ == "__main__":
    main()
