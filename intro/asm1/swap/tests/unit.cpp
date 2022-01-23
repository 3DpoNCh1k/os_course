#include <iostream>
#include <cassert>
#include "../swap.h"

void test_1() {
  __asm__ __volatile__(
      "movq $5, %rsi;"
      "movq $2, %rdx;");
  int rsi = get_rsi();
  int rdx = get_rdx();
  //    std::cout << "rsi = " << rsi << std::endl;
  //    std::cout << "rdx = " << rdx << std::endl;
  assert(rsi == 5);
  assert(rdx == 2);
  // Repeat if they are overwritten yet
  __asm__ __volatile__(
      "movq $5, %rsi;"
      "movq $2, %rdx;");
  rsi_rdx_swap();
  rsi = get_rsi();
  rdx = get_rdx();
  assert(rsi == 2);
  assert(rdx == 5);
}

int main() {
  test_1();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
