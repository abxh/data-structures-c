
name := count_alnum
include_dir := ../../include/
src_dir := ../../src

all: ${name}

${name}: bitarray
	cc -I${include_dir} -o ${name}.bin bitarray.o ${name}.c

bitarray:
	cc -I${include_dir} -c -o $@.o ${src_dir}/$@.c 

clean:
	rm -f bitarray.o
	rm -f ${name}.bin
