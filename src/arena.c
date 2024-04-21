#include <assert.h>  // assert
#include <stdbool.h> // false
#include <stdint.h>  // uintptr_t
#include <stdlib.h>  // size_t
#include <string.h>  // memset

#include "align_forward.h"       // arena_forward
#include "allocators/arena.h"    // arena_*
#include "header-only/is_pow2.h" // is_pow2

// Source:
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

void arena_init(void* arena_p, void* backing_buffer, size_t backing_buffer_length) {
    arena_type* t_arena_p = (arena_type*)arena_p;
    t_arena_p->buffer_p = (unsigned char*)backing_buffer;
    t_arena_p->buffer_length = backing_buffer_length;
    t_arena_p->current_offset = 0;
    t_arena_p->previous_offset = 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void arena_deallocate(void* a, void* ptr) {
    // do nothing. exists for completion purposes
}

#pragma GCC diagnostic pop

void arena_deallocate_all(void* arena_p) {
    arena_type* t_arena_p = (arena_type*)arena_p;

    t_arena_p->current_offset = 0;
    t_arena_p->previous_offset = 0;
}

void* arena_allocate(void* arena_p, size_t alignment, size_t size) {
    arena_type* t_arena_p = (arena_type*)arena_p;

    // clang-format off

    // Align `current_offset` forward to the specified alignment

    uintptr_t current_ptr =
        + (uintptr_t) t_arena_p->buffer_p
        + (uintptr_t) t_arena_p->current_offset;

    uintptr_t relative_offset = 
        + (uintptr_t) align_forward(current_ptr, alignment)
        - (uintptr_t) t_arena_p->buffer_p;

    // clang-format on

    // Check to see if the backing memory has space left
    if (relative_offset + size <= t_arena_p->buffer_length) {

        t_arena_p->previous_offset = relative_offset;
        t_arena_p->current_offset = relative_offset + size;

        void* ptr = (unsigned char*)&t_arena_p->buffer_p[relative_offset];
        memset(ptr, 0, size); // zero memory by default

        return ptr;
    }
    return NULL;
}

void* arena_reallocate(void* arena_p, void* old_memory_p, size_t alignment, size_t old_size, size_t new_size) {
    assert(is_pow2(alignment));

    arena_type* t_arena_p = (arena_type*)arena_p;
    unsigned char* t_old_memory_p = (unsigned char*)old_memory_p;

    if (old_memory_p == NULL || old_size == 0) {
        return arena_allocate(arena_p, alignment, new_size);
    } else if (!(t_arena_p->buffer_p <= t_old_memory_p && t_old_memory_p < t_arena_p->buffer_p + t_arena_p->buffer_length)) {
        assert(false && "Memory is out of bounds of the buffer in this arena");
        return NULL;
    }

    // optimization using previous_offset
    if (t_arena_p->buffer_p + t_arena_p->previous_offset == old_memory_p) {
        t_arena_p->current_offset = t_arena_p->previous_offset + new_size;
        if (new_size > old_size) {
            memset(&t_arena_p->buffer_p[t_arena_p->current_offset], 0, new_size - old_size);
        }
        return arena_p;
    }

    void* new_memory = arena_allocate(arena_p, alignment, new_size);
    size_t copy_size = old_size < new_size ? old_size : new_size;

    // Copy across old memory to the new memory
    memmove(new_memory, t_old_memory_p, copy_size);

    return new_memory;
}
