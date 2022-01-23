#pragma once
#include <cstdint>

const std::size_t PAGE_SIZE = 4096;
void* alloc_slab(int order);

void free_slab(void* slab);

struct CellNode {
  CellNode* next;
};

struct SlabNode {
  SlabNode(int slab_order, std::size_t object_size);

  SlabNode(int slab_order, size_t object_size, SlabNode* pointer);

  SlabNode* next = nullptr;
  SlabNode* prev = nullptr;
  int slab_order;
  std::size_t object_size;
  int cnt = 0;  // free objects slots
  CellNode* head = nullptr;
};

struct cache {
  cache();
  cache(int slab_order, std::size_t object_size);
  SlabNode* allocate_slab();
  void insert_node(SlabNode** list, SlabNode* node);
  void remove_node(SlabNode** list, SlabNode* node);
  SlabNode* empty_head =
      nullptr; /* список пустых SLAB-ов для поддержки cache_shrink */
  SlabNode* partial_head = nullptr; /* список частично занятых SLAB-ов */
  SlabNode* full_head = nullptr; /* список заполненых SLAB-ов */

  int slab_order;          /* используемый размер SLAB-а */
  std::size_t object_size; /* размер аллоцируемого объекта */
  std::size_t slab_objects = 0; /* количество объектов в одном SLAB-е */
};

/**
 * Функция инициализации будет вызвана перед тем, как
 * использовать это кеширующий аллокатор для аллокации.
 * Параметры:
 *  - cache - структура, которую вы должны инициализировать
 *  - object_size - размер объектов, которые должен
 *    аллоцировать этот кеширующий аллокатор
 **/
void cache_setup(struct cache* cache, std::size_t object_size);

/**
 * Функция освобождения будет вызвана когда работа с
 * аллокатором будет закончена. Она должна освободить
 * всю память занятую аллокатором. Проверяющая система
 * будет считать ошибкой, если не вся память будет
 * освбождена.
 **/
void cache_release(struct cache* cache);

/**
 * Функция аллокации памяти из кеширующего аллокатора.
 * Должна возвращать указатель на участок памяти размера
 * как минимум object_size байт (см cache_setup).
 * Гарантируется, что cache указывает на корректный
 * инициализированный аллокатор.
 **/
void* cache_alloc(struct cache* cache);

/**
 * Функция освобождения памяти назад в кеширующий аллокатор.
 * Гарантируется, что ptr - указатель ранее возвращенный из
 * cache_alloc.
 **/
void cache_free(struct cache* cache, void* ptr);

/**
 * Функция должна освободить все SLAB, которые не содержат
 * занятых объектов. Если SLAB не использовался для аллокации
 * объектов (например, если вы выделяли с помощью alloc_slab
 * память для внутренних нужд вашего алгоритма), то освбождать
 * его не обязательно.
 **/
void cache_shrink(struct cache* cache);