#include <cstdint>
#include <cstdio>

#include "elf_entry.h"

std::uintptr_t entry_point(const char* name) {
  struct elf_hdr header;
  FILE* file = fopen(name, "rb");
  fread(&header, sizeof(header), 1, file);
  return header.e_entry;
}