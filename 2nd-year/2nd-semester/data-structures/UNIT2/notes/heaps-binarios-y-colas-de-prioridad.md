# Heaps binarios y colas de prioridad

Estas notas desarrollan el contenido de `docs/06 - Heaps.pdf`, basado en CLRS y complementado con MIT 6.006. El foco es el **max-heap binario**: una representación eficiente del TDA cola de prioridad máxima.

Como apoyo práctico se proponen dos directorios hermanos de estas notas:

- [`../me/heaps/reference/`](../me/heaps/reference/): implementación de referencia para consultar después de intentar cada ejercicio.
- [`../me/heaps/practice/`](../me/heaps/practice/): esqueleto incompleto sobre el que implementar y probar las operaciones.

Los nombres usados aquí (`siftDown`, `buildHeap`, `maximum`, `extractMax`, `siftUp`, `insert` e `increaseKey`) coinciden con la implementación de referencia. Al crear otra interfaz pueden cambiar los nombres, pero deben preservarse sus contratos.

## 1. Motivación: planificación de procesos

Un **programa** es código almacenado. Un **proceso** es un programa en ejecución junto con su estado y los recursos que el sistema operativo administra: contador de programa, registros, memoria, archivos, etc.

En un modelo sencillo de cinco estados, un proceso puede estar en creación, listo, corriendo, esperando o terminado. Un proceso **listo** puede ejecutarse, pero espera a que la CPU quede disponible. Cuando hay varios procesos listos, el planificador debe elegir cuál despachar.

Una cola FIFO elegiría al primero que llegó. Esa política ignora la urgencia. Supongamos estos procesos y que una clave mayor significa mayor prioridad:

| Proceso | Orden de llegada | Prioridad |
|---|---:|---:|
| `P1` | 1 | 2 |
| `P2` | 2 | 8 |
| `P3` | 3 | 5 |

FIFO elegiría `P1`; una cola de prioridad máxima elegiría `P2`. El ejemplo motiva tres necesidades: insertar un proceso listo, consultar el más prioritario y extraerlo cuando vaya a usar la CPU. Cambiar dinámicamente una prioridad introduce una cuarta operación.

Una cola de prioridad solo organiza la elección según claves. No define por sí sola una política completa y justa de planificación: por ejemplo, dar siempre preferencia a claves altas puede causar inanición de prioridades bajas.

## 2. TDA cola de prioridad máxima

Un **tipo de dato abstracto** especifica qué hacen las operaciones, no cómo se representan internamente los datos. Para una colección `S` de elementos con clave:

| Operación | Efecto | Precondición principal |
|---|---|---|
| `insert(S, x, k)` | Inserta el elemento `x` con clave `k`. | Hay memoria o capacidad disponible. |
| `maximum(S)` | Devuelve algún elemento con clave máxima sin retirarlo. | `S` no está vacía. |
| `extractMax(S)` | Retira y devuelve algún elemento con clave máxima. | `S` no está vacía. |
| `increaseKey(S, x, k)` | Cambia la clave de `x` a `k`. | `x` está en `S` y `k >= key(x)`. |

`maximum` y `extractMax` son operaciones **parciales**: no tienen resultado normal para una colección vacía. Una API real debe documentar si lanza una excepción, devuelve un opcional o comunica el error de otra manera.

Las claves duplicadas son válidas. Si varios elementos comparten el máximo, el TDA puede devolver cualquiera de ellos salvo que se añada explícitamente una regla de desempate. Un heap ordinario no es una cola estable por orden de llegada.

### Costos según la representación

| Implementación | Insertar | Consultar máximo | Extraer máximo |
|---|---:|---:|---:|
| Arreglo sin ordenar | `O(1)` amortizado | `O(n)` | `O(n)` |
| Arreglo ordenado | `O(n)` | `O(1)` | `O(1)` |
| Max-heap binario | `O(log n)` | `Theta(1)` | `O(log n)` |

En el arreglo sin ordenar, insertar al final cuesta `O(1)` si hay capacidad; con un arreglo dinámico de crecimiento geométrico es `O(1)` **amortizado**, no necesariamente en cada inserción. En el arreglo ordenado, esta tabla supone que el máximo se guarda en un extremo desde el cual retirarlo cuesta constante; mantener el orden desplaza hasta `n` elementos al insertar.

El heap ofrece un equilibrio: mantiene el máximo en una posición conocida y evita mantener todos los elementos globalmente ordenados.

## 3. Qué es un heap binario

Un heap binario satisface dos propiedades independientes.

### 3.1. Propiedad de forma

Es un árbol binario **casi completo**:

