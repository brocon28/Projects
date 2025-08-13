import psutil


def get_memory_usage():
    # Returns overall CPU usage percent
    return psutil.virtual_memory()


