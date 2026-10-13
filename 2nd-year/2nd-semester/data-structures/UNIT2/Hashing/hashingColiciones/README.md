# Laboratorio: diccionarios y colisiones en C++

Requisitos: g++ compatible con C++17, make y, para depurar, GDB.

- `make`: compila los dos ejecutables.
- `make test-chain`: ejecuta las pruebas de la versión completa.
- `make test-linear`: inicialmente falla en la primera inserción. Es intencional: hay dos metodos pendientes.

Completar `LinearDictionary::get`, `LinearDictionary::put`, ampliar `common_tests` y `capacity_tests` según las actividades del laboratorio. No cambiar las firmas ni usar contenedores asociativos de biblioteca para resolver la actividad. No se implementa eliminación individual.
