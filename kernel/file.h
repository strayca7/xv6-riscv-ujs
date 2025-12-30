#ifndef FILE_H
#define FILE_H

#include "types.h"
#include "fs.h"
#include "sleeplock.h"

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define	mkdev(m,n)  ((uint)((m)<<16| (n)))

// VFS abstract operations
struct fs_ops {
  int (*read)(struct inode*, int, uint64, uint, int);
  int (*write)(struct inode *, int, uint64, uint, int);
  // stat, open, close, etc. can be added here
};

// in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+1+1];

  struct fs_ops *ops; // file system operations
};

// map major device number to device functions.
struct devsw {
  // origin
  // int (*read)(int, uint64, int);
  // int (*write)(int, uint64, int);

  // modified for Ramdisk
  int (*read)(int, uint64, uint,int);
  int (*write)(int, uint64, uint,int);
};

extern struct devsw devsw[];

#define CONSOLE   1
#define RD_MAJOR  2 // Ramdisk major device number

#endif // FILE_H