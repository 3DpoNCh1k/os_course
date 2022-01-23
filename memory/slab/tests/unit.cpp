#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <algorithm>

#include "../slab.h"

void* alloc_slab(int order) {
  assert(order >= 0 && order <= 10);
  std::size_t size = PAGE_SIZE * (1 << order);
  char* p = new (std::align_val_t(size)) char[size];
  assert((std::size_t)p % size == 0);
  std::cout << "memory pointer = " << std::size_t(p) << std::endl;
  return p;
}

void free_slab(void* slab) {
  delete[](char*) slab;
}

void test() {
  cache my_cache;
  cache* cache_p = &my_cache;
  std::size_t OBJECT_SIZE = 100;
  cache_setup(cache_p, OBJECT_SIZE);

  char* p = (char*)cache_alloc(cache_p);
  assert(p != nullptr);
  cache_free(cache_p, p);
  cache_release(cache_p);
}

void test_2() {
  std::mt19937 mt_rng(
      std::chrono::steady_clock::now().time_since_epoch().count());
  cache my_cache;
  cache* cache_p = &my_cache;
  std::size_t OBJECT_SIZE = PAGE_SIZE / 2;
  cache_setup(cache_p, OBJECT_SIZE);

  std::vector<char*> pointers;
  for (int rep = 0; rep < 2; ++rep) {
    for (int i = 0; i < 10; ++i) {
      char* p = (char*)cache_alloc(cache_p);
      assert(p != nullptr);
      pointers.push_back(p);
    }
    //        std::reverse(pointers.begin(), pointers.end());
    std::shuffle(pointers.begin(), pointers.end(), mt_rng);
    for (auto p : pointers) {
      cache_free(cache_p, p);
    }
    cache_shrink(cache_p);
  }
  cache_release(cache_p);
}

int main() {
  std::cout << "size of CellNode" << sizeof(CellNode) << "  sizeof Slab "
            << sizeof(SlabNode) << "  sizeof cache " << sizeof(cache)
            << std::endl;
  //    test();
  std::cout << "TEST 1 PASSED!" << std::endl;
  test_2();
  std::cout << "TEST 2 PASSED!" << std::endl;
  return 0;
}
