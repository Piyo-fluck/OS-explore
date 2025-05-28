// 必要なヘッダファイルをインクルード
#include <pthread.h> // POSIXスレッド関連の関数や型を定義（pthread_create, pthread_cancelなど）
#include <stdio.h>   // 標準入出力関数（printfなど）を使用
#include <unistd.h>  // sleep関数を使用（スレッドやプロセスの一時停止）

// キャンセル可能なスレッドが実行する関数
// 無限ループで動作し、キャンセルポイントを設ける
void* cancellable_thread(void* arg) {
    // ループカウンタを初期化
    int i = 0;
    
    // 無限ループでスレッドの動作を継続
    while (1) {
        // ループの各イテレーションでカウンタを増やして表示
        printf("スレッドのイテレーション %d\n", ++i);
        
        // 1秒間待機（スレッドの動作タイミングを観察）
        sleep(1);
        
        // キャンセルポイントを設定
        // pthread_testcancelはスレッドがキャンセル要求を受け取った場合に終了
        // キャンセルポイントはスレッドが安全に終了できる場所
        pthread_testcancel();
    }
    
    // ループが中断されない場合の戻り値（通常は到達しない）
    return NULL;
}

// プログラムのメイン関数
int main() {
    // スレッド識別子を格納する変数
    pthread_t thread;
    
    // 新しいスレッドを作成
    // thread: スレッド識別子
    // NULL: デフォルトのスレッド属性を使用（ジョイン可能）
    // cancellable_thread: スレッドが実行する関数
    // NULL: スレッド関数に渡す引数（この例では不要）
    if (pthread_create(&thread, NULL, cancellable_thread, NULL) != 0) {
        // スレッド作成に失敗した場合、エラーメッセージを表示
        perror("スレッド作成に失敗しました");
        return 1;
    }
    
    // スレッドを3秒間実行させるために待機
    // メインスレッドを一時停止して、スレッドの出力を確認
    sleep(3);
    
    // スレッドにキャンセル要求を発行
    printf("スレッドをキャンセル中...\n");
    if (pthread_cancel(thread) != 0) {
        // キャンセル要求に失敗した場合、エラーメッセージを表示
        perror("スレッドキャンセルに失敗しました");
        return 1;
    }
    
    // スレッドの終了を待機
    // pthread_joinはスレッドがキャンセルまたは終了するまでメインスレッドをブロック
    // NULL: スレッドの戻り値を受け取らない
    if (pthread_join(thread, NULL) != 0) {
        // スレッド待機に失敗した場合、エラーメッセージを表示
        perror("スレッド待機に失敗しました");
        return 1;
    }
    
    // スレッドがキャンセルされ、ジョインされたことを表示
    printf("スレッドがキャンセルされ、ジョインされました\n");
    
    // プログラム正常終了（終了コード0）
    return 0;
}