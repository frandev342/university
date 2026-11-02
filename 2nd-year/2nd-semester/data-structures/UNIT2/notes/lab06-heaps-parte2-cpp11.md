---
title: "Lab 6 de heaps - Parte II: C++11"
aliases:
  - "Heaps en C++11"
  - "Lab 6 Parte II"
tags:
  - estructuras-de-datos
  - heaps
  - cola-de-prioridad
  - cpp11
  - vector
  - raii
  - constexpr
status: desarrollado
---

# Lab 6 de heaps - Parte II: C++11

> [!abstract] Objetivo
> Portar a C++11 el max-heap de procesos desarrollado en C11, conservando sus invariantes y algoritmos, pero reemplazando la administración manual de memoria por una clase encapsulada que utiliza `std::vector`, RAII, referencias y funciones `constexpr`.

## Enlaces relacionados

- [[heaps-binarios-y-colas-de-prioridad]]: teoría general de heaps y colas de prioridad.
- [[lab06-heaps-parte1-c11]]: implementación previa en C11.
- [[lab06-heaps-c11-fundamentos]]: macros, `static`, `inline` y literales sin signo.
- `docs/ED_Lab06.pdf`: guía original del laboratorio.
- `heaps/heaps/cpp11/`: implementación C++11 integrada con las pruebas del laboratorio.
- `heaps/heaps/me/`: implementación personal desarrollada y revisada paso a paso.

## 1. Continuidad Desde C11

El cambio de lenguaje no modifica la estructura matemática del heap.

Se conservan:

- La representación de un árbol binario casi completo en posiciones contiguas.
- Los índices desde cero.
- La propiedad de max-heap basada solamente en `priority`.
- La construcción bottom-up.
- La reparación hacia arriba durante `push`.
- La reparación hacia abajo durante `pop`.
- Las complejidades asintóticas.

Para todo índice `i` con `1 <= i < size()` debe cumplirse:

```text
data_[parent(i)].priority >= data_[i].priority
```

El proceso de máxima prioridad siempre se encuentra en:

```cpp
data_[0]
```

> [!important] C++11 cambia la representación, no el algoritmo
> El heap sigue funcionando con las mismas propiedades. Lo que cambia es quién administra la memoria, cómo se expresa la interfaz y qué garantías proporciona el sistema de tipos.

## 2. Representación En C++11

Cada elemento continúa siendo un proceso compuesto por su identificador y su prioridad:

```cpp
struct Process {
  int pid;
  int priority;
};
```

La clase que representa el heap es:

```cpp
class Heap {
public:
  explicit Heap(std::size_t capacity);

  bool build(const std::vector<Process>& input);
  bool push(Process x);
  bool pop(Process& out);

  bool valid() const;
  std::size_t size() const { return data_.size(); }
  bool empty() const { return data_.empty(); }

private:
  void sift_down(std::size_t i);

  std::vector<Process> data_;
  std::size_t capacity_;
};
```

| Miembro | Responsabilidad |
|---|---|
| `data_` | Almacenar los procesos activos en posiciones contiguas. |
| `capacity_` | Mantener el límite lógico fijo establecido al construir el heap. |
| `sift_down` | Reparar un subárbol hacia abajo sin exponer la operación al usuario. |

Los elementos activos ocupan:

```text
data_[0] ... data_[data_.size() - 1]
```

Ya no existe un campo `size_`: `std::vector` conserva su tamaño internamente.

## 3. Encapsulación Y Clase

En C11, los campos de `HeapC` eran visibles para cualquier función que recibiera la estructura. En C++11 se separa la interfaz pública de la representación privada.

### Sección pública

```cpp
public:
  explicit Heap(std::size_t capacity);
  bool build(const std::vector<Process>& input);
  bool push(Process x);
  bool pop(Process& out);
  bool valid() const;
  std::size_t size() const;
  bool empty() const;
```

Estas son las operaciones que puede ejecutar un usuario del TDA.

### Sección privada

```cpp
private:
  void sift_down(std::size_t i);
  std::vector<Process> data_;
  std::size_t capacity_;
```

El usuario no puede modificar arbitrariamente `data_` ni cambiar `capacity_`. Esto ayuda a conservar los invariantes del heap.

> [!note] `class` y `struct`
> En C++, ambos pueden tener métodos y control de acceso. La diferencia predeterminada es que una `class` tiene miembros privados y una `struct` miembros públicos. `Process` es una estructura de datos simple; `Heap` protege una representación y por eso se expresa como clase.

## 4. Constructor, Lista De Inicialización Y `explicit`

```cpp
Heap::Heap(std::size_t capacity) : capacity_(capacity) {
  data_.reserve(capacity_);
}
```

La lista de inicialización:

```cpp
: capacity_(capacity)
```

