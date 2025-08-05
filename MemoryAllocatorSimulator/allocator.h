// allocator.h
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>  // for size_t

const size_t HEAP_SIZE = 1024 * 1024; // 1 MB Heap

struct BlockHeader {
    size_t size;
    bool is_free;
    BlockHeader* next;
};

void initialize_heap();
void* my_malloc(size_t size);
void my_free(void* ptr);
void print_heap_status(); // For debugging
void print_heap_status();

#endif
