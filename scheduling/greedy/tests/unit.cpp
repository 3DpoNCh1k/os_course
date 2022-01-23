#include <cassert>
#include <iostream>
#include <vector>

#include "../greedy.h"

void test() {
  std::vector<int> times = {9, 7, 3};
  std::vector<int> expected = {2, 1, 0};
  assert(schedule(times) == expected);
}

int main() {
  test();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
