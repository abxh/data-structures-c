/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 15
    - N := 8192

    Non-mutating operation types / properties:
    - node_parent_ptr
    - node_is_black
    - node_is_red
    - is_empty
    - contains_key
    - get_value / get_value_mut
    - search_node

    Mutating operation types:
    - init
    - node_init
    - insert_node
    - delete_node

    Sources used for testing:
    http://www.maxgcoding.com/validating-red-black-trees/
*/

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NAME                       bst
#define KEY_TYPE                   int
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#include "rbtree.h"

typedef struct {
    unsigned int value;
    bst_node_type node;
} extended_bst_node;

static int max_int(const int a, const int b)
{
    return (a >= b) ? a : b;
}

static int count_height(const bst_node_type* root_ptr)
{
    if (root_ptr == NULL) {
        return 0;
    }
    int height = 0;
    if (root_ptr->left_ptr) {
        height = 1 + count_height(root_ptr->left_ptr);
    }
    if (root_ptr->right_ptr) {
        height = max_int(height, 1 + count_height(root_ptr->right_ptr));
    }
    return height;
}

static bool is_valid_binary_search_tree(const float left_key, const bst_node_type* root_ptr, const float right_key)
{
    if (root_ptr == NULL) {
        return true;
    }
    if (!(left_key < root_ptr->key && root_ptr->key < right_key)) {
        return false;
    }
    return is_valid_binary_search_tree(left_key, root_ptr->left_ptr, root_ptr->key) &&
           is_valid_binary_search_tree(root_ptr->key, root_ptr->right_ptr, right_key);
}

static bool rbtree_check_for_no_consecutive_reds(const bst_node_type* parent_ptr, const bst_node_type* root_ptr)
{
    if (root_ptr == NULL) {
        return true;
    }
    if (parent_ptr && bst_node_is_red(parent_ptr) && bst_node_is_red(root_ptr)) {
        return false;
    }
    return rbtree_check_for_no_consecutive_reds(root_ptr, root_ptr->left_ptr) &&
           rbtree_check_for_no_consecutive_reds(root_ptr, root_ptr->right_ptr);
}

static int rbtree_get_black_height_of_some_path(const bst_node_type* root_ptr)
{
    if (root_ptr == NULL) {
        return 1;
    }
    return bst_node_is_black(root_ptr) + rbtree_get_black_height_of_some_path(root_ptr->left_ptr);
}

static bool rbtree_check_equal_black_height(const bst_node_type* root_ptr, const int height)
{
    if (root_ptr == NULL) {
        return height == 1;
    }
    const bool is_black = bst_node_is_black(root_ptr);

    return rbtree_check_equal_black_height(root_ptr->left_ptr, height - is_black) &&
           rbtree_check_equal_black_height(root_ptr->right_ptr, height - is_black);
}

static bool is_valid_red_black_tree(const bst_node_type* root_ptr)
{
    const bool is_valid_bst = is_valid_binary_search_tree(-INFINITY, root_ptr, INFINITY);
    const bool is_valid_234 = rbtree_check_for_no_consecutive_reds(NULL, root_ptr);
    const bool balanced_black_height = rbtree_check_equal_black_height(root_ptr, rbtree_get_black_height_of_some_path(root_ptr));

    return is_valid_bst && is_valid_234 && balanced_black_height;
}

static void preorder_traverse_and_print(const bst_node_type* root_ptr, bool print_children)
{
    if (root_ptr == NULL) {
        if (print_children) {
            printf(" ()");
        }
        return;
    }
    if (bst_node_is_red(root_ptr)) {
        printf(" ( [%d]", root_ptr->key);
    }
    else {
        printf(" ( %d", root_ptr->key);
    }
    bool dont_print_children = root_ptr->left_ptr == NULL && root_ptr->right_ptr == NULL;
    preorder_traverse_and_print(root_ptr->left_ptr, !dont_print_children);
    preorder_traverse_and_print(root_ptr->right_ptr, !dont_print_children);
    printf(" )");
}

