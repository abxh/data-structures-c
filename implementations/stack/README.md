# Readme
This is a simple implementation of a stack using a singly-linked list. The stack values are `void*`s which
points to the actual values themselves. This makes the stack able to store any type of value, and
`CREATE_STACK_INLINE_FUNCTIONS` can be used to create functions to work directly stack value types with
proper memory handling.

The added indirection with `void*`s can be meaningful for performance in a bad way, but it makes the stack
implementation more convenient to use.