- Todos los niveles salvo quizá el último están llenos.
- El último nivel se ocupa de izquierda a derecha, sin huecos.
- Con `n >= 1`, su altura medida en aristas es `floor(log2(n))`.
- Su forma permite almacenarlo de manera contigua, sin punteros a hijos.

La forma depende únicamente de `n`, no de las claves. Para `n = 0` no hay raíz; puede convenirse altura `-1`, pero esa convención no es necesaria para implementar las operaciones.

### 3.2. Propiedad de orden

En un **max-heap**, la clave de cada padre es mayor o igual que la de cada hijo:

```text
key(parent(i)) >= key(i)
```

En un **min-heap** se invierte la desigualdad. Este capítulo desarrolla max-heaps; las ideas son simétricas.

La relación es local, pero por transitividad implica que todo ancestro es mayor o igual que sus descendientes. Por ello, la raíz contiene un máximo global. No implica que:

- los hermanos estén ordenados entre sí;
- el recorrido por niveles forme un arreglo ordenado;
- exista una posición determinada para claves duplicadas.

Por ejemplo, `[20, 13, 18]` es un max-heap válido aunque el hijo izquierdo sea menor que el derecho. También `[20, 20, 18]` es válido: se usa `>=`, no `>`.

## 4. Representación en arreglo

### 4.1. Convención de CLRS: índices desde 1

CLRS representa el heap en `A[1..heapSize]`:

```text
raíz       = 1
PARENT(i)  = floor(i / 2), para i > 1
LEFT(i)    = 2i
RIGHT(i)   = 2i + 1
```

Un hijo existe solo si su índice es `<= heapSize`. Para `n > 0`:

- nodos internos: `A[1..floor(n/2)]`;
- hojas: `A[floor(n/2) + 1..n]`.

Las hojas son heaps válidos de un solo elemento. Esa observación será la base de `buildHeap`.

### 4.2. Convención de C++: índices desde 0

Con `std::vector<T> a`, el heap ocupa `a[0..n-1]`:

```text
raíz       = 0
PARENT(i)  = floor((i - 1) / 2), para i > 0
LEFT(i)    = 2i + 1
RIGHT(i)   = 2i + 2
```

Para `n > 0`:

- nodos internos: índices `0..n/2 - 1`;
- hojas: índices `n/2..n - 1`.

La división es entera. Nunca debe calcularse el padre de la raíz con `std::size_t`: al ser un tipo sin signo, `i - 1` desbordaría cuando `i == 0`.

La traducción entre convenciones es `indiceCpp = indiceClrs - 1`. No deben mezclarse fórmulas de una columna con índices de la otra.

| Concepto | CLRS, base 1 | C++, base 0 |
|---|---:|---:|
| Raíz | `1` | `0` |
| Padre | `i / 2` | `(i - 1) / 2` |
| Hijo izquierdo | `2 * i` | `2 * i + 1` |
| Hijo derecho | `2 * i + 1` | `2 * i + 2` |
| Primer índice hoja | `floor(n/2) + 1` | `n / 2` |
| Último índice interno | `floor(n/2)` | `n / 2 - 1` |

## 5. Restaurar hacia abajo: `maxHeapify` o `siftDown`

### Contrato

`siftDown(a, i)` no convierte mágicamente cualquier arreglo en heap. Su contrato local es:

- **Precondición:** `i < a.size()` y los subárboles izquierdo y derecho de `i`, si existen, ya son max-heaps. La única violación posible está entre `a[i]` y sus hijos.
- **Postcondición:** el subárbol cuya raíz es `i` es un max-heap y contiene exactamente los mismos elementos que antes.
- **Costo:** `O(h)` tiempo, donde `h` es la altura del subárbol; en el peor caso `O(log n)`. La versión iterativa usa `O(1)` espacio auxiliar.

### Lógica iterativa en C++

Este fragmento representativo usa índices desde 0. La condición `i < n / 2` significa que `i` es interno y, por tanto, que el hijo izquierdo existe. Además evita calcular hijos para una hoja.

```cpp
void siftDown(std::vector<int>& a, std::size_t i) {
  const std::size_t n = a.size();

  while (i < n / 2) {
    const std::size_t left = 2 * i + 1;
    const std::size_t right = left + 1;
    std::size_t largest = left;

    if (right < n && a[right] > a[left])
      largest = right;

    if (a[i] >= a[largest])
      return;

    std::swap(a[i], a[largest]);
    i = largest;
  }
}
```