int main(void)
{
    (void)(preorder_traverse_and_print);

    // N = 0
    {
        bst_node_type* bst;
        bst_init(&bst);
        assert(is_valid_red_black_tree(bst));
    }
    // N = 1
    {
        bst_node_type* bst;
        bst_init(&bst);

        assert(!bst_contains_key(&bst, 42));
        assert(!bst_contains_key(&bst, 69));

        assert(bst == NULL);
        assert(bst_is_empty(&bst));

        assert(is_valid_red_black_tree(bst));
    }
    // N = 1, insert_node
    {
        bst_node_type* bst;
        bst_init(&bst);

        bst_node_type node;

        bst_node_init(&node, 42);
        bst_insert_node(&bst, &node);

        assert(bst_contains_key(&bst, 42));
        assert(!bst_contains_key(&bst, 69));

        assert(bst == &node);
        assert(!bst_is_empty(&bst));

        assert(is_valid_red_black_tree(bst));
    }

    // N = 15, insert_node * 8, delete_node * 4, insert_node * 11
    {
        bst_node_type* bst;
        bst_init(&bst);

        bst_node_type node[8];
        const int values[8] = {3, 1, 5, 2, 4, 7, 6, 8};
        for (int i = 0; i < 8; i++) {
            bst_node_init(&node[i], values[i]);
            bst_insert_node(&bst, &node[i]);

            assert(is_valid_red_black_tree(bst));
        }

        const int deleted_values[4] = {3, 2, 4, 8};
        for (int i = 0; i < 4; i++) {
            bst_delete_node(&bst, bst_search_node(&bst, deleted_values[i]));

            assert(is_valid_red_black_tree(bst));
        }

        bst_node_type new_node[11];
        const int new_values[11] = {4, 12, 9, 10, 11, 16, 13, 14, 17, 15, 8};
        for (int i = 0; i < 11; i++) {
            bst_node_init(&new_node[i], new_values[i]);
            bst_insert_node(&bst, &new_node[i]);

            assert(is_valid_red_black_tree(bst));
        }
        assert(count_height(bst) == 4);
    }

    // N = 8192, 8 * (insert_node * 1024, delete_node * 1024, insert_node * 1024)
    const int lim = 1024;
    for (int seed = 0; seed < 8; seed++) {
        bst_node_type* bst;
        bst_init(&bst);

        srand(seed);

        extended_bst_node* node_buf = calloc(lim * 2, sizeof(extended_bst_node));
        size_t node_count = 0;

        for (int i = 0; i < lim; i++) {
            const int val = ((unsigned int)rand()) % lim;

            bst_node_type* node_ptr = bst_search_node(&bst, val);

            if (!node_ptr) {
                node_buf[node_count].value = 0;
                bst_node_init(&node_buf[node_count].node, val);
                bst_insert_node(&bst, &node_buf[node_count].node);
                node_count++;
            }
            else {
                rbtree_node_entry(node_ptr, extended_bst_node, node)->value++;
            }

            assert(is_valid_red_black_tree(bst));
        }

        assert(count_height(bst) <= 2 * log2(lim));

        for (int i = 0; i < lim; i++) {
            int val = ((unsigned int)rand()) % lim;
            bst_node_type* node_ptr = bst_search_node(&bst, val);
            if (node_ptr) {
                unsigned int* value_ptr = &rbtree_node_entry(node_ptr, extended_bst_node, node)->value;
                if (*value_ptr > 1) {
                    (*value_ptr)--;
                }
                else {
                    bst_delete_node(&bst, node_ptr);
                }
            }
            assert(is_valid_red_black_tree(bst));
        }

        for (int i = 0; i < lim; i++) {
            int val = ((unsigned int)rand()) % lim;

            bst_node_type* node_ptr = bst_search_node(&bst, val);

            if (!node_ptr) {
                node_buf[node_count].value = 0;
                bst_node_init(&node_buf[node_count].node, val);
                bst_insert_node(&bst, &node_buf[node_count].node);
                node_count++;
            }
            else {
                rbtree_node_entry(node_ptr, extended_bst_node, node)->value++;
            }
            assert(is_valid_red_black_tree(bst));
        }

        free(node_buf);
    }
}
