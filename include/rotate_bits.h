#include <assert.h> // static_assert
#include <limits.h> // CHAR_BIT
#include <stdlib.h> // size_t

static_assert(CHAR_BIT == 8, "a char is 8 bits");

size_t rotate_bits_left(size_t value, size_t count);

size_t rotate_bits_right(size_t value, size_t count);

size_t rotate_bits(size_t value, int shift);