construye directamente el miembro `capacity_` con el valor recibido.

El especificador:

```cpp
explicit Heap(std::size_t capacity);
```

evita conversiones implícitas como:

```cpp
Heap heap = 10U; // rechazado
```

La construcción intencional continúa permitida:

```cpp
Heap heap(10U);
```

`data_` no aparece expresamente en la lista porque su constructor por defecto crea un vector vacío válido.

## 5. `std::vector`: Tamaño, Capacidad Y Límite Lógico

Durante la práctica fue necesario distinguir tres valores:

| Expresión | Significado |
|---|---|
| `data_.size()` | Cantidad de objetos `Process` que existen actualmente en el vector. |
| `data_.capacity()` | Cantidad de elementos para los que el vector tiene almacenamiento reservado antes de necesitar crecer. |
| `capacity_` | Máximo lógico permitido por el TDA del laboratorio. |

Después de:

```cpp
data_.reserve(10U);
```

puede cumplirse:

```text
data_.size()     == 0
data_.capacity() >= 10
capacity_        == 10
```

### `reserve` no crea elementos

```cpp
data_.reserve(10U);
data_[0] = Process{1, 100}; // inválido: size() sigue siendo cero
```

`operator[]` solamente permite acceder a índices existentes:

```text
i < data_.size()
```

La capacidad física disponible no convierte esas posiciones en elementos del vector.

> [!failure] Error corregido durante el desarrollo de `build`
> Se intentó copiar `input` mediante `data_[i] = input[i]` después de usar solamente `reserve`. El código compilaba, pero accedía fuera del tamaño lógico del vector y producía comportamiento indefinido.

### `resize` sí crea o destruye elementos

```cpp
data_.resize(6U);
```

Después de esta operación existen seis objetos y son válidos los índices de `0` a `5`.

Por ejemplo:

```cpp
std::vector<int> values{10, 20, 30, 40, 50};
values.resize(6U);
```

produce conceptualmente:

```text
[10, 20, 30, 40, 50, 0]
```

### `push_back` crea un elemento al final

```cpp
data_.push_back(x);
```

La operación construye un nuevo elemento al final y aumenta `size()` en uno.

### La asignación de vectores ajusta el tamaño

```cpp
data_ = input;
```

Esta es una asignación por copia de `std::vector`, no un constructor de copia. `data_` ya existe.

La operación:

- Copia todos los elementos de `input`.
- Ajusta automáticamente `data_.size()`.
- Elimina elementos sobrantes si el nuevo vector es menor.
- Deja `data_` vacío si `input` está vacío.
- Puede reutilizar almacenamiento ya reservado cuando la implementación puede hacerlo.

Por eso es preferible a una copia manual precedida por `resize`.

## 6. RAII Y Administración De Memoria

RAII significa **Resource Acquisition Is Initialization**. El recurso pertenece a un objeto y su duración queda asociada al ciclo de vida de ese objeto.

En C11 era necesario administrar la reserva manualmente:

```c
HeapC heap;
heap_init(&heap, 10U);

/* utilizar heap */

heap_destroy(&heap);
```

En C++11:

```cpp
{
  heap11::Heap heap(10U);
  // utilizar heap
} // data_ se destruye y libera su memoria automáticamente
```

No se necesita:

- Llamar a `malloc`.
- Comprobar un puntero devuelto por `malloc`.
- Llamar a `free`.
- Implementar `heap_destroy`.
- Reinicializar manualmente `data`, `size` y `capacity` después de liberar.

Si `reserve` no puede obtener memoria, normalmente propaga una excepción como `std::bad_alloc`. El constructor no devuelve `false` porque todavía no existe un objeto completamente construido al cual consultar.

> [!success] Responsabilidad eliminada
> `std::vector` aplica RAII y evita fugas provocadas por olvidar una destrucción manual en las rutas normales y excepcionales del programa.

## 7. Espacio De Nombres

Los tipos y funciones se agrupan en:

```cpp
namespace heap11 {
  // Process, Heap, parent, left y right.
}
```

Desde fuera se utilizan como:

```cpp
heap11::Heap heap(10U);
heap11::Process process{42, 100};
```

El espacio de nombres evita conflictos con otras versiones del laboratorio:

```text
heap11::Heap
heap23::Heap
```

Un mismo `namespace` puede abrirse en varios archivos. `heap.hpp`, `heap.cpp` e `indices.hpp` siguen perteneciendo al mismo espacio lógico.

## 8. Include Guards

Cada encabezado necesita un identificador completo y específico:

```cpp
#ifndef ME_HEAP_CPP11_HPP
#define ME_HEAP_CPP11_HPP

// Declaraciones.

#endif // ME_HEAP_CPP11_HPP
```

