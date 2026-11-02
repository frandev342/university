#include "../c11/heap.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>

int main(void) {
  HeapC h;
  assert(heap_init(&h, 10U));
  assert(heap_valid(&h));
  Process output = {-1, -1};
  assert(!heap_pop(&h, &output));
  assert(output.pid == -1);
  Process init[] = {
    {0,4}, {1,1}, {2,3}, {3,2}, {4,16},
    {5,9}, {6,10}, {7,14}, {8,8}, {9,7}
  };
  assert(heap_build(&h, init, 10U));
  assert(h.size == 10U);
  assert(heap_valid(&h));
  assert(h.data[0].priority == 16);
  assert(!heap_push(&h, (Process){10, 100}));
  int previous = INT_MAX;
  for(size_t j = 0U; j < 10U; ++j) {
    assert(heap_pop(&h, &output));
    assert(output.priority <= previous);
    previous = output.priority;
    assert(heap_valid(&h));
  }
  assert(!heap_pop(&h, &output));
  assert(heap_push(&h, (Process){30, INT_MIN}));
  assert(heap_push(&h, (Process){31, INT_MAX}));
  assert(heap_push(&h, (Process){32, INT_MAX}));
  assert(heap_valid(&h));
  assert(heap_pop(&h, &output) && output.priority == INT_MAX);
  assert(heap_pop(&h, &output) && output.priority == INT_MAX);
  assert(heap_pop(&h, &output) && output.priority == INT_MIN);
  heap_destroy(&h);
  HeapC empty;
  assert(heap_init(&empty, 0U));
  assert(!heap_push(&empty, (Process){1, 1}));
  assert(heap_build(&empty, NULL, 0U));
  heap_destroy(&empty);
  HeapC stress;
  assert(heap_init(&stress, 64U));
  Process sample[64];
  for(size_t n = 0U; n <= 64U; ++n) {
    for(size_t i = 0U; i < n; ++i) {
      sample[i].pid = (int)i;
      sample[i].priority = (int)((i * 29U + n * 17U) % 41U) - 20;
    }
    assert(heap_build(&stress, sample, n) && heap_valid(&stress));
    int previous_priority = INT_MAX;
    for(size_t i = 0U; i < n; ++i) {
      assert(heap_pop(&stress, &output));
      assert(output.priority <= previous_priority);
      previous_priority = output.priority;
      assert(heap_valid(&stress));
    }
  }
  heap_destroy(&stress);
  puts("C11: OK");
  return 0;
}
