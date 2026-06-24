# TurtleC

TurtleC es una biblioteca sencilla en C para dibujar con una tortuga usando CSFML. Está pensada para cursos introductorios de programación en C, especialmente para practicar funciones, punteros, structs, memoria dinámica y, más adelante, recursión.

La biblioteca está inspirada en el módulo `turtle` de Python y en la tradición educativa de los gráficos de tortuga. La documentación oficial de Python explica que `turtle` implementa herramientas geométricas introducidas originalmente en Logo. Logo fue desarrollado en 1967 por Wally Feurzeig, Seymour Papert y Cynthia Solomon. TurtleC no es una copia de Python `turtle`; es una pequeña biblioteca didáctica escrita en C con una interfaz similar en espíritu.

Referencias históricas:

- Python Software Foundation, documentación del módulo `turtle`: https://docs.python.org/3/library/turtle.html
- Cynthia Solomon, historia de Logo: https://logothings.github.io/logothings/

## Dependencias

En Ubuntu puede instalar las dependencias con:

```bash
sudo apt install gcc make libcsfml-dev
```

## Compilación

Desde el directorio del proyecto:

```bash
make
```

Esto genera el ejecutable:

```bash
./testLine
```

## Limpieza

```bash
make clean
```

## Ejemplo de uso

```c
#include "turtlec.h"

int main(void){
  TurtleApp *app = turtleAppCreate(400, 200, "Test Line");

  if(app == NULL)
    return 1;

  Turtle *t = turtleAppGetTurtle(app);

  turtlePenUp(t);
  turtleGoTo(t, 50.0f, 100.0f);
  turtlePenDown(t);

  turtleSetColor(t, 255, 100, 0);
  turtleSetSpeed(t, 5.0f);
  turtleForward(t, 300.0f);

  turtleAppRun(app);
  turtleAppDestroy(app);

  return 0;
}
```

## Modelo básico de uso

La biblioteca separa dos conceptos principales:

- `TurtleApp`: representa la aplicación gráfica completa. Contiene la ventana y una tortuga.
- `Turtle`: representa la tortuga. Tiene posición, orientación, color de trazo, estado del lápiz y la lista de líneas dibujadas.

El flujo usual de un programa es:

```c
TurtleApp *app = turtleAppCreate(400, 200, "Mi dibujo");
Turtle *t = turtleAppGetTurtle(app);

/* instrucciones de dibujo */

turtleAppRun(app);
turtleAppDestroy(app);
```

## Coordenadas y ángulos

El sistema de coordenadas usa píxeles:

- El origen `(0, 0)` está en la esquina superior izquierda de la ventana.
- El eje `x` crece hacia la derecha.
- El eje `y` crece hacia abajo.
- La tortuga inicia en el centro de la ventana.
- La orientación inicial es `0` grados, apuntando hacia la derecha.
- `turtleRight` aumenta el ángulo.
- `turtleLeft` disminuye el ángulo.

## API pública

Todas las funciones públicas están declaradas en `turtlec.h`.

### Tipos públicos

```c
typedef struct Turtle Turtle;
typedef struct TurtleApp TurtleApp;
typedef void (*TurtleDrawFunc)(Turtle *turtle);
```

#### `Turtle`

Representa una tortuga. Sus campos están definidos en el encabezado para mantener la biblioteca simple y fácil de estudiar, pero se recomienda manipularla mediante las funciones públicas.

#### `TurtleApp`

Representa una aplicación gráfica con una ventana y una tortuga asociada. Normalmente se crea con `turtleAppCreate` y se destruye con `turtleAppDestroy`.

#### `TurtleDrawFunc`

Tipo de función que recibe un puntero a `Turtle`. Se usa con `turtleRun` para escribir programas donde las instrucciones de dibujo se agrupan en una función.

Ejemplo:

```c
void drawSquare(Turtle *t){
  for(int i = 0; i < 4; i++){
    turtleForward(t, 100.0f);
    turtleRight(t, 90.0f);
  }
}
```

### Funciones de inicialización y destrucción de `Turtle`

Estas funciones son de bajo nivel. Para la mayoría de programas se recomienda usar `turtleAppCreate`, `turtleAppGetTurtle` y `turtleAppDestroy`.

