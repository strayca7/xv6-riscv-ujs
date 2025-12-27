#include "../kernel/memlayout.h"
#include "../kernel/types.h"
#include "user.h"

// allocate more than half of physical memory,
// then fork. this will fail in the default
// kernel, which does not support copy-on-write.
// 分配超过一半的物理内存，然后 fork。
// 在不支持写时复制（COW）的默认内核中，这将失败。
void simpletest() {
  uint64 phys_size = PHYSTOP - KERNBASE;
  int sz = (phys_size / 3) * 2;

  printf("simple: ");

  // 分配 2/3 的物理内存。
  // 如果没有 COW，fork
  // 时会尝试复制这部分内存，导致总内存需求超过物理内存上限，从而失败。
  char *p = sbrk(sz);
  if (p == (char *)0xffffffffffffffffL) {
    printf("sbrk(%d) failed\n", sz);
    exit(-1);
  }

  // 写入数据，确保物理页被分配
  for (char *q = p; q < p + sz; q += 4096) {
    *(int *)q = getpid();
  }

  // fork 子进程。
  // 在 COW 机制下，这里只复制页表，不复制物理内存。
  int pid = fork();
  if (pid < 0) {
    printf("fork() failed\n");
    exit(-1);
  }

  if (pid == 0)
    exit(0);

  wait(0);

  // 释放内存
  if (sbrk(-sz) == (char *)0xffffffffffffffffL) {
    printf("sbrk(-%d) failed\n", sz);
    exit(-1);
  }

  printf("ok\n");
}

// three processes all write COW memory.
// this causes more than one page fault per page.
// 三个进程都写入 COW 内存。
// 这会导致每个页面发生多次缺页异常。
void threetest() {
  uint64 phys_size = PHYSTOP - KERNBASE;
  int sz = phys_size / 4;
  int pid1, pid2;

  printf("three: ");

  char *p = sbrk(sz);
  if (p == (char *)0xffffffffffffffffL) {
    printf("sbrk(%d) failed\n", sz);
    exit(-1);
  }

  // 第一个 fork：引用计数变为 2
  pid1 = fork();
  if (pid1 < 0) {
    printf("fork failed\n");
    exit(-1);
  }
  if (pid1 == 0) {
    // 子进程再次 fork：引用计数变为 3
    pid2 = fork();
    if (pid2 < 0) {
      printf("fork failed\n");
      exit(-1);
    }
    if (pid2 == 0) {
      // 孙子进程写入：触发缺页异常，分配新页，引用计数减 1
      for (char *q = p; q < p + sz; q += 4096) {
        *(int *)q = getpid();
      }
      exit(0);
    }
    // 子进程写入：触发缺页异常
    for (char *q = p; q < p + sz; q += 4096) {
      *(int *)q = getpid();
    }
    wait(0);
    exit(0);
  }

  // 父进程写入：触发缺页异常
  for (char *q = p; q < p + sz; q += 4096) {
    *(int *)q = getpid();
  }
  wait(0);

  printf("ok\n");
}

void filetest() {
  printf("file: ");

  char buf[100];
  int i;
  int fd[2];

  for (i = 0; i < sizeof(buf); i++)
    buf[i] = i;

  if (pipe(fd) != 0) {
    printf("pipe() failed\n");
    exit(-1);
  }

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(-1);
  }

  if (pid == 0) {
    // child writes to pipe
    // this tests copyout() to a COW page
    // 子进程写入管道
    // write 系统调用会调用内核的 copyout 函数将 buf 的内容复制到内核空间
    // 如果 buf 是 COW 页面，copyout 必须能够正确处理（触发缺页或手动处理）
    close(fd[0]);
    if (write(fd[1], buf, sizeof(buf)) != sizeof(buf)) {
      printf("write failed\n");
      exit(-1);
    }
    exit(0);
  }

  close(fd[1]);
  wait(0);
  printf("ok\n");
}

int main(int argc, char *argv[]) {
  simpletest();
  threetest();
  filetest();
  printf("ALL COW TESTS PASSED\n");
  // getpgfault() is a syscall that returns
  // the number of page faults so far for this process.
  printf("page fault count: %d\n", getpgfault());
  exit(0);
}