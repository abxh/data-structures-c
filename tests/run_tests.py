#!/usr/bin/env python
import os, sys, glob, pathlib

bin_dir = "../bin"
include_dir = "../src"

os.chdir(sys.path[0])

if not os.path.exists(bin_dir):
    os.makedirs(bin_dir)

for name in glob.glob("./*_tests.c"):
    s = pathlib.Path(name).stem
    os.system(f"gcc -Wall -Wextra -I{include_dir} -o {bin_dir}/{s}.bin ../src/*.c {s}.c")

for name in glob.glob("./*_tests.c"):
    s = pathlib.Path(name).stem
    os.system(f"../bin/{s}.bin")

