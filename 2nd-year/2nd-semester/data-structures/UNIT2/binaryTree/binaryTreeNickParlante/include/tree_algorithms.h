#ifndef TREE_ALGORITHMS_H
#define TREE_ALGORITHMS_H

#include "tree.h"

/*
 * Funciones que se desarrollaran durante la practica.
 *
 * No cambie sus firmas salvo indicacion expresa del docente.
 */

size_t treeSize(const Node *root);

int maxDepth(const Node *root);

/*
 * Precondicion: root != NULL y root representa un BST.
 */
int minValue(const Node *root);

int maxValue(const Node *root);

/*
 * Imprime los datos de un BST en orden creciente.
 */
void printTree(const Node *root);

/*
 * Imprime un arbol en postorden.
 */
void printPostorder(const Node *root);

/*
 * Retorna 1 si existe un camino raiz-hoja cuya suma sea sum;
 * en caso contrario retorna 0.
 */
int hasPathSum(const Node *root, int sum);

/*
 * Imprime todos los caminos raiz-hoja, uno por linea.
 */
void printPaths(const Node *root);

/*
 * Convierte el arbol en su imagen espejo.
 */
void mirror(Node *root);

/*
 * Retorna 1 si ambos arboles tienen la misma estructura y
 * los mismos valores; en caso contrario retorna 0.
 */
int sameTree(const Node *a, const Node *b);

/*
 * Reto opcional: verifica si un arbol binario satisface
 * la propiedad de arbol binario de busqueda.
 */
int isBST(const Node *root);
int isBST2(const Node *root);

#endif