Se elige el hijo de mayor clave. Intercambiar simplemente con el izquierdo puede dejar al padre por debajo del derecho. Con hijos iguales, escoger cualquiera es correcto; la comparación estricta anterior escoge el izquierdo.

### Invariante de ciclo

Al comienzo de cada iteración:

- los subárboles hijos de la posición actual son max-heaps;
- fuera del camino recorrido no se ha creado ninguna violación;
- la única clave que todavía puede estar demasiado abajo respecto de sus hijos es la que se está descendiendo.

Si `a[i]` ya domina al hijo mayor, domina a ambos y el algoritmo termina. Si no, al intercambiar con el hijo mayor, la nueva raíz local domina a los dos hijos. La posible violación se desplaza estrictamente un nivel hacia abajo. Como la altura es finita, el ciclo termina.

### Traza del ejemplo del PDF

El PDF aplica `MAX-HEAPIFY` al índice CLRS `i = 2`, que corresponde al índice C++ `1`:

```text
Inicial:       [16,  4, 10, 14, 7, 9, 3, 2, 8, 1]
índice actual:       1
```

Los hijos de `4` son `14` y `7`, y ambos subárboles ya son heaps. Se intercambia con el mayor:

```text
4 < 14
[16, 14, 10,  4, 7, 9, 3, 2, 8, 1]
                ^ i = 3
```

Ahora los hijos de `4` son `2` y `8`; vuelve a intercambiarse con el mayor:

```text
4 < 8
[16, 14, 10,  8, 7, 9, 3, 2, 4, 1]
                            ^ i = 8, hoja
```

La clave `4` descendió dos niveles. El multiconjunto se conservó y el árbol completo volvió a ser max-heap.

## 6. Construcción de abajo hacia arriba: `buildHeap`

Las hojas ya son heaps. Por eso solo hay que procesar los nodos internos desde el último hasta la raíz, es decir, **de abajo hacia arriba**. Al llegar a un nodo, sus hijos tienen índices mayores y sus subárboles ya fueron reparados; así se satisface la precondición de `siftDown`.

Con `std::size_t` no es seguro escribir `for (i = n/2 - 1; i >= 0; --i)`: `i >= 0` siempre es verdadero para un entero sin signo, y después de cero el índice pasa a un valor enorme. Esta forma no resta nunca a cero y es segura incluso para un vector vacío:

```cpp
void buildHeap(std::vector<int>& a) {
  for (std::size_t count = a.size() / 2; count > 0; --count)
    siftDown(a, count - 1);
}
```

`count` representa cuántos nodos internos faltan, no el índice que se procesa. Como el cuerpo solo se ejecuta si `count > 0`, `count - 1` siempre es válido y el decremento final nunca ocurre desde cero. Para `n = 10` se visitan `4, 3, 2, 1, 0`; para `n = 0` o `n = 1` no se visita ninguno.

### Traza de construcción

Para el arreglo de CLRS, expresado ahora con índices C++:

```text
entrada: [4, 1, 3, 2, 16, 9, 10, 14, 8, 7]
índices internos visitados: 4, 3, 2, 1, 0

i = 4: [4, 1, 3, 2, 16, 9, 10, 14, 8, 7]
i = 3: [4, 1, 3, 14, 16, 9, 10, 2, 8, 7]
i = 2: [4, 1, 10, 14, 16, 9, 3, 2, 8, 7]
i = 1: [4, 16, 10, 14, 7, 9, 3, 2, 8, 1]
i = 0: [16, 14, 10, 8, 7, 9, 3, 2, 4, 1]
```

Un heap resultante no es único. Distintas elecciones entre claves iguales pueden producir arreglos diferentes y todos ser correctos.

### Corrección

Invariante del recorrido: justo antes de procesar `i`, todos los nodos con índice mayor que `i` son raíces de subárboles que ya satisfacen la propiedad max-heap.

1. **Inicialización:** los índices desde `n/2` son hojas y ya son heaps.
2. **Mantenimiento:** los hijos de `i` tienen índices mayores, así que sus subárboles son heaps; `siftDown(a, i)` convierte también el subárbol de `i` en heap.
3. **Terminación:** después de procesar `i = 0`, el subárbol de la raíz, es decir, todo el arreglo, es un max-heap.

### Por qué tarda `Theta(n)`

Decir que hay `O(n)` llamadas y que cada una cuesta `O(log n)` produce la cota válida pero poco ajustada `O(n log n)`. No todas recorren la altura completa:

- aproximadamente `n/4` nodos internos tienen altura 1;
- aproximadamente `n/8` tienen altura 2;
- aproximadamente `n/16` tienen altura 3;
- en general, hay a lo sumo `ceil(n / 2^(h+1))` nodos de altura `h`.

