# chemical-formulae

The goal of this problem is to become more familiar with how to read a C file,
format and parse strings, use a string hashmap, and use some metric to sort
data and fuzzy search string entries.

For this problem, the entries are the `synonyms` listed [here](https://en.wikipedia.org/wiki/Glossary_of_chemical_formulae).

The `synonyms` are to be mapped to the `chemical formula`s and `CAS` numbers, if they exist.

You can use [this](get_data.py) python script to download the wikipedia table in csv format (comma-seperated-values).

Use some metric (levenshtein distance, longest common subsequence, string length), and sort
the entries after the user input (with your own sorting algorithm or C standard's `qsort`). Display
the first five entries most similar to the user input.
