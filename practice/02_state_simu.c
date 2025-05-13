#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_PROCESSES 3

typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

const char* state_names[] = {
    "NEW", "READY", "RUNNING", "WAITING", "TERMINATED"
};

typedef struct {
    int pid;
    ProcessState state;
    int priority;
} PCB;

void transition_state(PCB* process, ProcessState new_state) {
    printf("Process %d: %s -> %s\n", 
           process->pid, state_names[process->state], state_names[new_state]);
    process->state = new_state;
    sleep(1);  // Simulate delay
}

void simulate_process(PCB* process) {
    transition_state(process, READY);
    transition_state(process, RUNNING);

    // ランダムに WAITING または TERMINATED に移行
    if (rand() % 2 == 0) {
        transition_state(process, WAITING);
        transition_state(process, READY);
        transition_state(process, RUNNING);
    }

    transition_state(process, TERMINATED);
}

int main() {
    srand(time(NULL)); // 乱数の初期化

    PCB processes[NUM_PROCESSES];

    // 初期化とシミュレーション
    for (int i = 0; i < NUM_PROCESSES; i++) {
        processes[i].pid = i + 1;
        processes[i].state = NEW;
        processes[i].priority = rand() % 10;
        printf("\n--- Simulating Process %d (priority %d) ---\n", 
               processes[i].pid, processes[i].priority);
        simulate_process(&processes[i]);
    }

    return 0;
}
