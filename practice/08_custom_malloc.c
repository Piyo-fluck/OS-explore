#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))
#define BLOCK_SIZE sizeof(block_t)
// #define custom_malloc(size) debug_malloc(size, __FILE__, __LINE__)
// #define custom_free(ptr) debug_free(ptr, __FILE__, __LINE__)
// メモリブロックの構造体
typedef struct block_t {
    size_t size;          // ブロック全体のサイズ（ヘッダ部分も含む）
    bool is_free;         // このブロックが空きかどうか
    struct block_t* next; // 次のブロックへのポインタ（連結リスト用）
    struct block_t* prev; // 前のブロックへのポインタ（連結リスト用）
    char data[1];         // 実際のデータ領域の先頭（可変長配列のトリック）
} block_t;

// ヒープ管理用の構造体
typedef struct {
    block_t* free_list;  // 空きブロックのリストの先頭
    block_t* used_list;  // 使用中ブロックのリストの先頭
    size_t total_size;   // ヒープ全体のサイズ
    size_t used_size;    // 現在使用中のサイズ
} heap_t;

// グローバルなヒープ構造体
static heap_t heap = {0};

// ヒープの初期化関数
void init_heap(size_t initial_size) {
    initial_size = ALIGN(initial_size); // サイズをアラインメントに合わせる
    
    // OSからメモリを要求
    void* memory = sbrk(initial_size);
    if (memory == (void*)-1) {
        perror("Failed to initialize heap"); // メモリ確保失敗時のエラー表示
        return;
    }
    memset(memory, 0, initial_size);
    
    // 最初のブロックを初期化
    block_t* initial_block = (block_t*)memory;
    initial_block->size = initial_size; // ブロックサイズ設定
    initial_block->is_free = true;      // 空きブロックとしてマーク
    initial_block->next = NULL;
    initial_block->prev = NULL;
    
    // ヒープ構造体の初期化
    heap.free_list = initial_block; // 空きリストに登録
    heap.used_list = NULL;          // 使用中リストは空
    heap.total_size = initial_size; // 総メモリサイズ
    heap.used_size = 0;             // 使用中サイズ
}

// 最適な空きブロックを探す（ベストフィット法）
block_t* find_best_fit(size_t size) {
    block_t* current = heap.free_list;
    block_t* best_fit = NULL;
    size_t smallest_diff = SIZE_MAX;
    
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            size_t diff = current->size - size;
            if (diff < smallest_diff) {
                smallest_diff = diff;
                best_fit = current;
                
                // 完全一致なら探索終了
                if (diff == 0) break;
            }
        }
        current = current->next;
    }
    
    return best_fit;
}

// ブロックを分割する関数
void split_block(block_t* block, size_t size) {
    size_t remaining_size = block->size - size;
    
    // 残りサイズが新しいブロックとして十分な場合のみ分割
    if (remaining_size > BLOCK_SIZE + ALIGNMENT) {
        block_t* new_block = (block_t*)((char*)block + size);
        new_block->size = remaining_size;
        new_block->is_free = true;
        new_block->next = block->next;
        new_block->prev = block;
        
        if (block->next) {
            block->next->prev = new_block;
        }
        
        block->next = new_block;
        block->size = size;
    }
}

// メモリ確保関数（malloc相当）
// ...existing code...

void* custom_malloc(size_t size) {
    if (size == 0) return NULL;

    size_t total_size = ALIGN(size + BLOCK_SIZE);
    block_t* block = find_best_fit(total_size);

    if (block == NULL) {
        size_t request_size = total_size > 4096 ? total_size : 4096;
        void* memory = sbrk(request_size);
        if (memory == (void*)-1) {
            return NULL;
        }

        block = (block_t*)memory;
        block->size = request_size;
        block->is_free = true;
        block->next = heap.free_list;
        block->prev = NULL;

        if (heap.free_list) {
            heap.free_list->prev = block;
        }

        heap.free_list = block;
        heap.total_size += request_size;
    }

    split_block(block, total_size);

    // 空きリストから完全に外す
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        heap.free_list = block->next;
    }
    if (block->next) {
        block->next->prev = block->prev;
    }
    block->next = NULL;
    block->prev = NULL;

    // 使用中リストの先頭に追加
    block->is_free = false;
    block->next = heap.used_list;
    if (heap.used_list) {
        heap.used_list->prev = block;
    }
    block->prev = NULL;
    heap.used_list = block;

    heap.used_size += block->size;

    return block->data;
}
// ...existing code...

