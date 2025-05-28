#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread_function(void* arg) {
    int thread_id = *(int*)arg;
    printf("Thread %d is running\n", thread_id);
    free(arg);
    return NULL;
}

int main() {
    pthread_t threads[5];

    for (int i = 0; i < 5; i++) {
        int* arg = malloc(sizeof(int));
        *arg = i + 1;
        if (pthread_create(&threads[i], NULL, thread_function, arg) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads have finished execution\n");
    return 0;
}