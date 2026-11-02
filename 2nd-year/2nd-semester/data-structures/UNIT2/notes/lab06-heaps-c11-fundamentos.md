# Lab 6 de heaps: fundamentos de C11

Estas notas explican construcciones del lenguaje utilizadas en `heaps/heaps/c11/`, antes de implementar las operaciones pendientes del max-heap.

## 1. Literales enteros: `0U`, `1U` y `2U`

Los valores `0`, `1` y `2` son **literales enteros**. Sin sufijo, normalmente tienen tipo `int`:

```c
0   /* int */
1   /* int */
2   /* int */
```

El sufijo `U` significa `unsigned`. Por tanto:

```c
0U  /* unsigned int */
1U  /* unsigned int */
2U  /* unsigned int */
```

La letra puede escribirse en mayúscula o minúscula, pero `U` suele ser más legible:

```c
1U
1u
```

También existen otros sufijos:

```c
1L    /* long */
1UL   /* unsigned long */
1LL   /* long long */
1ULL  /* unsigned long long */
```

### Por qué aparecen en el heap

Los tamaños e índices utilizan `size_t`, un tipo entero sin signo definido en `<stddef.h>`:

```c
size_t size;
size_t capacity;
```

Por eso el código emplea constantes sin signo:

```c
h->size = 0U;
if (capacity != 0U) {
  /* ... */
}
```

El sufijo documenta que los tamaños nunca son negativos y ayuda a evitar algunas advertencias por mezclar enteros con y sin signo. No significa necesariamente que el literal tenga exactamente el mismo tipo que `size_t`: `size_t` puede ser, por ejemplo, `unsigned long`. Las conversiones aritméticas habituales de C convierten el operando de menor rango al tipo apropiado para efectuar la operación.

### El sufijo `U` no evita el underflow

La siguiente expresión sigue siendo peligrosa cuando `i == 0`:

```c
(i - 1U) / 2U
```

Los enteros sin signo no representan números negativos. Si `i` vale cero, la resta se ajusta módulo `SIZE_MAX + 1`:

```text
0 - 1 -> SIZE_MAX
```

Por eso la precondición de calcular el padre es:

```text
i > 0
```

Escribir `1` en vez de `1U` tampoco arreglaría el problema: al combinarse con un `size_t`, el `int` se convertiría normalmente al tipo sin signo de `i`.

## 2. Macros del preprocesador

En `c11/indices.h` aparece:

```c
#define HEAP_PARENT(i) (((i) - 1U) / 2U)
```

Es una **macro con parámetros**. No es una función. Antes de que el compilador analice el programa, el preprocesador sustituye textualmente cada uso.

Por ejemplo:

```c
size_t p = HEAP_PARENT(index);
```

se transforma aproximadamente en:

```c
size_t p = (((index) - 1U) / 2U);
```

### Por qué hay tantos paréntesis

Cada parámetro debe encerrarse entre paréntesis y la expresión completa también:

```c
#define HEAP_PARENT(i) (((i) - 1U) / 2U)
```

Esto evita que la precedencia de los operadores cambie el significado al sustituir una expresión compleja.

Una macro mal escrita sería:

```c
#define BAD_PARENT(i) i - 1U / 2U
```

Como la división tiene mayor precedencia que la resta, no calcularía `(i - 1) / 2`.

### Limitaciones de una macro

Una macro:

- No tiene firma ni tipo de retorno.
- No comprueba el tipo del argumento por sí misma.
- Produce diagnósticos menos claros en expresiones complejas.
- No es una función visible como tal para el enlazador.
- No tiene una dirección de función que pueda almacenarse en un puntero.
- Puede evaluar varias veces un argumento si el parámetro aparece repetido.

Esta macro concreta usa `i` una sola vez, por lo que no repite su evaluación. Aun así, conviene evitar argumentos con efectos laterales:

```c
HEAP_PARENT(i++); /* legal, pero poco claro */
```

El peligro de evaluación repetida se observa mejor en:

```c
#define MAX_MACRO(a, b) ((a) > (b) ? (a) : (b))
```

Aquí `a` aparece dos veces. Si se llama:

```c
MAX_MACRO(a++, b)
```

