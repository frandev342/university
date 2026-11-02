# Dos implementaciones de árbol AVL

Este proyecto compara dos implementaciones funcionalmente equivalentes de un
árbol AVL con claves `int`, valores repetidos y punteros al padre.

## Estructura

```text
AVL/
├── recursive_height/
│   ├── avl.hpp
│   ├── avl.cpp
│   └── main.cpp
├── stored_height/
│   ├── avl.hpp
│   ├── avl.cpp
│   └── main.cpp
├── Makefile
└── README.md
```

## Variante `recursive_height`

`Node` no contiene una altura:

```cpp
struct Node {
  int key;
  Node* left;
  Node* right;
  Node* parent;
};
```

Cada factor de balance llama a una función recursiva:

```cpp
int height(const Node* node);
```

La implementación conserva correctamente el invariante AVL, pero una inserción
o eliminación puede costar `O(n)` porque vuelve a recorrer subárboles para medir
sus alturas.

## Variante `stored_height`

`Node` almacena su altura:

```cpp
struct Node {
  int key;
  Node* left;
  Node* right;
  Node* parent;
  int height;
};
```

Las alturas se actualizan al ascender y después de cada rotación. Consultar el
factor de balance cuesta `O(1)`, por lo que inserción y eliminación conservan
`O(log n)`.

## Interfaz común

Las dos variantes proporcionan:

```cpp
std::size_t tree_size(const Node* node);
int height(const Node* node);

Node* search(Node* node, int key);
Node* minimum(Node* node);
Node* maximum(Node* node);
Node* successor(Node* node);
Node* lower_bound(Node* node, int key);
Node* upper_bound(Node* node, int key);

int avl_insert(Tree& tree, int key);
bool avl_erase(Tree& tree, int key);
bool avl_valid(const Tree& tree);
void destroy(Tree& tree);
```

Los namespaces evitan confundir ambos tipos:

```text
avl_recursive
avl_stored
```

## Convención Del Factor De Balance

Ambos proyectos siguen la convención utilizada en el PDF:

```text
balance(node) = height(right) - height(left)
```

Por tanto:

```text
factor positivo -> el lado derecho es más alto
factor negativo -> el lado izquierdo es más alto
```

Un factor mayor que `1` se repara hacia la izquierda y uno menor que `-1`
se repara hacia la derecha.

## Contratos

`avl_insert` devuelve:

```text
-1  no se pudo reservar el nodo
 1  la clave fue insertada
```

Cada repetición se almacena en un nodo independiente. Durante la búsqueda del
punto de inserción, una clave igual avanza inicialmente hacia el hijo izquierdo:

```cpp
link = key <= parent->key ? &parent->left : &parent->right;
```

Las rotaciones conservan el recorrido in-order, pero pueden dejar claves iguales
a ambos lados de un nodo. Por eso la propiedad estable que se valida es:

```text
subárbol izquierdo <= nodo <= subárbol derecho
```

No es posible exigir que todos los iguales permanezcan siempre a la izquierda y
mantener a la vez el balance AVL para cualquier entrada. Por ejemplo, tres claves
iguales necesitan una rotación y una de ellas terminará como hijo derecho.

`avl_erase` devuelve `false` cuando la clave no existe. En caso de éxito elimina
una aparición de la clave y restaura el balance.

`avl_valid` comprueba en un único recorrido:

- La propiedad BST no estricta que permite repetidos.
- Los punteros al padre.
- El balance AVL de todos los nodos.
- En `stored_height`, la corrección de cada altura almacenada.

## Complejidades

| Operación | Altura recursiva | Altura almacenada |
|---|---:|---:|
| `search`, límites, mínimo y máximo | `O(log n)` | `O(log n)` |
| Consulta `height(root)` | `O(n)` | `O(1)` |
| Rotación estructural | `O(1)` | `O(1)` |
| Cálculo del balance | `O(n)` en el peor subárbol | `O(1)` |
| `avl_insert` | `O(n)` | `O(log n)` |
| `avl_erase` | `O(n)` | `O(log n)` |
| `avl_valid` | `O(n)` | `O(n)` |
| Memoria adicional por nodo | Ninguna | Un `int` |

## Compilación

```bash
make
make test
make sanitize
make clean
```

Las pruebas cubren:

- Casos LL, RR, LR y RL.
- Rotaciones de raíz e internas.
- Claves `INT_MIN` e `INT_MAX`.
- Inserción y eliminación de 64 claves iguales.
- Búsqueda, mínimo, máximo, sucesor y límites.
- Inserción ordenada de 512 claves.
- Eliminación y validación después de cada operación.
- Destrucción completa sin fugas.

## Conclusión

Almacenar `height` no es necesario para obtener una forma AVL correcta. Sí es
necesario almacenar la altura o información equivalente, como el factor de
balance, para mantener actualizaciones `O(log n)`. La primera variante muestra
la solución estructural más directa; la segunda representa la implementación
AVL eficiente habitual.
