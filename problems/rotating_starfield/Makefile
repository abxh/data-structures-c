name := rotating_starfield

.PHONY: all
all:
	cc -I../../src/ $(wildcard ../../src/*.c) $(name).c -o $(name).out

.PHONY: clean
clean:
	rm $(name).out
