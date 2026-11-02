---
title: "Árboles rojinegros - Implementación en C11"
aliases:
  - "Árbol rojinegro en C"
  - "Red-black tree C11"
tags:
  - estructuras-de-datos
  - arboles
  - bst
  - balance
  - arboles-rojinegros
  - c11
status: desarrollado
---
p
# Árboles rojinegros - Implementación en C11

> [!abstract] Objetivo
> Comprender las cinco propiedades de Cormen, la cota de altura, las rotaciones y las reparaciones de inserción y eliminación mediante la implementación **C11** de `red-black-trees/c/red_black_tree.c`. Incluye un guion independiente para explicarla en 5–8 minutos.

## Material Relacionado

- `docs/tree-black-trees-Cormen.pdf`: referencia principal para propiedades, algoritmos y demostración.
- `docs/rojinegros.pdf` y `docs/rojinegros2.pdf`: referencias complementarias.
- [[arboles-binarios-de-busqueda]]: orden BST y operaciones básicas.
- [[arboles-rojinegros]]: nota equivalente centrada en la versión C++11.
- `red-black-trees/c/red_black_tree.h`: API pública C iterativa.
- `red-black-trees/c/red_black_tree.c`: nodos, centinela y algoritmos iterativos.
- `red-black-trees/c/main.c`: pruebas y ejemplo ejecutable.
- `red-black-trees/c_recursive/`: misma API C11 con descenso y reparaciones recursivos.
- `red-black-trees/MAKE_COMMANDS.md`: todos los objetivos de `make`.

> [!note] Convención
> Seguimos los cinco invariantes y el centinela `T.nil` de Cormen. Las claves `int` son **únicas**: la API representa un conjunto, no un multiconjunto.

## 1. El Problema De Un BST Ordinario

En un árbol binario de búsqueda (BST), cada nodo divide las claves:

```text
izquierda < nodo < derecha
```

Buscar, insertar y borrar recorren a lo sumo la altura `h`, con costo `O(h)`. Un BST corriente no limita esa altura: insertar `10, 20, 30, 40` produce una cadena de altura `n - 1`.

```text
10
  \
   20
     \
      30
        \
         40
```

En una cadena, las operaciones cuestan `O(n)`. El árbol rojinegro conserva el orden BST y añade colores con restricciones: desde un mismo nodo, los caminos descendentes hasta `NIL` no pueden diferir en longitud por más de un factor dos. Así obtiene altura `O(log n)` sin exigir subárboles geométricamente iguales.

## 2. Definición

Un árbol rojinegro es un BST donde cada nodo tiene color rojo o negro. Su representación C expresa los colores así:

```c
typedef enum {
  RB_RED,
  RB_BLACK
} RBColor;
```

Este `enum` está en `red_black_tree.c`: un usuario del TDA no modifica los colores directamente.

### Las Cinco Propiedades

1. Todo nodo es rojo o negro.
2. La raíz es negra.
3. Todas las hojas externas `NIL` son negras.
4. Los hijos de un nodo rojo son negros: no hay dos rojos consecutivos.
5. Para cualquier nodo, todos los caminos simples hacia hojas descendientes `NIL` contienen igual número de nodos negros.

> [!important] ¿Qué es una hoja `NIL`?
> Es una posición externa sin clave, no una hoja real con dato. Cada hijo ausente de un nodo real termina en `NIL`. En los diagramas se suele omitir, pero participa en el cómputo de negros.

### Ejemplo Válido

```text
       38B
      /   \
    19R   41B
   /   \
 12B   31B
 /
8R
```

Aquí `B` significa negro y `R` rojo. Cada enlace no dibujado conduce a un `NIL` negro. No hay dos rojos seguidos; todos los caminos desde `38` hacia `NIL` contienen el mismo número de negros.

### Ejemplos Inválidos

```text
10R                  raíz roja

  20B
  /
10R                  dos rojos consecutivos
/
5R

    20B
   /   \
 10B   30R            alturas negras diferentes
 /
5B
```

En el último ejemplo, los caminos que pasan por `5B` tienen más negros que los que terminan junto a `30R`.

## 3. El Centinela `NIL`

Cormen usa un centinela negro compartido en cada árbol. En C11 está **embebido** en la estructura privada:

```c
struct RBTree {
  RBNode nil;
  RBNode *root;
  size_t size;
};
```

`rb_tree_create()` reserva el árbol con `malloc`, inicializa el centinela y deja la raíz vacía:

```c
RBTree *tree = malloc(sizeof *tree);
if (tree == NULL)
  return NULL;

tree->nil.color = RB_BLACK;
tree->nil.left = &tree->nil;
tree->nil.right = &tree->nil;
tree->nil.parent = &tree->nil;
tree->root = &tree->nil;
tree->size = 0U;
```

El árbol vacío cumple `tree->root == &tree->nil`. En este proyecto, para comprobar que falta un hijo se usa:

```c
if (node->left == &tree->nil) {
  /* No hay hijo izquierdo real. */
}
```

No se compara con `NULL`: cada enlace ausente válido apunta al centinela.

### Ventajas

