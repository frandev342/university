CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
SANFLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

.PHONY: all test test-insert test-join test-append test-tree sanitize clean
all: tests

tests: treelist.c treelist.h tests.c
	$(CC) $(CFLAGS) treelist.c tests.c -o $@

test-insert: tests
	./tests insert

test-join: tests
	./tests join

test-append: tests
	./tests append

test-tree: tests
	./tests tree

test: tests
	./tests insert
	./tests join
	./tests append
	./tests tree

sanitize:
	$(CC) $(CFLAGS) $(SANFLAGS) treelist.c tests.c -o tests_san
	@echo 'Ejecute: ./tests_san insert (o join, append, tree)'

clean:
	rm -f tests tests_san *.o