El guard evita que el contenido del encabezado sea procesado más de una vez en una misma unidad de traducción.

Un nombre demasiado genérico, como `HEAP`, puede colisionar con otro encabezado. El comentario de `#endif` no afecta al compilador, pero debe coincidir con el nombre real para no confundir al lector.

## 9. Índices Como Funciones `constexpr`

En C11 se utilizaron macros o funciones `static inline`. En C++11 pueden expresarse las fórmulas mediante funciones tipadas:

```cpp
inline constexpr std::size_t parent(std::size_t i) {
  return (i - 1U) / 2U;
}

inline constexpr std::size_t left(std::size_t i) {
  return 2U * i + 1U;
}

inline constexpr std::size_t right(std::size_t i) {
  return 2U * i + 2U;
}
```

Ventajas respecto de una macro:

- Parámetros y retorno tipados.
- El argumento se evalúa una sola vez.
- Mejores diagnósticos del compilador.
- Respeto por el espacio de nombres.
- Posibilidad de evaluación durante la compilación.
- Uso normal durante la ejecución cuando el argumento no es constante.

En C++11, una función `constexpr` es implícitamente `inline`. Escribir ambas palabras es redundante, pero válido y didáctico:

```cpp
inline constexpr
```

### Verificación Durante La Compilación

```cpp
static_assert(parent(9U) == 4U, "incorrect parent");
static_assert(left(4U) == 9U, "incorrect left child");
static_assert(right(4U) == 10U, "incorrect right child");
```

`static_assert` exige que la expresión sea evaluable durante la compilación y detiene la compilación si es falsa.

Esto no significa que toda llamada a una función `constexpr` ocurra durante la compilación:

```cpp
std::size_t i;
std::cin >> i;
std::size_t p = parent(i); // ejecución
```

> [!danger] `constexpr` no valida precondiciones
> `parent(0U)` continúa siendo incorrecto. La resta sin signo produce underflow antes de la división. Solo debe llamarse cuando `i > 0U`.

## 10. Métodos `const` Y Métodos `inline`

Los métodos que solo consultan el heap se declaran `const`:

```cpp
bool valid() const;
std::size_t size() const;
bool empty() const;
```

El `const` situado después de los paréntesis se aplica al objeto implícito `this`. Dentro de esos métodos no puede modificarse normalmente `data_` ni `capacity_`.

Esto permite consultar un heap constante:

```cpp
const heap11::Heap heap(10U);
bool is_empty = heap.empty();
```

Los accesores pequeños están definidos dentro de la clase:

```cpp
std::size_t size() const { return data_.size(); }
bool empty() const { return data_.empty(); }
```

Una función definida dentro de una clase es implícitamente `inline`. Como en C, `inline` no obliga al compilador a sustituir llamadas; su significado también permite que la misma definición aparezca en varias unidades de traducción.

## 11. Referencias En La Interfaz

### Referencia constante de entrada

```cpp
bool build(const std::vector<Process>& input);
```

La referencia evita copiar el vector solo para llamar a `build`. El `const` impide que la función modifique el vector del usuario.

### Referencia de salida

```cpp
bool pop(Process& out);
```

`out` designa directamente un objeto proporcionado por el usuario:

```cpp
heap11::Process result{-1, -1};

if (heap.pop(result)) {
  // result contiene el proceso extraído.
}
```

A diferencia de un puntero, una referencia normal no se compara con `nullptr`. La interfaz presupone que se recibió un objeto válido.

### Paso por valor en `push`

```cpp
bool push(Process x);
```

`Process` contiene solamente dos enteros, por lo que copiarlo es económico. Además, `x` sirve como temporal estable mientras otros procesos se desplazan hacia abajo durante el ascenso por hueco.

## 12. Validación Del Heap

```cpp
bool Heap::valid() const {
  for (std::size_t i = 1U; i < size(); ++i) {
    if (data_[parent(i)].priority < data_[i].priority)
      return false;
  }
  return true;
}
```

El recorrido empieza en `1U` por dos razones:

- La raíz no tiene padre.
- Evita calcular `parent(0U)`.

Si algún hijo tiene prioridad mayor que su padre, el heap es inválido. No es necesario comparar hermanos ni ordenar todo el vector.

Complejidad:

```text
Tiempo: O(n)
Espacio auxiliar: O(1)
```

## 13. Reparación Hacia Abajo: `sift_down`

### Contrato

Antes de llamar a `sift_down(i)`:

- `i` debe ser un índice válido.
- Los subárboles de sus hijos ya deben ser max-heaps.
- La única posible violación se encuentra en `i`.

Después de la llamada, el subárbol con raíz en `i` vuelve a ser un max-heap.

### Primera versión: intercambios

La versión inicial intercambia el proceso actual con su hijo de mayor prioridad:

