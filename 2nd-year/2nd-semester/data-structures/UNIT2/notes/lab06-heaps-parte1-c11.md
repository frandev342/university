---
title: "Lab 6 de heaps - Parte I: C11"
aliases:
  - "Heaps en C11"
  - "Lab 6 Parte I"
tags:
  - estructuras-de-datos
  - heaps
  - cola-de-prioridad
  - c11
  - memoria-dinamica
  - macros
status: desarrollado
---

# Lab 6 de heaps - Parte I: C11

> [!abstract] Objetivo
> Implementar una cola de prioridad máxima de procesos mediante un heap binario con índices desde cero, capacidad fija y memoria administrada manualmente.

## Enlaces relacionados

- [[heaps-binarios-y-colas-de-prioridad]]: teoría general y algoritmos.
- [[lab06-heaps-c11-fundamentos]]: literales sin signo, macros, `static` e `inline`.
- `docs/ED_Lab06.pdf`: guía original del laboratorio.
- `heaps/heaps/c11/`: código base del profesor.
- `heaps/heaps/me/`: implementación desarrollada paso a paso.

## 1. Representación

Cada elemento de la cola es un proceso:

```c
typedef struct {
  int pid;
  int priority;
} Process;
```

El `pid` identifica al proceso. La propiedad del heap utiliza exclusivamente `priority`.

```c
typedef struct {
  Process *data;
  size_t size;
  size_t capacity;
} HeapC;
```

| Campo | Significado |
|---|---|
| `data` | Reserva dinámica que almacena los procesos. |
| `size` | Cantidad de procesos activos. |
| `capacity` | Cantidad máxima de procesos que caben. |

Los elementos activos ocupan:

```text
data[0] ... data[size - 1]
```

Las posiciones desde `size` hasta `capacity - 1` están reservadas, pero no forman parte del heap.

> [!warning] Memoria reservada no significa memoria inicializada
> Después de `heap_init`, `data` puede apuntar a una reserva válida, pero no debe leerse ninguna posición mientras `size == 0`.

## 2. Invariantes

### Invariante de representación

```text
0 <= size <= capacity

capacity > 0 -> data != NULL
```

### Invariante de orden

Para todo índice `i` con `1 <= i < size`:

```text
data[parent(i)].priority >= data[i].priority
```

El máximo siempre se encuentra en:

```c
data[0]
```

> [!important] Los hermanos no están ordenados
> El hijo izquierdo puede tener prioridad mayor, igual o menor que el derecho. El max-heap solo compara cada hijo con su padre.

Ejemplo válido:

```text
          100
         /   \
       20     90
      / \    / \
     5  15  80 85
```

Aunque `20 < 90` y `80 < 85`, cada padre domina a sus hijos.

## 3. Índices Desde Cero

```c
#define HEAP_PARENT(i) (((i) - 1U) / 2U)
#define HEAP_LEFT(i) (2U * (i) + 1U)
#define HEAP_RIGHT(i) (2U * (i) + 2U)
```

| Relación | Fórmula | Precondición |
|---|---|---|
| Padre | `(i - 1) / 2` | `i > 0` |
| Hijo izquierdo | `2 * i + 1` | Comprobar que existe antes de acceder. |
| Hijo derecho | `2 * i + 2` | Comprobar que existe antes de acceder. |

Ejemplo:

```text
             índice 0
            /        \
      índice 1      índice 2
      /     \       /     \
     3       4     5       6
```

> [!danger] No calcular `HEAP_PARENT(0)`
> `size_t` es un tipo sin signo. `0U - 1U` produce `SIZE_MAX`, no `-1`.

## 4. Literales Sin Signo Y `SIZE_MAX`

El sufijo `U` indica un literal entero sin signo:

```c
0U
1U
2U
```

Se utiliza porque tamaños e índices se representan con `size_t`.

`SIZE_MAX` es el máximo valor representable por `size_t`. Cumple un papel análogo a `LLONG_MAX`, pero corresponde a otro tipo:

