#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <algorithm>

#include "../helpers.h"
#include "../atomic.h"

void test_add() {
  int T = 10;
  int REP = 1000;
  atomic_int atom(0);
  auto inc = [&]() {
    for (int rep = 0; rep < REP; ++rep) {
      atomic_fetch_add(&atom, 1);
    }
  };
  std::vector<std::thread> threads;
  for (int i = 0; i < T; ++i) {
    threads.emplace_back(inc);
  }
  for (int i = 0; i < T; ++i) {
    threads[i].join();
  }
  std::cout << atom.atom << std::endl;
  assert(atom.atom == REP * T);
}

void test_exchange() {
  int T = 10;
  int REP = 50;
  atomic_int atom(0);
  std::vector<std::vector<int>> values(T, std::vector<int>(REP));
  std::vector<int> a(T * REP, 0);
  //    iota(a.begin(), a.end(), 0);

  //    int VALUE = 9;
  //    for(int i = 0; i < T * REP; ++i) {
  //        a[i] = rand() % VALUE;
  //    }

  std::random_shuffle(a.begin(), a.end());

  for (int i = 0; i < T; ++i) {
    for (int rep = 0; rep < REP; ++rep) {
      values[i][rep] = a[i * REP + rep];
    }
  }
  std::atomic<int> steps = 0;
  auto func = [&](int i) {
    for (int rep = 0; rep < REP; ++rep) {
      int expected = values[i][rep];
      int value = expected;
      assert(value == 0);
      if (atomic_compare_exchange(&atom, &value, 1)) {
        steps.fetch_add(1);
        break;
      }
    }
  };
  std::vector<std::thread> threads;
  for (int i = 0; i < T; ++i) {
    threads.emplace_back(func, i);
  }
  for (int i = 0; i < T; ++i) {
    threads[i].join();
  }
  std::cout << atom.atom << std::endl;
  std::cout << steps << std::endl;
  assert(atom.atom == 1);
  assert(atom.atom == steps);
}

int main() {
  //    test_add();
  std::cout << "TEST PASSED!" << std::endl;
  test_exchange();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
