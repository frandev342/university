#ifndef RED_BLACK_TREE_HPP
#define RED_BLACK_TREE_HPP

#include <cstddef>
#include <iosfwd>
#include <vector>

namespace red_black {

class RedBlackTree {
public:
  RedBlackTree();
  ~RedBlackTree();

  RedBlackTree(const RedBlackTree &) = delete;
  RedBlackTree &operator=(const RedBlackTree &) = delete;

  bool insert(int key);
  bool erase(int key);
  bool contains(int key) const;

  std::size_t size() const { return size_; }
  bool empty() const { return root_ == &nil_; }
  int height() const;
  std::vector<int> inorder() const;

  bool valid() const;
  void print(std::ostream &out) const;

private:
  enum class Color { Red, Black };

  struct Node {
    int key;
    Color color;
    Node *left;
    Node *right;
    Node *parent;

    Node(int value, Color node_color)
        : key(value), color(node_color), left(nullptr), right(nullptr),
          parent(nullptr) {}
  };

  Node *find_node(int key) const;
  Node *minimum(Node *node) const;

  void rotate_left(Node *node);
  void rotate_right(Node *node);
  void insert_fixup(Node *node);
  void erase_fixup(Node *node);
  void transplant(Node *old_root, Node *new_root);

  void destroy_nodes(Node *node);
  void inorder_nodes(const Node *node, std::vector<int> &keys) const;
  int node_height(const Node *node) const;
  void print_node(std::ostream &out, const Node *node,
                  std::size_t depth) const;
  bool validate_node(const Node *node, const Node *expected_parent,
                     long long lower, long long upper,
                     std::size_t &node_count, int &black_height) const;

  Node nil_;
  Node *root_;
  std::size_t size_;
};

} // namespace red_black

#endif // RED_BLACK_TREE_HPP
