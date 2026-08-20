
#define NAME arena
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "./../../arena/arena_template.h"

#include "container_of.h"
#include "list.h"

#include <stdalign.h>
#include <stdio.h>

static unsigned char buf[4096];

typedef struct {
    char c;
    struct list_node node;
} char_elm_type;

void list_stack(void)
{
    struct arena arena;
    arena_init(&arena, sizeof(buf), buf);

    struct list_node head;
    list_node_init(&head);
    assert(list_node_is_singular(&head));

    {
        char_elm_type *first_elm_ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
        first_elm_ptr->c = 'a';
        list_node_init(&first_elm_ptr->node);
        list_node_add_after(&head, &first_elm_ptr->node);

        for (size_t i = 1; i <= 'z' - 'a'; i++) {
            char_elm_type *ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
            ptr->c = (char)('a' + i);
            list_node_init(&ptr->node);
            list_node_add_after(&head, &ptr->node);
        }

        assert(list_node_is_last(&head, &first_elm_ptr->node));
    }

    {
        char c = 'z';
        struct list_node *node_ptr;
        struct list_node *next_ptr;

        LIST_FOR_EACH_SAFE(node_ptr, next_ptr, &head)
        {
            char_elm_type *elm = container_of_const(node_ptr, char_elm_type, node);
            assert(c == elm->c);
            if (c == 'm') {
                char_elm_type *ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
                ptr->c = 'M';
                list_node_init(&ptr->node);
                list_node_replace(node_ptr, &ptr->node);
            }
            c--;
        }
        assert(c + 1 == 'a');
    }

    {
        char c = 'z';
        struct list_node *node_ptr;

        LIST_FOR_EACH(node_ptr, &head)
        {
            char_elm_type *elm = container_of_const(node_ptr, char_elm_type, node);
            assert(elm->c != 'm');
            if (c == 'm') {
                assert(elm->c == 'M');
            }
            c--;
        }
        assert(c + 1 == 'a');
    }
}

void list_queue(void)
{
    struct arena arena;
    arena_init(&arena, sizeof(buf), buf);

    struct list_node tail;
    list_node_init(&tail);
    assert(list_node_is_singular(&tail));

    {
        char_elm_type *first_elm_ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
        first_elm_ptr->c = 'a';
        list_node_init(&first_elm_ptr->node);
        list_node_add_before(&tail, &first_elm_ptr->node);

        for (size_t i = 1; i <= 'z' - 'a'; i++) {
            char_elm_type *ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
            ptr->c = (char)('a' + i);
            list_node_init(&ptr->node);
            list_node_add_before(&tail, &ptr->node);
        }
        assert(list_node_is_first(&tail, &first_elm_ptr->node));
    }

    size_t count = 0;
    {
        char c = 'a';
        struct list_node *node_ptr;
        struct list_node *prev_ptr;
        struct list_node *next_ptr;

        LIST_FOR_EACH_SAFE(node_ptr, next_ptr, &tail)
        {
            char_elm_type *elm = container_of_const(node_ptr, char_elm_type, node);
            assert(c == elm->c);
            c++;
        }

        c--;
        assert(c == 'z');

        LIST_FOR_EACH_REVERSE_SAFE(node_ptr, prev_ptr, &tail)
        {
            char_elm_type *elm = container_of_const(node_ptr, char_elm_type, node);
            assert(c == elm->c);
            if (c == 'm') {
                list_node_remove(node_ptr);
            }
            c--;
            count++;
        }
    }

    size_t new_count = 0;
    {
        struct list_node *node_ptr;

        LIST_FOR_EACH_REVERSE(node_ptr, &tail)
        {
            new_count++;
        }
    }
    assert(count - 1 == new_count);
}

int main(void)
{
    list_stack();
    list_queue();
}
