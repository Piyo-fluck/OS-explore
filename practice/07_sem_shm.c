#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

#define SHM_NAME "/my_shm"
#define SEM_NAME "/my_sem"

int main() {
    // 共有メモリの準備
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666); //shm_open() はファイルディスクリプタを返す
    if (shm_fd == -1) { perror("shm_open"); exit(1); }
    // サイズを指定
    ftruncate(shm_fd, sizeof(int));

    // mmap() に渡して共有メモリとして使うだけ
    int* shared_value = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_value == MAP_FAILED) { perror("mmap"); exit(1); }
    *shared_value = 0;

    // セマフォの準備（初期値 = 1）
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) { perror("sem_open"); exit(1); }

    pid_t pid = fork();

    if (pid == 0) {
        // 子プロセス
        sem_wait(sem); // ロック
        int val = *shared_value;
        printf("Child read: %d\n", val);
        val += 100;
        *shared_value = val;
        printf("Child wrote: %d\n", val);
        sem_post(sem); // アンロック
        munmap(shared_value, sizeof(int));
        exit(0);
    } else {
        // 親プロセス
        sem_wait(sem); // ロック
        *shared_value = 1;
        printf("Parent wrote: %d\n", *shared_value);
        sem_post(sem); // アンロック

        wait(NULL); // 子の終了待ち

        sem_wait(sem);
        printf("Parent reads final: %d\n", *shared_value);
        sem_post(sem);

        // 後始末
        munmap(shared_value, sizeof(int));
        close(shm_fd);
        sleep(10); //この間にls -l /dev/shm/my_shm を実行する
        shm_unlink(SHM_NAME);
        sem_close(sem);
        sem_unlink(SEM_NAME);
    }

    return 0;
}
