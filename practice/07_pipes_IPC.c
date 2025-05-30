#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void demonstrate_pipe_communication() {
    int pipe_fd[2];
    char buffer[BUFFER_SIZE];
    
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }
    
    pid_t pid = fork();
    if (pid<0){
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        // Child process
        close(pipe_fd[1]);  // Close write end
        
        ssize_t bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
        perror("read");
        exit(1);
        }
        buffer[bytes_read] = '\0';  // Ensure null termination
        printf("Child received: %s\n", buffer);
        
        close(pipe_fd[0]);
        exit(0);
    } else {
        // Parent process
        close(pipe_fd[0]);  // Close read end
        
        const char* message = "Hello from parent!";
        if (write(pipe_fd[1], message, strlen(message) + 1) == -1) {
        perror("write");
        exit(1);
        }
        
        close(pipe_fd[1]);
        wait(NULL);
    }
}

int main() {
    demonstrate_pipe_communication();
    return 0;
}

// gcc -Wall 07_pipes_IPC.c -o tmp.o
// strace -o trace.log ./tmp.o  
// ↑プログラムのシステムコールをトレースし、結果を trace.log に保存
// strace -f -e trace=pipe,read,write,fork,clone,close,wait4 -o trace.log ./tmp.o
// オプション -f（子プロセスもトレース
// -e trace=pipe,read,write,fork（特定システムコールのみをトレース）

// 566   close(3)                          = 0
// 566   read(3, "\177ELF\2\1\0"..., 832) = 832　ELF ファイルヘッダの読み込み
// 566   close(3)                          = 0  ファイルディスクリプタの解放
// 566   clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f400cf28a10) = 567
// 子プロセス（PID 567）が作成された

// 566   close(3)                          = 0  パイプの読込端（pipe_fd[0]）を閉じる
// 566   write(4, "Hello from parent!\0", 19 <unfinished ...>   パイプの書込端（pipe_fd[1] = 4）に "Hello from parent!"（19バイト、ヌル終端含む）を書き込む
// 567   close(4 <unfinished ...>   // 親子プロセスの並行実行によるもの
// 566   <... write resumed>)              = 19     // 親プロセスの書き込みに関するシステムコールが再開、バイトを書き込む
// 567   <... close resumed>)              = 0      // 子プロセスのファイルディスクリプタ解放に関するシステムコールが再開
// 566   close(4 <unfinished ...>　// close(pipe_fd[1])（親プロセスがパイプの書込端を閉じる
// 567   read(3,  <unfinished ...>  子プロセスがパイプの読込端（FD 3）からデータを読む途中で中断
// 566   <... close resumed>)              = 0   // 子のread命令を待った後、closeが完了
// 567   <... read resumed>"Hello from parent!\0", 1024) = 19　親の write 完了を待つなどして制御が切り替わり、後に read が再開。19バイト（"Hello from parent!"）を読み込み成功（= 19）
// 566   wait4(-1,  <unfinished ...>    子プロセスの終了を待つ。子プロセス（PID 507）が終了し、戻り値 507 を返す。
// 567   write(1, "Child received: Hello from paren"..., 35) = 35   標準出力（fd=1）に "Child received: Hello from parent!\n"（35バイト）を書き込む。これがターミナルに表示される。
// 567   close(3)                          = 0　読み込み端を閉じる
// 567   +++ exited with 0 +++
// 566   <... wait4 resumed>NULL, 0, NULL) = 567
// 566   --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=567, si_uid=1000, si_status=0, si_utime=0, si_stime=1 /* 0.01 s */} ---
// 566   +++ exited with 0 +++

// (gdb) break demonstrate_pipe_communication
// (gdb) set follow-fork-mode child
// (gdb) run
// # ブレークポイントで停止
// (gdb) next  # pipe(pipe_fd)
// (gdb) next  # fork()
// # 子プロセスに切り替わる
// (gdb) break read  # read() で停止
// (gdb) continue
// # read() で停止
// (gdb) print buffer  # 読込前のバッファ（未定義）
// (gdb) continue
// # read() 後
// (gdb) print buffer  # 例: $1 = "Hello from parent!\0"
// (gdb) continue
// (gdb) quit