#include "../kernel/types.h"
#include "user.h"

// user-level mutex implementation by spinlock

void acquire(struct mutex *lk) {
  while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
    ;
}

void release(struct mutex *lk) { __sync_lock_release(&lk->locked); }
