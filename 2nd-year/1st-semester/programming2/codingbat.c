#include <stdio.h>

// EJERCICIOS DE CODING BAT (RECURSION 2)

#define MAX_LENGHT 100

int count11(char *S) {
  if (*S == '\0' || *(S + 1) == '\0')
    return 0;
  if (*S == '1' && *(S + 1) == '1') {
    return 1 + count11(S + 2);
  }
  return count11(++S);
}

void parenBitRec(char *S, char *new, int into) {
  if (*S == '\0') {
    *new = '\0';
    return;
  }
  if (*S == '(') {
    *new = *S;
    parenBitRec(++S, ++new, 1);
  }
  if (into) {
    *new = *S;
    if (*S == ')') {
      *(new + 1) = '\0';
      return;
    }
    parenBitRec(++S, ++new, 1);
    return;
  }
  parenBitRec(++S, new, 0);
}

void parenBit(char *S, char *new) { parenBitRec(S, new, 0); }

int main() {
  char S[MAX_LENGHT];
  char new[MAX_LENGHT];
  int c, i = 0;
  while ((c = getchar()) != '\n') {
    *(S + i) = c;
    i++;
  }

  *(S + i) = '\0';
  parenBit(S, new);
  printf("%s\n", new);
  return 0;
}
