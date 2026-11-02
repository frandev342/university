---
title: "Lab 6 de heaps - Guía de desarrollo de la Parte II en C++11"
aliases:
  - "Heap C++11"
  - "Lab 6 Parte II"
tags:
  - estructuras-de-datos
  - heaps
  - cpp11
  - vector
  - raii
  - constexpr
status: en-desarrollo
---

# Guía de desarrollo del heap en C++11

> [!abstract] Objetivo
> Portar a C++11 el max-heap de procesos desarrollado en C11 y estudiar qué cambia al usar clases, referencias, `std::vector`, RAII y funciones `constexpr`.

## Archivos

```text
me/
├── README.md
├── heap.hpp
├── heap.cpp
├── indices.hpp
├── main.cpp             # se añadirá para pruebas propias
└── c11_done/            # implementación terminada de la Parte I
```

| Archivo | Responsabilidad |
|---|---|
| `heap.hpp` | Tipos públicos y definición de la clase `Heap`. |
| `indices.hpp` | Funciones `constexpr` para calcular índices. |
| `heap.cpp` | Implementación de los métodos de `Heap`. |
| `main.cpp` | Pruebas progresivas de la implementación personal. |

## 1. Qué Se Conserva Desde C11

El algoritmo y sus invariantes no cambian.

### Forma

Los procesos ocupan posiciones contiguas desde cero:

```text
data_[0] ... data_[size - 1]
```

`std::vector` mantiene automáticamente esta forma casi completa cuando se utiliza `push_back` y `pop_back`.

### Orden

Para cada índice `i > 0`:

```text
data_[parent(i)].priority >= data_[i].priority
```

El heap compara únicamente `priority`. Los empates pueden resolverse en cualquier orden.

### Direcciones de reparación

| Operación | Posible violación | Reparación |
|---|---|---|
| Construir bottom-up | En la raíz de cada subárbol | `sift_down` |
| Insertar al final | Con el padre | Ascenso |
| Reemplazar la raíz al extraer | Con los hijos | `sift_down` |

## 2. Qué Cambia Respecto De C11

| C11 | C++11 |
|---|---|
| `HeapC` público | `class Heap` encapsulada |
| `Process *data` | `std::vector<Process> data_` |
| `size` explícito | `data_.size()` |
| `malloc` | `vector::reserve` |
| `free` y `heap_destroy` | Destructor automático de `vector` |
| Funciones que reciben `HeapC *` | Métodos que operan sobre `this` |
| `src` y `n` separados | `const std::vector<Process>& input` |
| Parámetro de salida `Process *out` | Referencia `Process& out` |
| Macros de índices | Funciones `constexpr` |

> [!important] El algoritmo no se vuelve diferente por estar en C++
> C++11 cambia la representación, la interfaz y la gestión de recursos. Las propiedades matemáticas y las complejidades del heap siguen siendo las mismas.

## 3. Primera Revisión De Los Archivos Personales

### `heap.hpp`

El include guard actual está incompleto:

```cpp
#ifndef HEAP

#endif
```

Debe utilizar un nombre específico y definirlo:

```cpp
#ifndef ME_HEAP_CPP11_HPP
#define ME_HEAP_CPP11_HPP

// Contenido del encabezado.

#endif
```

Un nombre genérico como `HEAP` podría colisionar con otro encabezado.

### `indices.hpp`

Las funciones y los `static_assert` actuales representan correctamente los índices desde cero.

Este include no es necesario:

```cpp
#include <assert.h>
```

`static_assert` es una palabra clave de C++11 y no necesita `<assert.h>`. El archivo solo requiere:

```cpp
#include <cstddef>
```

El comentario final del include guard debe coincidir con el guard utilizado:

```cpp
#endif // ME_HEAP_INDICES_HPP
```

## 4. Espacio De Nombres

La implementación se coloca en:

