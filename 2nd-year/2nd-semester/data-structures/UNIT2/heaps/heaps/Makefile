CC := gcc
CXX := g++
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Wconversion -O0 -g
CXX11FLAGS := -std=c++11 -Wall -Wextra -Wpedantic -Wconversion -O0 -g
CXX23FLAGS := -std=c++23 -Wall -Wextra -Wpedantic -Wconversion -O0 -g
.PHONY: all test demos asm sanitize clean
all: build/test_c11 build/test_cpp11 build/demo_cpp23 build/macro_c

build:
	mkdir -p build

build/test_c11: c11/heap.c c11/heap.h c11/indices.h tests/test_c11.c | build
	$(CC) $(CFLAGS) -o $@ c11/heap.c tests/test_c11.c

build/test_cpp11: cpp11/heap.cpp cpp11/heap.hpp cpp11/indices.hpp tests/test_cpp11.cpp | build
	$(CXX) $(CXX11FLAGS) -o $@ cpp11/heap.cpp tests/test_cpp11.cpp

build/demo_cpp23: cpp23/demo.cpp cpp23/heap23.hpp | build
	$(CXX) $(CXX23FLAGS) -o $@ cpp23/demo.cpp

build/macro_c: experimentos/macros_c.c | build
	$(CC) $(CFLAGS) -o $@ experimentos/macros_c.c

test: all
	./build/test_c11
	./build/test_cpp11
	./build/demo_cpp23

demos: build/macro_c build/demo_cpp23
	./build/macro_c
	./build/demo_cpp23

asm: | build
	$(CC) -std=c11 -O2 -S -o build/ensamblador_c.s experimentos/ensamblador_c.c
	$(CXX) -std=c++11 -O2 -S -o build/ensamblador_cpp.s experimentos/ensamblador_cpp.cpp
	@echo "Examine build/ensamblador_c.s y build/ensamblador_cpp.s"

sanitize:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer" CXX11FLAGS="$(CXX11FLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer" CXX23FLAGS="$(CXX23FLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer" all
	./build/test_c11
	./build/test_cpp11
	./build/demo_cpp23

clean:
	rm -rf build
