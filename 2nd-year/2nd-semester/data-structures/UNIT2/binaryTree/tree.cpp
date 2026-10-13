#include "tree.hpp"
#include <cstddef>
#include <new>

std::size_t tree_size(const Node *x) {
  if (x == nullptr)
    return 0;
  return 1 + tree_size(x->left) + tree_size(x->right);
}

int height(const Node *x) {
  if (x == nullptr)
    return -1;
  int hLeft = height(x->left);
  int hRigth = height(x->right);
  if (hLeft > hRigth)
    return 1 + hLeft;
  else
    return 1 + hRigth;
}

Node *newNode(Node *parent, int key) {
  return new (std::nothrow) Node{key, nullptr, nullptr, parent};
}

int insert(Tree &t, int key) {
  // **link apunta al enlace que podemos cambiar
  Node **link = &t.root;
  Node *parent = nullptr;
  while (*link != nullptr) {
    parent = *link;
    if (key == parent->key)
      return 0;
    link = key <= parent->key ? &parent->left : &parent->right;
  }
  Node *z = newNode(parent, key);
  if (z == nullptr)
    return -1;
  *link = z;
  return 1;
}

Node *search(Node *x, int key) {
  while (x != nullptr && x->key != key) {
    if (key <= x->key)
      x = x->left;
    else
      x = x->right;
  }
  return x;
}

// Costo O(h+1)
Node *minimum(Node *x) {
  if (x == nullptr)
    return nullptr;
  while (x->left != nullptr)
    x = x->left;
  return x;
}

// Costo O(h+1)
Node *maximum(Node *x) {
  if (x == nullptr)
    return nullptr;
  while (x->right != nullptr)
    x = x->right;
  return x;
}

Node *successor(Node *x) {
  if (x == nullptr)
    return nullptr;
  if (x->right != nullptr)
    return minimum(x->right);
  Node *current = x;
  while (current->parent != nullptr && current->parent->key < x->key)
    current = current->parent;
  return current->parent;
}

static void destroy(Node *root) {
  if (root == nullptr)
    return;
  destroy(root->left);
  destroy(root->right);
  delete root;
}

void destroy(Tree &t) {
  destroy(t.root);
  t.root = nullptr;
}

// - Tiempo: O(h).
// - Memoria auxiliar: O(1).
// - Árbol balanceado: O(log n).
// - Árbol degenerado: O(n).
Node *lower_bound(Node *x, int key) {
  Node *best = nullptr;
  while (x != nullptr) {
    if (x->key >= key) {
      best = x;
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return best;
}
// - Tiempo: O(h).
// - Memoria auxiliar: O(1).
Node *upper_bound(Node *x, int key) {
  Node *best = nullptr;
  while (x != nullptr) {
    if (x->key > key) {
      best = x;
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return best;
}

/*
Eliminación de casos estructurales
*/

// Cambiar la raíz de un árbol
// Reemplazar, dentro del árbol t, el enlace que apuntaba a u para que ahora
// apunte a v.
// No copia datos de v a u. El nodo v ocupa físicamente la posición estructural
// que antes ocupaba u.
void transplant(Tree &t, Node *u, Node *v) {
  if (u->parent == nullptr)
    t.root = v;
  // SÍ u es hijo izquierdo
  else if (u == u->parent->left)
    u->parent->left = v;
  // sí si u es hijo derecho
  else
    u->parent->right = v;
  // Actualizar padre ya que ahora v ocupa el lugar de u, evitando que v sea una
  // hoja
  if (v != nullptr)
    v->parent = u->parent;
}

void erase(Tree &t, Node *z) {
  if (z->left == nullptr) {
    transplant(t, z, z->right);
  } else if (z->right == nullptr) {
    transplant(t, z, z->left);
  } else {
    Node *y = minimum(z->right);
    if (y->parent != z) {
      transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
  }
  delete z;
}
