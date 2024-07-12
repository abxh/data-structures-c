# data-structures-c

[doxygen documentation](https://abxh.github.io/data-structures-c/)
 | ![tests](https://github.com/abxh/data-structures-c/actions/workflows/tests.yml/badge.svg?event=push)

Asserts are used to check various assumptions.
Use `-NDEBUG` for turning off asserts in release builds.

| **File**                                                                             | Description                                                                      |                                                                                                                                                                               |
|--------------------------------------------------------------------------------------|----------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [fstack.h](https://github.com/abxh/data-structures-c/blob/main/lib/fstack.h)         | Fixed-size array-based stack data structure                                      | [Documentation](https://abxh.github.io/data-structures-c/fstack_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/fstack/fstack.c)              |
| [fqueue.h](https://github.com/abxh/data-structures-c/blob/main/lib/fqueue.h)         | Fixed-size ring-buffer-based queue data structure                                | [Documentation](https://abxh.github.io/data-structures-c/fqueue_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/fqueue/fqueue.c)              |
| [fhashtable.h](https://github.com/abxh/data-structures-c/blob/main/lib/fhashtable.h) | Fixed-size hashtable data structure based on open adressing (robin hood hashing) | [Documentation](https://abxh.github.io/data-structures-c/fhashtable_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/fhashtable/fhashtable.c)  |

## Log

[5dd7555](https://github.com/abxh/data-structures-c/tree/5dd7555ac5e5312b42a5a2509fc0edee95acd621): Initial code and tests for fixed size stack / queue / hashtable.
