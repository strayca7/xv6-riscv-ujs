#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "defs.h"
#include "sem.h"

// Semaphore structure: 'used' marks allocation, 'value' is the
// current counter, 'lock' protects the fields, and 'chan' is
// used as the sleep/wakeup channel for waiters.
struct semaphore {
  int used;
  int value;
  struct spinlock lock;
  void *chan;
};

// Static array of semaphores and an allocation lock. We lazily
// initialize the allocation lock on first use.
static struct semaphore sems[NSEMS];
static struct spinlock semalloc_lock;
static int sem_inited = 0;

// Allocate a free semaphore slot and initialize it. Returns id
// on success or -1 if none available.
int
sem_alloc(int initval)
{
  int i;
  if(!sem_inited){
    // initialize global allocator lock once
    initlock(&semalloc_lock, "semalloc");
    sem_inited = 1;
  }
  acquire(&semalloc_lock);
  for(i = 0; i < NSEMS; i++){
    if(!sems[i].used){
      // initialize per-semaphore lock and fields
      initlock(&sems[i].lock, "sem");
      sems[i].used = 1;
      sems[i].value = initval;
      sems[i].chan = (void *)&sems[i];
      release(&semalloc_lock);
      return i;
    }
  }
  release(&semalloc_lock);
  return -1;
}

// Wait (P): decrement value or sleep until it becomes positive.
int
sem_wait_k(int id)
{
  if(id < 0 || id >= NSEMS) return -1;
  struct semaphore *s = &sems[id];
  acquire(&s->lock);
  // If no resources, go to sleep on s->chan (releases lock atomically
  // while sleeping; wakeup will reacquire the lock before returning).
  while(s->value <= 0){
    sleep(s->chan, &s->lock);
  }
  s->value--;
  release(&s->lock);
  return 0;
}

// Post (V): increment value and wake one or more waiters.
int
sem_post_k(int id)
{
  if(id < 0 || id >= NSEMS) return -1;
  struct semaphore *s = &sems[id];
  acquire(&s->lock);
  s->value++;
  // wakeup any processes sleeping on this semaphore channel
  wakeup(s->chan);
  release(&s->lock);
  return 0;
}
