#include "atomic.h"

/* Следующие две функции - ваше задание. Эти функции нужно
   реализовать используя load_linked и store_conditional для
   обращений к atomic_int */

/* atomic_fetch_add добавляет arg к значению записанному
   на которое указывает x. И возвращает предыдущее значение,
   на которое указывал x как результат.

   Т. е. если x указывает на значение 3138 и вы вызываете
   atomic_fetch_add с arg == 10, то функция должна изменить
   значение, на которое указывает x на 3148 и вернуть 3138
   в качестве результата. */
int atomic_fetch_add(atomic_int* x, int arg) {
  while (true) {
    int ret = load_linked(x);
    if (store_conditional(x, ret + arg)) {
      return ret;
    }
  }
}

/* atomic_compare_exchange сравнивает значение, на которое
   указывает x, со значением, на которое указывает expected_value,
   если они равны, то функция должна записать new_value в ячейку,
   на которую указывает x и возвращает true.
   В противном случае функция должна записать значение, на
   которое указывает x, в ячейку, на которую указывает
   expected_value и вернуть false.

   Т. е. если x указывает на значение 42, expected_value тоже
   указывает на 42, и функция вызывается с new_value == 3148,
   то функция должна записать 3148 в ячейку, на которую указывает
   x, и вернуть true.

   А если x указывает на значение 3148, expected_value указывает
   на значение 42, и new_value == 0, то функция должна записать
   в *expected_value значение 3148 и вернуть false.

   Обратите внимание, что функция должна возвращать false только
   если x хранит значение отличное от *expected_value, а
   store_conditional может вернуть false, даже если значение не
   изменилось. */
bool atomic_compare_exchange(atomic_int* x, int* expected_value,
                             int new_value) {
  int value = load_linked(x);
  while (value == *expected_value && !store_conditional(x, new_value)) {
    value = load_linked(x);
  }
  if (value != *expected_value) {
    *expected_value = value;
    return false;
  }
  return true;
}