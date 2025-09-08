#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <map>
#include <numeric>

using namespace std;

struct Point {
    string timestamp;
    double price;
};

struct Trade {
    string entryDate, exitDate;
    string side; // "LONG_SPREAD" or "SHORT_SPREAD"
    double entryPriceA, entryPriceB;
    double exitPriceA, exitPriceB;
    double qtyA, qtyB;
    double pnl;
};

static string trim(const string &s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    auto b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// load CSV with header: timestamp,close  (commas)
vector<Point> loadSimpleCSV(const string &filename) {
    vector<Point> out;
    ifstream ifs(filename);
    if (!ifs.is_open()) {
        cerr << "Failed to open " << filename << "\n";
        return out;
    }
    string line;
    getline(ifs, line); // skip header
    while (getline(ifs, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string ts, closeStr;
        if (!getline(ss, ts, ',')) continue;
        if (!getline(ss, closeStr, ',')) continue;
        ts = trim(ts);
        closeStr = trim(closeStr);
        try {
            double p = stod(closeStr);
            out.push_back({ts, p});
        } catch (...) {
            // skip bad line
            continue;
        }
    }
    return out;
}

// Align two series by timestamp (inner join)
void alignSeries(const vector<Point> &A, const vector<Point> &B,
                 vector<string> &timestamps, vector<double> &pa, vector<double> &pb) {
    map<string, double> ma, mb;
    for (auto &p : A) ma[p.timestamp] = p.price;
    for (auto &p : B) mb[p.timestamp] = p.price;
    for (auto &kv : ma) {
        auto it = mb.find(kv.first);
        if (it != mb.end()) {
            timestamps.push_back(kv.first);
            pa.push_back(kv.second);
            pb.push_back(it->second);
        }
    }
}

// Simple OLS regression priceA = alpha + beta * priceB
pair<double,double> olsSlopeIntercept(const vector<double> &x, const vector<double> &y) {
    int n = (int)x.size();
    if (n < 2) return {0.0, 0.0};
    double meanX = accumulate(x.begin(), x.end(), 0.0) / n;
    double meanY = accumulate(y.begin(), y.end(), 0.0) / n;
    double num = 0.0, den = 0.0;
    for (int i = 0; i < n; ++i) {
        num += (x[i] - meanX) * (y[i] - meanY);
        den += (x[i] - meanX) * (x[i] - meanX);
    }
    double beta = den == 0.0 ? 0.0 : num / den;
    double alpha = meanY - beta * meanX;
    return {alpha, beta};
}

vector<double> computeSpread(const vector<double> &A, const vector<double> &B, double alpha, double beta) {
    int n = (int)A.size();
    vector<double> s(n);
    for (int i = 0; i < n; ++i) s[i] = A[i] - (alpha + beta * B[i]);
    return s;
}

double rollingMeanWindow(const vector<double> &v, int start, int window) {
    double sum = 0.0;
    for (int i = start; i < start + window; ++i) sum += v[i];
    return sum / window;
}
double rollingStdWindow(const vector<double> &v, int start, int window, double mean) {
    double s = 0.0;
    for (int i = start; i < start + window; ++i) {
        double d = v[i] - mean;
        s += d*d;
    }
    return sqrt(s / window);
}

// Estimate half-life from AR(1) on s over a window.
double estimateHalfLife(const vector<double> &s, int start, int window) {
    if (start + window >= (int)s.size()) return 1e9;
    int n = window;
    vector<double> y(n), x(n);
    for (int i = 0; i < n; ++i) {
        y[i] = s[start + i + 1] - s[start + i];
        x[i] = s[start + i];
    }
    double meanX = accumulate(x.begin(), x.end(), 0.0) / n;
    double meanY = accumulate(y.begin(), y.end(), 0.0) / n;
    double num = 0.0, den = 0.0;
    for (int i = 0; i < n; ++i) {
        num += (x[i] - meanX) * (y[i] - meanY);
        den += (x[i] - meanX) * (x[i] - meanX);
    }
    if (den == 0.0) return 1e9;
    double b = num / den;
    double phi = 1.0 + b;
    if (phi <= 0.0 || phi >= 1.0) return 1e9;
    double halflife = -log(2.0) / log(phi);
    return halflife;
}

// Main pairs backtester (with automatic small-sample adjustments)
vector<Trade> backtestPairs(const vector<string> &timestamps,
                            const vector<double> &A,
                            const vector<double> &B,
                            int calibrateWindow,
                            int recomputeBetaEvery,
                            int spreadWindow,
                            double zEnter,
                            double zExit,
                            double volThreshold,
                            double capital,
                            double riskPerTrade,
                            double maxTradePct,
                            double stopLossSpread,
                            int maxHoldBars)
{
    vector<Trade> trades;
    int n = (int)A.size();
    if (n != (int)B.size() || n != (int)timestamps.size()) return trades;

    // Auto-adjust calibration window if not enough data:
    int minRequired = 10; // smallest allowed calibrateWindow for testing
    if (calibrateWindow + spreadWindow + 2 > n) {
        int newCal = max(minRequired, n/2); // use half the data for calibration if small sample
        cerr << "Not enough data for requested calibrateWindow+spreadWindow. "
             << "Auto-adjusting calibrateWindow from " << calibrateWindow << " to " << newCal << ".\n";
        calibrateWindow = newCal;
        if (calibrateWindow + spreadWindow + 2 > n) {
            // further reduce spreadWindow if needed
            spreadWindow = max(5, n - calibrateWindow - 2);
            cerr << "Also adjusted spreadWindow to " << spreadWindow << ".\n";
        }
    }

    if (n < calibrateWindow + spreadWindow + 2) {
        cerr << "Still not enough data after adjustments. Need at least calibrateWindow+spreadWindow+2 bars. Exiting.\n";
        return trades;
    }

    // initial calibration
    int betaStart = 0;
    int betaEnd = calibrateWindow; // [0, calibrateWindow)
    vector<double> X(B.begin() + betaStart, B.begin() + betaEnd);
    vector<double> Y(A.begin() + betaStart, A.begin() + betaEnd);
    auto ab = olsSlopeIntercept(X, Y);
    double alpha = ab.first, beta = ab.second;
    cout << fixed << setprecision(6);
    cout << "Initial OLS alpha=" << alpha << " beta=" << beta << "\n";
    cout << "Using calibrateWindow=" << calibrateWindow << ", spreadWindow=" << spreadWindow << "\n";

    vector<double> spread = computeSpread(A, B, alpha, beta);

    bool inPosition = false;
    bool longSpread = false;
    int entryIndex = -1;
    double entrySpread = 0.0;
    double entryPriceA = 0.0, entryPriceB = 0.0;
    double qtyA = 0.0, qtyB = 0.0;
    double equity = capital;

    for (int i = spreadWindow; i + 1 < n; ++i) {
        if (recomputeBetaEvery > 0 && (i % recomputeBetaEvery == 0) && (i >= calibrateWindow)) {
            vector<double> Xc(B.begin() + i - calibrateWindow, B.begin() + i);
            vector<double> Yc(A.begin() + i - calibrateWindow, A.begin() + i);
            auto abc = olsSlopeIntercept(Xc, Yc);
            alpha = abc.first; beta = abc.second;
            spread = computeSpread(A, B, alpha, beta);
            cout << "Recomputed alpha=" << alpha << " beta=" << beta << " at idx=" << i << " ts=" << timestamps[i] << "\n";
        }

        int start = i - spreadWindow + 1;
        double mean = rollingMeanWindow(spread, start, spreadWindow);
        double stddev = rollingStdWindow(spread, start, spreadWindow, mean);
        double currentSpread = spread[i];
        double z = (stddev == 0.0) ? 0.0 : (currentSpread - mean) / stddev;
        double halflife = estimateHalfLife(spread, start, spreadWindow);
        bool volOk = (stddev <= volThreshold);

        // Debug per bar
        cout << "i=" << i << " ts=" << timestamps[i]
             << " A=" << A[i] << " B=" << B[i]
             << " spread=" << currentSpread << " mean=" << mean << " std=" << stddev
             << " z=" << z << " hl=" << (halflife>1e8? -1.0: halflife)
             << " volOk=" << volOk << " inPos=" << inPosition << "\n";

        if (inPosition) {
            int held = i - entryIndex;
            bool exitSignal = false;
            string exitReason;

            if (longSpread && z >= zExit) { exitSignal = true; exitReason = "zExit"; }
            else if (!longSpread && z <= -zExit) { exitSignal = true; exitReason = "zExit"; }

            if (!exitSignal && stopLossSpread > 0.0) {
                if (longSpread && (currentSpread - entrySpread) <= -stopLossSpread) { exitSignal = true; exitReason = "stopLoss"; }
                if (!longSpread && (currentSpread - entrySpread) >= stopLossSpread) { exitSignal = true; exitReason = "stopLoss"; }
            }

            if (!exitSignal && held >= maxHoldBars) { exitSignal = true; exitReason = "maxHold"; }

            if (!exitSignal && stddev > volThreshold * 2.5) { exitSignal = true; exitReason = "volSpike"; }

            if (exitSignal) {
                double exitPriceA = A[i];
                double exitPriceB = B[i];

                double pnlA = (exitPriceA - entryPriceA) * qtyA;
                double pnlB = (entryPriceB - exitPriceB) * qtyB;
                double tradePnl = pnlA + pnlB;

                Trade tr;
                tr.entryDate = timestamps[entryIndex];
                tr.exitDate = timestamps[i];
                tr.side = longSpread ? "LONG_SPREAD" : "SHORT_SPREAD";
                tr.entryPriceA = entryPriceA;
                tr.entryPriceB = entryPriceB;
                tr.exitPriceA = exitPriceA;
                tr.exitPriceB = exitPriceB;
                tr.qtyA = qtyA;
                tr.qtyB = qtyB;
                tr.pnl = tradePnl;
                trades.push_back(tr);

                equity += tradePnl;

                cout << "EXIT " << tr.side << " entryIdx=" << entryIndex << " exitIdx=" << i << " reason=" << exitReason
                     << " pnl=" << tradePnl << " equity=" << equity << "\n";

                inPosition = false;
                entryIndex = -1;
                entryPriceA = entryPriceB = 0.0;
                qtyA = qtyB = 0.0;
            }
        } else {
            bool hlOk = (halflife > 0.0 && halflife < 2.0 * spreadWindow);
            if (hlOk && volOk) {
                if (z <= -zEnter) {
                    double dollarCap = capital * maxTradePct;
                    double exposurePerUnit = fabs(A[i]) + fabs(beta)*fabs(B[i]);
                    double units = (exposurePerUnit <= 0.0) ? 0.0 : (dollarCap / exposurePerUnit);
                    if (units <= 0.0) continue;
                    qtyA = units;
                    qtyB = beta * units;
                    entryPriceA = A[i];
                    entryPriceB = B[i];
                    entrySpread = currentSpread;
                    entryIndex = i;
                    inPosition = true;
                    longSpread = true;
                    cout << "ENTER LONG_SPREAD idx=" << i << " ts=" << timestamps[i]
                         << " qtyA=" << qtyA << " qtyB(short)=" << qtyB << " entrySpread=" << entrySpread << "\n";
                } else if (z >= zEnter) {
                    double dollarCap = capital * maxTradePct;
                    double exposurePerUnit = fabs(A[i]) + fabs(beta)*fabs(B[i]);
                    double units = (exposurePerUnit <= 0.0) ? 0.0 : (dollarCap / exposurePerUnit);
                    if (units <= 0.0) continue;
                    qtyA = units;
                    qtyB = beta * units;
                    entryPriceA = A[i];
                    entryPriceB = B[i];
                    entrySpread = currentSpread;
                    entryIndex = i;
                    inPosition = true;
                    longSpread = false;
                    cout << "ENTER SHORT_SPREAD idx=" << i << " ts=" << timestamps[i]
                         << " qtyA(short)=" << qtyA << " qtyB=" << qtyB << " entrySpread=" << entrySpread << "\n";
                }
            } // hlOk && volOk
        } // not in position
    } // for each bar

    ofstream ofs("trades_pairs.csv");
    ofs << "entryDate,exitDate,side,entryPriceA,entryPriceB,exitPriceA,exitPriceB,qtyA,qtyB,pnl\n";
    for (auto &t : trades) {
        ofs << t.entryDate << "," << t.exitDate << "," << t.side << ","
            << t.entryPriceA << "," << t.entryPriceB << ","
            << t.exitPriceA << "," << t.exitPriceB << ","
            << t.qtyA << "," << t.qtyB << "," << t.pnl << "\n";
    }
    ofs.close();

    return trades;
}

int main() {
    // filenames - replace with your files
    string fileA = "A.csv"; // e.g., "KO.csv"
    string fileB = "B.csv"; // e.g., "PEP.csv"

    cout << "Loading " << fileA << " and " << fileB << " ...\n";
    auto sa = loadSimpleCSV(fileA);
    auto sb = loadSimpleCSV(fileB);

    if (sa.empty() || sb.empty()) {
        cerr << "Error: one of the files is empty or not found.\n";
        return 1;
    }

    vector<string> timestamps;
    vector<double> A, B;
    alignSeries(sa, sb, timestamps, A, B);
    cout << "Aligned points: " << timestamps.size() << "\n";
    if (timestamps.size() < 200) {
        cout << "Warning: less than 200 aligned points; results will be noisy but will run for testing.\n";
    }

    // PARAMETERS - tune these
    int calibrateWindow = 120;      // initial regression window (will auto-shrink if data small)
    int recomputeBetaEvery = 0;     // 0 = never; set >0 to recompute beta periodically
    int spreadWindow = 20;
    double zEnter = 2.0;
    double zExit = 0.5;
    double volThreshold = 10.0;     // tune this to the scale of your spread
    double capital = 100000.0;
    double riskPerTrade = 0.02;
    double maxTradePct = 0.02;      // allocate 2% of capital per spread trade (conservative)
    double stopLossSpread = -1.0;   // disabled; set positive number (absolute spread adverse move) to enable
    int maxHoldBars = 60;

    // run backtest
    auto trades = backtestPairs(timestamps, A, B,
                                calibrateWindow,
                                recomputeBetaEvery,
                                spreadWindow,
                                zEnter,
                                zExit,
                                volThreshold,
                                capital,
                                riskPerTrade,
                                maxTradePct,
                                stopLossSpread,
                                maxHoldBars);

    // Summarize
    double totalPnL = 0.0;
    int wins = 0, losses = 0;
    for (auto &t : trades) {
        totalPnL += t.pnl;
        if (t.pnl > 0) ++wins; else ++losses;
    }
    cout << "Trades executed: " << trades.size() << " Wins: " << wins << " Losses: " << losses << " Total PnL: " << totalPnL << "\n";
    cout << "Trade log exported to trades_pairs.csv\n";
    return 0;
}
