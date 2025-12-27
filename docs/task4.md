## task4
> 题目：在内核中自建变量，实现缺页统计

> 创新：增加 COW（Copy-On-Write，写时复制）缺页统计功能

编译启动：
```bash
make qemu
```
运行 cowtest 测试程序：
```bash
cowtest
```
输出结果：
```
$ cowtest
simple: ok
three: ok
file: ok
ALL COW TESTS PASSED
page fault count: 98309
```
> 说明：测试程序采用 [MIT 6.828](https://pdos.csail.mit.edu/6.828/2020/labs/cow.html) 操作系统实验中的 COW 测试代码，运行结束后会输出当前进程的缺页中断次数。