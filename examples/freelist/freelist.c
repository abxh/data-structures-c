
#include <stdio.h>

#include "freelist.h"

static unsigned char buf[4096];

static int count_nodes(const internal_freelist_node_type* root_ptr)
{
    if (root_ptr == NULL) {
        return 0;
    }
    return 1 + count_nodes(root_ptr->left_ptr) + count_nodes(root_ptr->right_ptr);
}

static void preorder_traverse_and_print(const internal_freelist_node_type* root_ptr, bool print_children)
{
    if (root_ptr == NULL) {
        if (print_children) {
            printf(" ()");
        }
        return;
    }
    printf(" ( %zu", root_ptr->key);
    bool dont_print_children = root_ptr->left_ptr == NULL && root_ptr->right_ptr == NULL;
    preorder_traverse_and_print(root_ptr->left_ptr, !dont_print_children);
    preorder_traverse_and_print(root_ptr->right_ptr, !dont_print_children);
    printf(" )");
}

void chars_test(void)
{
    freelist_type fl;
    freelist_init(&fl, 4096, buf);

    char* ptrs[('z' - 'a' + 1) * 3] = {0};
    size_t n = 0;

    for (size_t i = 0; i < 'z' - 'a' + 1; i++) {
        ptrs[i] = freelist_allocate(&fl, i + 2);

        for (size_t j = 0; j < i + 1; j++) {
            ptrs[i][j] = 'a' + (char)j;
        }
        ptrs[i][i + 1] = '\0';
        n++;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < i + 1; j++) {
            assert(ptrs[i][j] == 'a' + (char)j);
        }
    }

    for (size_t i = 0; i < 'z' - 'a'; i++) {
        freelist_deallocate(&fl, ptrs[i]);
    }
    
    preorder_traverse_and_print(fl.rb_rootptr, true);
    printf("\n");

    freelist_deallocate(&fl, ptrs[n - 1]);

    preorder_traverse_and_print(fl.rb_rootptr, true);
    printf("\n");

    printf("%d\n", count_nodes(fl.rb_rootptr));
    printf("root block_size: %zu\n", fl.rb_rootptr->key);
    printf("left block_size: %zu\n", !fl.rb_rootptr->left_ptr ? 0 : fl.rb_rootptr->left_ptr->key);
    printf("right block_size: %zu\n", !fl.rb_rootptr->right_ptr ? 0 : fl.rb_rootptr->right_ptr->key);

    freelist_deallocate_all(&fl);
}

int main(void)
{
    chars_test();
}
