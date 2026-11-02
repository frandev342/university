---
title: "Árboles AVL - Integración con binaryTree"
aliases:
  - "AVL en binaryTree"
  - "Guía de implementación AVL"
tags:
  - estructuras-de-datos
  - arboles
  - bst
  - avl
  - cpp
status: planificado
---

# Árboles AVL - Integración con `binaryTree`

> [!abstract] Objetivo
> Extender la implementación C++ del árbol binario de búsqueda para construir un AVL que conserve automáticamente una altura `O(log n)` mediante alturas almacenadas, factores de balance y rotaciones.

## Material relacionado

- `../docs/07 - AVL.pdf`: presentación utilizada como punto de partida.
- `tree.hpp`: definición actual de `Node`, `Tree` y la interfaz BST.
- `tree.cpp`: implementación actual de búsqueda, inserción, eliminación y consultas BST.
- `main.cpp`: pruebas actuales del árbol binario de búsqueda.
- `../notes/arboles-binarios-de-busqueda.md`: notas del tema anterior.

## 1. Alcance

Esta guía se aplica a la implementación C++ formada por:

```text
tree.hpp
tree.cpp
main.cpp
```

La implementación C de `tree.h` y `tree.c` se mantiene separada. Más adelante puede portarse el mismo algoritmo, pero primero se desarrollará una versión AVL sobre la estructura C++ que ya dispone de punteros al padre.

La organización propuesta es:

```text
binaryTree/
├── tree.hpp
├── tree.cpp
├── main.cpp
├── avl.hpp
├── avl.cpp
├── main_avl.cpp
└── AVL.md
```

`tree.hpp` y `tree.cpp` continúan conteniendo el BST general. `avl.hpp` y `avl.cpp` añaden las operaciones que mantienen el balance.

> [!important] Un AVL no es una función aislada
> Un AVL es un BST con un invariante adicional. Todas las operaciones que cambian su estructura deben mantener ese invariante. Las consultas existentes se pueden reutilizar, pero `insert` y `erase` deben tener versiones específicas para AVL.

## 2. Qué Es Un AVL

Un AVL cumple simultáneamente dos propiedades.

### Propiedad BST

La versión AVL admite claves repetidas. Debido a las rotaciones, para cada nodo `x` se utiliza la propiedad no estricta:

```text
todas las claves de x->left  son menores o iguales que x->key
todas las claves de x->right son mayores o iguales que x->key
```

### Propiedad AVL

Para cada nodo:

```text
abs(height(left) - height(right)) <= 1
```

Se conserva la convención ya utilizada en `tree.cpp` y en el PDF:

```text
altura de nullptr = -1
altura de una hoja = 0
```

La altura de un nodo es:

```text
1 + max(altura izquierda, altura derecha)
```

### Consecuencia

Un AVL con `n` nodos tiene altura:

```text
O(log n)
```

Por eso las operaciones que recorren un camino desde la raíz cuestan:

```text
search       O(log n)
minimum      O(log n)
maximum      O(log n)
successor    O(log n)
lower_bound  O(log n)
upper_bound  O(log n)
insert       O(log n)
erase        O(log n)
```

En un BST sin balance, esas operaciones pueden degradarse a `O(n)`.

## 3. Qué Puede Reutilizarse

Estas operaciones no modifican la estructura y pueden utilizarse también en un AVL:

```cpp
std::size_t tree_size(const Node* x);
Node* search(Node* x, int key);
Node* minimum(Node* x);
Node* maximum(Node* x);
Node* successor(Node* x);
Node* lower_bound(Node* x, int key);
Node* upper_bound(Node* x, int key);
void destroy(Tree& t);
```

También puede conservarse la función recursiva:

```cpp
int height(const Node* x);
```

Sin embargo, se utilizará principalmente como función de comprobación. No debe recalcularse recursivamente la altura completa en cada paso de una inserción o eliminación AVL.

Estas operaciones BST no deben utilizarse para modificar un árbol que se quiere conservar como AVL:

