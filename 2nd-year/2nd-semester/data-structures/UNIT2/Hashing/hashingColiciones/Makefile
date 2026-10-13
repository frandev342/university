CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g -O0

.PHONY: all test-chain test-linear clean
all: test_chain test_linear

test_chain: dictionary.cc dictionary.h test_chain.cc common_tests.h
	$(CXX) $(CXXFLAGS) dictionary.cc test_chain.cc -o $@

test_linear: linear_dictionary.cc linear_dictionary.h test_linear.cc common_tests.h
	$(CXX) $(CXXFLAGS) linear_dictionary.cc test_linear.cc -o $@

test-chain: test_chain
	./test_chain

test-linear: test_linear
	./test_linear

clean:
	rm -f test_chain test_linear
