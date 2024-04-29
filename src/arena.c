/*
 * Source:
 * https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */

#include <assert.h>  // assert
#include <stdbool.h> // false
#include <stdint.h>  // uintptr_t
#include <stdlib.h>  // size_t
#include <string.h>  // memset

#include "align_forward.h" // align_forward
#include "arena.h"         // arena_*
#include "is_pow2.h"       // is_pow2

void arena_deallocate(void* a, void* ptr) {
    // do nothing. exists for completion purposes
}

void arena_deallocate_all(void* arena_ptr) {
    arena_type* typed_arena_ptr = (arena_type*)arena_ptr;

    typed_arena_ptr->current_offset = 0;
    typed_arena_ptr->previous_offset = 0;
}

void* arena_allocate_aligned(void* arena_ptr, size_t alignment, size_t size) {
    arena_type* typed_arena_ptr = (arena_type*)arena_ptr;

    // clang-format off

    // Align `current_offset` forward to the specified alignment

    uintptr_t current_ptr =
        + (uintptr_t) typed_arena_ptr->buffer_ptr
        + (uintptr_t) typed_arena_ptr->current_offset;

    uintptr_t relative_offset = 
        + (uintptr_t) align_forward(current_ptr, alignment)
        - (uintptr_t) typed_arena_ptr->buffer_ptr;

    // clang-format on

    // Check to see if the backing memory has space left
    if (relative_offset + size <= typed_arena_ptr->buffer_length) {

        typed_arena_ptr->previous_offset = relative_offset;
        typed_arena_ptr->current_offset = relative_offset + size;

        void* ptr = (unsigned char*)&typed_arena_ptr->buffer_ptr[relative_offset];
        memset(ptr, 0, size); // zero memory by default

        return ptr;
    }
    return NULL;
}

void* arena_reallocate_aligned(void* arena_ptr, void* old_memory_ptr, size_t alignment, size_t old_size, size_t new_size) {
    assert(is_pow2(alignment));

    arena_type* typed_arena_ptr = (arena_type*)arena_ptr;
    unsigned char* typed_old_memory_ptr = (unsigned char*)old_memory_ptr;

    if (old_memory_ptr == NULL || old_size == 0) {
        return arena_allocate_aligned(arena_ptr, alignment, new_size);
    } else if (!(typed_arena_ptr->buffer_ptr <= typed_old_memory_ptr &&
                 typed_old_memory_ptr < typed_arena_ptr->buffer_ptr + typed_arena_ptr->buffer_length)) {
        assert(false && "Memory is out of bounds of the buffer in this arena");
        return NULL;
    }

    // optimization using previous_offset
    if (typed_arena_ptr->buffer_ptr + typed_arena_ptr->previous_offset == old_memory_ptr) {
        typed_arena_ptr->current_offset = typed_arena_ptr->previous_offset + new_size;
        if (new_size > old_size) {
            memset(&typed_arena_ptr->buffer_ptr[typed_arena_ptr->current_offset], 0, new_size - old_size);
        }
        return arena_ptr;
    }

    void* new_memory = arena_allocate_aligned(arena_ptr, alignment, new_size);
    size_t copy_size = old_size < new_size ? old_size : new_size;

    // Copy across old memory to the new memory
    memmove(new_memory, typed_old_memory_ptr, copy_size);

    return new_memory;
}

void arena_init(void* arena_ptr, void* backing_buffer, size_t backing_buffer_length) {
    arena_type* typed_arena_ptr = (arena_type*)arena_ptr;
    typed_arena_ptr->buffer_ptr = (unsigned char*)backing_buffer;
    typed_arena_ptr->buffer_length = backing_buffer_length;
    typed_arena_ptr->current_offset = 0;
    typed_arena_ptr->previous_offset = 0;
    typed_arena_ptr->allocator_ops = (allocator_ops_type){
        .allocate_f_ptr = arena_allocate_aligned, .reallocate_f_ptr = arena_reallocate_aligned, .deallocate_f_ptr = arena_deallocate};
}
