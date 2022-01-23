#include <cassert>
#include <iostream>

#include "../elf_program_headers.h"

void test() {
  auto size = space("tests/test_example");
  assert(size == 4277);
}

int main() {
  test();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
