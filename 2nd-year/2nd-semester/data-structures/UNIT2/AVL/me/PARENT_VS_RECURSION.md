# AVL Con `parent` Frente A AVL Recursivo Sin `parent`

## Objetivo

Un árbol AVL combina dos invariantes:

1. Es un árbol binario de búsqueda.
2. La diferencia de altura entre los hijos de cualquier nodo es como máximo 1.

Hay dos formas habituales de conservar el camino desde el punto modificado hasta
la raíz:

- Guardar un puntero `parent` en cada nodo y ascender iterativamente.
- No guardar `parent` y utilizar el retorno de las llamadas recursivas.

Las dos formas pueden almacenar la altura y conseguir los mismos tiempos
asintóticos. La diferencia principal está en dónde se guarda el camino: de forma
permanente en los nodos o temporalmente en la pila de llamadas.

## Convención De Altura

Las implementaciones de `AVL/me/c11` y `AVL/me/cpp` utilizan:

```text
height(nullptr) = -1
height(hoja)    = 0
```

Por tanto:

```text
height(node) = 1 + max(height(node->left), height(node->right))
```

Ejemplo:

```text
        20          altura 2
       /  \
     10    30       altura 1
          /
         25         altura 0
```

La consulta no recorre el subárbol. Cada nodo almacena su altura:

```cpp
static int nodeHeight(const Node *node) {
  return node == nullptr ? -1 : node->height;
}
```

La actualización local cuesta `O(1)`:

```cpp
static void updateHeight(Node *node) {
  node->height =
      1 + std::max(nodeHeight(node->left), nodeHeight(node->right));
}
```

Cambiar desde la convención `nullptr = 0`, `hoja = 1` no modifica el factor de
balance ni las rotaciones. Solamente resta uno a todas las alturas.

## Factor De Balance

Las nuevas implementaciones usan:

```text
balance(node) = height(right) - height(left)
```

Entonces:

```text
balance > 1   demasiado peso a la derecha
balance < -1  demasiado peso a la izquierda
```

Otra implementación puede usar `left - right`. Ambas convenciones son válidas,
pero las condiciones de rotación deben respetar el signo elegido.

## Representación Con `parent`

```cpp
struct Node {
  int key;
  Node *left;
  Node *right;
  Node *parent;
  int height;
};
```

Cada nodo puede ascender directamente:

```text
nodo -> padre -> abuelo -> ... -> raíz
```

El camino permanece almacenado mientras el nodo exista.

### Inserción Iterativa

Primero se busca un enlace libre y se recuerda el padre:

```cpp
Node **link = &tree.root;
Node *parent = nullptr;

while (*link != nullptr) {
  parent = *link;
  link = key <= parent->key ? &parent->left : &parent->right;
}

*link = new Node(key, parent);
```

Después se asciende usando `parent`:

```cpp
Node *current = (*link)->parent;

while (current != nullptr) {
  Node *subtreeRoot = rebalance(tree, current);
  current = subtreeRoot->parent;
}
```

No se necesita una pila explícita ni recursión para recordar los ancestros.

### Rotación Con `parent`

Una rotación izquierda debe modificar más enlaces:

```cpp
static Node *rotateLeft(Tree &tree, Node *x) {
  Node *y = x->right;
  Node *beta = y->left;
  Node *oldParent = x->parent;

  y->parent = oldParent;
  if (oldParent == nullptr)
    tree.root = y;
  else if (oldParent->left == x)
    oldParent->left = y;
  else
    oldParent->right = y;

  y->left = x;
  x->parent = y;
  x->right = beta;
  if (beta != nullptr)
    beta->parent = x;

  updateHeight(x);
  updateHeight(y);
  return y;
}
```

Además de `left` y `right`, hay que mantener:

- El padre de la nueva raíz.
- El enlace del antiguo padre.
- El padre del nodo que baja.
- El padre del subárbol intermedio `beta`.
- La raíz global cuando `x` era la raíz.

La rotación sigue costando `O(1)`, pero tiene más invariantes y más posibilidades
de dejar un enlace inconsistente.

