# Comandos disponibles con `make`

Ejecutar desde `UNIT2/red-black-trees/`. Requiere `gcc`, `g++` y `make`.

| Comando | Qué hace |
|---|---|
| `make` o `make all` | Compila los cuatro proyectos. |
| `make c` | Compila la implementación iterativa C11. |
| `make cpp` | Compila la implementación iterativa C++11. |
| `make c_recursive` | Compila la implementación recursiva C11. |
| `make cpp_recursive` | Compila la implementación recursiva C++11. |
| `make test` | Ejecuta las cuatro suites de pruebas. |
| `make test-c` | Ejecuta solo las pruebas de `c/`. |
| `make test-cpp` | Ejecuta solo las pruebas de `cpp/`. |
| `make test-c-recursive` | Ejecuta solo las pruebas de `c_recursive/`. |
| `make test-cpp-recursive` | Ejecuta solo las pruebas de `cpp_recursive/`. |
| `make sanitize` | Compila y ejecuta las cuatro suites con ASan y UBSan. |
| `make sanitize-c` | Sanitizadores solo para `c/`. |
| `make sanitize-cpp` | Sanitizadores solo para `cpp/`. |
| `make sanitize-c-recursive` | Sanitizadores solo para `c_recursive/`. |
| `make sanitize-cpp-recursive` | Sanitizadores solo para `cpp_recursive/`. |
| `make rebuild` | Elimina `build/` y compila las cuatro versiones de nuevo. |
| `make clean` | Elimina solo `build/` y sus ejecutables. |
| `make help` | Muestra un resumen de objetivos en la terminal. |

Los programas normales se generan en `build/c`, `build/cpp`,
`build/c_recursive` y `build/cpp_recursive`. Por ejemplo:

```bash
make test-c-recursive
./build/c_recursive
```

Los sanitizados terminan en `_sanitize`, como `build/c_recursive_sanitize`.
Las cuatro compilaciones usan C11/C++11 y `-Wall -Wextra -Wpedantic
-Wconversion -O0 -g`. Los sanitizadores añaden
`-fsanitize=address,undefined -fno-omit-frame-pointer`.

Los programas ejecutan las pruebas y muestran el ejemplo de inserción de
Cormen antes de indicar `OK`. El directorio `build/` está ignorado por Git.
