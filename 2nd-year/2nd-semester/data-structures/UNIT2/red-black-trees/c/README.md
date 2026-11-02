# Árbol rojinegro C11 (iterativo)

`red_black_tree.h` declara el tipo opaco `RBTree` y la API; `red_black_tree.c`
implementa búsqueda, inserción y reparación iterativas con un centinela negro;
`main.c` contiene ejemplos y pruebas. La función privada `newNode` reserva e
inicializa el nodo rojo; `rb_tree_insert` encuentra dónde enlazarlo y repara el
árbol.

Desde el directorio padre: `make test-c` o `make sanitize-c`.
Las pruebas incluyen todas las 120 órdenes de inserción y las 120 órdenes
de eliminación de cinco claves, con validación tras cada borrado.

Crear con `rb_tree_create()` (comprobar `NULL`), insertar con
`rb_tree_insert()` (`1` insertado, `0` repetido, `-1` error) y terminar con
`rb_tree_destroy()`.
