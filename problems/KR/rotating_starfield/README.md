# rotating_starfield

This problem puts some extra "spice" onto the problem of bitwise rotation posed in K & R chapter 2.

> **note**: my solution has linux-specific elements.

**Problem:**

Generate a "sparse" random bit pattern of 1's and 0's of size 64x8, and rotate the bit patterns left to right or
right to left as time passes. It is "sparse" in the sense that there are a lot of 0's compared to 1's. You are left to
decide what "a lot" means.

Continously print each row of the bit pattern using ' ' to represent 0 and ´*´ to represent 1.
