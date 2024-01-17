# Readme
This is a simple implementation of a stack using a singly-linked list. It uses `void*`s to make the
stack more generic, and `CREATE_STACK_INLINE_FUNCTIONS` can be used to create functions specific to
a stack value type.

The order in which elements is added and removed is **LIFO** (**L**ast **I**n [is] **F**irst **O**ut).
