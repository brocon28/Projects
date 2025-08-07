#include "../include/Process.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

class NonPreemptiveSJF {
public:
    NonPreemptiveSJF(std::vector<Process*>& processes) : processes(processes) {}

    void execute() {
        int current_time = 0;
        int completed = 0;
        int n = processes.size();

        std::vector<Process*> ready_queue;
        std::vector<Process*> executed_order;

        while (completed < n) {
            // Add all processes that have arrived by current_time and are not completed
            for (auto& p : processes) {
                if (p->arrival_time <= current_time && !p->completed &&
                    std::find(ready_queue.begin(), ready_queue.end(), p) == ready_queue.end()) {
                    ready_queue.push_back(p);
                }
            }

            // Sort ready_queue by burst time
            std::sort(ready_queue.begin(), ready_queue.end(), [](Process* a, Process* b) {
                return a->burst_time < b->burst_time;
            });

            if (!ready_queue.empty()) {
                Process* current = ready_queue.front();
                ready_queue.erase(ready_queue.begin());

                current->start_time = current_time;
                current->completion_time = current_time + current->burst_time;
                current->turnaround_time = current->completion_time - current->arrival_time;
                current->waiting_time = current->turnaround_time - current->burst_time;
                current->completed = true;

                executed_order.push_back(current);
                current_time = current->completion_time;
                completed++;
            } else {
                current_time++;
            }
        }

        // Output results
        std::cout << "\nSJF (Non-Preemptive) Scheduling:\n\n";
        std::cout << "PID\tArrival\tBurst\tStart\tComplete\tTurnaround\tWaiting\n";
        for (auto& p : executed_order) {
            std::cout << p->pid << "\t"
                      << p->arrival_time << "\t"
                      << p->burst_time << "\t"
                      << p->start_time << "\t"
                      << p->completion_time << "\t\t"
                      << p->turnaround_time << "\t\t"
                      << p->waiting_time << "\n";
        }

        // Gantt Chart
        std::cout << "\nGantt Chart:\n|";
        for (auto& p : executed_order) {
            std::cout << " P" << p->pid << " |";
        }
        std::cout << "\n" << executed_order.front()->start_time;
        for (auto& p : executed_order) {
            std::cout << std::setw(5) << p->completion_time;
        }
        std::cout << "\n";
    }

private:
    std::vector<Process*>& processes;
};
