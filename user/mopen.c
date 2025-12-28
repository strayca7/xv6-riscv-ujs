#include "user.h"

// If you enable ext monitoring, when you use cat to read a file,
// you will see two or more "[MONITOR] PID ..." lines printed to the console.
// Because cat reads the file in a loop until EOF,
// cat read 512 bytes each time, so multiple read operations occur,
// and the last "[MONITOR] PID ..." line corresponds to the last read
// operation which reached EOF.
// Read EOF is also a read operation and will be logged.
int main() {
  monitor(1);
  printf("Monitor enabled.\n");
  exit(0);
}