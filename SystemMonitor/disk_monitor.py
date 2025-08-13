import psutil

def get_disk_usage(path='/'):
    return psutil.disk_usage(path)
