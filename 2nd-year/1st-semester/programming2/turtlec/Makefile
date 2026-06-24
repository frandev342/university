CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I.
LDLIBS = -lcsfml-graphics -lcsfml-window -lcsfml-system -lm

all: testLine

testLine: turtlec.c examples/testLine.c turtlec.h
	$(CC) $(CFLAGS) turtlec.c examples/testLine.c -o testLine $(LDLIBS)

clean:
	rm -f testLine
