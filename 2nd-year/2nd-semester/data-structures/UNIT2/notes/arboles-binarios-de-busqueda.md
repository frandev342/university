# Árboles binarios y árboles binarios de búsqueda

Estas notas integran el material de `docs/05 - Árboles binarios de búsqueda.pdf`, la práctica basada en Nick Parlante y las observaciones realizadas durante la implementación del proyecto `binaryTreeNickParlante`.

El código está expresado en C y sigue la convención concreta del proyecto:

```text
valor <= raíz  -> subárbol izquierdo
valor > raíz   -> subárbol derecho
```

Esta política permite claves repetidas, pero únicamente por el lado izquierdo correspondiente. Es importante conservar esta convención en `insert`, `isBST` y cualquier otra operación que dependa del orden.

## 1. Conceptos fundamentales

Un árbol es un grafo finito, conexo y sin ciclos. Como consecuencia:

- Entre dos nodos existe un único camino simple.
- Un árbol no vacío con `n` nodos posee `n - 1` aristas.
- Al escoger una raíz aparecen las relaciones padre, hijo, ancestro y descendiente.

Un árbol binario es una estructura definida recursivamente como:

- El árbol vacío, representado en C por `NULL`.
- Un nodo raíz con un subárbol izquierdo y otro derecho, ambos árboles binarios.

Las posiciones izquierda y derecha son diferentes. Un nodo con un único hijo izquierdo no tiene la misma estructura que uno con un único hijo derecho.

### Terminología

- **Raíz:** primer nodo del árbol; no tiene padre.
- **Hoja:** nodo sin hijos.
- **Nodo interno:** nodo con al menos un hijo.
- **Padre:** nodo conectado directamente por encima de otro.
- **Hijo:** nodo conectado directamente por debajo de otro.
- **Ancestro:** nodo que aparece en el camino desde la raíz hasta otro nodo.
- **Descendiente:** nodo situado dentro del subárbol de otro.
- **Subárbol:** árbol formado por un nodo y todos sus descendientes.

### Formas especiales

| Tipo | Condición |
|---|---|
| Estricto o propio | Cada nodo tiene cero o dos hijos. |
| Perfecto | Es estricto y todas las hojas tienen la misma profundidad. |
| Completo | Todos los niveles están llenos salvo quizá el último, que se llena de izquierda a derecha. |
| Degenerado o cadena | Cada nodo tiene como máximo un hijo. |

Un árbol binario de búsqueda ordinario no tiene por qué ser estricto, perfecto, completo ni balanceado.

En un árbol binario estricto, si `i` es el número de nodos internos y `e` el número de hojas:

```text
e = i + 1
```

Esta igualdad no se cumple en cualquier árbol binario. Por ejemplo, una cadena de tres nodos tiene dos nodos internos y una sola hoja.

### Árbol extendido y enlaces nulos

Cada nodo real tiene dos posiciones para hijos. Si se interpreta cada enlace `NULL` como un nodo externo conceptual, el árbol resultante es estricto.

Con `n` nodos reales hay:

```text
n + 1 enlaces nulos
```

Una hoja real y un enlace nulo no son lo mismo. La hoja almacena un dato; el enlace nulo representa la ausencia de un subárbol.

### Número de formas posibles

El número de árboles binarios estructuralmente distintos con `n` nodos es el número de Catalan:

```text
C0 = 1
Cn = suma(Ci * C(n-1-i)), para i desde 0 hasta n-1
```

Los primeros valores son:

| n | 0 | 1 | 2 | 3 | 4 | 5 |
|---:|---:|---:|---:|---:|---:|---:|
| Cn | 1 | 1 | 2 | 5 | 14 | 42 |

La recurrencia puede implementarse probando cada nodo como raíz. Si la raíz ocupa la posición `root`, quedan `root - 1` claves a la izquierda y `numKeys - root` a la derecha:

```c
int countTrees(int numKeys) {
  if (numKeys < 0)
    return 0;
  if (numKeys <= 1)
    return 1;

  int sum = 0;
  for (int root = 1; root <= numKeys; ++root) {
    int left = countTrees(root - 1);
    int right = countTrees(numKeys - root);
    sum += left * right;
  }
  return sum;
}
```

Esta versión recalcula los mismos valores muchas veces. Sin memoización tarda aproximadamente `O(3^n)` y utiliza `O(n)` de pila. Una recurrencia directa entre números de Catalan evita esa repetición:

```text
Cn = C(n-1) * 2 * (2n - 1) / (n + 1)
```

```c
long long countTrees2(int numKeys) {
  if (numKeys < 0)
    return 0;
  if (numKeys == 0)
    return 1;

  long long previous = countTrees2(numKeys - 1);
  return previous * 2 * (2LL * numKeys - 1) / (numKeys + 1);
}
```

Esta segunda versión tarda `O(n)` y utiliza `O(n)` de pila. El sufijo `LL` obliga a evaluar el producto como `long long`. Con aritmética `int`, `countTrees2(17)` ya puede desbordar en un producto intermedio aunque el resultado correcto, `129644790`, todavía quepa en `int`. Incluso `long long` posee un límite; para valores grandes se necesita aritmética de precisión arbitraria o detección explícita de desbordamiento.

## 2. Representación en C

El proyecto representa cada nodo así:

```c
typedef struct node {
  int data;
  struct node *left;
  struct node *right;
} Node;
```

Cada nodo contiene:

- El dato o clave, `data`.
- Un puntero a la raíz del subárbol izquierdo.
- Un puntero a la raíz del subárbol derecho.

Un puntero `Node *root` representa el árbol completo:

```c
Node *root = NULL;
```

`root == NULL` significa que el árbol está vacío.

### Creación de un nodo

```c
Node *newNode(int data) {
  Node *p = malloc(sizeof *p);
  if (p == NULL)
    return NULL;

  p->data = data;
  p->left = NULL;
  p->right = NULL;
  return p;
}
```

