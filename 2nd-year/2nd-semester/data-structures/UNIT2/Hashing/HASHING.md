---
title: "Hashing: encadenamiento y sondeo lineal"
aliases:
  - "Tablas hash"
  - "Hashing lineal"
tags:
  - estructuras-de-datos
  - hashing
  - diccionarios
  - colisiones
  - sondeo-lineal
  - cpp
status: desarrollado
---

# Hashing: Encadenamiento Y Sondeo Lineal

> [!abstract] Objetivo
> Entender cómo una tabla hash implementa un diccionario, por qué ocurren colisiones y cómo se resuelven mediante encadenamiento o direccionamiento abierto con sondeo lineal. Al final se relaciona cada idea con `Hashing/hashingColiciones/` y con el trabajo en `Hashing/me/`.

## Material Relacionado

- `docs/04 - Hashing.pdf`: material teórico principal disponible en `docs/`.
- `Hashing/hashingColiciones/`: laboratorio con encadenamiento terminado y sondeo lineal pendiente.
- `Hashing/me/`: implementación personal desarrollada desde cero.

> [!note] Sobre el nombre «hashing lineal»
> En este proyecto significa **direccionamiento abierto con sondeo lineal** (*linear probing*). No se refiere a *linear hashing*, que también es el nombre de otra técnica para hacer crecer archivos o tablas dinámicas.

## 1. El TDA Diccionario

Un diccionario mantiene asociaciones:

```text
clave -> valor
```

Ejemplos:

```text
código de estudiante -> nombre
DNI                  -> persona
palabra              -> frecuencia
nombre de usuario    -> datos de cuenta
```

Una clave presente tiene **un único valor vigente**. Insertar la misma clave otra vez actualiza su valor; no crea otra asociación independiente.

Las operaciones básicas del TDA son:

| Operación abstracta | Significado |
|---|---|
| Crear | Obtener un diccionario vacío. |
| Insertar/actualizar | Asociar una clave con un valor. |
| Buscar | Obtener el valor de una clave presente. |
| `contains` | Saber si una clave está presente. |
| Eliminar | Quitar una asociación si existe. |
| Tamaño | Número de asociaciones actuales. |

En el laboratorio, la interfaz concreta combina búsqueda y ausencia:

```cpp
bool get(unsigned key, int& value) const;
```

- Si encuentra `key`, escribe su valor en `value` y devuelve `true`.
- Si no la encuentra, devuelve `false` y debe conservar `value` sin cambios.

La inserción es:

```cpp
bool put(unsigned key, int value);
```

En `hashingColiciones`, `true` significa que la operación se completó, tanto si insertó una clave nueva como si actualizó una existente. `false` representa imposibilidad de insertar, por falta de memoria o por tabla llena según la representación.

> [!important] TDA frente a representación
> El comportamiento del diccionario es el mismo usando listas, sondeo lineal, árboles u otra representación. Lo que cambia es cómo se guardan los datos y cuánto cuestan las operaciones.

## 2. Por Qué No Usar Solamente Una Lista O Un Arreglo Ordenado

Con una lista no ordenada, para buscar una clave ausente puede ser necesario visitar sus `n` nodos:

```text
buscar: O(n)
actualizar: O(n), porque primero hay que encontrar la clave
eliminar: O(n)
```

Un arreglo ordenado permite búsqueda binaria `O(log n)`, pero insertar o eliminar puede desplazar `O(n)` elementos.

Hashing intenta responder esta pregunta:

```text
¿Podemos calcular directamente una zona pequeña donde debería estar la clave?
```

## 3. Acceso Directo Y Su Problema

Si las únicas claves posibles fueran `0..7`, podríamos usar la propia clave como índice:

```text
table[5] contiene directamente el valor de la clave 5
```

Buscar, insertar y eliminar serían `Θ(1)`. Esto se llama **direccionamiento directo**.

El problema aparece cuando el universo de claves `U` es enorme. Para claves entre `0` y `99 999 999`, una tabla directa necesita cien millones de posiciones aunque solo almacenemos mil asociaciones.