```cpp
int insert(Tree& t, int key);
void erase(Tree& t, Node* z);
```

Se reemplazan, para el uso AVL, por:

```cpp
int avl_insert(Tree& t, int key);
bool avl_erase(Tree& t, int key);
```

> [!warning] No mezclar APIs de modificación
> Después de crear un árbol mediante `avl_insert`, llamar al `insert` o `erase` BST puede romper el balance o dejar alturas almacenadas incorrectas.

## 4. Decisión Sobre Las Claves Repetidas

El PDF describe una propiedad BST que permite valores iguales. La versión AVL almacenará cada aparición en un nodo independiente, sin utilizar un contador.

```cpp
link = key <= parent->key ? &parent->left : &parent->right;
```

Durante la búsqueda del punto de inserción, una clave igual avanza inicialmente hacia el hijo izquierdo. La operación devuelve:

```text
-1 -> fallo de asignación
 1 -> inserción correcta
```

Una rotación conserva el recorrido in-order, pero puede colocar una clave igual a la derecha. Por eso "los iguales se insertan por la izquierda" es una política de inserción, no un invariante estructural permanente. Exigir que todos permanezcan siempre a la izquierda es incompatible con el balance AVL para entradas como `5, 5, 5`.

## 5. Altura Almacenada En Cada Nodo

La estructura actual es:

```cpp
struct Node {
  int key;
  Node* left = nullptr;
  Node* right = nullptr;
  Node* parent = nullptr;
};
```

Para mantener operaciones AVL eficientes se propone añadir:

```cpp
struct Node {
  int key;
  Node* left = nullptr;
  Node* right = nullptr;
  Node* parent = nullptr;
  int height = 0;
};
```

Una hoja nueva comienza con altura cero.

También debe actualizarse `newNode` para inicializar explícitamente el campo:

```cpp
Node* newNode(Node* parent, int key) {
  return new (std::nothrow) Node{key, nullptr, nullptr, parent, 0};
}
```

### Por Qué No Recalcular La Altura Recursivamente

Esta expresión es correcta matemáticamente:

```cpp
height(node->left) - height(node->right)
```

Pero la función actual `height` recorre todos los nodos del subárbol. Si se utiliza en cada antepasado después de cada inserción, el costo puede llegar a ser lineal.

Con una altura almacenada, consultar y actualizar un nodo cuesta `O(1)`:

```cpp
static int stored_height(const Node* node) {
  return node == nullptr ? -1 : node->height;
}
```

```cpp
static void update_height(Node* node) {
  const int left_height = stored_height(node->left);
  const int right_height = stored_height(node->right);
  node->height = 1 + std::max(left_height, right_height);
}
```

Para utilizar `std::max` se incluye:

```cpp
#include <algorithm>
```

> [!note] Dos conceptos de altura
> `node->height` es la altura almacenada y se consulta en `O(1)`. La función recursiva `height(node)` calcula la altura estructural real y puede utilizarse en pruebas para comprobar que el valor almacenado es correcto.

## 6. Factor De Balance

Se adoptará la convención:

```text
balance(node) = height(right) - height(left)
```

Implementación:

```cpp
static int balance_factor(const Node* node) {
  return stored_height(node->right) - stored_height(node->left);
}
```

Interpretación:

| Factor | Estado |
|---:|---|
| `-1` | Ligeramente pesado a la izquierda. |
| `0` | Ambos subárboles tienen la misma altura. |
| `1` | Ligeramente pesado a la derecha. |
| Menor que `-1` | Desbalanceado hacia la izquierda. |
| Mayor que `1` | Desbalanceado hacia la derecha. |

Un nodo cumple el invariante AVL cuando:

```cpp
-1 <= balance_factor(node) && balance_factor(node) <= 1
```

> [!warning] Convención de signos
> Esta guía sigue la convención `right - left` utilizada en el PDF. Un factor positivo significa que el lado derecho es más alto. La convención contraria también es válida, pero no deben mezclarse sus condiciones.

