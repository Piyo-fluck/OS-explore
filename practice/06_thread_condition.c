// 必要なヘッダファイルをインクルード
#include <pthread.h> // POSIXスレッド関連の関数や型（pthread_mutex_t, pthread_cond_tなど）を定義
#include <stdio.h>   // 標準入出力関数（printfなど）を使用
#include <stdlib.h>  // exit関数などを使用
#include <unistd.h>

// グローバル変数の定義
// ミューテックス：スレッド間の排他制御に使用
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// 条件変数：スレッド間の同期（待機と通知）に使用
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// 共有データが準備完了かどうかを示すフラグ
int ready = 0;

// プロデューサスレッドの関数
// データを準備し、コンシューマに通知
void* producer(void* arg) {
    // ミューテックスをロック（共有リソースへの排他アクセスを確保）
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("プロデューサのミューテックスロックに失敗しました");
        exit(1);
    }
    
    // 共有データ（ready）を準備（1に設定）
    ready = 1;
    // 準備完了をコンソールに表示
    printf("プロデューサー: データが準備完了\n");
    
    // 条件変数で待機中のコンシューマに通知
    // コンシューマがpthread_cond_waitで待機している場合に再開
    if (pthread_cond_signal(&cond) != 0) {
        perror("プロデューサの条件変数シグナルに失敗しました");
        exit(1);
    }
    
    // ミューテックスをアンロック（他のスレッドがアクセス可能に）
    if (pthread_mutex_unlock(&mutex) != 0) {
        perror("プロデューサのミューテックスアンロックに失敗しました");
        exit(1);
    }
    
    // スレッド終了（戻り値なし）
    return NULL;
}

// コンシューマスレッドの関数
// データが準備されるまで待機し、取得後に処理
void* consumer(void* arg) {
    // ミューテックスをロック（共有リソースへの排他アクセスを確保）
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("コンシューマのミューテックスロックに失敗しました");
        exit(1);
    }
    
    // データが準備されるまで待機
    // readyが1になるまでループ（スプリアスウェイクアップ対策）
    while (!ready) {
        // 待機中のメッセージを表示
        printf("コンシューマ: データ待機中\n");
        
        // 条件変数で待機（ミューテックスを解放し、シグナルを待つ）
        // シグナル受信後、ミューテックスを再ロック
        if (pthread_cond_wait(&cond, &mutex) != 0) {
            perror("コンシューマの条件変数待機に失敗しました");
            exit(1);
        }
    }
    
    // データが準備されたことを表示
    printf("コンシューマ: データを受信\n");
    
    // ミューテックスをアンロック（他のスレッドがアクセス可能に）
    if (pthread_mutex_unlock(&mutex) != 0) {
        perror("コンシューマのミューテックスアンロックに失敗しました");
        exit(1);
    }
    
    // スレッド終了（戻り値なし）
    return NULL;
}

// プログラムのメイン関数
int main() {
    // プロデューサとコンシューマのスレッド識別子
    pthread_t prod_thread, cons_thread;
    
    // コンシューマスレッドを最初に作成
    // NULL: デフォルトのスレッド属性（ジョイン可能）
    // consumer: コンシューマスレッドが実行する関数
    // NULL: 引数なし
    if (pthread_create(&cons_thread, NULL, consumer, NULL) != 0) {
        perror("コンシューマスレッド作成に失敗しました");
        return 1;
    }
    
    // コンシューマが先に待機状態に入るよう1秒待機
    // プロデューサが先に実行されるのを防ぐ
    sleep(1);
    
    // プロデューサスレッドを作成
    // NULL: デフォルトのスレッド属性（ジョイン可能）
    // producer: プロデューサスレッドが実行する関数
    // NULL: 引数なし
    if (pthread_create(&prod_thread, NULL, producer, NULL) != 0) {
        perror("プロデューサスレッド作成に失敗しました");
        return 1;
    }
    
    // プロデューサスレッドの終了を待機
    if (pthread_join(prod_thread, NULL) != 0) {
        perror("プロデューサスレッド待機に失敗しました");
        return 1;
    }
    
    // コンシューマスレッドの終了を待機
    if (pthread_join(cons_thread, NULL) != 0) {
        perror("コンシューマスレッド待機に失敗しました");
        return 1;
    }
    
    // ミューテックスを破棄（リソース解放）
    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("ミューテックス破棄に失敗しました");
        return 1;
    }
    
    // 条件変数を破棄（リソース解放）
    if (pthread_cond_destroy(&cond) != 0) {
        perror("条件変数破棄に失敗しました");
        return 1;
    }
    
    // プログラム正常終了（終了コード0）
    return 0;
}