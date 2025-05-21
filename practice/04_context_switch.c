#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE 8192

typedef struct {
    ucontext_t context;
    int id;
    char* stack;
} Process;

Process p1, p2;
ucontext_t cleanup_context;

void cleanup(void) {
    printf("All processes finished. Exiting.\n");
    exit(0);
}

void function1(void) {
    printf("Process 1 executing (start)\n");
    swapcontext(&p1.context, &p2.context);
    printf("Process 1 executing (resumed)\n");
    setcontext(&p2.context);  // ← これを追加
}


void function2(void) {
    printf("Process 2 executing (start)\n");
    swapcontext(&p2.context, &p1.context);  // 切り替え：p2 → p1
    printf("Process 2 executing (resumed)\n");
    // 終了時は cleanup_context に従って cleanup() が呼ばれる
}

int main() {
    // cleanup コンテキストの準備
    getcontext(&cleanup_context);
    cleanup_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    cleanup_context.uc_stack.ss_size = STACK_SIZE;
    cleanup_context.uc_link = NULL;
    makecontext(&cleanup_context, cleanup, 0);

    // プロセス1の準備
    getcontext(&p1.context);
    p1.stack = malloc(STACK_SIZE);
    p1.context.uc_stack.ss_sp = p1.stack;
    p1.context.uc_stack.ss_size = STACK_SIZE;
    p1.context.uc_link = &cleanup_context;
    makecontext(&p1.context, function1, 0);

    // プロセス2の準備
    getcontext(&p2.context);
    p2.stack = malloc(STACK_SIZE);
    p2.context.uc_stack.ss_sp = p2.stack;
    p2.context.uc_stack.ss_size = STACK_SIZE;
    p2.context.uc_link = &cleanup_context;
    makecontext(&p2.context, function2, 0);

    printf("Starting process 1\n");
    setcontext(&p1.context);  // 最初に p1 を実行

    return 0;
}