```cpp
void Heap::sift_down(std::size_t i) {
  while (i < size() / 2U) {
    const std::size_t left_index = left(i);
    const std::size_t right_index = right(i);
    std::size_t best = left_index;

    if (right_index < size() &&
        data_[right_index].priority > data_[left_index].priority) {
      best = right_index;
    }

    if (data_[i].priority >= data_[best].priority)
      return;

    std::swap(data_[i], data_[best]);
    i = best;
  }
}
```

La condición:

```cpp
i < size() / 2U
```

indica que `i` pertenece al conjunto de nodos internos y, por tanto, tiene hijo izquierdo.

El hijo derecho se consulta solamente después de comprobar:

```cpp
right_index < size()
```

### Segunda versión: hueco y temporal

La versión final evita intercambiar dos objetos en cada nivel:

```cpp
void Heap::sift_down(std::size_t i) {
  Process value = data_[i];

  while (i < size() / 2U) {
    const std::size_t left_index = left(i);
    const std::size_t right_index = right(i);
    std::size_t best = left_index;

    if (right_index < size() &&
        data_[right_index].priority > data_[left_index].priority) {
      best = right_index;
    }

    if (value.priority >= data_[best].priority)
      break;

    data_[i] = data_[best];
    i = best;
  }

  data_[i] = value;
}
```

El proceso que desciende se conserva en:

```cpp
Process value = data_[i];
```

`i` representa la posición actual del hueco. En cada iteración se sube el mejor hijo y el hueco baja a la posición que ese hijo ocupaba.

Invariante del ciclo:

```text
value conserva el proceso que está descendiendo
i identifica el hueco actual
los procesos desplazados hacia arriba no se pierden
los subárboles inferiores continúan siendo heaps
```

Al terminar, `value` se escribe una sola vez en su posición definitiva.

| Variante | Trabajo aproximado por nivel | Complejidad |
|---|---|---|
| Intercambios | Varias copias o movimientos por intercambio | `O(log n)` |
| Hueco | Una copia del hijo y una colocación final | `O(log n)` |

La optimización reduce constantes, pero no cambia la complejidad asintótica.

## 14. Construcción Bottom-Up: `build`

```cpp
bool Heap::build(const std::vector<Process>& input) {
  if (input.size() > capacity_)
    return false;

  data_ = input;

  for (std::size_t i = data_.size() / 2U; i > 0U; --i)
    sift_down(i - 1U);

  return true;
}
```

### Paso 1: validar antes de modificar

```cpp
if (input.size() > capacity_)
  return false;
```

La validación ocurre antes de asignar `input`. Si falla, el heap conserva su estado anterior.

### Paso 2: copiar y ajustar el tamaño

```cpp
data_ = input;
```

La asignación del vector sustituye todo el contenido anterior y establece el tamaño correcto.

### Paso 3: recorrer padres de abajo hacia arriba

Las hojas ya son heaps de un elemento. El primer índice que no corresponde a una hoja es:

```text
data_.size() / 2 - 1
```

El ciclo se expresa de forma segura para `std::size_t`:

```cpp
for (std::size_t i = data_.size() / 2U; i > 0U; --i)
  sift_down(i - 1U);
```

No se escribe una condición como `i >= 0`, porque un entero sin signo siempre es mayor o igual que cero y el ciclo no terminaría correctamente.

Complejidad:

```text
Copia del vector: Theta(n)
Heapify bottom-up: Theta(n)
Total: Theta(n)
Espacio auxiliar del algoritmo: O(1)
```

Aunque `sift_down` cuesta `O(log n)` en el peor caso, la construcción completa es `Theta(n)` porque la mayoría de nodos están cerca de las hojas y recorren muy pocos niveles.

## 15. Inserción: `push`

La versión optimizada utiliza el mismo concepto de hueco que `sift_down`, pero en dirección ascendente:

```cpp
bool Heap::push(Process x) {
  if (data_.size() == capacity_)
    return false;

  data_.push_back(x);
  std::size_t i = data_.size() - 1U;

  while (i > 0U) {
    const std::size_t parent_index = parent(i);

    if (data_[parent_index].priority >= x.priority)
      break;

    data_[i] = data_[parent_index];
    i = parent_index;
  }

  data_[i] = x;
  return true;
}
```

### Capacidad fija

```cpp
if (data_.size() == capacity_)
  return false;
```

La comprobación ocurre antes de `push_back`, por lo que un heap lleno no se modifica.

### Crear la nueva posición

```cpp
data_.push_back(x);
```

No puede utilizarse directamente `data_[data_.size()] = x`, porque ese índice todavía no existe. `push_back` construye el elemento y aumenta el tamaño.

### Evitar `parent(0U)`

```cpp
while (i > 0U)
```

