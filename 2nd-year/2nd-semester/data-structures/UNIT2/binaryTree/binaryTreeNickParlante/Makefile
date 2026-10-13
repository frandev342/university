CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude

SRC = src/main.c src/tree.c src/tree_algorithms.c
OBJ = $(SRC:.c=.o)
TARGET = trees

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
