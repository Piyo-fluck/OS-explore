#include <stdio.h>
#include <string.h>

int main() {
    // ディスプレイ設定
    int screen_width = 800;
    int screen_height = 600;
    int color_depth = 24; // bits per pixel

    // フォント設定（1文字あたりのサイズ）
    int font_width = 16;
    int font_height = 32;

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

    // 画面クリアに必要なVRAM（バイト）
    long screen_bits = screen_width * screen_height * color_depth;
    long screen_bytes = screen_bits / 8;

    // 1文字の表示に必要なVRAM（バイト）
    long char_bits = font_width * font_height * color_depth;
    long char_bytes = char_bits / 8;

    // メッセージ表示に必要なVRAM（バイト）
    long message_bytes = char_bytes * len;

    // 合計VRAM書き込み量（バイト）
    long total_bytes = screen_bytes + message_bytes;

    // 結果表示
    printf("\n--- 計算結果 ---\n");
    printf("画面クリア: %ld バイト\n", screen_bytes);
    printf("1文字あたり: %ld バイト\n", char_bytes);
    printf("メッセージ（%zu文字）: %ld バイト\n", len, message_bytes);
    printf("合計書き込み量: %ld バイト (約 %.2f MB)\n", total_bytes, total_bytes / (1024.0 * 1024.0));

    return 0;
}
