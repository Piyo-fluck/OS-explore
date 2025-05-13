#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int sig) {
    printf("Caught signal %d\n", sig);
    //exit(0);  // シグナルを受けたら終了する
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);

    while (1) {
        pause();
    }
}

// void handler(int sig) {...}
//     シグナルが届いたときに実行される関数（シグナルハンドラ）。
//     引数 sig はシグナル番号（たとえば SIGINT なら 2）で、ここではそれを表示しています。
// struct sigaction sa;
//     sigaction() に渡すための構造体。シグナルの挙動を設定するためのものです。
// sa.sa_handler = handler;
//     この行で「SIGINT が来たら handler() を呼んで」と設定しています。
// sigemptyset(&sa.sa_mask);
// sa_mask にシグナル集合（sigset_t）を設定することで、ハンドラ実行中に一時的にブロックするシグナルを指定できます。これにより、競合のあるシグナルを一時的に抑制できます。
//     これは「ハンドラが動いている間にブロックしたいシグナルは特にない」という意味です。
//     ちょっと難しく聞こえるかもしれませんが、「このハンドラが動作している間に、他のシグナルも届く可能性がある」という設定です。
// sa.sa_flags = 0;
//     特別な動作指定はしないという意味です。
// sigaction(SIGINT, &sa, NULL);
//     SIGINT（たとえば Ctrl+C）を受け取ったときに、上で設定したハンドラを使うようカーネルに伝えます。
// while (1) { pause(); }
//     無限ループで pause() を使って、シグナルが来るまで待機します。
//     pause() は「シグナルを受け取るまで何もしないで寝る」関数です。
//  システムコールの「再開」とは？
// たとえば次のような状況を想像してみてください：
//     プログラムが read()（ファイルを読む）などの システムコール を実行中
//     その最中に シグナルが届く
//     そのせいで read() が 途中で中断されてしまう ことがあります
// このとき、普通は read() は失敗（エラー）として返されますが、SA_RESTART を sigaction に指定すると：
//      中断されたシステムコールが、自動的に最初からもう一度実行される
// というふうに、プログラムが意識しなくてもうまく処理が継続されるという機能です。