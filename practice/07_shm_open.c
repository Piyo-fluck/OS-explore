#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // shm_open, O_* 定数
#include <sys/mman.h>   // mmap, PROT_*, MAP_*
#include <sys/stat.h>   // mode 定義
#include <unistd.h>     // ftruncate, fork
#include <string.h>     // memcpy
#include <sys/wait.h>   // wait

#define SHM_NAME "/my_shm_example"
#define SHM_SIZE sizeof(int)

int main() {
    // 共有メモリを作成（名前付き）
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // サイズを指定
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // mmap で共有メモリにマッピング
    int* shared_value = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_value == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 初期化
    *shared_value = 42;

    // プロセスを fork
    pid_t pid = fork();

    if (pid == 0) {
        // 子プロセス：共有メモリを書き換え
        *shared_value += 100;
        printf("Child wrote: %d\n", *shared_value);
        munmap(shared_value, SHM_SIZE);
        exit(0);
    } else {
        // 親プロセス
        wait(NULL);  // 子の終了待ち
        printf("Parent reads: %d\n", *shared_value);

        // クリーンアップ
        munmap(shared_value, SHM_SIZE);
        close(fd);
        shm_unlink(SHM_NAME);  // 忘れると /dev/shm に残る！
    }

    return 0;
}
