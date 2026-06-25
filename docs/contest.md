# ZephyrOS 参赛文档

## 项目概述

**ZephyrOS** 是为参加 **2026年全国大学生计算机系统能力大赛 - 操作系统设计赛（OS内核实现赛道）** 开发的 RISC-V 64 位操作系统内核。项目名称 "Zephyr"（和风）寓意轻量、快速、优雅的设计理念。

## 技术亮点

### 1. 基于 RISC-V 64 架构
- 完整支持 RV64GC 指令集
- 支持 M-Mode、S-Mode、U-Mode 三级特权模式
- 基于 OpenSBI 固件启动
- M-Mode 中断委托机制

### 2. SV39 虚拟内存管理
- 三级页表映射（512 项/级 × 9 位索引）
- 内核 Identity Mapping + 高地址映射
- 跳板页（trampoline）用于原子级用户态-内核态切换
- 用户空间支持 256TB 虚拟地址空间

### 3. 多进程调度
- 进程控制块（PCB）管理，最大 64 进程
- 基于 Round-Robin 的抢占式调度（10ms 时间片）
- 完整的 fork/exec/wait/exit 进程生命周期
- 进程树管理：孤儿进程自动收养
- 零开销上下文切换（swtch.S）

### 4. 日志文件系统
- VFS（虚拟文件系统）抽象层
- 写前日志（WAL）保证崩溃一致性
- 块缓存（Buffer Cache）LRU 淘汰策略
- 支持目录、文件、硬链接、设备文件
- inode 双层缓存（磁盘 + 内存）

### 5. 中断与异常处理
- PLIC 平台级中断控制器
- UART 16550 中断驱动串口
- VIRTIO MMIO 块设备驱动
- CLINT 定时器驱动（10ms 周期）
- 系统调用（21 个 POSIX 兼容接口）

### 6. 同步机制
- 自旋锁（Spinlock）：关中断保护，支持嵌套
- 睡眠锁（Sleeplock）：长临界区保护
- 管道（Pipe）：512B 环形缓冲区 IPC

## 支持的 POSIX 系统调用

| 编号 | 系统调用 | 功能 | 分类 |
|------|----------|------|------|
| 1 | fork | 创建子进程 | 进程 |
| 2 | exit | 终止进程 | 进程 |
| 3 | wait | 等待子进程 | 进程 |
| 4 | pipe | 创建管道 | IPC |
| 5 | read | 读取文件 | I/O |
| 6 | kill | 发送信号 | 进程 |
| 7 | exec | 执行程序 | 进程 |
| 8 | fstat | 获取文件状态 | 文件 |
| 9 | chdir | 切换目录 | 文件 |
| 10 | dup | 复制文件描述符 | I/O |
| 11 | getpid | 获取进程ID | 进程 |
| 12 | sbrk | 扩展堆空间 | 内存 |
| 13 | sleep | 睡眠 | 进程 |
| 14 | uptime | 系统运行时间 | 系统 |
| 15 | open | 打开文件 | 文件 |
| 16 | write | 写入文件 | I/O |
| 17 | mknod | 创建设备节点 | 文件 |
| 18 | unlink | 删除文件 | 文件 |
| 19 | link | 创建硬链接 | 文件 |
| 20 | mkdir | 创建目录 | 文件 |
| 21 | close | 关闭文件 | I/O |

## 内存布局

```
物理地址空间 (QEMU virt):
  0x00000000 - 0x0FFFFFFF : 保留
  0x10000000              : UART0 (16550)
  0x10001000              : VIRTIO MMIO
  0x02000000              : CLINT (定时器)
  0x0C000000              : PLIC (中断控制器)
  0x80000000              : 物理内存起始 (DRAM)
  0x88000000              : 物理内存结束 (128MB)

虚拟地址空间 (SV39):
  0x0000000000000000      : 用户代码/数据
  0x0000003FFFFFFFFF      : 用户栈 (向下增长)
  0x0000004000000000      : USERTOP
  0x0000003FFFFFE000      : TRAMPOLINE (跳板页)
  0x0000003FFFFFD000      : TRAPFRAME (陷入帧)
  0xFFFFFFFFC0000000      : 内核空间 (高地址映射)
```

## 性能指标

| 指标 | 数值 |
|------|------|
| 上下文切换时间 | < 1μs (QEMU 模拟) |
| 系统调用延迟 | < 2μs |
| 物理内存分配 | O(1) 时间复杂度 |
| 最大进程数 | 64 |
| 最大文件描述符 | 16/进程, 128/系统 |
| 文件系统块大小 | 1024 字节 |
| 调度时间片 | 10ms |

## 测试方法

```bash
# 启动内核
make run

# 在 Shell 中执行测试
$ echo Hello ZephyrOS!
$ ls /
$ cat /README
$ mkdir /testdir
$ cd /testdir
$ uptime
```

## 构建与运行

```bash
# 一键安装运行
chmod +x setup.sh && ./setup.sh

# 或手动构建
make clean && make && make run

# 调试
make debug    # 终端1
make gdb      # 终端2
```

## 参考资料

1. RISC-V Privileged Specification v1.12
2. The RISC-V Instruction Set Manual Volume II
3. xv6-riscv (MIT 6.S081)
4. rCore-Tutorial-Book (清华大学)
5. 2026 全国大学生计算机系统能力大赛技术方案