```cpp
namespace heap11 {
  // Process, Heap y funciones de índices.
}
```

Esto evita conflictos con otras implementaciones del mismo proyecto:

```text
heap11::Heap
heap23::Heap
```

Desde fuera se utiliza:

```cpp
heap11::Heap heap(10U);
heap11::Process process{42, 100};
```

No es necesario escribir `using namespace heap11;`. Mantener el prefijo hace explícito qué implementación se está usando.

## 5. Funciones De Índices `constexpr`

```cpp
namespace heap11 {

inline constexpr std::size_t parent(std::size_t i) {
  return (i - 1U) / 2U;
}

inline constexpr std::size_t left(std::size_t i) {
  return 2U * i + 1U;
}

inline constexpr std::size_t right(std::size_t i) {
  return 2U * i + 2U;
}

}
```

En C++11 una función `constexpr` no es una macro:

- Tiene parámetros y resultado tipados.
- Evalúa cada argumento una sola vez.
- Puede utilizarse durante la compilación cuando el contexto lo exige.
- También puede ejecutarse normalmente durante la ejecución.

En C++11, una función `constexpr` es implícitamente `inline`. Escribir:

```cpp
inline constexpr
```

es redundante, aunque válido y útil para la comparación didáctica del laboratorio.

### Evaluación En Compilación

```cpp
static_assert(parent(9U) == 4U, "incorrect parent");
static_assert(left(4U) == 9U, "incorrect left child");
static_assert(right(4U) == 10U, "incorrect right child");
```

`static_assert` obliga al compilador a verificar esas expresiones durante la compilación.

Esto no implica que todas las llamadas se evalúen en compilación:

```cpp
std::size_t i;
std::cin >> i;
std::size_t p = parent(i); // se evalúa durante la ejecución
```

> [!danger] La precondición no desaparece
> `parent(0U)` continúa produciendo underflow. `constexpr` mejora la expresión y permite evaluación constante, pero no valida automáticamente `i > 0`.

## 6. Representación Pública: `Process`

```cpp
namespace heap11 {

struct Process {
  int pid;
  int priority;
};

}
```

Puede construirse mediante inicialización uniforme:

```cpp
heap11::Process process{42, 100};
```

Como en C11, siempre se mueve o copia el objeto completo. No deben separarse el PID y su prioridad.

## 7. Diseño De La Clase `Heap`

Interfaz propuesta por el laboratorio:

```cpp
namespace heap11 {

class Heap {
public:
  explicit Heap(std::size_t capacity);

  bool build(const std::vector<Process>& input);
  bool push(Process process);
  bool pop(Process& out);

  bool valid() const;
  std::size_t size() const;
  bool empty() const;

private:
  void sift_down(std::size_t i);

  std::vector<Process> data_;
  std::size_t capacity_;
};

}
```

### Sección pública

Los usuarios pueden:

```text
construir
build
push
pop
valid
size
empty
```

### Sección privada

Los usuarios no pueden acceder directamente a:

```text
sift_down
capacity_
```

Esto protege el invariante: el arreglo no puede modificarse arbitrariamente desde fuera de la clase.

## 8. `std::vector`: Tamaño, Capacidad Y Límite Lógico

La clase mantiene:

```cpp
std::vector<Process> data_;
std::size_t capacity_;
```

Hay tres conceptos diferentes:

| Expresión | Significado |
|---|---|
| `data_.size()` | Cantidad actual de procesos activos. |
| `data_.capacity()` | Memoria reservada internamente por el vector. |
| `capacity_` | Límite lógico establecido por el laboratorio. |

El constructor ejecuta:

```cpp
data_.reserve(capacity_);
```

`reserve` aumenta la capacidad interna, pero no el tamaño:

```text
data_.size()     == 0
data_.capacity() >= capacity_
```

Por eso no puede accederse inmediatamente a:

```cpp
data_[0]
```

