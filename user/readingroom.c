// readingroom.c
// 用户态演示程序：创建多个读者进程并使用内核阅览室(syscalls)
// 流程：每个子进程执行注册(room_register)，等待一段时间，
// 然后注销(room_unregister)。父进程等待所有子进程结束，
// 最后调用 room_list 打印阅览室状态。

#include "kernel/types.h"
#include "user/user.h"

#define READERS 8

int
main(int argc, char *argv[])
{
  int i;
  for(i = 0; i < READERS; i++){
    int pid = fork();
    if(pid == 0){
      // 构造简短身份信息字符串（例如 "pid:3"）
      char info[64];
      int mypid = getpid();
      int x = mypid;
      int pos = 0;
      info[pos++] = 'p'; info[pos++] = 'i'; info[pos++] = 'd'; info[pos++] = ':';
      char tmp[16];
      int tp = 0;
      if(x == 0) tmp[tp++] = '0';
      while(x > 0){ tmp[tp++] = '0' + (x % 10); x /= 10; }
      while(tp > 0) info[pos++] = tmp[--tp];
      info[pos] = '\0';

      // 注册到阅览室，得到座位号或-1
      int seat = room_register(info);
      if(seat >= 0){
        printf("[reader %d] registered at seat %d\n", mypid, seat);
        // 模拟阅读：暂停若干时钟滴答
        pause(50 + (mypid % 20));
        // 注销座位
        room_unregister(seat);
        printf("[reader %d] unregistered from seat %d\n", mypid, seat);
      } else {
        printf("[reader %d] failed to register (full)\n", mypid);
      }
      exit(0);
    }
  }

  // 父进程等待所有子进程完成
  for(i = 0; i < READERS; i++) wait(0);

  // 打印最终阅览室状态
  char buf[512];
  int n = room_list(buf, sizeof(buf));
  if(n >= 0) printf("Final room state:\n%s", buf);
  return 0;
}