Si el trabajo al descender una altura `h` está acotado por `c h`, entonces:

```text
T(n) <= c n * sumatorio desde h=0 hasta floor(log2 n) de h / 2^(h+1)
     <= c n * sumatorio desde h=0 hasta infinito de h / 2^(h+1)
     = c n
```

La serie converge a `1`, por lo que `T(n) = O(n)`. El bucle llama a `siftDown` para `floor(n/2)` nodos internos y cada llamada realiza al menos trabajo constante, de modo que también es `Omega(n)`. En consecuencia, `buildHeap` tarda **`Theta(n)`**.

## 7. Consultar y extraer el máximo

### `maximum`

La propiedad de orden coloca un máximo en la raíz:

```cpp
const int& maximum(const std::vector<int>& a) {
  if (a.empty())
    throw std::out_of_range("maximum on empty heap");
  return a.front();
}
```

No modifica el heap y tarda `Theta(1)`. Devolver una referencia evita copiar, pero el llamador no debe conservarla después de una operación que pueda realocar el vector. Para objetos pequeños también es razonable devolver por valor.

### `extractMax`

Al retirar la raíz quedaría un hueco, lo que rompería la forma casi completa. Para conservarla:

1. Guardar el máximo.
2. Mover la última clave a la raíz.
3. Eliminar la última posición.
4. Si el heap no quedó vacío, restaurar hacia abajo desde la raíz.

```cpp
int extractMax(std::vector<int>& a) {
  if (a.empty())
    throw std::out_of_range("extractMax on empty heap");

  int result = a.front();
  a.front() = a.back();
  a.pop_back();

  if (!a.empty())
    siftDown(a, 0);
  return result;
}
```

La asignación y `pop_back` conservan la forma. Antes de `siftDown`, ambos subárboles de la raíz siguen siendo heaps: solo se cambió la raíz. Por tanto se cumple su contrato.

Traza del PDF:

```text
Inicial:         [16, 14, 10, 8, 7, 9, 3, 2, 4, 1]
Mover el último: [ 1, 14, 10, 8, 7, 9, 3, 2, 4]
Primer descenso: [14,  1, 10, 8, 7, 9, 3, 2, 4]
Segundo:         [14,  8, 10, 1, 7, 9, 3, 2, 4]
Resultado:       [14,  8, 10, 4, 7, 9, 3, 2, 1]
```

El PDF mantiene un `heap-size` menor que la capacidad del arreglo y muestra el `1` extraído fuera del heap. Con un `vector`, `pop_back()` elimina físicamente esa posición; el heap resultante es `[14, 8, 10, 4, 7, 9, 3, 2, 1]`. El peor caso tarda `O(log n)` y el espacio auxiliar es `O(1)`.

## 8. Restaurar hacia arriba e insertar

### `siftUp`

Si una clave nueva se añade al final, la forma sigue siendo válida. El heap anterior no cambia, así que la única violación posible está entre el nuevo nodo y su padre. Se intercambia hacia arriba mientras el hijo sea mayor:

```cpp
void siftUp(std::vector<int>& a, std::size_t i) {
  while (i > 0) {
    const std::size_t parent = (i - 1) / 2;
    if (a[parent] >= a[i])
      return;
    std::swap(a[parent], a[i]);
    i = parent;
  }
}
```

El chequeo `i > 0` ocurre antes de calcular el padre. El invariante es que el arreglo satisface el orden excepto, posiblemente, en la arista entre la posición actual y su padre. Cada intercambio arregla esa arista y desplaza la posible violación un nivel hacia la raíz.

### Inserción sin centinela `-infinity`

CLRS presenta la inserción colocando primero una clave `-infinity` y llamando a `increaseKey`. Es una formulación matemática útil, pero un tipo C++ genérico puede no tener un valor que represente menos infinito. Tampoco debe usarse `std::numeric_limits<int>::min()` como centinela si esa clave es un dato válido.

La implementación directa no necesita centinela:

```cpp
void insert(std::vector<int>& a, int key) {
  a.push_back(key);
  siftUp(a, a.size() - 1);
}
```

`push_back` conserva la forma y `siftUp` restaura el orden. El costo de subir es `O(log n)`; para un `vector`, el crecimiento de almacenamiento añade costo amortizado constante. Por ello la inserción sigue siendo `O(log n)` amortizado y `O(log n)` si la capacidad ya es suficiente.

Los duplicados no provocan un ciclo: con la condición estricta `a[parent] < a[i]`, las claves iguales no se intercambian.

## 9. Aumentar una clave