`a++` puede evaluarse una vez en la condición y otra vez en el resultado seleccionado.

### Otras formas de calcular el padre

Una función interna del encabezado:

```c
static inline size_t heap_parent(size_t i) {
  return (i - 1U) / 2U;
}
```

Una función externa declarada en el encabezado:

```c
/* indices.h */
size_t heap_parent(size_t i);
```

```c
/* indices.c */
size_t heap_parent(size_t i) {
  return (i - 1U) / 2U;
}
```

Una interfaz que representa explícitamente el error de pedir el padre de la raíz:

```c
static inline bool heap_parent(size_t i, size_t *result) {
  if (i == 0U || result == NULL)
    return false;

  *result = (i - 1U) / 2U;
  return true;
}
```

Esta última forma convierte la precondición `i > 0` en una validación de la propia interfaz.

En C11 también existe `_Generic` para construir macros que seleccionan funciones según el tipo, pero no es necesario para los índices del heap.

## 3. Significados de `static` en C

`static` no tiene un único significado. Su efecto depende del lugar donde se utiliza.

### 3.1. Función `static` a nivel de archivo

En `c11/heap.c` se declara:

```c
static void sift_down(HeapC *h, size_t i) {
  /* ... */
}
```

Aquí `static` concede **enlace interno**. La función solo puede ser nombrada desde la unidad de traducción donde está definida, en este caso `heap.c` después del preprocesamiento.

Esto es apropiado porque `sift_down` es un detalle privado de implementación. Los usuarios de `heap.h` operan mediante `heap_build` y `heap_pop`, no llamando directamente a `sift_down`.

Además, otro archivo podría tener su propia función `static sift_down` sin producir un conflicto durante el enlazado.

### 3.2. Variable global `static`

```c
static unsigned operation_count = 0U;
```

Una variable definida fuera de funciones con `static`:

- Existe durante toda la ejecución del programa.
- Tiene enlace interno.
- Solo puede nombrarse desde ese archivo.

Sin `static`, una variable global normalmente tiene enlace externo y podría compartirse entre archivos mediante `extern`.

### 3.3. Variable local `static`

```c
void count_call(void) {
  static unsigned calls = 0U;
  calls++;
}
```

Aunque `calls` solo es visible dentro de la función, su valor no se crea de nuevo en cada llamada. Tiene duración de almacenamiento estática y conserva su valor hasta terminar el programa.

No debe confundirse:

```text
Alcance: dónde puede escribirse el nombre.
Duración: cuánto tiempo existe el objeto.
Enlace: si el mismo nombre puede referirse a la entidad desde otros archivos.
```

### 3.4. `static` en parámetros de arreglo

C99 y versiones posteriores permiten:

```c
void process(size_t n, int values[static n]);
```

En este contexto, `static n` expresa que el llamador debe proporcionar acceso a por lo menos `n` elementos. No crea una variable persistente ni concede enlace interno. Es una promesa usada para expresar el contrato y permitir optimizaciones.

### 3.5. Lo que no significa en C

C no tiene miembros `static` de clases porque no tiene clases. Ese uso pertenece a C++ y tiene reglas diferentes.

## 4. Qué significa `inline`

Una función `inline` se declara como adecuada para que el compilador sustituya una llamada por el cuerpo de la función:

```c
static inline size_t parent_inline(size_t i) {
  return (i - 1U) / 2U;
}
```

Conceptualmente, esta llamada:

```c
size_t p = parent_inline(i);
```

podría convertirse en código equivalente a:

```c
size_t p = (i - 1U) / 2U;
```

### `inline` no es una orden de optimización

El compilador puede decidir:

- Expandir una función declarada `inline`.
- Mantener una llamada normal a esa función.
- Expandir una función que no fue declarada `inline`.

La decisión depende, entre otras cosas, de la optimización, el tamaño de la función y el contexto. Por tanto, `inline` no garantiza mayor velocidad.

En C, `inline` también participa en las reglas de definiciones y enlace. Esas reglas son más delicadas que una simple sugerencia de rendimiento.

### Por qué se usa `static inline` en un encabezado

Si una función pequeña se define en un `.h` incluido por varios archivos, `static` da a cada unidad de traducción su propia función con enlace interno:

