# 环境搭建指南

## 快速开始（推荐）

### 方式1：GitHub Codespaces（在线，无需安装）

1. 打开 GitHub 仓库页面
2. 点绿色 **<> Code** → **Codespaces** → **Create codespace on main**
3. 等待 30 秒加载完成后，在终端运行：

```bash
sudo apt update && sudo apt install -y gcc-riscv64-linux-gnu qemu-system-misc
make run
```

### 方式2：本地 Linux / WSL2

```bash
# 一键运行
chmod +x setup.sh
./setup.sh
```

---

## 手动环境搭建

### 1. 安装 RISC-V 工具链

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y build-essential git
sudo apt-get install -y gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu

# 验证
riscv64-linux-gnu-gcc --version
```

### 2. 安装 QEMU

```bash
# Ubuntu/Debian
sudo apt-get install -y qemu-system-misc

# 验证
qemu-system-riscv64 --version
```

### 3. 构建项目

```bash
# 编译内核
make kernel

# 编译用户程序
make user

# 制作文件系统镜像
make fs.img

# 或一键构建全部
make
```

## 运行

```bash
make run
```

## 调试

```bash
# 终端1：启动 QEMU（带 GDB 支持）
make debug

# 终端2：连接 GDB
make gdb
```

## 项目结构说明

- `kernel/` - 内核源码
- `user/` - 用户态程序
- `tools/` - 构建工具（mkfs 制作文件系统镜像）
- `scripts/` - 调试脚本
- `docs/` - 文档
- `Makefile` - 顶层构建文件
- `fs.img` - 构建生成的文件系统镜像