Hashing usa una tabla de `m` posiciones, normalmente comparable con el número de claves almacenadas, no con todas las claves posibles.

## 4. Función Hash

Una función hash convierte una clave en un índice válido:

```text
h : U -> {0, 1, ..., m - 1}
```

Notación:

| Símbolo | Significado |
|---|---|
| `U` | Universo de claves posibles. |
| `n` | Asociaciones almacenadas actualmente. |
| `m` | Número de casillas de la tabla. |
| `h(k)` | Índice inicial calculado para la clave `k`. |

El proyecto usa el método de división:

```text
h(k) = k mod m
```

Con `m = 7`:

| Clave | `k % 7` |
|---:|---:|
| 10 | 3 |
| 17 | 3 |
| 24 | 3 |
| 8 | 1 |
| 15 | 1 |
| 9 | 2 |

Una buena función debe ser rápida, determinista durante la vida de la tabla y distribuir adecuadamente las claves esperadas.

> [!warning] Usar un módulo primo no elimina automáticamente patrones malos
> Con `m = 11`, las claves `22, 33, 44, 55` caen todas en cero. La distribución depende de la función **y** de las claves recibidas.

## 5. Colisiones

Hay una colisión cuando dos claves diferentes producen el mismo índice:

```text
a != b y h(a) == h(b)
```

Por ejemplo:

```text
10 % 7 = 3
17 % 7 = 3
24 % 7 = 3
```

Una colisión no significa que las claves sean iguales. Significa que la función hash eligió la misma casilla inicial y necesitamos una regla para continuar.

Si el universo contiene más claves posibles que casillas, las colisiones son inevitables por el principio del palomar. El objetivo no es fingir que no existen, sino distribuirlas y resolverlas correctamente.

Este proyecto compara dos soluciones:

1. **Encadenamiento:** cada casilla contiene una lista de asociaciones.
2. **Direccionamiento abierto con sondeo lineal:** todas las asociaciones viven dentro del mismo arreglo y una colisión hace avanzar a la siguiente casilla.

## 6. Hashing Con Encadenamiento

La tabla es un arreglo de cabezas de listas:

```text
bucket[0] -> NULL
bucket[1] -> 15 -> 8 -> NULL
bucket[2] -> 9 -> NULL
bucket[3] -> 24 -> 17 -> 10 -> NULL
bucket[4] -> NULL
bucket[5] -> NULL
bucket[6] -> NULL
```

Cada nodo almacena:

```cpp
struct Node {
  unsigned key;
  int value;
  Node* next;
};
```

### Buscar

Para buscar `17`:

1. Calcular `17 % 7 = 3`.
2. Recorrer solamente `bucket[3]`.
3. Comparar con `24`, luego con `17`.

Para buscar `31`, también empezamos en `3`, pero se recorre toda la lista hasta `nullptr` porque la clave no está.

### Insertar O Actualizar

Primero se busca la clave en su lista:

- Si existe, se reemplaza el valor y `count` no cambia.
- Si no existe, se crea un nodo y se enlaza al comienzo; `count` aumenta.

Insertar al comienzo evita recorrer otra vez para localizar el final:

```cpp
Node* q = new (std::nothrow) Node{key, value, bucket[i]};
bucket[i] = q;
```

### Eliminar

Hay que localizar el enlace que apunta al nodo:

- Puede ser `bucket[i]` si es la cabeza.
- Puede ser `previous->next` si está dentro de la lista.

Luego se reconecta la lista, se libera el nodo y se decrementa el tamaño. La implementación actual del laboratorio no expone esta operación, pero esta es la estrategia natural.

### Ventajas Y Desventajas

| Ventajas | Desventajas |
|---|---|
| La cantidad de claves puede superar `m`. | Cada nodo necesita un puntero adicional. |
| Eliminar es natural al modificar enlaces. | Hay asignaciones dinámicas separadas. |
| Una colisión no obliga a buscar una casilla libre del arreglo. | Los nodos pueden estar dispersos en memoria. |