## 7. Rotaciones

Una rotación modifica localmente la estructura sin cambiar el recorrido in-order. Por tanto, conserva la propiedad BST.

Las dos primitivas necesarias son:

```cpp
Node* rotate_left(Tree& t, Node* x);
Node* rotate_right(Tree& t, Node* x);
```

Se recomienda mantenerlas como funciones privadas de `avl.cpp`:

```cpp
static Node* rotate_left(Tree& t, Node* x);
static Node* rotate_right(Tree& t, Node* x);
```

El valor retornado es la nueva raíz del subárbol rotado.

Cada rotación debe actualizar:

- El enlace del antiguo padre al subárbol.
- `t.root` cuando se rota la raíz completa.
- Los enlaces `left` y `right`.
- Los punteros `parent` de todos los nodos involucrados.
- Las alturas, primero del nodo que baja y después del nodo que sube.

## 8. Rotación Izquierda

Precondición:

```text
x != nullptr
x->right != nullptr
```

Transformación:

```text
      x                         y
     / \                       / \
    A   y       ---->         x   C
       / \                   / \
      B   C                 A   B
```

El nodo `y` sube, `x` baja a la izquierda y el subárbol `B` cambia de padre.

### Implementación Propuesta

```cpp
static Node* rotate_left(Tree& t, Node* x) {
  Node* y = x->right;
  Node* beta = y->left;
  Node* old_parent = x->parent;

  y->parent = old_parent;

  if (old_parent == nullptr) {
    t.root = y;
  } else if (old_parent->left == x) {
    old_parent->left = y;
  } else {
    old_parent->right = y;
  }

  y->left = x;
  x->parent = y;

  x->right = beta;
  if (beta != nullptr)
    beta->parent = x;

  update_height(x);
  update_height(y);
  return y;
}
```

### Orden De Actualización De Alturas

Después de la rotación, `x` queda debajo de `y`. Por eso debe calcularse primero:

```cpp
update_height(x);
```

Luego puede calcularse correctamente:

```cpp
update_height(y);
```

## 9. Rotación Derecha

Precondición:

```text
x != nullptr
x->left != nullptr
```

Transformación:

```text
        x                     y
       / \                   / \
      y   C      ---->      A   x
     / \                       / \
    A   B                     B   C
```

### Implementación Propuesta

```cpp
static Node* rotate_right(Tree& t, Node* x) {
  Node* y = x->left;
  Node* beta = y->right;
  Node* old_parent = x->parent;

  y->parent = old_parent;

  if (old_parent == nullptr) {
    t.root = y;
  } else if (old_parent->left == x) {
    old_parent->left = y;
  } else {
    old_parent->right = y;
  }

  y->right = x;
  x->parent = y;

  x->left = beta;
  if (beta != nullptr)
    beta->parent = x;

  update_height(x);
  update_height(y);
  return y;
}
```

## 10. Los Cuatro Casos De Desbalance

Con la convención `right - left`, los casos son:

| Caso | Condición en `x` | Condición en el hijo | Reparación |
|---|---|---|---|
| LL | `balance(x) < -1` | `balance(x->left) <= 0` | Rotación derecha en `x`. |
| LR | `balance(x) < -1` | `balance(x->left) > 0` | Izquierda en el hijo y derecha en `x`. |
| RR | `balance(x) > 1` | `balance(x->right) >= 0` | Rotación izquierda en `x`. |
| RL | `balance(x) > 1` | `balance(x->right) < 0` | Derecha en el hijo e izquierda en `x`. |

### LL: Izquierda-Izquierda

```text
        30                 20
       /                  /  \
      20       ->        10  30
     /
    10
```

```cpp
return rotate_right(t, x);
```

### RR: Derecha-Derecha

```text
    10                      20
      \                    /  \
       20       ->        10  30
         \
          30
```

```cpp
return rotate_left(t, x);
```

### LR: Izquierda-Derecha

```text
        30              30              20
       /               /               /  \
      10      ->      20      ->      10  30
        \            /
         20         10
```

