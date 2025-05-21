#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#define NUM_SWITCHES 1000

typedef struct {
    struct timespec start_time;
    struct timespec end_time;
    long long total_time;
} timing_info_t;

void measure_context_switch_overhead(timing_info_t *timing) {
    pid_t pid;
    int pipe_fd[2];
    char buf[1];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    clock_gettime(CLOCK_MONOTONIC, &timing->start_time);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child
        for (int i = 0; i < NUM_SWITCHES; i++) {
            if (read(pipe_fd[0], buf, 1) != 1) {
                perror("child read");
                exit(EXIT_FAILURE);
            }
            if (write(pipe_fd[1], "x", 1) != 1) {
                perror("child write");
                exit(EXIT_FAILURE);
            }
        }
        exit(0);
    } else {  // Parent
        for (int i = 0; i < NUM_SWITCHES; i++) {
            if (write(pipe_fd[1], "x", 1) != 1) {
                perror("parent write");
                exit(EXIT_FAILURE);
            }
            if (read(pipe_fd[0], buf, 1) != 1) {
                perror("parent read");
                exit(EXIT_FAILURE);
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &timing->end_time);

    timing->total_time = 
        (timing->end_time.tv_sec - timing->start_time.tv_sec) * 1000000000LL +
        (timing->end_time.tv_nsec - timing->start_time.tv_nsec);
}

int main() {
    timing_info_t timing;

    printf("Measuring context switch overhead...\n");
    measure_context_switch_overhead(&timing);

    printf("Average context switch time: %lld ns\n", 
           timing.total_time / (NUM_SWITCHES * 2));

    return 0;
}
