# queue

This is an implementation of a queue using a linked list, where a extra pointer to the back of
the queue is stored.

A freelist is maintained to reuse nodes, if they are dequeued.

`queue_helpers.h` can be used to convert between queue nodes and values.
