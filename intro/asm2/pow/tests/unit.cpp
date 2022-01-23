#include <cassert>
#include <iostream>

#include "../pow.h"

void test_1() {
  assert(my_pow(2, 5) == 32);
  assert(my_pow(0, 5) == 0);
  assert(my_pow(2, 0) == 1);
  assert(my_pow(1, 55) == 1);
  assert(my_pow(132, 1) == 132);
  assert(my_pow(13, 7) == 62748517);
}

int main() {
  test_1();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