- Podemos consultar `child->color` incluso si el hijo es externo.
- Inserción y borrado tienen menos excepciones por punteros nulos.
- `rb_tree_empty` equivale a comparar raíz y centinela.
- Todas las hojas externas son negras por construcción.

### Precaución Con El Padre

La función privada `transplant` puede asignar temporalmente al **único** centinela el padre de la posición borrada:

```c
new_root->parent = old_root->parent;
```

`erase_fixup` necesita ese padre cuando `new_root` es `&tree->nil`. Al terminar `rb_tree_erase`, se restaura:

```c
tree->nil.parent = &tree->nil;
tree->nil.color = RB_BLACK;
```

> [!warning] No copiar el árbol por valor
> El centinela contiene enlaces a su propia dirección. Por eso `RBTree` es opaco en `red_black_tree.h`: se usa mediante el puntero devuelto por `rb_tree_create()`, sin copiar ni trasladar su representación interna.

## 4. Altura Negra

En la notación de Cormen, `bh(x)` es el número de nodos negros en cualquier camino descendente desde `x` hasta una hoja `NIL`, **sin contar a `x`**. Por la propiedad 5, el valor es independiente del camino elegido.

```text
bh(NIL) = 0
```

Si `x` es una hoja real, `bh(x) = 1`: el camino descendente contiene el `NIL` negro.

### Lema De Tamaño

Un subárbol con raíz `x` y altura negra `bh(x)` tiene al menos:

```text
2^bh(x) - 1 nodos internos
```

**Inducción:** para `NIL`, `bh = 0` y hay `2^0 - 1 = 0` nodos internos. Para un nodo real, cada hijo tiene altura negra `bh(x)` si es rojo y `bh(x) - 1` si es negro; en cualquier caso posee al menos `bh(x) - 1`. Sumamos el mínimo de ambos subárboles más su raíz:

```text
1 + 2(2^(bh(x) - 1) - 1) = 2^bh(x) - 1
```

### Cota De Altura

La propiedad 4 impide rojos consecutivos: en un camino descendente de altura `h`, al menos la mitad de los niveles aportan negros. Por ello `bh(root) >= h/2`. Si `n` es el número de nodos reales:

```text
n >= 2^bh(root) - 1
n >= 2^(h/2) - 1
h <= 2 log2(n + 1)
```

> [!success] Resultado
> La altura es `O(log n)`; búsqueda, inserción y borrado avanzan por caminos logarítmicos. En C, además debemos considerar que `rb_tree_height()` calcula la altura recorriendo **todos** los nodos: esa consulta diagnóstica tarda `O(n)`.

## 5. Rotaciones

Una rotación reorganiza un número constante de enlaces sin cambiar el orden in-order.

### Rotación Izquierda

```text
      x                        y
     / \                      / \
    A   y     rotate_left     x   C
       / \        ---->       / \
      B   C                  A   B
```

Antes y después se mantiene `A < x < B < y < C`. En C, la función privada recibe explícitamente el árbol porque debe poder actualizar su raíz:

```c
static void rotate_left(RBTree *tree, RBNode *node) {
  RBNode *pivot = node->right;
  node->right = pivot->left;

  if (pivot->left != &tree->nil)
    pivot->left->parent = node;

  pivot->parent = node->parent;
  if (node->parent == &tree->nil)
    tree->root = pivot;
  else if (node == node->parent->left)
    node->parent->left = pivot;
  else
    node->parent->right = pivot;

  pivot->left = node;
  node->parent = pivot;
}
```

### Rotación Derecha

Es el espejo de la izquierda:

```text
      y                          x
     / \                        / \
    x   C    rotate_right      A   y
   / \          ---->             / \
  A   B                          B   C
```

En `rotate_right(tree, node)`, el pivote es `node->left`; si gira la raíz, `tree->root` debe apuntar al pivote.

### Qué Modifica Y Qué Conserva

Una rotación cuesta `O(1)` y conserva las claves y el orden BST. Cambia hijos, padres y posiblemente raíz. No arregla por sí misma los colores: `insert_fixup` y `erase_fixup` combinan rotación con recoloración.

> [!failure] Error habitual con punteros
> Modificar `node->left` o `node->right` sin corregir el `parent` del subárbol transferido. `rb_tree_valid()` detecta padres inconsistentes.

## 6. Inserción

`rb_tree_insert(tree, key)` busca primero como en un BST y, si la clave no existe, llama a la función privada `newNode`:

```c
while (current != &tree->nil) {
  parent = current;
  if (key == current->key)
    return 0; /* duplicado: no reserva nada */
  current = key < current->key ? current->left : current->right;
}

inserted = newNode(tree, parent, key);
if (inserted == NULL)
  return -1;
```

`newNode` reserva e inicializa un nodo rojo aislado, sin conectarlo aún al árbol:

```c
static RBNode *newNode(RBTree *tree, RBNode *parent, int key) {
  RBNode *node = malloc(sizeof *node);
  if (node == NULL)
    return NULL;

  node->key = key;
  node->color = RB_RED;
  node->left = &tree->nil;
  node->right = &tree->nil;
  node->parent = parent;
  return node;
}
```

