#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#define N 1000
// Timing structure
typedef struct {
    struct timeval start;
    struct timeval end;
} timing_t;

// Function to measure process creation time
long measure_process_creation() {
    timing_t timing;
    gettimeofday(&timing.start, NULL);
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit(0);
    } else {
        // Parent process
        wait(NULL);
    }
    
    gettimeofday(&timing.end, NULL);
    
    long microseconds = (timing.end.tv_sec - timing.start.tv_sec) * 1000000 +
                       (timing.end.tv_usec - timing.start.tv_usec);
    return microseconds;
}

// Function for thread creation measurement
void* thread_function(void* arg) {
    return NULL;
}

long measure_thread_creation() {
    timing_t timing;
    pthread_t thread;
    
    gettimeofday(&timing.start, NULL);
    
    if (pthread_create(&thread, NULL, thread_function, NULL) != 0) {
    perror("pthread_create failed");
    exit(1);
}
    pthread_join(thread, NULL);
    
    gettimeofday(&timing.end, NULL);
    
    long microseconds = (timing.end.tv_sec - timing.start.tv_sec) * 1000000 +
                       (timing.end.tv_usec - timing.start.tv_usec);
    return microseconds;
}

int main() {
    long total_time = 0;
    for (int i = 0; i < N; i++) {
    total_time += measure_process_creation();
    }
    printf("Average Process Creation Time: %ld microseconds\n", total_time/N);

    total_time = 0;
    for (int i = 0; i < N; i++) {
    total_time += measure_thread_creation();
    }
    printf("Average Thread Creation Time: %ld microseconds\n", total_time/N);

    return 0;
}