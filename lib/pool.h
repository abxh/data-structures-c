// Source used:
// https://www.gingerbill.org/article/2019/02/16/memory-allocation-strategies-004/

#include "align.h" // align, calc_alignment_padding

#include <stdalign.h>
#include <stddef.h>
#include <string.h>

struct pool_free_node {
    struct pool_free_node *next_ptr;
};

struct pool {
    unsigned char *buf_ptr;
    size_t buf_len;
    size_t chunk_size;

    struct pool_free_node *head;
};

static inline void pool_deallocate_all(struct pool *self)
{
    assert(self);

    const size_t chunk_count = self->buf_len / self->chunk_size;

    for (size_t i = 0; i < chunk_count; i++) {
        struct pool_free_node* node = (struct pool_free_node*)&self->buf_ptr[i * self->chunk_size];
        node->next_ptr = self->head;
        self->head = node;
    }
}

static inline void pool_init(struct pool *self, const size_t len, unsigned char *backing_buf, const size_t chunk_size_,
                             const size_t chunk_alignment)
{
    assert(self);
    assert(backing_buf);

    const uintptr_t padding = calc_alignment_padding(chunk_alignment, (uintptr_t)backing_buf);

    size_t chunk_size = chunk_size_ >= sizeof(struct pool_free_node) ? chunk_size_ : sizeof(struct pool_free_node);
    chunk_size = chunk_size + calc_alignment_padding(chunk_alignment, chunk_size);

    assert(padding + chunk_size <= len);

    self->buf_ptr = &backing_buf[padding];
    self->buf_len = len - padding;
    self->chunk_size = chunk_size;
    self->head = NULL;

    pool_deallocate_all(self);
}

static inline void *pool_allocate(struct pool *self)
{
    struct pool_free_node *node = self->head;

    if (node == NULL) {
        return NULL;
    }

    self->head = node->next_ptr;

    memset(node, 0, self->chunk_size);

    return node;
}

static inline void pool_deallocate(struct pool *self, void *ptr)
{
    assert(self);
    assert(ptr);
    assert(self->buf_ptr <= (unsigned char *)ptr && (unsigned char *)ptr < &self->buf_ptr[self->buf_len]);

    struct pool_free_node *node = (struct pool_free_node *)ptr;
    node->next_ptr = self->head;
    self->head = node;
}
