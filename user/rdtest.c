#include "../kernel/fcntl.h"
#include "../kernel/types.h"
#include "user.h"

#define MB (1024 * 1024)
#define CHUNK_SIZE (1024 * 1024) // 1MB
#define TOTAL_SIZE (256 * MB)
#define CHUNKS (TOTAL_SIZE / CHUNK_SIZE)

int main() {
  int fd;
  char *buf;
  int i, j;

  printf("Ramdisk 256MB Test\n");

  printf("[1/5] Loading driver...\n");
  if (load_ramdisk() < 0) {
    printf("Error: load failed (maybe already loaded?)\n");
  }

  printf("[2/5] Creating device node...\n");
  unlink("ramdisk");
  if (mknod("ramdisk", 2, 0) < 0) {
    printf("Error: mknod failed\n");
    exit(1);
  }

  buf = malloc(CHUNK_SIZE);
  if (buf == 0) {
    printf("Error: malloc failed\n");
    exit(1);
  }
  memset(buf, 0, CHUNK_SIZE); // Force allocation of physical pages for buffer

  printf("[3/5] Writing 256MB data...\n");
  fd = open("ramdisk", O_RDWR);
  if (fd < 0) {
    printf("Error: open failed\n");
    exit(1);
  }

  for (i = 0; i < CHUNKS; i++) {
    // Fill buffer with pattern
    for (j = 0; j < CHUNK_SIZE; j++) {
      buf[j] = (char)((i + j) & 0xff);
    }

    int n = write(fd, buf, CHUNK_SIZE);
    if (n != CHUNK_SIZE) {
      printf("\nWrite failed at chunk %d, wrote %d\n", i, n);
      exit(1);
    }
    if (i % 16 == 0) {
      printf("Writing: %d/%d MB\n", i, CHUNKS);
    }
  }
  printf("Writing: %d/%d MB... Done.\n", CHUNKS, CHUNKS);
  close(fd);

  printf("[4/5] Reading and Verifying 256MB data...\n");
  fd = open("ramdisk", O_RDWR);
  if (fd < 0) {
    printf("Error: open failed\n");
    exit(1);
  }

  for (i = 0; i < CHUNKS; i++) {
    int n = read(fd, buf, CHUNK_SIZE);
    if (n != CHUNK_SIZE) {
      printf("\nRead failed at chunk %d, read %d\n", i, n);
      exit(1);
    }

    // Verify
    for (j = 0; j < CHUNK_SIZE; j++) {
      if (buf[j] != (char)((i + j) & 0xff)) {
        printf("\nData mismatch at chunk %d, byte %d: expected %x got %x\n", i,
               j, (i + j) & 0xff, buf[j]);
        exit(1);
      }
    }
    if (i % 16 == 0) {
      printf("Verifying: %d/%d MB\n", i, CHUNKS);
    }
  }
  printf("Verifying: %d/%d MB... Passed.\n", CHUNKS, CHUNKS);
  close(fd);

  printf("[5/5] Unloading driver...\n");
  unload_ramdisk();

  free(buf);
  printf("Test Passed!\n");
  exit(0);
}