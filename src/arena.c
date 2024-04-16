
#include "allocators/arena.h" // arena_*
#include <stdlib.h>           // size_t

void* arena_alloc(arena_type* arena_p, size_t size, size_t alignment) {
    if (arena_p->arena_offset + size <= arena_p->arena_buffer_length) {
        void* ptr = &arena_p[arena_p->arena_offset];
        arena_p->arena_offset += size;
        return ptr;
    }
    return NULL;
}


