#include <stddef.h>
#include <stdatomic.h>
#include <assert.h>
#include <malloc.h>
#include <limits.h>
#include "wait_die.h"
#include "wdlock.h"

struct lock* create_lock();
void delete_lock(struct lock*);
struct condition* create_condition();
void delete_condition(struct condition* cv);

struct wdlock* create_wdlock() {
  struct wdlock* p = malloc(sizeof(struct wdlock));
  p->lock = create_lock();
  p->unlocked = create_condition();
  p->owner = NULL;
  p->next = NULL;
  return p;
}
void delete_wdlock(struct wdlock* wdlock) {
  assert(wdlock);
  delete_lock(wdlock->lock);
  delete_condition(wdlock->unlocked);
  assert(wdlock);
  free(wdlock);
}
struct wdlock_ctx* create_wdlock_ctx() {
  struct wdlock_ctx* p = malloc(sizeof(struct wdlock_ctx));
  p->locks = NULL;
  p->timestamp = 0;
  return p;
}
void delete_wdlock_ctx(struct wdlock_ctx* wdlock_ctx) {
  assert(wdlock_ctx);
  struct wdlock* head = wdlock_ctx->locks;
  while (head) {
    assert(head != NULL);
    struct wdlock* nxt = head->next;
    delete_wdlock(head);
    head = nxt;
  }
  assert(wdlock_ctx);
  free(wdlock_ctx);
}

void wdlock_ctx_init(struct wdlock_ctx* ctx) {
  static atomic_ullong next = 0;
  ctx->timestamp = atomic_fetch_add(&next, 1) + 1;
  ctx->locks = NULL;
}

// for stepik
// struct lock * create_lock() {
//    return malloc(sizeof(struct lock));
//}
// struct condition * create_condition() {
//    return malloc(sizeof(struct condition));
//}

void wdlock_init(struct wdlock* lock) {
  lock->lock = create_lock();
  lock->unlocked = create_condition();
  lock_init(lock->lock);
  condition_init(lock->unlocked);
  lock->owner = NULL;
  lock->next = NULL;
}

int wdlock_lock(struct wdlock* lk, struct wdlock_ctx* ctx) {
  assert(lk != NULL);
  assert(ctx != NULL);
  int ret = -1;
  lock(lk->lock);
  while (1) {
    if (lk->owner == ctx) {
      ret = 1;
      break;
    } else if (lk->owner == NULL) {
      assert(lk->next == NULL);
      lk->owner = ctx;
      lk->next = ctx->locks;
      ctx->locks = lk;
      ret = 1;
      break;
    } else if (lk->owner->timestamp < ctx->timestamp) {
      ret = 0;
      break;
    } else {
      assert(lk->owner->timestamp > ctx->timestamp);
      wait(lk->unlocked, lk->lock);
    }
  }
  assert(ret != -1);
  unlock(lk->lock);
  return ret;
}

void wdlock_unlock(struct wdlock_ctx* ctx) {
  struct wdlock* head = ctx->locks;
  while (head) {
    lock(head->lock);
    assert(head->owner == ctx);
    head->owner = NULL;
    struct wdlock* nxt = head->next;
    head->next = NULL;
    notify_all(head->unlocked);
    unlock(head->lock);
    head = nxt;
  }
  ctx->locks = NULL;
}