En el archivo real, `inserted` se declara antes del bucle. Si la clave está repetida no se llama a `newNode`; si falla la reserva se devuelve `-1` sin enlazar nada.

### Por Qué El Nodo Nuevo Es Rojo

Insertarlo negro aumentaría solamente la altura negra de los caminos que pasan por él. Insertarlo rojo mantiene la altura negra inicial; los problemas posibles son una raíz roja o un padre rojo. La reparación acaba siempre pintando la raíz de negro.

### Inserción BST Del Proyecto

El enlace nuevo se asigna a `tree->root`, `parent->left` o `parent->right`. Solo **después** de reservar e instalar el nodo se hace `++tree->size` y se llama a `insert_fixup(tree, inserted)`.

La función devuelve `1` si insertó, `0` si encontró un duplicado, `-1` si `tree == NULL` o falló `malloc`. En estos casos sin inserción no cambia el árbol válido existente.

### Estado Del Bucle De Reparación

```c
while (node->parent->color == RB_RED) {
  /* estudiar al tío respecto del abuelo */
}
tree->root->color = RB_BLACK;
```

El centinela negro detiene el bucle al llegar a la raíz. Si el padre es rojo, existe un abuelo real. Explicamos primero el caso donde el padre es hijo izquierdo; la otra rama de `insert_fixup` lo refleja simétricamente.

### Caso 1: Tío Rojo

```text
       G(B)                G(R)
      /   \               /   \
    P(R)  U(R)    ->     P(B)  U(B)
   /                    /
 Z(R)                 Z(R)
```

Se recolorean padre, tío y abuelo; se continúa desde el abuelo:

```c
node->parent->color = RB_BLACK;
uncle->color = RB_BLACK;
node->parent->parent->color = RB_RED;
node = node->parent->parent;
```

El número de negros visto desde el padre de `G` se conserva; podría aparecer una infracción rojo-rojo más arriba.

### Caso 2: Tío Negro Y Triángulo

Si `P` es hijo izquierdo de `G` y `Z` es hijo derecho de `P`:

```text
      G(B)                    G(B)
     /                       /
   P(R)      girar P       Z(R)
     \        ---->         /
     Z(R)                 P(R)
```

La rotación izquierda en el padre transforma el triángulo en la línea del caso 3:

```c
node = node->parent;
rotate_left(tree, node);
```

### Caso 3: Tío Negro Y Línea

```text
        G(B)                 P(B)
       /                    /   \
     P(R)       ->        Z(R)  G(R)
     /
   Z(R)
```

Se recolorea y se gira a la derecha el abuelo:

```c
node->parent->color = RB_BLACK;
node->parent->parent->color = RB_RED;
rotate_right(tree, node->parent->parent);
```

### Casos Espejo

Si el padre es hijo derecho de `G`, el tío es hijo izquierdo: se intercambian `left` y `right`, y `rotate_left` por `rotate_right`. El código C incluye ambas ramas explícitas.

### Límite De Trabajo

El caso 1 puede continuar hacia la raíz; los casos 2 y 3 finalizan con un máximo de **dos rotaciones** en total. Como la altura es `O(log n)`, la inserción también cuesta `O(log n)`.

## 7. Ejemplo De Inserción De Cormen

`c/main.c` usa exactamente:

```c
const int keys[] = {41, 38, 31, 12, 19, 8};
```

### Insertar `41`

Se inserta rojo como raíz y `insert_fixup` lo pinta negro:

```text
41B
```

### Insertar `38`

Su padre `41B` es negro, por lo que no hace falta girar:

```text
  41B
 /
38R
```

### Insertar `31`

`31R` y `38R` son rojos consecutivos. El tío es `NIL` negro, caso 3 izquierda-izquierda: rotación derecha en `41`.

```text
   38B
  /   \
31R   41R
```

### Insertar `12`

El padre `31R` y tío `41R` son rojos: caso 1. Ambos pasan a negro; `38` acaba negro por ser raíz.

```text
   38B
  /   \
31B   41B
/
12R
```

### Insertar `19`

Se forma `31-12-19`: tío negro y triángulo, caso 2 (girar a la izquierda en `12`), seguido del caso 3 (recolorear y girar a la derecha en `31`).

```text
      38B
     /   \
   19B   41B
  /   \
12R   31R
```

### Insertar `8`

El padre `12R` y tío `31R` son rojos: recoloración del caso 1.

```text
       38B
      /   \
    19R   41B
   /   \
 12B   31B
 /
8R
```

`rb_tree_print(tree, stdout)` dibuja el mismo árbol girado 90 grados; el hijo derecho queda arriba:

```text
    B:41
B:38
        B:31
    R:19
        B:12
            R:8
```

## 8. Eliminación

El borrado primero mantiene la estructura BST y luego repara la posible pérdida de altura negra.

### Etapa BST

`rb_tree_erase` busca `target` con `find_node`; devuelve `false` si no existe. Después:

1. Si falta el hijo izquierdo, lo reemplaza por el derecho.
2. Si falta el derecho, lo reemplaza por el izquierdo.
3. Si tiene dos hijos, busca `minimum(tree, target->right)` y traslada ese sucesor a la posición del nodo buscado.

