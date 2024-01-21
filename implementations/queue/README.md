# queue
This is a implementation of a queue using a singly-linked list with an extra pointer pointer to the last
node in the list.

The front element is the first to be dequeued and the back element is the last enqueued.

The queue values are `void*`s which points to the actual values themselves. The `void*` values
are expected to be heap allocated before being put onto the queue. Using `void*`s, 
This makes the queue be able to store addresses to any type of value, and the macro
`CREATE_QUEUE_INLINE_FUNCTIONS` can be used to create functions to work directly queue values
with proper memory handling.
