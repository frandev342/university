---
title: "Árboles rojinegros"
aliases:
  - "Árbol rojo-negro"
  - "Red-black trees"
  - "RBT"
tags:
  - estructuras-de-datos
  - arboles
  - bst
  - balance
  - arboles-rojinegros
  - cpp11
status: desarrollado
---

# Árboles rojinegros

> [!abstract] Objetivo
> Comprender por qué un árbol rojinegro mantiene altura `O(log n)`, cómo las rotaciones y recoloraciones restauran sus invariantes después de insertar o borrar, y cómo traducir los algoritmos de Cormen a una implementación segura en C++11.

## Material Relacionado

- `docs/tree-black-trees-Cormen.pdf`: referencia principal para propiedades, demostraciones y algoritmos.
- `docs/rojinegros.pdf`: material complementario para la descripción del balance y las reparaciones.
- `docs/rojinegros2.pdf`: material complementario para ejemplos y terminología.
- `notes/arboles-binarios-de-busqueda.md`: operaciones y propiedad de orden de un BST.
- `binaryTree/AVL.md`: comparación con otra familia de árboles balanceados.
- `red-black-trees/cpp/`: implementación C++11 con búsqueda y reparaciones iterativas.
- `red-black-trees/cpp_recursive/`: misma interfaz C++11 con descenso y reparaciones recursivos.
- [[arboles-rojinegros-c]]: misma estructura temática y guion adaptados a la implementación en C11.

> [!note] Criterio de integración
> Se conserva la formulación de cinco propiedades y el centinela `T.nil` usados por Cormen. Algunos textos agrupan o numeran las propiedades de otra manera, pero describen el mismo invariante estructural.

## 1. El Problema De Un BST Ordinario

Un árbol binario de búsqueda mantiene, para cada nodo `x`:

```text
claves del subárbol izquierdo < x.key
x.key < claves del subárbol derecho
```

Si las claves son únicas, buscar, insertar y borrar cuestan:

```text
O(h)
```

donde `h` es la altura del árbol.

La propiedad BST no controla la forma. Insertar claves ordenadas puede producir una cadena:

```text
10
  \
   20
     \
      30
        \
         40
```

En esa forma:

```text
h = n - 1
buscar = insertar = borrar = O(n)
```

Un árbol rojinegro añade información de color e invariantes globales que impiden que, desde un mismo nodo, un camino descendente hasta `NIL` sea más del doble de largo que otro. No busca que ambos subárboles tengan exactamente la misma altura; busca una condición más flexible y barata de mantener.

## 2. Definición

Un árbol rojinegro es un BST en el que cada nodo posee un color:

```text
RED o BLACK
```

Además del orden BST, satisface cinco propiedades.

### Las Cinco Propiedades

1. Cada nodo es rojo o negro.
2. La raíz es negra.
3. Todas las hojas `NIL` son negras.
4. Si un nodo es rojo, sus dos hijos son negros.
5. Para cada nodo, todos los caminos simples desde ese nodo hasta hojas descendientes `NIL` contienen la misma cantidad de nodos negros.

La cuarta propiedad también puede expresarse así:

```text
no existen dos nodos rojos consecutivos en un camino
```

La quinta propiedad no exige que todos los caminos tengan igual longitud. Los nodos rojos pueden hacer que un camino sea más largo, pero su distribución está limitada por la cuarta propiedad.

> [!important] Las hojas `NIL` sí cuentan
> Una hoja `NIL` no almacena una clave, pero es un nodo externo conceptual y su color es negro. Dibujos introductorios suelen omitirlas para no saturar el árbol.

### Ejemplo Válido

```text
             38B
           /     \
        19R       41B
       /   \
    12B    31B
    /
   8R
```

Si se muestran las hojas externas, cada enlace ausente termina en un `NIL` negro.

No hay pares rojo-rojo. Desde la raíz, todos los caminos hacia `NIL` contienen la misma cantidad de nodos negros.

### Ejemplos Inválidos

Raíz roja:

```text
10R
```

Dos rojos consecutivos:

```text
  20B
  /
10R
/
5R
```

Alturas negras distintas:

```text
    20B
   /   \
 10B   30R
 /
5B
```

El camino izquierdo contiene más nodos negros que el derecho.

## 3. El Centinela `NIL`

Cormen representa todos los hijos ausentes mediante un único nodo centinela negro:

```text
T.nil.color = BLACK
```

En lugar de:

