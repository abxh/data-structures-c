#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define VT_MOVUP "\033[A"
#define VT_CLEARLINE "\33[2K"
#define VT_MOVTOFRONT "\r"

uint64_t* generate_star_pattern() {
    uint64_t* star_pattern = calloc(8, sizeof(uint64_t));
    srand(time(NULL));
    for (int i = 0; i < 8; i++) {
        for (size_t j = 0; j < 8 * sizeof(uint64_t); j++) {
            uint64_t s = (rand() & 1) && (rand() & 1) && (rand() & 1) && (rand() & 1);
            star_pattern[i] |= s << j;
        }
    }
    return star_pattern;
}

uint64_t rotate_bits_left(uint64_t value, int rotate_bits_by) {
    return (value << rotate_bits_by) | (value >> (64 - rotate_bits_by));
}

uint64_t rotate_bits_right(uint64_t value, int rotate_bits_by) {
    return (value << (64 - rotate_bits_by)) | (value >> rotate_bits_by);
}

int main(void) {
    uint64_t* star_pattern = generate_star_pattern();
    const struct timespec duration = {0., 1. / 25. * 1e+9}; // 1 / 25 seconds
    int8_t rotation_dir = 0;

    while (true) {
        rotation_dir += 2; // we overflow deliberately
        if (rotation_dir >= 0) {
            for (size_t i = 0; i < 8; i++) {
                star_pattern[i] = rotate_bits_right(star_pattern[i], 1);
            }
        } else {
            for (size_t i = 0; i < 8; i++) {
                star_pattern[i] = rotate_bits_left(star_pattern[i], 1);
            }
        }
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j < 8 * sizeof(uint64_t); j++) {
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
