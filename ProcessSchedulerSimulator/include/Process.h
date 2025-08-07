#ifndef PROCESS_H
#define PROCESS_H

class Process {
public:
    int pid;
    int arrival_time;
    int burst_time;

    //RoundRobin
    int remaining_time = 0;
    // Add these for SJF:
    int start_time = -1;
    int completion_time = -1;
    int turnaround_time = -1;
    int waiting_time = -1;
    bool completed = false;

    Process(int id, int at, int bt)
        : pid(id), arrival_time(at), burst_time(bt), remaining_time(bt) {}
};

#endif
