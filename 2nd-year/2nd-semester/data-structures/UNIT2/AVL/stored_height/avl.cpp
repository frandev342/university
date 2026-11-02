#include "avl.hpp"

#include <algorithm>
#include <limits>
#include <new>

namespace avl_stored {

std::size_t tree_size(const Node *node) {
  if (node == nullptr)
    return 0U;
  return 1U + tree_size(node->left) + tree_size(node->right);
}

int height(const Node *node) {
  return node == nullptr ? -1 : node->height;
}

Node *search(Node *node, int key) {
  while (node != nullptr && node->key != key)
    node = key < node->key ? node->left : node->right;
  return node;
}

Node *minimum(Node *node) {
  if (node == nullptr)
    return nullptr;
  while (node->left != nullptr)
    node = node->left;
  return node;
}

Node *maximum(Node *node) {
  if (node == nullptr)
    return nullptr;
  while (node->right != nullptr)
    node = node->right;
  return node;
}

Node *successor(Node *node) {
  if (node == nullptr)
    return nullptr;
  if (node->right != nullptr)
    return minimum(node->right);

  Node *current = node;
  Node *parent = current->parent;
  while (parent != nullptr && current == parent->right) {
    current = parent;
    parent = parent->parent;
  }
  return parent;
}

Node *lower_bound(Node *node, int key) {
  Node *best = nullptr;
  while (node != nullptr) {
    if (node->key >= key) {
      best = node;
      node = node->left;
    } else {
      node = node->right;
    }
  }
  return best;
}

Node *upper_bound(Node *node, int key) {
  Node *best = nullptr;
  while (node != nullptr) {
    if (node->key > key) {
      best = node;
      node = node->left;
    } else {
      node = node->right;
    }
  }
  return best;
}

static void update_height(Node *node) {
  node->height = 1 + std::max(height(node->left), height(node->right));
}

static int balance_factor(const Node *node) {
  return height(node->right) - height(node->left);
}

static Node *rotate_left(Tree &tree, Node *x) {
  Node *y = x->right;
  Node *beta = y->left;
  Node *old_parent = x->parent;

  y->parent = old_parent;
  if (old_parent == nullptr)
    tree.root = y;
  else if (old_parent->left == x)
    old_parent->left = y;
  else
    old_parent->right = y;

  y->left = x;
  x->parent = y;
  x->right = beta;
  if (beta != nullptr)
    beta->parent = x;

  update_height(x);
  update_height(y);
  return y;
}

static Node *rotate_right(Tree &tree, Node *x) {
  Node *y = x->left;
  Node *beta = y->right;
  Node *old_parent = x->parent;

  y->parent = old_parent;
  if (old_parent == nullptr)
    tree.root = y;
  else if (old_parent->left == x)
    old_parent->left = y;
  else
    old_parent->right = y;

  y->right = x;
  x->parent = y;
  x->left = beta;
  if (beta != nullptr)
    beta->parent = x;

  update_height(x);
  update_height(y);
  return y;
}

static Node *rebalance(Tree &tree, Node *node) {
  update_height(node);
  const int balance = balance_factor(node);

  if (balance > 1) {
    if (balance_factor(node->right) < 0)
      rotate_right(tree, node->right);
    return rotate_left(tree, node);
  }

  if (balance < -1) {
    if (balance_factor(node->left) > 0)
      rotate_left(tree, node->left);
    return rotate_right(tree, node);
  }

  return node;
}

int avl_insert(Tree &tree, int key) {
  Node **link = &tree.root;
  Node *parent = nullptr;

  while (*link != nullptr) {
    parent = *link;
    link = key <= parent->key ? &parent->left : &parent->right;
  }

  Node *inserted = new (std::nothrow) Node(key, parent);
  if (inserted == nullptr)
    return -1;
  *link = inserted;

  Node *current = inserted->parent;
  while (current != nullptr) {
    Node *subtree_root = rebalance(tree, current);
    current = subtree_root->parent;
  }

  return 1;
}

bool avl_erase(Tree &tree, int key) {
  Node *target = search(tree.root, key);
  if (target == nullptr)
    return false;

  if (target->left != nullptr && target->right != nullptr) {
    Node *next = minimum(target->right);
    target->key = next->key;
    target = next;
  }

  Node *child = target->left != nullptr ? target->left : target->right;
  Node *parent = target->parent;

  if (child != nullptr)
    child->parent = parent;

  if (parent == nullptr)
    tree.root = child;
  else if (parent->left == target)
    parent->left = child;
  else
    parent->right = child;

  delete target;

  Node *current = parent;
  while (current != nullptr) {
    Node *subtree_root = rebalance(tree, current);
    current = subtree_root->parent;
  }

  return true;
}

static bool validate_node(const Node *node, const Node *expected_parent,
                          long long lower, long long upper,
                          int &actual_height) {
  if (node == nullptr) {
    actual_height = -1;
    return true;
  }

  if (node->parent != expected_parent)
    return false;
  if (node->key < lower || node->key > upper)
    return false;

  int left_height = -1;
  int right_height = -1;
  if (!validate_node(node->left, node, lower, node->key, left_height))
    return false;
  if (!validate_node(node->right, node, node->key, upper, right_height))
    return false;

  actual_height = 1 + std::max(left_height, right_height);
  if (node->height != actual_height)
    return false;

  const int difference = right_height - left_height;
  return difference >= -1 && difference <= 1;
}

bool avl_valid(const Tree &tree) {
  int actual_height = -1;
  return validate_node(tree.root, nullptr,
                       std::numeric_limits<long long>::lowest(),
                       std::numeric_limits<long long>::max(), actual_height);
}

static void destroy_nodes(Node *node) {
  if (node == nullptr)
    return;
  destroy_nodes(node->left);
  destroy_nodes(node->right);
  delete node;
}

void destroy(Tree &tree) {
  destroy_nodes(tree.root);
  tree.root = nullptr;
}

} // namespace avl_stored