```cpp
rotate_left(t, x->left);
return rotate_right(t, x);
```

### RL: Derecha-Izquierda

```text
    10               10                  20
      \                \                /  \
       30      ->       20      ->      10  30
      /                   \
     20                    30
```

```cpp
rotate_right(t, x->right);
return rotate_left(t, x);
```

## 11. Operación Local `rebalance`

`rebalance` recibe la raíz de un subárbol cuyos hijos ya tienen alturas correctas.

Contrato:

```text
Entrada:
  x es válido
  las alturas de sus hijos son correctas
  los subárboles izquierdo y derecho son AVL

Salida:
  retorna la nueva raíz del subárbol
  el subárbol cumple la propiedad AVL
  los padres y alturas locales quedan actualizados
```

### Implementación Propuesta

```cpp
static Node* rebalance(Tree& t, Node* x) {
  update_height(x);
  const int balance = balance_factor(x);

  if (balance > 1) {
    if (balance_factor(x->right) < 0)
      rotate_right(t, x->right);
    return rotate_left(t, x);
  }

  if (balance < -1) {
    if (balance_factor(x->left) > 0)
      rotate_left(t, x->left);
    return rotate_right(t, x);
  }

  return x;
}
```

El orden de las condiciones garantiza que `x->right` existe cuando `balance > 1` y que `x->left` existe cuando `balance < -1`.

## 12. Interfaz Pública AVL

Archivo propuesto `avl.hpp`:

```cpp
#ifndef AVL_HPP
#define AVL_HPP

#include "tree.hpp"

int avl_insert(Tree& t, int key);
bool avl_erase(Tree& t, int key);
bool avl_valid(const Tree& t);

#endif // AVL_HPP
```

La interfaz expone operaciones del TDA, no detalles como rotaciones o factores de balance.

| Operación | Responsabilidad |
|---|---|
| `avl_insert` | Insertar una clave y restaurar el balance. |
| `avl_erase` | Eliminar una clave y restaurar el balance. |
| `avl_valid` | Verificar BST, padres, alturas y balance. |

## 13. Inserción AVL

La inserción se divide en dos fases.

### Fase 1: insertar como BST

Se busca el enlace nulo donde debe colocarse la clave:

```cpp
Node** link = &t.root;
Node* parent = nullptr;

while (*link != nullptr) {
  parent = *link;
  link = key <= parent->key ? &parent->left : &parent->right;
}
```

Luego se crea una hoja de altura cero:

```cpp
Node* inserted = newNode(parent, key);
if (inserted == nullptr)
  return -1;

*link = inserted;
```

### Fase 2: reparar hacia la raíz

La inserción solo puede cambiar las alturas de los antepasados del nuevo nodo. No es necesario revisar otras ramas.

```cpp
Node* current = inserted->parent;

while (current != nullptr) {
  Node* subtree_root = rebalance(t, current);
  current = subtree_root->parent;
}
```

Después de una rotación, `subtree_root` puede ser diferente de `current`. Para continuar correctamente se utiliza el padre de la nueva raíz del subárbol.

### Implementación Completa Propuesta

```cpp
int avl_insert(Tree& t, int key) {
  Node** link = &t.root;
  Node* parent = nullptr;

  while (*link != nullptr) {
    parent = *link;
    link = key <= parent->key ? &parent->left : &parent->right;
  }

  Node* inserted = newNode(parent, key);
  if (inserted == nullptr)
    return -1;

  *link = inserted;

  Node* current = inserted->parent;
  while (current != nullptr) {
    Node* subtree_root = rebalance(t, current);
    current = subtree_root->parent;
  }

  return 1;
}
```

### Complejidad

La búsqueda del punto de inserción recorre un camino de altura `O(log n)`. La reparación recorre como máximo el mismo camino y cada rotación cuesta `O(1)`.

```text
Tiempo: O(log n)
Espacio auxiliar iterativo: O(1)
```