Si se conoce el índice del elemento, `increaseKey` valida que la nueva clave no sea menor, la asigna y restaura hacia arriba:

```cpp
void increaseKey(std::vector<int>& a, std::size_t i, int newKey) {
  if (i >= a.size())
    throw std::out_of_range("heap index out of range");
  if (newKey < a[i])
    throw std::invalid_argument("new key is smaller");

  a[i] = newKey;
  siftUp(a, i);
}
```

No hace falta examinar hijos: aumentar una clave preserva las desigualdades con ellos y solo puede violar la relación con el padre. Si la nueva clave es igual a la anterior, la operación es válida y termina sin intercambios.

Ejemplo del PDF, convertido a índices desde 0:

```text
Antes:            [16, 14, 10, 8, 7, 9, 3, 2, 4, 1]
increaseKey(8,15):[16, 14, 10, 8, 7, 9, 3, 2,15, 1]
Intercambio 8-3:  [16, 14, 10,15, 7, 9, 3, 2, 8, 1]
Intercambio 3-1:  [16, 15, 10,14, 7, 9, 3, 2, 8, 1]
```

El ascenso termina porque `16 >= 15`.

### Índice conocido frente a objeto conocido

El costo `O(log n)` presupone que el índice ya se conoce. Si la API recibe solo un objeto `x`, buscarlo linealmente en el arreglo cuesta `O(n)`, que domina al ascenso.

Para conservar `O(log n)` puede mantenerse un mapa `id -> indice`, donde `id` identifica al objeto de manera única. Cada intercambio realizado por `siftUp`, `siftDown`, inserción o extracción debe actualizar las dos entradas afectadas. Con claves duplicadas no sirve un mapa `clave -> indice`: dos objetos pueden compartir la misma clave. Debe usarse identidad estable, por ejemplo un ID de proceso.

El contrato pasa a incluir un invariante adicional:

```text
positions[a[i].id] == i para todo índice válido i
```

Olvidar actualizar el mapa no rompe necesariamente el orden visible del heap, pero hace que una futura operación dirigida modifique el objeto equivocado o acceda a un índice obsoleto.

## 10. Resumen de complejidades

Sea `n` el número de elementos actuales:

| Operación | Tiempo | Espacio auxiliar | Condición |
|---|---:|---:|---|
| `maximum` | `Theta(1)` | `O(1)` | Heap no vacío. |
| `siftDown` | `O(log n)` | `O(1)` iterativo | Subheaps hijos válidos. |
| `buildHeap` | `Theta(n)` | `O(1)` | Cualquier arreglo. |
| `extractMax` | `O(log n)` | `O(1)` | Heap no vacío. |
| `siftUp` | `O(log n)` | `O(1)` | Solo posible violación hacia el padre. |
| `insert` | `O(log n)` amortizado | `O(1)` auxiliar | Puede asignarse almacenamiento. |
| `increaseKey` con índice | `O(log n)` | `O(1)` | Nueva clave no menor. |
| `increaseKey` buscando objeto | `O(n)` | `O(1)` | Sin mapa de posiciones. |
| `increaseKey` con mapa | `O(log n)` esperado | `O(n)` para el mapa | Mapa sincronizado; hash esperado `O(1)`. |

`O(log n)` es una cota de peor caso para las operaciones de subida o bajada; si no hay violación, terminan en `Theta(1)`.

## 11. Invariantes de representación y corrección

Una implementación es más fácil de razonar si separa sus invariantes:

1. **Forma:** los `n` elementos ocupan exactamente `a[0..n-1]`. Un `vector` contiguo sin huecos la proporciona automáticamente.
2. **Orden:** para cada `i > 0`, `a[(i - 1) / 2] >= a[i]`.
3. **Conservación:** reparar solo intercambia elementos; no pierde ni duplica claves.
4. **Límites:** un índice se lee únicamente si es menor que `n`.
5. **Posiciones, si existen:** el mapa y el arreglo se describen mutuamente de forma consistente.

Cada operación rompe como máximo una parte controlada del orden:

| Operación preparatoria | Posible violación | Reparación correcta |
|---|---|---|
| Reemplazar raíz por último | Hacia los hijos | `siftDown(0)` |
| Añadir al final | Hacia el padre | `siftUp(n - 1)` |
| Aumentar una clave | Hacia el padre | `siftUp(i)` |
| Disminuir una clave, extensión no incluida en el TDA | Hacia los hijos | `siftDown(i)` |

Esta tabla explica por qué intercambiar arbitrariamente y llamar a una reparación cualquiera no basta: hay que demostrar primero qué zona sigue siendo heap.