```cpp
node->left == nullptr
```

la implementación comprueba:

```cpp
node->left == &nil_
```

El árbol vacío se representa con:

```cpp
root_ == &nil_
```

### Ventajas

- Todo hijo tiene un color consultable.
- Los algoritmos no repiten comprobaciones contra `nullptr`.
- La reparación del borrado puede desplazarse por un hijo ausente.
- El caso del árbol vacío se integra en la misma representación.

### Precaución Con El Padre

Durante `transplant`, incluso el centinela recibe temporalmente un padre:

```cpp
new_root->parent = old_root->parent;
```

Esto permite que `erase_fixup` conozca el padre del hueco que está reparando. Al terminar el borrado, el proyecto restaura:

```cpp
nil_.parent = &nil_;
```

> [!warning] Un solo centinela compartido
> `nil_` no se elimina junto con los nodos dinámicos. Es un miembro de `RedBlackTree` y todos los enlaces ausentes apuntan al mismo objeto.

## 4. Altura Negra

La altura negra de un nodo `x`, escrita `bh(x)`, es la cantidad de nodos negros en cualquier camino desde `x`, sin contar a `x`, hasta una hoja `NIL` descendiente.

La cantidad está bien definida por la propiedad 5.

Para una hoja centinela:

```text
bh(NIL) = 0
```

Para un nodo interno cuyos hijos son `NIL`, la altura negra es `1`, porque el camino descendente termina en el `NIL` negro.

### Lema De Tamaño

El subárbol con raíz en `x` contiene al menos:

```text
2^bh(x) - 1 nodos internos
```

#### Demostración Por Inducción

Caso base:

```text
x = NIL
bh(x) = 0
2^0 - 1 = 0
```

El centinela no es un nodo interno, por lo que la cota se cumple.

Paso inductivo:

- Cada hijo tiene altura negra `bh(x)` si el hijo es rojo.
- Cada hijo tiene altura negra `bh(x) - 1` si el hijo es negro.
- En ambos casos, la altura negra de cada hijo es al menos `bh(x) - 1`.

Por hipótesis inductiva, cada subárbol hijo contiene al menos:

```text
2^(bh(x) - 1) - 1
```

Sumando ambos hijos y el nodo `x`:

```text
1 + 2(2^(bh(x) - 1) - 1)
= 2^bh(x) - 1
```

### Cota De Altura

Sea `h` la altura del árbol.

Como un nodo rojo no puede tener un hijo rojo, al menos la mitad de los nodos de cualquier camino raíz-hoja son negros. Por tanto:

```text
bh(root) >= h / 2
```

Por el lema de tamaño:

```text
n >= 2^bh(root) - 1
n >= 2^(h/2) - 1
n + 1 >= 2^(h/2)
log2(n + 1) >= h / 2
h <= 2 log2(n + 1)
```

> [!success] Consecuencia principal
> La altura es `O(log n)`. Buscar, insertar y borrar recorren como máximo una cantidad logarítmica de niveles.

## 5. Rotaciones

Una rotación cambia enlaces locales sin cambiar el recorrido in-order. Por eso conserva la propiedad BST.

### Rotación Izquierda

Antes:

```text
      x
     / \
    A   y
       / \
      B   C
```

Después de `rotate_left(x)`:

```text
        y
       / \
      x   C
     / \
    A   B
```

El orden se conserva:

```text
A < x < B < y < C
```

Pasos estructurales:

```cpp
Node* pivot = node->right;
node->right = pivot->left;

if (pivot->left != &nil_)
  pivot->left->parent = node;

pivot->parent = node->parent;
// El padre anterior pasa a apuntar a pivot.

pivot->left = node;
node->parent = pivot;
```

### Rotación Derecha

Es la operación simétrica:

```text
        y                 x
       / \               / \
      x   C     ->       A   y
     / \                   / \
    A   B                 B   C
```

### Qué Modifica Y Qué Conserva

Una rotación:

- Modifica una cantidad constante de punteros.
- Cuesta `O(1)`.
- Conserva el orden in-order.
- Puede cambiar la raíz de todo el árbol.
- No garantiza por sí sola las propiedades rojinegras; se combina con recoloraciones.

> [!failure] Error frecuente
> Actualizar los hijos pero olvidar `parent`, o no actualizar `root_` cuando gira la raíz. `valid()` detecta ambos errores.

## 6. Inserción

La inserción tiene dos etapas:

