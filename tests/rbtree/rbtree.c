/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 15
    - N := 4096

    Non-mutating operation types / properties:
    - .count
    - .root_ptr
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
    - clear

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
#define VALUE_TYPE                 int
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#include "rbtree.h"

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

static bool is_valid_binary_search_tree(const bst_node_type* root_ptr)
{
    // no duplicates allowed (however the value can be used as a counter)

    if (root_ptr == NULL) {
        return true;
    }
    if (root_ptr->left_ptr && root_ptr->left_ptr->key > root_ptr->key) {
        return false;
    }
    if (root_ptr->right_ptr && root_ptr->key > root_ptr->right_ptr->key) {
        return false;
    }
    return is_valid_binary_search_tree(root_ptr->left_ptr) && is_valid_binary_search_tree(root_ptr->right_ptr);
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
    const bool is_valid_bst = is_valid_binary_search_tree(root_ptr);
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
        bst_type bst;
        bst_init(&bst);
        assert(is_valid_red_black_tree(bst.root_ptr));
        assert(bst.count == 0);
    }
    // N = 1
    {
        bst_type bst;
        bst_init(&bst);

        assert(!bst_contains_key(&bst, 42));
        assert(!bst_contains_key(&bst, 69));
        assert(bst_get_value(&bst, 42, -1) == -1);
        assert(bst_get_value(&bst, 69, -1) == -1);
        assert(bst_get_value_mut(&bst, 42) == NULL);
        assert(bst_get_value_mut(&bst, 69) == NULL);

        assert(bst.root_ptr == NULL);
        assert(bst_is_empty(&bst));

        assert(is_valid_red_black_tree(bst.root_ptr));
        assert(bst.count == 0);
    }
    // N = 1, insert_node
    {
        bst_type bst;
        bst_init(&bst);

        bst_node_type node;

        assert(bst.count == 0);
        bst_node_init(&node, 42, 69);
        bst_insert_node(&bst, &node);

        assert(bst_contains_key(&bst, 42));
        assert(!bst_contains_key(&bst, 69));
        assert(bst_get_value(&bst, 42, -1) == 69);
        assert(bst_get_value(&bst, 69, -1) == -1);
        assert(*bst_get_value_mut(&bst, 42) == 69);
        assert(bst_get_value_mut(&bst, 69) == NULL);

        assert(bst.root_ptr == &node);
        assert(!bst_is_empty(&bst));

        assert(is_valid_red_black_tree(bst.root_ptr));
        assert(bst.count == 1);
    }

    // N = 15, insert_node * 8, delete_node * 4, insert_node * 11
    {
        bst_type bst;
        bst_init(&bst);

        bst_node_type node[8];
        const int values[8] = {3, 1, 5, 2, 4, 7, 6, 8};
        for (int i = 0; i < 8; i++) {
            bst_node_init(&node[i], values[i], values[i]);
            bst_insert_node(&bst, &node[i]);

            assert(is_valid_red_black_tree(bst.root_ptr));
        }
        assert(bst.count == 8);

        const int deleted_values[4] = {3, 2, 4, 8};
        for (int i = 0; i < 4; i++) {
            bst_delete_node(&bst, bst_search_node(&bst, deleted_values[i]));

            assert(is_valid_red_black_tree(bst.root_ptr));
        }
        assert(bst.count == 4);

        bst_node_type new_node[11];
        const int new_values[11] = {4, 12, 9, 10, 11, 16, 13, 14, 17, 15, 8};
        for (int i = 0; i < 11; i++) {
            bst_node_init(&new_node[i], new_values[i], new_values[i]);
            bst_insert_node(&bst, &new_node[i]);

            assert(is_valid_red_black_tree(bst.root_ptr));
        }
        assert(bst.count == 15);
        assert(count_height(bst.root_ptr) == 4);
    }

    // N = 4096, 4 * (insert_node * 1024, delete_node * 1024, insert_node * 1024)

    const int lim = 1024;
    for (int seed = 0; seed < 4; seed++) {
        bst_type bst;
        bst_init(&bst);

        srand(seed);

        bst_node_type* node_buf = calloc(lim * 2, sizeof(bst_node_type));
        size_t count = 0;

        for (int i = 0; i < lim; i++) {
            const int val = ((unsigned int)rand()) % lim;

            bst_node_type* node_ptr = bst_search_node(&bst, val);

            if (!node_ptr) {
                bst_node_init(&node_buf[count], val, 0);
                bst_insert_node(&bst, &node_buf[count]);
                count++;
            }
            else {
                node_ptr->value++;
            }
            assert(is_valid_red_black_tree(bst.root_ptr));
        }

        assert(count_height(bst.root_ptr) <= 2 * log2(lim));

        for (int i = 0; i < lim; i++) {
            int val = ((unsigned int)rand()) % lim;
            bst_node_type* node_ptr = bst_search_node(&bst, val);
            if (node_ptr) {
                if (node_ptr->value > 1) {
                    node_ptr->value--;
                }
                else {
                    bst_delete_node(&bst, node_ptr);
                }
            }
            assert(is_valid_red_black_tree(bst.root_ptr));
        }

        for (int i = 0; i < lim; i++) {
            int val = ((unsigned int)rand()) % lim;
            bst_node_type* node_ptr = bst_search_node(&bst, val);
            if (!node_ptr) {
                bst_node_init(&node_buf[count], val, 0);
                bst_insert_node(&bst, &node_buf[count]);
                count++;
            }
            else {
                node_ptr->value++;
            }
            assert(is_valid_red_black_tree(bst.root_ptr));
        }

        free(node_buf);
    }
}