## Representación Sin `parent`

```cpp
struct Node {
  int key;
  Node *left;
  Node *right;
  int height;
};
```

No existe un enlace permanente hacia el ancestro. El camino se conserva en la
pila recursiva mientras dura la operación.

### Inserción Recursiva

```cpp
static Node *insertNode(Node *node, int key) {
  if (node == nullptr)
    return new Node(key);

  if (key <= node->key)
    node->left = insertNode(node->left, key);
  else
    node->right = insertNode(node->right, key);

  return rebalance(node);
}
```

La función pública conecta la raíz retornada:

```cpp
root = insertNode(root, key);
```

Cada llamada pendiente contiene temporalmente:

- El nodo ancestro.
- El punto exacto donde debe conectarse el hijo retornado.
- El lugar desde el que continuará el rebalanceo.

Las asignaciones siguientes son esenciales:

```cpp
node->left = insertNode(node->left, key);
node->right = insertNode(node->right, key);
return rebalance(node);
```

Una rotación cambia la raíz local. Si se ignora el retorno, el padre conservaría
un enlace hacia el nodo antiguo y se perdería la nueva raíz del subárbol.

### Rotación Sin `parent`

```cpp
static Node *rotateLeft(Node *x) {
  Node *y = x->right;
  Node *beta = y->left;

  x->right = beta;
  y->left = x;

  updateHeight(x);
  updateHeight(y);
  return y;
}
```

El llamador conecta el retorno. No hace falta conocer al padre ni modificar la
raíz global dentro de la rotación.

El orden de las alturas es importante:

1. Actualizar `x`, que bajó.
2. Actualizar `y`, cuya altura depende de la nueva altura de `x`.

## Eliminación

### Con `parent`

La eliminación iterativa guarda el padre del lugar físicamente eliminado y
asciende desde él:

```cpp
Node *current = parentOfRemovedNode;
while (current != nullptr) {
  Node *subtreeRoot = rebalance(tree, current);
  current = subtreeRoot->parent;
}
```

Hay que actualizar el `parent` del hijo que reemplaza al nodo eliminado.

### Sin `parent`

La eliminación recursiva devuelve la nueva raíz de cada subárbol:

```cpp
node->left = eraseNode(node->left, key);
node->right = eraseNode(node->right, key);
return rebalance(node);
```

Al eliminar un nodo con cero o un hijo se retorna directamente su reemplazo. Al
eliminar uno con dos hijos se copia el sucesor y se elimina una aparición en el
subárbol derecho.

## Complejidades

Suponiendo altura almacenada en ambas versiones:

| Operación | Con `parent`, iterativo | Sin `parent`, recursivo |
|---|---:|---:|
| Consultar altura de un nodo | `O(1)` | `O(1)` |
| Actualizar altura | `O(1)` | `O(1)` |
| Factor de balance | `O(1)` | `O(1)` |
| Rotación simple o doble | `O(1)` | `O(1)` |
| Buscar | `O(log n)` | `O(log n)` |
| Mínimo o máximo | `O(log n)` | `O(log n)` |
| Insertar | `O(log n)` | `O(log n)` |
| Eliminar | `O(log n)` | `O(log n)` |
| Validar todo el árbol | `O(n)` | `O(n)` |
| Destruir todo el árbol | `O(n)` | `O(n)` |

Las complejidades temporales principales son iguales porque un AVL mantiene:

```text
h = O(log n)
```

## Comparación De Memoria

### Con `parent`

```text
Memoria permanente por nodo: un puntero adicional
Memoria auxiliar al insertar/eliminar iterativamente: O(1)
```

En una plataforma de 64 bits, el puntero suele ocupar 8 bytes, aunque el tamaño
real de la estructura también depende de alineación y relleno.

### Sin `parent`

```text
Memoria permanente por nodo: no guarda parent
Memoria auxiliar al insertar/eliminar recursivamente: O(log n)
```

La memoria `O(log n)` desaparece al terminar la operación. Es la pila de llamadas,
no memoria almacenada permanentemente en el árbol.

