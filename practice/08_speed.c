#include <stdio.h>

int main() {
    // 映像設定（変更可能）
    int width = 1920;           // ピクセル幅
    int height = 1080;          // ピクセル高さ
    int color_depth = 24;       // ビット/ピクセル（例：24ビットカラー）
    int frame_rate = 5;        // フレーム/秒

    // 1フレームのサイズ（ビット）
    long long bits_per_frame = (long long)width * height * color_depth;

    // 1秒間に必要な通信量（ビット/秒）
    long long bits_per_second = bits_per_frame * frame_rate;

    // 結果表示
    printf("\n--- 映像転送の通信速度計算 ---\n");
    printf("解像度: %d x %d\n", width, height);
    printf("色深度: %d bits/pixel\n", color_depth);
    printf("フレームレート: %d fps\n", frame_rate);
    printf("1フレームのサイズ: %lld bits\n", bits_per_frame);
    printf("必要な通信速度: %lld bits/sec (約 %.2f Mbps)\n",
           bits_per_second, bits_per_second / 1e6);

    return 0;
}
