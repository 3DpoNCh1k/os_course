#pragma once

struct wdlock {
  /* wdlock_ctx должен хранить информацию обо всех
     захваченных wdlock-ах, а это поле позволит связать
     wdlock-и в список. */
  struct wdlock* next;

  /* Текущий владелец блокировки - из него мы извлекаем
     timestamp связанный с блокировкой, если блокировка
     свободна, то хранит NULL. */
  const struct wdlock_ctx* owner;

  /* lock и cv могут быть использованы чтобы дождаться
     пока блокировка не освободится либо у нее не сменится
     владелец. */
  struct lock* lock;
  struct condition* unlocked;
};

/* Каждый контекст имеет свой уникальный timestamp и хранит
   список захваченных блокировок. */
struct wdlock_ctx {
  unsigned long long timestamp;
  struct wdlock* locks;
};