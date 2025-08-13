import psutil
import time

_last_sent = None
_last_recv = None
_last_time = None

def get_network_usage():
    """Return total bytes sent/received and upload/download speed."""
    global _last_sent, _last_recv, _last_time

    counters = psutil.net_io_counters()
    bytes_sent = counters.bytes_sent
    bytes_recv = counters.bytes_recv
    current_time = time.time()

    if _last_sent is None:
        # First call — no speed data yet
        _last_sent, _last_recv, _last_time = bytes_sent, bytes_recv, current_time
        return {
            "bytes_sent": bytes_sent,
            "bytes_recv": bytes_recv,
            "sent_per_sec": 0,
            "recv_per_sec": 0
        }

    # Time elapsed since last call
    elapsed = current_time - _last_time
    if elapsed <= 0:
        elapsed = 1e-6  # avoid division by zero

    # Calculate per-second rates
    sent_per_sec = (bytes_sent - _last_sent) / elapsed
    recv_per_sec = (bytes_recv - _last_recv) / elapsed

    # Update previous values
    _last_sent, _last_recv, _last_time = bytes_sent, bytes_recv, current_time

    return {
        "bytes_sent": bytes_sent,
        "bytes_recv": bytes_recv,
        "sent_per_sec": sent_per_sec,
        "recv_per_sec": recv_per_sec
    }

def format_bytes(size):
    """Convert bytes into human-readable format."""
    for unit in ['B', 'KB', 'MB', 'GB']:
        if size < 1024:
            return f"{size:.1f} {unit}"
        size /= 1024
    return f"{size:.1f} TB"


def format_speed(size):
    """Convert bytes/sec into human-readable bits/sec format."""
    bits = size * 8
    for unit in ['bps', 'Kbps', 'Mbps', 'Gbps']:
        if bits < 1000:
            return f"{bits:.1f} {unit}"
        bits /= 1000
    return f"{bits:.1f} Tbps"
