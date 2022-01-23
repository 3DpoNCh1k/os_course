#include <cassert>
#include <iostream>

#include "../elf_entry.h"

void test_1() {
  int rv = system("make -f tests/Makefile");
  if (!WIFEXITED(rv)) {
    std::cout << "error" << std::endl;
    assert(false);
  }
  int status = WEXITSTATUS(rv);
  std::cout << "status = " << status << std::endl;
  assert(status == 0);
  auto ptr = entry_point("unit_executable");
  std::cout << "entry point address = " << ptr << std::endl;
}

void test_2() {
  auto ptr = entry_point("tests/test_example");
  assert(ptr == 0x10c0);
}

int main() {
  test_1();
  std::cout << "TEST PASSED!" << std::endl;
  test_2();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