`malloc(sizeof *p)` reserva exactamente la memoria necesaria para el objeto apuntado por `p`. Esta forma evita repetir el tipo y continúa siendo correcta si posteriormente cambia la declaración de `p`.

El nodo se crea aislado porque ambos hijos empiezan en `NULL`.

## 3. Árbol binario de búsqueda (ABB)

Un ABB añade una propiedad de orden al árbol binario. En este proyecto, para cada nodo `x`:

```text
Todas las claves de x->left  son <= x->data.
Todas las claves de x->right son >  x->data.
```

La condición comprende el subárbol completo, no solamente los hijos inmediatos.

Este árbol no es un ABB:

```text
       10
      /
     5
      \
       12
```

`12` es mayor que su padre `5`, pero se encuentra dentro del subárbol izquierdo de `10`, donde todos los valores deberían ser `<= 10`.

### Duplicados

La política de duplicados es parte del contrato de la estructura. El material general del PDF usa principalmente claves distintas, pero menciona que la práctica de Parlante dirige los iguales a la izquierda.

Por tanto:

```text
    10              10
   /                  \
  10                   10

 válido              inválido
```

Con duplicados, el recorrido inorder produce una secuencia no decreciente, no necesariamente estrictamente creciente.

## 4. Inserción y efecto del orden

La inserción recursiva del proyecto es:

```c
Node *insert(Node *root, int data) {
  if (root == NULL)
    return newNode(data);

  if (data <= root->data) {
    Node *p = insert(root->left, data);
    if (p != NULL)
      root->left = p;
  } else {
    Node *p = insert(root->right, data);
    if (p != NULL)
      root->right = p;
  }

  return root;
}
```

La idea es:

1. Si se llega a un enlace `NULL`, allí debe crearse el nodo.
2. Si el dato es menor o igual, se continúa por la izquierda.
3. Si es mayor, se continúa por la derecha.
4. Cada llamada devuelve la raíz del subárbol resultante para que el padre actualice su enlace.

### Construcción de `build123`

Para obtener:

```text
    2
   / \
  1   3
```

el primer valor insertado debe ser `2`. Tanto `{2, 1, 3}` como `{2, 3, 1}` producen esa forma.

Insertar `{1, 2, 3}` produce una cadena:

```text
1
 \
  2
   \
    3
```

El conjunto final es el mismo, pero la forma y el costo de las operaciones cambian.

### Altura y orden de inserción

Insertar valores ya ordenados puede producir un árbol degenerado. Un ABB ordinario no garantiza operaciones logarítmicas.

- Árbol aproximadamente balanceado: altura `O(log n)`.
- Árbol degenerado: altura `O(n)`.

Construir una cadena mediante `n` inserciones puede costar `O(n²)` en total, porque cada nueva inserción recorre una cadena cada vez más larga.

## 5. Profundidad, altura y `maxDepth`

La **profundidad de un nodo** es la distancia desde la raíz hasta ese nodo.

La **altura de un nodo** es la longitud del camino descendente más largo desde ese nodo hasta una hoja.

Existen dos convenciones frecuentes y no deben mezclarse.

### Convención del PDF: cantidad de aristas

```text
altura(árbol vacío) = -1
altura(hoja) = 0
```

Con esta convención no hace falta una función auxiliar; basta con que el caso base devuelva `-1`:

```c
int maxDepthEdges(const Node *root) {
  if (root == NULL)
    return -1;

  int leftDepth = maxDepthEdges(root->left);
  int rightDepth = maxDepthEdges(root->right);
  return 1 + (leftDepth > rightDepth ? leftDepth : rightDepth);
}
```

Así, una hoja calcula `1 + max(-1, -1) = 0`.

### Convención de `maxDepth` en la práctica: cantidad de nodos

```text
maxDepth(árbol vacío) = 0
maxDepth(hoja) = 1
```

Para un árbol no vacío:

```text
maxDepth = altura_en_aristas + 1
```

La implementación de la práctica es:

```c
int maxDepth(const Node *root) {
  if (root == NULL)
    return 0;

  int leftDepth = maxDepth(root->left);
  int rightDepth = maxDepth(root->right);

  if (leftDepth > rightDepth)
    return 1 + leftDepth;
  return 1 + rightDepth;
}
```

La profundidad máxima es uno por el nodo actual más la mayor profundidad de sus dos subárboles.

### Qué significa `O(h)` en la recursión

`h` representa la altura o profundidad máxima, según la convención utilizada. En el análisis de memoria, `O(h)` no es la cantidad total de llamadas, sino la cantidad máxima de llamadas activas simultáneamente en la pila.

En una cadena:

```text
maxDepth(1)
  espera maxDepth(2)
    espera maxDepth(3)
      espera maxDepth(4)
```

Hay cuatro llamadas activas a la vez. En cambio, en un árbol balanceado con siete nodos solo hay unas tres llamadas de nodos activas sobre el camino más profundo.

Por eso:

- Tiempo de `maxDepth`: `O(n)`, porque visita todos los nodos.
- Memoria auxiliar: `O(h)`, por la pila recursiva.
- Balanceado: pila `O(log n)`.
- Degenerado: pila `O(n)`.

## 6. Tamaño del árbol: `treeSize`

El tamaño es la cantidad total de nodos:

```c
size_t treeSize(const Node *root) {
  if (root == NULL)
    return 0;

  return treeSize(root->left) + 1 + treeSize(root->right);
}
```

La recurrencia es:

```text
tamaño = tamaño(izquierda) + 1 + tamaño(derecha)
```

El `1` cuenta el nodo actual.

- Tiempo: `O(n)`.
- Memoria auxiliar: `O(h)`.

Una altura pequeña no permite calcular el tamaño en `O(h)` con esta representación, porque ningún nodo almacena el tamaño de su subárbol. Es necesario visitar todos los nodos.

## 7. Recorridos en profundidad

Los tres recorridos cambian únicamente el momento en que se procesa la raíz:

