# Process Scheduling System

## Overview
We developed a Process Scheduling System using C on Linux OS, designed to manage processes efficiently and effectively. This system incorporates several scheduling algorithms, including First-Come, First-Served (FCFS), Round Robin (RR), and Shortest Job First (SJF). 

## Features
- **UI**: The system features a user-friendly interface for interaction where you provide your test cases through text file and write it's path.
![image](https://github.com/user-attachments/assets/a0c04841-5d17-4ba8-82b4-5d0d057f0447)
**sample of result**:<br>
![image](https://github.com/user-attachments/assets/ca4ea2c5-e5fb-4586-978b-b7a866ff04f8)
- **Scheduling Algorithms**: 
  - **FCFS**: Processes are scheduled in the order they arrive in the ready queue.
  - **RR**: Processes are assigned a fixed time slice in a circular order, ensuring fair allocation of CPU time.
  - **SJF**: The process with the smallest execution time is prioritized.
- **Data Structures**: Various data structures are utilized to implement the scheduling algorithms and manage process information efficiently.
- **Inter-Process Communication**: The system employs process forking and signal-based communication for seamless interaction between processes.
- **Memory Management**: 
  - The Buddy System algorithm is implemented using tree data structures to manage memory efficiently, allowing for dynamic allocation and deallocation of memory blocks.
