本仓库是 [xv6-riscv](https://github.com/mit-pdos/xv6-riscv) 操作系统的一个修改版本，用于完成大学操作系统课程的实验任务。

## 提交代码
请遵循以下步骤提交代码：
1. Clone 仓库到本地:
   ```bash
   git clone https://github.com/strayca7/xv6-riscv-ujs.git
2. 新建分支进行开发:
   ```bash
   git checkout -b feat/your-feature-name
   ```
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
## 附录: 相关资源
xv6 工具指南: https://pdos.csail.mit.edu/6.1810/2025/tools.html

xv6 设计手册: https://pdos.csail.mit.edu/6.828/2021/xv6/book-riscv-rev2.pdf