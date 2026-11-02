# Árbol binario de búsqueda → lista doblemente circular

Archivos base de la práctica de Estructuras de Datos (90 minutos).
Inspirado en *The Great Tree-List Recursion Problem* de Nick Parlante,
Stanford CS Education Library #109 (2000): http://cslibrary.stanford.edu/109/

## Objetivo

Convertir in situ un ABB en una lista doblemente enlazada circular,
reutilizando **exactamente los nodos existentes**, en **O(n)** tiempo.
En el árbol `small`/`large` son hijo izquierdo/derecho, y en la lista
son anterior/siguiente, respectivamente.

## Archivos

- `treelist.h`: interfaz y estructura del nodo.
- `treelist.c`: utilidades dadas y cuatro funciones por completar.
- `tests.c`: pruebas automáticas por etapa. No modificar.
- `Makefile`: compilación con GCC C11.

## Trabajo

Resuelva `TODO 1` a `TODO 4` en `treelist.c`, de manera gradual:

    make test-insert
    make test-join
    make test-append
    make test-tree
    make test

Para analizar fallos de memoria (si GCC lo admite):

    make sanitize
    ./tests_san tree

`treeToList` **no reserva ni libera nodos**. Al convertir el árbol, su
representación original se destruye: libere el resultado con `freeList`,
no con `freeTree`. Use `freeTree` solo si todavía es un árbol.

## Entrega

Adjunte `treelist.c`, el resultado de `make test` y el dibujo solicitado
en la guía. No se requiere modificar `treelist.h` ni `tests.c`.
