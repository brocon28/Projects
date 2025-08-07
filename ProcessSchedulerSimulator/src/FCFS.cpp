#include "../include/Process.h"
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

class FCFS {
    multimap<int, Process*> processMap;
public:
    FCFS(const vector<Process*>& processes) {
        for (auto p : processes) {
            processMap.insert({p->arrival_time, p});
        }
    }

    void execute() {
        int current_time = 0;
        double total_turnaround = 0;
        double total_waiting = 0;

        cout << left << setw(8) << "PID"
             << setw(15) << "Arrival Time"
             << setw(12) << "Burst Time"
             << setw(12) << "Start Time"
             << setw(17) << "Completion Time"
             << setw(17) << "Turnaround Time"
             << setw(14) << "Waiting Time" << endl;

        cout << string(95, '-') << endl;

        while (!processMap.empty()) {
            auto iter = processMap.begin();
            Process* p = iter->second;

            // If CPU is idle (current_time < arrival)
            if (current_time < p->arrival_time) {
                current_time = p->arrival_time;
            }

            int start_time = current_time;
            int completion_time = start_time + p->burst_time;
            int turnaround_time = completion_time - p->arrival_time;
            int waiting_time = turnaround_time - p->burst_time;

            // Print process info
            cout << left << setw(8) << p->pid
                 << setw(15) << p->arrival_time
                 << setw(12) << p->burst_time
                 << setw(12) << start_time
                 << setw(17) << completion_time
                 << setw(17) << turnaround_time
                 << setw(14) << waiting_time << endl;

            total_turnaround += turnaround_time;
            total_waiting += waiting_time;

            current_time = completion_time;
            processMap.erase(iter);
        }

        int n = processMap.size(); // It's 0 now, so use input size instead

        cout << string(95, '-') << endl;
        cout << "Average Turnaround Time: " << total_turnaround / (n == 0 ? 1 : n) << endl;
        cout << "Average Waiting Time:    " << total_waiting / (n == 0 ? 1 : n) << endl;
    }
};
