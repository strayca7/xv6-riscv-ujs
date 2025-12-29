本仓库是 [xv6-riscv](https://github.com/mit-pdos/xv6-riscv) 操作系统的一个修改版本，用于完成大学操作系统课程的实验任务。

## 提交代码
请遵循以下步骤提交代码：
1. Clone 仓库到本地:
   ```bash
   git clone https://github.com/strayca7/xv6-riscv-ujs.git
   git checkout main
   ```
   *实验将基于 main 分支进行开发。*
2. 新建分支进行开发:
   ```bash
   git checkout -b feat/your-feature-name origin/main
   ```
   > 如果已经创建了分支，但忘记关联上游，可以使用以下命令补救：
   >```bash
   > # 将当前分支关联到远程的 main 分支
   >git branch -u origin/main
   >```
3. 完成代码修改后，提交代码:
   ```bash
   git add .
   git commit -m "feat: add your feature description"
   ```
4. Push 分支到远程仓库:
   ```bash
   git push origin feat/your-feature-name
   ```
5. 在 GitHub 上创建 Pull Request，描述你的修改内容。
> 注意：riscv 分支以及 main 分支为非直接修改分支。riscv 分支为原分支 fork 而来，main 分支为课程实验修改分支。**请勿直接在 main 分支上进行修改**。
## 使用说明
### 安装依赖
在基于 Debian 或 Ubuntu 的系统上，可以使用以下命令安装所需的依赖项：
```bash
sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu 
```
### 构建和运行 xv6
```bash
make qemu
```
### 退出 xv6
在 QEMU 窗口中，按下 `Ctrl + A`，然后按 `X` 退出 xv6。
## 运行测试程序
各实验任务的测试程序和运行说明请参见 [`docs/`](https://github.com/strayca7/xv6-riscv-ujs/tree/main/docs)下的相应文档文件。
## 附录: 相关资源
xv6 工具指南: https://pdos.csail.mit.edu/6.1810/2025/tools.html

xv6 设计手册: https://pdos.csail.mit.edu/6.828/2021/xv6/book-riscv-rev2.pdf
## 注意事项
xv6 源码未遵循 POSIX 标准，在常用 IDE 中可能会出现语法错误提示，但不影响编译和运行。可使用 `.vscode/` 目录下的 `settings.json` 文件配置 VSCode 以减少错误提示。
