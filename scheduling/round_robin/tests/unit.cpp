#include <cassert>
#include <iostream>
#include <queue>

#include "../round_robin.h"

void sleep(int t) {
  while (t--) timer_tick();
}

void test() {
  int T = 5;
  scheduler_setup(T);
  assert(rest == 5);
  timer_tick();
  assert(rest == 5);
  assert(current_thread() == -1);
  new_thread(1);
  new_thread(2);
  sleep(5);
  assert(rest == 5);
  assert(current_thread() == 2);
  std::queue<int> expected({2, 1});
  assert(q == expected);
  new_thread(3);
  expected = std::queue<int>({2, 1, 3});
  assert(q == expected);
  sleep(3);
  exit_thread();
  assert(rest == 5);
  assert(current_thread() == 1);
  sleep(2);
  block_thread();
  assert(rest == 5);
  assert(current_thread() == 3);
  expected = std::queue<int>({3});
  assert(q == expected);
  new_thread(4);
  expected = std::queue<int>({3, 4});
  assert(q == expected);
  sleep(4);
  assert(rest == 1);
  assert(current_thread() == 3);
  wake_thread(1);
  expected = std::queue<int>({3, 4, 1});
  assert(q == expected);
  sleep(2);
  assert(rest == 4);
  assert(current_thread() == 4);
  expected = std::queue<int>({4, 1, 3});
  assert(q == expected);
  sleep(3);
  block_thread();
  sleep(1);
  assert(rest == 4);
  assert(current_thread() == 1);
  expected = std::queue<int>({1, 3});
  assert(q == expected);
  sleep(10);
  assert(rest == 4);
  assert(current_thread() == 1);
  expected = std::queue<int>({1, 3});
  assert(q == expected);
  exit_thread();
  sleep(6);
  assert(rest == 4);
  assert(current_thread() == 3);
  expected = std::queue<int>({3});
  assert(q == expected);
  block_thread();
  assert(rest == 5);
  assert(current_thread() == -1);
  expected = std::queue<int>();
  assert(q == expected);
}

int main() {
  test();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
