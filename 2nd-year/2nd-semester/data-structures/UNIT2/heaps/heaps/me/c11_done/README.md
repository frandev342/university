# Guía de desarrollo del heap en C11

Este directorio contiene una implementación personal y progresiva del max-heap del laboratorio 6. El objetivo es escribir cada parte después de comprender su contrato, comprobarla y continuar solamente cuando las pruebas de la etapa actual funcionen.

## Archivos

```text
me/
├── README.md
├── heap.h
├── heap.c
├── indices.h
└── test_heap.c       # se añadirá al comenzar las pruebas
```

Responsabilidades:

| Archivo | Responsabilidad |
|---|---|
| `heap.h` | Tipos públicos y declaraciones de la interfaz. |
| `indices.h` | Cálculos de padre, hijo izquierdo e hijo derecho. |
| `heap.c` | Gestión de memoria y operaciones del max-heap. |
| `test_heap.c` | Pruebas de cada operación e invariante. |

## 1. Invariantes

Antes de implementar funciones hay que definir qué debe permanecer verdadero.

### Representación

```text
0 <= size <= capacity

capacity > 0 -> data != NULL
```

Los elementos activos ocupan posiciones contiguas:

```text
data[0] ... data[size - 1]
```

Las posiciones desde `size` hasta `capacity - 1` están reservadas, pero no pertenecen al heap actual.

### Propiedad de max-heap

Para cada índice `i > 0`:

```text
data[parent(i)].priority >= data[i].priority
```

El heap compara solamente `priority`. El campo `pid` identifica al proceso, pero no determina su posición. Dos procesos con la misma prioridad pueden extraerse en cualquier orden.

### Operaciones fallidas

Una operación fallida debe conservar intacto el estado observable:

- Una construcción inválida no cambia el heap anterior.
- Una inserción en un heap lleno no cambia `data` ni `size`.
- Una extracción de un heap vacío no cambia el heap ni el objeto indicado por `out`.

## 2. Orden de implementación

```text
1. Definir estructuras y contratos       heap.h
2. Implementar cálculos de índices        indices.h
3. Gestionar memoria                      heap_init / heap_destroy
4. Validar el invariante                  heap_valid
5. Reparar hacia abajo                    sift_down
6. Construir un heap                      heap_build
7. Insertar y ascender                    heap_push
8. Extraer y descender                    heap_pop
9. Añadir casos límite                    test_heap.c
```

Este orden respeta las dependencias:

```text
sift_down <- heap_build
sift_down <- heap_pop
ascenso   <- heap_push
```

## 3. Etapa 1: interfaz pública en `heap.h`

El encabezado debe contener un include guard propio para no colisionar con el código original del profesor:

```c
#ifndef ME_HEAP_H
#define ME_HEAP_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  int pid;
  int priority;
} Process;

typedef struct {
  Process *data;
  size_t size;
  size_t capacity;
} HeapC;

bool heap_init(HeapC *h, size_t capacity);
void heap_destroy(HeapC *h);

bool heap_build(HeapC *h, const Process *src, size_t n);
bool heap_push(HeapC *h, Process value);
bool heap_pop(HeapC *h, Process *out);

bool heap_valid(const HeapC *h);

#endif
```

Esta interfaz no expone `sift_down` ni un posible `sift_up`, porque son detalles internos de `heap.c`.

## 4. Etapa 2: índices en `indices.h`

El archivo debe tener un include guard diferente del utilizado en `heap.h`:

```c
#ifndef ME_HEAP_INDICES_H
#define ME_HEAP_INDICES_H

#include <stddef.h>

/* Precondición: i > 0. */
#define HEAP_PARENT(i) (((i) - 1U) / 2U)
#define HEAP_LEFT(i) (2U * (i) + 1U)
#define HEAP_RIGHT(i) (2U * (i) + 2U)

static inline size_t parent_inline(size_t i) {
  return (i - 1U) / 2U;
}

#endif
```

Relaciones esperadas:

```text
parent(1) = 0
parent(2) = 0
parent(5) = 2

left(0)  = 1
right(0) = 2

left(2)  = 5
right(2) = 6
```

No se debe evaluar `parent(0)`. Como `size_t` es un tipo sin signo, `0U - 1U` produce `SIZE_MAX` por underflow.

Calcular el índice de un hijo tampoco demuestra que exista. Antes de acceder a `data[child]` debe comprobarse que el índice está dentro de `size`.

## 5. Etapa 3: ciclo de vida

