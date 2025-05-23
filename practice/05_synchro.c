#include <pthread.h>      // pthreadライブラリのインクルード
#include <stdio.h>        // 標準入出力
#include <stdlib.h>       // 標準ライブラリ

// グローバルなミューテックスロック
pthread_mutex_t lock;
// 共有カウンタ（複数スレッドでアクセスされる）
int shared_counter = 0;

// カウンタをインクリメントするスレッド関数
void* increment_counter(void* arg) {
    // 100,000回インクリメント
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&lock);   // ミューテックスロックを取得（排他制御開始）
        shared_counter++;            // クリティカルセクション（共有資源へのアクセス）
        pthread_mutex_unlock(&lock); // ミューテックスロックを解放（排他制御終了）
    }
    return NULL; // 戻り値なし
}

int main() {
    pthread_t threads[2]; // 2つのスレッドIDを格納する配列

    pthread_mutex_init(&lock, NULL); // ミューテックス初期化

    // スレッドの生成（2つ）
    for (int i = 0; i < 2; i++) {
        // スレッド作成に失敗した場合はエラー出力して終了
        if (pthread_create(&threads[i], NULL, increment_counter, NULL) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    // すべてのスレッドの終了を待つ
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL); // スレッドの終了待ち
    }

    pthread_mutex_destroy(&lock); // ミューテックス破棄（リソース解放）

    // 最終的なカウンタ値を表示
    printf("Final counter value: %d\n", shared_counter); // 結果表示
    return 0; // 正常終了
}