La condición se comprueba antes de calcular el padre.

### El proceso que asciende es `x`

Durante el ascenso, `i` representa un hueco conceptual. Por eso la comparación siempre debe hacerse contra `x.priority`.

> [!failure] Error corregido durante el desarrollo de `push`
> Inicialmente se comparó al padre con `data_[i].priority`. Después del primer desplazamiento, `data_[i]` contenía un valor antiguo y ya no representaba al proceso que ascendía. La implementación fallaba cuando un elemento necesitaba subir más de un nivel.

Ejemplo del error al insertar `100`:

```text
Heap inicial:             [3, 1, 2]
Después de push_back:     [3, 1, 2, 100]
Primer desplazamiento:    [3, 1, 2, 1]
Hueco conceptual: índice 1
```

En ese momento debe compararse `3` con `100`, no con el `1` que permanece físicamente en la posición del hueco.

La condición correcta es:

```cpp
if (data_[parent_index].priority >= x.priority)
  break;
```

Complejidad:

```text
Mejor caso: O(1)
Peor caso: O(log n)
Espacio auxiliar: O(1)
```

## 16. Extracción: `pop`

```cpp
bool Heap::pop(Process& out) {
  if (data_.empty())
    return false;

  out = data_.front();
  data_.front() = data_.back();
  data_.pop_back();

  if (!data_.empty())
    sift_down(0U);

  return true;
}
```

Pasos:

1. Rechazar el heap vacío sin modificar `out`.
2. Copiar la raíz, que contiene un máximo, en `out`.
3. Mover el último elemento a la raíz.
4. Eliminar la última posición mediante `pop_back`.
5. Reparar hacia abajo si todavía quedan elementos.

Operaciones de `vector` utilizadas:

```cpp
data_.front()  // primer elemento
data_.back()   // último elemento
data_.pop_back()
data_.empty()
```

### Caso de un elemento

Si el vector contiene solo `[A]`, `front()` y `back()` designan el mismo objeto:

```text
out = A
A se asigna a sí mismo
pop_back deja el vector vacío
no se llama a sift_down
```

La autoasignación es innecesaria, pero válida. Separar este caso puede evitarla, aunque agrega una rama especial sin aportar una mejora relevante para este tipo pequeño.

Complejidad:

```text
Mejor caso: O(1)
Peor caso: O(log n)
Espacio auxiliar: O(1)
```

## 17. Contratos De Error

La interfaz utiliza `bool` para fallos esperados del TDA:

| Operación | Condición de fallo | Estado después del fallo |
|---|---|---|
| `build` | `input.size() > capacity_` | El heap anterior se conserva. |
| `push` | `size() == capacity_` | El heap no se modifica. |
| `pop` | `empty() == true` | El heap y `out` no se modifican. |

Los fallos de reserva de memoria son diferentes. `std::vector` los comunica mediante excepciones, no mediante el valor booleano de estas operaciones.

> [!important] Validar antes de modificar
> En las tres operaciones, la condición esperada de fallo se comprueba antes de cualquier cambio observable. Esto facilita razonar sobre el estado del objeto.

## 18. Comparación Entre C11 Y C++11

| Aspecto | C11 | C++11 |
|---|---|---|
| Tipo principal | `HeapC` | `class Heap` |
| Almacenamiento | `Process*` | `std::vector<Process>` |
| Tamaño actual | Campo `size` | `data_.size()` |
| Capacidad lógica | Campo `capacity` | Miembro `capacity_` |
| Reserva | `malloc` | `vector::reserve` |
| Liberación | `free` en `heap_destroy` | Destructor automático |
| Operaciones | Funciones con `HeapC*` | Métodos sobre `this` |
| Entrada de `build` | Puntero y cantidad | `const std::vector<Process>&` |
| Salida de `pop` | `Process*` | `Process&` |
| Índices | Macros o `static inline` | Funciones `constexpr` |
| Encapsulación | Convención | `public` y `private` |
| Error de reserva | `NULL` / `false` | Excepción |

### Qué ya no debe hacerse

```cpp
malloc(...);       // innecesario para data_
free(...);         // vector libera automáticamente
heap_destroy(...); // no existe
```

### Qué continúa siendo responsabilidad del algoritmo

- Respetar `capacity_`.
- Mantener la propiedad de orden.
- No calcular el padre de la raíz.
- Comprobar que el hijo derecho existe.
- Elegir correctamente entre reparación ascendente y descendente.
- Mantener juntos `pid` y `priority` al mover un proceso.

## 19. Complejidades

