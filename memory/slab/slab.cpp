#include <cstdint>
#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <iostream>

#include "slab.h"

void cache_setup(struct cache* cache_p, std::size_t object_size) {
  *cache_p =
      cache(0, int(std::max(object_size, (std::size_t)sizeof(CellNode))));
}

void list_release(SlabNode* head) {
  while (head) {
    SlabNode* nxt = head->next;
    free_slab(head);
    head = nxt;
  }
}

void cache_release(struct cache* cache) {
  for (SlabNode* head :
       {cache->empty_head, cache->partial_head, cache->full_head}) {
    list_release(head);
  }
}

SlabNode* remove_from_slab(SlabNode* slab_head, void** return_p) {
  assert(slab_head->cnt > 0);
  CellNode* cell_head = slab_head->head;
  assert(cell_head != nullptr);
  *return_p = cell_head;
  slab_head->cnt--;
  slab_head->head = cell_head->next;
  cell_head->next = nullptr;
  if (slab_head->head == nullptr) {
    // slab full
    return slab_head->next;
  }
  return slab_head;
}

void* cache_alloc(struct cache* cache) {
  for (int rep = 0; rep < 2; ++rep) {
    int i = 0;
    for (SlabNode** head_ptr : {&cache->partial_head, &cache->empty_head}) {
      SlabNode* head = *head_ptr;
      if (head) {
        void* p_expected = head->head;
        // assume object_size in slab == cache allocation object size
        void* p = nullptr;
        SlabNode* new_head = remove_from_slab(head, &p);
        if (i) {
          cache->remove_node(head_ptr, head);
          if (head->cnt == 0) {
            cache->insert_node(&cache->full_head, head);
          } else {
            cache->insert_node(&cache->partial_head, head);
          }
        } else if (new_head != head) {
          cache->remove_node(head_ptr, head);
          assert(head != *head_ptr);
          assert(head->cnt == 0);
          cache->insert_node(&cache->full_head, head);
        }
        assert(p);
        assert(p == p_expected);
        return p;
      }
      ++i;
    }
    cache->allocate_slab();
  }
  assert(false);
}

void cache_free(struct cache* cache, void* ptr) {
  //    std::size_t slab_size = 1 << cache->slab_order;
  SlabNode* node = (SlabNode*)((std::size_t)ptr &
                               ~(PAGE_SIZE * (1 << cache->slab_order) - 1));
  node->cnt++;

  CellNode* cur = (CellNode*)ptr;
  cur->next = nullptr;
  if (node->head) {
    CellNode* next = node->head->next;
    cur->next = next;
    node->head->next = cur;
  } else {
    node->head = cur;
  }
  bool removed = false;
  if (node->cnt == 1) {
    cache->remove_node(&cache->full_head, node);
    removed = true;
  } else if (node->cnt == (int)cache->slab_objects) {
    cache->remove_node(&cache->partial_head, node);
    removed = true;
  }

  if (removed) {
    if (node->cnt == (int)cache->slab_objects) {
      cache->insert_node(&cache->empty_head, node);
    } else {
      cache->insert_node(&cache->partial_head, node);
    }
  }
}

void cache_shrink(struct cache* cache) {
  list_release(cache->empty_head);
  cache->empty_head = nullptr;
}

SlabNode* cache::allocate_slab() {
  auto* slab = (SlabNode*)alloc_slab(slab_order);
  *slab = SlabNode(slab_order, object_size, slab);
  insert_node(&empty_head, slab);
  return slab;
}

void cache::insert_node(SlabNode** list, SlabNode* node) {
  if (*list) {
    (*list)->prev = node;
    node->next = *list;
  }
  *list = node;
}

void cache::remove_node(SlabNode** list, SlabNode* node) {
  assert(*list);
  SlabNode* next = node->next;
  SlabNode* prev = node->prev;
  if (prev) {
    prev->next = next;
  } else {
    assert(*list == node);
    *list = next;
  }

  if (next) {
    next->prev = prev;
  }
  node->next = nullptr;
  node->prev = nullptr;
}

cache::cache(int slab_order, std::size_t object_size)
    : slab_order(slab_order), object_size(object_size) {
  std::size_t size = PAGE_SIZE * (1 << slab_order) - sizeof(SlabNode);
  assert(size >= object_size);
  slab_objects = size / object_size;
  assert(slab_objects > 0);
}

cache::cache() {
  slab_order = object_size = 0;
}

SlabNode::SlabNode(int slab_order, std::size_t object_size, SlabNode* pointer)
    : slab_order(slab_order), object_size(object_size) {
  assert(PAGE_SIZE * (1 << slab_order) >= (std::size_t)sizeof(SlabNode));
  cnt = (PAGE_SIZE * (1 << slab_order) - sizeof(SlabNode)) / object_size;
  assert(cnt > 0);
  // this relates to cache
  //    head = (CellNode *) (this + 1);
  head = (CellNode*)(pointer + 1);
  //    std::cout << (std::size_t) head << std::endl;
  CellNode* cur_block_as_cell = head;
  char* cur_block_as_object = (char*)head;
  for (int i = 0; i < cnt; ++i) {
    cur_block_as_cell->next =
        i == cnt - 1 ? nullptr : (CellNode*)(cur_block_as_object + object_size);
    cur_block_as_object += object_size;
    cur_block_as_cell = (CellNode*)cur_block_as_object;
  }
}
