# Práctica 1 — Stack, Queue, correctitud y costo

## Requisitos

- C++20
- GNU Make
- doctest

En Ubuntu/Debian:

```bash
sudo apt update
sudo apt install g++ make doctest-dev
```

## Flujo de trabajo recomendado

```bash
make test
```

Compila y ejecuta las pruebas con AddressSanitizer y UndefinedBehaviorSanitizer.

Cuando todas las pruebas pasen:

```bash
make bench
```

Ejecuta los experimentos de rendimiento con optimización `-O2`.

También puede usarse:

```bash
make run
```

que ejecuta primero los tests y, solo si pasan, continúa con el benchmark.

## Archivos que debes implementar

- `include/stack_raw.hpp`
- `include/queue_shift.hpp`
- `include/queue_circular.hpp`
- `include/bounds.hpp`

`include/stack_vector.hpp` se proporciona completo como referencia de una implementación basada en STL y regla de cero.

## Importante

- No modifiques `tests/` para hacer pasar una implementación incorrecta.
- `top`, `front` y `pop` sobre una estructura vacía deben lanzar `std::out_of_range`.
- Las clases con memoria manual deben dejar el objeto origen vacío después de un movimiento.
- Las pruebas aleatorias usan STL como oráculo.
- El benchmark se ejecuta solo después de superar las pruebas.
