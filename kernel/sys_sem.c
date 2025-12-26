#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "sem.h"
#include "room.h"

// Syscall implementations: these are thin wrappers that extract
// syscall arguments from the user trapframe and call the kernel
// helper functions implemented above. Using small wrappers keeps
// user/kernel interface simple and easy to test.

// syscall: int sem_init(int initval)
uint64
sys_sem_init(void)
{
  int initv;
  argint(0, &initv);
  return sem_alloc(initv);
}

// syscall: int sem_wait(int id)
uint64
sys_sem_wait(void)
{
  int id;
  argint(0, &id);
  return sem_wait_k(id);
}

// syscall: int sem_post(int id)
uint64
sys_sem_post(void)
{
  int id;
  argint(0, &id);
  return sem_post_k(id);
}

// syscall: int room_register(char *info)
// Copies a user string into a kernel buffer then calls the
// kernel registration routine.
uint64
sys_room_register(void)
{
  char buf[ROOM_INFOLEN];
  int n = argstr(0, buf, ROOM_INFOLEN);
  if(n < 0) return -1;
  return kroom_register(buf);
}

// syscall: int room_unregister(int seat)
uint64
sys_room_unregister(void)
{
  int seat;
  argint(0, &seat);
  return kroom_unregister(seat);
}

// syscall: int room_list(char *buf, int len)
// Copies a snapshot of the room into user memory at 'buf'.
uint64
sys_room_list(void)
{
  uint64 addr;
  int len;
  argaddr(0, &addr);
  argint(1, &len);
  return kroom_list((char*)addr, len);
}