La función privada `transplant(tree, old_root, new_root)` conecta al padre anterior con el nuevo subárbol. El sucesor recibe el color original de `target` al ocupar su posición.

### El Color Que Importa

Se guarda el color **del nodo que abandona su posición original**, que puede ser el sucesor y no `target`:

```c
RBColor removed_color = moved->color;
```

Después de `free(target)` se decrementa `tree->size`. Si el color retirado era negro, se llama a `erase_fixup(tree, replacement)`; si era rojo, la reparación no hace falta.

### El Concepto De Negro Extra

Si se elimina un negro, los caminos por esa posición parecen perder una unidad de altura negra. El **negro extra** es una idea del algoritmo, no un tercer valor de `RBColor`: el puntero `replacement` señala desde dónde se repara el déficit. Puede señalar al centinela.

Explicamos los casos para un reemplazo hijo izquierdo; los del lado derecho están reflejados en `erase_fixup`.

### Caso 1: Hermano Rojo

Se vuelve negro el hermano, rojo el padre y se gira el padre a la izquierda:

```c
sibling->color = RB_BLACK;
node->parent->color = RB_RED;
rotate_left(tree, node->parent);
sibling = node->parent->right;
```

El nuevo hermano es negro; el caso 1 prepara otro de los casos siguientes, no concluye el borrado.

### Caso 2: Hermano Negro Con Dos Hijos Negros

El hermano pasa a rojo y el déficit asciende al padre:

```c
sibling->color = RB_RED;
node = node->parent;
```

Puede repetirse a lo largo de varios antepasados. Es el único caso que propaga el negro extra hacia arriba.

### Caso 3: Hermano Negro, Hijo Cercano Rojo Y Lejano Negro

Con reemplazo izquierdo, el hijo cercano es `sibling->left` y el lejano es `sibling->right`. Se recolorea y gira el hermano a la derecha para preparar el caso 4:

```c
sibling->left->color = RB_BLACK;
sibling->color = RB_RED;
rotate_right(tree, sibling);
sibling = node->parent->right;
```

### Caso 4: Hermano Negro Con Hijo Lejano Rojo

Se distribuyen colores y se gira el padre a la izquierda; el déficit desaparece:

```c
sibling->color = node->parent->color;
node->parent->color = RB_BLACK;
sibling->right->color = RB_BLACK;
rotate_left(tree, node->parent);
node = tree->root;
```

### Resumen De Los Cuatro Casos

| Caso | Configuración | Reparación |
|---|---|---|
| 1 | Hermano rojo | Recolorear y girar el padre. |
| 2 | Hermano negro, dos hijos negros | Pintar hermano rojo y subir. |
| 3 | Hermano negro, cercano rojo y lejano negro | Girar el hermano. |
| 4 | Hermano negro, lejano rojo | Recolorear, girar el padre y terminar. |

Si el reemplazo es derecho, `left` y `right` se intercambian. La reparación tarda `O(log n)` y necesita como máximo **tres rotaciones**.

> [!warning] Orden importante en C
> `target` se libera con `free(target)` antes de `erase_fixup`. La reparación debe usar `replacement` y sus enlaces válidos, nunca desreferenciar el puntero ya liberado. El centinela recupera color negro y padre propio al finalizar.

## 9. Diseño Del Proyecto C11

### API Pública Y Tipo Opaco

`red_black_tree.h` declara, sin mostrar la estructura interna:

```c
typedef struct RBTree RBTree;

RBTree *rb_tree_create(void);
void rb_tree_destroy(RBTree *tree);
int rb_tree_insert(RBTree *tree, int key);
bool rb_tree_erase(RBTree *tree, int key);
bool rb_tree_contains(const RBTree *tree, int key);
size_t rb_tree_size(const RBTree *tree);
bool rb_tree_empty(const RBTree *tree);
int rb_tree_height(const RBTree *tree);
size_t rb_tree_inorder(const RBTree *tree, int *output, size_t capacity);
bool rb_tree_valid(const RBTree *tree);
void rb_tree_print(const RBTree *tree, FILE *output);
```

Como `RBTree` está incompleto en el encabezado, un cliente no puede declararlo por valor, tomar su `sizeof`, editar colores ni copiar su centinela accidentalmente.

La carpeta `c_recursive/` mantiene estas mismas firmas y contratos. En vez del bucle de búsqueda usa `search(tree, node, key)` recursivo; `insert_at` desciende recursivamente usando `Node **link`; el caso de tío rojo llama de nuevo a `insert_fixup` con el abuelo y el caso 2 del borrado llama a `erase_fixup` con el padre. La raíz, los colores y los padres siguen reparándose en cada actualización; una recursión sencilla de inserción BST sin esas reparaciones no sería rojinegra.

### Nodo Interno

`red_black_tree.c` define:

```c
typedef struct RBNode {
  int key;
  RBColor color;
  struct RBNode *left;
  struct RBNode *right;
  struct RBNode *parent;
} RBNode;
```

