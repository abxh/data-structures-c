
name := calculator
include_dir := ../../include/
src_dir := ../../src

default: ${name}

kattis:
	cc -DKATTIS -lm -I${include_dir} -o ${name}.bin ${name}.c

debug:
	cc -DDEBUG -lm -I${include_dir} -o ${name}.bin ${name}.c

${name}:
	cc -lm -I${include_dir} -o ${name}.bin ${name}.c

clean:
	rm -f ${name}.bin