Primero debe añadirse un elemento mediante `push_back` o asignarse un vector durante `build`.

> [!warning] `reserve` no crea elementos
> Es el equivalente conceptual a reservar almacenamiento, pero `vector` mantiene separada la memoria disponible de los objetos construidos.

## 9. Constructor Y `explicit`

```cpp
Heap::Heap(std::size_t capacity) : capacity_(capacity) {
  data_.reserve(capacity_);
}
```

La lista de inicialización construye `capacity_` directamente con el valor recibido.

El especificador:

```cpp
explicit Heap(std::size_t capacity);
```

evita conversiones implícitas como:

```cpp
Heap heap = 10U; // rechazado por explicit
```

La construcción intencional sigue permitida:

```cpp
Heap heap(10U);
```

### Error De Reserva

`vector::reserve` no devuelve `false`. Si no puede reservar, lanza:

```cpp
std::bad_alloc
```

El objeto no termina de construirse. Los miembros que ya se hubieran construido se destruyen automáticamente.

No se necesita una función equivalente a:

```c
heap_destroy
```

Cuando el objeto `Heap` sale de alcance, el destructor de `vector` libera automáticamente sus recursos.

## 10. RAII

RAII significa **Resource Acquisition Is Initialization**: la propiedad de un recurso se vincula a la vida de un objeto.

C11:

```c
HeapC heap;
heap_init(&heap, 10U);
/* usar */
heap_destroy(&heap);
```

C++11:

```cpp
{
  heap11::Heap heap(10U);
  // usar
} // vector libera su memoria automáticamente
```

Si una función termina anticipadamente o se lanza una excepción, los destructores de los objetos locales siguen ejecutándose.

> [!success] Responsabilidad eliminada
> El usuario ya no tiene que recordar una llamada manual a `free` o `heap_destroy`.

## 11. Métodos `const`

Un método que no modifica el objeto debe declararse `const`:

```cpp
bool valid() const;
std::size_t size() const;
bool empty() const;
```

Esto permite llamarlo sobre objetos constantes:

```cpp
const Heap heap(10U);
heap.empty();
```

Dentro de esos métodos no se pueden modificar normalmente `data_` ni `capacity_`.

Los accesores pequeños pueden definirse dentro de la clase:

```cpp
std::size_t size() const { return data_.size(); }
bool empty() const { return data_.empty(); }
```

Una función definida dentro de la clase es implícitamente `inline`.

## 12. Referencias En La Interfaz

### Entrada constante

```cpp
bool build(const std::vector<Process>& input);
```

`const std::vector<Process>&` significa:

- No copiar el vector solamente para llamar a `build`.
- No permitir que `build` modifique el vector del llamador.

### Parámetro de salida

```cpp
bool pop(Process& out);
```

`out` es una referencia a un objeto existente. Se usa directamente:

```cpp
Process result{-1, -1};
heap.pop(result);
```

A diferencia de `Process *out`, una referencia normal no se comprueba contra `nullptr`. La interfaz presupone que se recibió un objeto válido.

## 13. Orden De Implementación

```text
1. Corregir include guards                    heap.hpp
2. Completar índices y static_assert          indices.hpp
3. Definir Process y class Heap               heap.hpp
4. Implementar constructor y accesores        heap.cpp / heap.hpp
5. Implementar valid                          heap.cpp
6. Portar sift_down con intercambios          heap.cpp
7. Verificar build                            heap.cpp
8. Verificar push                             heap.cpp
9. Verificar pop                              heap.cpp
10. Optimizar sift_down con un temporal       heap.cpp
11. Crear pruebas propias                     main.cpp
```

La tarea principal del código base es `sift_down`, pero conviene comprender y probar el contrato de todos los métodos.

## 14. `valid`

Contrato:

```text
Devuelve true si cada padre tiene prioridad mayor o igual que sus hijos.
No modifica el heap.
```

Pseudocódigo:

