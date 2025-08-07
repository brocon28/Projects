# LogAnalyzer - Apache Log File Analyzer
LogAnalyzer is a command-line Python tool for parsing and analyzing Apache access log files. It summarizes key statistics such as IP request frequency, most accessed endpoints, HTTP methods used, and response status codes. This tool is useful for system administrators, developers, or anyone needing insight into web server activity.

## Requirements
- Python 3.8 or higher
Optional dependencies can be installed via:

## Project Structure
LogAnalyzer/
├── analyzer.py # Main script to run analysis
├── parser.py # Parses individual log lines using regex
├── report.py # (Optional) Writes summaries to a file
├── utils.py # (Optional) Helper functions
├── logs/
│ ├── access.log # Sample full Apache log
│ └── test.log # Small log for quick testing
├── log_summary.txt # Output file for analysis results
├── requirements.txt # Dependencies list
└── README.md

## Usage
To analyze a log file, run the following:
python3 analyzer.py logs/access.log --save
Replace `logs/access.log` with the path to any Apache log file you want to analyze.
Also saves the file

## Sample Output
--- Summary Report ---

Total Requests: 9331
Total Unique IPs: 1674
Total Data Transferred: 2620.01 MB
Estimated Bot Requests: 1017

Top 5 IPs:
66.249.73.135: 432 requests
46.105.14.53: 364 requests
130.237.218.86: 293 requests
50.16.19.13: 113 requests
209.85.238.199: 102 requests

Top 5 Methods:
GET: 9325 requests
POST: 5 requests
OPTIONS: 1 requests

Top 5 Endpoints:
/favicon.ico: 788 requests
/style2.css: 532 requests
/reset.css: 528 requests
/images/jordan-80.png: 522 requests
/images/web/2009/banner.png: 506 requests

Status Code Counts:
200: 8913 responses
206: 45 responses
301: 163 responses
403: 2 responses
404: 205 responses
416: 2 responses
500: 1 responses

Total Errors (4xx and 5xx): 210
Example Error Line:
66.249.73.185 - - [17/May/2015:10:05:22 +0000] "GET /doc/index.html?org/elasticsearch/action/search/SearchResponse.html HTTP/1.1" 404 294 "-" "Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)"

## Notes
- Supports standard Apache log formats
- Can be extended to output JSON, CSV, or graphical summaries
- Built to be modular for future enhancements

