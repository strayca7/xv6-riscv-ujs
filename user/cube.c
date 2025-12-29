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
  exit(0);
}
