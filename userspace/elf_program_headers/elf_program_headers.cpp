#include <cstdint>
#include <cstdio>
#include <cassert>
#include <iostream>

#include "elf_program_headers.h"

std::size_t space(const char* name) {
  FILE* f = fopen(name, "rb");
  elf_hdr header;
  fread(&header, sizeof(header), 1, f);
  auto offset = header.e_phoff;
  auto cnt = header.e_phnum;
  auto size = header.e_phentsize;
  //    std::cout << &header << std::endl;
  //    std::cout << offset << " " << cnt << " " << size << std::endl;
  assert(offset >= sizeof(header));
  auto delta = offset - sizeof(header);
  char* p = new char[delta + cnt * size];
  fread(p, 1, delta + cnt * size, f);
  elf_phdr* program_header = (elf_phdr*)(p + delta);

  assert(size == sizeof(elf_phdr));
  std::size_t memsz = 0;

  //    std::cout << &program_header << std::endl;
  for (int i = 0; i < cnt; ++i) {
    memsz += program_header->p_type == PT_LOAD ? program_header->p_memsz : 0;
    program_header++;
  }
  delete[] p;
  return memsz;
}