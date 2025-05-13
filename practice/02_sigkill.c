#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void catch_function(int signal) {
    puts("User-defined signal 1 (SIGUSR1) caught.");
}

int main(void) {
    if (signal(SIGUSR1, catch_function) == SIG_ERR) {
        fputs("An error occurred while setting a signal handler.\n", stderr);
        return EXIT_FAILURE;
    }

    printf("My PID is %d. Send SIGUSR1 to me!\n", getpid());

    // 無限ループでずっと待つ
    while (1) {
        pause();  // シグナルが来るまでスリープ
    }

    return 0;
}
