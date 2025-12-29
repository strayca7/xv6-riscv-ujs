#include "../kernel/types.h"
#include "../kernel/fcntl.h"
#include "user.h"

int main() {
  printf("Loading driver...\n");
  if (load_ramdisk() < 0) {
    printf("Error: load failed\n");
    exit(1);
  }

  printf("Creating device node...\n");
  mknod("ramdisk", 2, 0); // major 2 for Ramdisk

  int fd = open("ramdisk", O_RDWR);
  if (fd < 0) {
    printf("Error: open failed\n");
    exit(1);
  }

  printf("Writing data...\n");
  char *msg = "Dynamic Ramdisk Test";
  if (write(fd, msg, strlen(msg)) != strlen(msg)) {
    printf("Error: write failed\n");
  }

  // Seek to 0 (close and reopen is the simplest way in xv6 without lseek)
  close(fd);
  fd = open("ramdisk", O_RDWR);

  printf("Reading data...\n");
  char buf[64];
  memset(buf, 0, sizeof(buf));
  if (read(fd, buf, sizeof(buf)) < 0) {
    printf("Error: read failed\n");
  } else {
    printf("Read back: %s\n", buf);
  }
  
  close(fd);

  printf("Unloading driver...\n");
  unload_ramdisk();
  
  exit(0);
}