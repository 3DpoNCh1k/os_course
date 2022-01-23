#pragma once

#include <mutex>
#include <unordered_set>
#include <thread>
#include <cassert>
#include <chrono>

// my atomic_int for fast testing

enum OPERATION {
  LOAD,
  STORE,
};

struct atomic_int {
  explicit atomic_int(int value) : atom(value){};
  int atom;
  std::unordered_set<std::thread::id> read_set;
  std::mutex mtx;
};