1. Insertar la clave como en un BST.
2. Reparar las propiedades rojinegras.

### Por Qué El Nodo Nuevo Es Rojo

Un nodo nuevo comienza rojo:

```cpp
Node* inserted = new Node(key, Color::Red);
```

Si comenzara negro, todos los caminos que pasen por él ganarían un nodo negro y podría romperse inmediatamente la propiedad 5.

Al insertarlo rojo, la altura negra no cambia. Las únicas infracciones posibles son:

- El nodo nuevo es la raíz y quedó rojo.
- Su padre también es rojo.

La reparación final siempre pinta la raíz de negro.

### Inserción BST Del Proyecto

El proyecto modela un conjunto de enteros, por lo que las claves son únicas:

```cpp
while (current != &nil_) {
  parent = current;
  if (key == current->key)
    return false;
  current = key < current->key ? current->left : current->right;
}
```

Esta decisión permite validar un orden estricto:

```text
subárbol izquierdo < nodo < subárbol derecho
```

### Estado Del Bucle De Reparación

El bucle continúa mientras el padre sea rojo:

```cpp
while (node->parent->color == Color::Red) {
  // reparar
}
```

Si el padre es rojo, el abuelo existe y es negro antes de reparar, porque un árbol válido previo a la inserción no contenía dos rojos consecutivos.

Los casos se describen suponiendo que el padre está a la izquierda del abuelo. Si está a la derecha, se intercambian `left` y `right`.

### Caso 1: Tío Rojo

```text
         G(B)                 G(R)
        /   \                /   \
     P(R)   U(R)     ->    P(B)  U(B)
     /                     /
   Z(R)                  Z(R)
```

Operación:

```cpp
parent.color = BLACK;
uncle.color = BLACK;
grandparent.color = RED;
node = grandparent;
```

No se rota. La cantidad de nodos negros vista desde el padre del subárbol se conserva, pero el abuelo puede entrar en conflicto con su propio padre. Por eso el análisis continúa desde el abuelo.

### Caso 2: Tío Negro Y Triángulo

```text
        G(B)
       /
     P(R)
       \
       Z(R)
```

Se rota sobre el padre:

```cpp
node = node->parent;
rotate_left(node);
```

El triángulo se convierte en la línea del caso 3:

```text
        G(B)
       /
     Z(R)
     /
   P(R)
```

### Caso 3: Tío Negro Y Línea

```text
          G(B)              P(B)
         /                 /   \
       P(R)       ->      Z(R)  G(R)
       /
     Z(R)
```

Operación:

```cpp
node->parent->color = Color::Black;
node->parent->parent->color = Color::Red;
rotate_right(node->parent->parent);
```

La rotación y el intercambio de colores eliminan el par rojo-rojo sin cambiar la altura negra.

### Casos Espejo

Si el padre es hijo derecho del abuelo:

| Caso original | Caso espejo |
|---|---|
| Tío derecho | Tío izquierdo |
| Triángulo izquierda-derecha | Triángulo derecha-izquierda |
| `rotate_left(parent)` | `rotate_right(parent)` |
| `rotate_right(grandparent)` | `rotate_left(grandparent)` |

> [!tip] Cómo estudiar los casos
> Memorizar primero una sola orientación. La otra mitad del código es exactamente el espejo: izquierda por derecha y rotación izquierda por rotación derecha.

### Límite De Trabajo

- El caso 1 puede repetirse hacia la raíz.
- Los casos 2 y 3 terminan la reparación local.
- Se realizan como máximo dos rotaciones.
- El recorrido ascendente cuesta `O(log n)`.

## 7. Ejemplo De Inserción De Cormen

Secuencia:

```text
41, 38, 31, 12, 19, 8
```

### Insertar `41`

Se crea rojo y después se pinta la raíz de negro:

```text
41B
```

### Insertar `38`

Su padre es negro; no hay conflicto:

```text
  41B
 /
38R
```

### Insertar `31`

Se forma una línea izquierda-izquierda. Se recolorea y se rota a la derecha en `41`:

```text
    38B
   /   \
 31R   41R
```

### Insertar `12`

El padre `31` y el tío `41` son rojos. Se aplica el caso 1:

```text
    38B
   /   \
 31B   41B
 /
12R
```

`38` se recolorea temporalmente rojo, pero al ser raíz termina negro.

### Insertar `19`

Se forma el triángulo `31-12-19`:

