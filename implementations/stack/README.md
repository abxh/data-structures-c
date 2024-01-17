# stack

This is a simple implementation of a stack using a singly-linked list. The stack values are `void*`s which
points to the actual values themselves. This makes the stack be able to store addresses to any type of value,
and the macro `CREATE_STACK_INLINE_FUNCTIONS` can be used to create functions to work directly stack values
with proper memory handling.

The added indirection with `void*`s can be meaningful for performance in a bad way, but it makes the stack
implementation more convenient to use.
