# Árbol rojinegro C11 (recursivo)

Misma interfaz que `../c/`: `red_black_tree.h`, `red_black_tree.c` y `main.c`.
`search`, `minimum` y `insert_at` descienden recursivamente. `insert_fixup`
repite el caso de tío rojo sobre el abuelo; `erase_fixup` repite el caso de
hermano negro con dos hijos negros sobre el padre. La reparación continúa
manteniendo el centinela y los invariantes de Cormen.

Desde el directorio padre: `make test-c-recursive` o
`make sanitize-c-recursive`. La recursión adicional usa `O(log n)` de pila
en árboles válidos.
