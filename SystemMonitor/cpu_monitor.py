import psutil

def get_cpu_usage():
    # Returns overall CPU usage percent
    return psutil.cpu_percent(interval=1)

def get_per_core_usage():
    # Returns list of per-core CPU usage percents
    return psutil.cpu_percent(interval=1, percpu=True)