## 12. Errores frecuentes

- Confundir árbol casi completo con árbol de búsqueda. En un heap, el hijo izquierdo no tiene por qué ser menor que el derecho.
- Creer que el arreglo queda ordenado. Solo se garantiza el orden ancestro-descendiente.
- Exigir claves distintas. Los duplicados son completamente válidos.
- Mezclar índices desde 1 y desde 0, especialmente en las fórmulas de los hijos.
- Acceder a `right` antes de verificar `right < n`.
- Ejecutar `siftDown` sobre un nodo cuyos subárboles todavía no son heaps.
- Construir desde la raíz hacia las hojas. Esa dirección no preserva la precondición de `siftDown`.
- Usar `for (std::size_t i = n/2 - 1; i >= 0; --i)`, que desborda y además falla inicialmente para `n < 2`.
- Llamar `siftDown(a, 0)` después de extraer el único elemento sin contemplar que el vector quedó vacío.
- Usar `-infinity` artificial aunque el mínimo del tipo pueda ser una clave válida.
- Permitir que `increaseKey` reciba una clave menor; la reparación hacia arriba ya no sería adecuada.
- Afirmar que `increaseKey(x)` cuesta `O(log n)` sin explicar cómo se localiza `x`.
- Mantener un mapa por clave cuando existen duplicados, en lugar de usar una identidad única.
- Actualizar el arreglo pero no el mapa de posiciones durante un intercambio.
- Esperar estabilidad entre prioridades iguales sin almacenar y comparar un criterio adicional.

## 13. Estrategia de pruebas

No basta con comparar un arreglo final fijo, porque varios heaps pueden ser válidos. Conviene comprobar propiedades.

### Oráculo de validez

```cpp
bool isMaxHeap(const std::vector<int>& a) {
  for (std::size_t i = 1; i < a.size(); ++i) {
    if (a[(i - 1) / 2] < a[i])
      return false;
  }
  return true;
}
```

Para cada operación también debe comprobarse que se conserva el multiconjunto de elementos, salvo la inserción o extracción esperada.

### Casos deterministas mínimos

- Vector vacío y vector de un elemento.
- Dos elementos en ambos órdenes.
- Cantidades pares e impares de elementos.
- Arreglo ya heap, estrictamente creciente y estrictamente decreciente.
- Todos los valores iguales y mezcla de duplicados.
- Valores negativos, cero, mínimo y máximo representables del tipo.
- Nodo interno con solo hijo izquierdo.
- `siftDown` sin intercambio, con uno y con descenso hasta una hoja.
- `siftUp` sin intercambio y con ascenso hasta la raíz.
- Excepciones de `maximum`, `extractMax`, índice inválido y disminución en `increaseKey`.

### Pruebas de secuencias

1. Insertar muchos valores y verificar `isMaxHeap` después de cada inserción.
2. Extraer hasta vaciar y comprobar que la secuencia devuelta es no creciente.
3. Construir desde arreglos aleatorios y comparar el multiconjunto antes y después.
4. Comparar una secuencia aleatoria de operaciones con un oráculo sencillo como `std::multiset`.
5. Si hay mapa de posiciones, verificar `positions[a[i].id] == i` después de **cada** operación.

Las pruebas aleatorias detectan combinaciones imprevistas, pero no reemplazan casos pequeños diseñados para cubrir límites y contratos.

## 14. Ejercicios escalonados de implementación

Los ejercicios están pensados para resolverse en [`../me/heaps/practice/`](../me/heaps/practice/) y contrastarse después con [`../me/heaps/reference/`](../me/heaps/reference/). Cada etapa debe compilar y probarse antes de avanzar.

### Ejercicio 1: `isMaxHeap`

**Objetivo:** reconocer si un vector completo satisface el orden max-heap.

**Precondición:** el vector representa un árbol casi completo por su almacenamiento contiguo.

**Pasos del algoritmo:**

1. Recorrer cada índice `i` desde `1`.
2. Calcular `(i - 1) / 2`.
3. Devolver `false` si el padre es menor que el hijo.
4. Devolver `true` si no se encontró violación.

**Postcondición:** el resultado es verdadero si y solo si toda arista padre-hijo cumple `padre >= hijo`.

**Complejidad:** `Theta(n)` tiempo y `O(1)` espacio.

### Ejercicio 2: `siftDown`

**Objetivo:** restaurar un subárbol cuya única posible violación está en su raíz.

**Precondición:** `i` es válido y los subárboles de sus hijos ya son max-heaps.

**Pasos del algoritmo:**

