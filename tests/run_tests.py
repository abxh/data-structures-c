#!/usr/bin/env python
import os, sys, glob, pathlib

bin_dir = "../bin"
src_dir = "../src"

include_dir = ["../include"] + glob.glob('../include/*/')
include_flags = ' '.join(f"-I{d}" for d in include_dir)

os.chdir(sys.path[0])

if not os.path.exists(bin_dir):
    os.makedirs(bin_dir)

for name in glob.glob("./*_tests.c"):
    test_file = pathlib.Path(name).stem
    if os.system(f"gcc -Wall -Wextra {include_flags} -o {bin_dir}/{test_file}.o {src_dir}/*.c {test_file}.c") != 0:
        exit(1)

overall_res = True
for name in glob.glob("./*_tests.c"):
    test_file = pathlib.Path(name).stem
    overall_res &= os.system(f"../bin/{test_file}.o") == 0

os.system(f'rm -rf ${bin_dir}')

exit(0 if overall_res else 1)
