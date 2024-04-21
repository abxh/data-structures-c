#pragma once

#include <stdlib.h> // size_t

typedef struct arena_type {
    size_t buffer_length;
    unsigned char* buffer_p;

    size_t previous_offset;
    size_t current_offset;
} arena_type;

void arena_init(void* arena_p, void* backing_buffer, size_t backing_buffer_length);

void* arena_allocate(void* arena_p, size_t alignment, size_t size);

void* arena_reallocate(void* arena_p, void* old_memory, size_t alignment, size_t old_size, size_t new_size);

void arena_deallocate(void* arena_p, void* ptr);

void arena_deallocate_all(void* arena_p);
