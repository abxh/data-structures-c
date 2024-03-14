# strmap

This is an implementation of strmap (where strings are mapped to strings) using a chaining hash table.

The hash function is hard coded to `fnv_hash64`, and the hash table is resized when the chain length
exceeds a fixed number of nodes.

Buckets are implemented as linked lists for simplicity, but may be replaced by fixed size arrays for additional
performance.

Strings are expected to have a null character (`\0` byte) at the end. Otherwise the functions may loop
forever. Be careful about user inputs if security is important.
