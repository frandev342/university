#ifndef AVL_RECURSIVE_HEIGHT_HPP
#define AVL_RECURSIVE_HEIGHT_HPP

#include <cstddef>

namespace avl_recursive {

struct Node {
  int key;
  Node *left;
  Node *right;
  Node *parent;

  explicit Node(int value, Node *parent_node = nullptr)
      : key(value), left(nullptr), right(nullptr), parent(parent_node) {}
};

struct Tree {
  Node *root;

  Tree() : root(nullptr) {}
};

std::size_t tree_size(const Node *node);
int height(const Node *node);

Node *search(Node *node, int key);
Node *minimum(Node *node);
Node *maximum(Node *node);
Node *successor(Node *node);
Node *lower_bound(Node *node, int key);
Node *upper_bound(Node *node, int key);

int avl_insert(Tree &tree, int key);
bool avl_erase(Tree &tree, int key);
bool avl_valid(const Tree &tree);
void destroy(Tree &tree);

} // namespace avl_recursive

#endif // AVL_RECURSIVE_HEIGHT_HPP
