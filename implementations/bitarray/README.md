# Bitarray

All operations take a constant runtime. This means that even as the size of the
bitarray grows very big, the time to run the operations is upper bounded by some
constant.

Big endian order is used. This means that we start from the most significant
bit for the operations. For example, for the bits `1000`, we start from `1`.

Word size is 8 bits (uint8_t). Though ideally word size should be set to `size_t`
for optimal performance and usage of space in a given machine.