```c
static inline size_t parent_inline(size_t i) {
  return (i - 1U) / 2U;
}
```

Esto evita errores de definiciones múltiples durante el enlazado y permite que el cuerpo esté disponible para optimización en cada archivo.

Para ayudantes pequeños en encabezados C, `static inline` suele ser más sencillo que usar `inline` solo, cuyas reglas pueden requerir además una definición externa.

## 5. Macro frente a función `static inline`

Se comparan estas dos formas:

```c
#define HEAP_PARENT(i) (((i) - 1U) / 2U)
```

```c
static inline size_t parent_inline(size_t i) {
  return (i - 1U) / 2U;
}
```

| Aspecto | Macro | `static inline` |
|---|---|---|
| Momento de procesamiento | Sustitución textual del preprocesador | Análisis como función por el compilador |
| Tipo del parámetro | No tiene | `size_t` |
| Tipo del resultado | Depende de la expresión sustituida | `size_t` |
| Evaluación del argumento | Depende de cuántas veces aparezca el parámetro | Exactamente una vez |
| Comprobación de tipos | Limitada al resultado de la expansión | Sí, según la firma |
| Dirección de función | No existe | Puede obtenerse |
| Depuración | Se observa el código expandido | Existe semánticamente como función |
| Uso en expresión constante de C11 | Puede funcionar si toda la expansión es constante | Una llamada no es una expresión constante entera de C11 |
| Enlace | No aplica como función | Interno por `static` |
| Optimización | Puede producir código directo | Normalmente produce el mismo código optimizado |

### Evaluación de argumentos

La diferencia general más importante se ve con macros que repiten parámetros:

```c
#define SQUARE(x) ((x) * (x))
```

```c
SQUARE(i++); /* i++ aparece dos veces tras la expansión */
```

Una función evalúa el argumento antes de entrar y recibe el valor resultante una sola vez:

```c
static inline int square(int x) {
  return x * x;
}
```

```c
square(i++); /* i aumenta una sola vez */
```

`HEAP_PARENT` usa su parámetro una sola vez, así que no presenta concretamente esa repetición. La función sigue aportando tipos, alcance y mejores diagnósticos.

### Rendimiento

Con optimización, ambas formas suelen generar las mismas instrucciones para un cálculo tan pequeño. El laboratorio propone `make asm` precisamente para inspeccionarlo.

No debe concluirse que una macro es más rápida solo porque el preprocesador la sustituye textualmente. El optimizador puede eliminar por completo la llamada a `parent_inline`.

## 6. Precondiciones que ninguna forma elimina

Tanto la macro como la función necesitan:

```text
i > 0
```

Estas dos expresiones tienen el mismo underflow si `i == 0`:

```c
HEAP_PARENT(0U);
parent_inline(0U);
```

La función mejora la seguridad de tipos, pero no valida automáticamente el contrato. Para hacerlo debe añadirse una comprobación o diseñarse una interfaz que pueda comunicar el error.

También hay que evitar calcular hijos con índices arbitrariamente grandes:

```c
2U * i + 1U
```

En `sift_down`, una forma segura de comprobar primero que existe un hijo izquierdo es:

```c
while (i < h->size / 2U) {
  const size_t left = 2U * i + 1U;
  /* ... */
}
```

La condición demuestra que `i` es un nodo interno antes de efectuar el cálculo.

## 7. Ideas centrales

1. `U` indica que un literal tiene una variante entera sin signo; no evita por sí solo underflow ni overflow.
2. Una macro con parámetros es sustitución textual, no una función.
3. Los paréntesis de una macro protegen contra problemas de precedencia, pero no proporcionan tipos.
4. Una función `static` a nivel de archivo es privada para esa unidad de traducción.
5. Una variable local `static` conserva su valor entre llamadas.
6. `inline` no obliga al compilador a eliminar una llamada ni garantiza mejor rendimiento.
7. `static inline` es una forma práctica de definir ayudantes pequeños y tipados en encabezados C.
8. Una función evalúa cada argumento una vez; una macro puede repetirlo si el parámetro aparece varias veces.
9. Macro y función pueden generar exactamente el mismo código máquina optimizado.
10. Tanto `HEAP_PARENT` como `parent_inline` requieren `i > 0`.