## 7. Direccionamiento Abierto

En direccionamiento abierto **no hay listas ni nodos externos**. Cada asociación ocupa directamente una casilla del arreglo:

```cpp
struct Slot {
  unsigned key;
  int value;
  bool occupied;
};
```

Cuando la casilla inicial está ocupada por otra clave, se sigue una **secuencia de sondeo**. Todas las claves permanecen dentro de `slot`.

Consecuencias:

- Como máximo se almacenan `m` asociaciones.
- Para buscar correctamente hay que reproducir la misma secuencia usada al insertar.
- La ocupación alta hace que los sondeos sean largos.
- No se necesita una asignación dinámica por asociación.

## 8. Qué Es Exactamente El Sondeo Lineal

El sondeo lineal prueba posiciones consecutivas y vuelve al inicio al alcanzar el final:

```text
probe(k, step) = (h(k) + step) mod m
               = (k mod m + step) mod m
```

En el código:

```cpp
static std::size_t probe(unsigned key, std::size_t step) {
  return (key % M + step) % M;
}
```

Para `M = 7` y `key = 17`, cuya posición inicial es `3`:

| `step` | Índice probado |
|---:|---:|
| 0 | `(3 + 0) % 7 = 3` |
| 1 | `(3 + 1) % 7 = 4` |
| 2 | `(3 + 2) % 7 = 5` |
| 3 | `(3 + 3) % 7 = 6` |
| 4 | `(3 + 4) % 7 = 0` |
| 5 | `(3 + 5) % 7 = 1` |
| 6 | `(3 + 6) % 7 = 2` |

En `M` pasos se han visitado todas las posiciones exactamente una vez. Por eso los bucles deben probar **como máximo `M` casillas**: continuar más repetiría la secuencia para siempre.

### Ejemplo De Inserción

Insertamos `10`, `17`, `24`, `8`, con `M = 7`:

1. `10 % 7 = 3`: la posición 3 está libre; guardar `10`.
2. `17 % 7 = 3`: colisiona con `10`; probar 4 y guardar `17`.
3. `24 % 7 = 3`: colisiona en 3 y 4; guardar en 5.
4. `8 % 7 = 1`: guardar directamente en 1.

```text
índice:   0    1    2    3     4     5    6
clave:    -    8    -    10    17    24   -
```

Aunque `17` está en la posición 4, buscarlo debe comenzar en su índice hash `3` y avanzar. **La función hash indica el inicio de la búsqueda, no necesariamente la posición final.**

### Búsqueda Exitosa

Para buscar `24`:

```text
probe(24, 0) = 3 -> contiene 10, continuar
probe(24, 1) = 4 -> contiene 17, continuar
probe(24, 2) = 5 -> contiene 24, éxito
```

### Búsqueda Ausente

Para buscar `31`, `31 % 7 = 3`:

```text
3 -> 10
4 -> 17
5 -> 24
6 -> vacío, detenerse: 31 no está
```

En la implementación actual, donde nunca se elimina una clave individual, encontrar una casilla nunca ocupada permite terminar. Si la clave hubiera sido insertada, no habría podido saltar esa casilla vacía.

### Vuelta Circular

Si `13 % 7 = 6` y la posición 6 está ocupada, el siguiente índice es:

```text
(6 + 1) % 7 = 0
```

El módulo hace que la búsqueda continúe desde el comienzo del arreglo.

## 9. Algoritmos De `get` Y `put` Lineales

### `get`

La lógica correcta es:

```cpp
for (std::size_t step = 0; step < M; ++step) {
  const Slot& current = slot[probe(key, step)];

  if (!current.occupied)
    return false;
  if (current.key == key) {
    value = current.value;
    return true;
  }
}
return false;
```

Aspectos esenciales:

- Hay que comprobar `occupied` antes de interpretar la clave y el valor.
- Solo se escribe `value` al encontrar exactamente `key`.
- El bucle se limita a `M` posiciones.
- Una casilla vacía termina la búsqueda porque no hay eliminación individual.