#### `void turtleInit(Turtle *turtle, sfRenderWindow *window, unsigned int width, unsigned int height);`

Inicializa una tortuga usando una ventana de CSFML ya creada.

Parámetros:

- `turtle`: puntero a la tortuga que será inicializada.
- `window`: ventana de CSFML donde se dibujará.
- `width`: ancho de la ventana en píxeles.
- `height`: alto de la ventana en píxeles.

Después de llamar esta función, la tortuga queda en el centro de la ventana, con el lápiz abajo, color blanco y orientación inicial de `0` grados.

#### `void turtleDestroy(Turtle *turtle);`

Libera los recursos internos asociados a una tortuga.

Parámetros:

- `turtle`: puntero a la tortuga que se desea destruir.

No destruye la ventana de CSFML. Si la tortuga fue obtenida desde un `TurtleApp`, no llame directamente esta función; use `turtleAppDestroy`.

### Movimiento

#### `void turtleForward(Turtle *turtle, float dist);`

Avanza la tortuga en la dirección actual.

Parámetros:

- `turtle`: tortuga que se moverá.
- `dist`: distancia en píxeles.

Si el lápiz está abajo, dibuja una línea desde la posición anterior hasta la nueva posición. Si el lápiz está arriba, solo mueve la tortuga. Una distancia negativa produce un movimiento en sentido contrario.

#### `void turtleBackward(Turtle *turtle, float dist);`

Retrocede la tortuga respecto de su dirección actual.

Parámetros:

- `turtle`: tortuga que se moverá.
- `dist`: distancia en píxeles.

Internamente equivale a llamar `turtleForward` con la distancia negativa.

#### `void turtleGoTo(Turtle *turtle, float x, float y);`

Mueve la tortuga a una posición absoluta de la ventana.

Parámetros:

- `turtle`: tortuga que se moverá.
- `x`: coordenada horizontal de destino.
- `y`: coordenada vertical de destino.

Si el lápiz está abajo, dibuja una línea desde la posición actual hasta `(x, y)`. Si el lápiz está arriba, solo cambia la posición.

#### `void turtleHome(Turtle *turtle);`

Lleva la tortuga al centro de la ventana y restablece su orientación a `0` grados.

Parámetros:

- `turtle`: tortuga que será reposicionada.

Esta función no borra las líneas dibujadas previamente.

### Giro y orientación

#### `void turtleRight(Turtle *turtle, float angle);`

Gira la tortuga hacia la derecha.

Parámetros:

- `turtle`: tortuga que girará.
- `angle`: ángulo de giro en grados.

#### `void turtleLeft(Turtle *turtle, float angle);`

Gira la tortuga hacia la izquierda.

Parámetros:

- `turtle`: tortuga que girará.
- `angle`: ángulo de giro en grados.

### Lápiz y color

#### `void turtlePenUp(Turtle *turtle);`

Levanta el lápiz.

Parámetros:

- `turtle`: tortuga cuyo lápiz se levantará.

Después de llamar esta función, los movimientos no dibujan líneas.

#### `void turtlePenDown(Turtle *turtle);`

Baja el lápiz.

Parámetros:

- `turtle`: tortuga cuyo lápiz se bajará.

Después de llamar esta función, los movimientos vuelven a dibujar líneas.

#### `void turtleSetColor(Turtle *turtle, uint8_t r, uint8_t g, uint8_t b);`

Cambia el color del lápiz y de la figura que representa a la tortuga.

Parámetros:

- `turtle`: tortuga cuyo color se cambiará.
- `r`: componente roja, entre `0` y `255`.
- `g`: componente verde, entre `0` y `255`.
- `b`: componente azul, entre `0` y `255`.

Ejemplo:

```c
turtleSetColor(t, 255, 0, 0);
```

### Velocidad

#### `void turtleSetSpeed(Turtle *turtle, float speed);`

Cambia la velocidad visual de la tortuga.

Parámetros:

- `turtle`: tortuga cuya velocidad se cambiará.
- `speed`: valor positivo de velocidad.

Un valor mayor produce una animación más rápida. Si `speed` es menor o igual que cero, la biblioteca usa `1.0f` como valor por defecto.

