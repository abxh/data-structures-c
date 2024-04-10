
name := calculator
include_dir := ../../include/
src_dir := ../../src

all: ${name}

debug: nextpow2
	cc -DDEBUG -lm -I${include_dir} -o ${name}.bin nextpow2.o ${name}.c

${name}: nextpow2
	cc -lm -I${include_dir} -o ${name}.bin nextpow2.o ${name}.c

nextpow2:
	cc -I${include_dir} -c -o $@.o ${src_dir}/$@.c 

clean:
	rm -f nextpow2.o
	rm -f ${name}.bin