### `put`

La lógica debe distinguir actualización, inserción y tabla llena:

```cpp
for (std::size_t step = 0; step < M; ++step) {
  Slot& current = slot[probe(key, step)];

  if (current.occupied && current.key == key) {
    current.value = value;
    return true;
  }
  if (!current.occupied) {
    current.key = key;
    current.value = value;
    current.occupied = true;
    ++count;
    return true;
  }
}
return false; // Se probaron M casillas: tabla llena y clave ausente.
```

Como no existen marcas de borrado, la primera casilla vacía es segura para insertar. Actualizar no modifica `count`; ocupar una casilla nueva sí.

> [!warning] Tabla llena
> No basta comprobar `count == M` y devolver inmediatamente `false`: la clave podría estar presente y una actualización todavía debe funcionar. Se puede buscar primero o recorrer la secuencia completa y actualizar si aparece.

## 10. El Problema De Eliminar En Sondeo Lineal

Supongamos:

```text
índice: 3    4    5    6
clave:  10   17   24   vacío
```

Si eliminamos `17` marcando simplemente la posición 4 como vacía:

```text
índice: 3    4      5    6
clave:  10   vacío  24   vacío
```

Una búsqueda de `24` comienza en 3, pasa por `10`, encuentra la posición 4 vacía y concluye incorrectamente que `24` no existe.

Por eso, si se implementa eliminación individual, cada casilla suele necesitar tres estados:

```cpp
enum class State { Empty, Occupied, Deleted };
```

- `Empty`: nunca estuvo ocupada; la búsqueda puede terminar.
- `Occupied`: contiene una asociación vigente.
- `Deleted`: hubo una asociación; la búsqueda debe continuar, pero una inserción puede reutilizarla.

La marca `Deleted` también se llama **tumba** o *tombstone*.

Otra estrategia es borrar y reinsertar correctamente el grupo siguiente, pero es más delicada. El laboratorio evita el problema porque no implementa eliminación individual; `clear()` vacía toda la tabla de una vez.

## 11. Agrupamiento Primario

El sondeo lineal tiende a crear bloques contiguos de casillas ocupadas:

```text
... vacío | 10 | 17 | 24 | 31 | vacío ...
```

Una clave cuyo hash cae en cualquier posición del bloque recorre parte de él y probablemente se agrega al final. El bloque crece y atrae más colisiones. Esto se llama **agrupamiento primario** (*primary clustering*).

Ventajas del sondeo lineal:

- Es muy sencillo.
- Recorre memoria contigua, lo que suele favorecer la caché.
- Con una tabla poco ocupada puede ser muy rápido.

Desventajas:

- El agrupamiento aumenta la longitud de los sondeos.
- La tabla nunca puede superar `m` claves.
- El rendimiento cae cuando el factor de carga se acerca a 1.
- Borrar requiere tumbas o reorganización.

## 12. Factor De Carga

El factor de carga es:

```text
alpha = n / m
```

### En Encadenamiento

`alpha` es la longitud **promedio** de las listas, contando las listas vacías. Puede ser mayor que 1 porque cada bucket admite muchos nodos.

No garantiza que todas las listas tengan esa longitud. Con `n = 6`, `m = 7`, tanto una distribución `1,1,1,1,1,1,0` como `0,0,0,6,0,0,0` tienen `alpha = 6/7`, pero la segunda tiene una lista larga.

### En Direccionamiento Abierto

Como cada casilla almacena como máximo una asociación:

```text
0 <= alpha <= 1
```

Con `alpha = 1`, la tabla está llena. Mucho antes de llegar a 1 pueden aparecer secuencias largas por agrupamiento.

## 13. Complejidad

Suponiendo una distribución adecuada y que calcular el hash y comparar claves cuesta `O(1)`:

### Encadenamiento

```text
tiempo esperado = O(1 + alpha)
peor caso       = O(n)
```

