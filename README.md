# ZephyrOS — RISC-V 64-bit OS Kernel

**2026 全国大学生计算机系统能力大赛 · OS 内核实现赛道**

ZephyrOS 是一个从零开始编写的精简 RISC-V 64 位操作系统内核，用于在 QEMU `virt` 机器上运行。它实现了：

- ✅ 通过 UART 控制台进行输入/输出
- ✅ RISC-V SV39 虚拟内存（3 级页表，512GB 虚拟地址空间）
- ✅ 基于 `fork()` 和 `exec()` 的进程管理
- ✅ 系统调用（`write`、`read`、`fork`、`exit`、`wait`、`getpid`、`sleep`、`uptime`）
- ✅ 抢占式循环调度（计时器中断触发 `yield()`）
- ✅ 内核陷阱和用户陷阱处理
- ✅ 带锁的同步机制

**设计灵感**：灵感来源于 xv6-riscv (MIT 6.S081)，但代码为全新编写，仅保留最基础的功能，以确保无错误运行。

---

## 快速开始

### 前提条件（WSL / Linux / GitHub Codespaces）

```bash
# 安装 RISC-V 工具链和 QEMU
sudo apt-get update
sudo apt-get install -y gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu qemu-system-misc
```

### 编译

```bash
git clone https://github.com/你的用户名/ZephyrOS.git
cd ZephyrOS
make clean
make
```

### 在 QEMU 中运行

```bash
make run
```

您将看到：

```
ZephyrOS 1.0 - RISC-V 64-bit OS Kernel
2026 National Computer System Capability Competition

fork succeeded: parent PID=1, child PID=2
ZephyrOS user shell started.
>
```

现在输入字符，它们将会被回显。按 `q` 退出 `init` 进程（这会触发 panic，因为 init 进程不应退出）。

### 调试

```bash
make debug    # 在调试模式下启动 QEMU（等待 GDB 连接端口 1234）
make gdb      # 在另一个终端中启动 GDB
```

---

## 项目结构

```
ZephyrOS/
├── kernel/                # 内核源代码
│   ├── entry.S           # 内核入口点（M 模式 → S 模式切换）
│   ├── start.c           # 早期初始化（M 模式CSR、计时器）
│   ├── main.c           # 内核主函数（驱动初始化）
│   ├── trap.c           # 陷阱处理（用户/内核）
│   ├── trapoline.S      # 用户/内核转换代码
│   ├── kernelvec.S      # 内核陷阱向量
│   ├── swtch.S         # 上下文切换（内核线程）
│   ├── proc.c          # 进程管理（fork、exec、wait、exit）
│   ├── vm.c            # 虚拟内存（SV39 页表）
│   ├── kalloc.c        # 物理页面分配器
│   ├── spinlock.c      # 自旋锁
│   ├── syscall.c       # 系统调用调度
│   ├── sysproc.c       # 进程相关系统调用
│   ├── uart.c          # NS16550A UART 驱动
│   ├── plic.c         # PLIC 中断控制器驱动
│   ├── console.c       # 控制台 I/O（行缓冲）
│   ├── printf.c       # 内核 printf 实现
│   ├── string.c       # 内存/字符串工具函数
│   ├── initcode.S     # 第一个用户程序（嵌入式）
│   ├── kernel.ld       # 内核链接器脚本
│   └── Makefile       # 内核构建规则
├── Makefile            # 顶层构建规则
├── .github/
│   └── workflows/
│       └── build.yml   # GitHub Actions CI
└── README.md
```

---

## 系统调用

| 编号 | 名称   | 描述                     |
|------|--------|--------------------------|
| 1    | fork   | 创建子进程             |
| 2    | exit   | 终止当前进程             |
| 3    | wait   | 等待子进程退出           |
| 4    | getpid | 获取当前进程 ID          |
| 5    | kill   | 终止进程（部分实现）   |
| 6    | sbrk   | 调整进程内存大小         |
| 7    | sleep  | 休眠指定秒数（基于计时器）|
| 8    | uptime | 获取系统运行时间（ticks）|
| 9    | write  | 写入文件描述符           |
| 10   | read   | 读取文件描述符           |
| 11   | exec   | 执行程序（存根）       |

---

## 已知限制

1. **无文件系统** — 此精简版本不包含磁盘或文件系统。第一个用户进程（`initcode`）直接嵌入内核。
2. **无 `exec()`** — `exec` 系统调用是存根（返回 -1）。要运行新程序，需要添加文件系统。
3. **单用户控制台** — 仅支持 UART 控制台 I/O。
4. **无网络** — 未实现网络驱动。

---

## 比赛要求对照

- ✅ RISC-V 64 位（RV64GC）
- ✅ SV39 页表
- ✅ M/S/U 模式切换
- ✅ 进程管理（fork/wait/exit）
- ✅ 系统调用
- ✅ 中断处理（计时器 + 外部）
- ✅ 可在 QEMU 上运行
- ✅ 可使用 GitHub Actions 进行 CI 构建

---

## 许可证

MIT 许可证。
