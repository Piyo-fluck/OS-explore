#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ARRAY_SIZE 10000000
#define NUM_THREADS 4

double* shared_array;

typedef struct {
    struct timespec start_time;
    struct timespec end_time;
    long long total_time;
} timing_info_t;

void* thread_function(void* arg) {
    int thread_id = *(int*)arg;
    int chunk_size = ARRAY_SIZE / NUM_THREADS;
    int start = thread_id * chunk_size;
    int end = start + chunk_size;

    for (int i = start; i < end; i++) {
        shared_array[i] = shared_array[i] * 2.0;
    }

    return NULL;
}

int main() {
    timing_info_t timing;
    shared_array = malloc(ARRAY_SIZE * sizeof(double));
    if (shared_array == NULL) {
        perror("malloc failed");
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Initialize array
    for (int i = 0; i < ARRAY_SIZE; i++) {
        shared_array[i] = (double)i;
    }

    clock_gettime(CLOCK_MONOTONIC, &timing.start_time);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]) != 0) {
            perror("pthread_create failed");
            free(shared_array);
            return 1;
        }
    }

    // Wait for threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &timing.end_time);

    timing.total_time = 
        (timing.end_time.tv_sec - timing.start_time.tv_sec) * 1000000000LL +
        (timing.end_time.tv_nsec - timing.start_time.tv_nsec);

    printf("Thread processing time: %lld nanoseconds\n", timing.total_time);

    free(shared_array);
    return 0;
}