## 14. Eliminación AVL

El PDF se concentra en inserción, pero un árbol deja de ser AVL si se utiliza la eliminación BST sin reparación. Para integrar completamente el TDA se necesita una eliminación específica.

La eliminación tiene tres fases:

```text
1. Encontrar la clave.
2. Eliminar como BST.
3. Rebalancear todos los antepasados afectados.
```

### Decisión De Interfaz

Se propone eliminar por clave:

```cpp
bool avl_erase(Tree& t, int key);
```

Esto permite una implementación sencilla para el caso de dos hijos: copiar la clave del sucesor y eliminar físicamente el sucesor.

> [!warning] Identidad de los nodos
> Cuando el nodo tiene dos hijos, el objeto encontrado puede conservarse con otra clave mientras se destruye el nodo sucesor. Por eso esta interfaz define la operación en términos de claves y no promete conservar punteros externos a nodos eliminados o modificados.

### Reducir A Un Nodo Con Cero O Un Hijo

```cpp
Node* target = search(t.root, key);
if (target == nullptr)
  return false;

if (target->left != nullptr && target->right != nullptr) {
  Node* next = minimum(target->right);
  target->key = next->key;
  target = next;
}
```

Después de este bloque, `target` tiene como máximo un hijo.

### Desconectar El Nodo

```cpp
Node* child = target->left != nullptr ? target->left : target->right;
Node* parent = target->parent;

if (child != nullptr)
  child->parent = parent;

if (parent == nullptr) {
  t.root = child;
} else if (parent->left == target) {
  parent->left = child;
} else {
  parent->right = child;
}

delete target;
```

El primer nodo cuya altura puede cambiar es `parent`.

### Reparar Hasta La Raíz

A diferencia de la inserción, una eliminación puede disminuir la altura de varios subárboles. Debe continuarse hasta la raíz incluso después de una rotación.

```cpp
Node* current = parent;

while (current != nullptr) {
  Node* subtree_root = rebalance(t, current);
  current = subtree_root->parent;
}
```

### Implementación Completa Propuesta

```cpp
bool avl_erase(Tree& t, int key) {
  Node* target = search(t.root, key);
  if (target == nullptr)
    return false;

  if (target->left != nullptr && target->right != nullptr) {
    Node* next = minimum(target->right);
    target->key = next->key;
    target = next;
  }

  Node* child = target->left != nullptr ? target->left : target->right;
  Node* parent = target->parent;

  if (child != nullptr)
    child->parent = parent;

  if (parent == nullptr) {
    t.root = child;
  } else if (parent->left == target) {
    parent->left = child;
  } else {
    parent->right = child;
  }

  delete target;

  Node* current = parent;
  while (current != nullptr) {
    Node* subtree_root = rebalance(t, current);
    current = subtree_root->parent;
  }

  return true;
}
```

### Complejidad

```text
Búsqueda: O(log n)
Localización del sucesor: O(log n)
Reparación: O(log n)
Tiempo total: O(log n)
Espacio auxiliar iterativo: O(1)
```

## 15. Validación Completa

Una función de pruebas AVL no debe comprobar únicamente el factor de balance. Debe verificar simultáneamente:

- La propiedad BST no estricta que permite claves repetidas.
- Los punteros al padre.
- La altura almacenada de cada nodo.
- El factor AVL de cada nodo.
- Que la raíz tenga `parent == nullptr`.

### Resultado Recursivo

La validación puede calcular la altura estructural real mientras recorre una sola vez el árbol.

```cpp
static bool validate_node(const Node* node,
                          const Node* expected_parent,
                          long long lower,
                          long long upper,
                          int& actual_height) {
  if (node == nullptr) {
    actual_height = -1;
    return true;
  }

  if (node->parent != expected_parent)
    return false;

  if (node->key < lower || node->key > upper)
    return false;

  int left_height;
  int right_height;

  if (!validate_node(node->left, node, lower, node->key, left_height))
    return false;

  if (!validate_node(node->right, node, node->key, upper, right_height))
    return false;

  actual_height = 1 + std::max(left_height, right_height);

  if (node->height != actual_height)
    return false;

  const int difference = right_height - left_height;
  return difference >= -1 && difference <= 1;
}
```

