#ifndef HEAP_INDICES_CPP11_HPP
#define HEAP_INDICES_CPP11_HPP
#include <cstddef>
namespace heap11 {
/* inline es redundante: constexpr ya lo implica para funciones. */
// constexpr significa que una función puede evaluarse durante la compilación
// cuando recibe valores conocidos por el compilador.
inline constexpr std::size_t parent(std::size_t i) {
  return (i - 1U) / 2U; // precondicion: i > 0
}
// constexpr std::size_t result = parent(9U);
// El compilador puede calcular directamente:
// parent(9) = (9 - 1) / 2 = 4
/* TODO: constexpr
puede utilizarse:
- Durante la compilación.
- Durante la ejecución.
- En static_assert.
- Para inicializar variables constexpr.
- En otros contextos que requieran expresiones constantes.
Es más flexible a diferencia de una función normal que solo
funciona en EJECUCIÓN.

Una expresión constante es una expresión cuyo resultado puede calcularse durante
la compilación.
Un rvalue puede no ser constante, y una expresión que usa un lvalue constexpr
puede ser constante.

OBLIGATORIO
- Debe tener un return
*/
/* TODO: inline
- Permite definir una función en un encabezado
- PUede expandirse en el lugar de la llamada
El compilador podría transformar:
std::size_t p = parent(i);
en algo equivalente a:
std::size_t p = (i - 1U) / 2U;
En este caso inline está dentro de constexpr, por eso no es necesario
escribirlo en este caso
*/

inline constexpr std::size_t left(std::size_t i) { return 2U * i + 1U; }
inline constexpr std::size_t right(std::size_t i) { return 2U * i + 2U; }

// static assert: comprobación realizada durante la compilación
/*
static_assert
static_assert(parent(9U) == 4U, "incorrect parent");
- Se evalúa durante la compilación.
- Su condición debe conocerse durante la compilación.
- Si falla, no se genera el ejecutable.
- No añade trabajo durante la ejecución.
- Es una característica propia del lenguaje.
assert
assert(heap.valid());
- Se evalúa durante la ejecución.
- Puede utilizar valores calculados en tiempo de ejecución.
- Si falla, el programa se detiene.
- Puede desactivarse mediante NDEBUG.
- Se obtiene con <cassert>.
*/
static_assert(parent(9U) == 4U, "padre incorrecto");
static_assert(left(4U) == 9U, "hijo izquierdo incorrecto");
static_assert(right(4U) == 10U, "hijo derecho incorrecto");
} // namespace heap11
//
/*
inline
Permite definir cómodamente una función en un encabezado y puede facilitar
su expansión, pero no garantiza una optimización.

constexpr
Permite que una función o valor se evalúe durante la compilación.
También puede utilizarse normalmente durante la ejecución.

inline constexpr
En C++11, inline es redundante porque constexpr ya implica inline.

static_assert
Comprueba una condición durante la compilación. Si es falsa, el programa
no compila.
*/
#endif