| Operación | Tiempo | Espacio auxiliar |
|---|---:|---:|
| Constructor | Reserva dependiente del asignador | Reserva `O(capacity)` |
| `size` | `O(1)` | `O(1)` |
| `empty` | `O(1)` | `O(1)` |
| `valid` | `O(n)` | `O(1)` |
| `sift_down` | `O(log n)` | `O(1)` |
| `build` | `Theta(n)` | `O(1)` algorítmico |
| `push` | `O(log n)` | `O(1)` |
| `pop` | `O(log n)` | `O(1)` |

La asignación inicial de `input` en `build` también requiere tiempo lineal. El almacenamiento de los `n` procesos pertenece a la representación del heap y no se cuenta como espacio auxiliar del algoritmo de reparación.

## 20. Pruebas Realizadas

### Compilación Estricta

```bash
g++ -std=c++11 -Wall -Wextra -Wpedantic -Wconversion \
  me/heap.cpp tests/test_cpp11.cpp \
  -o /tmp/opencode/test-me-cpp11
```

Resultado:

```text
C++11: OK
```

No se produjeron advertencias del compilador.

### Sanitizadores

```bash
g++ -std=c++11 -Wall -Wextra -Wpedantic -Wconversion \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  me/heap.cpp tests/test_cpp11.cpp \
  -o /tmp/opencode/test-me-cpp11-sanitize
```

Resultado:

```text
C++11: OK
ASan: sin errores detectados
UBSan: sin errores detectados
```

### Casos Cubiertos

- Heap recién construido y vacío.
- Capacidad cero.
- `build` con tamaños desde `0` hasta `64`.
- Entradas desordenadas.
- Prioridades positivas y negativas.
- Prioridades repetidas.
- Valores `INT_MIN` e `INT_MAX`.
- Inserciones que no ascienden.
- Inserciones que ascienden varios niveles.
- Rechazo de `push` cuando el heap está lleno.
- Rechazo de `pop` cuando el heap está vacío.
- Extracción de un único proceso.
- Extracciones repetidas hasta vaciar.
- Orden no creciente de todas las prioridades extraídas.
- `valid()` después de cada operación relevante.

> [!note] Alcance de las pruebas
> Superar las pruebas aumenta la confianza en la implementación, pero no sustituye una demostración de corrección. Los invariantes y contratos explican por qué los algoritmos deben funcionar para entradas generales.

## 21. Errores Y Observaciones Del Desarrollo

### Confundir capacidad con tamaño

Código problemático:

```cpp
data_.reserve(capacity_);
data_[0] = x;
```

Corrección conceptual:

```text
reserve -> almacenamiento disponible
resize  -> elementos existentes
push_back -> crea un elemento al final
```

### Suponer que `operator[]` amplía el vector

Si un vector tiene cinco elementos, sus índices válidos son `0..4`:

```cpp
values[5] = 4; // compila, pero tiene comportamiento indefinido
```

Para añadir el sexto elemento:

```cpp
values.push_back(4);
```

O para crear primero esa posición:

```cpp
values.resize(6U);
values[5] = 4;
```

### Comparar contra el hueco en `push`

Después de desplazar un padre, `data_[i]` contiene un valor antiguo. El proceso que asciende se conserva en `x`, así que las comparaciones deben usar `x.priority`.

### Acceder al hijo derecho sin comprobarlo

Un nodo puede tener solamente hijo izquierdo. Debe evaluarse primero:

```cpp
right_index < size()
```

El cortocircuito de `&&` garantiza que el acceso al hijo derecho ocurra solo si existe.

### Bucle descendente con `std::size_t`

Esta forma es incorrecta:

```cpp
for (std::size_t i = last; i >= 0U; --i)
```

`std::size_t` no representa números negativos. La forma segura es:

```cpp
for (std::size_t i = data_.size() / 2U; i > 0U; --i)
  sift_down(i - 1U);
```

### Creer que `inline` obliga a optimizar

Ni `inline` ni `constexpr` garantizan que el compilador sustituya una llamada. La decisión de optimización depende del compilador y sus opciones.

## 22. Ideas Centrales Aprendidas

1. Un `std::vector` administra tanto almacenamiento como objetos construidos.
2. `capacity()` y `size()` representan conceptos distintos.
3. `reserve` no habilita el acceso mediante índices; `resize`, `push_back` y la asignación sí crean elementos.
4. La asignación `data_ = input` ajusta correctamente contenido y tamaño.
5. RAII elimina la necesidad de una destrucción manual del heap.
6. La encapsulación impide que el usuario rompa directamente el invariante.
7. Los métodos `const` documentan y verifican que una consulta no modifica el objeto.
8. Las referencias expresan entrada y salida sin usar punteros anulables.
9. `constexpr` permite verificar las fórmulas de índices mediante `static_assert`.
10. `constexpr` no elimina la precondición `i > 0` de `parent`.
11. El patrón de hueco conserva un proceso en un temporal y desplaza solo los elementos del camino.
12. En `push`, las comparaciones deben hacerse contra el proceso temporal que asciende.
13. En `sift_down`, las comparaciones deben hacerse contra el proceso temporal que desciende.
14. `build` bottom-up sigue siendo `Theta(n)` en C++11.
15. Las pruebas con sanitizadores complementan, pero no sustituyen, el razonamiento mediante invariantes.

