#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <algorithm>

#include "../myalloc.h"

const std::size_t EXTRA_SIZE = 2 * sizeof(Node);

void test_small_alloc() {
  std::size_t ALLOCATED_SIZE = 1000;
  char* buf = new char[ALLOCATED_SIZE];
  mysetup(buf, ALLOCATED_SIZE);
  int n = 100;
  char* a = (char*)myalloc(n);
  for (int i = 0; i < n; ++i) {
    a[i] = i;
  }
}

void test_nullptr() {
  std::size_t ALLOCATED_SIZE = 1000;
  char* buf = new char[ALLOCATED_SIZE];
  mysetup(buf, ALLOCATED_SIZE);
  assert(myalloc(ALLOCATED_SIZE) == nullptr);
  void* p1 = myalloc(ALLOCATED_SIZE - EXTRA_SIZE * 2);
  assert(p1 != nullptr);
  myfree(p1);
  void* p2 = myalloc(400);
  void* p3 = myalloc(400);
  assert(p2 != nullptr);
  assert(p3 != nullptr);
  assert(myalloc(100) == nullptr);
}

void test_stress() {
  std::mt19937 mt_rng(
      std::chrono::steady_clock::now().time_since_epoch().count());
  auto randint = [&](int a, int b) {
    return std::uniform_int_distribution<>(a, b)(mt_rng);
  };
  std::size_t ALLOCATED_SIZE = 2'000'000;
  char* buf = new char[ALLOCATED_SIZE];
  mysetup(buf, ALLOCATED_SIZE);
  int cnt = 0;
  for (int i = 0; i < 300000; ++i) {
    std::size_t need_size = randint(16, 20'000);
    void* p = myalloc(need_size);
    bool ok = p != nullptr;
    // std::cout << "need_size = " << need_size << "  ok = " << ok << std::endl;
    if (ok) {
      Node* front = (Node*)((char*)p - sizeof(Node));
      assert(front->used);
      assert(!front->next && !front->prev);
      assert(front->size == need_size + EXTRA_SIZE);
      Node* back = (Node*)((char*)p + need_size);
      assert(*front == *back);

      if (randint(0, 1)) {
        myfree(p);
      }
    }
    cnt += ok;
  }
  std::cout << cnt << std::endl;
}

std::size_t get_max_size() {
  std::size_t L = 0, R = 1e9;
  while (L < R) {
    std::size_t M = (L + R + 1) / 2;
    void* p = myalloc(M);
    if (p) {
      L = M;
      myfree(p);
    } else {
      R = M - 1;
    }
  }
  return L;
}

void test_fragmentation() {
  std::mt19937 mt_rng(
      std::chrono::steady_clock::now().time_since_epoch().count());
  std::size_t ALLOCATED_SIZE = 2'000'000;
  char* buf = new char[ALLOCATED_SIZE];
  mysetup(buf, ALLOCATED_SIZE);
  assert(get_length() == 1);
  std::size_t MAX_SIZE = get_max_size();
  std::vector<std::size_t> blocks = {16, 96, 1008, 96, 16, 1008};
  for (int rep = 0; rep < 12; ++rep) {
    assert(MAX_SIZE == get_max_size());
    std::vector<void*> pointers;
    for (int i = 0; i < 300000; ++i) {
      std::size_t need_size = blocks[rep / 2];
      void* p = myalloc(need_size);
      bool ok = p != nullptr;
      //            std::cout << "need_size = " << need_size << "  ok = " << ok
      //            << std::endl;
      if (ok) {
        Node* front = (Node*)((char*)p - sizeof(Node));
        assert(front->used);
        assert(!front->next && !front->prev);
        assert(front->size == need_size + EXTRA_SIZE);
        Node* back = (Node*)((char*)p + need_size);
        assert(*front == *back);

        pointers.push_back(p);
      }
      assert(get_length() <= 1);
    }

    std::shuffle(pointers.begin(), pointers.end(), mt_rng);

    for (auto p : pointers) {
      assert(p != nullptr);
      myfree(p);
    }
    assert(get_length() == 1);
  }
}

int main() {
  std::cout << "size of Node = " << sizeof(Node) << std::endl;
  test_small_alloc();
  std::cout << "TEST 1 PASSED!" << std::endl;
  test_nullptr();
  std::cout << "TEST 2 PASSED!" << std::endl;
  test_stress();
  std::cout << "TEST 3 PASSED!" << std::endl;
  test_fragmentation();
  std::cout << "TEST 4 PASSED!" << std::endl;
  return 0;
}
