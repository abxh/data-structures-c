#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../../src/bithacks/rotate_bits.h"

#define VT_MOVUP "\033[A"
#define VT_CLEARLINE "\33[2K"
#define VT_MOVTOFRONT "\r"

size_t* generate_star_pattern() {
    size_t* star_pattern = calloc(8, sizeof(size_t));
    srand(time(NULL));
    for (int i = 0; i < 8; i++) {
        for (size_t j = 0; j < 8 * sizeof(*star_pattern); j++) {
            size_t s = (rand() & 1) && (rand() & 1) && (rand() & 1) && (rand() & 1);
            star_pattern[i] |= s << j;
        }
    }
    return star_pattern;
}

int main(void) {
    size_t* star_pattern = generate_star_pattern();
    const struct timespec duration = {0., 1. / 25. * 1e+9}; // 1 / 25 seconds
    int8_t rotation_dir = 0;

    while (true) {
        rotation_dir += 2; // we overflow deliberately
        for (size_t i = 0; i < 8; i++) {
            star_pattern[i] = rotate_bits(star_pattern[i], (rotation_dir >= 0) - (rotation_dir <= 0));
        }
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j < 8 * sizeof(*star_pattern); j++) {
                printf("%c", ((star_pattern[i] >> j) & 1) ? '*' : ' ');
            }
            putchar('\n');
        }
        nanosleep(&duration, NULL);
        for (int i = 0; i < 8; i++) {
            printf(VT_CLEARLINE);
            printf(VT_MOVUP);
        }
        printf(VT_MOVTOFRONT);
    }
    free(star_pattern);
    return 0;
}
