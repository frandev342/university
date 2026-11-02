#ifndef AVL_H
#define AVL_H
#include <math.h>
#include <stdlib.h>

template <typename T> class AVL {
private:
  struct Node {
    T key;
    Node *left;
    Node *right;
    std::size_t height;
  };
  Node *root;
  Node *leftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
  }
  Node *rightRotate(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
  }
  void updHeight(Node *n) {
    if (n) {
      updHeight(n->left);
      updHeight(n->right);
      n->height = 1 + std::max(n->left->height, n->right->height);
    }
  }
};

#endif // !
