#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_REF 100
#define MAX_FRAME 20

// コンマ区切りまたはスペース区切りのページ参照列を入力
void input_pages(int *pages, int *n, int *frame_size) {
    char buf[1024];
    printf("ページ参照列をカンマまたはスペース区切りで入力（例: 0,2,1,3,5 または 0 2 1 3 5）:\n");
    fgets(buf, sizeof(buf), stdin);

    int i = 0;
    char *p = buf;
    while (*p) {
        // 数字をスキップ
        while (*p && !isdigit(*p) && *p != '-') p++;
        if (!*p) break;
        pages[i++] = strtol(p, &p, 10);
        // カンマやスペースをスキップ
        while (*p && (isspace(*p) || *p == ',')) p++;
    }
    *n = i;
    printf("フレーム数を入力してください: \n");
    scanf("%d", frame_size);
    getchar(); // 改行消費
}

// FIFOアルゴリズム
int fifo(int *pages, int n, int frame_size, int *final_frame) {
    int frame[MAX_FRAME] = {0};
    int front = 0, faults = 0;
    for (int i = 0; i < frame_size; i++) frame[i] = -1;

    for (int i = 0; i < n; i++) {
        int found = 0;
        for (int j = 0; j < frame_size; j++) {
            if (frame[j] == pages[i]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            frame[front] = pages[i];
            front = (front + 1) % frame_size;
            faults++;
        }
    }
    for (int i = 0; i < frame_size; i++) final_frame[i] = frame[i];
    return faults;
}

// LRUアルゴリズム
int lru(int *pages, int n, int frame_size, int *final_frame) {
    int frame[MAX_FRAME] = {0};
    int recent[MAX_FRAME] = {0};
    int faults = 0, time = 0;
    for (int i = 0; i < frame_size; i++) frame[i] = -1;

    for (int i = 0; i < n; i++) {
        int found = 0;
        for (int j = 0; j < frame_size; j++) {
            if (frame[j] == pages[i]) {
                found = 1;
                recent[j] = ++time;
                break;
            }
        }
        if (!found) {
            int lru_idx = 0;
            for (int j = 1; j < frame_size; j++) {
                if (recent[j] < recent[lru_idx]) lru_idx = j;
            }
            frame[lru_idx] = pages[i];
            recent[lru_idx] = ++time;
            faults++;
        }
    }
    for (int i = 0; i < frame_size; i++) final_frame[i] = frame[i];
    return faults;
}


void print_result(const char *name, int faults, int *frame, int frame_size) {
    printf("[%s] ページフォルト数: %d\n", name, faults);
    printf("[%s] 最終フレーム: ", name);
    for (int i = 0; i < frame_size; i++) {
        if (frame[i] != -1)
            printf("%d ", frame[i]);
    }
    printf("\n");
}

// ...existing code...

// OPT（最適）アルゴリズム
int opt(int *pages, int n, int frame_size, int *final_frame) {
    int frame[MAX_FRAME] = {0};
    for (int i = 0; i < frame_size; i++) frame[i] = -1;
    int faults = 0;

    for (int i = 0; i < n; i++) {
        int found = 0;
        // すでにフレーム内にあるか確認
        for (int j = 0; j < frame_size; j++) {
            if (frame[j] == pages[i]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            // 空きフレームがあればそこに入れる
            int empty = -1;
            for (int j = 0; j < frame_size; j++) {
                if (frame[j] == -1) {
                    empty = j;
                    break;
                }
            }
            if (empty != -1) {
                frame[empty] = pages[i];
            } else {
                // どのページを置き換えるか決める
                int farthest = -1, idx = -1;
                for (int j = 0; j < frame_size; j++) {
                    int k;
                    for (k = i + 1; k < n; k++) {
                        if (frame[j] == pages[k]) break;
                    }
                    if (k > farthest) {
                        farthest = k;
                        idx = j;
                    }
                }
                frame[idx] = pages[i];
            }
            faults++;
        }
    }
    for (int i = 0; i < frame_size; i++) final_frame[i] = frame[i];
    return faults;
}

// ...existing code...

int main() {
    int pages[MAX_REF], n, frame_size;
    int fifo_frame[MAX_FRAME], lru_frame[MAX_FRAME], opt_frame[MAX_FRAME];

    input_pages(pages, &n, &frame_size);

    int fifo_faults = fifo(pages, n, frame_size, fifo_frame);
    int lru_faults = lru(pages, n, frame_size, lru_frame);
    int opt_faults = opt(pages, n, frame_size, opt_frame);

    print_result("FIFO", fifo_faults, fifo_frame, frame_size);
    print_result("LRU", lru_faults, lru_frame, frame_size);
    print_result("OPT", opt_faults, opt_frame, frame_size);

    return 0;
}
// ...existing code...