1. Mientras `i` sea un nodo interno, obtener sus hijos existentes.
2. Elegir el hijo de mayor clave.
3. Terminar si la clave actual es mayor o igual.
4. Intercambiar y continuar desde el hijo elegido.

**Postcondición:** el subárbol de `i` original es max-heap y conserva sus elementos.

**Complejidad:** `O(log n)` tiempo en el peor caso y `O(1)` espacio iterativo.

### Ejercicio 3: `buildHeap`

**Objetivo:** transformar cualquier vector en max-heap in situ.

**Precondición:** ninguna propiedad de orden; el vector puede estar vacío.

**Pasos del algoritmo:**

1. Identificar `n / 2` como el primer índice hoja.
2. Recorrer los índices internos en orden descendente con un patrón seguro para enteros sin signo.
3. Invocar `siftDown` en cada uno.

**Postcondición:** todo el vector satisface la propiedad max-heap y conserva el mismo multiconjunto.

**Complejidad:** `Theta(n)` tiempo y `O(1)` espacio auxiliar.

### Ejercicio 4: `maximum` y `extractMax`

**Objetivo:** consultar y retirar la raíz respetando los contratos del TDA.

**Precondición:** heap válido; para ambas operaciones, no vacío.

**Pasos del algoritmo:**

1. En `maximum`, validar y devolver la raíz sin modificar nada.
2. En `extractMax`, guardar la raíz y mover el último elemento a ella.
3. Reducir el vector.
4. Si quedan elementos, ejecutar `siftDown` desde cero.
5. Devolver el valor guardado.

**Postcondición:** `maximum` no modifica el heap; `extractMax` devuelve un máximo y deja un heap con una aparición menos de ese valor.

**Complejidad:** `Theta(1)` para `maximum`; `O(log n)` para `extractMax`; `O(1)` espacio auxiliar.

### Ejercicio 5: `siftUp` e `insert`

**Objetivo:** insertar directamente, sin depender de un centinela menor que todas las claves.

**Precondición:** antes de insertar, el vector es un max-heap.

**Pasos del algoritmo:**

1. Añadir la clave al final.
2. Mientras no esté en la raíz, comparar con su padre.
3. Terminar si el padre domina la clave.
4. Si no, intercambiar y continuar desde el padre.

**Postcondición:** el resultado es un max-heap con una aparición adicional de la clave.

**Complejidad:** `O(log n)` tiempo amortizado con `vector` y `O(1)` espacio auxiliar.

### Ejercicio 6: `increaseKey` por índice

**Objetivo:** aumentar la prioridad de un elemento ya localizado.

**Precondición:** heap válido, índice dentro del rango y `newKey >= a[i]`.

**Pasos del algoritmo:**

1. Validar índice y monotonía de la nueva clave.
2. Asignar la clave.
3. Ejecutar `siftUp` desde `i`.

**Postcondición:** el vector vuelve a ser max-heap; solo cambió la clave indicada y ahora vale `newKey`.

**Complejidad:** `O(log n)` tiempo y `O(1)` espacio.

### Ejercicio 7: elementos con identidad y mapa de posiciones

**Objetivo:** implementar aumento de prioridad por ID sin búsqueda lineal y admitir claves duplicadas.

**Precondición:** cada elemento tiene ID único; arreglo y mapa están sincronizados.

**Pasos del algoritmo:**

1. Representar cada entrada como `{id, key}`.
2. Consultar el índice mediante `positions[id]`.
3. Centralizar los intercambios en una función que actualice el arreglo y ambos índices del mapa.
4. Reutilizar esa función en `siftUp` y `siftDown`.
5. Actualizar el mapa también al insertar y extraer.

**Postcondición:** se preservan simultáneamente el orden del heap y la correspondencia `ID <-> indice`.

**Complejidad:** `O(log n)` tiempo esperado por operación dirigida con tabla hash y `O(n)` espacio adicional.

### Ejercicio 8: pruebas de propiedades

**Objetivo:** verificar la estructura sin depender de una única disposición válida.

**Precondición:** están disponibles las operaciones anteriores y un generador de casos.

**Pasos del algoritmo:**

1. Generar vectores con tamaños, signos y duplicados variados.
2. Ejecutar `buildHeap` y comprobar `isMaxHeap` y conservación del multiconjunto.
3. Extraer repetidamente y comprobar orden no creciente.
4. Ejecutar secuencias mixtas contra `std::multiset`.

**Postcondición:** toda secuencia probada coincide con el modelo y preserva los invariantes después de cada paso.

**Complejidad:** depende del número de casos; documentar por separado el costo del algoritmo probado y el del oráculo.

