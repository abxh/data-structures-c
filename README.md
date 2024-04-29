# dsa-c

[Documentation](https://abxh.github.io/dsa-c/index.html)

## FAQ
Why these PASTE/CONCAT/JOIN are defined and used?

These are used to create the symbols (struct names / function names) for different data
structure types - particularly those that involve arrays.

Alternatives:
1. Seperate data structure code for each type used.
2. `void*` or `unsigned char` type arrays, which are converted at runtime.
3. Macros expanded with type parameters.

These have there pros and cons. (1) leads to potentially a lot duplication of code,
(2) creates an extra layer of indirection leading to higher cache misses and (3)
is hard to maintain.

todo:
Why is
For linked lists here are the pp
