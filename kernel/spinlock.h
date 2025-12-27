// Mutual exclusion lock.
#ifndef KERNEL_SPINLOCK_H
#define KERNEL_SPINLOCK_H

#include "types.h"

struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

#endif // KERNEL_SPINLOCK_H