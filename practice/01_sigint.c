#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static void catch_function(int signal) {
    puts("Interactive attention signal caught.");
}
// catch_function は signal(SIGINT, ...) で登録されるハンドラ関数です。
// main()
// ├── signal(SIGINT, catch_function);  ← ハンドラ登録
// ├── puts("Raising SIGINT signal.");  ← 文字出力
// ├── raise(SIGINT);                   ← シグナル発行！
// │   └─→ OS が catch_function() を呼ぶ
// │       └── puts("User-defined signal 1 (SIGINT) caught.");
// ├── puts("Exiting.");                 ← ハンドラから戻ってきて実行
// └── return 0;


int main(void) {
    if (signal(SIGINT, catch_function) == SIG_ERR) {
        fputs("An error occurred while setting a signal handler.\n", stderr);
        return EXIT_FAILURE;
    }
    puts("Raising the interactive attention signal.");
    if (raise(SIGINT) != 0) {
        fputs("Error raising the signal.\n", stderr);
        return EXIT_FAILURE;
    }
    puts("Exiting.");
    return 0;
}