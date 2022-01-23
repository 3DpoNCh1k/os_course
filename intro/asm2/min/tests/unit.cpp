#include <cassert>
#include <iostream>

#include "../min.h"

void test_1() {
  int a = 2;
  int b = 5;
  int c = min(a, b);
  assert(c == 2);
}

int main() {
  test_1();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
