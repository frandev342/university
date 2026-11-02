#include "avl.hpp"
#include <algorithm>
#include <new>

// Inicializa una hoja: altura 0 e hijos nulos. Tiempo O(1).
AVL::Node::Node(int nodeValue)
    : value(nodeValue), height(0), left(nullptr), right(nullptr) {}

// Inicializa un árbol vacío. Tiempo O(1), memoria auxiliar O(1).
AVL::AVL() : root(nullptr), size_(0) {}

// Libera el árbol completo. Tiempo O(n), memoria auxiliar O(h).
AVL::~AVL() { clear(); }

// Consulta la altura almacenada; nullptr vale -1. Tiempo O(1).
int AVL::NodeHeight(const Node *node) {
  return (node == nullptr) ? -1 : node->height;
}

// Recalcula localmente la altura de un nodo no nulo. Tiempo O(1).
void AVL::updateHeight(Node *node) {
  node->height = 1 + std::max(NodeHeight(node->left), NodeHeight(node->right));
}

// Calcula height(right) - height(left). Tiempo O(1).
int AVL::balanceFactor(const Node *node) {
  return NodeHeight(node->right) - NodeHeight(node->left);
}

// Rota a la izquierda y devuelve la nueva raíz. Tiempo y memoria O(1).
AVL::Node *AVL::rotateLeft(Node *node) {
  Node *y = node->right;
  node->right = y->left;
  y->left = node;
  updateHeight(node);
  updateHeight(y);
  return y;
}

// Actualiza y restaura el balance local; devuelve la nueva raíz. Tiempo O(1).
AVL::Node *AVL::rebalance(Node *node) {
  updateHeight(node);
  const int balancefac = balanceFactor(node);
  if (balancefac > 1) {
    if (balanceFactor(node->right) < 0)
      node->right = rotateRight(node->right);
    return rotateLeft(node);
  } else if (balancefac < -1) {
    if (balanceFactor(node->left) > 0)
      node->left = rotateLeft(node->left);
    return rotateRight(node);
  }
  return node;
}

// Inserta recursivamente y rebalancea al regresar. Tiempo y memoria O(log n).
AVL::Node *AVL::insertNode(AVL::Node *node, int value, bool &inserted) {
  if (!node) {
    Node *p = new (std::nothrow) Node(value);
    if (!p)
      return nullptr;
    inserted = true;
    return p;
  }
  if (value <= node->value) {
    Node *p = insertNode(node->left, value, inserted);
    if (!inserted)
      return node;
    node->left = p;
  } else {
    Node *p = insertNode(node->right, value, inserted);
    if (!inserted)
      return node;
    node->right = p;
  }
  return rebalance(node);
}

// Elimina una aparición y rebalancea al regresar. Tiempo y memoria O(log n).
AVL::Node *AVL::eraseNode(Node *node, int value, bool &erased) {
  if (!node)
    return nullptr;
  if (value < node->value)
    node->left = eraseNode(node->left, value, erased);
  else if (value > node->value)
    node->right = eraseNode(node->right, value, erased);
  else {
    erased = true;
    if (!node->left && !node->right) {
      delete node;
      return nullptr;
    }
    if (node->right && !node->left) {
      Node *replacement = node->right;
      delete node;
      return replacement;
    }
    if (node->left && !node->right) {
      Node *replacement = node->left;
      delete node;
      return replacement;
    }
    // En caso tenga dos hijos
    Node *successor = minimumNode(node->right);
    node->value = successor->value;
    bool successorErased = false;
    node->right = eraseNode(node->right, successor->value, successorErased);
  }
  if (!erased)
    return node;
  return rebalance(node);
}

// Obtiene el menor nodo de un subárbol no vacío. Tiempo y memoria O(log n).
AVL::Node *AVL::minimumNode(Node *node) {
  if (!node->left)
    return node;
  return minimumNode(node->left);
}

// Rota a la derecha y devuelve la nueva raíz. Tiempo y memoria O(1).
AVL::Node *AVL::rotateRight(Node *node) {
  Node *y = node->left;
  node->left = y->right;
  y->right = node;
  updateHeight(node);
  updateHeight(y);
  return y;
}

// Libera un subárbol en postorden. Tiempo O(n), memoria auxiliar O(h).
void AVL::destroy(Node *node) {
  if (!node)
    return;
  destroy(node->left);
  destroy(node->right);
  delete node;
}

// Vacía el árbol y reinicia su contador. Tiempo O(n), memoria auxiliar O(h).
void AVL::clear() {
  destroy(root);
  root = nullptr;
  size_ = 0;
}

// Inserta una clave y actualiza raíz y tamaño. Tiempo y memoria O(log n).
bool AVL::insert(int value) {
  bool inserted = false;
  root = insertNode(root, value, inserted);
  if (inserted)
    size_++;
  return inserted;
}

// Elimina una aparición y actualiza raíz y tamaño. Tiempo y memoria O(log n).
bool AVL::erase(int value) {
  bool erased = false;
  root = eraseNode(root, value, erased);
  if (erased)
    size_--;
  return erased;
}
