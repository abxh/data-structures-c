name := calculator

.PHONY: all
all:
	cc -I../../src/ $(wildcard ../../src/*.c) -lm $(name).c -o $(name).out

.PHONY: clean
clean:
	rm $(name).out
