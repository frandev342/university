#ifndef ME_AVL_H
#define ME_AVL_H
#include <cstddef>

class AVL {
public:
  // Crea un AVL vacío. Tiempo y memoria auxiliar O(1).
  AVL();

  // Libera todos los nodos. Tiempo O(n), memoria auxiliar O(h).
  ~AVL();

  AVL(const AVL &) = delete;
  AVL &operator=(const AVL &) = delete;

  // Modificaciones y búsqueda. Tiempo O(log n), memoria auxiliar O(log n).
  bool insert(int value);
  bool erase(int value);
  bool contains(int value) const;

  // Consultas de metadatos almacenados. Tiempo y memoria auxiliar O(1).
  std::size_t size() const;
  int height() const;
  bool empty() const;

  // Obtiene extremos; devuelve false si está vacío. Tiempo O(log n), memoria
  // O(1).
  bool minimum(int &value) const;
  bool maximum(int &value) const;

  // Vacía el árbol. Tiempo O(n), memoria auxiliar O(h).
  void clear();

  // Comprueba orden, alturas y balance. Tiempo O(n), memoria auxiliar O(h).
  bool valid() const;

private:
  struct Node {
    explicit Node(int nodeValue);
    int value;
    int height;
    Node *left;
    Node *right;
  };
  Node *root;
  std::size_t size_;

  // Consulta la altura almacenada; nullptr vale -1. Tiempo O(1).
  static int NodeHeight(const Node *node);

  // Recalcula localmente la altura de un nodo no nulo. Tiempo O(1).
  static void updateHeight(Node *node);

  // Calcula height(right) - height(left). Tiempo O(1).
  static int balanceFactor(const Node *node);

  // Libera un subárbol en postorden. Tiempo O(n), memoria auxiliar O(h).
  static void destroy(Node *node);

  // Rotaciones simples que devuelven la nueva raíz. Tiempo O(1).
  static Node *rotateLeft(Node *node);
  static Node *rotateRight(Node *node);

  // Restaura el balance local y devuelve la nueva raíz. Tiempo O(1).
  static Node *rebalance(Node *node);

  // Inserta y rebalancea un subárbol. Tiempo y memoria auxiliar O(log n).
  static Node *insertNode(Node *node, int value, bool &inserted);

  // Elimina y rebalancea un subárbol. Tiempo y memoria auxiliar O(log n).
  static Node *eraseNode(Node *node, int value, bool &erased);

  // Obtiene el mínimo de un subárbol no vacío. Tiempo y memoria O(log n).
  static Node *minimumNode(Node *node);
};

#endif
