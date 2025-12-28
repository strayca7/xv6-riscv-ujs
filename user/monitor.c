#include "../kernel/types.h"
#include "../kernel/fcntl.h"
#include "user.h"

int main() {
  int fd;
  
  printf("--- Test 1: Monitor OFF ---\n");
  monitor(0); // 关闭监控
  fd = open("test_off", O_CREATE | O_WRONLY);
  write(fd, "hello\n", 6);
  close(fd);
  
  printf("\n--- Test 2: Monitor ON ---\n");
  monitor(1); // 开启监控
  fd = open("test_on", O_CREATE | O_WRONLY);
  write(fd, "world\n", 6); // 应该在控制台看到 [MONITOR] 输出
  close(fd);

  monitor(0);
  
  exit(0);
}