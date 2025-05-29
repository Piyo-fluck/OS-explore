// Process Scheduling Overhead Processes require more CPU time 
// for context switching due to memory management unit (MMU) updates and TLB flushes.
// It's a benchmark:
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_ITERATIONS 1000000

typedef struct {
    struct timespec start_time;
    struct timespec end_time;
    long long total_time;
} timing_info_t;

void measure_context_switch_overhead(timing_info_t *timing) {
    int pipe_fd[2];
    pipe(pipe_fd);
    
    char buffer[1];
    
    clock_gettime(CLOCK_MONOTONIC, &timing->start_time);
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            read(pipe_fd[0], buffer, 1);
            write(pipe_fd[1], "x", 1);
        }
        exit(0);
    } else {
        // Parent process
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            write(pipe_fd[1], "x", 1);
            read(pipe_fd[0], buffer, 1);
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &timing->end_time);
    
    timing->total_time = 
        (timing->end_time.tv_sec - timing->start_time.tv_sec) * 1000000000LL +
        (timing->end_time.tv_nsec - timing->start_time.tv_nsec);
    printf("Average context switch time: %lld nanoseconds\n",
           timing->total_time / (NUM_ITERATIONS * 2));
}

int main() {
    timing_info_t timing;
    measure_context_switch_overhead(&timing);
    return 0;
}