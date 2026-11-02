# Laboratorio: implementacion de heaps en C11, C++11 y C++23

Universidad Nacional de San Agustin de Arequipa - Estructuras de Datos.

## Requisitos

GCC/G++ con soporte de C++23 y biblioteca estandar con `<expected>` (probado con GCC 14.2).
Ubuntu: `sudo apt install gcc g++ make` (si la version instalada es antigua, se necesitara una mas reciente).

```sh
make            # compilar las tres versiones
make demos      # macro repetida, C++23
make test       # al inicio C11 y C++11 FALLAN (TODO)
make asm        # compara ejemplos de ensamblador a -O2
make sanitize   # ejecuta pruebas compiladas con ASan/UBSan
make clean
```

## En que trabajar

- `c11/heap.c`: TODO C1-C4 (sift_down, build, push, pop).
- `c11/indices.h`: examinar macros y alternativa `static inline`.
- `cpp11/indices.hpp`: `inline constexpr` (redundancia de `inline` explicada en guia).
- `cpp11/heap.cpp`: TODO CPP1 (portar `sift_down`; opcional: variante con un temporal).
- `cpp23/heap23.hpp`: heap de capacidad fija que es valido en evaluacion constante;
  `std::expected<Process,Error>` para la extraccion.
- `cpp23/demo.cpp`: pruebas `static_assert`, uso de `std::ranges` (C++20) y
  `if consteval` (C++23).

## Contratos

Max-heap basado solo en `priority`; los empates pueden salir en cualquier orden.
Capacidad fija. `push` falla si esta lleno; `pop` si esta vacio. La memoria del
heap C pertenece a `HeapC` y se libera con `heap_destroy`. C++11 encapsula el
arreglo en `std::vector`; C++23 usa `std::array` para permitir pruebas constexpr.

La precondicion de parent(i) es `i > 0`. No se deben calcular hijos sin antes
verificar que existen y que no hay desbordamiento. No se usa heapsort.

Las pruebas iniciales NO bastan como demostracion formal de correccion:
proponer minimo tres pruebas adicionales, justificar y aportar resultados.

## Entrega

Codigo, resultados de `make test`, tres pruebas propias y una pagina de analisis
comparando macros, `static inline`, `constexpr`, compilacion/ejecucion, memoria
administrada y posibles sesgos de comparacion de rendimiento. Indicar el uso de IA.
