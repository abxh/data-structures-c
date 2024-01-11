# Bitarray

This is a implementation of a bitarray using a fixed length array of 8-bit words.
The operations treat the words as one contigious whole and get and set bits at a
given index.

The word is an unsigned 8-bit integer. It's unsigned as the sign bit can be better
used as a value and operations on values are less prone to undefined behaviour, as
per the book "Effective C".

Big endian order is used. This means that we start from the most significant
bit for the operations. For example, for the bits `1000`, we start from `1`.

Finally, `NULL` is not checked for, so it must be checked manually after the allocation.
