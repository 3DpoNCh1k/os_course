#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include "../paging.h"

void test() {
  int q = 4;
  uint64_t r = 0;
  std::vector<MemoryCell> cells = {
      {0, 4097},
      {4096, 8193},
      {8192, 12289},
      {12288, 16385},
  };
  std::vector<uint64_t> queries = {0, 4096, 42, 131313};
  std::vector<std::pair<uint64_t, bool>> expected = {
      {16384, true},
      {0, false},
      {16426, true},
      {0, false},
  };
  PageTable pageTable(r, cells);
  for (int i = 0; i < q; ++i) {
    auto [paddr, ok] = pageTable.map(queries[i]);
    auto [expected_paddr, expected_ok] = expected[i];
    assert(ok == expected_ok && (!ok || paddr == expected_paddr));
  }
}

int main() {
  test();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
