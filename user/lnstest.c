#include "../kernel/fcntl.h"
#include "../kernel/param.h"
#include "../kernel/stat.h"
#include "../kernel/types.h"
#include "user.h"

void cleanup(void) {
  unlink("/testsymlink/a");
  unlink("/testsymlink/b");
  unlink("/testsymlink/c");
  unlink("/testsymlink/1");
  unlink("/testsymlink/2");
  unlink("/testsymlink/3");
  unlink("/testsymlink/cycle");
  unlink("/testsymlink");
}

void testsymlinks(void) {
  int r, fd1, fd2;
  char buf[4];
  struct stat st;

  printf("test symlinks\n");

  cleanup();
  mkdir("/testsymlink");

  fd1 = open("/testsymlink/a", O_CREATE | O_RDWR);
  if (fd1 < 0) {
    printf("error: open /testsymlink/a failed\n");
    exit(1);
  }
  write(fd1, "hello", 5);
  close(fd1);

  r = symlink("/testsymlink/a", "/testsymlink/b");
  if (r < 0) {
    printf("error: symlink /testsymlink/a -> /testsymlink/b failed\n");
    exit(1);
  }

  fd2 = open("/testsymlink/b", O_RDWR);
  if (fd2 < 0) {
    printf("error: open /testsymlink/b failed\n");
    exit(1);
  }
  r = read(fd2, buf, 4);
  if (r != 4) {
    printf("error: read failed\n");
    exit(1);
  }
  if (buf[0] != 'h' || buf[1] != 'e' || buf[2] != 'l' || buf[3] != 'l') {
    printf("error: read wrong bytes\n");
    exit(1);
  }
  close(fd2);

  if (stat("/testsymlink/b", &st) < 0) {
    printf("error: stat /testsymlink/b failed\n");
    exit(1);
  }
  if (st.type != T_FILE) {
    printf("error: /testsymlink/b is not a file\n");
    exit(1);
  }

  r = symlink("/testsymlink/b", "/testsymlink/c");
  if (r < 0) {
    printf("error: symlink /testsymlink/b -> /testsymlink/c failed\n");
    exit(1);
  }

  fd2 = open("/testsymlink/c", O_RDWR);
  if (fd2 < 0) {
    printf("error: open /testsymlink/c failed\n");
    exit(1);
  }
  r = read(fd2, buf, 4);
  if (r != 4) {
    printf("error: read failed\n");
    exit(1);
  }
  if (buf[0] != 'h' || buf[1] != 'e' || buf[2] != 'l' || buf[3] != 'l') {
    printf("error: read wrong bytes\n");
    exit(1);
  }
  close(fd2);

  r = symlink("/testsymlink/nonexistent", "/testsymlink/d");
  if (r < 0) {
    printf(
        "error: symlink /testsymlink/nonexistent -> /testsymlink/d failed\n");
    exit(1);
  }

  fd2 = open("/testsymlink/d", O_RDWR);
  if (fd2 >= 0) {
    printf("error: open /testsymlink/d succeeded\n");
    exit(1);
  }

  r = symlink("/testsymlink/c", "/testsymlink/1");
  if (r < 0) {
    printf("error: symlink /testsymlink/c -> /testsymlink/1 failed\n");
    exit(1);
  }
  r = symlink("/testsymlink/1", "/testsymlink/2");
  if (r < 0) {
    printf("error: symlink /testsymlink/1 -> /testsymlink/2 failed\n");
    exit(1);
  }
  r = symlink("/testsymlink/2", "/testsymlink/3");
  if (r < 0) {
    printf("error: symlink /testsymlink/2 -> /testsymlink/3 failed\n");
    exit(1);
  }

  fd2 = open("/testsymlink/3", O_RDWR);
  if (fd2 < 0) {
    printf("error: open /testsymlink/3 failed\n");
    exit(1);
  }
  r = read(fd2, buf, 4);
  if (r != 4) {
    printf("error: read failed\n");
    exit(1);
  }
  if (buf[0] != 'h' || buf[1] != 'e' || buf[2] != 'l' || buf[3] != 'l') {
    printf("error: read wrong bytes\n");
    exit(1);
  }
  close(fd2);

  // test O_NOFOLLOW
  fd2 = open("/testsymlink/b", O_RDWR | O_NOFOLLOW);
  if (fd2 >= 0) {
    printf("error: open /testsymlink/b with O_NOFOLLOW succeeded\n");
    exit(1);
  }

  // test cycles
  r = symlink("/testsymlink/cycle", "/testsymlink/cycle");
  if (r < 0) {
    printf("error: symlink /testsymlink/cycle -> /testsymlink/cycle failed\n");
    exit(1);
  }
  fd2 = open("/testsymlink/cycle", O_RDWR);
  if (fd2 >= 0) {
    printf("error: open /testsymlink/cycle succeeded\n");
    exit(1);
  }

  cleanup();
  printf("test symlinks: ok\n");
}

int main(int argc, char *argv[]) {
  testsymlinks();
  exit(0);
}