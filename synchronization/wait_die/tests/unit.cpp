#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>
#include <sstream>

#include "../wdlock.h"

struct lock;
struct condition;

extern "C" struct lock* create_lock();
extern "C" void delete_lock(struct lock*);
extern "C" condition* create_condition();
extern "C" void delete_condition(condition* cv);

extern "C" void lock_init(struct lock*);
extern "C" void condition_init(struct condition*);

extern "C" wdlock* create_wdlock();
extern "C" void delete_wdlock(struct wdlock*);
extern "C" wdlock_ctx* create_wdlock_ctx();
extern "C" void delete_wdlock_ctx(struct wdlock_ctx*);

extern "C" void wdlock_init(struct wdlock* lock);
extern "C" void wdlock_ctx_init(struct wdlock_ctx* ctx);
extern "C" int wdlock_lock(struct wdlock* l, struct wdlock_ctx* ctx);
extern "C" void wdlock_unlock(struct wdlock_ctx* ctx);

void test_create_delete() {
  struct lock* lk = create_lock();
  lock_init(lk);
  delete_lock(lk);
  struct condition* cv = create_condition();
  condition_init(cv);
  delete_condition(cv);
  struct wdlock* wdlk = create_wdlock();
  wdlock_init(wdlk);
  delete_wdlock(wdlk);
  struct wdlock_ctx* wdlk_ctx = create_wdlock_ctx();
  wdlock_ctx_init(wdlk_ctx);
  delete_wdlock_ctx(wdlk_ctx);
}

void test() {
  // like globals
  wdlock* wdLockA = create_wdlock();
  wdlock_init(wdLockA);
  assert(wdLockA->next == nullptr);
  assert(wdLockA->owner == nullptr);
  wdlock* wdLockB = create_wdlock();
  wdlock_init(wdLockB);
  auto f = [&]() {
    wdlock_ctx* ctx = create_wdlock_ctx();
    wdlock_ctx_init(ctx);

    assert(wdlock_lock(wdLockA, ctx));

    assert(wdLockA->owner == ctx);
    assert(ctx->locks == wdLockA);
    assert(wdLockA->next == nullptr);
    assert(wdlock_lock(wdLockA, ctx));

    assert(wdLockA->owner == ctx);
    assert(ctx->locks == wdLockA);
    assert(wdLockA->next == nullptr);

    assert(wdlock_lock(wdLockB, ctx));

    assert(wdLockB->owner == ctx);
    assert(ctx->locks == wdLockB);
    assert(wdLockB->next == wdLockA);
    assert(wdLockA->next == nullptr);

    wdlock_unlock(ctx);
    assert(wdLockA->next == nullptr);
    assert(wdLockB->owner == nullptr);
    assert(wdLockB->next == nullptr);
    assert(wdLockB->owner == nullptr);
    assert(ctx->locks == nullptr);

    delete_wdlock_ctx(ctx);
  };
  f();

  delete_wdlock(wdLockA);
  delete_wdlock(wdLockB);
}

