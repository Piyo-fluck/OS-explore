// 必要なヘッダファイルをインクルード
#include <pthread.h> // POSIXスレッド関連の関数や型を定義
#include <stdio.h>   // 標準入出力関数（printfなど）を使用
#include <stdlib.h>  // exit関数などを使用
#include <unistd.h>  // sleep関数を使用（スレッドの一時停止）

// スレッドに渡すパラメータを格納する構造体
typedef struct {
    int thread_id;      // スレッドの識別番号
    char* message;      // スレッドが表示するメッセージ
    int sleep_time;     // スレッドがスリープする秒数
} thread_params_t;

// スレッドが実行する関数
// 引数としてvoidポインタを受け取り、構造体を介してパラメータを取得
void* parameterized_thread(void* arg) {
    // 引数をthread_params_t型にキャストしてパラメータを取得
    thread_params_t* params = (thread_params_t*)arg;
    
    // スレッド開始時にIDとメッセージを表示
    printf("スレッド %d 開始、メッセージ: %s\n", 
           params->thread_id, params->message);
    
    // 指定された秒数だけスレッドを一時停止
    // sleepはスレッドの実行を指定秒数ブロック
    sleep(params->sleep_time);
    
    // スレッド終了時にIDを表示
    printf("スレッド %d 終了\n", params->thread_id);
    
    // スレッドを明示的に終了（NULLを返して終了）
    pthread_exit(NULL);
}

// プログラムのメイン関数
int main() {
    // 3つのスレッドを格納する配列
    pthread_t threads[3];
    // 各スレッドに渡すパラメータの配列（構造体で初期化）
    thread_params_t params[3] = {
        {1, "First Thread", 2},  // スレッド1: ID=1, メッセージ="First Thread", スリープ2秒
        {2, "Second Thread", 3}, // スレッド2: ID=2, メッセージ="Second Thread", スリープ3秒
        {3, "Third Thread", 1}   // スレッド3: ID=3, メッセージ="Third Thread", スリープ1秒
    };

    // スレッドを作成（3つ）
    for (int i = 0; i < 3; i++) {
        // pthread_createでスレッドを生成
        // threads[i]: スレッド識別子
        // NULL: デフォルトのスレッド属性を使用
        // parameterized_thread: スレッドが実行する関数
        // &params[i]: スレッドに渡すパラメータ（構造体のアドレス）
        if (pthread_create(&threads[i], NULL, parameterized_thread, &params[i]) != 0) {
            // スレッド作成に失敗した場合、エラーメッセージを表示
            perror("スレッド作成に失敗しました");
            // プログラムを異常終了（終了コード1）
            exit(1);
        }
    }

    // すべてのスレッドの終了を待機
    for (int i = 0; i < 3; i++) {
        // pthread_joinで指定スレッドの終了を待つ
        // NULL: スレッドの戻り値を受け取らない
        pthread_join(threads[i], NULL);
    }

    // プログラム正常終了（終了コード0）
    return 0;
}