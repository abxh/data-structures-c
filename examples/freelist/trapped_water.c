/* Not my code. Following is used as an example. */

#include "freelist.h"

static unsigned char global_freelist_buf[128];

#define calloc(n, size)                                                                    \
    __extension__({                                                                        \
        void *p = freelist_allocate((struct freelist *)global_freelist_buf, (n) * (size)); \
        memset(p, 0, (n) * (size));                                                        \
        p;                                                                                 \
    })

#define free(ptr) freelist_deallocate((struct freelist *)global_freelist_buf, (ptr))

static int trap(const int *height, const int n)
{
    int trapped = 0;
    int *buffer = (int *)calloc(n, sizeof(int));

    int max_height = -1;
    for (int i = 0; i < n; ++i) {
        const int x = buffer[i] = height[i];
        if (x > max_height) {
            max_height = x;
        }
    }

    for (int h = 0; h < max_height; ++h) {
        int first_index = -1;
        int last_index = -1;

        for (int i = 0; i < n; ++i) {
            if (buffer[i] > 0) {
                first_index = i;
                i = n + 1;
            }
        }
        for (int i = n - 1; i >= 0; --i) {
            if (buffer[i] > 0) {
                last_index = i;
                i = -1;
            }
        }

        if (first_index == last_index) {
            h = max_height + 1;
        }
        else {
            for (int i = first_index; i <= last_index; ++i) {
                if (buffer[i] == 0) {
                    ++trapped;
                }
            }
            for (int i = 0; i < n; ++i) {
                const int x = buffer[i];
                if (x > 0) {
                    buffer[i] = (x - 1);
                }
            }
        }
    }

    free(buffer);

    return trapped;
}

void trapped_water()
{
    freelist_init((struct freelist *)global_freelist_buf, sizeof(global_freelist_buf) - sizeof(struct freelist),
                  &global_freelist_buf[sizeof(struct freelist)]);

    assert(((struct freelist *)global_freelist_buf)->buf_used == 0);

    const int a0[] = {0, 1, 0, 1, 0};
    const int a1[] = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    const int a2[] = {4, 2, 0, 3, 2, 5};
    const int a3[] = {0, 1, 0};
    const int a4[] = {0, 1, 0, 1};

    const int x0 = trap(a0, 5);
    assert(x0 == 1);
    const int x1 = trap(a1, 12);
    assert(x1 == 6);
    const int x2 = trap(a2, 6);
    assert(x2 == 9);
    const int x3 = trap(a3, 3);
    assert(x3 == 0);
    const int x4 = trap(a4, 4);
    assert(x4 == 1);

    assert(((struct freelist *)global_freelist_buf)->buf_used == 0);
}
