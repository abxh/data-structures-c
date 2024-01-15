#include <ctype.h>
#include <stdio.h>
#include "bitarray.h"

// 8 bit/word * 8 words = 64 > 26 letters * 2 + 10 digits = 62
#define NUM_OF_LETTERS ('z' - 'a' + 1)
#define NUM_OF_DIGITS  ('9' - '0' + 1)

int main(void) {
    Bitarray *bitarray_p = ba_new(sizeof(BA_WORD) * 8 > 64 ? 1 : 64 / (sizeof(BA_WORD) * 8)); 

    if (bitarray_p == NULL) {
        return 1;
    }

    puts("Input (Ctrl+d for EOF):");
    for (int c = fgetc(stdin); c != EOF; c = fgetc(stdin)) {
        if (islower(c)) {
            ba_set_true(bitarray_p, c - 'a');
        }
        else if (isupper(c)) {
            ba_set_true(bitarray_p, c - 'A' + NUM_OF_LETTERS);
        }
        else if (isdigit(c)) {
            ba_set_true(bitarray_p, c - '0' + 2 * NUM_OF_LETTERS);
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
        if (((i + 1) % 8 != 1)) {
            printf(", ");
        }
        if (i < NUM_OF_LETTERS) {
            printf("%c:%c", (int) i + 'a', ba_get(bitarray_p, i) ? '*' : ' ');
        } else if (i < 2 * NUM_OF_LETTERS) {
            printf("%c:%c", (int) i - 1 * NUM_OF_LETTERS + 'A', ba_get(bitarray_p, i) ? '*' : ' ');
        } else if (i < 2 * NUM_OF_LETTERS + NUM_OF_DIGITS) {
            printf("%c:%c", (int) i - 2 * NUM_OF_LETTERS + '0', ba_get(bitarray_p, i) ? '*' : ' ');
        }
        if (((i + 1) % 8 == 0)) {
            putchar('\n');
        }
    }
    putchar('\n');

    ba_free(bitarray_p);
    return 0;
}
