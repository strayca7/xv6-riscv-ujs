// Special version of printf that is safe to call from multiple processes
#include "../kernel/types.h"
#include "user.h"
#include <stdarg.h>

void vprintf(int fd, const char *fmt, va_list ap);

#define SEM_PRINT_ID 99

void vprintf(int fd, const char *fmt, va_list ap);

// Note:
// 不可以直接使用 spinlock 实现的 mutex，在 fork 后子进程会复制父进程的内存，
// 导致 A 进程锁住的是 进程 A 内存里 的 printlock，
//  而 B 进程锁住的是 进程 B 内存里 的 printlock，
// 它们互不干扰，无法实现互斥效果。

void printfl(const char *fmt, ...) {
  va_list ap;

  // 尝试获取打印锁信号量
  // 如果信号量不存在(返回-1)，说明可能还没创建，直接打印
  int locking = (sem_p(SEM_PRINT_ID) == 0);

  va_start(ap, fmt);
  vprintf(1, fmt, ap); // 1 = stdout
  va_end(ap);

  if (locking) {
    sem_v(SEM_PRINT_ID);
  }
}
