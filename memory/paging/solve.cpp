#include <cassert>
#include <iostream>

#include "paging.h"

int main() {
  uint64_t r;
  int m, q;
  std::cin >> m >> q >> r;
  std::vector<MemoryCell> cells;
  for (int i = 0; i < m; ++i) {
    uint64_t paddr, value;
    std::cin >> paddr >> value;
    assert(paddr % 8 == 0);
    cells.emplace_back(paddr, value);
  }
  std::vector<uint64_t> queries(q);
  for (int i = 0; i < q; ++i) {
    std::cin >> queries[i];
  }
  PageTable pageTable(r, cells);
  for (int i = 0; i < q; ++i) {
    auto [paddr, ok] = pageTable.map(queries[i]);
    if (ok) {
      std::cout << paddr;
    } else {
      std::cout << "fault";
    }
    std::cout << std::endl;
  }
  return 0;
}
