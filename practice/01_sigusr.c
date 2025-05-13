#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static void catch_function(int signal) {
    puts("User-defined signal 1 (SIGUSR1) caught.");
}

// SIGUSR1 や SIGUSR2 はアプリケーション側が自由に使ってよいシグナル。
// 1. signal(SIGUSR1, catch_function);
//     OS に「このプロセスが SIGUSR1 を受け取ったら、catch_function() を呼び出して処理してね」と登録します。
//     登録が成功すれば、その後このシグナルが発生したときに C 言語の関数が実行されるようになります。
// 2. raise(SIGUSR1);
//     自分自身（現在のプロセス）に対して SIGUSR1 を送信します。
//     実質的には、kill(getpid(), SIGUSR1) と同じです。
// 3. OS がシグナルを検知し、ハンドラを実行
//     OS はプロセスが自分自身に SIGUSR1 を送ったことを検出。
//     すでに登録していた catch_function(int) が呼ばれます。
//     その中の puts() により "User-defined signal 1 (SIGUSR1) caught." が表示されます。
// 4. ハンドラの処理が終わった後、元の main() に戻る
//     raise() の後ろの puts("Exiting.") が実行されて "Exiting." と表示されます。

int main(void) {
    if (signal(SIGUSR1, catch_function) == SIG_ERR) {
        fputs("An error occurred while setting a signal handler.\n", stderr);
        return EXIT_FAILURE;
    }

    puts("Raising SIGUSR1 signal.");
    if (raise(SIGUSR1) != 0) {
        fputs("Error raising the signal.\n", stderr);
        return EXIT_FAILURE;
    }

    puts("Exiting.");
    return 0;
}