| Recorrido | Orden local |
|---|---|
| Preorden | raíz, izquierda, derecha |
| Inorder | izquierda, raíz, derecha |
| Postorden | izquierda, derecha, raíz |

Para:

```text
       8
      / \
     3  10
    / \
   1   6
```

los recorridos son:

```text
Preorden:  8, 3, 1, 6, 10
Inorder:   1, 3, 6, 8, 10
Postorden: 1, 6, 3, 10, 8
```

Cada recorrido visita todos los nodos exactamente una vez:

- Tiempo: `O(n)`.
- Memoria recursiva: `O(h)`.

### Inorder: `printTree`

```c
void printTree(const Node *root) {
  if (root == NULL)
    return;

  printTree(root->left);
  printf("%d ", root->data);
  printTree(root->right);
}
```

El recorrido inorder existe para cualquier árbol binario, pero solo produce valores ordenados cuando el árbol cumple la propiedad de ABB.

La función no imprime el salto de línea para que el llamador controle el formato:

```c
printf("inorder: ");
printTree(root);
printf("\n");
```

### Postorden: `printPostorder`

```c
void printPostorder(const Node *root) {
  if (root == NULL)
    return;

  printPostorder(root->left);
  printPostorder(root->right);
  printf("%d ", root->data);
}
```

No basta con mover el `printf`; las llamadas recursivas también deben invocar `printPostorder`. Llamar a `printTree` recorrería cada subárbol inorder y el error podría quedar oculto si todos los hijos son hojas.

El postorden es especialmente útil para liberar memoria: primero procesa los hijos y después el padre.

## 8. Mínimo y máximo

En un ABB:

- El mínimo está en el extremo izquierdo.
- El máximo está en el extremo derecho.

```c
int minValue(const Node *root) {
  while (root->left != NULL)
    root = root->left;
  return root->data;
}

int maxValue(const Node *root) {
  while (root->right != NULL)
    root = root->right;
  return root->data;
}
```

Estas funciones tienen como precondición que `root != NULL` y que el árbol sea un ABB.

- Tiempo: `O(h)`.
- Memoria auxiliar iterativa: `O(1)`.

La versión recursiva tiene el mismo tiempo, pero usa `O(h)` de pila. Como mínimo y máximo recorren un único camino, la versión iterativa es particularmente natural.

El mínimo no tiene hijo izquierdo, pero sí puede tener hijo derecho. Ser el mínimo no implica ser hoja.

Una interfaz más general puede devolver `Node *` o `NULL` en vez de un `int`. Así no necesita reservar `INT_MIN` o `INT_MAX` como códigos de error, pues ambos son claves válidas.

## 9. Caminos raíz-hoja

Una hoja cumple:

```c
root->left == NULL && root->right == NULL
```

No debe confundirse una hoja con cualquier enlace `NULL`. Un nodo puede tener un hijo ausente y otro presente; en ese caso todavía no es hoja.

### `hasPathSum`

La función debe determinar si existe un camino completo desde la raíz hasta una hoja cuya suma sea `sum`.

```c
int hasPathSum(const Node *root, int sum) {
  if (root == NULL)
    return 0;

  int remaining = sum - root->data;

  if (root->left == NULL && root->right == NULL)
    return remaining == 0;

  return hasPathSum(root->left, remaining) ||
         hasPathSum(root->right, remaining);
}
```

En cada nivel se resta el valor actual. Al llegar a una hoja, el camino sirve exactamente cuando la suma restante es cero.

Un caso base como este es incorrecto para la definición raíz-hoja:

```c
if (root == NULL)
  return sum == 0;
```

Podría aceptar un camino que termina en el hijo ausente de un nodo que todavía tiene otro hijo.

El operador `||` utiliza cortocircuito: si el lado izquierdo encuentra un camino, no se evalúa el derecho.

- Tiempo máximo: `O(n)`.
- Memoria recursiva: `O(h)`.

### `printPaths`

Para imprimir cada camino raíz-hoja se mantiene un arreglo con el camino actual y una longitud que se pasa por valor:

```c
static void printArray(const int path[], size_t length) {
  for (size_t i = 0; i < length; ++i)
    printf("%d ", path[i]);
  printf("\n");
}

static void printPathsRecur(const Node *root,
                            int path[], size_t pathLen) {
  if (root == NULL)
    return;

  path[pathLen++] = root->data;

  if (root->left == NULL && root->right == NULL) {
    printArray(path, pathLen);
    return;
  }

  printPathsRecur(root->left, path, pathLen);
  printPathsRecur(root->right, path, pathLen);
}
```

La longitud se pasa por valor, por lo que cada rama recibe su propia copia lógica del índice. Los elementos del arreglo pueden reutilizarse porque cada llamada solo considera las primeras `pathLen` posiciones.

La versión del proyecto utiliza `int path[1000]`. Eso impone la precondición de que ningún camino tenga más de 1000 nodos. En código general debería proporcionarse la capacidad, comprobarla o reservar memoria según la profundidad máxima.

## 10. Imagen espejo: `mirror`

Reflejar un árbol significa intercambiar el hijo izquierdo y derecho de cada nodo:

```c
void mirror(Node *root) {
  if (root == NULL)
    return;

  Node *temp = root->left;
  root->left = root->right;
  root->right = temp;

  mirror(root->left);
  mirror(root->right);
}
```

Es necesario procesar ambos subárboles. Repetir `mirror(root->left)` dos veces deja sin procesar el derecho; un árbol de tres nodos puede ocultar ese error porque ambos hijos son hojas.

También es correcto reflejar primero ambos subárboles y realizar el intercambio al final:

```c
void mirror(Node *root) {
  if (root == NULL)
    return;

  mirror(root->left);
  mirror(root->right);

  Node *temp = root->left;
  root->left = root->right;
  root->right = temp;
}
```

Intercambiar antes corresponde a procesar el nodo antes que sus descendientes; intercambiar después corresponde a postorden. Ambas variantes producen `nodo(mirror(derecho), mirror(izquierdo))`.

