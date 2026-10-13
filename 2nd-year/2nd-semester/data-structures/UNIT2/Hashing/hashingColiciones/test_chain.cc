#include "dictionary.h"
#include "common_tests.h"

int main(){
  common_tests<Dictionary>();
  // Prueba del destructor con nodos aun presentes.
  {
    Dictionary d;
    check(d.put(10, 80), "Preparar prueba del destructor");
    check(d.put(17, 90), "Agregar un nodo que colisiona");
  }
}
