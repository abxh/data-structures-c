# chemical-formulae

The goal of this problem is to become more familiar with how to read a C file,
format and parse strings, use a string hashmap, and use some metric to sort
data and fuzzy search string entries.

**Problem:**

The keys (to be used for mapping) are the `synonym`s listed in the following wikipedia tables:
[here](https://en.wikipedia.org/wiki/Glossary_of_chemical_formulae).

The `synonym` keys are to be mapped to the `formula` and `CAS` values.

You can use [this](get_data.py) python script to download the wikipedia tables in csv format (comma-seperated-values format).

Use some metric (e.g. levenshtein distance, longest common subsequence, string length), and sort
the keys after the user input (with your own sorting algorithm or C standard's `qsort`). Display
the first five keys most similar to the user input.
