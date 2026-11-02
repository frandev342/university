#include "tree.hpp"
#include <iostream>

using namespace std;

Tree buildExample(const int arr[], int n) {
  Tree t;
  for (int i = 0; i < n; i++) {
    insert(t, arr[i]);
  }
  return t;
}

int main() {
  int arr[] = {5, 3, 1, 4, 9, 6};
  // Construir un árbol
  Tree tree = buildExample(arr, sizeof(arr) / sizeof(arr[0]));
  // Altura de un nodo
  cout << "Height: " << height(tree.root) << endl;
  // Search
  int searchedKey = 4;
  Node *node1 = search(tree.root, searchedKey);
  if (node1 == nullptr)
    cout << "key not found\n";
  else {
    if (node1->parent != nullptr)
      cout << "Parent of " << searchedKey << " is: " << node1->parent->key
           << endl;
    else
      cout << "The node is the root" << endl;
    // Successor
    Node *next = successor(node1);
    if (next != nullptr)
      cout << "Successor of " << node1->key << " is: " << next->key << endl;
    else
      cout << "The node has not successor" << endl;
  };

  destroy(tree);
  return 0;
}
