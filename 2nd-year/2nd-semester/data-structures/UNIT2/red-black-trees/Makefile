CC := gcc
CXX := g++
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Wconversion -O0 -g
CXXFLAGS := -std=c++11 -Wall -Wextra -Wpedantic -Wconversion -O0 -g
SANITIZERS := -fsanitize=address,undefined -fno-omit-frame-pointer
VARIANTS := c cpp c_recursive cpp_recursive
PROGRAMS := $(addprefix build/,$(VARIANTS))

.PHONY: all c cpp c_recursive cpp_recursive test test-c test-cpp \
	test-c-recursive test-cpp-recursive sanitize sanitize-c sanitize-cpp \
	sanitize-c-recursive sanitize-cpp-recursive rebuild clean help

all: $(PROGRAMS)

c: build/c
cpp: build/cpp
c_recursive: build/c_recursive
cpp_recursive: build/cpp_recursive

build:
	mkdir -p build

build/c: c/red_black_tree.c c/main.c c/red_black_tree.h | build
	$(CC) $(CFLAGS) c/red_black_tree.c c/main.c -o $@

build/cpp: cpp/red_black_tree.cpp cpp/main.cpp cpp/red_black_tree.hpp | build
	$(CXX) $(CXXFLAGS) cpp/red_black_tree.cpp cpp/main.cpp -o $@

build/c_recursive: c_recursive/red_black_tree.c c_recursive/main.c c_recursive/red_black_tree.h | build
	$(CC) $(CFLAGS) c_recursive/red_black_tree.c c_recursive/main.c -o $@

build/cpp_recursive: cpp_recursive/red_black_tree.cpp cpp_recursive/main.cpp cpp_recursive/red_black_tree.hpp | build
	$(CXX) $(CXXFLAGS) cpp_recursive/red_black_tree.cpp cpp_recursive/main.cpp -o $@

test: test-c test-cpp test-c-recursive test-cpp-recursive

test-c: build/c
	./build/c
test-cpp: build/cpp
	./build/cpp
test-c-recursive: build/c_recursive
	./build/c_recursive
test-cpp-recursive: build/cpp_recursive
	./build/cpp_recursive

sanitize: sanitize-c sanitize-cpp sanitize-c-recursive sanitize-cpp-recursive

sanitize-c: | build
	$(CC) $(CFLAGS) $(SANITIZERS) c/red_black_tree.c c/main.c -o build/c_sanitize
	./build/c_sanitize
sanitize-cpp: | build
	$(CXX) $(CXXFLAGS) $(SANITIZERS) cpp/red_black_tree.cpp cpp/main.cpp -o build/cpp_sanitize
	./build/cpp_sanitize
sanitize-c-recursive: | build
	$(CC) $(CFLAGS) $(SANITIZERS) c_recursive/red_black_tree.c c_recursive/main.c -o build/c_recursive_sanitize
	./build/c_recursive_sanitize
sanitize-cpp-recursive: | build
	$(CXX) $(CXXFLAGS) $(SANITIZERS) cpp_recursive/red_black_tree.cpp cpp_recursive/main.cpp -o build/cpp_recursive_sanitize
	./build/cpp_recursive_sanitize

rebuild: clean
	$(MAKE) all

clean:
	rm -rf build

help:
	@printf '%s\n' \
		'make                       Compila las cuatro versiones.' \
		'make c                     Compila C iterativo.' \
		'make cpp                   Compila C++ iterativo.' \
		'make c_recursive           Compila C recursivo.' \
		'make cpp_recursive         Compila C++ recursivo.' \
		'make test                  Prueba las cuatro versiones.' \
		'make test-c                Prueba C iterativo.' \
		'make test-cpp              Prueba C++ iterativo.' \
		'make test-c-recursive      Prueba C recursivo.' \
		'make test-cpp-recursive    Prueba C++ recursivo.' \
		'make sanitize              Prueba las cuatro con ASan y UBSan.' \
		'make sanitize-c            Sanitiza C iterativo.' \
		'make sanitize-cpp          Sanitiza C++ iterativo.' \
		'make sanitize-c-recursive  Sanitiza C recursivo.' \
		'make sanitize-cpp-recursive Sanitiza C++ recursivo.' \
		'make rebuild               Limpia y recompila todo.' \
		'make clean                 Borra build/.' \
		'make help                  Muestra esta lista.'
