
#include "arena.h"
#include "list.h"

#include <stdalign.h>

static unsigned char buf[4096];

typedef struct {
    char c;
    list_node_type node;
} char_elm_type;

void list_stack(void) {
    arena_type arena;
    arena_init(&arena, sizeof(buf), buf);

    list_node_type head;
    list_node_init(&head);

    for (size_t i = 0; i <= 'z' - 'a'; i++) {
        char_elm_type* ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
        ptr->c = 'a' + i;

        list_node_init(&ptr->node);
        list_node_add_after(&ptr->node, &head);
    }


    {
        char c = 'z';
        list_node_type* node_ptr = head.next_ptr;

        while (!list_node_is_first(node_ptr, &head)) {
            list_node_type* next_ptr = node_ptr->next_ptr; // this must be stored, should the list be modified under interation
            char_elm_type* elm = list_node_entry(node_ptr, char_elm_type, node);

            assert(c == elm->c);

            if (c == 'm') {
                char_elm_type* ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
                ptr->c = 'M';

                list_node_init(&ptr->node);
                list_node_replace(node_ptr, &ptr->node);
            }

            c--;
            node_ptr = next_ptr;
        }
    }

    {
        char c = 'z';
        list_node_type* node_ptr = head.next_ptr;
        while (!list_node_is_first(node_ptr, &head)) {
            char_elm_type* elm = list_node_entry(node_ptr, char_elm_type, node);

            assert(elm->c != 'm');
            if (c == 'm') {
                assert(elm->c == 'M');
            }

            c--;
            node_ptr = node_ptr->next_ptr;
        }
    }
}

void list_queue(void) {
    arena_type arena;
    arena_init(&arena, sizeof(buf), buf);

    list_node_type tail;
    list_node_init(&tail);

    char_elm_type* first_elm_ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
    first_elm_ptr->c = 'a';

    list_node_init(&first_elm_ptr->node);
    list_node_add_before(&first_elm_ptr->node, &tail);

    for (size_t i = 1; i <= 'z' - 'a'; i++) {
        char_elm_type* ptr = arena_allocate_aligned(&arena, alignof(char_elm_type), sizeof(char_elm_type));
        ptr->c = 'a' + i;

        list_node_init(&ptr->node);
        list_node_add_before(&ptr->node, &tail);
    }

    assert(list_node_is_first(&first_elm_ptr->node, &tail));

    size_t count = 0;
    {
        char c = 'a';
        list_node_type* node_ptr = &first_elm_ptr->node;

        while (!list_node_is_last(node_ptr, &tail)) {
            list_node_type* next_ptr = node_ptr->next_ptr; // this must be stored, should the list be modified under interation
            char_elm_type* elm = list_node_entry(node_ptr, char_elm_type, node);

            assert(c == elm->c);

            if (c == 'm') {
                list_node_remove(node_ptr);
            }

            c++;
            node_ptr = next_ptr;

            count++;
        }
    }

    size_t new_count = 0;
    {
        list_node_type* node_ptr = &first_elm_ptr->node;

        while (!list_node_is_last(node_ptr, &tail)) {
            node_ptr = node_ptr->next_ptr;

            new_count++;
        }
    }
}

int main(void) {
    list_stack();
    list_queue();
}
