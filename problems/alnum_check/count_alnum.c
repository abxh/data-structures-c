#include <ctype.h>
#include <stdio.h>
#include "bitarray.h"

#define NUM_OF_LETTERS ('z' - 'a' + 1)
#define NUM_OF_DIGITS  ('9' - '0' + 1)

int set_on(int value) {
    return value | 1; // or just 1.
}

int main(void) {
    Bitarray *bitarray_p = ba_new(8U); // 8 bit/word * 8 words = 64 > 26 letters * 2 + 10 digits

    puts("Input (Ctrl+d for EOF):");
    for (int c; c != EOF; c = fgetc(stdin)) {
        if (islower(c)) {
            ba_update_at(bitarray_p, c - 'a', set_on);
        }
        else if (isupper(c)) {
            ba_update_at(bitarray_p, c - 'A' + NUM_OF_LETTERS, set_on);
        }
        else if (isdigit(c)) {
            ba_update_at(bitarray_p, c - '0' + 2 * NUM_OF_LETTERS, set_on);
        }
    }
    
    putchar('\n');
    printf("Bit array:\n");
    ba_print(bitarray_p);
    putchar('\n');

    size_t i;
    for (i = 0; i < bitarray_p->num_of_words * 8; i++) {
        if (i >= 2 * NUM_OF_LETTERS + NUM_OF_DIGITS) {
            putchar('.');
            break;
        }
        if (((i + 1) & 0b111) != 0b001) { // (i + 1) % 8 != 1
            printf(", ");
        }
        if (i < NUM_OF_LETTERS) {
            printf("%c:%c", (int) i + 'a', ba_get(bitarray_p, i) ? '*' : ' ');
        } else if (i < 2 * NUM_OF_LETTERS) {
            printf("%c:%c", (int) (i - 1 * NUM_OF_LETTERS) + 'A', ba_get(bitarray_p, i) ? '*' : ' ');
        } else if (i < 2 * NUM_OF_LETTERS + NUM_OF_DIGITS) {
            printf("%c:%c", (int) (i - 2 * NUM_OF_LETTERS) + '0', ba_get(bitarray_p, i) ? '*' : ' ');
        }
        if (((i + 1) & 0b1111) == 0b1000 || ((i + 1) & 0b1111) == 0b0000) { // (i + 1) % 8 == 0
            putchar('\n');
        }
    }
    if (((i + 1) & 0b11) != 0b01) { // (i + 1) % 8 != 1
        putchar('\n');
    }

    ba_free(bitarray_p);
    return 0;
}
