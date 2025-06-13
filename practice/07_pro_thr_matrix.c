#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define MATRIX_SIZE 2000
#define NUM_WORKERS 4

typedef struct {
    double* a;
    double* b;
    double* c;
    int start_row;
    int end_row;
} matrix_work_t;

void* thread_matrix_multiply(void* arg) {
    matrix_work_t* work = (matrix_work_t*)arg;
    
    for (int i = work->start_row; i < work->end_row; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            double sum = 0.0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += work->a[i * MATRIX_SIZE + k] * 
                       work->b[k * MATRIX_SIZE + j];
            }
            work->c[i * MATRIX_SIZE + j] = sum;
        }
    }
    
    return NULL;
}

void process_matrix_multiply(double* a, double* b, double* c,
                           int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            double sum = 0.0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += a[i * MATRIX_SIZE + k] * 
                       b[k * MATRIX_SIZE + j];
            }
            c[i * MATRIX_SIZE + j] = sum;
        }
    }
}

void compare_performance() {
    // Allocate matrices
    size_t matrix_bytes = MATRIX_SIZE * MATRIX_SIZE * sizeof(double);
    
    double* a = mmap(NULL, matrix_bytes,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    double* b = mmap(NULL, matrix_bytes,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    double* c = mmap(NULL, matrix_bytes,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (a == MAP_FAILED || b == MAP_FAILED || c == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // Initialize matrices
    srand((unsigned int)time(NULL));
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
        c[i] = 0.0; // Initialize result matrix
    }
    
    struct timespec start, end;
    
    // Thread implementation
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pthread_t threads[NUM_WORKERS];
    matrix_work_t thread_work[NUM_WORKERS];
    int rows_per_worker = MATRIX_SIZE / NUM_WORKERS;
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        thread_work[i].a = a;
        thread_work[i].b = b;
        thread_work[i].c = c;
        thread_work[i].start_row = i * rows_per_worker;
        thread_work[i].end_row = (i == NUM_WORKERS - 1) ? 
                                MATRIX_SIZE : (i + 1) * rows_per_worker;
        
        if (pthread_create(&threads[i], NULL,
                         thread_matrix_multiply, &thread_work[i]) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            exit(1);
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long thread_time = (end.tv_sec - start.tv_sec) * 1000000000L +
                      (end.tv_nsec - start.tv_nsec);
    
    // Clear result matrix for process implementation
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        c[i] = 0.0;
    }

    // Process implementation
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }
        if (pid == 0) {
            int start_row = i * rows_per_worker;
            int end_row = (i == NUM_WORKERS - 1) ?
                         MATRIX_SIZE : (i + 1) * rows_per_worker;
            
            process_matrix_multiply(a, b, c, start_row, end_row);
            exit(0);
        }
    }
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        int status;
        if (wait(&status) == -1) {
            perror("wait failed");
            exit(1);
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long process_time = (end.tv_sec - start.tv_sec) * 1000000000L +
                       (end.tv_nsec - start.tv_nsec);
    
    printf("Thread implementation time: %ld nanoseconds (%.3f ms)\n", 
           thread_time, thread_time / 1000000.0);
    printf("Process implementation time: %ld nanoseconds (%.3f ms)\n", 
           process_time, process_time / 1000000.0);
    
    // Clean up
    if (munmap(a, matrix_bytes) == -1 || 
        munmap(b, matrix_bytes) == -1 || 
        munmap(c, matrix_bytes) == -1) {
        perror("munmap failed");
    }
}

int main() {
    compare_performance();
    return 0;
}