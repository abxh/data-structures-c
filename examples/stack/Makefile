CC         := cc
CFLAGS     := -c -Wall -DDEBUG -I../../src
LDFLAGS    :=
SOURCES    := stack.c
OBJECTS    := ${SOURCES:.c=.o}
EXECUTABLE := stack.out

all: ${SOURCES} ${EXECUTABLE}

$(EXECUTABLE): ${OBJECTS}
		${CC} ${LDFLAGS} ${OBJECTS} -o $@

.c.o:
		${CC} ${CFLAGS} $< -o $@

clean: ${OBJECTS} ${EXECUTABLE}
		rm -rf ${OBJECTS} ${EXECUTABLE}
