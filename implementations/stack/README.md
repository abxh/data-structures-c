# stack

This is a implementation of a stack using a dynamic array of 8-bit long bytes.

The head node is the first to be popped and the last inserted.

The data size is passed to the new function, which is stored in a struct and used throughout
the methods to allocate memory for the values. The macro `CREATE_STACK_INLINE_FUNCTIONS` can be
used to create functions to work directly stack values.
