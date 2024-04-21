#include <stdlib.h> // size_t

struct arena_type;
typedef struct arena_type arena_type;

void arena_init(void* arena_p, void* backing_buffer, size_t backing_buffer_length);

void arena_free(void* arena_p, void* ptr);

void arena_free_all(void* arena_p);

void* arena_alloc(void* arena_p, size_t size);

void* arena_alloc_aligned(void* arena_p, size_t alignment, size_t size);

void* arena_resize(void* arena_p, void* old_memory, size_t old_size, size_t new_size);

void* arena_resize_aligned(void* arena_p, void* old_memory, size_t alignment, size_t old_size, size_t new_size);
