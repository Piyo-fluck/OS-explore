#include <stdio.h>
#include <stdlib.h>

#define SIZE (100 * 1024 * 1024)
#define REPEAT 100  // 繰り返し回数を増やして負荷をかける

int main() {
    char *array = malloc(SIZE);
    if (!array) {
        perror("malloc failed");
        return 1;
    }

    // 初期化（キャッシュやTLBのヒットに偏らないようにする）
    for (int i = 0; i < SIZE; i++) {
        array[i] = i % 256;
    }

    volatile long long sum = 0;

    for (int r = 0; r < REPEAT; r++) {
        for (int i = 0; i < SIZE; i += 4096) {
            sum += array[i];
        }
    }

    printf("Sum: %lld\n", sum);
    free(array);
    return 0;
}
