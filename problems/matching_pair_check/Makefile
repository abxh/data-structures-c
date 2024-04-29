name := matching_pair_check

.PHONY: all
all:
	cc -I../../src/ $(wildcard ../../src/*.c) $(name).c -o $(name).out

.PHONY: clean
clean:
	rm $(name).out