La interfaz pública sería:

```cpp
bool avl_valid(const Tree& t) {
  int actual_height;
  return validate_node(t.root,
                       nullptr,
                       std::numeric_limits<long long>::lowest(),
                       std::numeric_limits<long long>::max(),
                       actual_height);
}
```

Para esta implementación se incluye:

```cpp
#include <limits>
```

### Complejidad

```text
Tiempo: O(n)
Pila recursiva: O(h), que en un AVL es O(log n)
```

## 16. Archivo `avl.cpp` Propuesto

La organización interna esperada es:

```cpp
#include "avl.hpp"

#include <algorithm>
#include <limits>

static int stored_height(const Node* node);
static void update_height(Node* node);
static int balance_factor(const Node* node);

static Node* rotate_left(Tree& t, Node* x);
static Node* rotate_right(Tree& t, Node* x);
static Node* rebalance(Tree& t, Node* x);

static bool validate_node(const Node* node,
                          const Node* expected_parent,
                          long long lower,
                          long long upper,
                          int& actual_height);

int avl_insert(Tree& t, int key);
bool avl_erase(Tree& t, int key);
bool avl_valid(const Tree& t);
```

Las funciones `static` tienen enlace interno: son detalles privados de esta unidad de traducción y no forman parte de la interfaz pública.

## 17. Invariantes De Las Operaciones

### Invariante De Inserción

Mientras se asciende desde el nuevo nodo:

```text
los subárboles por debajo de current ya son AVL
sus alturas almacenadas son correctas
solo current y sus antepasados pueden necesitar actualización
```

Después de `rebalance(current)`, el subárbol retornado vuelve a cumplir las propiedades locales.

### Invariante De Eliminación

Mientras se asciende desde el padre del nodo eliminado:

```text
la estructura BST ya es correcta
los subárboles inferiores ya fueron reparados
current es el siguiente nodo cuya altura puede haber disminuido
```

La reparación continúa hasta que no queden antepasados.

### Invariante De Rotación

Antes y después de una rotación, el recorrido in-order contiene exactamente las mismas claves en el mismo orden.

Para la rotación izquierda:

```text
A < x < B < y < C
```

Antes:

```text
A, x, B, y, C
```

Después:

```text
A, x, B, y, C
```

La forma cambia, pero el orden BST se conserva.

## 18. Orden Recomendado De Implementación

No conviene implementar todas las operaciones al mismo tiempo.

### Etapa 1: representación y consultas locales

- [ ] Añadir `height` a `Node`.
- [ ] Actualizar `newNode` para crear hojas con altura cero.
- [ ] Crear `avl.hpp`.
- [ ] Implementar `stored_height`.
- [ ] Implementar `update_height`.
- [ ] Implementar `balance_factor`.

Pruebas iniciales:

```text
nullptr tiene altura -1
una hoja tiene altura 0
un nodo con una hoja izquierda tiene factor -1
un nodo con una hoja derecha tiene factor +1
```

### Etapa 2: rotaciones aisladas

- [ ] Implementar `rotate_left`.
- [ ] Implementar `rotate_right`.
- [ ] Probar rotaciones en la raíz.
- [ ] Probar rotaciones en un subárbol interno.
- [ ] Comprobar todos los punteros `parent`.
- [ ] Comprobar las alturas después de cada rotación.

### Etapa 3: reparación local

- [ ] Implementar `rebalance`.
- [ ] Probar LL.
- [ ] Probar RR.
- [ ] Probar LR.
- [ ] Probar RL.

### Etapa 4: inserción AVL

- [ ] Implementar la fase BST de `avl_insert`.
- [ ] Conectar la reparación hacia la raíz.
- [ ] Insertar claves repetidas como nodos independientes.
- [ ] Comprobar `avl_valid` después de cada inserción.

