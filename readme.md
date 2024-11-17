# dsa-c

[doxygen documentation](https://abxh.github.io/dsa-c/) | ![tests](https://github.com/abxh/dsa-c/actions/workflows/tests.yml/badge.svg?event=push)

Generic, header-only and performant data structures. New memory allocation is kept to a minimum. Not thread-friendly.

All data types are expected to be Plain-Old-Datas (PODs). No explicit iterator mechanism is provided, but
macros can provide a primitive syntactical replacement.

Made for my own exploration and use.

Run `make test` to run all tests and examples. The `libsan` and `ubsan` sanitizers is required for building the tests.

Asserts are used to check various assumptions. Use `NDEBUG` flag to turn off asserts in release builds.

| **File**                                                                             | Description                                              |                                                                                                                                                             |
|--------------------------------------------------------------------------------------|----------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [fstack.h](https://github.com/abxh/dsa-c/blob/main/dsa/fstack.h)         | Fixed-size array-based stack                             | [Documentation](https://abxh.github.io/dsa-c/fstack_8h.html) [Examples](https://github.com/abxh/dsa-c/blob/main/examples/fstack/)   |
| [fqueue.h](https://github.com/abxh/dsa-c/blob/main/dsa/fqueue.h)         | Fixed-size queue based on ring buffer                    | [Documentation](https://abxh.github.io/dsa-c/fqueue_8h.html) [Examples](https://github.com/abxh/dsa-c/blob/main/examples/fqueue/)   |
| [fpqueue.h](https://github.com/abxh/dsa-c/blob/main/dsa/fpqueue.h)       | Fixed-size priority queue based on binary (max-)heap     | [Documentation](https://abxh.github.io/dsa-c/fpqueue_8h.html)                                                                                   |
| [fhashtable.h](https://github.com/abxh/dsa-c/blob/main/dsa/fhashtable.h) | Fixed-size open-adressing hashtable (robin hood hashing) | [Documentation](https://abxh.github.io/dsa-c/fhashtable_8h.html)                                                                                |
| [arena.h](https://github.com/abxh/dsa-c/blob/main/dsa/arena.h)           | Arena allocator                                          | [Documentation](https://abxh.github.io/dsa-c/arena_8h.html)                                                                                     |
| [pool.h](https://github.com/abxh/dsa-c/blob/main/dsa/pool.h)             | Pool allocator                                           | [Documentation](https://abxh.github.io/dsa-c/pool_8h.html)                                                                                      |
| [freelist.h](https://github.com/abxh/dsa-c/blob/main/dsa/freelist.h)     | Best-fit free list allocator (with underlying free tree) | [Documentation](https://abxh.github.io/dsa-c/freelist_8h.html)                                                                                  |
| [list.h](https://github.com/abxh/dsa-c/blob/main/dsa/list.h)             | Intrusive circular doubly linked list                    | [Documentation](https://abxh.github.io/dsa-c/list_8h.html)                                                                                      |
| [rbtree.h](https://github.com/abxh/dsa-c/blob/main/dsa/rbtree.h)         | Intrusive red-black tree                                 | [Documentation](https://abxh.github.io/dsa-c/rbtree_8h.html)                                                                                    |
