#include "defs.h"
#include "file.h"
#include "fs.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "spinlock.h"
#include "stat.h"
#include "types.h"

extern int default_read(struct inode *ip, int user_dst, uint64 dst, uint off,
                        int n);
extern int default_write(struct inode *ip, int user_src, uint64 src, uint off,
                         int n);

// A wrapper around default_read to log read operations
int ext_write(struct inode *ip, int user_src, uint64 src, uint off, int n) {
  struct proc *p = myproc();
  printf("[MONITOR] PID %d is writing to inode %d (size %d)\n", p->pid,
         ip->inum, n);

  return default_write(ip, user_src, src, off, n);
}

// A wrapper around default_write to log write operations
int ext_read(struct inode *ip, int user_dst, uint64 dst, uint off, int n) {
  struct proc *p = myproc();
  printf("[MONITOR] PID %d is reading from inode %d (size %d)\n", p->pid,
         ip->inum, n);

  return default_read(ip, user_dst, dst, off, n);
}

struct fs_ops ext_fs_ops = {
    .read = ext_read,
    .write = ext_write,
};

int monitor_enabled = 0; // Set to 1 to enable monitoring, 0 to disable
