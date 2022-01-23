#include <cassert>
#include "paging.h"

PageTable::PageTable(uint64_t root_table,
                     const std::vector<MemoryCell>& cells) {
  root = root_table;
  for (auto [paddr, value] : cells) {
    memory[paddr] = value;
  }
}

uint64_t PageTable::get_memory_value(uint64_t paddr) {
  return memory.count(paddr) ? memory[paddr] : 0;
}

bool PageTable::is_used(uint64_t record) {
  return record & 1;
}

uint64_t PageTable::get_paddr(uint64_t record) {
  uint64_t BLOCK = (1UL << PADDR_SIZE) - 1;
  uint64_t SHIFT = PADDR_INDEX + 1 - PADDR_SIZE;
  return (record & (BLOCK << SHIFT)) >> SHIFT;
}

std::pair<uint64_t, bool> PageTable::map(uint64_t vaddr) {
  std::pair<uint64_t, bool> FAIL = {0, false};
  uint64_t current_paddr = root;
  for (int level = 0; level < N; ++level) {
    assert(current_paddr % 8 == 0);
    uint64_t BLOCK = (1UL << TABLE_SIZE) - 1;
    uint64_t SHIFT = indices[level] + 1 - TABLE_SIZE;
    uint64_t position_value = (vaddr & (BLOCK << SHIFT)) >> SHIFT;
    assert(position_value < 512);
    current_paddr += 8 * position_value;
    uint64_t record = get_memory_value(current_paddr);
    if (!is_used(record)) {
      return FAIL;
    }
    current_paddr = get_paddr(record) << OFFSET;
  }
  uint64_t offset = vaddr & ((1UL << OFFSET) - 1);
  return {current_paddr + offset, true};
}