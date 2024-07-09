name := calculator
# flags := -DDEBUG

.PHONY: all clean test

all:
	cc -I../../src/ $(flags) $(wildcard ../../src/*.c) -lm $(name).c -o a.out

clean:
	rm a.out