1. Rotación izquierda en `12`.
2. `19` pasa a negro y `31` a rojo.
3. Rotación derecha en `31`.

```text
      38B
     /   \
   19B   41B
  /  \
12R  31R
```

### Insertar `8`

El padre `12` y el tío `31` son rojos. Se recolorea:

```text
       38B
      /   \
    19R   41B
   /  \
 12B  31B
 /
8R
```

Este es el árbol que imprime el proyecto. La impresión está girada 90 grados: el subárbol derecho aparece arriba.

```text
    B:41
B:38
        B:31
    R:19
        B:12
            R:8
```

## 8. Eliminación

El borrado conserva primero el orden BST y después restaura los colores.

### Etapa BST

Sea `target` el nodo que contiene la clave.

- Sin hijo izquierdo: se reemplaza por su hijo derecho.
- Sin hijo derecho: se reemplaza por su hijo izquierdo.
- Con dos hijos: se mueve a su posición el mínimo del subárbol derecho, es decir, su sucesor.

La función auxiliar `transplant` reemplaza una raíz de subárbol:

```cpp
void RedBlackTree::transplant(Node* old_root, Node* new_root);
```

### El Color Que Importa

El algoritmo conserva el color original del nodo que sale físicamente de su posición:

```cpp
Color removed_color = moved->color;
```

Si era rojo, quitarlo no cambia la cantidad de negros en ningún camino y no puede crear un nuevo par rojo-rojo.

Si era negro, los caminos que pasaban por esa posición pierden una unidad de altura negra. Entonces se ejecuta `erase_fixup` sobre el reemplazo.

### El Concepto De Negro Extra

El negro extra no es un tercer color almacenado en el nodo. Es una herramienta de razonamiento:

```text
el nodo replacement representa una posición a la que le falta compensar
un nodo negro eliminado
```

La reparación desplaza o elimina ese déficit sin romper el orden BST.

Los casos siguientes suponen que `node` es hijo izquierdo. La otra orientación es simétrica.

### Caso 1: Hermano Rojo

```text
        P(B/R)                    P(R)
       /     \                   /   \
   X(extra)  W(R)      ->       Wl(B) W(B)
             /  \
           Wl(B) Wr(B)
```

Operación:

```cpp
sibling->color = Color::Black;
node->parent->color = Color::Red;
rotate_left(node->parent);
sibling = node->parent->right;
```

Este caso transforma un hermano rojo en un hermano negro. No finaliza por sí solo; conduce a uno de los otros casos.

### Caso 2: Hermano Negro Con Dos Hijos Negros

```text
        P(?)
       /   \
      X    W(B)
          /   \
        B       B
```

Se pinta el hermano de rojo y el negro extra sube al padre:

```cpp
sibling->color = Color::Red;
node = node->parent;
```

Este es el único caso de borrado que puede repetirse hacia la raíz.

### Caso 3: Hermano Negro, Hijo Cercano Rojo Y Lejano Negro

Para un `node` izquierdo, el hijo cercano del hermano es el izquierdo y el lejano es el derecho:

```text
      W(B)
     /   \
   N(R)  F(B)
```

Se recolorea y se rota el hermano hacia la derecha:

```cpp
sibling->left->color = Color::Black;
sibling->color = Color::Red;
rotate_right(sibling);
sibling = node->parent->right;
```

El resultado queda preparado para el caso 4.

### Caso 4: Hermano Negro Con Hijo Lejano Rojo

```cpp
sibling->color = node->parent->color;
node->parent->color = Color::Black;
sibling->right->color = Color::Black;
rotate_left(node->parent);
node = root_;
```

El nuevo subárbol conserva la altura negra previa y el déficit desaparece. Asignar `node = root_` termina el bucle.

### Resumen De Los Cuatro Casos

| Caso | Hermano | Hijos del hermano | Acción principal |
|---|---|---|---|
| 1 | Rojo | Negros | Recolorear y girar el padre. |
| 2 | Negro | Ambos negros | Pintar hermano rojo y subir. |
| 3 | Negro | Cercano rojo, lejano negro | Girar el hermano. |
| 4 | Negro | Lejano rojo | Recolorear, girar el padre y terminar. |

La reparación de borrado realiza como máximo tres rotaciones y tarda `O(log n)`.

## 9. Diseño Del Proyecto C++11