## 15. Cuatro preguntas del PDF, con respuestas

### 1. ¿Un max-heap obliga a que los hermanos estén ordenados?

No. La propiedad compara cada hijo con su padre, no un hijo con el otro. `[10, 3, 8]` es un max-heap: `10 >= 3` y `10 >= 8`, aunque el hermano izquierdo sea menor que el derecho. Elegir al mayor de los hijos en `siftDown` es una necesidad del algoritmo de reparación, no una regla de orden permanente entre hermanos.

### 2. Con `A = [1, 2, 3, 4]`, ¿por qué falla aplicar `HEAPIFY` primero en `i = 1` y luego en `i = 2`?

Los índices de la pregunta son CLRS, desde 1. La primera llamada en `i = 1` no cumple la precondición: su subárbol izquierdo, enraizado en `i = 2` y formado por `2` con hijo `4`, todavía no es max-heap. Si aun así se ejecuta, se obtiene:

```text
[1, 2, 3, 4] --HEAPIFY(1)--> [3, 2, 1, 4]
                 --HEAPIFY(2)--> [3, 4, 1, 2]
```

El último arreglo viola el orden en la raíz porque `3 < 4`. Reparar después al hijo puede subir una clave por encima del padre ya procesado. En orden correcto, `HEAPIFY(2)` produce `[1, 4, 3, 2]` y luego `HEAPIFY(1)` produce `[4, 2, 3, 1]`.

### 3. Si llamamos repetidamente a `HEAPIFY`, ¿por qué la construcción no toma `Theta(n log n)`?

Porque `O(log n)` es el costo máximo de una llamada, no el costo de todas. La mitad de los nodos son hojas y ni siquiera se procesan; aproximadamente un cuarto puede bajar como máximo un nivel, un octavo como máximo dos, etc. La suma ponderada `n * sum(h / 2^(h+1))` es lineal. Como además se procesan `floor(n/2)` nodos internos, el costo total es `Theta(n)`.

### 4. ¿Qué precondición se pierde si reparáramos los padres antes que sus hijos?

Se pierde la garantía de que los dos subárboles hijos ya sean max-heaps al llamar a `siftDown` sobre el padre. Una reparación posterior en un hijo puede elevar dentro de ese subárbol una clave mayor que el padre, reintroduciendo una violación en una zona que ya no se visitará. El recorrido descendente de índices, de hijos hacia padres, mantiene exactamente esa precondición.

## 16. Extensión opcional

**Heapsort** puede construirse a partir de `buildHeap` y extracciones conceptuales dentro del mismo arreglo. No forma parte del contenido desarrollado en el PDF de esta unidad, por lo que se deja únicamente como extensión opcional; no debe confundirse con las operaciones requeridas para el TDA cola de prioridad.

## 17. Lista de comprobación

Antes de considerar terminada una implementación:

- Las fórmulas corresponden a índices desde 0.
- `siftDown` elige al mayor de los hijos existentes.
- Todos los accesos a hijos verifican límites.
- `buildHeap` recorre solo nodos internos y en dirección descendente.
- El bucle descendente funciona con `std::size_t`, incluso para `n = 0`.
- `extractMax` trata correctamente heaps de un elemento.
- `insert` no depende de un centinela inexistente.
- `increaseKey` rechaza disminuciones y distingue localizar de reparar.
- Los duplicados funcionan y no se promete estabilidad accidentalmente.
- Las pruebas verifican orden, forma, conservación y, si existe, el mapa de posiciones.

## 18. Compilación y ejecución

Para compilar y ejecutar la implementación de referencia con advertencias y sanitizadores:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Wconversion \
  -fsanitize=address,undefined \
  me/heaps/reference/main.cpp me/heaps/reference/max_heap.cpp \
  -o /tmp/max_heap_reference

/tmp/max_heap_reference
```

Mientras se completa el esqueleto de práctica, puede comprobarse que cada archivo sigue compilando con:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Wconversion \
  -c me/heaps/practice/max_heap_practice.cpp \
  -o /tmp/max_heap_practice.o
```

Los métodos pendientes lanzan `std::logic_error`; deben sustituirse uno a uno siguiendo los contratos descritos en la sección 14.

## Referencias

- Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest y Clifford Stein, *Introduction to Algorithms*, secciones sobre heaps binarios y colas de prioridad.
- MIT OpenCourseWare, 6.006 *Introduction to Algorithms* (Fall 2011), Lecture 4: *Heaps and Heap Sort*.
- Material de la unidad: [`../docs/06 - Heaps.pdf`](../docs/06%20-%20Heaps.pdf).
