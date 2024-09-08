EXEC_NAME := a.out

CFLAGS     += -I./../../lib
CFLAGS     += -Wall -Wextra -pedantic -Wconversion -Wshadow
CFLAGS     += -ggdb3

EXAMPLE_FILES  := $(wildcard *.c)
OBJ_FILES      := $(EXAMPLE_FILES:.c=.o)

.PHONY: all clean test

all: $(EXEC_NAME)

clean:
	rm -rf $(OBJ_FILES)
	rm -rf $(EXEC_NAME)

test: $(EXEC_NAME)
	./a.out

$(EXEC_NAME): $(OBJ_FILES)
	$(CC) $(LD_FLAGS) $^ -o $(EXEC_NAME)

$(OBJ_FILES): $(SRC_FILES)

$(SRC_FILES):
	$(CC) -c $(CFLAGS) $@
