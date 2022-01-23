#include <cassert>

#include "myalloc.h"

Node* head;
char* BUF;
std::size_t SIZE;

std::size_t get_length() {
  std::size_t length = 0;
  Node* cur = head;
  while (cur) {
    length++;
    cur = cur->next;
  }
  return length;
}

void check_bounds(Node* node) {
  // only for front node
  assert((char*)node >= BUF && (char*)node + node->size <= BUF + SIZE);
}

void sync_node_back(Node* node_front) {
  Node* node_back =
      (Node*)(((char*)node_front + node_front->size) - sizeof(Node));
  *node_back = *node_front;
}

Node* create_node(void* buf, std::size_t size, bool used) {
  Node* node_front = (Node*)buf;
  *node_front = {used, nullptr, nullptr, size};
  sync_node_back(node_front);
  check_bounds(node_front);
  return node_front;
}

void insert_node(Node* node) {
  assert(node->next == nullptr && node->prev == nullptr);
  check_bounds(node);
  if (head) {
    head->prev = node;
    sync_node_back(head);
    node->next = head;
    sync_node_back(node);
  }
  head = node;
}

void* remove_node(Node* node) {
  Node* prev = node->prev;
  Node* next = node->next;
  if (prev != nullptr) {
    prev->next = next;
    sync_node_back(prev);
  } else {
    head = next;
  }
  if (next != nullptr) {
    next->prev = prev;
    sync_node_back(next);
  }
  return node;
}

Node* merge(Node* left, Node* right) {
  assert(!left->used && !right->used);
  assert(left->size <= SIZE);
  assert(right->size <= SIZE);
  assert(left->size + right->size <= SIZE);
  check_bounds(left);
  check_bounds(right);
  std::size_t size = left->size + right->size;
  Node* node = create_node(left, size, false);
  check_bounds(node);
  return node;
}

void mysetup(void* buf, std::size_t size) {
  BUF = (char*)buf;
  SIZE = size;
  head = create_node(BUF, SIZE, false);
}

void* myalloc(std::size_t size) {
  Node* cur = head;
  while (cur != nullptr) {
    assert(cur->used == false);
    // second 2 * sizeof(Node) - for rest block's size, so we can create it and
    // don't lose memory, could be any greater or equal constant
    if (cur->size >= size + 2 * sizeof(Node) + 2 * sizeof(Node)) {
      std::size_t rest_size = cur->size - (size + 2 * sizeof(Node));
      void* node_start = remove_node(cur);
      Node* new_node = create_node(node_start, size + 2 * sizeof(Node), true);
      void* rest_node_start = (char*)node_start + size + 2 * sizeof(Node);
      Node* rest_node = create_node(rest_node_start, rest_size, false);
      insert_node(rest_node);
      check_bounds(new_node);
      return (char*)new_node + sizeof(Node);
    }
    cur = cur->next;
  }
  return nullptr;
}

void myfree(void* p) {
  char* node_start = (char*)p - sizeof(Node);
  Node* node = (Node*)node_start;
  assert(node->used);
  std::size_t size = node->size;
  node = create_node(node_start, size, false);
  while (true) {
    bool merged = false;
    // check left
    if (node_start >= BUF + sizeof(Node)) {
      Node* left_node_back = (Node*)(node_start - sizeof(Node));
      if (!left_node_back->used && left_node_back->size) {
        assert(node_start >= BUF + left_node_back->size);
        Node* left_node = (Node*)(node_start - left_node_back->size);
        remove_node(left_node);
        node = merge(left_node, node);
        merged = true;
      }
    }
    // check right
    node_start = node_start + size;
    if (node_start + sizeof(Node) < BUF + SIZE) {
      Node* right_node_front = (Node*)node_start;
      if (!right_node_front->used && right_node_front->size) {
        assert(node_start + right_node_front->size <= BUF + SIZE);
        remove_node(right_node_front);
        node = merge(node, right_node_front);
        merged = true;
      }
    }
    if (!merged) {
      break;
    }
    node_start = (char*)node;
    size = node->size;
  }
  insert_node(node);
}