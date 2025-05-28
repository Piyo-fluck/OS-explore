// 必要なヘッダファイルをインクルード
#include <pthread.h> // POSIXスレッド関連の関数や型を定義
#include <stdio.h>   // 標準入出力関数（printfなど）を使用
#include <stdlib.h>  // exit関数などを使用

// スレッドが実行する関数
// 引数としてvoidポインタを受け取り、スレッドの処理を定義
void* thread_function(void* arg) {
    // 引数をint型にキャストして値を取得し、表示
    // ポインタ経由で渡された値を参照
    printf("スレッドが実行中、引数の値: %d\n", *(int*)arg);
    // スレッドの終了時にNULLを返す（戻り値は任意）
    return NULL;
}

// プログラムのメイン関数
int main() {
    // スレッド識別子を格納する変数
    pthread_t thread;
    // スレッド属性を格納する構造体
    pthread_attr_t attr;
    // スレッドに渡す引数（整数値42）
    int arg = 42;

    // スレッド属性を初期化
    // pthread_attr_t構造体をデフォルト値で設定
    pthread_attr_init(&attr);

    // スレッドのスタックサイズを1MB（1024 * 1024バイト）に設定
    // デフォルトのスタックサイズを変更してメモリ使用量を制御
    pthread_attr_setstacksize(&attr, 1024 * 1024);

    // スレッドのデタッチ状態を設定（ジョイン可能）
    // PTHREAD_CREATE_JOINABLE: スレッド終了後にpthread_joinで待機可能
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // 新しいスレッドを作成
    // thread: スレッド識別子
    // attr: スレッド属性（スタックサイズやデタッチ状態）
    // thread_function: スレッドが実行する関数
    // &arg: スレッド関数に渡す引数（int型のポインタ）
    if (pthread_create(&thread, &attr, thread_function, &arg) != 0) {
        // スレッド作成に失敗した場合、エラーメッセージを表示
        perror("スレッド作成に失敗しました");
        // プログラムを異常終了（終了コード1）
        exit(1);
    }

    // スレッド属性を破棄
    // 初期化した属性リソースを解放（メモリリーク防止）
    pthread_attr_destroy(&attr);

    // スレッドの終了を待機
    // pthread_joinは指定したスレッドが終了するまでメインスレッドをブロック
    // 第2引数NULL: スレッドの戻り値を受け取らない
    pthread_join(thread, NULL);

    // プログラム正常終了（終了コード0）
    return 0;
}