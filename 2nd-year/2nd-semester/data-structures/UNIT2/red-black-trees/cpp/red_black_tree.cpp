#include "red_black_tree.hpp"

#include <limits>
#include <ostream>
#include <string>

namespace red_black {

RedBlackTree::RedBlackTree()
    : nil_(0, Color::Black), root_(&nil_), size_(0U) {
  nil_.left = &nil_;
  nil_.right = &nil_;
  nil_.parent = &nil_;
}

RedBlackTree::~RedBlackTree() { destroy_nodes(root_); }

RedBlackTree::Node *RedBlackTree::find_node(int key) const {
  Node *current = root_;
  while (current != &nil_ && current->key != key)
    current = key < current->key ? current->left : current->right;
  return current;
}

RedBlackTree::Node *RedBlackTree::minimum(Node *node) const {
  while (node->left != &nil_)
    node = node->left;
  return node;
}

bool RedBlackTree::contains(int key) const {
  return find_node(key) != &nil_;
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

bool RedBlackTree::insert(int key) {
  Node *parent = &nil_;
  Node *current = root_;

  while (current != &nil_) {
    parent = current;
    if (key == current->key)
      return false;
    current = key < current->key ? current->left : current->right;
  }

  Node *inserted = new Node(key, Color::Red);
  inserted->left = &nil_;
  inserted->right = &nil_;
  inserted->parent = parent;

  if (parent == &nil_)
    root_ = inserted;
  else if (key < parent->key)
    parent->left = inserted;
  else
    parent->right = inserted;

  ++size_;
  insert_fixup(inserted);
  return true;
}

void RedBlackTree::insert_fixup(Node *node) {
  while (node->parent->color == Color::Red) {
    if (node->parent == node->parent->parent->left) {
      Node *uncle = node->parent->parent->right;

      if (uncle->color == Color::Red) {
        node->parent->color = Color::Black;
        uncle->color = Color::Black;
        node->parent->parent->color = Color::Red;
        node = node->parent->parent;
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
        node = node->parent->parent;
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

  root_->color = Color::Black;
  root_->parent = &nil_;
}

void RedBlackTree::transplant(Node *old_root, Node *new_root) {
  if (old_root->parent == &nil_)
    root_ = new_root;
  else if (old_root == old_root->parent->left)
    old_root->parent->left = new_root;
  else
    old_root->parent->right = new_root;

  // erase_fixup also needs the parent when new_root is the shared sentinel.
  new_root->parent = old_root->parent;
}

bool RedBlackTree::erase(int key) {
  Node *target = find_node(key);
  if (target == &nil_)
    return false;

  Node *moved = target;
  Color removed_color = moved->color;
  Node *replacement = &nil_;

  if (target->left == &nil_) {
    replacement = target->right;
    transplant(target, target->right);
  } else if (target->right == &nil_) {
    replacement = target->left;
    transplant(target, target->left);
  } else {
    moved = minimum(target->right);
    removed_color = moved->color;
    replacement = moved->right;

    if (moved->parent == target) {
      replacement->parent = moved;
    } else {
      transplant(moved, moved->right);
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

void RedBlackTree::erase_fixup(Node *node) {
  while (node != root_ && node->color == Color::Black) {
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
        node = node->parent;
      } else {
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
        node = root_;
      }
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
        node = node->parent;
      } else {
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
        node = root_;
      }
    }
  }

  node->color = Color::Black;
}

int RedBlackTree::node_height(const Node *node) const {
  if (node == &nil_)
    return -1;

  const int left_height = node_height(node->left);
  const int right_height = node_height(node->right);
  return 1 + (left_height > right_height ? left_height : right_height);
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

bool RedBlackTree::validate_node(const Node *node,
                                 const Node *expected_parent, long long lower,
                                 long long upper, std::size_t &node_count,
                                 int &black_height) const {
  if (node == &nil_) {
    black_height = 1; // Every NIL leaf is black.
    return true;
  }

  if (node->parent != expected_parent)
    return false;
  if (node->key <= lower || node->key >= upper)
    return false;
  if (node->left == nullptr || node->right == nullptr)
    return false;
  if (node->color == Color::Red &&
      (node->left->color == Color::Red || node->right->color == Color::Red))
    return false;

  int left_black_height = 0;
  int right_black_height = 0;
  if (!validate_node(node->left, node, lower, node->key, node_count,
                     left_black_height))
    return false;
  if (!validate_node(node->right, node, node->key, upper, node_count,
                     right_black_height))
    return false;
  if (left_black_height != right_black_height)
    return false;

  ++node_count;
  black_height = left_black_height + (node->color == Color::Black ? 1 : 0);
  return true;
}

bool RedBlackTree::valid() const {
  if (nil_.color != Color::Black || nil_.left != &nil_ ||
      nil_.right != &nil_)
    return false;
  if (root_ == nullptr)
    return false;
  if (root_ == &nil_)
    return size_ == 0U;
  if (root_->parent != &nil_ || root_->color != Color::Black)
    return false;

  std::size_t node_count = 0U;
  int black_height = 0;
  const bool structure_is_valid =
      validate_node(root_, &nil_, std::numeric_limits<long long>::lowest(),
                    std::numeric_limits<long long>::max(), node_count,
                    black_height);
  return structure_is_valid && node_count == size_;
}

void RedBlackTree::print_node(std::ostream &out, const Node *node,
                              std::size_t depth) const {
  if (node == &nil_)
    return;

  print_node(out, node->right, depth + 1U);
  out << std::string(depth * 4U, ' ')
      << (node->color == Color::Black ? "B:" : "R:") << node->key << '\n';
  print_node(out, node->left, depth + 1U);
}

void RedBlackTree::print(std::ostream &out) const {
  if (empty()) {
    out << "(empty)\n";
    return;
  }
  print_node(out, root_, 0U);
}

void RedBlackTree::destroy_nodes(Node *node) {
  if (node == &nil_)
    return;

  destroy_nodes(node->left);
  destroy_nodes(node->right);
  delete node;
}

} // namespace red_black
