
#include "std_allocator.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void* std_allocate(void* context_ptr, size_t alignment, size_t size) {
    return malloc(size);
}

void* std_reallocate(void* context_ptr, void* mem_ptr, size_t alignment, size_t old_size, size_t new_size) {
    return realloc(mem_ptr, new_size);
}

void std_deallocate(void* context_ptr, void* mem_ptr) {
    return free(mem_ptr);
}

#pragma GCC diagnostic pop