### Etapa 5: validación completa

- [ ] Comprobar propiedad BST.
- [ ] Comprobar punteros al padre.
- [ ] Comprobar alturas almacenadas.
- [ ] Comprobar factores de balance.

### Etapa 6: eliminación AVL

- [ ] Eliminar una hoja.
- [ ] Eliminar un nodo con un hijo.
- [ ] Eliminar un nodo con dos hijos.
- [ ] Eliminar la raíz.
- [ ] Reparar todos los antepasados.
- [ ] Vaciar completamente el árbol.

## 19. Plan De Pruebas

### Rotación LL

```cpp
avl_insert(tree, 30);
avl_insert(tree, 20);
avl_insert(tree, 10);
```

Resultado esperado:

```text
raíz = 20
hijo izquierdo = 10
hijo derecho = 30
```

### Rotación RR

```cpp
avl_insert(tree, 10);
avl_insert(tree, 20);
avl_insert(tree, 30);
```

Resultado esperado:

```text
raíz = 20
hijo izquierdo = 10
hijo derecho = 30
```

### Rotación LR

```cpp
avl_insert(tree, 30);
avl_insert(tree, 10);
avl_insert(tree, 20);
```

Resultado esperado:

```text
raíz = 20
hijo izquierdo = 10
hijo derecho = 30
```

### Rotación RL

```cpp
avl_insert(tree, 10);
avl_insert(tree, 30);
avl_insert(tree, 20);
```

Resultado esperado:

```text
raíz = 20
hijo izquierdo = 10
hijo derecho = 30
```

### Rotación Interna

Debe probarse una rotación cuyo nodo desbalanceado no sea la raíz. Este caso detecta errores al reconectar el subárbol con su antiguo padre.

Ejemplo:

```text
insertar: 50, 30, 70, 20, 10
```

La rotación ocurre dentro del subárbol izquierdo de `50`. La raíz global debe continuar siendo `50`.

### Secuencia Ordenada

```text
insertar: 1, 2, 3, ..., 1000
```

En un BST normal esta entrada produce una lista. En el AVL debe cumplirse:

```cpp
assert(avl_valid(tree));
```

Y la altura debe mantenerse proporcional a `log2(n)`.

### Claves Repetidas

```cpp
assert(avl_insert(tree, 10) == 1);
assert(avl_insert(tree, 10) == 1);
assert(tree_size(tree.root) == 2U);
assert(avl_valid(tree));
```

### Eliminación

Probar independientemente:

- Hoja.
- Nodo con hijo izquierdo.
- Nodo con hijo derecho.
- Nodo con dos hijos.
- Raíz con cero, uno y dos hijos.
- Clave inexistente.
- Eliminaciones que producen LL, RR, LR y RL.
- Extracción repetida hasta dejar el árbol vacío.

### Compatibilidad De Consultas

Después de cada secuencia deben seguir funcionando:

```cpp
search
minimum
maximum
successor
lower_bound
upper_bound
tree_size
height
```

## 20. Compilación Y Sanitizadores

Cuando existan `avl.cpp` y `main_avl.cpp`, compilar con advertencias estrictas:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion \
  tree.cpp avl.cpp main_avl.cpp -o avl_test
```

Ejecutar también con sanitizadores:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  tree.cpp avl.cpp main_avl.cpp -o avl_test_sanitize

./avl_test_sanitize
```

AddressSanitizer puede detectar:

- Accesos después de `delete`.
- Escrituras fuera de memoria válida.
- Fugas si no se llama a `destroy`.

UndefinedBehaviorSanitizer puede detectar otras operaciones inválidas, pero ninguno de los dos verifica por sí solo la propiedad AVL. Para eso se necesita `avl_valid`.

### Verificación Del Diseño De Esta Guía

Los algoritmos propuestos se transcribieron en una implementación temporal y se comprobaron con:

