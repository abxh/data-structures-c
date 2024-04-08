# ll_queue

`ll_queue` is an implementation of a generic queue using a linked list, where a extra pointer
to the back of the queue is stored.

A freelist is maintained to reuse nodes, if they are dequeued.

`ll_queue_wrapper.h` can be used to convert between queue nodes and values.