// 隣接する空きブロックを統合する関数
void coalesce_blocks(block_t* block) {
    // 次のブロックが空きなら統合
    if (block->next && block->next->is_free) {
        block->size += block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
    
    // 前のブロックが空きなら統合
    if (block->prev && block->prev->is_free) {
        block->prev->size += block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
        block = block->prev;
    }
}

// メモリ解放関数（free相当）
// ...existing code...

void custom_free(void* ptr) {
    if (!ptr) return;

    block_t* block = (block_t*)((char*)ptr - BLOCK_SIZE);
    if (block->is_free) {
        fprintf(stderr, "Error: Double free detected at %p\n", ptr);
        return;
    }
    block->is_free = true;

    // 使用中リストから削除
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        heap.used_list = block->next;
    }
    if (block->next) {
        block->next->prev = block->prev;
    }

    // 空きリストの先頭に追加
    block->next = heap.free_list;
    block->prev = NULL;
    if (heap.free_list && heap.free_list != block) { // 追加: 自分自身でなければ
        heap.free_list->prev = block;
    }
    heap.free_list = block;

    heap.used_size -= block->size;

    // 隣接する空きブロックを統合
    coalesce_blocks(block);
}

// ...existing code...

// メモリ使用状況を表示する関数
void print_memory_stats() {
    printf("\nMemory Statistics:\n");
    printf("Total Heap Size: %zu bytes\n", heap.total_size);
    printf("Used Size: %zu bytes\n", heap.used_size);
    printf("Free Size: %zu bytes\n", heap.total_size - heap.used_size);
    
    printf("\nFree Blocks:\n");
    block_t* current = heap.free_list;
    while (current) {
        printf("Block at %p, size: %zu\n", (void*)current, current->size);
        current = current->next;
    }
    
    printf("\nUsed Blocks:\n");
    current = heap.used_list;
    while (current) {
        printf("Block at %p, size: %zu\n", (void*)current, current->size);
        current = current->next;
    }
}

typedef struct {
    void* ptr;
    size_t size;
    const char* file;
    int line;
} allocation_info_t;

#define MAX_ALLOCATIONS 1000
static allocation_info_t allocations[MAX_ALLOCATIONS];
static int allocation_count = 0;

void* debug_malloc(size_t size, const char* file, int line) {
    void* ptr = custom_malloc(size);
    if (ptr && allocation_count < MAX_ALLOCATIONS) {
        allocations[allocation_count].ptr = ptr;
        allocations[allocation_count].size = size;
        allocations[allocation_count].file = file;
        allocations[allocation_count].line = line;
        allocation_count++;
    }
    return ptr;
}

void debug_free(void* ptr, const char* file, int line) {
    if (!ptr) return;

    bool found = false;
    for (int i = 0; i < allocation_count; i++) {
        if (allocations[i].ptr == ptr) {
            memmove(&allocations[i], &allocations[i + 1], 
                    (allocation_count - i - 1) * sizeof(allocation_info_t));
            allocation_count--;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "Error: Attempting to free unallocated pointer %p at %s:%d\n", 
                ptr, file, line);
        return;
    }
    custom_free(ptr);
}

void check_leaks() {
    if (allocation_count > 0) {
        printf("\nMemory Leaks Detected:\n");
        for (int i = 0; i < allocation_count; i++) {
            printf("Leak: %zu bytes at %p, allocated in %s:%d\n",
                   allocations[i].size, allocations[i].ptr,
                   allocations[i].file, allocations[i].line);
        }
    } else {
        printf("No memory leaks detected\n");
    }
}

int main() {
    init_heap(1024 * 1024);
    
    int* numbers = (int*)custom_malloc(10 * sizeof(int));
    char* string = (char*)custom_malloc(100);
    
    for (int i = 0; i < 10; i++) {
        numbers[i] = i;
    }
    strcpy(string, "Hello, World!");
    
    print_memory_stats();
    
    custom_free(numbers);
    custom_free(string);
    
    check_leaks();
    
    return 0;
}

// AddressSanitizerの使用：
// コンパイル時にASanを有効化：
// bash 
// gcc -g -fsanitize=address -o a.out 08_custom_malloc.c
// ./a.out