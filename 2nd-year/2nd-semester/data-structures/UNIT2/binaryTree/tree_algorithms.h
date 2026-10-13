#ifndef TREE_ALGORITHMS_H
#define TREE_ALGORITHMS_H

#include "tree.h"

void printInorder(const Node *root);
void printPostorder(const Node *root);
int size(const Node *root);

int maxDepth(const Node *root);
int minValue(const Node *root);
int minValueRec(const Node *root);

int maxValue(const Node *root);
int hasPathSum(const Node *root, int sum);

void printPaths(const Node *root);
void mirror(Node *root);
int sameTree(const Node *a, const Node *b);

int countTrees(int numKeys);

long long countTrees2(int numKeys);

int isBST(const Node *root);

int isBSTv2(const Node *root);

#endif //
