#pragma once

#include <cstdint>

// Memory allocation using border markers and double-linked list of free nodes
// node_front and node_back - markers
// not used node_front - node in the list
// ...[node_front1][user's memory][node_back1]...[node_front2][user's
// memory][node_back2]
//    |-----------------size-----------------|

struct Node {
  bool used;
  Node* next;
  Node* prev;
  std::size_t size;
  bool operator==(const Node& he) const {
    return used == he.used && next == he.next && prev == he.prev &&
           size == he.size;
  }
};

extern Node* head;
extern char* BUF;
extern std::size_t SIZE;

std::size_t get_length();

void mysetup(void* buf, std::size_t size);

void* myalloc(std::size_t size);

void myfree(void* p);