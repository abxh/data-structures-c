#!/usr/bin/env python

import pandas as pd

df = pd.read_html("https://en.wikipedia.org/wiki/Glossary_of_chemical_formulae")

l = []
for i in range(len(df)):
    # hacky try catch append. may not be necessary.
    try:
        l.append(df[i][["Chemical formula", "Synonyms", "CAS number"]])
    except:
        try:
            l.append(df[i][["Chemical formula", "Synonyms"]])
        except:
            continue
df_new = pd.concat(l)

# print(df_new, "\n")
print("Writing to data.csv")

df_new.to_csv("data.csv", index=False)
