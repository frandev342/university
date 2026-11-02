#ifndef TREE_ALGORITHMS_H
#define TREE_ALGORITHMS_H

#include "tree.h"

// Cuenta todos los nodos del árbol. Tiempo O(n), memoria auxiliar O(h).
int size(const Node *root);

// Calcula la altura en aristas; el árbol vacío vale -1. Tiempo O(n), memoria O(h).
int maxDepth(const Node *root);

// Obtiene el mínimo de un BST no vacío. Tiempo O(h), memoria auxiliar O(1).
int minValue(const Node *root);

// Obtiene recursivamente el mínimo de un BST no vacío. Tiempo y memoria O(h).
int minValueRec(const Node *root);

// Obtiene el máximo de un BST no vacío. Tiempo O(h), memoria auxiliar O(1).
int maxValue(const Node *root);

// Obtiene recursivamente el máximo de un BST no vacío. Tiempo y memoria O(h).
int maxValueRec(const Node *root);

// Imprime raíz, izquierda y derecha. Tiempo O(n), memoria auxiliar O(h).
void printPreorder(const Node *root);

// Imprime izquierda, raíz y derecha. Tiempo O(n), memoria auxiliar O(h).
void printInorder(const Node *root);

// Imprime izquierda, derecha y raíz. Tiempo O(n), memoria auxiliar O(h).
void printPostorder(const Node *root);

// Intercambia recursivamente ambos hijos. Tiempo O(n), memoria auxiliar O(h).
void mirror(Node *root);

// Comprueba valores y estructura iguales. Tiempo O(n), memoria auxiliar O(h).
int sameTree(const Node *a, const Node *b);

// Cuenta los BST distintos con n claves. Tiempo O(n), memoria auxiliar O(n).
long long countTrees(int numKeys);

// Valida cada subárbol con sus extremos. Tiempo O(nh), peor O(n^2); memoria O(h).
int isBST(const Node *root);

// Valida el BST propagando límites. Tiempo O(n), memoria auxiliar O(h).
int isBST2(const Node *root);

// Busca una suma en una ruta raíz-hoja. Tiempo O(n), memoria auxiliar O(h).
int hasPathSum(const Node *root, int sum);

// Imprime todas las rutas raíz-hoja. Tiempo O(n + salida), memoria O(h).
void printPaths(const Node *root);

// Duplica cada nodo como hijo izquierdo. Tiempo O(n), memoria auxiliar O(h).
void doubleTree(Node *root);

#endif
