
name := chemical_formulae
include_dir := ../../include/
src_dir := ../../src
single_dep := strmap

all: ${name}

${name}: ${single_dep}
	cc -I${include_dir} -o ${name}.bin ${single_dep}.o ${name}.c

${single_dep}:
	cc -I${include_dir} -c -o $@.o ${src_dir}/$@.c 

clean:
	rm -f ${single_dep}.o
	rm -f ${name}.bin
