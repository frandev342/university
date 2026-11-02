#include "red_black_tree.hpp"

#include <limits>
#include <ostream>
#include <string>

namespace red_black_recursive {

RedBlackTree::RedBlackTree()
    : nil_(0, Color::Black), root_(&nil_), size_(0U) {
  nil_.left = nil_.right = nil_.parent = &nil_;
}

RedBlackTree::~RedBlackTree() { destroy_nodes(root_); }

void RedBlackTree::destroy_nodes(Node *node) {
  if (node == &nil_)
    return;
  destroy_nodes(node->left);
  destroy_nodes(node->right);
  delete node;
}

RedBlackTree::Node *RedBlackTree::search(Node *node, int key) const {
  if (node == &nil_ || node->key == key)
    return node;
  return search(key < node->key ? node->left : node->right, key);
}

RedBlackTree::Node *RedBlackTree::minimum(Node *node) const {
  return node->left == &nil_ ? node : minimum(node->left);
}

bool RedBlackTree::contains(int key) const {
  return search(root_, key) != &nil_;
}

void RedBlackTree::rotate_left(Node *node) {
  Node *pivot = node->right;
  node->right = pivot->left;
  if (pivot->left != &nil_)
    pivot->left->parent = node;
  pivot->parent = node->parent;
  if (node->parent == &nil_)
    root_ = pivot;
  else if (node == node->parent->left)
    node->parent->left = pivot;
  else
    node->parent->right = pivot;
  pivot->left = node;
  node->parent = pivot;
}

void RedBlackTree::rotate_right(Node *node) {
  Node *pivot = node->left;
  node->left = pivot->right;
  if (pivot->right != &nil_)
    pivot->right->parent = node;
  pivot->parent = node->parent;
  if (node->parent == &nil_)
    root_ = pivot;
  else if (node == node->parent->right)
    node->parent->right = pivot;
  else
    node->parent->left = pivot;
  pivot->right = node;
  node->parent = pivot;
}

// Descend recursively through links so rotations can still change root_.
void RedBlackTree::insert_at(Node **link, Node *parent,
                             int key, Node *&inserted) {
  Node *node = *link;
  if (node == &nil_) {
    inserted = new Node(key, Color::Red);
    inserted->left = inserted->right = &nil_;
    inserted->parent = parent;
    *link = inserted;
    return;
  }
  if (key == node->key)
    return;
  if (key < node->key)
    insert_at(&node->left, node, key, inserted);
  else
    insert_at(&node->right, node, key, inserted);
}

void RedBlackTree::insert_fixup(Node *node) {
  if (node->parent->color != Color::Red)
    return;
  if (node->parent == node->parent->parent->left) {
    Node *uncle = node->parent->parent->right;
    if (uncle->color == Color::Red) {
      node->parent->color = Color::Black;
      uncle->color = Color::Black;
      node->parent->parent->color = Color::Red;
      insert_fixup(node->parent->parent);
    } else {
      if (node == node->parent->right) {
        node = node->parent;
        rotate_left(node);
      }
      node->parent->color = Color::Black;
      node->parent->parent->color = Color::Red;
      rotate_right(node->parent->parent);
    }
  } else {
    Node *uncle = node->parent->parent->left;
    if (uncle->color == Color::Red) {
      node->parent->color = Color::Black;
      uncle->color = Color::Black;
      node->parent->parent->color = Color::Red;
      insert_fixup(node->parent->parent);
    } else {
      if (node == node->parent->left) {
        node = node->parent;
        rotate_right(node);
      }
      node->parent->color = Color::Black;
      node->parent->parent->color = Color::Red;
      rotate_left(node->parent->parent);
    }
  }
}

bool RedBlackTree::insert(int key) {
  Node *inserted = nullptr;
  insert_at(&root_, &nil_, key, inserted);
  if (inserted == nullptr)
    return false;
  ++size_;
  insert_fixup(inserted);
  root_->color = Color::Black;
  return true;
}

void RedBlackTree::transplant(Node *old_node, Node *replacement) {
  if (old_node->parent == &nil_)
    root_ = replacement;
  else if (old_node == old_node->parent->left)
    old_node->parent->left = replacement;
  else
    old_node->parent->right = replacement;
  // NIL also needs a temporary parent during erase_fixup.
  replacement->parent = old_node->parent;
}

// Case 2 recurses on the parent; cases 1, 3, 4 use local rotations.
void RedBlackTree::erase_fixup(Node *node) {
  if (node == root_ || node->color == Color::Red) {
    node->color = Color::Black;
    return;
  }
  if (node == node->parent->left) {
    Node *sibling = node->parent->right;
    if (sibling->color == Color::Red) {
      sibling->color = Color::Black;
      node->parent->color = Color::Red;
      rotate_left(node->parent);
      sibling = node->parent->right;
    }
    if (sibling->left->color == Color::Black &&
        sibling->right->color == Color::Black) {
      sibling->color = Color::Red;
      erase_fixup(node->parent);
      return;
    }
    if (sibling->right->color == Color::Black) {
      sibling->left->color = Color::Black;
      sibling->color = Color::Red;
      rotate_right(sibling);
      sibling = node->parent->right;
    }
    sibling->color = node->parent->color;
    node->parent->color = Color::Black;
    sibling->right->color = Color::Black;
    rotate_left(node->parent);
  } else {
    Node *sibling = node->parent->left;
    if (sibling->color == Color::Red) {
      sibling->color = Color::Black;
      node->parent->color = Color::Red;
      rotate_right(node->parent);
      sibling = node->parent->left;
    }
    if (sibling->right->color == Color::Black &&
        sibling->left->color == Color::Black) {
      sibling->color = Color::Red;
      erase_fixup(node->parent);
      return;
    }
    if (sibling->left->color == Color::Black) {
      sibling->right->color = Color::Black;
      sibling->color = Color::Red;
      rotate_left(sibling);
      sibling = node->parent->left;
    }
    sibling->color = node->parent->color;
    node->parent->color = Color::Black;
    sibling->left->color = Color::Black;
    rotate_right(node->parent);
  }
  root_->color = Color::Black;
}

bool RedBlackTree::erase(int key) {
  Node *target = search(root_, key);
  if (target == &nil_)
    return false;
  Node *moved = target;
  Color removed_color = moved->color;
  Node *replacement;
  if (target->left == &nil_) {
    replacement = target->right;
    transplant(target, replacement);
  } else if (target->right == &nil_) {
    replacement = target->left;
    transplant(target, replacement);
  } else {
    moved = minimum(target->right);
    removed_color = moved->color;
    replacement = moved->right;
    if (moved->parent == target) {
      replacement->parent = moved;
    } else {
      transplant(moved, replacement);
      moved->right = target->right;
      moved->right->parent = moved;
    }
    transplant(target, moved);
    moved->left = target->left;
    moved->left->parent = moved;
    moved->color = target->color;
  }
  delete target;
  --size_;
  if (removed_color == Color::Black)
    erase_fixup(replacement);
  if (root_ != &nil_)
    root_->parent = &nil_;
  nil_.parent = &nil_;
  nil_.color = Color::Black;
  return true;
}

int RedBlackTree::node_height(const Node *node) const {
  if (node == &nil_)
    return -1;
  int left = node_height(node->left);
  int right = node_height(node->right);
  return 1 + (left > right ? left : right);
}

int RedBlackTree::height() const { return node_height(root_); }

void RedBlackTree::inorder_nodes(const Node *node,
                                std::vector<int> &keys) const {
  if (node == &nil_)
    return;
  inorder_nodes(node->left, keys);
  keys.push_back(node->key);
  inorder_nodes(node->right, keys);
}

std::vector<int> RedBlackTree::inorder() const {
  std::vector<int> keys;
  keys.reserve(size_);
  inorder_nodes(root_, keys);
  return keys;
}

bool RedBlackTree::validate(const Node *node, const Node *parent,
                            long long low, long long high, std::size_t &count,
                            int &blacks) const {
  if (node == &nil_) {
    blacks = 1;
    return true;
  }
  if (node == nullptr || node->parent != parent || node->key <= low ||
      node->key >= high || node->left == nullptr || node->right == nullptr)
    return false;
  if (node->color == Color::Red &&
      (node->left->color == Color::Red || node->right->color == Color::Red))
    return false;
  int left = 0;
  int right = 0;
  if (!validate(node->left, node, low, node->key, count, left) ||
      !validate(node->right, node, node->key, high, count, right) ||
      left != right)
    return false;
  ++count;
  blacks = left + (node->color == Color::Black ? 1 : 0);
  return true;
}

bool RedBlackTree::valid() const {
  if (root_ == nullptr || nil_.color != Color::Black ||
      nil_.left != &nil_ || nil_.right != &nil_ || nil_.parent != &nil_)
    return false;
  if (root_ == &nil_)
    return size_ == 0U;
  std::size_t count = 0U;
  int blacks = 0;
  return root_->color == Color::Black && root_->parent == &nil_ &&
         validate(root_, &nil_, std::numeric_limits<long long>::lowest(),
                  std::numeric_limits<long long>::max(), count, blacks) &&
         count == size_;
}

void RedBlackTree::print_nodes(const Node *node, std::ostream &out,
                               std::size_t depth) const {
  if (node == &nil_)
    return;
  print_nodes(node->right, out, depth + 1U);
  out << std::string(depth * 4U, ' ')
      << (node->color == Color::Black ? "B:" : "R:") << node->key << '\n';
  print_nodes(node->left, out, depth + 1U);
}

void RedBlackTree::print(std::ostream &out) const {
  if (empty())
    out << "(empty)\n";
  else
    print_nodes(root_, out, 0U);
}

} // namespace red_black_recursive