```text
SIZE_MAX  -> máximo de size_t
LLONG_MAX -> máximo de long long
```

Antes de reservar memoria se comprueba:

```c
if (capacity > SIZE_MAX / sizeof(Process))
  return false;
```

La condición garantiza que esta multiplicación sea representable:

```c
capacity * sizeof(Process)
```

No garantiza que el sistema tenga memoria suficiente. Ese segundo problema lo detecta `malloc` cuando devuelve `NULL`.

Una forma mantenible de expresar el tamaño es:

```c
sizeof *h->data
```

Si cambia el tipo de `data`, el cálculo sigue siendo correcto.

## 5. Objeto Frente A Puntero

```c
HeapC heap;
```

Crea un objeto real. El objeto no puede ser `NULL`, aunque sus miembros locales tienen valores indeterminados hasta ser inicializados.

```c
HeapC *heap = NULL;
```

Crea un puntero que no apunta a un objeto.

Uso correcto:

```c
HeapC heap;

if (!heap_init(&heap, capacity)) {
  /* manejar error */
}
```

`&heap` es la dirección válida del objeto. `heap_init` sobrescribe todos sus miembros antes de leerlos.

También puede inicializarse defensivamente:

```c
HeapC heap = {0};
```

Esto deja inicialmente:

```text
data = NULL
size = 0
capacity = 0
```

## 6. Ciclo De Vida

```text
heap_init -> usar el heap -> heap_destroy
```

### `heap_init`

```c
bool heap_init(HeapC *h, size_t capacity) {
  if (h == NULL)
    return false;

  h->data = NULL;
  h->size = 0U;
  h->capacity = 0U;

  if (capacity > SIZE_MAX / sizeof(Process))
    return false;

  if (capacity != 0U) {
    h->data = malloc(capacity * sizeof(Process));
    if (h->data == NULL)
      return false;
  }

  h->capacity = capacity;
  return true;
}
```

Responsabilidades:

1. Validar el puntero recibido.
2. Establecer un estado vacío seguro.
3. Evitar overflow en el cálculo de bytes.
4. Reservar memoria para una capacidad fija.
5. Comunicar errores mediante `false`.

> [!warning] No reinicializar un heap activo
> Llamar otra vez a `heap_init` sin ejecutar antes `heap_destroy` pierde la dirección de la reserva anterior y produce una fuga.

### `heap_destroy`

```c
void heap_destroy(HeapC *h) {
  if (h == NULL)
    return;

  free(h->data);
  h->data = NULL;
  h->size = 0U;
  h->capacity = 0U;
}
```

`free(NULL)` es válido. Reiniciar todos los campos evita conservar un puntero colgante.

Complejidad:

```text
Tiempo: O(1)
```

## 7. Validación: `heap_valid`

```c
bool heap_valid(const HeapC *h) {
  if (h == NULL || h->size > h->capacity ||
      (h->capacity != 0U && h->data == NULL))
    return false;

  for (size_t i = 1U; i < h->size; ++i) {
    if (h->data[HEAP_PARENT(i)].priority < h->data[i].priority)
      return false;
  }

  return true;
}
```

`heap_valid` es un oráculo de pruebas. Comprueba la representación y cada relación padre-hijo, pero no repara el heap.

```text
Tiempo: O(n)
Espacio auxiliar: O(1)
```

## 8. Reparación Hacia Abajo: `sift_down`

```c
static void sift_down(HeapC *h, size_t i);
```

### Contrato

Precondición:

```text
i es válido
los subárboles de sus hijos ya son max-heaps
la única posible violación está en i
```

Postcondición:

```text
el subárbol original de i es un max-heap
se conservan exactamente los mismos procesos
```

### Algoritmo

1. Comprobar primero si existe hijo izquierdo.
2. Elegir al hijo existente con mayor prioridad.
3. Terminar si el nodo actual ya domina al hijo mayor.
4. Intercambiar procesos completos.
5. Continuar desde el hijo elegido.

