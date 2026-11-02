# Árbol rojinegro C++11 (recursivo)

Misma interfaz que `../cpp/`, en el namespace `red_black_recursive`.
`search`, `minimum` e `insert_at` son recursivos. El caso 1 de inserción
recurre al abuelo y el caso 2 de borrado al padre. La clase libera sus nodos
al salir de ámbito; los enlaces al padre permiten las rotaciones locales.

Desde el directorio padre: `make test-cpp-recursive` o
`make sanitize-cpp-recursive`. La recursión adicional usa `O(log n)` de pila
en árboles válidos.