Lo que no debe hacerse es intercalar el intercambio entre las llamadas. Después de reflejar el hijo izquierdo, el intercambio cambia qué subárbol señala `root->right`; una segunda llamada mediante ese puntero puede procesar otra vez el mismo subárbol y omitir el otro. Las dos llamadas deben quedar juntas, antes o después del intercambio.

- Tiempo: `O(n)`. Es óptimo porque todos los nodos deben modificarse.
- Memoria auxiliar: `O(h)`.
- No crea ni elimina nodos; solamente intercambia punteros.

Si se refleja un ABB, normalmente deja de cumplir la convención original de búsqueda. Su inorder aparece en orden inverso.

## 11. Comparación estructural: `sameTree`

Dos árboles son iguales si tienen:

- La misma estructura.
- El mismo valor en cada posición correspondiente.

```c
int sameTree(const Node *a, const Node *b) {
  if (a == b)
    return 1;

  if (a == NULL || b == NULL)
    return 0;

  return a->data == b->data &&
         sameTree(a->left, b->left) &&
         sameTree(a->right, b->right);
}
```

Los casos son:

1. Si `a == b`, ambos punteros representan exactamente el mismo subárbol. Esto incluye el caso donde ambos son `NULL`.
2. Si solo uno es `NULL`, las estructuras son diferentes.
3. Si ambos existen, deben coincidir el dato, el lado izquierdo y el derecho.

No basta con manejar únicamente el caso donde ambos son `NULL`. Si uno es `NULL` y el otro no, acceder a `a->data` o `b->data` desreferencia un puntero nulo. La comprobación con `||` debe ocurrir antes de acceder a los datos.

Comparar solamente las llamadas recursivas comprueba la estructura, pero ignora los valores.

Gracias al cortocircuito de `&&`, la función se detiene en la primera diferencia.

- Tiempo máximo: `O(n)`.
- Memoria recursiva: `O(h)`.

## 12. Validación de un ABB

### Por qué una comprobación local no basta

Comprobar solo:

```text
hijo izquierdo <= padre
hijo derecho > padre
```

no garantiza la propiedad global. Cada nodo debe respetar las restricciones de todos sus ancestros.

### Versión con mínimo y máximo de cada subárbol

Una posibilidad es comprobar:

```c
int isBST(const Node *root) {
  if (root == NULL)
    return 1;

  if (root->left != NULL && maxValue(root->left) > root->data)
    return 0;

  if (root->right != NULL && minValue(root->right) <= root->data)
    return 0;

  return isBST(root->left) && isBST(root->right);
}
```

Se usa el máximo del lado izquierdo porque todos sus elementos deben ser `<= root->data`. Se usa el mínimo del derecho porque todos deben ser `> root->data`.

Esta versión es intuitiva, pero repite recorridos. En el peor caso puede costar `O(n²)`.

El resultado de las llamadas recursivas debe propagarse directamente:

```c
return isBST(root->left) && isBST(root->right);
```

Una terminación como esta es incorrecta porque siempre devuelve verdadero, incluso si un descendiente detecta una violación:

```c
if (isBST(root->left) && isBST(root->right))
  return 1;
return 1;
```

Por ejemplo, el árbol `10 <- 5 <- 6` contiene una violación en el subárbol de `5`. La llamada correspondiente puede devolver `0`, pero el último `return 1` descartaría ese resultado.

### Versión óptima con límites heredados

Cada llamada recibe el intervalo válido para el nodo actual:

```c
#include <limits.h>

static int isBSTUtil(const Node *root,
                     long long lowerExclusive,
                     long long upperInclusive) {
  if (root == NULL)
    return 1;

  if (root->data <= lowerExclusive ||
      root->data > upperInclusive)
    return 0;

  return isBSTUtil(root->left,
                   lowerExclusive, root->data) &&
         isBSTUtil(root->right,
                   root->data, upperInclusive);
}

int isBST2(const Node *root) {
  return isBSTUtil(root, LLONG_MIN, LLONG_MAX);
}
```

El intervalo válido es:

```text
lowerExclusive < dato <= upperInclusive
```

Al bajar a la izquierda, el dato actual se convierte en límite superior inclusivo. Esto permite igualdad a la izquierda.

Al bajar a la derecha, el dato actual se convierte en límite inferior exclusivo. Esto exige un valor estrictamente mayor.

### Por qué `long long`, `LLONG_MIN` y `LLONG_MAX`

Los datos de los nodos son `int`, mientras que los límites son `long long`. En las plataformas habituales, `long long` posee un rango mayor que `int`.

`LLONG_MIN` y `LLONG_MAX`, definidos en `<limits.h>`, representan los extremos del tipo `long long`. Se usan como cotas iniciales que contienen cualquier valor de tipo `int`.

No conviene iniciar el límite inferior con `INT_MIN` porque es exclusivo. Un nodo cuyo dato fuera exactamente `INT_MIN` sería rechazado incorrectamente:

```text
INT_MIN <= INT_MIN  -> verdadero -> rechazado
```

Tampoco conviene expresar el lado derecho mediante `root->data + 1`: si el dato es `INT_MAX`, la suma desborda un entero con signo y produce comportamiento indefinido.

La desigualdad estricta en el límite evita la suma:

```c
root->data <= lowerExclusive
```

Esta versión tiene:

- Tiempo: `O(n)`.
- Memoria recursiva: `O(h)`.
- Una sola visita por nodo.

El PDF presenta otra solución portable que usa punteros a nodos como límites y `NULL` para representar ausencia de cota. La idea es la misma: transmitir restricciones de todos los ancestros sin usar `key - 1` ni `key + 1`.

## 13. Liberación de memoria

La memoria debe liberarse en postorden:

```c
void freeTree(Node *root) {
  if (root == NULL)
    return;

  freeTree(root->left);
  freeTree(root->right);
  free(root);
}
```

