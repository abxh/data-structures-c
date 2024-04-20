#include <stdlib.h> // size_t

struct arena_type;
typedef struct arena_type arena_type;

void arena_init(arena_type* arena_p, void* backing_buffer, size_t backing_buffer_length);

void arena_free(arena_type* arena_p, void* ptr);

void arena_free_all(arena_type* arena_p);

void* arena_alloc(arena_type* arena_p, size_t size);

void* arena_alloc_aligned(arena_type* arena_p, size_t size, size_t alignment);

void* arena_resize(arena_type* arena_p, void* old_memory, size_t old_size, size_t new_size);

void* arena_resize_aligned(arena_type* arena_p, void* old_memory, size_t old_size, size_t new_size, size_t alignment);
