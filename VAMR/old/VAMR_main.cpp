#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

// ------------------------
// Data Structures
// ------------------------
struct OHLCV {
    string timestamp;
    double open, high, low, close;
    long volume;
};

struct Trade {
    string timestamp;
    string action; // "BUY", "SELL", "SHORT", "COVER"
    double price;
    double size;
    double pnl;
};

// ------------------------
// Utility Functions
// ------------------------
string trim(const string& s) {
    auto start = s.begin();
    while (start != s.end() && isspace(*start)) start++;
    auto end = s.end();
    do { end--; } while (distance(start, end) > 0 && isspace(*end));
    return string(start, end + 1);
}

bool safeStod(const string& s, double& val) {
    try { val = stod(trim(s)); return true; }
    catch (...) { return false; }
}

bool safeStol(const string& s, long& val) {
    try { val = stol(trim(s)); return true; }
    catch (...) { return false; }
}

vector<OHLCV> loadCSV(const string& filename) {
    vector<OHLCV> data;
    ifstream file(filename);
    if (!file.is_open()) { cerr << "Failed to open file: " << filename << endl; return data; }

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        OHLCV bar;

        if (!getline(ss, bar.timestamp, ',')) continue;
        if (!getline(ss, token, ',') || !safeStod(token, bar.open)) continue;
        if (!getline(ss, token, ',') || !safeStod(token, bar.high)) continue;
        if (!getline(ss, token, ',') || !safeStod(token, bar.low)) continue;
        if (!getline(ss, token, ',') || !safeStod(token, bar.close)) continue;
        if (!getline(ss, token, ',') || !safeStol(token, bar.volume)) continue;

        data.push_back(bar);
    }
    file.close();
    return data;
}

double rollingMean(const vector<double>& prices, int startIdx, int window) {
    double sum = 0.0;
    for (int i = startIdx; i < startIdx + window; ++i) sum += prices[i];
    return sum / window;
}

double rollingStdDev(const vector<double>& prices, int startIdx, int window, double mean) {
    double sum = 0.0;
    for (int i = startIdx; i < startIdx + window; ++i) sum += pow(prices[i] - mean, 2);
    return sqrt(sum / window);
}

// ------------------------
// VAMR Strategy Optimized
// ------------------------
vector<Trade> runVAMRStrategy(const vector<OHLCV>& data,
                              int window = 20,
                              double zThreshold = 2.0,
                              double exitZ = 0.0,
                              double volMultiplier = 2.0,
                              double capital = 100000.0,
                              double riskPerTrade = 0.02,
                              double maxTradePct = 0.1,
                              double stopLossPct = 0.05)
{
    vector<Trade> trades;
    vector<double> closes;
    for (auto& bar : data) closes.push_back(bar.close);

    double cash = capital;
    double position = 0.0;
    double entryPrice = 0.0;
    bool isLong = false, isShort = false;
    int holdBars = 0;

    for (size_t i = 0; i + window < closes.size(); ++i) {
        double mean = rollingMean(closes, i, window);
        double stddev = rollingStdDev(closes, i, window, mean);
        if (stddev == 0) continue;
        double price = closes[i + window];
        double zScore = (price - mean) / stddev;

        // Adjusted volatility filter
        double volThreshold = stddev * volMultiplier;

        holdBars++;

        // --- LONG ENTRY ---
        if (!isLong && !isShort && zScore <= -zThreshold) {
            double size = min((capital * riskPerTrade) / stddev, capital * maxTradePct / price);
            cash -= price * size;
            position += size;
            entryPrice = price;
            holdBars = 0;
            isLong = true;
            trades.push_back({data[i + window].timestamp, "BUY", price, size, 0.0});
            cout << "BUY executed: Size=" << size << " at Price=" << price << endl;
        }

        // --- LONG EXIT ---
        if (isLong && (price <= entryPrice * (1 - stopLossPct) || zScore >= exitZ)) {
            double pnl = (price - entryPrice) * position;
            cash += price * position;
            trades.push_back({data[i + window].timestamp, "SELL", price, position, pnl});
            cout << "SELL executed: Size=" << position << " at Price=" << price << " | PnL=" << pnl << endl;
            position = 0;
            isLong = false;
            holdBars = 0;
        }

        // --- SHORT ENTRY ---
        if (!isShort && !isLong && zScore >= zThreshold) {
            double size = min((capital * riskPerTrade) / stddev, capital * maxTradePct / price);
            position += size;
            entryPrice = price;
            cash += price * size; // Cash received from short
            holdBars = 0;
            isShort = true;
            trades.push_back({data[i + window].timestamp, "SHORT", price, size, 0.0});
            cout << "SHORT executed: Size=" << size << " at Price=" << price << endl;
        }

        // --- SHORT EXIT ---
        if (isShort && (price >= entryPrice * (1 + stopLossPct) || zScore <= exitZ)) {
            double pnl = (entryPrice - price) * position;
            cash -= price * position; // Covering short
            trades.push_back({data[i + window].timestamp, "COVER", price, position, pnl});
            cout << "COVER executed: Size=" << position << " at Price=" << price << " | PnL=" << pnl << endl;
            position = 0;
            isShort = false;
            holdBars = 0;
        }
    }

    return trades;
}

// ------------------------
// Main
// ------------------------
int main() {
    string filename = "daily_SPY.csv";
    vector<OHLCV> data = loadCSV(filename);

    if (data.empty()) {
        cerr << "No data loaded!" << endl;
        return 1;
    }

    vector<Trade> trades = runVAMRStrategy(data);

    double totalPnL = 0.0;
    cout << fixed << setprecision(2);
    cout << "\nTrades Summary:\n";
    cout << "Timestamp\tAction\tPrice\tSize\tPnL\n";
    for (auto& t : trades) {
        cout << t.timestamp << "\t" << t.action << "\t" << t.price << "\t" << t.size << "\t" << t.pnl << "\n";
        totalPnL += t.pnl;
    }

    cout << "\nTotal PnL: $" << totalPnL << endl;
    return 0;
}
