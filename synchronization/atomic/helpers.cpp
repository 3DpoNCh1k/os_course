#include <random>
#include "helpers.h"

std::atomic<int> atom_rnd = 1;

int randint(int a, int b) {
  int expected = 1;
  while (!atom_rnd.compare_exchange_strong(expected, 0)) {
    expected = 1;
  }
  static std::mt19937 mt_rng(
      std::chrono::steady_clock::now().time_since_epoch().count());
  auto rv = std::uniform_int_distribution<>(a, b)(mt_rng);
  atom_rnd.store(1);
  return rv;
};

void maybe_yield() {
  std::chrono::microseconds sleep_time(randint(1, 100));
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + sleep_time;
  do {
    std::this_thread::yield();
  } while (std::chrono::high_resolution_clock::now() < end);
}

/* load_linked читает значение из ячейки памяти, на которую
   указывает x, и возвращает прочитанное значение. */
int load_linked(atomic_int* x) {
  int value;
  {
    std::lock_guard guard(x->mtx);
    value = x->atom;
    std::thread::id who = std::this_thread::get_id();
    x->read_set.insert(who);
  }
  maybe_yield();
  return value;

  // for testing
  //  int value = x->atom;
  //  maybe_yield();
  //  return value;
}

/* store_conditional сохраняет значение new_value в ячейку
   памяти, на которую указывает x, но только при выполнении
   двух условий:
     - ячейка памяти ранее была прочитана с помощью load_linked
     - между последним load_linked для этой ячейки памяти и
       вызовом store_conditional никто не пытался записать
       значение в ячейку памяти
   Функция возвращает true, если значение было успешно записано
   и false в противном случае. */
bool store_conditional(atomic_int* x, int new_value) {
  bool value = false;
  {
    std::lock_guard guard(x->mtx);
    std::thread::id who = std::this_thread::get_id();
    if (x->read_set.count(who)) {
      x->atom = new_value;
      x->read_set.clear();
      value = true;
    }
  }
  maybe_yield();
  return value;

  // for testing
  //  x->atom = new_value;
  //  maybe_yield();
  //  return true;
}