Primero se liberan los hijos y después el nodo que contiene los punteros para alcanzarlos. Si se libera el padre antes, leer posteriormente `root->left` o `root->right` sería un acceso a memoria ya liberada.

- Tiempo: `O(n)`.
- Memoria recursiva: `O(h)`.

Un árbol degenerado muy grande puede agotar la pila debido a una profundidad recursiva `O(n)`.

Todo árbol construido debe liberarse exactamente una vez. Si `main` crea `root`, `root1`, `root2` y `root3`, debe llamar a `freeTree` para cada árbol independiente.

## 14. Búsqueda en un ABB

La propiedad de orden permite descartar un subárbol completo después de cada comparación:

```c
Node *search(Node *root, int key) {
  while (root != NULL && root->data != key) {
    if (key < root->data)
      root = root->left;
    else
      root = root->right;
  }

  return root;
}
```

Si encuentra la clave, devuelve su nodo. Si llega a `NULL`, la clave no existe. Una búsqueda fallida identifica además el enlace donde podría insertarse la clave.

- Tiempo: `O(h)` para un árbol no vacío.
- Espacio auxiliar iterativo: `O(1)`.
- Balanceado: `O(log n)`.
- Degenerado: `O(n)`.

## 15. Sucesor, predecesor y `lower_bound`

El sucesor de un nodo es el siguiente nodo en el recorrido inorder.

Existen dos casos:

1. Si tiene subárbol derecho, el sucesor es el mínimo de ese subárbol.
2. Si no lo tiene y existen punteros al padre, se sube hasta encontrar el primer ancestro al que se llega desde su lado izquierdo.

El predecesor es simétrico: intercambia izquierda por derecha y mínimo por máximo.

`lower_bound(key)` busca el menor elemento cuya clave sea mayor o igual que `key`. La clave consultada no necesita existir. Mientras recorre un único camino, conserva el mejor candidato encontrado.

Estas operaciones cuestan `O(h)` y pueden implementarse con `O(1)` de memoria auxiliar cuando existen los enlaces necesarios.

## 16. Eliminación en un ABB

Aunque la práctica actual no implementa eliminación individual, el PDF distingue tres casos estructurales para eliminar un nodo `z`:

| Hijos de `z` | Acción |
|---|---|
| Ninguno | El enlace que apuntaba a `z` pasa a `NULL`. |
| Uno | El único hijo ocupa el lugar de `z`. |
| Dos | El sucesor de `z` ocupa su lugar. |

Cuando `z` tiene dos hijos, se utiliza normalmente:

```text
y = mínimo del subárbol derecho de z
```

`y` puede reemplazar a `z` porque:

- No tiene hijo izquierdo; si lo tuviera, no sería el mínimo.
- Todo el subárbol izquierdo de `z` sigue siendo menor que `y`.
- Los demás elementos del subárbol derecho son mayores que `y`.

Si los nodos contienen puntero al padre, cada trasplante debe mantener el invariante bidireccional entre padre e hijo.

La eliminación cuesta `O(h)` porque la búsqueda del nodo y del sucesor recorre como máximo caminos de altura `h`.

## 17. ABB frente a min-heap binario

No deben confundirse:

| Propiedad | ABB | Min-heap binario |
|---|---|---|
| Orden | Todo el subárbol izquierdo es menor y el derecho mayor, según la política de duplicados. | Cada padre es menor o igual que sus hijos. |
| Forma | Arbitraria salvo balanceo adicional. | Completa. |
| Mínimo | Extremo izquierdo. | Raíz. |
| Buscar una clave cualquiera | `O(h)` | `O(n)` en el peor caso. |

Un heap está diseñado principalmente para implementar colas de prioridad. Un ABB está diseñado para búsqueda ordenada y consultas relacionadas con el orden.

## 18. Resumen de complejidades

`n` es la cantidad de nodos y `h` la altura o profundidad máxima relevante.

| Operación | Tiempo | Espacio auxiliar |
|---|---:|---:|
| `newNode` | `O(1)` | `O(1)` |
| Búsqueda iterativa | `O(h)` | `O(1)` |
| Inserción recursiva | `O(h)` | `O(h)` |
| `minValue`, `maxValue` iterativos | `O(h)` | `O(1)` |
| `treeSize` | `O(n)` | `O(h)` |
| `maxDepth` | `O(n)` | `O(h)` |
| Inorder, preorden, postorden | `O(n)` | `O(h)` |
| `hasPathSum` | `O(n)` | `O(h)` |
| `printPaths` | `O(n + salida)` | `O(h)` más el arreglo |
| `mirror` | `O(n)` | `O(h)` |
| `sameTree` | `O(n)` en el peor caso | `O(h)` |
| `countTrees` sin memoización | aproximadamente `O(3^n)` | `O(n)` |
| `countTrees2` recursivo | `O(n)` | `O(n)` |
| `isBST` con extremos repetidos | `O(n²)` en el peor caso | `O(h)` |
| `isBST2` con límites | `O(n)` | `O(h)` |
| `freeTree` | `O(n)` | `O(h)` |

En un árbol balanceado, `h = O(log n)`. En una cadena, `h = O(n)`.

## 19. Errores comunes observados durante la práctica

### Confundir forma con cantidad de nodos

`treeSize` no permite comprobar que `build123` tenga la forma correcta. Tanto un árbol balanceado como una cadena de tres nodos tienen tamaño 3.

### Probar solo con hojas

Un árbol de tres nodos puede ocultar errores recursivos. Por ejemplo, llamar accidentalmente a `printTree` dentro de `printPostorder` parece funcionar cuando ambos hijos son hojas.

### Confundir nodo hoja con enlace nulo

Una hoja tiene ambos hijos nulos. Un hijo nulo aislado no implica que el nodo padre sea una hoja. Esta distinción es esencial en `hasPathSum`.

### Repetir una rama recursiva

Funciones como `mirror` deben procesar izquierda y derecha exactamente una vez. Copiar dos veces `mirror(root->left)` omite el otro subárbol.

