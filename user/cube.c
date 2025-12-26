<<<<<<< HEAD
#include "../kernel/stat.h"
#include "../kernel/types.h"
#include "user.h"

// Cube the arguments and print the result
int main(int argc, char *argv[]) {
  if (argc == 2 && strcmp(argv[1], "-m") != 0) {
    int n = atoi(argv[1]);
    printf("%d\n", cube(n));
    exit(0);
  } else if (argc > 2 && strcmp(argv[1], "-m") == 0) {
    if (argc == 2) {
      fprintf(2, "Usage: cube <number...>\n");
      exit(1);
    }
    int out;
    for (int i = 2; i < argc; i++) {
      int n = atoi(argv[i]);
      out = cube(n);
      printf("%d", out);
      if (i < argc - 1)
        printf(" ");
      else
        printf("\n");
    }
  } else {
    fprintf(2, "Usage: cube <number>\n");
    exit(1);
  }
=======
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
>>>>>>> 51ca9a2 (Merge pull request #1 from strayca7/feat/stray-cube)
  exit(0);
}