```c
static void sift_down(HeapC *h, size_t i) {
  while (i < h->size / 2U) {
    size_t left = HEAP_LEFT(i);
    size_t right = HEAP_RIGHT(i);
    size_t best = left;

    if (right < h->size &&
        h->data[right].priority > h->data[left].priority) {
      best = right;
    }

    if (h->data[i].priority >= h->data[best].priority)
      return;

    Process temp = h->data[best];
    h->data[best] = h->data[i];
    h->data[i] = temp;
    i = best;
  }
}
```

La condición:

```c
i < h->size / 2U
```

garantiza que existe un hijo izquierdo antes de calcularlo.

> [!failure] Error corregido durante el desarrollo
> Comparar `right == 2U * i + 2U` no comprueba que el hijo exista: es verdadero por definición. La comprobación correcta es `right < h->size`.

Complejidad:

```text
Mejor caso: O(1)
Peor caso: O(log n)
Espacio auxiliar: O(1)
```

## 9. Construcción Bottom-Up: `heap_build`

`heap_build` copia un arreglo de procesos sobre una reserva ya creada por `heap_init` y lo reorganiza como max-heap.

```c
bool heap_build(HeapC *h, const Process *src, size_t n) {
  if (h == NULL || n > h->capacity)
    return false;

  if (n != 0U && src == NULL)
    return false;

  for (size_t i = 0U; i < n; ++i)
    h->data[i] = src[i];

  h->size = n;

  for (size_t k = n / 2U; k > 0U; --k)
    sift_down(h, k - 1U);

  return true;
}
```

Las hojas ocupan:

```text
n / 2 ... n - 1
```

Las hojas ya son heaps de un elemento. Por eso se procesan los nodos internos desde el último hasta la raíz.

El bucle descendente debe ser seguro para `size_t`:

```c
for (size_t k = n / 2U; k > 0U; --k)
```

No debe utilizarse:

```c
for (size_t i = n / 2U - 1U; i >= 0U; --i)
```

La condición `i >= 0U` siempre es verdadera y el índice termina envolviéndose hasta `SIZE_MAX`.

### Ejemplo

Entrada:

```text
[4, 1, 3, 2, 16, 9, 10, 14, 8, 7]
```

Índices procesados:

```text
4, 3, 2, 1, 0
```

Resultado:

```text
[16, 14, 10, 8, 7, 9, 3, 2, 4, 1]
```

Complejidad:

```text
Tiempo: Theta(n)
Espacio auxiliar: O(1)
```

> [!failure] Error corregido durante el desarrollo
> `heap_build` no debe llamar nuevamente a `heap_init`. Hacerlo pierde la reserva anterior, cambia la capacidad y puede producir una fuga. `heap_build` utiliza la memoria ya reservada.

## 10. Inserción: `heap_push`

Al insertar, el nuevo proceso se coloca en la siguiente posición libre. La forma sigue siendo casi completa, pero puede violarse la relación con el padre.

```c
bool heap_push(HeapC *h, Process x) {
  if (h == NULL || h->size == h->capacity)
    return false;

  size_t i = h->size;
  h->data[i] = x;
  h->size++;

  while (i > 0U) {
    size_t parent = HEAP_PARENT(i);

    if (h->data[parent].priority >= h->data[i].priority)
      break;

    h->data[i] = h->data[parent];
    h->data[parent] = x;
    i = parent;
  }

  return true;
}
```

La comprobación `i > 0U` debe ocurrir antes de calcular el padre.

La implementación desplaza los padres hacia abajo mientras `x` asciende. El invariante de ciclo es:

```text
al comenzar cada iteración, x se encuentra en data[i]
```

> [!failure] Errores corregidos durante el desarrollo
> La posición libre es `data[size]`, no `data[size + 1]`. La inserción debe reparar hacia arriba, no llamar repetidamente a `sift_down`.

Complejidad:

```text
Mejor caso: O(1)
Peor caso: O(log n)
Espacio auxiliar: O(1)
```

