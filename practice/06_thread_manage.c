// 必要なヘッダファイルをインクルード
#include <pthread.h> // POSIXスレッド関連の関数や型を定義
#include <stdio.h>   // 標準入出力関数（printfなど）を使用
#include <stdlib.h>  // メモリ割り当て（malloc, free）やexit関数を使用

// スレッドが実行する関数
// 動的に割り当てた整数（42）を返し、メインスレッドで使用可能にする
void* return_value_thread(void* arg) {
    // 整数を格納するメモリを動的に割り当て
    // sizeof(int)で適切なサイズを確保
    int* result = malloc(sizeof(int));
    
    // 割り当てに失敗した場合（メモリ不足など）をチェック
    if (result == NULL) {
        fprintf(stderr, "メモリ割り当てに失敗しました\n");
        exit(1);
    }
    
    // 割り当てたメモリに値42を格納
    *result = 42;
    
    // メモリのポインタをスレッドの戻り値として返す
    // void*型にキャストして汎用性を確保
    return (void*)result;
}

// プログラムのメイン関数
int main() {
    // スレッド識別子を格納する変数
    pthread_t thread;
    // スレッドの戻り値を受け取るポインタ
    void* result;

    // 新しいスレッドを作成
    // thread: スレッド識別子
    // NULL: デフォルトのスレッド属性を使用
    // return_value_thread: スレッドが実行する関数
    // NULL: スレッド関数に渡す引数（この例では不要）
    if (pthread_create(&thread, NULL, return_value_thread, NULL) != 0) {
        // スレッド作成に失敗した場合、エラーメッセージを表示
        perror("スレッド作成に失敗しました");
        // プログラムを異常終了（終了コード1）
        exit(1);
    }
    
    // スレッドの終了を待機し、戻り値を取得
    // pthread_joinはスレッドが終了するまでメインスレッドをブロック
    // resultにスレッドの戻り値（void*）を格納
    if (pthread_join(thread, &result) != 0) {
        // スレッド待機に失敗した場合、エラーメッセージを表示
        perror("スレッド待機に失敗しました");
        exit(1);
    }
    
    // スレッドの戻り値をint型にキャストして表示
    // resultはスレッド内で割り当てられたメモリを指す
    printf("スレッドが返した値: %d\n", *(int*)result);
    
    // 動的に割り当てられたメモリを解放
    // メモリリークを防ぐために必須
    free(result);
    
    // プログラム正常終了（終了コード0）
    return 0;
}

// sequenceDiagram
//     participant Main
//     participant Thread
//     Main->>Thread: Create thread (pthread_create)
//     Thread->>Thread: Allocate memory (malloc)
//     Note over Thread: Store 42 in memory
//     Thread-->>Main: Return pointer (via pthread_join)
//     Main->>Main: Print value (42)
//     Main->>Main: Free memory