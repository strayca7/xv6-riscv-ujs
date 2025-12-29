#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "spinlock.h"
#include "types.h"

#define SHM_MAX 8   // maximum number of shared memory pages

struct {
  struct spinlock lock;
  struct {
    // unique identifier for the shared memory page
    // user can specify id by any uint value
    uint id;
    
    void *pa;
    int ref;    // reference count
  } frames[SHM_MAX];
} shm_table;

void shminit() {
  initlock(&shm_table.lock, "shm");
  for (int i = 0; i < SHM_MAX; i++) {
    shm_table.frames[i].id = 0;
    shm_table.frames[i].pa = 0;
    shm_table.frames[i].ref = 0;
  }
}

// Open or create a shared memory page with given id.
// Param id is an unique identifier for the shared memory page.
// In Task 5, we provide 8 shared memory pages that you can specify by any id.
// Returns the virtual address of the mapped shared memory page in
// the caller process's address space.
// On error, returns -1.
uint64 shm_open(int id) {
  struct proc *p = myproc();
  void *pa = 0;
  uint64 va = 0;
  int i;

  acquire(&shm_table.lock);

  // Check if already exists
  for (i = 0; i < SHM_MAX; i++) {
    if (shm_table.frames[i].id == id && shm_table.frames[i].pa != 0) {
      pa = shm_table.frames[i].pa;
      shm_table.frames[i].ref++;
      incref((uint64)pa);
      break;
    }
  }

  if (pa == 0) {
    for (i = 0; i < SHM_MAX; i++) {
      if (shm_table.frames[i].pa == 0) {
        pa = kalloc();
        if (pa == 0) {
          release(&shm_table.lock);
          return -1;
        }
        memset(pa, 0, PGSIZE);
        shm_table.frames[i].id = id;
        shm_table.frames[i].pa = pa;
        shm_table.frames[i].ref = 1;
        incref((uint64)pa);
        break;
      }
    }
  }
  release(&shm_table.lock);

  if (pa == 0)
    return -1;

  va = PGROUNDUP(p->sz);
  // map shared memory page(pa) to process's address space at va
  if (mappages(p->pagetable, va, PGSIZE, (uint64)pa, PTE_W | PTE_R | PTE_U) <
      0) {
    return -1;
  }

  p->sz = va + PGSIZE;
  return va;
}

int shm_close(int id) {
  int i;
  int found = 0;

  acquire(&shm_table.lock);
  for (i = 0; i < SHM_MAX; i++) {
    if (shm_table.frames[i].id == id && shm_table.frames[i].pa != 0) {
      found = 1;
      // COW
      shm_table.frames[i].ref--;
      // free physical page if no references
      if (shm_table.frames[i].ref == 0) {
        kfree(shm_table.frames[i].pa);
        shm_table.frames[i].pa = 0;
        shm_table.frames[i].id = 0;
      }
      break;
    }
  }
  release(&shm_table.lock);
  return found ? 0 : -1;
}

// Check if the physical page is a shared memory page
int is_shared_page(uint64 pa) {
  int i;
  int yes = 0;
  acquire(&shm_table.lock);
  for (i = 0; i < SHM_MAX; i++) {
    if (shm_table.frames[i].pa == (void *)pa) {
      yes = 1;
      break;
    }
  }
  release(&shm_table.lock);
  return yes;
}