## 11. Extracción: `heap_pop`

La raíz contiene un proceso de máxima prioridad. Para retirarla sin romper la forma se mueve el último proceso a la raíz y se repara hacia abajo.

```c
bool heap_pop(HeapC *h, Process *out) {
  if (h == NULL || out == NULL || h->size == 0U)
    return false;

  *out = h->data[0];
  h->data[0] = h->data[h->size - 1U];
  h->size--;

  if (h->size != 0U)
    sift_down(h, 0U);

  return true;
}
```

Si el heap tiene un elemento, la asignación de raíz a raíz es redundante pero válida. Después `size` pasa a cero y no se llama a `sift_down`.

El antiguo último valor puede permanecer físicamente en la reserva. Ya no forma parte del heap porque se encuentra fuera de `data[0..size-1]`.

Complejidad:

```text
Tiempo: O(log n)
Espacio auxiliar: O(1)
```

## 12. Contratos De Error

| Operación | Motivos de fallo | Estado tras el fallo |
|---|---|---|
| `heap_init` | `h == NULL`, overflow o `malloc == NULL` | Objeto no inicializado o estado vacío seguro, según el caso. |
| `heap_build` | Heap nulo, exceso de capacidad o fuente nula con `n > 0` | Heap anterior intacto. |
| `heap_push` | Heap nulo o lleno | Heap intacto. |
| `heap_pop` | Heap nulo, salida nula o heap vacío | Heap y `out` intactos. |

La regla utilizada es:

```text
validar primero, modificar después
```

## 13. Complejidades

| Operación | Tiempo | Espacio auxiliar |
|---|---:|---:|
| `heap_init` | `O(1)` | Reserva `O(capacity)` |
| `heap_destroy` | `O(1)` | `O(1)` |
| `heap_valid` | `O(n)` | `O(1)` |
| `sift_down` | `O(log n)` | `O(1)` |
| `heap_build` | `Theta(n)` | `O(1)` |
| `heap_push` | `O(log n)` | `O(1)` |
| `heap_pop` | `O(log n)` | `O(1)` |

## 14. Experimento: Macro Frente A `static inline`

Código estudiado:

```c
#define MAX_MACRO(a, b) ((a) > (b) ? (a) : (b))

static inline int max_inline(int a, int b) {
  return a > b ? a : b;
}
```

Con:

```c
int a = 9;
const int b = 3;
```

La llamada:

```c
MAX_MACRO(a++, b)
```

se expande a:

```c
((a++) > b ? (a++) : b)
```

Resultado:

```text
macro:  valor = 10, a = 11
inline: valor = 9,  a = 10
```

La macro evalúa `a++` en la condición y nuevamente en la rama verdadera. La función evalúa el argumento una sola vez y recibe una copia del valor anterior.

No hay comportamiento indefinido en este experimento porque el primer operando de `?:` se evalúa antes de la rama seleccionada y solamente una rama se ejecuta.

### `static`

En una función definida a nivel de archivo:

```c
static void sift_down(...);
```

concede enlace interno: la función solo puede nombrarse desde ese archivo `.c`.

Una función sin `static` tiene normalmente enlace externo y puede utilizarse desde otros archivos mediante una declaración.

### `inline`

Indica que la función es adecuada para expansión en el lugar de llamada, pero no obliga al compilador a hacerlo ni garantiza mejor rendimiento.

### `static inline`

```text
static -> función privada para la unidad de traducción
inline -> permite ofrecer el cuerpo para una posible expansión
```

Sigue siendo una función real:

- Tiene parámetros y resultado tipados.
- Evalúa cada argumento una vez.
- Produce mejores diagnósticos.
- Puede tener dirección de función.

Una macro solo realiza sustitución textual. Con optimización, ambas alternativas pueden producir las mismas instrucciones; esto debe observarse mediante ensamblador, no asumirse.

## 15. Pruebas Realizadas

### Construcción

- Arreglo desordenado del ejemplo de CLRS.
- Heap resultante válido.
- Conservación de PID y prioridad durante los intercambios.

