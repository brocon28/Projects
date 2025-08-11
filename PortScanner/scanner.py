import socket
import utils

def scan_port(ip, port, timeout=1.0):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(timeout)
    try:
        result = sock.connect_ex((ip, port))
        #print(f"Debug: Trying {ip}:{port}, connect_ex returned {result}")
        return result == 0
    except socket.error as e:
        print(f"Socket error: {e}")
        return False
    finally:
        sock.close()