### Comparar punteros en vez de datos

Una expresión como:

```c
root->left <= root->right
```

compara direcciones, no valores, y no valida la propiedad del ABB.

### Verificar solamente hijos inmediatos

`isBST` debe conservar restricciones de todos los ancestros. Una comparación local no basta.

### No comparar datos en `sameTree`

Comparar solamente subárboles detecta la forma, pero dos árboles con la misma forma pueden almacenar datos diferentes.

### No propagar un resultado recursivo

Una llamada recursiva puede calcular el resultado correcto y aun así perderse si su función llamadora no lo devuelve. En una función que retorna `int`, ejecutar `minValueRec(root->left);` y llegar al final sin `return` produce comportamiento indefinido. En validadores como `isBST`, devolver `1` después de que un descendiente devolvió `0` oculta el error. Debe utilizarse `return llamadaRecursiva(...);` o combinar y devolver explícitamente los resultados.

### Confiar solo en pruebas simétricas

Un árbol donde todos los nodos internos tienen dos hijos puede ocultar una condición incorrecta en `printPaths`. Un árbol pequeño con hijos desiguales puede ocultar accesos nulos en `sameTree` o errores profundos de `isBST`. Las cadenas y los nodos con un solo hijo son casos de prueba obligatorios.

### Ignorar el desbordamiento intermedio

Que el resultado matemático quepa en un tipo no implica que todos los productos intermedios también quepan. En C, desbordar un entero con signo produce comportamiento indefinido. Debe ampliarse el tipo antes de multiplicar, por ejemplo mediante `2LL`, y aun así documentar el rango admitido.

### Olvidar el cortocircuito

Los operadores `&&` y `||` permiten detener la evaluación cuando el resultado ya es conocido. Esto simplifica `sameTree`, `hasPathSum` e `isBST`.

### Usar un formato no estándar

En C11, para imprimir un resultado booleano representado como `int`, debe utilizarse `%d`, no `%b`:

```c
printf("%d\n", hasPathSum(root, target));
```

### No liberar todos los árboles

Cada llamada independiente a una función constructora crea nodos diferentes. Todos esos árboles deben liberarse, incluso si almacenan exactamente los mismos valores.

### Mezclar enteros con y sin signo

Si la longitud es `int` y el índice es `size_t`, el compilador advierte por comparar tipos con signo diferente. Es preferible utilizar `size_t` para ambos cuando representan tamaños o índices.

## 20. Casos de prueba recomendados

No es suficiente probar únicamente `build123`. Cada operación debería comprobarse con:

1. Árbol vacío.
2. Un único nodo.
3. Árbol balanceado.
4. Cadena hacia la izquierda.
5. Cadena hacia la derecha.
6. Nodo con un solo hijo.
7. Valores negativos y cero.
8. Duplicados colocados a la izquierda.
9. Diferencias profundas, no solo en la raíz.
10. `INT_MIN` e `INT_MAX` para validar límites.

Para `isBST`, son especialmente importantes:

```text
Árbol vacío                         -> válido
Un nodo                             -> válido
Duplicado a la izquierda            -> válido
Duplicado a la derecha              -> inválido
Violación en un nieto               -> inválido
Árbol reflejado                     -> normalmente inválido
Nodo con INT_MIN o INT_MAX          -> válido si está bien ubicado
```

Para `hasPathSum` debe incluirse un nodo con un solo hijo, porque detecta implementaciones que consideran erróneamente cualquier enlace `NULL` como final de un camino.

## 21. Implementación de un ABB en C++

La versión en C++ utiliza punteros al padre y representa un **conjunto**: cada clave puede aparecer como máximo una vez. Esta política es diferente de la práctica anterior en C, donde los duplicados se dirigían a la izquierda.

Los archivos se dividen así:

- `tree.hpp`: estructuras y declaraciones públicas.
- `tree.cpp`: implementación de las operaciones.
- `main.cpp`: construcción, uso y pruebas.

### Representación

```cpp
#ifndef TREE_HPP
#define TREE_HPP

#include <cstddef>

struct Node {
  int key;
  Node *left = nullptr;
  Node *right = nullptr;
  Node *parent = nullptr;
};

struct Tree {
  Node *root = nullptr;
};

#endif // TREE_HPP
```

`Tree` permite distinguir la raíz del resto de los enlaces y hace posible modificarla mediante una referencia `Tree &`. El puntero `parent` permite navegar hacia los ancestros sin usar una pila explícita.

Los invariantes estructurales son:

```text
t.root == nullptr o t.root->parent == nullptr

x->left != nullptr  -> x->left->parent == x
x->right != nullptr -> x->right->parent == x
```

Además, como las claves son únicas:

```text
claves de x->left  < x->key
claves de x->right > x->key
```

Estas condiciones se aplican a los subárboles completos, no solamente a los hijos inmediatos.

### Creación de nodos

```cpp
#include <new>

Node *newNode(Node *parent, int key) {
  return new (std::nothrow) Node{key, nullptr, nullptr, parent};
}
```

`new (std::nothrow)` devuelve `nullptr` si no puede reservar memoria. El orden de inicialización corresponde a `key`, `left`, `right` y `parent`. La declaración del encabezado debe coincidir exactamente con la definición; `Node *` y `const Node *` producen firmas diferentes en C++.

### Tamaño y altura

```cpp
std::size_t tree_size(const Node *x) {
  if (x == nullptr)
    return 0;
  return 1 + tree_size(x->left) + tree_size(x->right);
}

int height(const Node *x) {
  if (x == nullptr)
    return -1;

  int leftHeight = height(x->left);
  int rightHeight = height(x->right);
  return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}
```

Esta versión de `height` cuenta aristas:

```text
height(árbol vacío) = -1
height(hoja) = 0
```

Ambas funciones visitan todos los nodos: tiempo `O(n)` y pila `O(h + 1)`.

### Inserción mediante un puntero al enlace

