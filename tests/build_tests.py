#!/usr/bin/env python
import os, sys

bin_dir = "../bin"
include_dir = "../src"

os.chdir(sys.path[0])

if not os.path.exists(bin_dir):
    os.makedirs(bin_dir)

for test_name in ["stack_tests", "fstack_tests"]:
    os.system(f"gcc -Wall -Wextra -I{include_dir} -o {bin_dir}/{test_name}.bin ../src/*.c {test_name}.c")
