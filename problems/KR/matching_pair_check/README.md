# matching_pair_check

This is a problem based on K & R chapter 1 exercise 24 with some tweaks.

**Problem:**

For a given input, check if there are any unbalanced parentheses, brackets and curly braces. Also check
for single, double and back quotes. And finally check for the C comment symbol pair `/*` and `*/`.

Every starting symbol cannot end with the wrong ending symbol in between which there can be zero or more
characters. These characters _can_ include correctly written pairs of parentheses, brackets and etc.

Quotes cannot be "stacked" like parentheses, brackets and curly braces. That is, you can have `([()])`,
but not `"'""'"`. The comment sign cannot "stack" as well, but you can have `/* /* */`. Pairing rules in
the comment block is ignored.

If you have implemented the above, then support escape sequences using `\<special-char>`.