```text
para i desde 1 hasta data_.size() - 1:
    p = parent(i)
    si data_[p].priority < data_[i].priority:
        devolver false

devolver true
```

No necesita comprobar punteros ni comparar `size` con capacidad: `std::vector` mantiene su propia representación válida y la clase controla el límite lógico.

Complejidad:

```text
Tiempo: O(n)
Espacio auxiliar: O(1)
```

## 15. Primera Versión De `sift_down`: Intercambios

Firma privada:

```cpp
void Heap::sift_down(std::size_t i);
```

### Contrato

Precondición:

```text
i es válido
los subárboles de sus hijos ya son max-heaps
la única posible violación está en i
```

Pasos:

1. Comprobar si existe hijo izquierdo.
2. Elegir al hijo de mayor prioridad.
3. Terminar si el proceso actual ya domina a ese hijo.
4. Intercambiar mediante `std::swap`.
5. Continuar desde el hijo elegido.

Estructura:

```cpp
while (i < data_.size() / 2U) {
  const std::size_t left_index = left(i);
  const std::size_t right_index = right(i);
  std::size_t best = left_index;

  // Si el derecho existe y tiene mayor prioridad, actualizar best.
  // Si el actual domina a best, terminar.
  // Intercambiar data_[i] y data_[best].
  // Continuar desde best.
}
```

Para utilizar `std::swap` se incluye:

```cpp
#include <utility>
```

Complejidad:

```text
Tiempo: O(log n)
Espacio auxiliar: O(1)
```

## 16. Segunda Versión De `sift_down`: Hueco Y Temporal

El laboratorio propone mejorar la constante de implementación evitando intercambios repetidos.

En lugar de realizar en cada nivel:

```text
temp = actual
actual = hijo
hijo = temp
```

se guarda una sola vez el proceso que desciende:

```cpp
Process value = data_[i];
```

Después se suben hijos hacia el hueco:

```text
guardar value

mientras exista hijo mayor que value:
    copiar hijo mayor en el hueco actual
    mover el hueco hacia la posición del hijo

colocar value en el hueco final
```

Invariante:

```text
value contiene el proceso que desciende
el resto del camino ya conserva los procesos necesarios
```

Estructura:

```cpp
Process value = data_[i];

while (i < data_.size() / 2U) {
  // Elegir al hijo mayor.

  if (data_[best].priority <= value.priority)
    break;

  data_[i] = data_[best];
  i = best;
}

data_[i] = value;
```

### Comparación

| Variante | Trabajo por nivel | Complejidad |
|---|---|---|
| Con `std::swap` | Varias asignaciones o movimientos | `O(log n)` |
| Con hueco | Una copia del hijo por nivel y colocación final | `O(log n)` |

La optimización no cambia la complejidad asintótica. Reduce la cantidad de movimientos de objetos.

> [!tip] Orden de aprendizaje
> Implementar primero la versión con `std::swap`, probarla y solo después sustituirla por la variante con temporal.

## 17. `build`

Contrato:

```text
Si input.size() > capacity_, devolver false sin modificar el heap.
En caso de éxito, copiar input y construir bottom-up.
```

Pasos:

1. Validar el límite lógico.
2. Asignar `input` a `data_`.
3. Recorrer los nodos internos desde el último hasta la raíz.
4. Llamar `sift_down` sobre cada uno.

Bucle seguro:

```cpp
for (std::size_t k = data_.size() / 2U; k > 0U; --k) {
  sift_down(k - 1U);
}
```

`data_ = input` cambia automáticamente el tamaño del vector. No hay que mantener un campo `size_` separado.

Complejidad:

```text
Tiempo: Theta(n)
Espacio auxiliar del algoritmo: O(1)
```

La copia del vector también cuesta `Theta(n)`.

## 18. `push`

Contrato:

```text
Si data_.size() == capacity_, devolver false sin modificar el heap.
En caso de éxito, insertar y restaurar hacia arriba.
```

