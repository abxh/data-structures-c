
name := rotating_starfield
include_dir := ../../include/
src_dir := ../../src

all: ${name}

${name}: rotate_bits
	cc -lm -I${include_dir} -o ${name}.bin rotate_bits.o ${name}.c

rotate_bits:
	cc -I${include_dir} -c -o $@.o ${src_dir}/$@.c 

clean:
	rm -f rotate_bits.o
	rm -f ${name}.bin
	rm -f data.csv
