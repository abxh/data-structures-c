
#include "arena.h"
#include "list.h"

#include <stdalign.h>

typedef struct {
    char c;
    struct list_node node;
} con_type;

void list_stack(void)
{
    unsigned char *buf = malloc(4096);

    struct arena arena;
    arena_init(&arena, 4096, buf);

    struct list_node head;
    list_node_init(&head);

    for (size_t i = 0; i <= 'z' - 'a'; i++) {
        con_type *ptr = arena_allocate_aligned(&arena, alignof(con_type), sizeof(con_type));
        if (!ptr) {
            assert(false);
        }
        ptr->c = 'a' + (char)i;

        list_node_init(&ptr->node);
        list_node_add_after(&ptr->node, &head);
    }

    {
        char c = 'z';
        struct list_node *node_ptr = head.next_ptr;

        while (!list_node_is_head(node_ptr, &head)) {
            struct list_node *next_ptr = node_ptr->next_ptr; // this must be stored, should the list be modified under
                                                             // iteration (not in this particular case)
            con_type *elm = list_node_entry(node_ptr, con_type, node);

            assert(c == elm->c);

            if (c == 'm') {
                con_type *ptr = arena_allocate_aligned(&arena, alignof(con_type), sizeof(con_type));
                if (!ptr) {
                    assert(false);
                }
                ptr->c = 'M';

                list_node_init(&ptr->node);
                list_node_replace(node_ptr, &ptr->node);
            }

            c--;
            node_ptr = next_ptr;
        }
        assert(c + 1 == 'a');
    }

    {
        char c = 'z';
        struct list_node *node_ptr = head.next_ptr;
        while (!list_node_is_head(node_ptr, &head)) {
            con_type *elm = list_node_entry(node_ptr, con_type, node);

            if (c == 'm') {
                assert(elm->c == 'M');
            }
            else {
                assert(elm->c == c);
            }

            c--;
            node_ptr = node_ptr->next_ptr;
        }
        assert(c + 1 == 'a');
    }

    {
        char c = 'z';
        while (!list_node_is_head(head.next_ptr, &head)) {
            struct list_node *node_ptr = list_node_remove(head.next_ptr);
            con_type *elm = list_node_entry(node_ptr, con_type, node);

            if (c == 'm') {
                assert(elm->c == 'M');
            }
            else {
                assert(elm->c == c);
            }

            c--;
        }
        assert(c + 1 == 'a');
    }

    free(buf);
}

void list_queue(void)
{
    unsigned char *buf = malloc(4096);

    struct arena arena;
    arena_init(&arena, 4096, buf);

    struct list_node tail;
    list_node_init(&tail);

    con_type *first_elm_ptr = arena_allocate_aligned(&arena, alignof(con_type), sizeof(con_type));
    if (!first_elm_ptr) {
        assert(false);
    }
    first_elm_ptr->c = 'a';

    list_node_init(&first_elm_ptr->node);
    list_node_add_before(&first_elm_ptr->node, &tail);

    for (size_t i = 1; i <= 'z' - 'a'; i++) {
        con_type *ptr = arena_allocate_aligned(&arena, alignof(con_type), sizeof(con_type));
        if (!ptr) {
            assert(false);
        }
        ptr->c = 'a' + (char)i;

        list_node_init(&ptr->node);
        list_node_add_before(&ptr->node, &tail);
    }

    assert(list_node_is_first(&first_elm_ptr->node, &tail));

    size_t count = 0;
    {
        char c = 'a';
        struct list_node *node_ptr = &first_elm_ptr->node;

        while (!list_node_is_tail(node_ptr, &tail)) {
            struct list_node *next_ptr =
                node_ptr->next_ptr; // this must be stored, should the list be modified under iteration
            con_type *elm = list_node_entry(node_ptr, con_type, node);

            assert(c == elm->c);

            if (c == 'm') {
                list_node_remove(node_ptr);
            }

            c++;
            node_ptr = next_ptr;

            count++;
        }
    }

    {
        size_t new_count = 0;
        struct list_node *node_ptr = &first_elm_ptr->node;
        while (!list_node_is_tail(node_ptr, &tail)) {
            node_ptr = node_ptr->next_ptr;
            new_count++;
        }
        assert(count - 1 == new_count);
    }

    {
        char c = 'z';

        while (!list_node_is_tail(tail.prev_ptr, &tail)) {
            struct list_node *node_ptr = list_node_remove(tail.prev_ptr);
            con_type *elm = list_node_entry(node_ptr, con_type, node);

            if (c == 'm') {
                c--;
            }
            assert(c == elm->c);
            c--;
        }
        assert(c + 1 == 'a');
    }

    free(buf);
}

int main(void)
{
    list_stack();
    list_queue();
}
