// 必要なヘッダファイルをインクルード
#include <pthread.h> // POSIXスレッド関連の関数や型（pthread_barrier_t, pthread_createなど）を定義
#include <stdio.h>   // 標準入出力関数（printfなど）を使用

// スレッドの数（定数）
#define NUM_THREADS 3
// バリアオブジェクト：スレッド間の同期ポイントを定義
pthread_barrier_t barrier;

// スレッドが実行する関数
// バリアで同期し、スレッドIDに基づく動作を行う
void* barrier_thread(void* arg) {
    // 引数をint型にキャストしてスレッドIDを取得
    int id = *(int*)arg;
    
    // バリア到達前にスレッドIDを表示
    printf("スレッド %d: バリア前\n", id);
    
    // バリアで待機
    // NUM_THREADS（3つ）のスレッドがすべて到達するまでブロック
    // 到達後、全スレッドが同時に再開
    if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
        // 1つのスレッドがこの条件を満たし、特別な処理が可能
        // この例では使用しないが、ログ出力などに活用可能
        printf("スレッド %d: バリア解放スレッド\n", id);
    }
    
    // バリア通過後にスレッドIDを表示
    printf("スレッド %d: バリア後\n", id);
    
    // スレッド終了（戻り値なし）
    return NULL;
}

// プログラムのメイン関数
int main() {
    // スレッド識別子を格納する配列（3スレッド分）
    pthread_t threads[NUM_THREADS];
    // 各スレッドに渡すIDを格納する配列
    int thread_ids[NUM_THREADS];
    
    // バリアを初期化
    // NULL: デフォルト属性
    // NUM_THREADS: バリアを通過するために必要なスレッド数（3）
    if (pthread_barrier_init(&barrier, NULL, NUM_THREADS) != 0) {
        // バリア初期化に失敗した場合、エラーメッセージを表示
        perror("バリア初期化に失敗しました");
        return 1;
    }
    
    // 3つのスレッドを作成
    for (int i = 0; i < NUM_THREADS; i++) {
        // スレッドIDを配列に設定（0, 1, 2）
        thread_ids[i] = i;
        // スレッドを作成
        // threads[i]: スレッド識別子
        // NULL: デフォルトのスレッド属性（ジョイン可能）
        // barrier_thread: スレッドが実行する関数
        // &thread_ids[i]: スレッドIDのアドレスを引数として渡す
        if (pthread_create(&threads[i], NULL, barrier_thread, &thread_ids[i]) != 0) {
            perror("スレッド作成に失敗しました");
            return 1;
        }
    }
    
    // すべてのスレッドの終了を待機
    for (int i = 0; i < NUM_THREADS; i++) {
        // pthread_joinでスレッド終了を待つ
        // NULL: スレッドの戻り値を受け取らない
        if (pthread_join(threads[i], NULL) != 0) {
            perror("スレッド待機に失敗しました");
            return 1;
        }
    }
    
    // バリアを破棄（リソース解放）
    if (pthread_barrier_destroy(&barrier) != 0) {
        perror("バリア破棄に失敗しました");
        return 1;
    }
    
    // プログラム正常終了（終了コード0）
    return 0;
}