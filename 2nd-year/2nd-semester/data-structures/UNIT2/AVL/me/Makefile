CC := gcc
CXX := g++
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -O0 -g
CXXFLAGS := -std=c++11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -O0 -g
SANITIZERS := -fsanitize=address,undefined -fno-omit-frame-pointer
BUILD_DIR := build

.PHONY: all test sanitize clean

all: $(BUILD_DIR)/avl_c11 $(BUILD_DIR)/avl_cpp

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/avl_c11: c11/avl.c c11/avl.h c11/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) c11/avl.c c11/main.c -o $@

$(BUILD_DIR)/avl_cpp: cpp/avl.cpp cpp/avl.hpp cpp/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) cpp/avl.cpp cpp/main.cpp -o $@

test: all
	./$(BUILD_DIR)/avl_c11
	./$(BUILD_DIR)/avl_cpp

sanitize: | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZERS) c11/avl.c c11/main.c -o $(BUILD_DIR)/avl_c11_sanitize
	$(CXX) $(CXXFLAGS) $(SANITIZERS) cpp/avl.cpp cpp/main.cpp -o $(BUILD_DIR)/avl_cpp_sanitize
	./$(BUILD_DIR)/avl_c11_sanitize
	./$(BUILD_DIR)/avl_cpp_sanitize

clean:
	rm -rf $(BUILD_DIR)