### Dibujo de figuras simples

#### `void turtleCircle(Turtle *turtle, float radius);`

Dibuja una aproximación poligonal de un círculo.

Parámetros:

- `turtle`: tortuga que dibujará el círculo.
- `radius`: radio aproximado del círculo en píxeles.

La función aproxima el círculo usando varios pasos cortos de avance y giro.

### Limpieza y redibujado

#### `void turtleClear(Turtle *turtle);`

Borra todas las líneas dibujadas por la tortuga.

Parámetros:

- `turtle`: tortuga cuyo dibujo se limpiará.

No cambia la posición, orientación, color ni estado del lápiz.

#### `void turtleDraw(Turtle *turtle);`

Dibuja en la ventana las líneas acumuladas y la figura de la tortuga.

Parámetros:

- `turtle`: tortuga que será dibujada.

Normalmente no es necesario llamar esta función directamente, porque `turtleUpdateDisplay` y `turtleAppRun` la usan internamente.

#### `void turtleUpdateDisplay(Turtle *turtle);`

Actualiza la ventana gráfica después de una operación de dibujo o movimiento.

Parámetros:

- `turtle`: tortuga asociada a la ventana que se actualizará.

Procesa eventos básicos de la ventana, limpia la pantalla, redibuja las líneas, redibuja la tortuga y aplica una pausa según la velocidad configurada.

### Aplicación gráfica

#### `TurtleApp *turtleAppCreate(unsigned int width, unsigned int height, const char *title);`

Crea una aplicación gráfica con una ventana y una tortuga inicializada.

Parámetros:

- `width`: ancho de la ventana en píxeles.
- `height`: alto de la ventana en píxeles.
- `title`: título de la ventana.

Retorna:

- Un puntero a `TurtleApp` si la creación fue exitosa.
- `NULL` si no se pudo reservar memoria o crear la ventana.

La aplicación creada debe destruirse con `turtleAppDestroy`.

#### `Turtle *turtleAppGetTurtle(TurtleApp *app);`

Obtiene la tortuga asociada a una aplicación.

Parámetros:

- `app`: aplicación creada previamente con `turtleAppCreate`.

Retorna:

- Un puntero a la tortuga asociada a la aplicación.
- `NULL` si `app` es `NULL`.

#### `void turtleAppRun(TurtleApp *app);`

Mantiene abierta la ventana de la aplicación hasta que el usuario la cierre.

Parámetros:

- `app`: aplicación que se ejecutará.

Esta función debe llamarse después de las instrucciones de dibujo para que la ventana permanezca visible.

#### `void turtleAppDestroy(TurtleApp *app);`

Destruye la aplicación gráfica y libera sus recursos.

Parámetros:

- `app`: aplicación creada con `turtleAppCreate`.

Libera la tortuga interna, destruye la ventana y libera la memoria reservada para `TurtleApp`. Después de llamar esta función, no debe usarse nuevamente el puntero `app` ni la tortuga obtenida desde él.

### Ejecución simplificada

#### `void turtleRun(TurtleDrawFunc drawFunc, unsigned int width, unsigned int height, const char *title);`

Crea una aplicación, ejecuta una función de dibujo, mantiene la ventana abierta y libera los recursos al terminar.

Parámetros:

- `drawFunc`: función que recibe una tortuga y contiene las instrucciones de dibujo.
- `width`: ancho de la ventana en píxeles.
- `height`: alto de la ventana en píxeles.
- `title`: título de la ventana.

Ejemplo:

```c
#include "turtlec.h"

void drawSquare(Turtle *t){
  for(int i = 0; i < 4; i++){
    turtleForward(t, 100.0f);
    turtleRight(t, 90.0f);
  }
}

int main(void){
  turtleRun(drawSquare, 400, 400, "Square");
  return 0;
}
```

## Notas para estudiantes

- Cada función recibe explícitamente el puntero a la tortuga que modifica.
- Las funciones que crean recursos deben tener una función correspondiente que los libere.
- `turtleAppCreate` usa memoria dinámica, por eso siempre debe verificarse si retorna `NULL`.
- `turtleAppDestroy` debe llamarse antes de terminar el programa.
