#include <stdio.h>
#include <string.h>

int main() {
    // 通信設定
    int baud_rate = 9600;       // 通信速度（bps）ex 19300
    int start_bits = 1;         // スタートビット数
    int data_bits = 8;          // データビット数
    int stop_bits = 2;          // ストップビット数

    // ANSI エスケープシーケンス（画面クリア）
    const char *ansi_clear = "\x1b[2J";  // ESC [ 2 J
    int ansi_bytes = strlen(ansi_clear);

    // ユーザー入力
    char message[256];
    printf("表示する文字列を入力してください: ");
    fgets(message, sizeof(message), stdin);

    // 改行を除去
    size_t len = strlen(message);
    if (len > 0 && message[len - 1] == '\n') {
        message[len - 1] = '\0';
        len--;
    }

    // 総バイト数（ANSI + メッセージ）
    int total_bytes = ansi_bytes + len;

    // 1バイトあたりの送信ビット数
    int bits_per_byte = start_bits + data_bits + stop_bits;

    // 総送信ビット数
    int total_bits = total_bytes * bits_per_byte;

    // 送信時間（秒）
    double transmission_time = (double)total_bits / baud_rate;

    // 結果表示
    printf("\n--- 計算結果 ---\n");
    printf("ANSIシーケンス: %d バイト\n", ansi_bytes);
    printf("メッセージ: %zu バイト\n", len);
    printf("合計: %d バイト, %d ビット\n", total_bytes, total_bits);
    printf("送信時間: %.6f 秒\n", transmission_time);

    return 0;
}
