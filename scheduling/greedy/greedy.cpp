#include <algorithm>
#include <vector>
#include <utility>

#include "greedy.h"

std::vector<int> schedule(const std::vector<int>& times) {
  std::vector<std::pair<int, int>> pairs;
  int n = (int)times.size();
  pairs.reserve(n);
  for (int i = 0; i < n; ++i) {
    pairs.emplace_back(times[i], i);
  }
  std::sort(pairs.begin(), pairs.end());
  std::vector<int> order(n);
  for (int i = 0; i < n; ++i) {
    order[i] = pairs[i].second;
  }
  return order;
}
