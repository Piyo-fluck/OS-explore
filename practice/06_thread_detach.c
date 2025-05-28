// 必要なヘッダファイルをインクルード
#include <pthread.h> // POSIXスレッド関連の関数や型を定義（pthread_create, pthread_attr_tなど）
#include <stdio.h>   // 標準入出力関数（printfなど）を使用
#include <unistd.h>  // sleep関数を使用（スレッドやプロセスの一時停止）

// デタッチスレッドが実行する関数
// 引数は使用せず、スレッドの独立した動作を定義
void* detached_thread(void* arg) {
    // スレッド開始時にメッセージを表示
    printf("デタッチスレッドが実行中\n");
    
    // 2秒間スレッドを一時停止（メインスレッドとの動作タイミングを観察）
    sleep(2);
    
    // スレッド終了時にメッセージを表示
    printf("デタッチスレッドが終了\n");
    
    // スレッドを明示的に終了（NULLを返して正常終了）
    pthread_exit(NULL);
}

// プログラムのメイン関数
int main() {
    // スレッド識別子を格納する変数
    pthread_t thread;
    // スレッド属性を格納する構造体
    pthread_attr_t attr;

    // スレッド属性を初期化
    // pthread_attr_t構造体をデフォルト値で設定
    if (pthread_attr_init(&attr) != 0) {
        // 属性初期化に失敗した場合、エラーメッセージを表示
        perror("スレッド属性の初期化に失敗しました");
        return 1;
    }

    // スレッドをデタッチ状態に設定
    // PTHREAD_CREATE_DETACHED: スレッド終了時にリソースが自動解放され、pthread_join不要
    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
        // デタッチ状態設定に失敗した場合、エラーメッセージを表示
        perror("デタッチ状態の設定に失敗しました");
        return 1;
    }

    // 新しいデタッチスレッドを作成
    // thread: スレッド識別子
    // attr: スレッド属性（デタッチ状態を設定済み）
    // detached_thread: スレッドが実行する関数
    // NULL: スレッド関数に渡す引数（この例では不要）
    if (pthread_create(&thread, &attr, detached_thread, NULL) != 0) {
        // スレッド作成に失敗した場合、エラーメッセージを表示
        perror("スレッド作成に失敗しました");
        return 1;
    }

    // スレッド属性を破棄
    // 初期化した属性リソースを解放（メモリリーク防止）
    if (pthread_attr_destroy(&attr) != 0) {
        // 属性破棄に失敗した場合、エラーメッセージを表示
        perror("スレッド属性の破棄に失敗しました");
        return 1;
    }

    // メインスレッドの動作を継続
    printf("メインスレッドが継続中...\n");
    
    // 3秒間待機（デタッチスレッドの出力を確認するため）
    // デタッチスレッドは独立して動作するため、pthread_joinは不要
    sleep(3);
    
    // メインスレッド終了時にメッセージを表示
    printf("メインスレッドが終了\n");

    // プログラム正常終了（終了コード0）
    return 0;
}