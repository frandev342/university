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
  // UN array inicializado en nullptr todos los elementos
  std::array<Node *, M> bucket{};
  std::size_t count = 0;

public:
  Dictionary() = default;
  // Prohibo al compilador usar el constructor por copia, osea no podrá usar el
  // que tiene por implícito el compilador
  Dictionary(const Dictionary &) = delete;
  Dictionary &operator=(const Dictionary &) = delete;
  ~Dictionary();

  // Methods
  bool get(unsigned key, int &value) const;
  bool put(unsigned key, int value);
  bool contains(unsigned key) const;
  std::size_t size() const { return count; }
  void clear();
};

#endif
