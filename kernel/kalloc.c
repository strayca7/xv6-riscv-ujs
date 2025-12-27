// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "types.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

// stored at the beginning 8 bytes of a free page
struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

// reference count
struct spinlock ref_lock;
int ref_count[PHYSTOP / PGSIZE]; // absolute maximum of physical pages
                                 // not need to minus KERNBASE
#define PA2INDEX(pa) (((uint64)(pa)) / PGSIZE)

void kinit() {
  initlock(&kmem.lock, "kmem");
  initlock(&ref_lock, "ref_count");
  freerange(end, (void *)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end) {
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE) {
    // since kfree will check and decrease reference count,
    // we need to set reference count to 1 here
    acquire(&ref_lock);
    ref_count[PA2INDEX(p)] = 1;
    release(&ref_lock);
    kfree(p);
  }
}

// Increase the reference count of the physical page at pa.
void incref(uint64 pa) {
  acquire(&ref_lock);
  ref_count[PA2INDEX(pa)]++;
  release(&ref_lock);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa) {
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // decrease reference count
  acquire(&ref_lock);
  if (ref_count[PA2INDEX((uint64)pa)] <= 0)
    panic("kfree: reference count less than or equal to 0");

  ref_count[PA2INDEX((uint64)pa)]--;

  if (ref_count[PA2INDEX((uint64)pa)] > 0) {
    // still has references, do not free
    release(&ref_lock);
    return;
  }
  release(&ref_lock);

  // counter reference count equal to 0, actually free the page
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *kalloc(void) {
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if (r) {
    memset((char *)r, 5, PGSIZE); // fill with junk

    // initialize reference count to 1
    acquire(&ref_lock);
    ref_count[PA2INDEX((uint64)r)] = 1;
    release(&ref_lock);
  }
  return (void *)r;
}
