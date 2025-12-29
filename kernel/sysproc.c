#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64 sys_exit(void) {
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return kfork(); }

uint64 sys_wait(void) {
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64 sys_sbrk(void) {
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64 sys_pause(void) {
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_kill(void) {
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Task 1
// print cube of a number
uint64 sys_cube(void) {
  int n;
  argint(0, &n);
  // printf("%d\n", n * n * n);
  return n * n * n;
}

// Task 4
extern uint64 pg_fault_cnt;
// get the number of page faults
uint64 sys_getpgfault(void) {
  return pg_fault_cnt;
}

// Task 2
extern int monitor_enabled;
uint64 sys_monitor(void) {
  int enable;
  argint(0, &enable);
  monitor_enabled = enable;
  return 0;
}