#include <pthread.h>   // POSIXスレッド用
#include <stdio.h>     // 標準入出力
#include <stdlib.h>    // 動的メモリ割り当てなど
#include <unistd.h>    // sleep関数

#define NUM_THREADS 5  // プロデューサー・コンシューマーの数
#define QUEUE_SIZE 10  // キューの最大サイズ（固定長）

// スレッドセーフなキューの構造体
typedef struct {
    int data[QUEUE_SIZE];              // キュー本体（固定長配列）
    int front;                         // データの先頭インデックス
    int rear;                          // データの末尾インデックス
    pthread_mutex_t mutex;            // キュー操作の排他制御用ミューテックス
    pthread_cond_t not_full;          // キューが満杯ではないことを通知する条件変数
    pthread_cond_t not_empty;         // キューが空でないことを通知する条件変数
} thread_safe_queue_t;

// キューのグローバル変数（初期化子付き）
thread_safe_queue_t queue = {
    .front = 0,
    .rear = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .not_full = PTHREAD_COND_INITIALIZER,
    .not_empty = PTHREAD_COND_INITIALIZER
};

// キューの初期化関数（使わなくても今回は問題ないが、安全のため用意）
void queue_init(thread_safe_queue_t* q) {
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

// キューの破棄関数
void queue_destroy(thread_safe_queue_t* q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);
}

// キューに値を追加（プロデューサー用）
void queue_push(thread_safe_queue_t* q, int value) {
    pthread_mutex_lock(&q->mutex);

    // キューが満杯なら待機（リングバッファなので 1つ空けておく）
    while ((q->rear + 1) % QUEUE_SIZE == q->front) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    // データ追加
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % QUEUE_SIZE;

    // データ追加したので、not_emptyを通知（消費者へ）
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

// キューから値を取り出し（コンシューマー用）
int queue_pop(thread_safe_queue_t* q) {
    pthread_mutex_lock(&q->mutex);

    // キューが空なら待機
    while (q->front == q->rear) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    // データ取得
    int value = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;

    // データを1つ消費したので、not_fullを通知（生産者へ）
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);

    return value;
}

// プロデューサースレッドが実行する関数
void* producer(void* arg) {
    int id = *(int*)arg;

    for (int i = 0; i < 5; i++) {
        int value = id * 100 + i;  // 識別しやすい値
        queue_push(&queue, value);
        printf("Producer %d: Pushed %d\n", id, value);
        sleep(1);  // 少し休む（出力の順序を見やすく）
    }

    return NULL;
}

// コンシューマースレッドが実行する関数
void* consumer(void* arg) {
    int id = *(int*)arg;

    for (int i = 0; i < 5; i++) {
        int value = queue_pop(&queue);
        printf("Consumer %d: Popped %d\n", id, value);
        sleep(1);  // 少し休む
    }

    return NULL;
}

// メイン関数
int main() {
    pthread_t producers[NUM_THREADS];
    pthread_t consumers[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    queue_init(&queue);  // （すでに初期化されているが明示的に）

    // プロデューサー・コンシューマーを起動
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &thread_ids[i]);
        pthread_create(&consumers[i], NULL, consumer, &thread_ids[i]);
    }

    // すべてのスレッドの終了を待つ
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    queue_destroy(&queue);  // キュー破棄

    printf("All threads have finished execution\n");
    return 0;
}
