// Simple reading-room (阅览室) data model and API.
// - ROOM_CAPACITY: number of seats available
// - ROOM_INFOLEN: max bytes stored per registered reader
// Functions provide kernel-side operations used by syscalls.

#ifndef ROOM_H
#define ROOM_H

#define ROOM_CAPACITY 5
#define ROOM_INFOLEN 64

// Initialize room structures and semaphores; idempotent.
int room_init(void);

// Register a reader: copies 'info' into an available seat and
// returns seat index, or -1 if failed. Uses counting semaphore
// to wait for an available seat.
int kroom_register(const char *info);

// Unregister reader at seat index 'seat'. Returns 0 on success.
int kroom_unregister(int seat);

// Copy a textual list of seats into user buffer 'buf' of length
// 'len'. Returns number of bytes written or -1 on error.
int kroom_list(char *buf, int len);

#endif
