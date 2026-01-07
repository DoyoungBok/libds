CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -g
SANITIZERS = -fsanitize=address,undefined -fno-omit-frame-pointer
LDFLAGS = $(SANITIZERS)

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

.PHONY: all clean

# Default target: build all tests
all: test_dynamic_array test_linked_list test_stack test_queue test_hash_table test_hash_table_generic


# ===================== Dynamic Array Test =====================

test_dynamic_array: tests/test_dynamic_array.c $(OBJ)
	$(CC) $(CFLAGS) $(SANITIZERS) -o test_dynamic_array \
	tests/test_dynamic_array.c $(OBJ) $(LDFLAGS)


# ===================== Linked List Test =====================

test_linked_list: tests/test_linked_list.c $(OBJ)
	$(CC) $(CFLAGS) $(SANITIZERS) -o test_linked_list \
	tests/test_linked_list.c $(OBJ) $(LDFLAGS)


# ===================== Stack Test =====================

test_stack: tests/test_stack.c $(OBJ)
	$(CC) $(CFLAGS) $(SANITIZERS) -o test_stack \
	tests/test_stack.c $(OBJ) $(LDFLAGS)

# ===================== Queue Test =====================

test_queue: tests/test_queue.c $(OBJ)
	$(CC) $(CFLAGS) $(SANITIZERS) -o test_queue \
	tests/test_queue.c $(OBJ) $(LDFLAGS)


# ===================== Hash Table Test =====================

test_hash_table: tests/test_hash_table.c $(OBJ)
	$(CC) $(CFLAGS) $(SANITIZERS) -o test_hash_table \
	tests/test_hash_table.c $(OBJ) $(LDFLAGS)


# ===================== Hash Table Generic Test =====================

test_hash_table_generic: tests/test_hash_table_generic.c $(OBJ)
	$(CC) $(CFLAGS) $(SANITIZERS) -o test_hash_table_generic \
	tests/test_hash_table_generic.c $(OBJ) $(LDFLAGS)


# ===================== Clean =====================

clean:
	rm -f src/*.o test_dynamic_array test_linked_list test_stack test_queue test_hash_table test_hash_table_generic
