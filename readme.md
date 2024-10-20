# data-structures-c

[doxygen documentation](https://abxh.github.io/data-structures-c/)
 | ![tests](https://github.com/abxh/data-structures-c/actions/workflows/tests.yml/badge.svg?event=push)

Generic, header-only and performant data structures. New memory allocation is kept to a minimum.

All data types are expected to be Plain-Old-Datas (PODs). No explicit iterator mechanism is provided, but
macros can provide a primitive syntactical replacement.

Made for my own exploration and use.

Run `make test` to run all tests and examples. The `libsan` and `ubsan` sanitizers is required for building the tests.

Asserts are used to check various assumptions. Use `NDEBUG` flag to turn off asserts in release builds.

| **File**                                                                             | Description                                              |                                                                                                                                                             |
|--------------------------------------------------------------------------------------|----------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [fstack.h](https://github.com/abxh/data-structures-c/blob/main/lib/fstack.h)         | Fixed-size array-based stack                             | [Documentation](https://abxh.github.io/data-structures-c/fstack_8h.html) [Examples](https://github.com/abxh/data-structures-c/blob/main/examples/fstack/)   |
| [fqueue.h](https://github.com/abxh/data-structures-c/blob/main/lib/fqueue.h)         | Fixed-size queue based on ring buffer                    | [Documentation](https://abxh.github.io/data-structures-c/fqueue_8h.html) [Examples](https://github.com/abxh/data-structures-c/blob/main/examples/fqueue/)   |
| [fpqueue.h](https://github.com/abxh/data-structures-c/blob/main/lib/fpqueue.h)       | Fixed-size priority queue based on binary (max-)heap     | [Documentation](https://abxh.github.io/data-structures-c/fpqueue_8h.html)                                                                                   |
| [fhashtable.h](https://github.com/abxh/data-structures-c/blob/main/lib/fhashtable.h) | Fixed-size open-adressing hashtable (robin hood hashing) | [Documentation](https://abxh.github.io/data-structures-c/fhashtable_8h.html)                                                                                |
| [arena.h](https://github.com/abxh/data-structures-c/blob/main/lib/arena.h)           | Arena allocator                                          | [Documentation](https://abxh.github.io/data-structures-c/arena_8h.html)                                                                                     |
| [pool.h](https://github.com/abxh/data-structures-c/blob/main/lib/pool.h)             | Pool allocator                                           | [Documentation](https://abxh.github.io/data-structures-c/pool_8h.html)                                                                                      |
| [freelist.h](https://github.com/abxh/data-structures-c/blob/main/lib/freelist.h)     | Best-fit free list allocator (with underlying free tree) | [Documentation](https://abxh.github.io/data-structures-c/freelist_8h.html)                                                                                  |
| [list.h](https://github.com/abxh/data-structures-c/blob/main/lib/list.h)             | Intrusive circular doubly linked list                    | [Documentation](https://abxh.github.io/data-structures-c/list_8h.html)                                                                                      |
| [rbtree.h](https://github.com/abxh/data-structures-c/blob/main/lib/rbtree.h)         | Intrusive red-black tree                                 | [Documentation](https://abxh.github.io/data-structures-c/rbtree_8h.html)                                                                                    |

## TODO
- write tests for the ones not tested.
- vector impl
- chaining hashtable impl.
