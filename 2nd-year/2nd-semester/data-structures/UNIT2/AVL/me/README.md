# AVL Desde Cero

Dos implementaciones equivalentes de un árbol AVL con altura almacenada:

```text
me/
├── c11/    implementación procedural en C11
├── cpp/    clase con encapsulación en C++11
└── Makefile
```

Los archivos anteriores `avl.h`, `avl.cpp` y `main.cpp` se conservan como el
avance original. Las versiones completas viven en sus propios directorios.

## Invariantes

- El recorrido in-order está ordenado de forma no decreciente.
- Se permiten claves repetidas.
- Cada nodo almacena su altura: `nullptr`/`NULL` tiene altura -1 y una hoja 0.
- Para todo nodo, la diferencia entre alturas izquierda y derecha está entre
  `-1` y `1`.
- El factor de balance es `height(right) - height(left)`: positivo significa
  mayor altura a la derecha y negativo mayor altura a la izquierda.
- El contador de elementos coincide con la cantidad real de nodos.

Las rotaciones actualizan únicamente los dos nodos afectados y cuestan `O(1)`.
No se almacenan punteros al padre: los helpers recursivos devuelven la nueva raíz
de cada subárbol.

## API C++

`cpp/avl.hpp` define `AVLTree`. Los nodos son privados y la API pública ofrece:

```cpp
bool insert(int value);
bool erase(int value);
bool contains(int value) const;
std::size_t size() const;
int height() const;
bool empty() const;
bool minimum(int& value) const;
bool maximum(int& value) const;
void clear();
bool valid() const;
std::vector<int> inorder() const;
```

`insert` devuelve `false` únicamente si falla la reserva de memoria. `erase`
elimina una aparición y devuelve `false` si la clave no existe.

## API C11

`c11/avl.h` mantiene `AVLNode` como tipo incompleto. El usuario crea el objeto
`AVL` y controla su ciclo de vida:

```c
AVL tree;
avl_init(&tree);
avl_insert(&tree, 10);
avl_destroy(&tree);
```

`avl_inorder` devuelve la cantidad total de elementos aunque el buffer sea más
pequeño. Esto permite consultar primero el tamaño necesario pasando `NULL, 0`.

## Complejidades

| Operación | Tiempo | Memoria auxiliar |
|---|---:|---:|
| Altura, tamaño, vacío | `O(1)` | `O(1)` |
| Rotación simple o doble | `O(1)` | `O(1)` |
| Buscar, mínimo, máximo | `O(log n)` | `O(1)` |
| Insertar | `O(log n)` | `O(log n)` |
| Eliminar | `O(log n)` | `O(log n)` |
| Recorrido, validación, limpiar | `O(n)` | `O(log n)` |

La memoria auxiliar `O(log n)` corresponde a la pila recursiva, ya que un AVL
mantiene altura `O(log n)`.

## Compilación

Desde `AVL/me/`:

```bash
make
make test
make sanitize
make clean
```

Las pruebas cubren los casos LL, RR, LR y RL; claves repetidas; borrado de hoja,
nodo con un hijo, nodo con dos hijos y raíz; extremos; inserciones ordenadas;
recorrido in-order; reutilización después de `clear`; y validación tras cada
mutación.

La comparación detallada entre una implementación iterativa con punteros
`parent` y estas implementaciones recursivas sin `parent` está en
[`PARENT_VS_RECURSION.md`](PARENT_VS_RECURSION.md).
