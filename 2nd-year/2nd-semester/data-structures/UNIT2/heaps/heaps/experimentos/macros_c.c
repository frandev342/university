#include <stdio.h>
#define MAX_MACRO(a, b) ((a) > (b) ? (a) : (b))
static inline int max_inline(int a, int b) { return a > b ? a : b; }
int main(void) {
  int a = 9;
  const int b = 3;
  // Sustituye textualmente los argumentos
  const int macro_result = MAX_MACRO(
      a++, b); // puede evaluar a++ dos veces ((a++) > (b) ? (a++) : (b)
  // Primero evalúa (a++) > b,  se produce el valor 9 y luego de incrementa.
  // Luego de eso queda en macro_result = 10++. Por lo que luego de esta
  // sentencia a valdrá 11
  printf("macro: valor=%d, a=%d\n", macro_result, a);
  a = 9;

  // EL valor enviado a la función es 9, una copia encima, por lo que el
  // inccremento no sucede dentro de la función Luego de la función sucede el
  // incremento, a = 10
  const int inline_result = max_inline(a++, b);
  printf("inline: valor=%d, a=%d\n", inline_result, a);
  puts("Explique la evaluacion repetida de la macro (sin UB en este caso).");
  return 0;
}

// hay un comando para macros y revisar como es exactamente
