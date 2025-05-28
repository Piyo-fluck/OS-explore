// cache_test.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE (100 * 1024 * 1024) // 100MB

int main() {
    long long sum = 0;
    char *array = malloc(SIZE);
    if (!array) {
        perror("malloc failed");
    return 1;
    }
    memset(array, 0, SIZE);
    

    for (int i = 0; i < SIZE; i += 64) { // 64バイト単位にするとL1キャッシュっぽい
        sum += array[i];
    }

    printf("Sum: %lld\n", sum);
    free(array);
    return 0;
}

// gcc cache_test.c -o cache_test
// gcc cache_miss_test.c -o cache_miss_test
// perf stat -e cache-misses,cache-references,dTLB-loads,dTLB-load-misses ./cache_test
// # キャッシュ効く場合
// taskset -c 0 perf stat -e cache-references,cache-misses,dTLB-loads,dTLB-load-misses ./cache_test
// perf stat -e cache-misses,cache-references,dTLB-loads,dTLB-load-misses ./cache_miss_test
// # キャッシュミス多発の場合
// taskset -c 0 perf stat -e cache-references,cache-misses,dTLB-loads,dTLB-load-misses ./cache_miss_test
// cd "C:\Users\あなた\Documents\perf_test\"
// gcc -g -O2 -o cache_test.exe cache_test.c