Las primeras funciones de `heap.c` serán `heap_init` y `heap_destroy`. Todavía no dependen de los algoritmos del heap.

### `heap_init`

Firma:

```c
bool heap_init(HeapC *h, size_t capacity);
```

Objetivo: crear un heap vacío con capacidad fija.

Pasos:

1. Rechazar `h == NULL`.
2. Colocar temporalmente los campos en un estado vacío y seguro.
3. Comprobar que `capacity * sizeof(Process)` no desborde.
4. Reservar memoria si `capacity != 0U`.
5. Rechazar el fallo de `malloc`.
6. Registrar la capacidad y devolver `true`.

Postcondición en caso de éxito:

```text
size == 0
capacity == capacidad solicitada
data != NULL si capacity > 0
```

Casos mínimos:

```text
heap_init(NULL, 10) -> false
heap_init(&h, 0)    -> true, data == NULL
heap_init(&h, 10)   -> true, size == 0, capacity == 10
```

No debe llamarse por segunda vez sobre un heap activo sin ejecutar antes `heap_destroy`, porque se perdería la dirección de la reserva anterior.

### `heap_destroy`

Firma:

```c
void heap_destroy(HeapC *h);
```

Pasos:

1. Si `h == NULL`, terminar.
2. Liberar `h->data`.
3. Asignar `data = NULL`.
4. Asignar `size = 0U`.
5. Asignar `capacity = 0U`.

Como `free(NULL)` es válido, reiniciar los campos permite destruir nuevamente un heap ya destruido.

Complejidad:

```text
Tiempo: O(1)
```

`Process` no administra recursos propios, así que no hay que recorrer sus elementos antes de liberar el arreglo.

## 6. Etapa 4: `heap_valid`

Firma:

```c
bool heap_valid(const HeapC *h);
```

Objetivo: comprobar la representación y la propiedad max-heap. No debe reparar nada.

Debe rechazar:

```text
h == NULL
size > capacity
capacity > 0 y data == NULL
un hijo con prioridad mayor que su padre
```

Para verificar el orden se recorren los nodos desde `i = 1` y se compara cada uno con su padre.

Complejidad:

```text
Tiempo: O(n)
Espacio auxiliar: O(1)
```

Esta función será el oráculo de las pruebas posteriores:

```c
assert(heap_valid(&heap));
```

## 7. Etapa 5: `sift_down`

Firma privada sugerida:

```c
static void sift_down(HeapC *h, size_t i);
```

Objetivo: restaurar el orden desde `i` hacia abajo.

Precondición:

```text
los subárboles de los hijos de i ya son max-heaps
la única posible violación se encuentra en i
```

Algoritmo:

1. Comprobar primero si existe hijo izquierdo.
2. Calcular los índices de los hijos.
3. Elegir el hijo existente con mayor prioridad.
4. Terminar si el proceso actual ya domina al hijo mayor.
5. Intercambiar ambos procesos.
6. Continuar desde el hijo elegido.

No debe intercambiarse siempre con el hijo izquierdo. Si el derecho es mayor, la propiedad seguiría violada.

Una condición segura para saber que existe hijo izquierdo es:

```c
i < h->size / 2U
```

Esa comprobación se realiza antes de calcular `2U * i + 1U`.

Postcondición:

```text
el subárbol original de i es un max-heap
se conserva exactamente el mismo conjunto de procesos
```

Complejidad:

```text
Tiempo: O(log n)
Espacio auxiliar: O(1)
```

## 8. Etapa 6: `heap_build`

Firma:

```c
bool heap_build(HeapC *h, const Process *src, size_t n);
```

Objetivo: copiar `n` procesos y convertirlos en max-heap.

Validaciones previas:

```text
h != NULL
n <= capacity
src != NULL cuando n > 0
```

`heap_build(&h, NULL, 0U)` es válido y construye un heap vacío.

Pasos:

1. Validar todos los argumentos antes de modificar el heap.
2. Copiar `src[0..n-1]` a `data` cuando `n > 0`.
3. Asignar `size = n`.
4. Procesar los nodos internos desde el último hasta la raíz.
5. Aplicar `sift_down` a cada nodo interno.

Bucle descendente seguro:

```c
for (size_t k = n / 2U; k > 0U; --k) {
  sift_down(h, k - 1U);
}
```

No debe usarse `i >= 0U` como condición de un bucle descendente, porque siempre es verdadera para un entero sin signo.

Complejidad:

```text
Tiempo: Theta(n)
Espacio auxiliar: O(1)
```

## 9. Etapa 7: `heap_push`

