// System call numbers
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_pipe    4
#define SYS_read    5
#define SYS_kill    6
#define SYS_exec    7
#define SYS_fstat   8
#define SYS_chdir   9
#define SYS_dup    10
#define SYS_getpid 11
#define SYS_sbrk   12
#define SYS_pause  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_mknod  17
#define SYS_unlink 18
#define SYS_link   19
#define SYS_mkdir  20
#define SYS_close  21

// Task 1: cube syscall
#define SYS_cube        22

// Task 4: getpgfault syscall
#define SYS_getpgfault  23

// Task 5: shared memory and semaphore syscalls
#define SYS_shm_open    24
#define SYS_shm_close   25
#define SYS_sem_create  26
#define SYS_sem_free    27
#define SYS_sem_p       28
#define SYS_sem_v       29


// Task 2: monitor syscall and symlink syscall
#define SYS_monitor     30
#define SYS_symlink     31

// Task 4: ramdisk
#define SYS_load_ramdisk      32
#define SYS_unload_ramdisk    33
