
#include <stdio.h>

#include "freelist.h"

static unsigned char buf[4096];

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

    /* for (size_t i = 0; i < 'z' - 'a' + 1; i += 2) { */
    /*     freelist_deallocate(&fl, ptrs[i]); */
    /* } */

    freelist_deallocate_all(&fl);
}

int main(void)
{
    chars_test();
}
