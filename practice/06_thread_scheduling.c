#include <pthread.h>      // pthreadライブラリのインクルード
#include <stdio.h>        // 標準入出力
#include <sched.h>        // スケジューリング関連の定義

// スレッドで実行される関数
void* priority_thread(void* arg) {
    int policy;                   // スケジューリングポリシー格納用
    struct sched_param param;     // スケジューリングパラメータ格納用

    // 現在のスレッドのスケジューリングポリシーとパラメータを取得
    // スレッド内で pthread_getschedparam を使って、実際の優先度を確認
    pthread_getschedparam(pthread_self(), &policy, &param);

    // スレッドの優先度を表示
    printf("Thread priority: %d\n", param.sched_priority);

    return NULL; // 戻り値なし
}

int main() {
    pthread_t thread;             // スレッドID格納用
    pthread_attr_t attr;          // スレッド属性格納用
    struct sched_param param;     // スケジューリングパラメータ格納用

    pthread_attr_init(&attr);     // pthread_attr_t を使ってスレッド属性を初期化

    // スケジューリングポリシーをSCHED_FIFO（リアルタイムFIFO）に設定
    // 優先度が高いスレッドが実行され続け、他のスレッドに譲らない
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    param.sched_priority = 50;    // 優先度を50に設定（範囲はシステム依存）
    pthread_attr_setschedparam(&attr, &param); // 属性に優先度を設定

    // 属性付きでスレッドを生成
    pthread_create(&thread, &attr, priority_thread, NULL);

    // スレッドの終了を待つ
    pthread_join(thread, NULL);

    pthread_attr_destroy(&attr);  // スレッド属性を破棄（リソース解放）

    return 0; // 正常終了
}