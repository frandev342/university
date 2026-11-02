#ifndef RB_CPP_RECURSIVE_TREE_HPP
#define RB_CPP_RECURSIVE_TREE_HPP

#include <cstddef>
#include <iosfwd>
#include <vector>

namespace red_black_recursive {

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
    Node(int value, Color c) : key(value), color(c), left(nullptr),
                               right(nullptr), parent(nullptr) {}
  };

  Node *search(Node *node, int key) const;
  Node *minimum(Node *node) const;
  void insert_at(Node **link, Node *parent, int key, Node *&inserted);
  void insert_fixup(Node *node);
  void erase_fixup(Node *node);
  void rotate_left(Node *node);
  void rotate_right(Node *node);
  void transplant(Node *old_node, Node *replacement);
  void destroy_nodes(Node *node);
  int node_height(const Node *node) const;
  void inorder_nodes(const Node *node, std::vector<int> &keys) const;
  bool validate(const Node *node, const Node *parent, long long low,
                long long high, std::size_t &count, int &blacks) const;
  void print_nodes(const Node *node, std::ostream &out,
                   std::size_t depth) const;

  Node nil_;
  Node *root_;
  std::size_t size_;
};

} // namespace red_black_recursive
#endif
