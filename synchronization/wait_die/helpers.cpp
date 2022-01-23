#include <mutex>
#include <condition_variable>

struct lock {
  std::mutex mtx;
};

extern "C" lock* create_lock() {
  return new lock;
}

extern "C" void delete_lock(lock* lock) {
  delete lock;
}

extern "C" void lock_init(struct lock*) {
  ;
}
extern "C" void lock(struct lock* lock) {
  lock->mtx.lock();
}
extern "C" void unlock(struct lock* lock) {
  lock->mtx.unlock();
}

extern "C" int try_lock(struct lock* lock) {
  return lock->mtx.try_lock();
}

struct condition {
  std::condition_variable_any cv;
};

extern "C" condition* create_condition() {
  return new condition;
}

extern "C" void delete_condition(condition* cv) {
  delete cv;
}

extern "C" void condition_init(struct condition*) {
  ;
}
extern "C" void wait(struct condition* cv, struct lock* lock) {
  cv->cv.wait(lock->mtx);
}
extern "C" void notify_one(struct condition* cv) {
  cv->cv.notify_one();
}
extern "C" void notify_all(struct condition* cv) {
  cv->cv.notify_all();
}