`cpp/` corresponde al algoritmo con búsqueda, inserción y reparaciones iterativas mostrado en esta nota. `cpp_recursive/` ofrece la misma interfaz con `search`, descenso de inserción y mínimo recursivos; el caso de tío rojo vuelve a llamar a `insert_fixup` sobre el abuelo y el caso 2 de borrado a `erase_fixup` sobre el padre. Ambas variantes mantienen los mismos invariantes, rotaciones y complejidad, y pueden probarse por separado con `make test-cpp` y `make test-cpp-recursive` desde `red-black-trees/`.

La clase pública es:

```cpp
class RedBlackTree {
public:
  RedBlackTree();
  ~RedBlackTree();

  RedBlackTree(const RedBlackTree&) = delete;
  RedBlackTree& operator=(const RedBlackTree&) = delete;

  bool insert(int key);
  bool erase(int key);
  bool contains(int key) const;

  std::size_t size() const;
  bool empty() const;
  int height() const;
  std::vector<int> inorder() const;

  bool valid() const;
  void print(std::ostream& out) const;
};
```

### Nodo Interno

```cpp
enum class Color { Red, Black };

struct Node {
  int key;
  Color color;
  Node* left;
  Node* right;
  Node* parent;
};
```

`enum class` evita convertir accidentalmente el color a entero y mantiene los nombres dentro del ámbito `Color`:

```cpp
Color::Red
Color::Black
```

### Propiedad De Recursos

Cada nodo real se reserva con `new` y pertenece a un único árbol. El destructor libera recursivamente todos los nodos excepto el centinela.

Copiar superficialmente el objeto duplicaría punteros y provocaría dobles liberaciones. Por eso copia y asignación están eliminadas explícitamente.

> [!note] Alcance didáctico
> Podría implementarse copia profunda o movimiento, pero no son necesarios para estudiar los algoritmos rojinegros. Omitirlos mantiene el proyecto pequeño y enfocado.

### Contratos Públicos

| Operación | Resultado |
|---|---|
| `insert(key)` | `true` si insertó; `false` si la clave ya existía. |
| `erase(key)` | `true` si borró; `false` si la clave no existía. |
| `contains(key)` | Indica si la clave pertenece al conjunto. |
| `height()` | Devuelve `-1` para el árbol vacío y `0` para una hoja real. |
| `inorder()` | Devuelve las claves en orden creciente. |
| `valid()` | Comprueba todos los invariantes sin modificar el árbol. |

## 10. Validación De Invariantes

`valid()` no forma parte del algoritmo de balance. Es una función de diagnóstico y pruebas que recorre todo el árbol.

Comprueba:

1. El centinela es negro y sus hijos apuntan a sí mismo.
2. La raíz existe, es negra y su padre es el centinela.
3. El orden BST es estricto en todo el subárbol.
4. Los enlaces `parent` coinciden con la estructura descendente.
5. Ningún nodo rojo tiene un hijo rojo.
6. Ambos subárboles de cada nodo tienen igual altura negra.
7. La cantidad recorrida coincide con `size_`.

### Límites De Orden

No basta comparar un nodo con sus hijos inmediatos. La validación propaga un intervalo permitido:

```cpp
validate_node(node->left, node, lower, node->key, ...);
validate_node(node->right, node, node->key, upper, ...);
```

Se usan límites `long long` para admitir correctamente `INT_MIN` e `INT_MAX` sin restar o sumar uno, operaciones que podrían desbordarse.

### Altura Negra En El Validador

El caso base asigna uno al centinela porque esta función auxiliar cuenta el `NIL` negro en el camino que retorna:

```cpp
if (node == &nil_) {
  black_height = 1;
  return true;
}
```

Luego compara ambos caminos:

```cpp
if (left_black_height != right_black_height)
  return false;
```

y añade el nodo actual si es negro:

```cpp
black_height = left_black_height
             + (node->color == Color::Black ? 1 : 0);
```

Esta convención interna incluye el nodo actual al devolver el resultado. Es distinta de la notación matemática `bh(x)`, que excluye `x`, pero ambas permiten verificar la misma igualdad de caminos.

## 11. Complejidades

| Operación | Tiempo | Espacio auxiliar |
|---|---:|---:|
| `contains` | `O(log n)` | `O(1)` |
| `insert` | `O(log n)` | `O(1)` |
| `erase` | `O(log n)` | `O(1)` |
| Rotación | `O(1)` | `O(1)` |
| `inorder` | `O(n)` | `O(n)` para resultado y pila |
| `height` | `O(n)` | `O(log n)` de pila |
| `valid` | `O(n)` | `O(h)` de pila; `O(log n)` si es válido |
| Destrucción | `O(n)` | `O(log n)` de pila |