void test_stress() {
  // like globals
  std::vector<wdlock*> locks;

  int N = 50;
  int T = 10;
  for (int i = 0; i < N; ++i) {
    wdlock* wdLock = create_wdlock();
    wdlock_init(wdLock);
    locks.push_back(wdLock);
  }

  int n = locks.size();
  std::vector<int> order(n);
  std::iota(order.begin(), order.end(), 0);

  std::atomic<int> atom_orders = 0;

  std::vector<int> stats_k(n + 1, 0);
  std::vector<int> stats_id(T + 10, 0);

  std::atomic<int> atom_stats_k = 1;
  std::atomic<int> atom_stats_id = 1;

  auto update_stats_k = [&](int k) {
    int expected = 1;
    while (!atom_stats_k.compare_exchange_strong(expected, 0)) {
      expected = 1;
    }
    stats_k[k]++;
    atom_stats_k.store(1);
  };

  auto update_stats_id = [&](unsigned long long id) {
    int expected = 1;
    while (!atom_stats_id.compare_exchange_strong(expected, 0)) {
      expected = 1;
    }
    stats_id[id]++;
    atom_stats_id.store(1);
  };

  std::mt19937 mt_rng(
      std::chrono::steady_clock::now().time_since_epoch().count());
  //    auto randint = [&](int a, int b) {
  //        return std::uniform_int_distribution<>(a, b)(mt_rng);
  //    };

  std::atomic<int> atom_maybe = 1;

  auto maybe_yield = [&]() {
    //        int expected = 1;
    //        while(!atom_maybe.compare_exchange_strong(expected, 0)) {
    //            expected = 1;
    //        }
    //        auto value = randint(0, 1);
    //        atom_maybe.store(1);
    //        if(value == 0) {
    //            std::this_thread::yield();
    //        }
  };

  auto f = [&]() {
    std::thread::id this_id = std::this_thread::get_id();
    std::cout << this_id << std::endl;

    wdlock_ctx* ctx = create_wdlock_ctx();
    wdlock_ctx_init(ctx);

    int REP = 10000;
    for (int rep = 0; rep < REP; ++rep) {
      auto my_order = order;
      std::random_shuffle(my_order.begin(), my_order.end());
      std::vector<int> taken;
      int k = 0;
      for (int i = 0; i < n; ++i) {
        maybe_yield();
        int j = my_order[i];
        //                std::stringstream msg;
        //                msg << "rep=" << rep << "  i=" <<i << " ctx->timestamp
        //                = " << ctx->timestamp << " trying lock " << j;
        //                std::cout << msg.str() << std::endl;
        int res = wdlock_lock(locks[j], ctx);
        //                msg.clear();
        if (!res) {
          //                    msg << "rep=" << rep << "  i=" <<i << "
          //                    ctx->timestamp = " << ctx->timestamp << " failed
          //                    lock " << j; std::cout << msg.str() <<
          //                    std::endl;
          break;
        } else {
          //                    msg << "rep=" << rep << "  i=" <<i << "
          //                    ctx->timestamp = " << ctx->timestamp << " taken
          //                    lock " << j; std::cout << msg.str() <<
          //                    std::endl;
          taken.push_back(j);
          ++k;
        }
      }
      assert(k >= 0 && k <= n);
      if (k == n) {
        update_stats_id(ctx->timestamp);
      }

      update_stats_k(k);

      for (int i = 0; i < k; ++i) {
        int j = my_order[i];
        assert(taken[i] == j);
        if (locks[j]->owner != ctx) {
          std::cout << j << " " << locks[j]->owner << "  " << ctx << std::endl;
        }
        assert(locks[j]->owner == ctx);
      }
      wdlock_unlock(ctx);
    }

    //        auto timestamp =  ctx->timestamp;
    delete_wdlock_ctx(ctx);
    //        std::cout << "ctx->timestamp = " << timestamp << "  DONE !!! " <<
    //        std::endl;
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < T; ++i) {
    threads.emplace_back(f);
  }
  for (int i = 0; i < T; ++i) {
    threads[i].join();
  }

  for (int i = 0; i < n; ++i) {
    delete_wdlock(locks[i]);
  }

  std::cout << "stats_k" << std::endl;
  for (int i = 0; i <= n; ++i) {
    std::cout << stats_k[i] << " \n"[i == n];
  }

  std::cout << "stats_id" << std::endl;
  for (int i = 0; i < (int)stats_id.size(); ++i) {
    std::cout << stats_id[i] << " \n"[i == (int)stats_id.size() - 1];
  }
}

int main() {
  test_create_delete();
  std::cout << "TEST PASSED!" << std::endl;
  test();
  std::cout << "TEST PASSED!" << std::endl;
  test_stress();
  std::cout << "TEST PASSED!" << std::endl;
  return 0;
}
