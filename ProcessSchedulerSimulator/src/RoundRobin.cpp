#include "../include/Process.h"
#include <vector>
#include <iostream>
#include <queue>
#include <iomanip>
using namespace std;

class RoundRobin {
    vector<Process*> processes;
    int time_quantum;

public:
    RoundRobin(const vector<Process*>& procs, int tq) : processes(procs), time_quantum(tq) {}

    void execute() {
        int n = processes.size();
        vector<int> remaining_time(n);
        vector<int> start_times;  // timeline start times for Gantt chart
        vector<string> gantt_blocks; // to hold each block string

        for (int i = 0; i < n; i++)
            remaining_time[i] = processes[i]->burst_time;

        queue<int> q;
        int current_time = 0;

        // Enqueue processes that arrive at time 0
        for (int i = 0; i < n; i++) {
            if (processes[i]->arrival_time <= current_time)
                q.push(i);
        }

        vector<bool> in_queue(n, false);
        for (int i = 0; i < n; i++) {
            if (processes[i]->arrival_time <= current_time)
                in_queue[i] = true;
        }

        cout << "Quantum time: " << time_quantum << "\n";

        while (!q.empty()) {
            int idx = q.front();
            q.pop();

            start_times.push_back(current_time);

            int exec_time = min(time_quantum, remaining_time[idx]);

            // Create the block string with fixed width for alignment
            string block = "| P" + to_string(processes[idx]->pid) + "(" + to_string(remaining_time[idx]) + ")";
            int block_width = 8;
            if ((int)block.size() < block_width)
                block += string(block_width - block.size(), ' ');
            gantt_blocks.push_back(block);

            current_time += exec_time;
            remaining_time[idx] -= exec_time;

            // Enqueue newly arrived processes during this time slice
            for (int i = 0; i < n; i++) {
                if (!in_queue[i] && processes[i]->arrival_time <= current_time) {
                    q.push(i);
                    in_queue[i] = true;
                }
            }

            if (remaining_time[idx] > 0) {
                q.push(idx);
            }
        }

        // Print the Gantt chart line
        for (const auto& block : gantt_blocks)
            cout << block;
        cout << "|\n";

        // Print the timeline line aligned with gantt blocks
        for (size_t i = 0; i < start_times.size(); i++) {
            cout << setw(8) << start_times[i];
        }
        cout << setw(8) << current_time << "\n";
    }
};
