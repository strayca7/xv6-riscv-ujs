// Room implementation notes:
// - 'seat_sem' is a counting semaphore tracking free seats (initially ROOM_CAPACITY).
// - 'mutex_sem' is a binary semaphore used to ensure mutually exclusive
//    access to the 'seats' array during register/unregister.
// This design separates availability (seat_sem) from critical-section
// protection (mutex_sem) to illustrate semaphore use.

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"
#include "sem.h"
#include "room.h"

// seats[] stores identity strings for each occupied seat; 'used' flags
// whether a seat is occupied.
static char seats[ROOM_CAPACITY][ROOM_INFOLEN];
static int used[ROOM_CAPACITY];
static int seat_sem = -1;   // counting semaphore for available seats
static int mutex_sem = -1;  // binary semaphore for mutual exclusion
static int room_inited = 0;

// Initialize the room: allocate semaphores and clear state.
int
room_init(void)
{
  if(room_inited) return 0;
  seat_sem = sem_alloc(ROOM_CAPACITY); // initial count = number of seats
  mutex_sem = sem_alloc(1);            // binary mutex
  if(seat_sem < 0 || mutex_sem < 0)
    return -1;
  int i;
  for(i = 0; i < ROOM_CAPACITY; i++){
    used[i] = 0;
    seats[i][0] = '\0';
  }
  room_inited = 1;
  return 0;
}

// Register a reader: wait for a free seat (seat_sem), acquire mutex,
// write info into the first free slot, release mutex and return seat index.
int
kroom_register(const char *info)
{
  if(!room_inited) room_init();
  if(sem_wait_k(seat_sem) < 0) return -1;         // wait for availability
  if(sem_wait_k(mutex_sem) < 0) { sem_post_k(seat_sem); return -1; }
  int i;
  for(i = 0; i < ROOM_CAPACITY; i++){
    if(!used[i]){
      used[i] = 1;
      safestrcpy(seats[i], (char*)info, ROOM_INFOLEN);
      sem_post_k(mutex_sem);
      return i;
    }
  }
  // no free slot found (shouldn't happen because of seat_sem)
  sem_post_k(mutex_sem);
  sem_post_k(seat_sem);
  return -1;
}

// Unregister: mark seat as free and post seat_sem to signal availability.
int
kroom_unregister(int seat)
{
  if(!room_inited) return -1;
  if(seat < 0 || seat >= ROOM_CAPACITY) return -1;
  if(sem_wait_k(mutex_sem) < 0) return -1;
  if(!used[seat]){
    sem_post_k(mutex_sem);
    return -1;
  }
  used[seat] = 0;
  seats[seat][0] = '\0';
  sem_post_k(mutex_sem);
  sem_post_k(seat_sem); // increase available seats
  return 0;
}

// Produce a textual snapshot of the room and copy it out to user space.
int
kroom_list(char *buf, int len)
{
  if(!room_inited) room_init();
  char tmp[ROOM_CAPACITY * ROOM_INFOLEN + 64];
  int off = 0;
  int i;
  for(i = 0; i < ROOM_CAPACITY; i++){
    // Build line: "seat %d: <info>\n" without using snprintf.
    safestrcpy(tmp+off, "seat ", sizeof(tmp)-off);
    off += strlen(tmp+off);
    char num[16];
    int npos = 0;
    int x = i;
    if(x == 0) num[npos++] = '0';
    while(x > 0){ num[npos++] = '0' + (x % 10); x /= 10; }
    while(npos > 0) tmp[off++] = num[--npos];
    tmp[off] = '\0';
    safestrcpy(tmp+off, ": ", sizeof(tmp)-off);
    off += strlen(tmp+off);
    if(used[i]){
      safestrcpy(tmp+off, seats[i], sizeof(tmp)-off);
      off += strlen(tmp+off);
    } else {
      safestrcpy(tmp+off, "<empty>", sizeof(tmp)-off);
      off += strlen(tmp+off);
    }
    safestrcpy(tmp+off, "\n", sizeof(tmp)-off);
    off += 1;
    if(off >= sizeof(tmp)-1) break;
  }
  if(len < off) off = len-1;
  if(off < 0) off = 0;
  tmp[off] = '\0';
  struct proc *p = myproc();
  if(copyout(p->pagetable, (uint64)buf, tmp, off+1) < 0)
    return -1;
  return off;
}
