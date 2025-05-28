// 必要なヘッダファイルをインクルード
#include <pthread.h> // POSIXスレッド関連の関数や型（pthread_key_t, pthread_setspecificなど）を定義
#include <stdio.h>   // 標準入出力関数（printfなど）を使用
#include <stdlib.h>  // メモリ割り当て（malloc, free）を使用

// スレッドローカルストレージ（TLS）のキー
// 各スレッドに固有のデータを関連付けるために使用
pthread_key_t thread_key;

// クリーンアップ関数
// スレッド終了時にTLSデータ（動的に割り当てたメモリ）を解放
void cleanup_thread_data(void* data) {
    // 渡されたデータを解放（メモリリーク防止）
    free(data);
}

// スレッドが実行する関数
// TLSを使用してスレッド固有のデータを管理
void* thread_function(void* arg) {
    // スレッドに渡された引数をint型にキャスト
    // スレッドIDとして使用（例: 1, 2, 3）
    int* id = (int*)arg;
    
    // スレッド固有のデータ用にメモリを動的に割り当て
    int* data = malloc(sizeof(int));
    if (data == NULL) {
        // メモリ割り当てに失敗した場合、エラーメッセージを表示
        perror("メモリ割り当てに失敗しました");
        pthread_exit(NULL);
    }
    
    // 割り当てたメモリに引数の値をコピー
    *data = *id;
    
    // TLSキーにデータ（メモリのポインタ）を関連付け
    // スレッドごとに独立したストレージに保存
    if (pthread_setspecific(thread_key, data) != 0) {
        perror("TLSデータの設定に失敗しました");
        free(data); // エラー時にメモリ解放
        pthread_exit(NULL);
    }
    
    // TLSからデータを取得
    // thread_keyに関連付けられたスレッド固有のポインタを取得
    int* tls_data = pthread_getspecific(thread_key);
    if (tls_data == NULL) {
        // TLSデータ取得に失敗した場合（まれなケース）
        perror("TLSデータの取得に失敗しました");
        free(data); // エラー時にメモリ解放
        pthread_exit(NULL);
    }
    
    // TLSデータの値とスレッドIDを表示
    printf("スレッド %d: TLS値 = %d\n", *id, *tls_data);
    
    // スレッド終了（クリーンアップ関数が自動的に呼ばれる）
    // cleanup_thread_dataがdataを解放
    return NULL;
}

// プログラムのメイン関数
int main() {
    // 3つのスレッド識別子を格納する配列
    pthread_t threads[3];
    // 各スレッドに渡す引数（スレッドID: 1, 2, 3）
    int thread_args[3] = {1, 2, 3};
    
    // TLSキーを生成
    // thread_key: スレッド固有のデータを管理するキー
    // cleanup_thread_data: スレッド終了時に呼ばれるクリーンアップ関数
    if (pthread_key_create(&thread_key, cleanup_thread_data) != 0) {
        perror("TLSキー作成に失敗しました");
        return 1;
    }
    
    // 3つのスレッドを作成
    for (int i = 0; i < 3; i++) {
        // スレッドを作成
        // threads[i]: スレッド識別子
        // NULL: デフォルトのスレッド属性（ジョイン可能）
        // thread_function: スレッドが実行する関数
        // &thread_args[i]: スレッドIDのアドレスを引数として渡す
        if (pthread_create(&threads[i], NULL, thread_function, &thread_args[i]) != 0) {
            perror("スレッド作成に失敗しました");
            return 1;
        }
    }
    
    // すべてのスレッドの終了を待機
    for (int i = 0; i < 3; i++) {
        // pthread_joinでスレッド終了を待つ
        // NULL: スレッドの戻り値を受け取らない
        if (pthread_join(threads[i], NULL) != 0) {
            perror("スレッド待機に失敗しました");
            return 1;
        }
    }
    
    // TLSキーを破棄
    // すべてのスレッド終了後に呼び出し（リソース解放）
    if (pthread_key_delete(thread_key) != 0) {
        perror("TLSキー破棄に失敗しました");
        return 1;
    }
    
    // プログラム正常終了（終了コード0）
    return 0;
}