#include <queue>
#include <cassert>

#include "round_robin.h"

int rest, full;
std::queue<int> q;

void scheduler_setup(int timeslice) {
  full = rest = timeslice;
}

void new_thread(int thread_id) {
  q.push(thread_id);
}

void exit_thread() {
  q.pop();
  rest = full;
}

void block_thread() {
  q.pop();
  rest = full;
}

void wake_thread(int thread_id) {
  q.push(thread_id);
}

void timer_tick() {
  if (!q.empty()) {
    rest--;
    if (!rest) {
      q.push(q.front());
      q.pop();
      rest = full;
    }
  } else {
    assert(rest == full);
  }
}

int current_thread() {
  return q.empty() ? -1 : q.front();
}