```cpp
int insert(Tree &t, int key) {
  Node **link = &t.root;
  Node *parent = nullptr;

  while (*link != nullptr) {
    parent = *link;

    if (key == parent->key)
      return 0;

    link = key < parent->key
               ? &parent->left
               : &parent->right;
  }

  Node *node = newNode(parent, key);
  if (node == nullptr)
    return -1;

  *link = node;
  return 1;
}
```

`link` no apunta solamente a un nodo: apunta al puntero que debe modificarse. Inicialmente es `&t.root`; después puede ser `&parent->left` o `&parent->right`. Por ello, la asignación final `*link = node` sirve tanto para insertar la primera raíz como para conectar cualquier hijo.

El resultado distingue:

```text
 1 -> clave insertada
 0 -> clave ya existente
-1 -> reserva de memoria fallida
```

La inserción tarda `O(h + 1)` y utiliza `O(1)` de memoria auxiliar.

### Búsqueda, mínimo y máximo

```cpp
Node *search(Node *x, int key) {
  while (x != nullptr && x->key != key) {
    if (key < x->key)
      x = x->left;
    else
      x = x->right;
  }
  return x;
}

Node *minimum(Node *x) {
  if (x == nullptr)
    return nullptr;
  while (x->left != nullptr)
    x = x->left;
  return x;
}

Node *maximum(Node *x) {
  if (x == nullptr)
    return nullptr;
  while (x->right != nullptr)
    x = x->right;
  return x;
}
```

Las tres operaciones recorren un único camino, por lo que tardan `O(h + 1)` y usan `O(1)` de espacio auxiliar. Devolver un nodo o `nullptr` permite que `INT_MIN` e `INT_MAX` sigan siendo claves válidas.

### Sucesor

El sucesor es el siguiente nodo en inorder:

```cpp
Node *successor(Node *x) {
  if (x == nullptr)
    return nullptr;

  if (x->right != nullptr)
    return minimum(x->right);

  Node *parent = x->parent;
  while (parent != nullptr && x == parent->right) {
    x = parent;
    parent = parent->parent;
  }
  return parent;
}
```

Existen dos casos:

1. Si `x` tiene hijo derecho, el sucesor es el mínimo de ese subárbol.
2. Si no lo tiene, se sube mientras se llega al padre desde un hijo derecho. El primer padre alcanzado desde su lado izquierdo es el sucesor.

Comparar la relación entre punteros expresa mejor la estructura que comparar claves y sigue siendo correcto si posteriormente cambia la política de claves repetidas.

### `lower_bound` y `upper_bound`

```cpp
Node *lower_bound(Node *x, int key) {
  Node *best = nullptr;

  while (x != nullptr) {
    if (x->key >= key) {
      best = x;
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return best;
}

Node *upper_bound(Node *x, int key) {
  Node *best = nullptr;

  while (x != nullptr) {
    if (x->key > key) {
      best = x;
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return best;
}
```

Las definiciones son:

```text
lower_bound(key) = menor clave >= key
upper_bound(key) = menor clave > key
```

Cuando el nodo actual es candidato se guarda en `best`, pero se continúa hacia la izquierda por si existe otro candidato menor. Si ningún nodo cumple la condición, se devuelve `nullptr`.

En un árbol sin duplicados:

- Si `key` existe, `lower_bound(key)` devuelve ese nodo.
- Si `key` existe, `upper_bound(key)` devuelve su sucesor.
- Si `key` no existe, ambas consultas devuelven el mismo candidato.

Ambas tardan `O(h + 1)` y utilizan `O(1)` de memoria auxiliar.

### Trasplante de subárboles

```cpp
void transplant(Tree &t, Node *u, Node *v) {
  if (u->parent == nullptr)
    t.root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  if (v != nullptr)
    v->parent = u->parent;
}
```

`transplant(t, u, v)` cambia el enlace que apuntaba a `u` para que apunte a `v`. Si `u` era la raíz, actualiza `t.root`; en otro caso actualiza el hijo izquierdo o derecho de su padre. Finalmente, hace que `v` reconozca al antiguo padre de `u`.

Sus precondiciones son:

```text
u != nullptr
u pertenece a t
los enlaces parent son coherentes
```

`v` sí puede ser `nullptr`, lo que permite desconectar una hoja. La función tarda `O(1)`, pero deliberadamente no realiza una eliminación completa:

- No libera `u`.
- No modifica los hijos de `u`.
- No conecta automáticamente los hijos de `u` con `v`.
- No retira automáticamente a `v` de su posición anterior.
- No garantiza por sí sola el orden del ABB.

Es una primitiva de bajo nivel que evita repetir en `erase` los casos raíz, hijo izquierdo e hijo derecho.

### Eliminación

```cpp
void erase(Tree &t, Node *z) {
  if (z->left == nullptr) {
    transplant(t, z, z->right);
  } else if (z->right == nullptr) {
    transplant(t, z, z->left);
  } else {
    Node *y = minimum(z->right);

    if (y->parent != z) {
      transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
  }

  delete z;
}
```

Las precondiciones son `z != nullptr` y que `z` pertenezca a `t`. Después de `delete z`, cualquier puntero externo que todavía apunte a `z` queda inválido.

Los casos estructurales son:

| Estado de `z` | Acción |
|---|---|
| Sin hijo izquierdo | `z->right` reemplaza a `z`; esto también cubre una hoja. |
| Sin hijo derecho | `z->left` reemplaza a `z`. |
| Dos hijos | El sucesor `y = minimum(z->right)` reemplaza a `z`. |

El sucesor `y` no tiene hijo izquierdo. Si no es el hijo derecho inmediato de `z`, primero se sustituye `y` por su posible hijo derecho:

```cpp
transplant(t, y, y->right);
```

Después `y` adopta el subárbol derecho de `z`. En todos los casos con dos hijos, `y` reemplaza a `z` y adopta su subárbol izquierdo.