Firma:

```c
bool heap_push(HeapC *h, Process value);
```

Objetivo: insertar al final y restaurar el orden hacia la raíz.

Validaciones:

```text
h != NULL
size < capacity
```

Si el heap está lleno, debe devolver `false` sin modificarlo.

Pasos:

1. Colocar el proceso en `data[size]`.
2. Incrementar `size`.
3. Mientras no esté en la raíz, calcular el padre.
4. Terminar si el padre tiene prioridad mayor o igual.
5. Intercambiar con el padre y continuar desde allí.

La comprobación debe respetar este orden lógico:

```c
i > 0U &&
data[HEAP_PARENT(i)].priority < data[i].priority
```

Así no se evalúa `HEAP_PARENT(0U)`.

El ascenso puede escribirse directamente en `heap_push` o aislarse:

```c
static void sift_up(HeapC *h, size_t i);
```

Complejidad:

```text
Tiempo: O(log n)
Espacio auxiliar: O(1)
```

## 10. Etapa 8: `heap_pop`

Firma:

```c
bool heap_pop(HeapC *h, Process *out);
```

Objetivo: retirar y devolver un proceso de prioridad máxima.

Validaciones:

```text
h != NULL
out != NULL
size > 0
```

En caso de error no debe cambiarse el heap ni `*out`.

Pasos:

1. Guardar una copia de la raíz.
2. Reducir `size`.
3. Si quedan elementos, mover el antiguo último elemento a la raíz.
4. Ejecutar `sift_down(h, 0U)`.
5. Escribir el máximo guardado en `*out`.
6. Devolver `true`.

Para un heap de un elemento:

```text
size pasa de 1 a 0
no se llama sift_down
```

Complejidad:

```text
Tiempo: O(log n)
Espacio auxiliar: O(1)
```

## 11. Etapa 9: pruebas

Crear `me/test_heap.c` y avanzar por grupos pequeños.

### Ciclo de vida

```text
Inicialización con capacidad cero.
Inicialización con capacidad positiva.
Destrucción de heap inicializado.
Destrucción con puntero NULL.
```

### Construcción

```text
Arreglo vacío.
Un elemento.
Ejemplo del PDF.
Arreglo ya válido.
Arreglo creciente.
Arreglo decreciente.
```

### Inserción

```text
Inserción en heap vacío.
Inserción sin ascenso.
Inserción que ascienda hasta la raíz.
Inserción con prioridad repetida.
Inserción en heap lleno sin modificarlo.
```

### Extracción

```text
Extracción de heap vacío sin modificar out.
Extracción del único elemento.
Extracciones repetidas en orden no creciente.
Extracción con prioridades repetidas.
```

### Valores límite

```text
INT_MIN
INT_MAX
prioridades negativas
capacidad cero
```

Después de cada operación exitosa:

```c
assert(heap_valid(&heap));
```

## 12. Compilación

Desde la raíz de `heaps/heaps`:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion \
  me/heap.c me/test_heap.c -o build/test_me

./build/test_me
```

Cuando la implementación básica funcione, compilar con sanitizadores:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  me/heap.c me/test_heap.c -o build/test_me_sanitize

./build/test_me_sanitize
```

## 13. Lista de comprobación

- [ ] `heap.h` define `Process`, `HeapC` y la interfaz.
- [ ] `indices.h` usa un include guard propio.
- [ ] Nunca se calcula el padre de la raíz.
- [ ] Se comprueba la existencia de los hijos antes de accederlos.
- [ ] `heap_init` detecta desbordamiento y fallo de `malloc`.
- [ ] `heap_destroy` reinicia todos los campos.
- [ ] `heap_valid` detecta violaciones padre-hijo.
- [ ] `sift_down` elige al hijo de mayor prioridad.
- [ ] `heap_build` recorre los nodos internos de abajo hacia arriba.
- [ ] `heap_push` no modifica un heap lleno.
- [ ] `heap_pop` no modifica `out` si el heap está vacío.
- [ ] Un heap de un elemento se extrae sin llamar a `sift_down`.
- [ ] Las extracciones sucesivas producen prioridades no crecientes.
- [ ] La implementación compila sin advertencias.
- [ ] ASan y UBSan no detectan errores.

## Primer objetivo

Completar y revisar solamente:

```text
me/heap.h
me/indices.h
```

Después se implementarán `heap_init` y `heap_destroy`. No conviene comenzar con `sift_down` hasta tener una representación, una interfaz y un ciclo de vida coherentes.
