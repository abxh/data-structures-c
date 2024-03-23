# astack

`astack` is an implementation of a stack using an array. The stack has a max number of elements it can
store, but it can be manually resized if needed.

This is more performant than the linked-list based [stack](../stack) as it utilizes the cache better.

If the max number of elements can be defined, and one is willing to allocate more space than needed, then the
array-based implementation should be preferred.
