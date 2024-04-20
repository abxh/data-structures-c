#include "allocators/arena.h"    // arena_*
#include "align_forward.h"       // arena_forward
#include "header-only/is_pow2.h" // is_pow2
#include <assert.h>              // assert
#include <stdbool.h>             // false
#include <stdint.h>              // uintptr_t
#include <stdlib.h>              // size_t
#include <string.h>              // memset

// Source:
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

typedef struct arena_type {
    size_t buffer_length;
    unsigned char* buffer_p;

    size_t previous_offset;
    size_t current_offset;
} arena_type;

void arena_init(arena_type* arena_p, void* backing_buffer, size_t backing_buffer_length) {
    arena_p->buffer_p = (unsigned char*)backing_buffer;
    arena_p->buffer_length = backing_buffer_length;
    arena_p->current_offset = 0;
    arena_p->previous_offset = 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void arena_free(arena_type* a, void* ptr) {
    // do nothing. exists for completion purposes
}

#pragma GCC diagnostic pop

void arena_free_all(arena_type* arena_p) {
    arena_p->current_offset = 0;
    arena_p->previous_offset = 0;
}

void* arena_alloc_aligned(arena_type* arena_p, size_t size, size_t alignment) {

    // clang-format off

    // Align `current_offset` forward to the specified alignment

    uintptr_t current_ptr =
        + (uintptr_t) arena_p->buffer_p
        + (uintptr_t) arena_p->current_offset;

    uintptr_t relative_offset = 
        + (uintptr_t) align_forward(current_ptr, alignment)
        - (uintptr_t) arena_p->buffer_p;

    // clang-format on

    // Check to see if the backing memory has space left
    if (relative_offset + size <= arena_p->buffer_length) {

        arena_p->previous_offset = relative_offset;
        arena_p->current_offset = relative_offset + size;

        void* ptr = (unsigned char*)&arena_p->buffer_p[relative_offset];
        memset(ptr, 0, size); // zero memory by default

        return ptr;
    }
    return NULL;
}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void*))
#endif

void* arena_alloc(arena_type* arena_p, size_t size) {
    return arena_alloc_aligned(arena_p, size, DEFAULT_ALIGNMENT);
}

void* arena_resize_aligned(arena_type* arena_p, void* old_memory_p, size_t old_size, size_t new_size, size_t alignment) {
    assert(is_pow2(alignment));

    unsigned char* old_memory_as_uc_p = (unsigned char*)old_memory_p;

    if (old_memory_p == NULL || old_size == 0) {
        return arena_alloc_aligned(arena_p, new_size, alignment);
    } else if (!(arena_p->buffer_p <= old_memory_as_uc_p && old_memory_as_uc_p < arena_p->buffer_p + arena_p->buffer_length)) {
        assert(false && "Memory is out of bounds of the buffer in this arena");
        return NULL;
    }

    // optimization using previous_offset
    if (arena_p->buffer_p + arena_p->previous_offset == old_memory_p) {
        arena_p->current_offset = arena_p->previous_offset + new_size;
        if (new_size > old_size) {
            memset(&arena_p->buffer_p[arena_p->current_offset], 0, new_size - old_size);
        }
        return arena_p;
    }

    void* new_memory = arena_alloc_aligned(arena_p, new_size, alignment);
    size_t copy_size = old_size < new_size ? old_size : new_size;

    // Copy across old memory to the new memory
    memmove(new_memory, old_memory_as_uc_p, copy_size);

    return new_memory;
}

void* arena_resize(arena_type* arena_p, void* old_memory, size_t old_size, size_t new_size) {
    return arena_resize_aligned(arena_p, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);
}