```text
los cuatro casos LL, RR, LR y RL
inserción ordenada de las claves 1..1000
validación después de cada inserción
eliminación de todas las claves pares
eliminación posterior de todas las claves impares
validación después de cada eliminación
AddressSanitizer
UndefinedBehaviorSanitizer
```

Todas las comprobaciones finalizaron correctamente. Esto valida el diseño descrito, pero no marca las etapas anteriores como completadas: la integración real en `tree.hpp`, `avl.hpp` y `avl.cpp` todavía debe desarrollarse de forma incremental.

## 21. Errores Frecuentes

### Actualizar Solo Los Hijos

Una rotación también debe actualizar el enlace del padre anterior o `t.root`. Si se omite, el subárbol rotado puede quedar desconectado.

### Olvidar El Subárbol Intermedio

En una rotación izquierda, `y->left` no puede perderse. Ese subárbol, llamado `beta`, pasa a ser `x->right`.

```cpp
x->right = beta;
```

También debe actualizarse:

```cpp
beta->parent = x;
```

cuando `beta` no es nulo.

### Actualizar Alturas En Orden Incorrecto

Después de una rotación se actualiza primero el nodo que baja y luego el que sube.

### Continuar Desde Un Nodo Antiguo

Después de `rebalance`, la raíz local puede cambiar. El ascenso debe continuar con:

```cpp
current = subtree_root->parent;
```

No debe suponerse que `current->parent` conserva el mismo significado después de una rotación.

### Utilizar La Convención De Balance Equivocada

Si `balance = right - left`, entonces:

```text
balance > 1  -> pesado a la derecha
balance < -1 -> pesado a la izquierda
```

Invertir estas decisiones produce rotaciones incorrectas.

### Calcular Alturas Recursivamente En Cada Nodo

Puede producir una estructura balanceada, pero pierde la complejidad `O(log n)` de las actualizaciones AVL.

### Mezclar `insert` Con `avl_insert`

`insert` no actualiza alturas ni rota. Una sola llamada puede invalidar los metadatos y el balance.

### Mezclar `erase` Con `avl_erase`

`erase` conserva la propiedad BST, pero no restaura la propiedad AVL.

### Validar Solo La Raíz

Que la raíz tenga factor `-1`, `0` o `1` no garantiza que todos los subárboles estén balanceados. La validación debe recorrer todos los nodos.

## 22. Complejidades Finales

| Operación | AVL | BST degenerado sin balance |
|---|---:|---:|
| `search` | `O(log n)` | `O(n)` |
| `minimum` / `maximum` | `O(log n)` | `O(n)` |
| `successor` | `O(log n)` | `O(n)` |
| `lower_bound` / `upper_bound` | `O(log n)` | `O(n)` |
| `avl_insert` | `O(log n)` | No aplica |
| `avl_erase` | `O(log n)` | No aplica |
| Rotación individual | `O(1)` | No aplica |
| `avl_valid` | `O(n)` | No aplica |
| `tree_size` | `O(n)` | `O(n)` |
| `destroy` | `O(n)` | `O(n)` |

El almacenamiento adicional es un entero por nodo:

```cpp
int height;
```

Las operaciones iterativas de inserción y eliminación utilizan espacio auxiliar `O(1)`. La validación y destrucción recursivas utilizan pila `O(h)`, que en un AVL es `O(log n)`.

## 23. Primera Meta De Desarrollo

La primera etapa debe limitarse a:

```text
1. Añadir la altura almacenada a Node.
2. Crear avl.hpp y avl.cpp.
3. Implementar stored_height.
4. Implementar update_height.
5. Implementar balance_factor.
6. Probar manualmente estos tres auxiliares.
```

Después se implementará una rotación a la vez, primero de forma aislada y solo entonces se conectará `rebalance` con `avl_insert`.

> [!success] Diseño propuesto
> El proyecto puede reutilizar su BST actual, pero las mutaciones AVL se mantienen separadas. Las consultas son comunes; `avl_insert` y `avl_erase` son responsables de conservar alturas, padres y balance después de cada modificación.
