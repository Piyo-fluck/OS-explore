#include <stdio.h>
#include <stdlib.h>

// シミュレーションパラメータ
double delta = 1e-6; // 各命令の実行時間 [s]
int N = 2560;         // 割り込み処理の命令数
double T = 1e-2;     // クォンタムの大きさ [s]
int num_processes = 10; // プロセスの数
double simulation_time = 1.0; // シミュレーション時間 [s]

typedef struct {
    int id;
    double remaining_time; // プロセスが実行すべき残り時間
} Process;

int main() {
    // プロセスの初期化
    Process* processes = (Process*)malloc(num_processes * sizeof(Process));
    for (int i = 0; i < num_processes; i++) {
        processes[i].id = i;
        processes[i].remaining_time = simulation_time; // 各プロセスに十分な実行時間を設定
    }

    double current_time = 0.0; // 現在時刻
    double process_execution_time = 0.0; // プロセス実行に使った時間
    int current_process = 0; // 現在実行中のプロセス
    int cycle_count = 0; // サイクル数（デバッグ用）

    // シミュレーションループ
    while (current_time < simulation_time) {
        // クォンタム分のプロセス実行
        double execution_time = T;
        if (current_time + execution_time > simulation_time) {
            execution_time = simulation_time - current_time;
        }
        process_execution_time += execution_time;
        current_time += execution_time;

        // 割り込み処理のオーバーヘッド
        double interrupt_time = N * delta;
        if (current_time + interrupt_time > simulation_time) {
            interrupt_time = simulation_time - current_time;
        }
        current_time += interrupt_time;

        // 次のプロセスに切り替え
        current_process = (current_process + 1) % num_processes;
        cycle_count++;

        // シミュレーション終了条件
        if (current_time >= simulation_time) {
            break;
        }
    }

    // 実効利用率の計算
    double effective_utilization = process_execution_time / simulation_time;

    // 結果の出力
    printf("シミュレーション結果:\n");
    printf("1回の割り込みに要する時間: %.6f [s]\n", N*delta);
    printf("1秒間最大の呼び出し回数: %.6f [s]\n", 1/(N*delta));
    printf("総シミュレーション時間: %.6f [s]\n", simulation_time);
    printf("プロセス実行時間: %.6f [s]\n", process_execution_time);
    printf("実効利用率: %.6f\n", effective_utilization);
    printf("理論値 (T / (T + N*delta)): %.6f\n", T / (T + N * delta));
    printf("サイクル数: %d\n", cycle_count);
    printf("総割り込み実行時間: %.6f\n", cycle_count*N*delta);
    for (int q = 0; q< num_processes; q++) {
        printf("プロセス %d 実行時間: %.6f\n",q, simulation_time-processes[q].remaining_time);
    }

    // メモリ解放
    free(processes);

    return 0;
}