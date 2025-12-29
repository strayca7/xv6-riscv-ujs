#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"
#include "../kernel/fcntl.h"

int main() {
  int fd;
  
  // 1. 创建文件 a
  fd = open("a", O_CREATE | O_WRONLY);
  write(fd, "hello", 5);
  close(fd);
  
  // 2. 创建链接 b -> a
  if(symlink("a", "b") < 0){
    printf("symlink failed\n");
    exit(1);
  }
  
  // 3. 打开 b，应该读到 "hello"
  fd = open("b", O_RDONLY);
  if(fd < 0){
    printf("open b failed\n");
    exit(1);
  }
  // ... read and verify ...
  
  printf("symlink test passed\n");
  exit(0);
}