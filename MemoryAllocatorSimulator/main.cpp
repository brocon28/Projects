// main.cpp
#include "allocator.h"
#include <iostream>

int main() {
    initialize_heap();
    std::cout << "Heap initialized." << std::endl;

    void* ptr1 = my_malloc(256);
    std::cout << "Allocated 256 bytes at " << ptr1 << std::endl;

    void* ptr2 = my_malloc(512);
    std::cout << "Allocated 512 bytes at " << ptr2 << std::endl;

    my_free(ptr1);
    std::cout << "Freed first block." << std::endl;

    void* ptr3 = my_malloc(128);
    std::cout << "Allocated 128 bytes at " << ptr3 << std::endl;

    print_heap_status();
    //add more allocations/frees and print_heap_status() for debugging

    return 0;
}