En un árbol rojinegro válido, la pila recursiva es `O(log n)` porque el propio invariante limita la altura. Mientras `valid()` diagnostica una estructura rota, su pila puede llegar a `O(n)` si el árbol quedó degenerado.

Cada nodo almacena tres punteros, una clave y un color. La memoria total es `O(n)`.

## 12. Comparación Con AVL

Ambos son BST auto-balanceados, pero usan invariantes diferentes.

| Aspecto | AVL | Rojinegro |
|---|---|---|
| Información de balance | Altura o factor de balance | Un color |
| Restricción | Diferencia de alturas a lo sumo 1 | Igual altura negra y sin rojos consecutivos |
| Balance | Más estricto | Más flexible |
| Altura máxima | Menor en general | `2 log2(n + 1)` |
| Búsquedas | Suelen recorrer menos niveles | También `O(log n)` |
| Actualizaciones | Puede reparar más nodos al borrar | Suele favorecer muchas inserciones y borrados |
| Uso típico | Predominio de consultas | Conjuntos y mapas con actualizaciones frecuentes |

No debe concluirse que uno es siempre mejor. La elección depende de costos constantes, patrón de operaciones e implementación disponible.

## 13. Pruebas Del Proyecto

Las pruebas usan una macro `CHECK` que permanece activa incluso si se define `NDEBUG`, y llaman `valid()` después de cada modificación.

Cobertura:

- Árbol vacío.
- Inserción clásica de Cormen.
- Rechazo de claves repetidas.
- Borrado de hoja, nodo con hijos y raíz.
- `INT_MIN` e `INT_MAX`.
- Inserción ordenada de 512 claves.
- Borrado de todas las claves pares y luego impares.
- Ocho órdenes pseudoaleatorios reproducibles de 160 claves.
- 5000 operaciones mixtas comparadas paso a paso con `std::set`.
- Validación después de cada inserción y borrado.
- Liberación completa bajo AddressSanitizer.

Comandos:

```bash
cd red-black-trees
make test
make sanitize
```

Salida esperada:

```text
Cormen insertion example (right subtree is above):
    B:41
B:38
        B:31
    R:19
        B:12
            R:8
Red-black tree: OK
```

Las opciones de compilación incluyen:

```text
-std=c++11
-Wall -Wextra -Wpedantic -Wconversion
-fsanitize=address,undefined
```

## 14. Errores Frecuentes

### Tratar `NIL` Como `nullptr`

Incorrecto para esta representación:

```cpp
if (node->left == nullptr)
```

Correcto:

```cpp
if (node->left == &nil_)
```

### Insertar El Nodo Como Negro

Puede aumentar la altura negra solamente en algunos caminos. Insertarlo rojo mantiene inicialmente la propiedad 5.

### Olvidar Los Casos Espejo

Los tres casos de inserción y cuatro de borrado deben existir para ambas orientaciones.

### Confundir Tío Y Hermano

- En inserción se analiza el **tío** del nodo nuevo.
- En borrado se analiza el **hermano** del nodo con negro extra.

### Rotar Sin Actualizar La Raíz

Si el nodo rotado no tenía padre real, el pivote debe convertirse en `root_`.

### Actualizar Solo Punteros Descendentes

Cada cambio de hijo también debe mantener el puntero `parent` correspondiente.

### Borrar El Sucesor Pero Reparar Desde El Nodo Equivocado

La reparación comienza en el reemplazo de la posición física cuyo color negro fue retirado, no necesariamente en la clave solicitada originalmente.

### Recolorear El Centinela Permanentemente

`nil_` debe terminar negro. Los invariantes y `valid()` dependen de ello.

### Afirmar Que El Árbol Es Perfectamente Balanceado

Un rojinegro no exige igual altura geométrica. Garantiza una cota logarítmica mediante la altura negra.

## 15. Guion De Exposición De 5 A 8 Minutos

Duración objetivo: aproximadamente **7 minutos**.

### 0:00-0:45 - Problema Y Motivación

> Un árbol binario de búsqueda permite buscar siguiendo un solo camino, pero su costo depende de la altura. Si inserto `10, 20, 30, 40` en ese orden, obtengo una cadena y buscar puede costar tiempo lineal. Un árbol rojinegro evita esa degeneración añadiendo un color a cada nodo y manteniendo reglas que fuerzan una altura logarítmica.

