#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP
#include <array>
#include <cstddef>

// static es lo mismo para todos dentro de la clase
class Dictionary {
  // struct Node es un aggregate (sin constructores, sin miembros privados, sin
  // herencia)
  struct Node {
    unsigned key;
    int value;
    Node *next;
  };
  // Pertenece a la clase y no al objeto, M es compartido por todos los objetos
  // constexpr: Evaluado en compile time, por ejemplo el tamaño del arreglo debe
  // ser en compile time. el equivalente de constexpr es #define M 7
  static constexpr std::size_t M = 7;
  std::array<Node *, M> bucket{};
  std::size_t count = 0;

public:
  Dictionary() = default;
  Dictionary(const Dictionary &) = delete;
  Dictionary &operator=(const Dictionary &) = delete;
  ~Dictionary();

  bool get(unsigned key, int &value) const;
  bool put(unsigned key, int value);
  std::size_t size() const { return count; }
  void clear();
};
#endif

// % función hash, para determinar el índice dentro del diccionario
