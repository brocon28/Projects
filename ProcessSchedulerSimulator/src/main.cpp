#include "../include/Process.h"
#include "FCFS.cpp"
#include "SJF.cpp"
#include "RoundRobin.cpp"

#include <vector>
#include <iostream>

using namespace std;

void runFCFS() {
  cout << "===== FCFS Scheduling =====" << endl;

  vector<Process*> processes = {
    new Process{1, 0, 4},
    new Process{2, 1, 3},
    new Process{3, 2, 1},
    new Process{4, 3, 2},
    new Process{5, 4, 5}
  };

  FCFS scheduler(processes);
  scheduler.execute();

  for (auto p : processes) {
    delete p;
  }

  cout << endl;
}

void runNonPreemptiveSJF() {
  cout << "===== SJF (Non-Preemptive) Scheduling =====" << endl;

  vector<Process*> processes = {
    new Process{1, 0, 8},
    new Process{2, 1, 4},
    new Process{3, 2, 9},
    new Process{4, 3, 5}
  };

  NonPreemptiveSJF scheduler(processes);
  scheduler.execute();

  for (auto p : processes) {
    delete p;
  }

  cout << endl;
}

void runRoundRobin() {
  cout << "===== Round Robin Scheduling =====" << endl;

  vector<Process*> processes = {
    new Process{1, 0, 5},
    new Process{2, 1, 4},
    new Process{3, 2, 2},
    new Process{4, 3, 1}
  };

  int time_quantum = 2;
  RoundRobin rr(processes, time_quantum);
  rr.execute();

  for (auto p : processes) {
    delete p;
  }

  cout << endl;
}


int main() {
  runFCFS();
  runNonPreemptiveSJF();
  runRoundRobin();
  return 0;
}
