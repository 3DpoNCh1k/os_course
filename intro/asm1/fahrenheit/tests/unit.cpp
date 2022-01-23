#include <iostream>
#include <cassert>
#include "../fahrenheit.h"
#include "../../swap/swap.h"

void test_1() {
  __asm__ __volatile__("movq $40, %rsi;");
  int rsi = get_rsi();
  //    std::cout << "rsi = " << rsi << std::endl;
  assert(rsi == 40);
  // Repeat if they are overwritten yet
  __asm__ __volatile__("movq $40, %rsi;");
  fahrenheit_to_celcius();
  rsi = get_rsi();
  assert(rsi == 4);
}

int main() {
  test_1();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
