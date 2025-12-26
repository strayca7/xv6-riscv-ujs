#include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

// Cube the first argument and print the result
// 将第一个参数立方并打印结果
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "Usage: cube <number>\n");
    exit(1);
  }
  int n = atoi(argv[1]);
  cube(n);
  exit(0);
}