## Operaciones Favorecidas Por `parent`

Guardar `parent` resulta útil para:

- Rebalancear iterativamente con memoria auxiliar `O(1)`.
- Obtener la profundidad de un nodo subiendo hasta la raíz.
- Calcular sucesor y predecesor desde un `Node *`.
- Crear iteradores bidireccionales.
- Navegar desde cualquier nodo hasta sus ancestros.
- Implementar inserción y eliminación sin recursión ni pila explícita.

Sin `parent`, un sucesor sigue siendo posible si se dispone de la raíz y se vuelve
a buscar el camino. Lo que no puede hacerse directamente es ascender desde un
`Node *` aislado.

## Ventajas De No Guardar `parent`

- Nodo más pequeño.
- Menos enlaces que mantener.
- Rotaciones más cortas.
- Menor riesgo de punteros inconsistentes.
- La recursividad expresa naturalmente el descenso y el ascenso.
- La nueva raíz del subárbol se comunica mediante el valor retornado.

## Combinaciones Posibles

### `parent` Más Iteración

Es la combinación que mejor aprovecha el puntero al padre:

```text
camino permanente en parent
ascenso iterativo
memoria auxiliar O(1)
```

### Sin `parent` Más Recursividad

Es la combinación estructuralmente más sencilla:

```text
camino temporal en la pila
ascenso mediante retornos
memoria auxiliar O(log n)
```

### `parent` Más Recursividad

Es correcto, pero normalmente redundante. El camino existe simultáneamente en
los punteros `parent` y en la pila recursiva. Puede justificarse si otras
operaciones necesitan `parent`, pero la inserción no aprovecha su principal
ventaja de memoria auxiliar.

### Sin `parent` Más Iteración

También es posible, pero debe guardarse explícitamente el camino:

```cpp
std::vector<Node **> path;
```

Después se recorre al revés para actualizar alturas y reconectar las raíces
retornadas por las rotaciones. Esto reproduce manualmente lo que la pila
recursiva hace automáticamente y requiere `O(log n)` de memoria auxiliar.

## API Pública Y Helpers Privados

La representación elegida no cambia la separación de responsabilidades.

La API pública trabaja con el árbol y con claves:

```cpp
bool insert(int key);
bool erase(int key);
bool contains(int key) const;
int height() const;
std::size_t size() const;
```

Los helpers privados trabajan con nodos:

```cpp
static int nodeHeight(const Node *node);
static void updateHeight(Node *node);
static Node *rotateLeft(Node *node);
static Node *rotateRight(Node *node);
static Node *rebalance(Node *node);
```

Así el usuario no puede modificar enlaces, alturas o factores de balance y romper
los invariantes del AVL.

## Errores Frecuentes

- Recalcular alturas recorriendo subárboles: degrada inserción y eliminación.
- No actualizar las alturas después de una rotación.
- Actualizar primero la nueva raíz y después el nodo que bajó.
- Ignorar la raíz retornada por una rotación recursiva.
- Con `parent`, olvidar actualizar el padre de `beta`.
- Con `parent`, olvidar actualizar la raíz global.
- Mezclar las convenciones `right - left` y `left - right`.
- Mezclar `nullptr = -1` con nodos nuevos inicializados en altura 1.
- Permitir que funciones públicas expongan o modifiquen `Node *` internos.

## Conclusión

Ninguna variante es universalmente superior:

| Prioridad de diseño | Opción natural |
|---|---|
| Código corto y rotaciones simples | Sin `parent`, recursivo |
| Menos memoria permanente por nodo | Sin `parent`, recursivo |
| Memoria auxiliar `O(1)` al modificar | Con `parent`, iterativo |
| Iteradores y navegación ascendente | Con `parent`, iterativo |

Para aprender y construir un AVL desde cero, la versión recursiva sin `parent`
suele ser más directa. Para una estructura que exponga iteradores o necesite
navegación frecuente entre nodos y ancestros, `parent` puede justificar su costo.
