# matching_pair_check

This is a problem based on K & R chapter 1 exercise 24 with some tweaks.

**Problem:**

For a given input, check if there are any unbalanced parentheses, brackets and curly braces. Also check
for single, double and back quotes. And finally check for the C comment symbol pair `/*` and `*/`.

Every starting symbol cannot end with the wrong corresponding ending symbol. For example, any `(` cannot
subsequently end with anything but `)` between which there can be zero or more characters in between. This
includes pairs of symbols like `{}`, `""`, and etc.
