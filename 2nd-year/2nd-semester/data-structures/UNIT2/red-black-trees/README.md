# Árboles rojinegros: cuatro proyectos independientes

Todos implementan un conjunto de claves `int` sin repetidos, con centinela
negro, inserción, borrado, consultas y validación de invariantes según Cormen.

```text
red-black-trees/
├── c/                C11; descenso y reparación iterativos
├── cpp/              C++11; descenso y reparación iterativos
├── c_recursive/      C11; descenso y propagación recursivos
├── cpp_recursive/    C++11; descenso y propagación recursivos
├── Makefile
└── MAKE_COMMANDS.md
```

Cada subdirectorio contiene su propio encabezado, implementación y programa de
pruebas (`main.c` o `main.cpp`). Se compilan por separado; no se enlazan entre sí.

## Qué cambia en las variantes recursivas

La búsqueda, el descenso de inserción y la búsqueda del sucesor usan llamadas
recursivas. En la inserción, el caso del tío rojo continúa recursivamente desde
el abuelo. En el borrado, el caso de hermano negro con dos hijos negros propaga
el déficit recursivamente hacia el padre. Los otros casos terminan con
recoloraciones y rotaciones locales. Los recorridos, el validador y la liberación
de nodos también son recursivos en las cuatro variantes. Ninguna variante
reconstruye el árbol entero después de una actualización.

La altura sigue limitada por `2 log2(n + 1)`: la profundidad adicional de la
recursión es `O(log n)` en árboles válidos. Las consultas y actualizaciones
siguen siendo `O(log n)`; los recorridos y la validación cuestan `O(n)`.

## API y memoria

En C se usa un tipo `RBTree` opaco y memoria explícita:

```c
RBTree *tree = rb_tree_create();
if (tree == NULL)
  return 1;
int status = rb_tree_insert(tree, 41); /* 1 éxito, 0 repetido, -1 error */
rb_tree_destroy(tree);
```

El encabezado C está destinado a C11; no contiene compatibilidad adicional
para enlazar directamente con C++. El árbol no se copia porque su centinela
guarda punteros a sí mismo.

En C++ el árbol se construye automáticamente y su destructor libera los nodos:

```cpp
red_black::RedBlackTree tree;            // cpp/
// red_black_recursive::RedBlackTree tree; // cpp_recursive/
tree.insert(41);
```

## Compilar y probar

```bash
make             # compila los cuatro
make test        # ejecuta las cuatro suites
make sanitize    # ASan + UBSan para las cuatro
make help        # lista todas las opciones individuales
```

Los objetivos individuales y los ejecutables resultantes se describen en
[`MAKE_COMMANDS.md`](MAKE_COMMANDS.md). Las pruebas cubren la secuencia clásica
`41, 38, 31, 12, 19, 8`, límites `INT_MIN`/`INT_MAX`, operaciones ordenadas,
mezcladas y comparación con un conjunto de referencia.

Las notas de estudio con guion están en
`../notes/arboles-rojinegros.md` (C++) y
`../notes/arboles-rojinegros-c.md` (C).