Pasos:

1. Comprobar el límite lógico.
2. Añadir el proceso mediante `push_back`.
3. Obtener el índice `data_.size() - 1U`.
4. Comparar con el padre mientras el índice sea mayor que cero.
5. Intercambiar hacia arriba.

La condición debe comprobar primero:

```cpp
i > 0U
```

antes de evaluar:

```cpp
parent(i)
```

Como el constructor reservó `capacity_`, las inserciones dentro del límite no deberían requerir crecimiento de la reserva interna.

Complejidad:

```text
Tiempo: O(log n)
Espacio auxiliar: O(1)
```

## 19. `pop`

Contrato:

```text
Si el heap está vacío, devolver false y conservar out.
En caso de éxito, escribir un máximo en out y retirarlo.
```

Operaciones de `vector` utilizadas:

```cpp
data_.front()  // raíz
data_.back()   // último elemento
data_.pop_back()
data_.empty()
```

Pasos:

1. Rechazar un heap vacío.
2. Copiar `front()` en `out`.
3. Copiar `back()` sobre `front()`.
4. Ejecutar `pop_back()`.
5. Si quedan elementos, llamar `sift_down(0U)`.

Para un elemento, `front()` y `back()` designan el mismo proceso. Después de `pop_back`, el vector queda vacío y no se llama a `sift_down`.

Complejidad:

```text
Tiempo: O(log n)
Espacio auxiliar: O(1)
```

## 20. Manejo De Errores

La interfaz utiliza dos mecanismos distintos.

### Fallos esperados del TDA

```text
push en heap lleno -> false
pop en heap vacío  -> false
build demasiado grande -> false
```

### Fallo de reserva

```text
vector::reserve -> puede lanzar std::bad_alloc
```

El constructor no puede devolver `false`. Si se quiere manejar el fallo en `main`:

```cpp
try {
  heap11::Heap heap(100U);
  // usar heap
} catch (const std::bad_alloc&) {
  // no se pudo reservar memoria
}
```

Para `std::bad_alloc` se incluye:

```cpp
#include <new>
```

## 21. Pruebas Propias

Crear `me/main.cpp` y probar por etapas.

### Construcción Y Estado Inicial

```text
Capacidad cero.
Heap recién construido: empty() == true.
Heap recién construido: size() == 0.
Heap recién construido: valid() == true.
```

### `build`

```text
Vector vacío.
Un proceso.
Entrada desordenada de CLRS.
Entrada ya válida.
Entrada mayor que capacity_: false y estado anterior intacto.
```

### `push`

```text
Insertar en heap vacío.
Insertar sin ascenso.
Ascender hasta la raíz.
Prioridades duplicadas.
INT_MIN e INT_MAX.
Rechazar cuando size() == capacity_.
```

### `pop`

```text
Rechazar heap vacío conservando out.
Extraer un elemento.
Extraer repetidamente hasta vaciar.
Comprobar prioridades no crecientes.
Comprobar valid() después de cada extracción.
```

### Estrés

```text
Construir tamaños desde 0 hasta la capacidad.
Generar prioridades positivas y negativas.
Extraer todos los elementos.
Comprobar el orden y el invariante tras cada operación.
```

## 22. Esqueleto De `main.cpp`

```cpp
#include "heap.hpp"

#include <cassert>
#include <climits>
#include <iostream>
#include <vector>

int main() {
  heap11::Heap heap(10U);

  assert(heap.empty());
  assert(heap.size() == 0U);
  assert(heap.valid());

  std::vector<heap11::Process> input{
      {0, 4}, {1, 1},  {2, 3},  {3, 2}, {4, 16},
      {5, 9}, {6, 10}, {7, 14}, {8, 8}, {9, 7},
  };

  assert(heap.build(input));
  assert(heap.valid());

  heap11::Process output{-1, -1};
  int previous = INT_MAX;

  while (heap.pop(output)) {
    assert(output.priority <= previous);
    previous = output.priority;
    assert(heap.valid());
  }

  std::cout << "C++11 personal: OK\n";
}
```