---

## 23. Documentación De Entrega Como Autoevaluación

> [!info] Aclaración
> Esta sección adopta el formato de una documentación de entrega para completar la revisión personal. No corresponde a una entrega pendiente ni implica que estos archivos deban enviarse.

### 23.1. Objetivo Revisado

Se implementó y estudió una cola de prioridad máxima de capacidad fija en C++11. La implementación utiliza `std::vector<Process>` para almacenar procesos, compara exclusivamente el campo `priority` y conserva las siguientes garantías:

```text
size() <= capacity_

para todo i > 0:
data_[parent(i)].priority >= data_[i].priority
```

Los empates de prioridad pueden extraerse en cualquier orden.

### 23.2. Archivos Revisados

| Archivo | Contenido |
|---|---|
| `me/heap.hpp` | Interfaz pública, `Process` y clase `Heap`. |
| `me/indices.hpp` | Fórmulas `constexpr` y verificaciones `static_assert`. |
| `me/heap.cpp` | Constructor, validación y operaciones del heap. |
| `me/main.cpp` | Programa de experimentación personal. |
| `tests/test_cpp11.cpp` | Pruebas automatizadas de referencia. |

### 23.3. Funcionalidad Implementada

| Función | Estado | Observación |
|---|---|---|
| Constructor | Completa | Reserva almacenamiento mediante `vector::reserve`. |
| `size` / `empty` | Completa | Consultas constantes y de tiempo `O(1)`. |
| `valid` | Completa | Verifica la relación de cada nodo con su padre. |
| `sift_down` | Completa | Variante optimizada mediante hueco y temporal. |
| `build` | Completa | Copia el vector y construye bottom-up. |
| `push` | Completa | Inserta y repara hacia arriba mediante hueco. |
| `pop` | Completa | Extrae la raíz y repara hacia abajo. |

### 23.4. Tres Pruebas Propias Justificadas

#### Prueba 1: ascenso de varios niveles

Entrada conceptual:

```text
Insertar prioridades: 1, 2, 3, 100
```

Objetivo:

```text
Comprobar que el último proceso puede atravesar más de un nivel y llegar a la raíz.
```

Justificación:

```text
Una prueba donde el proceso sube un solo nivel no detecta el error de comparar
contra el contenido físico del hueco en vez de comparar contra x.priority.
```

Resultado:

```text
La prueba detectó el error inicial de push.
Después de corregir la comparación con x.priority, valid() permaneció verdadero.
```

#### Prueba 2: extracción completa y orden

Entrada conceptual:

```text
Construir heaps con tamaños de 0 a 64 y prioridades positivas, negativas y repetidas.
Extraer hasta vaciar cada heap.
```

Objetivo:

```text
Comprobar simultáneamente pop, sift_down, reducción del tamaño y orden de salida.
```

Condiciones verificadas:

```cpp
assert(out.priority <= previous_priority);
assert(heap.valid());
assert(heap.size() == remaining - 1U);
```

Resultado:

```text
Todas las extracciones conservaron el heap y produjeron prioridades no crecientes.
```

#### Prueba 3: contratos de frontera

Casos:

```text
Heap con capacidad cero.
push cuando size() == capacity_.
pop sobre heap vacío.
build con entrada mayor que capacity_.
```

Objetivo:

```text
Verificar que los fallos esperados devuelven false sin modificar el estado previo.
```

Resultado:

```text
Las operaciones fueron rechazadas correctamente y se conservaron sus parámetros
de salida o el contenido anterior, según el contrato correspondiente.
```

### 23.5. Resultados De Compilación Y Ejecución

Configuración:

```text
Estándar: C++11
Advertencias: -Wall -Wextra -Wpedantic -Wconversion
Sanitizadores: AddressSanitizer y UndefinedBehaviorSanitizer
```

Resultado observado:

```text
C++11: OK
Sin advertencias del compilador.
Sin errores informados por ASan.
Sin errores informados por UBSan.
```

### 23.6. Macro, `static inline` Y `constexpr`

| Propiedad | Macro C | `static inline` C | `constexpr` C++11 |
|---|---|---|---|
| Sustitución textual | Sí | No | No |
| Parámetros tipados | No | Sí | Sí |
| Evaluación única del argumento | Depende de la macro | Sí | Sí |
| Alcance mediante namespace | No | No | Sí |
| Uso en expresión constante | Limitado al texto producido | Depende del contexto de C | Sí, cuando los argumentos lo permiten |
| Verificación con `static_assert` | Indirecta | No necesariamente constante | Sí |

