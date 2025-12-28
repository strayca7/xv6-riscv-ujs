#include "defs.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "spinlock.h"
#include "types.h"

#define SEM_MAX 16

struct sem {
  struct spinlock lock;

  // unique identifier for the semaphore
  // user can specify id by any int value
  int id;

  int value; // resource count
  int valid;
};

struct {
  struct spinlock lock;
  struct sem sems[SEM_MAX];
} sem_table;

void seminit() {
  initlock(&sem_table.lock, "sem_table");
  for (int i = 0; i < SEM_MAX; i++) {
    initlock(&sem_table.sems[i].lock, "sem");
    sem_table.sems[i].valid = 0;
  }
}

// Create or find a semaphore
int sem_create(int id, int value) {
  int i;
  
  acquire(&sem_table.lock);
  // Check if already exists
  for(i = 0; i < SEM_MAX; i++){
    if(sem_table.sems[i].valid && sem_table.sems[i].id == id){
      release(&sem_table.lock);
      return 0; // Already exists
    }
  }

  // Create new
  for(i = 0; i < SEM_MAX; i++){
    if(!sem_table.sems[i].valid){
      sem_table.sems[i].valid = 1;
      sem_table.sems[i].id = id;
      sem_table.sems[i].value = value;
      release(&sem_table.lock);
      return 0;
    }
  }
  release(&sem_table.lock);
  return -1; // Table full
}

// Free a semaphore
int sem_free(int id) {
  int i;
  acquire(&sem_table.lock);
  for(i = 0; i < SEM_MAX; i++){
    if(sem_table.sems[i].valid && sem_table.sems[i].id == id){
      sem_table.sems[i].valid = 0;
      release(&sem_table.lock);
      return 0;
    }
  }
  release(&sem_table.lock);
  return -1;
}

int sem_p(int id) { // Acquire / Wait
  struct sem *s = 0;

  acquire(&sem_table.lock);
  for (int i = 0; i < SEM_MAX; i++) {
    if (sem_table.sems[i].valid && sem_table.sems[i].id == id) {
      s = &sem_table.sems[i];
      break;
    }
  }
  release(&sem_table.lock);

  if (s == 0) return -1;

  acquire(&s->lock);
  while (s->value <= 0) {
    sleep(s, &s->lock);
  }
  s->value--;
  release(&s->lock);
  return 0;
}

int sem_v(int id) { // Release / Signal
  struct sem *s = 0;

  acquire(&sem_table.lock);
  for (int i = 0; i < SEM_MAX; i++) {
    if (sem_table.sems[i].valid && sem_table.sems[i].id == id) {
      s = &sem_table.sems[i];
      break;
    }
  }
  release(&sem_table.lock);

  if (s == 0) return -1;

  acquire(&s->lock);
  s->value++;
  wakeup(s);
  release(&s->lock);
  return 0;
}
