#include "treelist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond, text) do {                         \
  if(!(cond)) {                                         \
    fprintf(stderr, "FALLO: %s (linea %d)\n",          \
            text, __LINE__);                           \
    ++failures;                                         \
  }                                                     \
} while(0)

static Node *singleton(int data) {
  Node *p = newNode(data);
  if(p == NULL) {
    fprintf(stderr, "Sin memoria\n");
    exit(2);
  }
  p->small = p;
  p->large = p;
  return p;
}

static int inorder(Node *p, int *out, int cap) {
  if(p == NULL) return 0;
  int a = inorder(p->small, out, cap);
  if(a < 0 || a >= cap) return -1;
  out[a] = p->data;
  int b = inorder(p->large, out + a + 1, cap - a - 1);
  if(b < 0) return -1;
  return a + 1 + b;
}

static int sameValues(const int *a, const int *b, int n) {
  for(int i = 0; i < n; ++i) if(a[i] != b[i]) return 0;
  return 1;
}

static void checkCircular(Node *head, const int *want, int n,
                          Node **originals) {
  if(n == 0) {
    CHECK(head == NULL, "lista vacia debe tener head=NULL");
    return;
  }
  if(head == NULL) {
    CHECK(0, "falta la cabeza de la lista");
    return;
  }
  Node *p = head;
  Node *seen[64];
  CHECK(n <= 64, "test demasiado grande");
  for(int i = 0; i < n && i < 64; ++i) {
    if(p == NULL || p->small == NULL || p->large == NULL) {
      CHECK(0, "puntero NULL dentro de la lista");
      return;
    }
    CHECK(p->data == want[i], "valores no ordenados o faltantes");
    CHECK(p->large->small == p, "enlaces siguiente/anterior inconsistentes");
    CHECK(p->small->large == p, "enlaces anterior/siguiente inconsistentes");
    for(int j = 0; j < i; ++j) {
      if(seen[j] == p) {
        CHECK(0, "ciclo prematuro/repeticion de un nodo");
        return;
      }
    }
    seen[i] = p;
    if(originals != NULL) {
      int found = 0;
      for(int j = 0; j < n; ++j) if(p == originals[j]) found = 1;
      CHECK(found, "se agrego un nodo que no pertenecia al arbol");
    }
    p = p->large;
  }
  CHECK(p == head, "la lista no cierra exactamente en n pasos");
  p = head;
  for(int i = n - 1; i >= 0; --i) {
    p = p->small;
    if(p == NULL) {
      CHECK(0, "enlace anterior NULL");
      return;
    }
    CHECK(p->data == want[i], "recorrido inverso incorrecto");
  }
  CHECK(p == head, "recorrido inverso no cierra");
}

static void collect(Node *root, Node **out, int *i) {
  if(root == NULL) return;
  out[(*i)++] = root;
  collect(root->small, out, i);
  collect(root->large, out, i);
}

static int testInsert(void) {
  Node *root = NULL;
  int input[] = {4, 2, 5, 1, 3, 2};
  int want[]  = {1, 2, 2, 3, 4, 5};
  for(int i = 0; i < 6; ++i) {
    int ok = treeInsert(&root, input[i]);
    CHECK(ok == 1, "treeInsert debe devolver 1 al insertar");
    if(!ok) break;
  }
  if(root != NULL) {
    int actual[6];
    int n = inorder(root, actual, 6);
    CHECK(n == 6, "numero incorrecto de nodos en el arbol");
    if(n == 6) CHECK(sameValues(actual, want, 6), "inorden incorrecto");
    CHECK(root->data == 4, "la primera insercion debe ser la raiz");
    freeTree(root);
  } else {
    CHECK(0, "treeInsert no modifico la raiz mediante Node **");
  }
  return failures;
}

static int testJoin(void) {
  Node *a = singleton(1);
  Node *b = singleton(2);
  join(a, b);
  CHECK(a->large == b, "join: a->large debe ser b");
  CHECK(b->small == a, "join: b->small debe ser a");
  free(a);
  free(b);
  return failures;
}

static int testAppend(void) {
  CHECK(append(NULL, NULL) == NULL, "append(NULL,NULL)");
  Node *a = singleton(1);
  CHECK(append(NULL, a) == a, "append(NULL,a) debe devolver a");
  CHECK(append(a, NULL) == a, "append(a,NULL) debe devolver a");
  int w1[] = {1};
  checkCircular(a, w1, 1, NULL);
  Node *b = singleton(2);
  Node *head = append(a, b);
  int w2[] = {1, 2};
  CHECK(head == a, "append(a,b) debe mantener la cabeza de a");
  checkCircular(head, w2, 2, NULL);
  if(head != NULL && head->large != NULL && head->large->large == head)
    freeList(head);
  else { free(a); free(b); }

  Node *c = singleton(3);
  Node *d = singleton(4);
  Node *e = singleton(5);
  Node *cd = append(c, d);
  Node *cde = cd != NULL ? append(cd, e) : NULL;
  int w3[] = {3, 4, 5};
  checkCircular(cde, w3, 3, NULL);
  if(cde != NULL && cde->large != NULL && cde->large->small == cde)
    freeList(cde);
  /* Si append falla, se omite liberar posibles nodos sueltos
     para no dereferenciar enlaces corruptos en el propio test. */
  return failures;
}

static void treeScenario(const int *input, const int *want, int n) {
  Node *root = NULL;
  for(int i = 0; i < n; ++i) {
    if(!treeInsert(&root, input[i])) {
      CHECK(0, "no fue posible construir el arbol de prueba");
      freeTree(root);
      return;
    }
  }
  Node *originals[64];
  int collected = 0;
  collect(root, originals, &collected);
  Node *head = treeToList(root);
  checkCircular(head, want, n, originals);
  /* La lista solo se libera si ha cerrado correctamente en ambos sentidos.
     Evita que un error de punteros desencadene un recorrido infinito. */
  if(head != NULL) {
    Node *p = head;
    int valid = 1;
    for(int i = 0; i < n; ++i) {
      if(p == NULL || p->large == NULL || p->small == NULL ||
         p->large->small != p || p->small->large != p) {
        valid = 0;
        break;
      }
      p = p->large;
    }
    if(valid && p == head) freeList(head);
  }
}

static int testTree(void) {
  CHECK(treeToList(NULL) == NULL, "treeToList(NULL) debe ser NULL");
  int a[] = {4, 2, 1, 3, 5};
  int x[] = {1, 2, 3, 4, 5};
  treeScenario(a, x, 5);
  int b[] = {7};
  treeScenario(b, b, 1);
  int c[] = {1, 2, 3, 4, 5};
  treeScenario(c, c, 5);
  int d[] = {5, 4, 3, 2, 1};
  treeScenario(d, x, 5);
  int e[] = {4, 2, 4, 1, 2, 7, 4};
  int y[] = {1, 2, 2, 4, 4, 4, 7};
  treeScenario(e, y, 7);
  return failures;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "Uso: %s insert|join|append|tree\n", argv[0]);
    return 2;
  }
  if(strcmp(argv[1], "insert") == 0) testInsert();
  else if(strcmp(argv[1], "join") == 0) testJoin();
  else if(strcmp(argv[1], "append") == 0) testAppend();
  else if(strcmp(argv[1], "tree") == 0) testTree();
  else { fprintf(stderr, "Prueba no reconocida\n"); return 2; }
  if(failures > 0) {
    fprintf(stderr, "Resultado: %d fallo(s) en %s\n", failures, argv[1]);
    return 1;
  }
  printf("OK: %s\n", argv[1]);
  return 0;
}
