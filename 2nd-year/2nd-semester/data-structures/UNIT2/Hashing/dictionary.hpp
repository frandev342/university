#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP
#include <array>
#include <cstddef>

class Dictionary {
  struct Node {
    unsigned key;
    int value;
    Node *next;
  };
  static constexpr std::size_t M = 7;
};

#endif // !
