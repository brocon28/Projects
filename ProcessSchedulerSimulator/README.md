# ProcessSchedulerSimulator

## Overview

**ProcessSchedulerSimulator** is a C++ project that simulates CPU scheduling algorithms commonly studied in Operating Systems courses. It provides implementations of multiple scheduling strategies, allowing users to observe how processes are scheduled, how wait and turnaround times are calculated, and how the CPU executes tasks over time.

This project is ideal for students and developers interested in understanding process scheduling concepts and their practical applications.

---

## Implemented Scheduling Algorithms

- **First-Come, First-Served (FCFS)**  
  The simplest scheduling algorithm where the process that arrives first gets executed first.

- **Shortest Job First (SJF) - Non-Preemptive**  
  Selects the process with the shortest burst time from the ready queue and runs it to completion before moving on.

- **Round Robin (RR)**  
  Each process gets a fixed time quantum to execute in a cyclic order, providing a fair share of CPU time.

---

## Features

- Supports dynamic process input with PID, arrival time, and burst time.
- Calculates and displays key metrics:
  - Start Time
  - Completion Time
  - Turnaround Time
  - Waiting Time
- Generates a Gantt chart visualization of process execution timeline.
- Modular design allowing easy addition of more scheduling algorithms.
- Detailed console output for step-by-step execution tracing.

---

## How to Build

Ensure you have a C++17 compatible compiler installed (e.g., `g++`).

From the root project directory:

```bash
make
