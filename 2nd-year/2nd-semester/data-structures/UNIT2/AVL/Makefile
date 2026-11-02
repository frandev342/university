CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Wpedantic -Wconversion -O0 -g
SANITIZERS := -fsanitize=address,undefined -fno-omit-frame-pointer

.PHONY: all test sanitize clean

all: build/recursive_height build/stored_height

build:
	mkdir -p build

build/recursive_height: recursive_height/avl.cpp recursive_height/avl.hpp recursive_height/main.cpp | build
	$(CXX) $(CXXFLAGS) recursive_height/avl.cpp recursive_height/main.cpp -o $@

build/stored_height: stored_height/avl.cpp stored_height/avl.hpp stored_height/main.cpp | build
	$(CXX) $(CXXFLAGS) stored_height/avl.cpp stored_height/main.cpp -o $@

test: all
	./build/recursive_height
	./build/stored_height

sanitize: | build
	$(CXX) $(CXXFLAGS) $(SANITIZERS) recursive_height/avl.cpp recursive_height/main.cpp -o build/recursive_height_sanitize
	$(CXX) $(CXXFLAGS) $(SANITIZERS) stored_height/avl.cpp stored_height/main.cpp -o build/stored_height_sanitize
	./build/recursive_height_sanitize
	./build/stored_height_sanitize

clean:
	rm -rf build