Cuando `y == z->right`, no debe ejecutarse `y->right = z->right`, porque equivaldría a `y->right = y` y crearía un ciclo. Por eso esa asignación está dentro de `if (y->parent != z)`.

La eliminación tarda `O(h + 1)` por la búsqueda del sucesor y utiliza `O(1)` de memoria auxiliar.

### Liberación de memoria

```cpp
static void destroy(Node *root) {
  if (root == nullptr)
    return;

  destroy(root->left);
  destroy(root->right);
  delete root;
}

void destroy(Tree &t) {
  destroy(t.root);
  t.root = nullptr;
}
```

Los nodos se liberan en postorden para no perder el acceso a los hijos. Dejar `t.root` en `nullptr` evita que el objeto conserve un puntero colgante. El tiempo es `O(n)` y la pila es `O(h + 1)`.

### Propiedad manual y copias de `Tree`

Esta implementación usa punteros crudos y una llamada manual a `destroy`. `Tree` no tiene destructor ni semántica de copia profunda. Por tanto, una copia normal solo duplica el puntero raíz:

```cpp
Tree a = /* árbol con nodos */;
Tree b = a; // a.root y b.root apuntan a los mismos nodos
```

Después de `destroy(a)`, `b.root` queda colgante. Si se intentara destruir ambos, se produciría una doble liberación. Mientras se mantenga este diseño educativo, debe existir un único propietario lógico de cada conjunto de nodos y deben evitarse las copias de árboles no vacíos.

Si más adelante `Tree` incorpora un destructor, también debe definir o prohibir copia y movimiento de acuerdo con la regla de cinco. Una opción mínima es eliminar la copia:

```cpp
struct Tree {
  Node *root = nullptr;

  Tree() = default;
  Tree(const Tree &) = delete;
  Tree &operator=(const Tree &) = delete;
};
```

Con esa restricción, una función como `buildExample` debe construir sobre un `Tree &` recibido como argumento o `Tree` debe implementar además una transferencia de propiedad mediante movimiento. No conviene depender de que el compilador aplique NRVO al devolver una variable local con nombre.

### Uso seguro

Antes de acceder al resultado de una consulta debe comprobarse `nullptr`:

```cpp
Node *node = search(tree.root, key);

if (node != nullptr) {
  if (node->parent != nullptr)
    std::cout << node->parent->key << '\n';

  Node *next = successor(node);
  if (next != nullptr)
    std::cout << next->key << '\n';
}
```

La eliminación por clave también debe validar la búsqueda:

```cpp
Node *node = search(tree.root, key);
if (node != nullptr)
  erase(tree, node);
```

Llamar `erase(tree, nullptr)` produce comportamiento indefinido porque `erase` accede inmediatamente a `z->left`.

### Pruebas recomendadas para `erase`

La eliminación debe probarse por separado con:

1. Árbol de un solo nodo.
2. Hoja que sea hijo izquierdo.
3. Hoja que sea hijo derecho.
4. Nodo con solo hijo izquierdo.
5. Nodo con solo hijo derecho.
6. Nodo con dos hijos y sucesor inmediato.
7. Nodo con dos hijos y sucesor profundo.
8. Sucesor profundo que tenga hijo derecho.
9. Eliminación de la raíz en cada caso aplicable.
10. Clave inexistente, comprobada antes de llamar a `erase`.

Después de cada operación conviene verificar:

- El inorder permanece estrictamente creciente.
- `tree_size` disminuye exactamente en uno.
- La raíz tiene padre `nullptr`.
- Cada hijo no nulo apunta de vuelta a su padre.
- AddressSanitizer no detecta fugas, uso tras liberación ni doble liberación.

Una compilación útil durante el desarrollo es:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Wconversion \
  -fsanitize=address,undefined main.cpp tree.cpp -o tree
./tree
```

### Resumen de complejidades en C++

| Operación | Tiempo | Espacio auxiliar |
|---|---:|---:|
| `tree_size`, `height` | `O(n)` | `O(h + 1)` |
| `insert`, `search` | `O(h + 1)` | `O(1)` |
| `minimum`, `maximum` | `O(h + 1)` | `O(1)` |
| `successor` | `O(h + 1)` | `O(1)` |
| `lower_bound`, `upper_bound` | `O(h + 1)` | `O(1)` |
| `transplant` | `O(1)` | `O(1)` |
| `erase` | `O(h + 1)` | `O(1)` |
| `destroy` | `O(n)` | `O(h + 1)` |

En un árbol aproximadamente balanceado, `h = O(log n)`. En un árbol degenerado, `h = n - 1`. Un ABB ordinario no garantiza por sí solo operaciones logarítmicas.

## 22. Ideas centrales para recordar

1. Un árbol binario limita el número de hijos, pero no garantiza equilibrio.
2. La forma del ABB depende del orden de inserción.
3. La propiedad de búsqueda se aplica a subárboles completos.
4. Un recorrido inorder solo queda ordenado si el árbol cumple la propiedad de ABB.
5. Las operaciones que siguen un camino cuestan `O(h)`.
6. Las operaciones que deben visitar todos los nodos cuestan `O(n)`.
7. En una función recursiva, `O(h)` de memoria describe llamadas simultáneas, no llamadas totales.
8. El postorden permite liberar hijos antes que padres.
9. Las políticas de duplicados deben expresarse con desigualdades coherentes en todas las funciones.
10. La validación mediante límites heredados es más eficiente y segura que repetir búsquedas de mínimos y máximos.

## Referencias

- Alfredo Paz-Valderrama, *Árboles binarios de búsqueda: estructura, orden y operaciones*, material incluido en `docs/05 - Árboles binarios de búsqueda.pdf`.
- Nick Parlante, *Binary Trees*, Stanford CS Education Library, documento 110, 2000-2001.
- Cormen, Leiserson, Rivest y Stein, *Introduction to Algorithms*, 4.ª edición, capítulo 12.
- Erik Demaine, MIT 6.006, *Binary Trees, Part 1*, clase 6, primavera de 2020.