El centinela `nil` es **un miembro de `RBTree`**, mientras cada nodo real se reserva aparte con `malloc`.

### Propiedad De Recursos

En C no hay destructor automático. El ciclo de vida debe escribirse expresamente:

```c
#include "red_black_tree.h"

int main(void) {
  RBTree *tree = rb_tree_create();
  if (tree == NULL)
    return 1;

  if (rb_tree_insert(tree, 41) != 1) {
    rb_tree_destroy(tree);
    return 1;
  }

  rb_tree_print(tree, stdout);
  rb_tree_destroy(tree);
  return 0;
}
```

`rb_tree_destroy` libera recursivamente los nodos reales y luego la propia estructura `RBTree`. El centinela embebido **no** recibe un `free` individual. Después de destruir, no debe usarse ese puntero. `rb_tree_destroy(NULL)` no hace nada.

### Contratos Públicos

| Operación C | Resultado |
|---|---|
| `rb_tree_create()` | Árbol vacío o `NULL` si falla `malloc`. |
| `rb_tree_insert(tree, key)` | `1` insertado; `0` duplicado; `-1` árbol `NULL` o fallo de memoria. |
| `rb_tree_erase(tree, key)` | `true` si eliminó; `false` si no encontró la clave o `tree == NULL`. |
| `rb_tree_contains(tree, key)` | Pertenencia al conjunto; `false` si `tree == NULL`. |
| `rb_tree_size(tree)` | Número de claves; `0` si `tree == NULL`. |
| `rb_tree_empty(tree)` | `true` si está vacío o `tree == NULL`. |
| `rb_tree_height(tree)` | Altura en aristas; `-1` si está vacío o `tree == NULL`. |
| `rb_tree_inorder(tree, output, capacity)` | Cantidad total de claves; escribe como máximo `capacity` si `output != NULL`. |
| `rb_tree_valid(tree)` | Diagnóstico de invariantes; `false` si `tree == NULL`. |
| `rb_tree_print(tree, output)` | Imprime en un `FILE *` válido; no imprime si algún puntero es `NULL`. |

Uso correcto del arreglo de salida:

```c
size_t total = rb_tree_inorder(tree, NULL, 0U);
int first_two[2];
size_t reported = rb_tree_inorder(tree, first_two, 2U);
/* reported == total; first_two contiene solo las primeras min(total, 2) claves. */
```

Si se quiere almacenar **todas** las claves, hay que proporcionar un arreglo con capacidad suficiente. El valor devuelto no indica cuántos elementos se escribieron, sino cuántos hay en total. Mientras se leen las claves, no se debe modificar el árbol entre el cálculo de tamaño y el recorrido.

## 10. Validación De Invariantes

`rb_tree_valid(tree)` sirve para pruebas; no balancea ni sustituye `insert_fixup` o `erase_fixup`. Comprueba en `O(n)`:

1. Centinela negro, con hijos y padre propios en el estado externo.
2. Raíz no nula y negra, con el centinela por padre.
3. Orden BST **estricto** en todo el subárbol.
4. Punteros al padre correctos.
5. Ausencia de hijos rojos para cada nodo rojo.
6. Igual altura negra en ambos subárboles de todo nodo.
7. Total de nodos recorridos igual a `tree->size`.

### Límites De Orden

La validación pasa intervalos hacia abajo; comparar solo con el padre inmediato no bastaría. `long long` permite establecer extremos estrictos `LLONG_MIN` y `LLONG_MAX` sin calcular `key + 1`, que podría desbordar para `INT_MAX`:

```c
validate_node(tree, node->left, node, lower,
              (long long)node->key, node_count, &left_black_height);
validate_node(tree, node->right, node, (long long)node->key,
              upper, node_count, &right_black_height);
```

### Altura Negra En El Validador

La función auxiliar **incluye** el `NIL` negro en su contador:

```c
if (node == &tree->nil) {
  *black_height = 1;
  return true;
}
```

Después compara ambos lados y suma el nodo actual solo si es negro:

```c
if (left_black_height != right_black_height)
  return false;
*black_height = left_black_height + (node->color == RB_BLACK ? 1 : 0);
```

> [!note] Dos convenciones compatibles
> La notación teórica `bh(x)` no incluye `x` y da `bh(NIL) = 0`. El contador del validador devuelve también el color del nodo actual y por eso toma `NIL = 1`. La comparación entre caminos sigue comprobando la misma propiedad 5.

## 11. Complejidades

| Operación | Tiempo | Espacio auxiliar |
|---|---:|---:|
| `rb_tree_contains` | `O(log n)` | `O(1)` |
| `rb_tree_insert` | `O(log n)` | `O(1)` más el nodo nuevo |
| `rb_tree_erase` | `O(log n)` | `O(1)` |
| Rotación | `O(1)` | `O(1)` |
| `rb_tree_inorder` | `O(n)` | `O(log n)` de pila, más arreglo del llamador |
| `rb_tree_height` | `O(n)` | `O(log n)` de pila |
| `rb_tree_valid` | `O(n)` | `O(h)` de pila; `O(log n)` si el árbol es válido |
| `rb_tree_destroy` | `O(n)` | `O(log n)` de pila si es válido |

