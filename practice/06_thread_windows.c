#include <windows.h>
#include <stdio.h>

DWORD WINAPI ThreadFunc(LPVOID param) {
    int priority = GetThreadPriority(GetCurrentThread());
    printf("Thread running with priority: %d\n", priority);
    return 0;
}

int main() {
    HANDLE thread;

    // スレッド作成
    thread = CreateThread(
        NULL,        // デフォルトのセキュリティ属性
        0,           // デフォルトのスタックサイズ
        ThreadFunc,  // 実行関数
        NULL,        // パラメータ
        0,           // フラグ
        NULL         // スレッドIDは不要なのでNULL
    );

    if (thread == NULL) {
        fprintf(stderr, "スレッドの作成に失敗しました。\n");
        return 1;
    }

    // 優先度の設定（例：THREAD_PRIORITY_HIGHEST）
    if (!SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST)) {
        fprintf(stderr, "優先度の設定に失敗しました。\n");
    }

    // スレッド終了を待つ
    WaitForSingleObject(thread, INFINITE);

    // ハンドルを閉じる
    CloseHandle(thread);

    return 0;
}
