#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Create a new process
    pid = fork();

    if (pid < 0) {
        // Fork failed
        fprintf(stderr, "Fork failed!\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("Child process: PID = %d\n", getpid());
        printf("Child process: Parent PID = %d\n", getppid());
        // Execute a new program
        char *args[] = {"ls", "-l", NULL};
        execvp("ls", args);
        // If execvp returns, it must have failed
        perror("execvp");
        exit(1);
    } else {
        // Parent process
        printf("Parent process: PID = %d\n", getpid());
        printf("Parent process: Child PID = %d\n", pid);
        // Wait for the child process to complete
        wait(NULL);
        printf("Parent process: Child has terminated\n");
    }
    // -	ファイルの種別（-は通常ファイル、dはディレクトリ）
    // rwx	所有者（user）の権限：読み(r)、書き(w)、実行(x)
    // rwx	グループ（group）の権限
    // rwx	その他（others）の権限
    // total	総ブロック数
    // -rwxrwxrwx	アクセス権・ファイル種別
    // 1	ハードリンク数
    // user	所有ユーザー
    // user	所有グループ
    // 10240	サイズ（バイト）
    // May 8 12:00	最終更新日
    // myprogram	ファイル名
    return 0;
}