El peor caso ocurre si todas las claves caen en la misma lista. Si mantenemos `m = Θ(n)`, entonces `alpha = Θ(1)` y el tiempo esperado es `O(1)`.

### Sondeo Lineal

Con una tabla suficientemente vacía y una distribución adecuada, buscar e insertar tienen tiempo esperado cercano a `O(1)`. En el peor caso pueden examinar las `m` casillas:

```text
peor caso = O(m)
```

Si `m` es proporcional a `n`, también se expresa como `O(n)`. A diferencia del encadenamiento, su análisis exacto depende mucho de la carga y del patrón de agrupamiento.

> [!important] Esperado no significa garantizado
> Una tabla hash puede ofrecer tiempo esperado constante y aun tener peor caso lineal. La afirmación `O(1)` depende de una buena distribución y de controlar el factor de carga.

## 14. La Matemática Del PDF, Explicada Intuitivamente

El PDF supone **hashing uniforme simple**: cada clave tiene probabilidad `1/m` de caer en cada casilla y las asignaciones de claves diferentes se modelan como independientes.

Para una búsqueda ausente `q`, cada clave almacenada tiene probabilidad `1/m` de colisionar con ella. Con `n` claves:

```text
colisiones esperadas = n * (1/m) = n/m = alpha
```

Acceder a la casilla cuesta una constante y recorrer sus colisiones cuesta proporcionalmente a su cantidad:

```text
E[tiempo] = Θ(1 + alpha)
```

No hace falta memorizar toda la suma para comprender el resultado: **si las claves se reparten uniformemente, cada bucket recibe en promedio `n/m` claves**.

### Hashing Universal

Una función como `k % m` puede ser mala para ciertos patrones. El hashing universal elige una función una sola vez de una familia, por ejemplo:

```text
h_a,b(k) = ((a*k + b) mod p) mod m
```

con restricciones sobre `a`, `b` y el primo `p`. La elección aleatoria dificulta que un conjunto fijo de claves fuerce siempre las mismas colisiones.

Universalidad **no significa cero colisiones**. Significa que, para dos claves distintas fijadas, la probabilidad de que la función elegida las haga colisionar está acotada por aproximadamente `1/m`.

Para el código actual basta dominar primero `key % M`; la familia universal explica cómo obtener mejores garantías ante patrones de entrada.

## 15. Invariantes Que Debes Poder Comprobar

### Encadenamiento

1. Cada nodo de clave `k` está en `bucket[k % M]`.
2. Cada clave aparece como máximo una vez.
3. `count` coincide con el total de nodos.
4. Cada lista termina en `nullptr` y no tiene ciclos.
5. Dos listas distintas no comparten nodos.

### Sondeo Lineal Sin Borrado Individual

1. `count` coincide con la cantidad de casillas `occupied`.
2. Cada clave aparece como máximo una vez.
3. `count <= M`.
4. Si una clave está en la tabla, se encuentra siguiendo `probe(key, step)` antes de la primera casilla vacía.
5. `get` no modifica el parámetro de salida cuando falla.
6. Actualizar una clave no cambia `count`.
7. Una inserción fallida por tabla llena no cambia ninguna casilla.

## 16. Comparación Directa

| Aspecto | Encadenamiento | Sondeo lineal |
|---|---|---|
| Lugar de los datos | Nodos en listas | Casillas del arreglo |
| Resolución de colisión | Recorrer lista del bucket | Probar casillas consecutivas |
| Capacidad fija `m` | Puede almacenar más de `m` claves | Máximo `m` claves |
| Memoria extra | Un `next` por nodo | Estado por casilla |
| Localidad de memoria | Menor | Buena, arreglo contiguo |
| Eliminación | Natural con enlaces | Necesita tumbas o reorganización |
| Problema típico | Una lista demasiado larga | Agrupamiento primario |
| Factor de carga | Puede ser mayor que 1 | Como máximo 1 |

## 17. Análisis De `Hashing/hashingColiciones`

Esta sección explica los archivos del laboratorio tal como están ahora.

