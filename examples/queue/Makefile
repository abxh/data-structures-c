CC         := cc
CFLAGS     := -c -Wall -DDEBUG -I../../src
LDFLAGS    :=
SOURCES    := queue.c
OBJECTS    := ${SOURCES:.c=.o}
EXECUTABLE := queue.out

all: ${SOURCES} ${EXECUTABLE}

$(EXECUTABLE): ${OBJECTS}
		${CC} ${LDFLAGS} ${OBJECTS} -o $@

.c.o:
		${CC} ${CFLAGS} $< -o $@

clean: ${OBJECTS} ${EXECUTABLE}
		rm -rf ${OBJECTS} ${EXECUTABLE}