Mostrar:

```text
BST degenerado: h = n - 1
Rojinegro:      h <= 2 log2(n + 1)
```

### 0:45-1:45 - Definición Y Cinco Propiedades

> Sigue siendo un BST, así que conserva el orden de las claves. Además: cada nodo es rojo o negro, la raíz es negra, las hojas externas NIL son negras, un rojo no puede tener hijos rojos y todos los caminos desde un nodo hasta NIL tienen la misma cantidad de negros.

> Las hojas NIL no guardan datos. Son posiciones externas negras y normalmente no se dibujan. En el código uso un único centinela compartido para representarlas.

Señalar en un dibujo:

```text
       38B
      /   \
    19R   41B
   /  \
 12B  31B
 /
8R
```

Preguntar visualmente:

```text
¿Hay dos rojos seguidos? No.
¿Coincide la cantidad de negros hasta NIL? Sí.
```

### 1:45-2:35 - Por Qué Es Logarítmico

> La propiedad de caminos negros impide que una rama pierda demasiados nodos negros, y la prohibición de rojos consecutivos impide insertar una cantidad arbitraria de rojos entre ellos. Por eso el camino más largo puede ser como máximo el doble del camino negro.

> Un subárbol de altura negra `b` tiene al menos `2^b - 1` nodos. Al combinar ambas ideas resulta `h <= 2 log2(n + 1)`. Así, búsqueda, inserción y borrado cuestan `O(log n)`.

No desarrollar toda la inducción salvo que el docente la solicite; dejar la fórmula visible.

### 2:35-3:20 - Rotaciones

> Cuando una actualización rompe una propiedad, no reconstruimos todo el árbol. Usamos recoloraciones y rotaciones locales. Una rotación cambia quién es padre de quién, cuesta tiempo constante y conserva el orden in-order.

Mostrar:

```text
    x                   y
   / \                 / \
  A   y      ->        x   C
     / \              / \
    B   C            A   B
```

> A, x, B, y y C conservan exactamente el mismo orden; solo cambia la forma.

### 3:20-5:15 - Inserción Y Sus Tres Casos

> Primero se inserta como en cualquier BST, pero el nodo nuevo se pinta rojo. Si fuera negro cambiaría la altura negra de una sola rama. Al ser rojo, la única infracción posible es tener padre rojo.

Caso 1:

> Si padre y tío son rojos, ambos pasan a negro, el abuelo pasa a rojo y continuamos desde el abuelo. Es una reparación por recoloración que puede subir por el árbol.

Caso 2:

> Si el tío es negro y los nodos forman un triángulo, giramos el padre para convertirlo en una línea.

Caso 3:

> En la línea, el padre pasa a negro, el abuelo a rojo y giramos el abuelo. La infracción desaparece.

> Los mismos casos existen al otro lado como imágenes espejo. Una inserción necesita como máximo dos rotaciones.

Usar la parte más reconocible del ejemplo:

```text
41, 38, 31
```

Antes de reparar `31`:

```text
  41B
 /
38R
/
31R
```

Después:

```text
   38B
  /   \
31R   41R
```

### 5:15-6:05 - Implementación C++

> Cada nodo guarda clave, color y punteros izquierdo, derecho y padre. Todos los enlaces ausentes apuntan al centinela negro `nil_`. La clase rechaza repetidos, por lo que representa un conjunto.

Mostrar la interfaz:

```cpp
bool insert(int key);
bool erase(int key);
bool contains(int key) const;
bool valid() const;
```

> La función `valid` no balancea. Recorre el árbol para comprobar orden BST, raíz negra, ausencia de rojos consecutivos, alturas negras, padres y tamaño. Las pruebas la ejecutan después de cada operación.

### 6:05-6:40 - Borrado

> El borrado primero actúa como en un BST. Si el nodo que sale era rojo, termina. Si era negro, algunos caminos pierden una unidad de altura negra. La reparación interpreta esa posición como si tuviera un negro extra y analiza el hermano en cuatro casos: hermano rojo; hermano negro con dos hijos negros; hijo cercano rojo; o hijo lejano rojo.

> Es más largo que insertar, pero sigue recorriendo un solo camino, usa como máximo tres rotaciones y cuesta `O(log n)`.

### 6:40-7:00 - Cierre

