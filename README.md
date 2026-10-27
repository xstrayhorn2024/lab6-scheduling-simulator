# Scheduling Simulator

CPU scheduling simulator in C that computes waiting and turnaround times for
FCFS, preemptive SJF (SRTF), Priority, and Round Robin, with support for
process arrival times and a configurable quantum.

## Build and run

    gcc -Wall -o schedsim schedsim.c util.c
    ./schedsim <input-file> [quantum]

The quantum defaults to 2. Each input line is: `pid burst arrival wt tat priority`.
Malformed lines are skipped with a warning.