El recorrido in-order **no** reserva el arreglo de salida: su capacidad es responsabilidad del llamador. La memoria del árbol completo es `O(n)` para nodos más un centinela por árbol. Durante el diagnóstico de un árbol ya corrupto y degenerado, la pila de `rb_tree_valid` podría llegar a `O(n)`.

## 12. Comparación Con AVL

| Aspecto | AVL | Rojinegro C11 |
|---|---|---|
| Información por nodo | Altura o factor de balance | Color (`RBColor`) |
| Regla | Diferencia de alturas hasta 1 | Misma altura negra, sin rojos consecutivos |
| Balance | Más estricto | Más flexible |
| Altura rojinegra | — | `h <= 2 log2(n + 1)` |
| Búsqueda | `O(log n)` | `O(log n)` |
| Inserción y borrado | `O(log n)` con alturas eficientes | `O(log n)` |
| Recursos en C | Según la implementación | `malloc`, `free`, API opaca |

AVL suele tener menor altura para muchas entradas; rojinegro admite más flexibilidad y limita a dos las rotaciones de inserción y a tres las de borrado. Ninguna opción es universalmente superior.

## 13. Pruebas Del Proyecto

`c/main.c` usa `CHECK(...)` para que las pruebas sigan activas aunque se compile con `NDEBUG`. Verifica:

- Árbol vacío y algunos contratos de `NULL`.
- Recorrido con capacidad menor que el tamaño total.
- Secuencia de Cormen `41, 38, 31, 12, 19, 8`.
- Rechazo de duplicados, eliminación de clave ausente, hoja, nodo con dos hijos y raíz.
- `INT_MIN` e `INT_MAX`.
- Inserción de `1` a `512`, comprobando la altura.
- Eliminación de pares y luego impares.
- Ocho órdenes pseudoaleatorios reproducibles de 160 claves.
- 5000 operaciones mezcladas contrastadas con un arreglo booleano de referencia.
- Las 120 órdenes de inserción combinadas con las 120 órdenes de eliminación para cinco claves (14 400 combinaciones).
- `rb_tree_valid` después de cada modificación de los recorridos principales.

Desde `UNIT2/red-black-trees/`:

```bash
make c           # compila solo C11
make test-c      # ejecuta pruebas y muestra el ejemplo
make sanitize-c  # ejecuta C11 con ASan y UBSan
make test-c-recursive     # ejecuta la variante recursiva
make sanitize-c-recursive # sanitiza la variante recursiva
```

Salida final:

```text
Cormen insertion example in C (right subtree is above):
    B:41
B:38
        B:31
    R:19
        B:12
            R:8
Red-black tree C11: OK
```

`make` y `make test` incluyen también la versión C++11; `make help` enumera todos los objetivos.

## 14. Errores Frecuentes

### Confundir `NIL` Con `NULL`

En un árbol válido `node->left == &tree->nil` indica hijo ausente; `NULL` no representa una hoja. Intentar consultar `node->left->color` si se inicializó a `NULL` provoca acceso inválido.

### Crear Un Árbol Sin Comprobar `malloc`

`rb_tree_create()` puede devolver `NULL`. Antes de usar el puntero hay que comprobarlo, y después liberar el árbol con `rb_tree_destroy()` exactamente una vez.

### Intentar Copiar O Liberar El Centinela

La dirección `&tree->nil` es propia de un árbol. El diseño opaco impide copiar la estructura; `destroy_nodes` se detiene en el centinela y no le hace `free` individual.

### Olvidar `parent` O `root` En Una Rotación

Los enlaces entre subárboles tienen dos direcciones. Cambiar un hijo sin actualizar el padre inverso, o no cambiar `tree->root`, rompe la estructura.

### Insertar Un Nodo Negro

La nueva ruta sumaría un negro solo de un lado. El nodo nuevo empieza rojo y `insert_fixup` elimina el posible par rojo-rojo.

### Olvidar Los Casos Espejo

Inserción contempla tres casos y sus reflejos; eliminación cuatro casos y sus reflejos. El parentesco izquierdo/derecho cambia qué sobrino es el cercano y cuál el lejano.

### Reparar Desde `target` Después De `free(target)`

Si se borró un negro, `erase_fixup` recibe `replacement`. Usar `target` después de liberar su memoria sería comportamiento indefinido.

### Tratar El Resultado De `rb_tree_inorder` Como Elementos Escritos

Devuelve la **cantidad total** de claves aunque el arreglo solo alcance para un prefijo. Acceder más allá de `capacity` sería un error del cliente.

### Confundir `bool` E `int` De Retorno

`rb_tree_erase` devuelve `bool`; `rb_tree_insert` devuelve `int` para distinguir duplicado y fallo. No comprobar `-1` como si significara éxito: en C cualquier entero no nulo es verdadero.

## 15. Guion De Exposición De 5 A 8 Minutos

**Duración objetivo: 7 minutos.** La teoría sigue a Cormen y los fragmentos mostrados corresponden a `c/red_black_tree.c` o `c/main.c`.

### 0:00–0:45 — Motivación

