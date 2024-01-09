#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define VT_MOVUP      "\033[A"
#define VT_CLEARLINE  "\33[2K"
#define VT_MOVTOFRONT "\r"

uint64_t *generate_star_pattern() {
    srandom(time(NULL));
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8 * sizeof(uint64_t); j++) {
            unsigned int s = (random() & 1) && (random() & 1) && (random() & 1) && (random() & 1);
            putchar(s ? '*' : ' ');
        }
        putchar('\n');
    }
}

int main(void) {
    uint64_t *star_pattern = generate_star_pattern();

    return 1;

    const struct timespec duration = {0., 1./60.*1e+9};

    while (true) {
        system("clear");
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j < 8 * sizeof(uint64_t); j++) {
                printf("%c", ((star_pattern[i] >> j) & 1) ? '*' : ' ');
            }
            putchar('\n');
        }
        nanosleep(&duration, NULL);
    }
    free(star_pattern);
    return 0;
}
