# Process Scheduling System

## Overview
We developed a Process Scheduling System using C on Linux OS, designed to manage processes efficiently and effectively. This system incorporates several scheduling algorithms, including First-Come, First-Served (FCFS), Round Robin (RR), and Shortest Job First (SJF). 

## Features
- **UI**: The system features a user-friendly interface for interaction and monitoring.
  ![image](https://github.com/user-attachments/assets/d25f6fa7-d29c-403a-930c-f74fd5d7ba89)
- **Scheduling Algorithms**: 
  - **FCFS**: Processes are scheduled in the order they arrive in the ready queue.
  - **RR**: Processes are assigned a fixed time slice in a circular order, ensuring fair allocation of CPU time.
  - **SJF**: The process with the smallest execution time is prioritized.
- **Data Structures**: Various data structures are utilized to implement the scheduling algorithms and manage process information efficiently.
- **Inter-Process Communication**: The system employs process forking and signal-based communication for seamless interaction between processes.
- **Memory Management**: 
  - The Buddy System algorithm is implemented using tree data structures to manage memory efficiently, allowing for dynamic allocation and deallocation of memory blocks.