> Un BST puede degradarse a una cadena: si inserto `10, 20, 30, 40`, buscar deja de costar logarítmico y pasa a `O(n)`. El árbol rojinegro conserva la propiedad de búsqueda y añade una marca de color por nodo para limitar la altura.

Mostrar la cadena y la cota:

```text
BST degenerado: h = n - 1
Árbol rojinegro: h <= 2 log2(n + 1)
```

### 0:45–1:45 — Definición Y Cinco Propiedades

> Cada nodo es rojo o negro. La raíz es negra; las hojas externas NIL son negras; un rojo no puede tener hijos rojos; y todos los caminos desde un mismo nodo hasta NIL tienen igual número de negros. `NIL` no guarda clave: representa un hijo ausente.

Usar el árbol de Cormen y pedir que localicen `19R`, sus hijos negros y las posiciones externas `NIL`.

```text
       38B
      /   \
    19R   41B
   /   \
 12B   31B
 /
8R
```

> En C, los colores son `RB_RED` y `RB_BLACK`, definidos en un `enum` privado de la implementación.

### 1:45–2:35 — Altura Negra

> Se llama altura negra `bh(x)` al número de negros hasta una hoja externa sin contar `x`. Un subárbol de altura negra `b` necesita como mínimo `2^b - 1` nodos reales. Como entre dos negros no puede haber más de un rojo, la altura total es como máximo `2 log2(n + 1)`.

Mostrar la fórmula y concluir:

```text
search, insert, erase: O(log n)
```

> Es una garantía de altura, no de simetría perfecta.

### 2:35–3:20 — Rotaciones Y Punteros En C

> La rotación cambia unos pocos punteros, conserva el recorrido in-order y cuesta `O(1)`. En C la función recibe `RBTree *tree` para actualizar la raíz y corrige los `parent` de los subárboles transferidos.

```text
      x                    y
     / \                  / \
    A   y       ->       x   C
       / \              / \
      B   C            A   B
```

Señalar en el código `rotate_left(tree, node)` y el caso `node->parent == &tree->nil`.

### 3:20–5:15 — Inserción En C Y Tres Casos

> `rb_tree_insert` recorre el BST, devuelve `0` si encuentra duplicado y pide memoria con `malloc`. Si falla, devuelve `-1`. Si reserva el nodo, lo enlaza rojo y ejecuta `insert_fixup`.

```c
inserted->color = RB_RED;
inserted->left = &tree->nil;
inserted->right = &tree->nil;
```

> Rojo evita cambiar inmediatamente la altura negra. Si el padre también es rojo, hay tres casos: tío rojo, recolorear y subir; tío negro con triángulo, girar el padre; tío negro con línea, recolorear y girar el abuelo. La orientación contraria es el espejo.

Mostrar `41, 38, 31`:

```text
  41B                   38B
 /                     /   \
38R          ->       31R  41R
/
31R
```

> Insertar requiere como máximo dos rotaciones. `c/main.c` valida los invariantes después de insertar cada clave del ejemplo.

### 5:15–6:05 — API Opaca Y Memoria

> El encabezado declara `typedef struct RBTree RBTree;`, pero no expone los nodos. Para usar el árbol en C llamo a `rb_tree_create()`, compruebo `NULL` y al final llamo a `rb_tree_destroy()`.

Si se expone la variante recursiva, señalar que cambia el *mecanismo de descenso y propagación*, no los cinco invariantes ni los códigos de retorno. Puede ejecutarse con `make test-c-recursive`.

```c
RBTree *tree = rb_tree_create();
if (tree == NULL)
  return 1;

rb_tree_insert(tree, 41);
rb_tree_print(tree, stdout);
rb_tree_destroy(tree);
```

> Internamente hay un solo centinela negro `tree->nil` por árbol. Todos los hijos ausentes apuntan a él. El validador comprueba orden, color, padres, altura negra y tamaño.

### 6:05–6:40 — Borrado

> Se localiza el nodo como en un BST; con dos hijos se usa el sucesor. Si el color quitado físicamente era negro, llamamos a `erase_fixup` sobre el reemplazo: se analiza al hermano y sus hijos en cuatro casos. Durante la reparación el centinela puede tener un padre temporal; después se restaura.

> Borrar cuesta `O(log n)` y como máximo tres rotaciones. En C además liberamos el nodo con `free` y finalmente el árbol con `rb_tree_destroy`.

### 6:40–7:00 — Cierre Y Demostración

> Color y altura negra limitan la forma del BST; rotaciones conservan el orden y recoloraciones restauran los invariantes. La versión C11 muestra también la disciplina de comprobar reservas y liberar memoria.

Ejecutar desde `red-black-trees/`:

```bash
make test-c
```

Se debe ver el árbol con raíz negra `38` y el mensaje `Red-black tree C11: OK`.

## 16. Plan De Diapositivas

