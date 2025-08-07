import re

log_pattern = re.compile(
    r'(?P<ip>\S+) \S+ \S+ \[(?P<timestamp>[^\]]+)\] '
    r'"(?P<method>\S+) (?P<endpoint>\S+) \S+" '
    r'(?P<status>\d{3}) (?P<size>\d+)'
)

def parse_log_line(line):
    match = log_pattern.match(line)
    if match:
        #print(match.groupdict())
        return match.groupdict()
    return None
