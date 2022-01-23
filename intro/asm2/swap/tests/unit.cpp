#include <cassert>
#include <cstdint>
#include <iostream>

#include "../swap.h"

void test_1() {
  int64_t a = 2;
  int64_t b = 5;
  swap(&a, &b);
  assert(a == 5);
  assert(b == 2);
}

int main() {
  test_1();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
