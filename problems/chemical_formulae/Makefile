
name := chemical_formulae
include_dir := ../../include/
src_dir := ../../src

all: ${name}

${name}: strmap
	cc -lm -I${include_dir} -o ${name}.bin strmap.o ${name}.c

strmap:
	cc -I${include_dir} -c -o $@.o ${src_dir}/$@.c 

clean:
	rm -f strmap.o
	rm -f ${name}.bin
	rm -f data.csv
