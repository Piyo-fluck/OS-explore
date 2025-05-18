#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// Process state enumeration
typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

// プロセス制御ブロック
typedef struct {
    int pid;
    ProcessState state;
    int priority;
    int remaining_time;
    int burst_time;
    int arrival_time;
    int turnaround_time; // 終了時刻 - 到着時刻
} PCB;

// Function to simulate state transition
void transition_state(PCB *process, ProcessState new_state) {
    //printf("Transitioning Process %d: %d -> %d\n", process->pid, process->state, new_state);

    process->state = new_state;
}

void srtf(PCB *processes, int num_processes) {
    int time = 0;
    int done = 0;
    int sum_burst_time = 0;

    while (done < num_processes) {
        int min_time = 10000;
        int min_index = -1;
        // Find process with the shortest remaining time
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].remaining_time > 0 && processes[i].remaining_time < min_time &&
                processes[i].arrival_time <= time) {
                min_time = processes[i].remaining_time;
                min_index = i;
            }
        }

        if (min_index != -1) {
            PCB *process = &processes[min_index];
            transition_state(process, RUNNING);
            printf("Process %d executing from time %d to %d\n", process->pid, time, time + 1);
            process->remaining_time--;
            time++;
            if (process->remaining_time ==0 ) {
                process->remaining_time = 0;
                process->turnaround_time = time - process->arrival_time;
                transition_state(process, TERMINATED);
                done++;
            }
        } else {
            // No process is ready, increment time
            time++;
        }
    }
    printf("\nTurnaround Times:\n");
    for (int i = 0; i < num_processes; i++) {
        printf("Process %d: Turnaround Time = %d\n", processes[i].pid, processes[i].turnaround_time);
        sum_burst_time += processes[i].turnaround_time;
    }
    printf("Average Turnaround Time = %.2f\n", (float)sum_burst_time / (float)num_processes);
}

void round_robin(PCB *processes, int num_processes, int time_quantum) {
    int time = 2;
    int done = 0;
    int sum_burst_time = 0;
    
    // Create a simple queue for READY processes (using an array for simplicity)
    int ready_queue[100];
    int front = 0, rear = -1, queue_size = 0;
    
    while (done < num_processes) {
        // Check for arriving processes and transition to READY
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == time && processes[i].remaining_time > 0) {
                transition_state(&processes[i], READY);
                ready_queue[++rear] = i; // Add to ready queue
                queue_size++;
            }
        }
        
        // If there are processes in the ready queue, execute the front process
        if (queue_size > 0) {
            int i = ready_queue[front]; // Get process at front of queue
            PCB *process = &processes[i];
            
            transition_state(process, RUNNING);
            int exec_time = (process->remaining_time <= time_quantum) ? process->remaining_time : time_quantum;
            printf("Process %d executing from time %d to %d\n", process->pid, time, time + exec_time);
            process->remaining_time -= exec_time;
            time += exec_time;
            
            if (process->remaining_time == 0) {
                process->turnaround_time = time - process->arrival_time;
                transition_state(process, TERMINATED);
                done++;
            } else {
                transition_state(process, READY);
                ready_queue[++rear] = i; // Requeue the process
                queue_size++;
            }
            
            // Remove the process from the front of the queue
            front++;
            queue_size--;
        } else {
            // No ready processes, increment time
            time++;
        }
    }
    
    printf("\nTurnaround Times:\n");
    for (int i = 0; i < num_processes; i++) {
        printf("Process %d: Turnaround Time = %d\n", processes[i].pid, processes[i].turnaround_time);
        sum_burst_time += processes[i].turnaround_time;
    }
    printf("Average Turnaround Time = %.2f\n", (float)sum_burst_time / (float)num_processes);
    printf("Total Time: %d\n", time);
}

// Shortest Job First (SJF)
void sjf(PCB *processes, int num_processes) {
    int time = 2;
    int done = 0;
    int sum_burst_time = 0;

    while (done < num_processes) {
        int min_time = 10000;
        int min_index = -1;
        // Find process with the shortest remaining time
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].remaining_time > 0 && processes[i].remaining_time < min_time &&
                processes[i].arrival_time <= time) {
                min_time = processes[i].remaining_time;
                min_index = i;
            }
        }

        if (min_index != -1) {
            PCB *process = &processes[min_index];
            transition_state(process, RUNNING);
            printf("Process %d executing from time %d to %d\n", process->pid, time, time + process->burst_time);
            time += process->remaining_time; // Execute until the process finishes
            process->remaining_time = 0;
            process->turnaround_time = time - process->arrival_time;
            transition_state(process, TERMINATED);
            done++;
        }
    }
    printf("\nTurnaround Times:\n");
    for (int i = 0; i < num_processes; i++) {
        printf("Process %d: Turnaround Time = %d\n", processes[i].pid, processes[i].turnaround_time);
        sum_burst_time += processes[i].turnaround_time;
    }
    printf("Average Turnaround Time = %.2f\n", (float)sum_burst_time / (float)num_processes);
    printf("Total Time: %d\n",time);
    printf("\n");
}

int main() {
    PCB processes[] = {
        {1, NEW, 1, 4, 4, 2, 0},  // pid=1, 到着時刻=0
        {2, NEW, 1, 8, 8, 4, 0},    // pid=2, 到着時刻=2
        {3, NEW, 1, 4, 4, 5, 0},     // pid=3, 到着時刻=4
        {4, NEW, 1, 3, 3, 8, 0},     // pid=4, 到着時刻=10
    };
    int n = sizeof(processes) / sizeof(processes[0]);

    // Shortest Remaining Time First
    printf("Shortest Remaining Time First\n");
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time; // Reset remaining time
        transition_state(&processes[i], READY);
    }
    srtf(processes, n);

    // Round Robin Scheduling
    printf("\nRound Robin Scheduling\n");
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time; // Reset remaining time
        transition_state(&processes[i], READY);
    }
    round_robin(processes, n, 1);  // Time quantum of 3

    // Reset for SJF
    printf("\nShortest Job First (SJF) Scheduling\n");
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time; // Reset remaining time
        transition_state(&processes[i], READY);
    }
    sjf(processes, n);

    return 0;
}
