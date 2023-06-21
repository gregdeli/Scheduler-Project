# Description
This is a C program that implements various scheduling algorithms for process management. It reads input from a file and simulates process execution based on the selected scheduling policy.
# Features 
- First-Come, First-Served (FCFS) scheduling algorithm
- Shortest Job First (SJF) scheduling algorithm
- Round Robin (RR) scheduling algorithm
- Priority (PRIO) scheduling algorithm
# Prerequisites
Before running the program, ensure that you have the following prerequisites:

- C compiler (e.g., GCC)
- Linux operating system
# Usage
To compile the program, use the following command:
```
gcc scheduler.c -o scheduler
```
To run the program, use the following command:
```
./scheduler <algorithm> [quantum] <input_file>
```
Alternatively you can use the run.sh file:
```
./run.sh
```
- algorithm : Specify the scheduling algorithm to use (BATCH, SJF, RR, PRIO).
- [quantum]: Required only for RR and PRIO algorithms. Specify the quantum time in milliseconds.
- input_file: Specify the input file containing the list of processes to schedule.
# Examples 
Run RR algorithm with a quantum of 100 milliseconds:
```
./scheduler RR 100 input.txt
 ```
Run SJF algorithm:
```
./scheduler SJF reverse.txt
 ```
