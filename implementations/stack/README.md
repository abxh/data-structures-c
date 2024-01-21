# stack

This is a implementation of a stack using a singly-linked list.

The head node is the first to be popped and the last inserted.

The stack values are `void*`s which points to the actual values themselves. The `void*` values
are expected to be heap allocated before being put onto the stack. Using `void*`s, it makes the stack
be able to store addresses to any type of value, and the macro `CREATE_STACK_INLINE_FUNCTIONS` can be
used to create functions to work directly stack values with proper memory handling.
