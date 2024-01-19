#include <ctype.h>
#include <stdio.h>

#include "bitarr.h"

#define NUM_OF_LETTERS ('z' - 'a' + 1)
#define NUM_OF_DIGITS ('9' - '0' + 1)

int main(void) {
    Bitarr* bitarr_p = bitarr_new((sizeof(uint8_t) * 8)); // 8 bit/word * 8 words = 64 > 26 letters * 2 + 10 digits = 62

    if (bitarr_p == NULL) {
        return 1;
    }

    puts("Input (Ctrl+d for EOF):");
    for (int c = fgetc(stdin); c != EOF; c = fgetc(stdin)) {
        if (islower(c)) {
            bitarr_set_true(bitarr_p, c - 'a');
        } else if (isupper(c)) {
            bitarr_set_true(bitarr_p, c - 'A' + NUM_OF_LETTERS);
        } else if (isdigit(c)) {
            bitarr_set_true(bitarr_p, c - '0' + 2 * NUM_OF_LETTERS);
        }
    }

    putchar('\n');
    printf("Bit array:\n");
    bitarr_print(bitarr_p);
    putchar('\n');

    size_t i;
    for (i = 0; i < bitarr_p->num_of_words * 8; i++) {
        if (i >= 2 * NUM_OF_LETTERS + NUM_OF_DIGITS) {
            putchar('.');
            break;
        }
        if (((i + 1) % 8 != 1)) {
            printf(", ");
        }
        if (i < NUM_OF_LETTERS) {
            printf("%c:%c", (int)i + 'a', bitarr_get(bitarr_p, i) ? '*' : ' ');
        } else if (i < 2 * NUM_OF_LETTERS) {
            printf("%c:%c", (int)i - 1 * NUM_OF_LETTERS + 'A', bitarr_get(bitarr_p, i) ? '*' : ' ');
        } else if (i < 2 * NUM_OF_LETTERS + NUM_OF_DIGITS) {
            printf("%c:%c", (int)i - 2 * NUM_OF_LETTERS + '0', bitarr_get(bitarr_p, i) ? '*' : ' ');
        }
        if (((i + 1) % 8 == 0)) {
            putchar('\n');
        }
    }
    putchar('\n');

    bitarr_free(bitarr_p);
    return 0;
}
