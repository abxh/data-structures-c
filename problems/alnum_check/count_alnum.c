#include "./bitarray.h"

int main(void) {
    Bitarray *bitarray = ba_new(55U);
    ba_print(bitarray);
    ba_free(bitarray);
    return 0;
}
