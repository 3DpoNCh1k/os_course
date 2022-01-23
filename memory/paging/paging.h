#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <utility>

struct MemoryCell {
  MemoryCell(uint64_t paddr, uint64_t value) : paddr(paddr), value(value){};
  uint64_t paddr;
  uint64_t value;
};

struct PageTable {
  PageTable(uint64_t root_table, const std::vector<MemoryCell>& cells);
  std::pair<uint64_t, bool> map(uint64_t vaddr);
  uint64_t get_memory_value(uint64_t paddr);
  uint64_t root;
  static const int N = 4;
  static const int OFFSET = 12;
  static const int TABLE_SIZE = 9;
  std::vector<int> indices = {47, 38, 29, 20};
  std::map<uint64_t, uint64_t> memory;

  static bool is_used(uint64_t record);
  static const int PADDR_INDEX = 51;
  static const int PADDR_SIZE = 40;
  static uint64_t get_paddr(uint64_t record);
};