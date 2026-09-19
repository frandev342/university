import time
import random


def merge(left, right):
    result = []
    i = j = 0

    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            j += 1

    while i < len(left):
        result.append(left[i])
        i += 1

    while j < len(right):
        result.append(right[j])
        j += 1

    return result


def merge_sort(array):
    if len(array) <= 1:
        return array

    mid = len(array) // 2

    left = merge_sort(array[:mid])
    right = merge_sort(array[mid:])

    return merge(left, right)


sizes = [1000, 5000, 10000, 50000, 100000, 500000, 1000000]

for n in sizes:
    array = [random.randint(0, 1000000) for _ in range(n)]

    inicio = time.perf_counter()

    merge_sort(array)

    fin = time.perf_counter()

    tiempo = fin - inicio

    print(f"Tiempo de ejecución (n={n}): {tiempo:.6f} segundos")
