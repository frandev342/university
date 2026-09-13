CXX ?= g++
CXXFLAGS_COMMON = -std=c++20 -Wall -Wextra -Wpedantic -Iinclude
CXXFLAGS_TEST = $(CXXFLAGS_COMMON) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
CXXFLAGS_BENCH = $(CXXFLAGS_COMMON) -O2 -DNDEBUG

.PHONY: all test bench run clean

all: run

tests_runner: tests/test_main.cpp tests/test_structures.cpp
	$(CXX) $(CXXFLAGS_TEST) $^ -o $@

test: tests_runner
	./tests_runner

benchmark: bench/benchmark.cpp
	$(CXX) $(CXXFLAGS_BENCH) $< -o $@

bench: benchmark
	./benchmark

run: test bench

clean:
	rm -f tests_runner benchmark
