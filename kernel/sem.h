// Simple semaphore API for xv6
// Implementation uses a spinlock per semaphore and sleep/wakeup
// to block and wake processes. A kernel alloc function returns
// a small integer id which user syscalls use to operate on the
// semaphore (sem_wait/sem_post). This is a minimal counting
// semaphore implementation for teaching/demo purposes.

#ifndef SEM_H
#define SEM_H

#include "types.h"

#define NSEMS 64

// Allocate a semaphore with initial value 'initval'. Returns
// semaphore id (0..NSEMS-1) or -1 on failure.
int sem_alloc(int initval);

// Decrement (P). Blocks the caller if value <= 0.
int sem_wait_k(int id);

// Increment (V). Wakes a waiting process if any.
int sem_post_k(int id);

#endif
