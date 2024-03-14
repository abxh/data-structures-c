#!/usr/bin/env python

import pandas as pd

df = pd.read_html("https://en.wikipedia.org/wiki/Glossary_of_chemical_formulae")

l = []
for i in range(len(df)):
    try:
        l.append(df[i][["Chemical formula", "Synonyms"]])
    except:
        continue
df_new = pd.concat(l)

print(df_new)
print();
print("Writing to data.csv")

df_new.to_csv("data.csv", index=False, header=False)