### Inserción

- Heap vacío.
- Capacidad de un elemento.
- Ascenso parcial.
- Ascenso hasta la raíz.
- Prioridades repetidas.
- Inserción rechazada por capacidad llena.
- Estado intacto después del fallo.

### Extracción

- Heap vacío y `out` intacto.
- Heap de un elemento.
- Extracción repetida hasta vaciar.
- Prioridades duplicadas.
- `INT_MIN` e `INT_MAX`.
- Validación después de cada extracción.

### Herramientas

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  main.c heap.c -o heap
```

ASan ayuda a detectar accesos fuera de límites, uso después de liberar y fugas. UBSan ayuda a detectar varias formas de comportamiento indefinido. No deben confundirse con una herramienta que detecte todos los valores sin inicializar.

## 16. Uso Completo

```c
#include "heap.h"

#include <stdlib.h>

int main(void) {
  const Process input[] = {
      {0, 4}, {1, 1},  {2, 3},  {3, 2}, {4, 16},
      {5, 9}, {6, 10}, {7, 14}, {8, 8}, {9, 7},
  };

  const size_t n = sizeof input / sizeof input[0];
  HeapC heap;

  if (!heap_init(&heap, n * 2U))
    return EXIT_FAILURE;

  if (!heap_build(&heap, input, n)) {
    heap_destroy(&heap);
    return EXIT_FAILURE;
  }

  if (!heap_push(&heap, (Process){10, 20})) {
    heap_destroy(&heap);
    return EXIT_FAILURE;
  }

  Process maximum;
  while (heap_pop(&heap, &maximum)) {
    /* Las prioridades salen en orden no creciente. */
  }

  heap_destroy(&heap);
  return EXIT_SUCCESS;
}
```

## 17. Ideas Centrales Aprendidas

1. Un heap combina una forma casi completa con un orden padre-hijos.
2. Un arreglo representa la forma sin almacenar punteros entre nodos.
3. `size` describe los elementos activos y `capacity` la reserva disponible.
4. Reservar memoria no inicializa procesos ni aumenta `size`.
5. `sift_down` requiere que los subárboles hijos ya sean heaps.
6. La construcción bottom-up respeta esa precondición y cuesta `Theta(n)`.
7. Una inserción rompe potencialmente el orden hacia el padre y debe ascender.
8. Una extracción reemplaza la raíz y rompe potencialmente el orden hacia los hijos.
9. `size_t` exige cuidado con underflow, overflow y bucles descendentes.
10. Validar antes de modificar conserva el estado cuando una operación falla.
11. En C, la propiedad de memoria debe documentarse y terminar con `free`.
12. Una macro no es una función y puede evaluar argumentos varias veces.
13. `static inline` conserva tipos y evaluación única sin exigir una llamada en el código generado.
14. Las pruebas deben verificar propiedades, no solamente una disposición concreta.

## 18. Preparación Para C++11

La Parte II conserva el mismo algoritmo, pero cambia decisiones del lenguaje:

```text
C11                          C++11
malloc/free                  std::vector y RAII
HeapC + funciones            class Heap
macros                       inline constexpr
bool + parámetro de salida   métodos con referencias
gestión manual               destrucción automática
```

Preguntas para la transición:

- ¿Qué responsabilidad deja de tener el usuario cuando `std::vector` administra la memoria?
- ¿Qué ocurre si `vector::reserve` no puede reservar?
- ¿Qué aporta una función `constexpr` frente a una macro?
- ¿Por qué `constexpr` no obliga a evaluar siempre durante la compilación?
- ¿Cómo se traduce `sift_down` sin cambiar su complejidad?

> [!success] Estado de la Parte I
> Se desarrollaron y revisaron `heap_init`, `heap_destroy`, `heap_valid`, `sift_down`, `heap_build`, `heap_push` y `heap_pop`. La siguiente etapa es comparar esta implementación con la versión C++11 basada en RAII y `std::vector`.
