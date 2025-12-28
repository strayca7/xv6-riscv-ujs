#include "../kernel/stat.h"
#include "../kernel/types.h"
#include "user.h"

#define MAX_SEATS 5

// 辅助信号量
#define SEM_PRINT 99  // 打印锁信号量ID

// 业务逻辑信号量
#define SEM_SEATS 100 // 座位资源信号量ID
#define SEM_MUTEX 101 // 登记表互斥锁信号量ID

#define SHM_KEY 200 // 共享内存标识符

// 读者信息结构体
struct reader {
  int pid;        // 进程ID
  char phone[16]; // 身份信息
  int used;       // 是否被占用
};

// 阅览室共享数据结构
struct library {
  struct reader seats[MAX_SEATS];
  int reader_count;
};

// 简单的伪随机数生成器
unsigned int next_random = 1;

int rand(void) {
  next_random = next_random * 1103515245 + 12345;
  return (unsigned int)(next_random / 65536) % 32768;
}

void srand(unsigned int seed) { next_random = seed; }

void generate_phone(char *buf) {
  char *p = buf;
  // 格式: 138-XXXX-XXXX
  *p++ = '1';
  *p++ = '3';
  *p++ = '8';
  *p++ = '-';
  for (int i = 0; i < 4; i++)
    *p++ = '0' + (rand() % 10);
  *p++ = '-';
  for (int i = 0; i < 4; i++)
    *p++ = '0' + (rand() % 10);
  *p = '\0';
}

void reader_process() {
  struct library *lib;
  int my_pid = getpid();
  int i;
  int slot = -1;

  // 初始化随机种子
  srand(my_pid + uptime());

  // 打开共享内存
  lib = (struct library *)shm_open(SHM_KEY);
  if (lib == (void *)-1) {
    printfl("Reader %d: shm_open failed\n", my_pid);
    exit(1);
  }

  printfl("Reader %d is waiting for a seat...\n", my_pid);

  // P(Seats): 申请座位资源 (如果满了则阻塞)
  sem_p(SEM_SEATS);

  // P(Mutex): 申请修改登记表的权限 (互斥)
  sem_p(SEM_MUTEX);

  // 注册
  for (i = 0; i < MAX_SEATS; i++) {
    if (lib->seats[i].used == 0) {
      slot = i;
      lib->seats[i].used = 1;
      lib->seats[i].pid = my_pid;
      // 模拟填写个人信息
      generate_phone(lib->seats[i].phone);
      lib->reader_count++;
      break;
    }
  }

  if (slot != -1) {
    printfl("[Register] Reader %d took seat %d. Phone: %s. Total: %d\n", my_pid,
            slot, lib->seats[slot].phone, lib->reader_count);
  } else {
    printfl("Error: Reader %d acquired semaphore but no seat found!\n", my_pid);
  }

  // 4. V(Mutex): 释放登记表锁
  sem_v(SEM_MUTEX);

  // --- 阅读过程 (非临界区) ---
  printfl("Reader %d is reading...\n", my_pid);
  pause(10); // 模拟阅读时间

  // 5. P(Mutex): 申请修改登记表的权限 (互斥)
  sem_p(SEM_MUTEX);

  // --- 临界区：注销操作 ---
  if (slot != -1) {
    printfl("[Unregister] Reader %d leaving seat %d. Clearing info...\n",
            my_pid, slot);
    // 撤销个人信息
    lib->seats[slot].used = 0;
    lib->seats[slot].pid = 0;
    memset(lib->seats[slot].phone, 0, 16);
    lib->reader_count--;
    printfl("Reader %d left. Total: %d\n", my_pid, lib->reader_count);
  }

  // 6. V(Mutex): 释放登记表锁
  sem_v(SEM_MUTEX);

  // 7. V(Seats): 释放座位资源 (唤醒等待的读者)
  sem_v(SEM_SEATS);

  shm_close(SHM_KEY);
  exit(0);
}

int main(int argc, char *argv[]) {
  struct library *lib;
  int i;
  int num_readers = 8; // 模拟8个读者，座位只有5个

  // 初始化打印锁信号量 (必须最先创建)
  if (sem_create(SEM_PRINT, 1) < 0) {
    printf("Error creating SEM_PRINT\n");
    exit(1);
  }

  printfl("Library Simulation Started (5 Seats, %d Readers)\n", num_readers);

  // 初始化信号量
  // SEM_SEATS: 初始值为5，代表5个座位资源
  if (sem_create(SEM_SEATS, MAX_SEATS) < 0) {
    printfl("Error creating SEM_SEATS\n");
    exit(1);
  }
  // SEM_MUTEX: 初始值为1，代表互斥锁
  if (sem_create(SEM_MUTEX, 1) < 0) {
    printfl("Error creating SEM_MUTEX\n");
    exit(1);
  }

  // 初始化共享内存
  lib = (struct library *)shm_open(SHM_KEY);
  if (lib == (void *)-1) {
    printfl("Error creating shared memory\n");
    exit(1);
  }

  // 清空共享内存
  memset(lib, 0, sizeof(struct library));
  lib->reader_count = 0;

  // 创建读者进程
  for (i = 0; i < num_readers; i++) {
    int pid = fork();
    if (pid < 0) {
      printfl("Fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      reader_process();
    }
  }

  // 等待所有子进程结束
  for (i = 0; i < num_readers; i++) {
    wait(0);
  }

  printfl("All readers finished. Library closing.\n");

  // 清理资源
  shm_close(SHM_KEY);
  sem_free(SEM_SEATS);
  sem_free(SEM_MUTEX);
  sem_free(SEM_PRINT);

  exit(0);
}