### `dictionary.h`: encadenamiento

```cpp
class Dictionary {
  struct Node {
    unsigned key;
    int value;
    Node* next;
  };
  static constexpr std::size_t M = 7;
  std::array<Node*, M> bucket{};
  std::size_t count = 0;
  ...
};
```

Detalles importantes:

- `bucket{}` inicializa todos los punteros en `nullptr`.
- `M` es compartido por todos los objetos y conocido en compilación.
- `count` registra asociaciones, no buckets ocupados.
- Se prohíbe copiar el objeto porque una copia superficial compartiría nodos y ambos destructores intentarían liberarlos.
- El destructor llama a `clear()`.

### `dictionary.cc`: `get`

```cpp
const Node* p = bucket[key % M];
while (p != nullptr && p->key != key)
  p = p->next;
if (p == nullptr)
  return false;
value = p->value;
return true;
```

Calcula el bucket, recorre solo esa lista y escribe el valor únicamente al encontrar la clave. Por ello una consulta fallida conserva el parámetro de salida.

### `dictionary.cc`: `put`

1. Calcula `i = key % M`.
2. Busca la clave en `bucket[i]`.
3. Si existe, actualiza `p->value` y devuelve `true`; `count` no cambia.
4. Si no existe, reserva `Node{key, value, bucket[i]}`.
5. El nuevo nodo pasa a ser la cabeza y `count` aumenta.
6. Si `new (std::nothrow)` falla, devuelve `false` sin modificar la lista.

### `dictionary.cc`: `clear` Y Destructor

El bucle externo visita cada cabeza **por referencia**:

```cpp
for (Node*& head : bucket)
```

El bucle interno adelanta la cabeza antes de borrar el nodo actual. Al final todas las cabezas quedan `nullptr` y `count` vuelve a cero. El destructor reutiliza esa misma función, evitando fugas.

### `linear_dictionary.h`

Define siete casillas contiguas:

```cpp
struct Slot {
  unsigned key = 0;
  int value = 0;
  bool occupied = false;
};
```

No se puede usar `key == 0` para decidir si está libre porque la clave cero es válida. `occupied` separa el estado de ocupación del contenido de la clave.

La función `probe` expresa exactamente el sondeo lineal y la vuelta circular:

```cpp
(key % M + step) % M
```

### `linear_dictionary.cc`

Actualmente es un **esqueleto intencional**:

- `get` solo revisa `slot[key % M]`, no comprueba `occupied`, no compara la clave y no sondea colisiones. Debe reemplazarse por el algoritmo de la sección 9.
- `put` devuelve siempre `false`; debe implementar actualización, inserción, límite de `M` sondeos y tabla llena.
- `clear` sí marca todas las casillas libres y reinicia `count`. No necesita liberar memoria porque los `Slot` son miembros del arreglo, no objetos creados con `new`.

### `common_tests.h`

Es una plantilla para ejecutar los mismos contratos sobre `Dictionary` y `LinearDictionary`:

- El diccionario comienza vacío.
- Una consulta ausente devuelve `false` y conserva `value`.
- La primera inserción aumenta el tamaño.
- Una actualización conserva el tamaño y cambia el valor.
- `clear` deja tamaño cero.

Todavía tiene un `TODO` para colisiones, clave/valor cero, ausencia y reutilización.

### `test_chain.cc`

Ejecuta las pruebas comunes sobre `Dictionary`. Luego crea un ámbito con claves `10` y `17`, que colisionan, y deja que el destructor libere los nodos cuando el objeto sale del bloque.

### `test_linear.cc`

Ejecuta las mismas pruebas sobre `LinearDictionary`. `capacity_tests()` está pendiente y debería comprobar tabla llena, actualización con tabla llena, vuelta circular y que una inserción fallida no cambie el estado.

### `Makefile`

```bash
make             # compila test_chain y test_linear
make test-chain  # ejecuta encadenamiento, actualmente completo
make test-linear # falla porque get/put lineales son TODO
make clean       # elimina ambos ejecutables
```

