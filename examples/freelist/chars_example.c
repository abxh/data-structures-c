
#include <assert.h>
#include <stdio.h>

#include "freelist.h"

static void traverse_freelist_header_and_print(struct freelist_header *head, struct freelist *fl,
                                               const size_t prev_size, char *marked_ptr, const int freetree_node_count)
{
    assert(fl);

    if (!head) {
        assert(freetree_node_count == 0);
        return;
    }
    assert(prev_size == freelist_header_prev_size(head));

    const bool fl_is_marked = marked_ptr != NULL && head == (void *)(marked_ptr - sizeof(struct freelist_header));

    printf(" -> %s%s %zu", fl_is_marked ? "*" : "", freelist_header_is_freed(head) ? "[freed]" : "",
           head->curr_block_size);

    traverse_freelist_header_and_print(freelist_header_next(head, fl), fl, head->curr_block_size, marked_ptr,
                                       freetree_node_count - (freelist_header_should_be_in_freetree(head) ? 1 : 0));
}

static int count_freetree_nodes(const struct freetree_node *root_ptr)
{
    if (root_ptr == NULL) {
        return 0;
    }
    return 1 + count_freetree_nodes(root_ptr->left_ptr) + count_freetree_nodes(root_ptr->right_ptr);
}

static void preorder_traverse_freetree_and_print(const struct freetree_node *root_ptr, bool print_children)
{
    if (root_ptr == NULL) {
        if (print_children) {
            printf(" ()");
        }
        return;
    }
    printf(" ( %zu", root_ptr->key.curr_block_size);
    bool dont_print_children = root_ptr->left_ptr == NULL && root_ptr->right_ptr == NULL;
    preorder_traverse_freetree_and_print(root_ptr->left_ptr, !dont_print_children);
    preorder_traverse_freetree_and_print(root_ptr->right_ptr, !dont_print_children);
    printf(" )");
}
static unsigned char buf[4096];

extern void chars_example(void)
{
    (void)(traverse_freelist_header_and_print);
    (void)(count_freetree_nodes);
    (void)(preorder_traverse_freetree_and_print);

    struct freelist fl;
    freelist_init(&fl, 4096, buf);

    char *ptrs['z' - 'a' + 1] = {0};

    for (size_t i = 0; i < 'z' - 'a' + 1; i++) {
        ptrs[i] = freelist_allocate(&fl, i + 2);

        for (size_t j = 0; j < i + 1; j++) {
            ptrs[i][j] = 'a' + (char)j;
        }
        ptrs[i][i + 1] = '\0';
    }

    for (size_t i = 0; i < 'z' - 'a' + 1; i++) {
        for (size_t j = 0; j < i + 1; j++) {
            assert(ptrs[i][j] == 'a' + (char)j);
        }
    }

    for (size_t i = 0; i < 'z' - 'a' + 1; i++) {
        if (!(i % 3 == 0 || i % 4 == 0)) {
            continue;
        }

        /*
        printf("%zu\n", i);
        traverse_freelist_header_and_print(fl.head, &fl, 0, ptrs[i], (int)count_freetree_nodes(fl.rb_rootptr));
        printf("\n");
        printf("%zu (tree):\n", i);
        preorder_traverse_freetree_and_print(fl.rb_rootptr, false);
        printf("\n");
        */

        freelist_deallocate(&fl, ptrs[i]);
    }

    for (size_t i = 0; i < 'z' - 'a' + 1; i++) {
        if (!(i % 3 == 0 || i % 4 == 0)) {
            continue;
        }
        ptrs[i] = freelist_allocate(&fl, i + 2);

        for (size_t j = 0; j < i + 1; j++) {
            ptrs[i][j] = 'a' + (char)j;
        }
        ptrs[i][i + 1] = '\0';
    }

    for (size_t i = 0; i < 'z' - 'a' + 1; i++) {
        for (size_t j = 0; j < i + 1; j++) {
            assert(ptrs[i][j] == 'a' + (char)j);
        }
    }

    for (size_t i = 0; i < 'z' - 'a' + 1; i++) {
        /*
        printf("%zu\n", i);
        traverse_freelist_header_and_print(fl.head, &fl, 0, ptrs[i], (int)count_freetree_nodes(fl.rb_rootptr));
        printf("\n");
        printf("%zu (tree):\n", i);
        preorder_traverse_freetree_and_print(fl.rb_rootptr, false);
        printf("\n");
        */

        freelist_deallocate(&fl, ptrs[i]);
    }

    /*
    printf("end:\n");
    traverse_freelist_header_and_print(fl.head, &fl, 0, NULL, (int)count_freetree_nodes(fl.rb_rootptr));
    printf("\n");
    printf("end (tree):\n");
    preorder_traverse_freetree_and_print(fl.rb_rootptr, false);
    printf("\n");

    printf("%zu\n", alignof(max_align_t));
    */

    freelist_deallocate_all(&fl);
}