| Diapositiva | Contenido centrado en C11 | Tiempo |
|---:|---|---:|
| 1 | BST degenerado y motivación | 45 s |
| 2 | Cinco propiedades y `NIL` | 60 s |
| 3 | Altura negra y cota `2 log2(n + 1)` | 50 s |
| 4 | Rotaciones: `tree->root`, `parent` e in-order | 45 s |
| 5 | `malloc`, nodo rojo, tres casos de `insert_fixup` | 115 s |
| 6 | API opaca, `rb_tree_create`, `rb_tree_valid`, `rb_tree_destroy` | 50 s |
| 7 | Borrado, `free`, complejidad y cierre | 55 s |

Los diagramas explican mejor las rotaciones que una captura del `insert_fixup` completo. Para la diapositiva de C, mostrar solo la firma y el fragmento corto relevante.

## 17. Preguntas Probables

### ¿Por qué la raíz es negra?

Es una normalización: si una raíz queda roja, volverla negra suma un negro a **todos** los caminos desde ella y no rompe su igualdad de altura negra.

### ¿Por qué se inserta rojo?

Para no sumar un negro únicamente a los caminos que cruzan el nodo nuevo. Puede surgir un par rojo-rojo; los casos de `insert_fixup` lo resuelven.

### ¿Una rotación cambia los resultados de búsqueda?

No. Conserva el orden in-order y por ello la propiedad BST; únicamente cambia enlaces y alturas locales.

### ¿Qué significa `NIL` si en C existe `NULL`?

En este proyecto `NULL` es un puntero nulo o señal de fallo de asignación; `&tree->nil` es un **nodo válido en memoria**, negro y sin clave lógica. No son intercambiables.

### ¿Por qué `RBTree` no se declara en el encabezado?

Para proteger el invariante y evitar copiar por valor un objeto cuyo centinela apunta a sí mismo. Un cliente utiliza exclusivamente un `RBTree *` creado por la API.

### ¿Qué hago si `rb_tree_insert` devuelve `-1`?

Tratarlo como error: puede indicar árbol nulo o fallo al reservar memoria. El árbol válido previo no se modifica por esa inserción fallida. `0` significa duplicado y `1` éxito.

### ¿Qué es el negro extra del borrado?

Una cuenta conceptual del negro perdido en algunos caminos; no hay un tercer valor en `RBColor`.

### ¿Por qué `rb_tree_inorder` recibe un arreglo y no devuelve uno?

La API en C deja al llamador decidir cuánto almacenamiento proporciona. Escribe un prefijo de hasta `capacity` claves y devuelve el **tamaño total** para que el llamador detecte si su arreglo fue insuficiente.

### ¿Cuándo se libera el centinela?

No recibe `free` por separado: pertenece a la memoria del propio `RBTree`, que `rb_tree_destroy()` libera después de liberar los nodos reales.

### ¿Cuántas rotaciones hacen falta?

Como máximo dos para insertar y tres durante la reparación del borrado; los bucles de recoloración pueden avanzar más niveles sin rotar.

## 18. Lista De Comprobación

- [ ] Puedo enunciar las cinco propiedades y diferenciar nodo real de `NIL`.
- [ ] Sé obtener `h <= 2 log2(n + 1)` a partir de la altura negra.
- [ ] Puedo explicar por qué `RBTree` es opaco y cómo crear y destruirlo.
- [ ] Comparo hijos ausentes con `&tree->nil`, no con `NULL`.
- [ ] La raíz y los `parent` se actualizan en las rotaciones.
- [ ] Al reservar un nodo compruebo el resultado de `malloc`.
- [ ] Distingo los retornos `-1`, `0` y `1` de `rb_tree_insert`.
- [ ] Puedo explicar los tres casos de inserción y sus espejos.
- [ ] Puedo explicar el déficit negro y los cuatro casos de borrado.
- [ ] Entiendo por qué `transplant` cambia temporalmente `nil.parent`.
- [ ] No uso `target` después de `free(target)`.
- [ ] Entiendo que `rb_tree_inorder` devuelve el total, no la cantidad escrita.
- [ ] `make test-c` y `make sanitize-c` concluyen correctamente.

## 19. Resumen Para Repaso

```text
Rojinegro = BST + colores + raíz negra + NIL negro
           + nunca dos rojos consecutivos
           + misma altura negra en caminos descendentes

Consecuencia: h <= 2 log2(n + 1)

Representación C11:
  RBTree *tree = rb_tree_create();
  un centinela embebido tree->nil por árbol
  nodos reales creados con malloc, eliminados con free
  rb_tree_destroy(tree) libera todo

Insertar: BST -> nodo rojo -> recolorear/girar (3 casos + espejo)
Borrar:   BST/sucesor -> si se quitó negro, reparar (4 casos + espejo)

Contratos:
  insert: 1 éxito, 0 duplicado, -1 error
  erase:  true eliminado, false ausente
  inorder: escribe hasta capacity, devuelve tamaño total

Costos: contains/insert/erase O(log n)
        rotación O(1), validación/in-order/altura O(n)

Pruebas C11: make test-c; memoria y UB: make sanitize-c
```

> [!summary] Idea central
> El color limita la altura; las rotaciones preservan el orden y las recoloraciones preservan las rutas negras. En C, el TDA añade una responsabilidad explícita: reservar, verificar y liberar la memoria sin romper el centinela compartido.