El laboratorio usa C++17 y advertencias `-Wall -Wextra -Wpedantic`.

## 18. Estado De `Hashing/me`

Tu implementación personal reproduce la misma estructura y es adecuada para reconstruirla desde cero.

Aspectos a corregir o terminar:

1. En `me/dictionary.cc`, `get` contiene:

   ```cpp
   value = p->key;
   ```

   Debe copiar `p->value`; la clave y el valor no son lo mismo.

2. En `me/dictionary.cc`, actualizar una clave devuelve `false`, mientras el contrato de `common_tests` espera `true` para una actualización completada. Debes decidir y documentar un contrato consistente; si quieres ser compatible con el laboratorio, devuelve `true`.

3. `me/linear_dictionary.cc` todavía está vacío. Implementa primero `get`, después `put`, usando como máximo `M` sondeos.

4. El guard del encabezado `dictionary.h` se llama `DICTIONARY_HPP`. Funciona, pero `DICTIONARY_H` o una extensión `.hpp` serían nombres más consistentes.

> [!tip] Orden recomendado para programar desde cero
> Escribe la representación y `probe`; implementa `get`; prueba clave inicial, colisión y ausencia; implementa `put`; prueba actualización y tabla llena; por último `clear` y pruebas de reutilización.

## 19. Preguntas Típicas De Examen

### ¿Una colisión es un error?

No. Es una consecuencia normal de comprimir un universo grande en `m` índices. El error sería no tener una estrategia correcta para resolverla.

### ¿Por qué buscar no siempre es `O(1)`?

El hash calcula una zona inicial en `O(1)`, pero todavía puede ser necesario recorrer una lista o una secuencia de sondeo. El tiempo constante es esperado bajo supuestos de distribución y carga; el peor caso es lineal.

### ¿Qué diferencia hay entre clave e índice hash?

La clave identifica una asociación; el índice solo indica dónde comenzar a buscar. Claves distintas pueden tener el mismo índice.

### ¿Por qué una actualización no aumenta `size()`?

Porque sigue existiendo una sola asociación para esa clave; únicamente cambia su valor vigente.

### ¿Por qué sondear como máximo `M` veces?

La secuencia lineal con módulo visita las `M` posiciones. El paso siguiente repetiría la posición inicial y podría crear un bucle infinito.

### ¿Por qué `get` lineal puede detenerse en una casilla vacía?

Sin eliminaciones individuales, ninguna inserción puede saltar una casilla vacía. Si la clave estuviera almacenada, aparecería antes de ese hueco en su secuencia.

### ¿Por qué no basta `occupied = false` para eliminar?

Porque rompería la cadena de sondeo de otras claves desplazadas por colisiones. Se necesita un estado `Deleted` o reconstruir el grupo.

### ¿Qué significa `alpha = n/m`?

En encadenamiento, longitud promedio de listas; en direccionamiento abierto, fracción de casillas ocupadas. No describe la distribución completa.

## 20. Resumen Para Repaso

```text
Diccionario: una clave -> un valor vigente

Hash:
  h(k) calcula un índice inicial entre 0 y m-1
  colisión: claves distintas con el mismo índice

Encadenamiento:
  bucket[i] es una lista
  colisión -> recorrer/agregar en esa lista
  alpha puede ser > 1
  esperado O(1 + alpha), peor O(n)

Sondeo lineal:
  todas las claves viven en el arreglo
  probe(k,s) = (k % M + s) % M
  colisión -> probar la siguiente casilla
  como máximo M sondeos
  alpha <= 1
  agrupamiento primario
  borrar necesita tumbas

Contratos esenciales:
  get fallido no cambia la salida
  put existente actualiza sin aumentar count
  put nuevo aumenta count
  clear permite reutilizar el objeto
```

> [!summary] Idea central
> Hashing no conoce mágicamente la posición exacta. Calcula una posición inicial y utiliza una estrategia de colisiones. Encadenamiento continúa dentro de una lista; sondeo lineal continúa dentro del arreglo.