> En resumen, el color no es decorativo: codifica suficiente información para limitar la altura. Las rotaciones conservan el orden y las recoloraciones conservan la altura negra. El resultado es un BST dinámico con búsqueda, inserción y borrado garantizados en `O(log n)`.

Ejecutar si hay terminal disponible:

```bash
cd red-black-trees
make test
```

## 16. Plan De Diapositivas

Para una exposición breve bastan siete diapositivas:

| Diapositiva | Contenido | Tiempo |
|---:|---|---:|
| 1 | BST degenerado y motivación | 45 s |
| 2 | Cinco propiedades y árbol ejemplo | 60 s |
| 3 | Altura negra y cota logarítmica | 50 s |
| 4 | Rotación izquierda | 45 s |
| 5 | Tres casos de inserción | 115 s |
| 6 | Representación C++ y validación | 50 s |
| 7 | Borrado, complejidad y conclusión | 55 s |

Evitar colocar todo el código de `insert_fixup` en una diapositiva. Es más claro mostrar los tres diagramas y después relacionarlos con los nombres de las funciones.

## 17. Preguntas Probables

### ¿Por qué la raíz debe ser negra?

Es una normalización que simplifica propiedades y demostraciones. Pintar de negro una raíz roja añade un negro por igual a todos los caminos, por lo que no rompe la igualdad de altura negra.

### ¿Por qué no insertar directamente un nodo negro?

Porque solamente los caminos que pasen por el nodo nuevo ganarían un negro. Insertarlo rojo conserva inicialmente la altura negra.

### ¿Una rotación cambia el resultado de una búsqueda?

No. Conserva el recorrido in-order y, por tanto, la propiedad BST.

### ¿El negro extra es un color real?

No. Es un recurso conceptual para seguir la unidad de altura negra perdida al eliminar un nodo negro.

### ¿Por qué usar `NIL` en vez de `nullptr`?

Porque las reparaciones consultan repetidamente el color de hijos y hermanos. Un centinela negro evita casos especiales y sigue directamente el algoritmo de Cormen.

### ¿Cuándo conviene AVL y cuándo rojinegro?

AVL mantiene un balance más estricto y puede favorecer búsquedas. Rojinegro permite más flexibilidad estructural y suele ser atractivo cuando hay muchas actualizaciones. Ambos garantizan operaciones `O(log n)`.

### ¿Qué ocurre con claves repetidas?

Es una decisión de interfaz, no una propiedad esencial del color. Este proyecto las rechaza y funciona como un conjunto. Otra implementación podría guardar un contador o definir una política consistente para repeticiones.

### ¿Cuántas rotaciones se necesitan?

La inserción requiere como máximo dos. La reparación del borrado requiere como máximo tres.

## 18. Lista De Comprobación

Antes de considerar correcta una implementación:

- [ ] La raíz es negra.
- [ ] El centinela es negro.
- [ ] Todos los enlaces ausentes apuntan al centinela.
- [ ] El recorrido in-order está ordenado.
- [ ] No hay un nodo rojo con hijo rojo.
- [ ] Todo nodo tiene caminos descendentes de igual altura negra.
- [ ] Las rotaciones actualizan hijos, padres y raíz.
- [ ] La inserción implementa casos 1, 2 y 3 con sus espejos.
- [ ] El borrado implementa casos 1, 2, 3 y 4 con sus espejos.
- [ ] El destructor no intenta liberar el centinela.
- [ ] Las pruebas validan después de cada actualización.
- [ ] ASan y UBSan terminan sin errores.

## 19. Resumen Para Repaso

```text
Árbol rojinegro = BST + un bit de color + cinco propiedades

Propiedades clave:
- raíz negra
- NIL negro
- rojo no tiene hijo rojo
- igual cantidad de negros en todos los caminos descendentes

Consecuencia:
h <= 2 log2(n + 1)

Inserción:
- nodo nuevo rojo
- tío rojo -> recolorear y subir
- tío negro + triángulo -> girar padre
- tío negro + línea -> recolorear y girar abuelo

Borrado de negro:
- reparar un déficit conceptual de negro
- cuatro casos según hermano y sobrinos

Costos:
search, insert, erase = O(log n)
rotation = O(1)
valid, inorder = O(n)
```

> [!summary] Idea central
> El árbol no mantiene alturas exactas. Controla cuántos nodos negros aparecen en cada camino y prohíbe rojos consecutivos. Esas dos restricciones bastan para impedir cadenas largas y garantizar altura logarítmica.
