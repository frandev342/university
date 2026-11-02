#ifndef TREE_HPP
#define TREE_HPP

#include <cstddef>
struct Node {
  int key;
  Node *left = nullptr;
  Node *right = nullptr;
  Node *parent = nullptr;
};
struct Tree {
  Node *root = nullptr;
};

Node *newNode(Node *parent, int key);

std::size_t tree_size(const Node *x);
int height(const Node *x);

int insert(Tree &t, int key);
Node *search(Node *x, int key);

Node *minimum(Node *x);
Node *maximum(Node *x);

Node *successor(Node *x);

void destroy(Tree &t);

Node *lower_bound(Node *x, int key);
Node *upper_bound(Node *x, int key);

void transplant(Tree &t, Node *u, Node *v);
void erase(Tree &t, Node *z);
#endif // !
