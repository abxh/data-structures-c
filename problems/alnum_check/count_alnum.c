#include <ctype.h>
#include <stdio.h>

#include "bitarray.h"

#define NUM_OF_LETTERS ('z' - 'a' + 1)
#define NUM_OF_DIGITS ('9' - '0' + 1)
#define TOTAL (NUM_OF_DIGITS + 2 * NUM_OF_LETTERS)

int main(void) {
    Bitarray* bitarray_p;

    if (!bitarray_init(&bitarray_p, TOTAL)) {
        return 1;
    }

    puts("Input (Ctrl+d for EOF):");
    for (int c = fgetc(stdin); c != EOF; c = fgetc(stdin)) {
        if (islower(c)) {
            bitarray_set_true(bitarray_p, c - 'a');
        } else if (isupper(c)) {
            bitarray_set_true(bitarray_p, c - 'A' + NUM_OF_LETTERS);
        } else if (isdigit(c)) {
            bitarray_set_true(bitarray_p, c - '0' + 2 * NUM_OF_LETTERS);
        }
    }

    putchar('\n');
    printf("Bit array:\n");
    bitarray_print(bitarray_p);
    putchar('\n');

    size_t i;
    for (i = 0; i < TOTAL ; i++) {
        if (i >= TOTAL) {
            putchar('.');
            break;
        }
        if (((i + 1) % 8 != 1)) {
            printf(", ");
        }
        if (i < NUM_OF_LETTERS) {
            printf("%c:%c", (int)i + 'a', bitarray_get(bitarray_p, i) ? '*' : ' ');
        } else if (i < 2 * NUM_OF_LETTERS) {
            printf("%c:%c", (int)i - 1 * NUM_OF_LETTERS + 'A', bitarray_get(bitarray_p, i) ? '*' : ' ');
        } else if (i < 2 * NUM_OF_LETTERS + NUM_OF_DIGITS) {
            printf("%c:%c", (int)i - 2 * NUM_OF_LETTERS + '0', bitarray_get(bitarray_p, i) ? '*' : ' ');
        }
        if (((i + 1) % 8 == 0)) {
            putchar('\n');
        }
    }
    putchar('\n');

    bitarray_deinit(&bitarray_p);
    return 0;
}
