#include <stdio.h>
#include <stdlib.h>

// プロセスの状態
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

// 状態遷移
void transition_state(PCB *process, ProcessState new_state) {
    printf("Transitioning Process %d: %d -> %d\n", process->pid, process->state, new_state);
    process->state = new_state;
}

// FCFSスケジューリング
void fcfs(PCB *processes, int num_processes) {
    int sum_burst_time = 0;
    int time = 0;

    // 到着時間順にソート
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = i + 1; j < num_processes; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                PCB temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    for (int i = 0; i < num_processes; i++) {
        PCB *p = &processes[i];
        if (time < p->arrival_time) {
            time = p->arrival_time; // CPUがアイドルなら時間を進める
        }

        transition_state(p, RUNNING);
        printf("Process %d executing from time %d to %d\n", p->pid, time, time + p->burst_time);
        time += p->burst_time;
        p->remaining_time = 0;
        p->turnaround_time = time - p->arrival_time;
        transition_state(p, TERMINATED);
    }

    printf("\nTurnaround Times:\n");
    for (int i = 0; i < num_processes; i++) {
        printf("Process %d: Turnaround Time = %d\n", processes[i].pid, processes[i].turnaround_time);
        sum_burst_time += processes[i].turnaround_time;
    }
    printf("Average Turnaround Time = %.2f\n", (float)sum_burst_time / (float)num_processes);
    printf("Total Time = %d\n", time);
}

int main() {
    PCB processes[] = {
        {1, NEW, 5, 4, 4, 2, 0},  // pid=1, 到着時刻=0
        {2, NEW, 3, 8, 8, 4, 0},    // pid=2, 到着時刻=2
        {3, NEW, 2, 4, 4, 5, 0},     // pid=3, 到着時刻=4
        {4, NEW, 2, 3, 3, 8, 0},     // pid=3, 到着時刻=4
    };
    int n = sizeof(processes) / sizeof(processes[0]);

    printf("FCFS Scheduling\n");
    for (int i = 0; i < n; i++) {
        transition_state(&processes[i], READY);
    }
    fcfs(processes, n);

    return 0;
}
