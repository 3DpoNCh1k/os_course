#pragma once

struct lock;
void lock_init(struct lock* lock);
void lock(struct lock* lock);
int try_lock(struct lock* lock);
void unlock(struct lock* lock);

struct condition;
void condition_init(struct condition* cv);
void wait(struct condition* cv, struct lock* lock);
void notify_one(struct condition* cv);
void notify_all(struct condition* cv);

struct wdlock;
struct wdlock_ctx;

/* Всегда вызывается перед тем, как использовать контекст.

   ВАЖНО: функция является частью интерфейса - не меняйте
          ее имя и аргументы.
*/
void wdlock_ctx_init(struct wdlock_ctx* ctx);

/* Всегда вызывается перед тем, как использовать блокировку.

   ВАЖНО: функция является частью интерфейса - не меняйте
          ее имя и аргументы.
*/
void wdlock_init(struct wdlock* lock);

/* Функция для захвата блокировки l контекстом ctx. Если
   захват блокировки прошел успешно функция должна вернуть
   ненулевое значение. Если же захват блокировки провалился
   из-за проверки timestamp-а, то функция должна вернуть 0.

   Помните, что контекст должен хранить информацию о
   захваченных блокировках, чтобы иметь возможность освободить
   их в функции wdlock_unlock.

   ВАЖНО: функция является частью интерфейса - не меняйте
          ее имя и аргументы.
*/
int wdlock_lock(struct wdlock* l, struct wdlock_ctx* ctx);

/* Функция для освбождения всех блокировок, захваченных
   контекстом ctx. При этом может случиться так, что этот
   контекст не владеет ни одной блокировкой, в этом случае
   ничего делать не нужно.

   ВАЖНО: функция является частью интерфейса - не меняйте
          ее имя и аргументы.
*/
void wdlock_unlock(struct wdlock_ctx* ctx);
