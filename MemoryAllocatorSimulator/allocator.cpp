// allocator.cpp
#include "allocator.h"
#include <iostream>

static char heap[HEAP_SIZE];
static BlockHeader* free_list = nullptr;

void initialize_heap() {
    free_list = (BlockHeader*)heap;
    free_list->size = HEAP_SIZE - sizeof(BlockHeader);
    free_list->is_free = true;
    free_list->next = nullptr;
}
void* my_malloc(size_t size) {
    BlockHeader* current = free_list;
    while (current) {
        if (current->is_free && current->size >= size) {
            // Split block if it's large enough
            if (current->size >= size + sizeof(BlockHeader) + 1) {
                BlockHeader* new_block = (BlockHeader*)((char*)current + sizeof(BlockHeader) + size);
                new_block->size = current->size - size - sizeof(BlockHeader);
                new_block->is_free = true;
                new_block->next = current->next;
                current->next = new_block;
                current->size = size;
            }
            current->is_free = false;
            return (char*)current + sizeof(BlockHeader);
        }
        current = current->next;
    }
    return nullptr; // No sufficient block found
}
void my_free(void* ptr) {
    if (!ptr) return;
    BlockHeader* block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    block->is_free = true;

    // Optional: Coalescing adjacent free blocks
    BlockHeader* current = free_list;
    while (current) {
        if (current->is_free && current->next && current->next->is_free) {
            current->size += sizeof(BlockHeader) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void print_heap_status() {
    BlockHeader* current = free_list;
    size_t offset = 0;  // Track where we are in the heap (optional but helps visualization)

    std::cout << "Heap Status:\n";
    while (current) {
        // Calculate offset by pointer arithmetic (optional, for educational visualization)
        offset = (char*)current - heap;  // heap is the base char array

        std::cout << "Block at offset " << offset
                  << " | Size: " << current->size
                  << " bytes | Status: " << (current->is_free ? "[FREE]" : "[USED]")
                  << std::endl;

        current = current->next;
    }
}