No se llama a `heap_destroy`: `std::vector` libera la reserva automáticamente al destruirse `heap`.

## 23. Compilación

Desde `heaps/heaps`:

```bash
g++ -std=c++11 -Wall -Wextra -Wpedantic -Wconversion \
  me/heap.cpp me/main.cpp -o build/test_me_cpp11

./build/test_me_cpp11
```

Con sanitizadores:

```bash
g++ -std=c++11 -Wall -Wextra -Wpedantic -Wconversion \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  me/heap.cpp me/main.cpp -o build/test_me_cpp11_sanitize

./build/test_me_cpp11_sanitize
```

## 24. Complejidades Esperadas

| Operación | Tiempo | Espacio auxiliar |
|---|---:|---:|
| Constructor | `O(1)` en el modelo usual; la reserva depende del asignador | Reserva `O(capacity)` |
| `size`, `empty` | `O(1)` | `O(1)` |
| `valid` | `O(n)` | `O(1)` |
| `sift_down` | `O(log n)` | `O(1)` |
| `build` | `Theta(n)` | `O(1)` algorítmico |
| `push` | `O(log n)` | `O(1)` |
| `pop` | `O(log n)` | `O(1)` |

La complejidad asintótica del heap no cambia respecto de C11.

## 25. Errores Frecuentes

- Confundir `reserve` con `resize`.
- Acceder a `data_[0]` después de `reserve` cuando `size() == 0`.
- Usar `data_.capacity()` como límite lógico en lugar de `capacity_`.
- Calcular `parent(0U)`.
- Acceder al hijo derecho sin comprobar `right_index < data_.size()`.
- Reparar hacia abajo después de insertar.
- Reparar hacia arriba después de extraer la raíz.
- Procesar padres antes que hijos en `build`.
- Usar un bucle descendente incorrecto con `std::size_t`.
- Olvidar `const` en métodos que no modifican el heap.
- Copiar el vector de entrada por valor innecesariamente.
- Añadir un destructor manual para liberar la memoria de `vector`.
- Suponer que `constexpr` obliga a ejecutar siempre durante la compilación.
- Suponer que `inline` garantiza una optimización.

## 26. Lista De Comprobación

- [ ] `heap.hpp` tiene un include guard completo y específico.
- [ ] La clase se encuentra en `namespace heap11`.
- [ ] `Process` conserva juntos PID y prioridad.
- [ ] `data_` y `capacity_` son privados.
- [ ] El constructor es `explicit`.
- [ ] El constructor usa una lista de inicialización.
- [ ] `reserve` no se confunde con `resize`.
- [ ] Los accesores son métodos `const`.
- [ ] Los índices son funciones `constexpr`.
- [ ] Los `static_assert` verifican fórmulas en compilación.
- [ ] Nunca se calcula `parent(0U)`.
- [ ] `sift_down` comprueba la existencia del hijo derecho.
- [ ] `build` recorre nodos internos de abajo hacia arriba.
- [ ] `push` conserva el estado cuando está lleno.
- [ ] `pop` conserva `out` cuando está vacío.
- [ ] Las extracciones producen prioridades no crecientes.
- [ ] `valid()` es verdadero después de cada operación exitosa.
- [ ] No existe una llamada manual a `free` ni `heap_destroy`.
- [ ] Compila sin advertencias en C++11.
- [ ] ASan y UBSan no detectan errores.

## Primer Objetivo

Completar y revisar solamente:

```text
me/indices.hpp
me/heap.hpp
```

Después se implementarán el constructor, `size`, `empty` y `valid`. Finalmente se portará `sift_down` primero con `std::swap` y luego con la variante optimizada mediante un temporal.
