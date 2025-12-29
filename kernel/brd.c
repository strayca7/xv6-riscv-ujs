// Ramdisk
#include "defs.h"
#include "file.h"
#include "fs.h"
#include "memlayout.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "spinlock.h"
#include "types.h"

#define RD_SIZE (256 * 1024 * 1024) // 256MB
#define RD_PAGES (RD_SIZE / PGSIZE) // 65536 pages

struct {
  struct spinlock lock;
  void *pages[RD_PAGES];
  int loaded; // loaded flag
} rd_dev;

void rd_init() {
  initlock(&rd_dev.lock, "ramdisk");
  rd_dev.loaded = 0;
}

// rd_walk get the physical address of the idx-th page in ramdisk.
// If alloc is non-zero, allocate the page if it does not exist.
static void *rd_walk(uint idx, int alloc) {
  if (idx >= RD_PAGES)
    return 0;
  void *pa = rd_dev.pages[idx];
  if (pa == 0 && alloc) {
    pa = kalloc();
    if (pa == 0)
      return 0;
    memset(pa, 0, PGSIZE);
    rd_dev.pages[idx] = pa;
  }
  return pa;
}

int rd_read(int user_dst, uint64 dst, uint off, int n) {
  if (!rd_dev.loaded || off >= RD_SIZE)
    return -1;
  if (off + n > RD_SIZE)
    n = RD_SIZE - off;

  int total = 0;
  while (total < n) {
    uint idx = (off + total) / PGSIZE;
    uint page_off = (off + total) % PGSIZE;
    int m = PGSIZE - page_off;
    if (m > n - total)
      m = n - total;

    acquire(&rd_dev.lock);
    void *pa = rd_walk(idx, 0);
    release(&rd_dev.lock);

    if (pa == 0) {
      // blank hole
      // use a temporary zero page
      // use a empty page to avoid kernel stack overflow(4kB)
      void *zero_page = kalloc();
      if (zero_page == 0)
        return -1;
      memset(zero_page, 0, PGSIZE);

      if (user_dst) {
        if (copyout(myproc()->pagetable, dst + total, zero_page, m) < 0) {
          kfree(zero_page);
          return -1;
        }
      } else {
        memmove((void *)(dst + total), zero_page, m);
      }
      kfree(zero_page);
      total += m;
    } else {
      if (user_dst) {
        if (copyout(myproc()->pagetable, dst + total, (char *)pa + page_off,
                    m) < 0)
          return -1;
      } else {
        memmove((void *)(dst + total), (char *)pa + page_off, m);
      }
      total += m;
    }
  }
  return total;
}

int rd_write(int user_src, uint64 src, uint off, int n) {
  if (!rd_dev.loaded || off >= RD_SIZE)
    return -1;
  if (off + n > RD_SIZE)
    n = RD_SIZE - off;

  int total = 0;
  while (total < n) {
    uint idx = (off + total) / PGSIZE;
    uint page_off = (off + total) % PGSIZE;
    int m = PGSIZE - page_off;
    if (m > n - total)
      m = n - total;

    acquire(&rd_dev.lock);
    void *pa = rd_walk(idx, 1);
    release(&rd_dev.lock);

    // out of memory
    if (pa == 0)
      return -1;

    if (user_src) {
      if (copyin(myproc()->pagetable, (char *)pa + page_off, src + total, m) <
          0)
        return -1;
    } else {
      memmove((char *)pa + page_off, (void *)(src + total), m);
    }
    total += m;
  }
  return total;
}

uint64 sys_load_ramdisk(void) {
  acquire(&rd_dev.lock);
  if (rd_dev.loaded) {
    release(&rd_dev.lock);
    return -1; // already loaded
  }

  devsw[RD_MAJOR].read = rd_read;
  devsw[RD_MAJOR].write = rd_write;
  rd_dev.loaded = 1;
  release(&rd_dev.lock);
  printf("ramdisk: loaded (dynamic alloc mode)\n");
  return 0;
}

uint64 sys_unload_ramdisk(void) {
  acquire(&rd_dev.lock);
  if (!rd_dev.loaded) {
    release(&rd_dev.lock);
    return -1; // not loaded
  }

  devsw[RD_MAJOR].read = 0;
  devsw[RD_MAJOR].write = 0;
  rd_dev.loaded = 0;

  // free all allocated pages
  for (int i = 0; i < RD_PAGES; i++) {
    if (rd_dev.pages[i]) {
      kfree(rd_dev.pages[i]);
      rd_dev.pages[i] = 0;
    }
  }
  release(&rd_dev.lock);
  printf("ramdisk: unloaded and memory freed\n");
  return 0;
}