Conclusión:

```text
constexpr expresa mejor las fórmulas de índices en C++11 porque conserva tipos,
alcance y evaluación única, y además permite comprobar ejemplos durante la
compilación. Ninguna alternativa elimina las precondiciones del algoritmo.
```

### 23.7. Compilación Frente A Ejecución

`static_assert` comprueba durante la compilación:

```cpp
static_assert(parent(9U) == 4U, "incorrect parent");
```

Una llamada cuyos datos se conocen solo durante la ejecución se evalúa normalmente:

```cpp
std::size_t p = parent(i);
```

`constexpr` significa que una función puede participar en evaluación constante cuando el contexto y los argumentos lo permiten. No significa que todas sus llamadas se ejecuten durante la compilación.

### 23.8. Memoria Administrada

En C11, el programador era responsable de:

```text
calcular bytes
detectar overflow
llamar malloc
comprobar NULL
mantener size y capacity
llamar free
evitar fugas y dobles liberaciones
```

En C++11, `std::vector` se responsabiliza de:

```text
reservar almacenamiento
construir elementos
destruir elementos
mantener size y capacidad interna
liberar la memoria durante la destrucción
```

El algoritmo todavía mantiene `capacity_` porque es una regla lógica del TDA, no una necesidad de `std::vector`.

### 23.9. Posibles Sesgos Al Comparar Rendimiento

Una comparación de rendimiento entre C11 y C++11 puede ser engañosa si no se controlan:

- La misma versión del compilador.
- El mismo nivel de optimización, por ejemplo `-O2`.
- La misma máquina y carga del sistema.
- La misma distribución y cantidad de datos.
- El costo de reserva y asignación de memoria.
- La presencia de sanitizadores o símbolos de depuración.
- La entrada/salida por consola dentro de la región medida.
- La eliminación de código sin efectos observables por el optimizador.
- El calentamiento de cachés y la cantidad de repeticiones.
- Diferencias algorítmicas accidentales entre las versiones comparadas.

No debe concluirse que una función es más rápida solamente por estar marcada `inline` o `constexpr`. Para comparar implementaciones debe medirse el programa optimizado, repetir el experimento y observar tanto tiempo como código generado.

### 23.10. Uso De Inteligencia Artificial

La inteligencia artificial se utilizó como apoyo para:

- Explicar conceptos de C++11 y `std::vector`.
- Revisar incrementalmente las implementaciones escritas.
- Diseñar casos límite y pruebas temporales.
- Ejecutar compilación con advertencias y sanitizadores.
- Identificar errores de razonamiento, como la diferencia entre `reserve` y `resize` y la comparación incorrecta durante el ascenso de `push`.
- Organizar esta documentación de autoevaluación.

El desarrollo se realizó de forma incremental: primero se estudió el contrato de cada operación, luego se implementó manualmente, se revisó el resultado y finalmente se comprobó mediante pruebas. La herramienta no sustituye la comprensión de los invariantes ni la responsabilidad de validar el código.

### 23.11. Lista Final De Autoevaluación

- [x] La clase está dentro de `namespace heap11`.
- [x] `Process` conserva juntos PID y prioridad.
- [x] `data_` y `capacity_` son privados.
- [x] El constructor es `explicit` y usa lista de inicialización.
- [x] Se distingue entre `reserve`, `resize`, `size` y `capacity`.
- [x] La memoria es administrada mediante RAII.
- [x] Los accesores y `valid` son métodos `const`.
- [x] Los índices se expresan mediante funciones `constexpr`.
- [x] Los `static_assert` comprueban las fórmulas básicas.
- [x] Nunca se calcula `parent(0U)` dentro de los algoritmos.
- [x] `sift_down` comprueba la existencia del hijo derecho.
- [x] `build` recorre los nodos internos de abajo hacia arriba.
- [x] `push` conserva el estado cuando el heap está lleno.
- [x] `push` compara los padres contra el proceso temporal que asciende.
- [x] `pop` conserva `out` cuando el heap está vacío.
- [x] Las extracciones producen prioridades no crecientes.
- [x] `valid()` permanece verdadero después de las operaciones exitosas.
- [x] No existe una liberación manual de la memoria de `vector`.
- [x] La implementación compila sin advertencias en C++11.
- [x] Las pruebas pasan con ASan y UBSan.

> [!success] Estado de la Parte II
> La implementación C++11 está completa y validada. La revisión permitió comprender cómo `std::vector`, RAII, referencias, métodos `const`, encapsulación y `constexpr` cambian la expresión del TDA sin modificar los algoritmos fundamentales del heap.
