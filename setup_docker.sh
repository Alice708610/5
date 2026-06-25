#!/bin/bash
# ==============================================
# ZephyrOS - Docker 环境下一键运行脚本
# ==============================================

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}"
echo "============================================="
echo "  ZephyrOS - Docker 编译运行"
echo "============================================="
echo -e "${NC}"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# 创建 Docker 镜像
echo -e "${YELLOW}[1/3] 构建 Docker 镜像...${NC}"

docker build -t zephyros-builder -f- "$SCRIPT_DIR" <<'DOCKERFILE'
FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        gcc-riscv64-linux-gnu \
        binutils-riscv64-linux-gnu \
        qemu-system-misc \
        && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
DOCKERFILE

echo -e "${GREEN}  -> Docker 镜像构建完成${NC}"

# 编译
echo -e "${YELLOW}[2/3] 编译 ZephyrOS 内核...${NC}"
docker run --rm -v "$SCRIPT_DIR:/workspace" zephyros-builder \
    bash -c "cd /workspace && make clean 2>/dev/null; make"

echo -e "${GREEN}  -> 编译完成${NC}"

# 运行
echo ""
echo -e "${GREEN}============================================="
echo "  ZephyrOS 编译成功! 启动 QEMU..."
echo "  按 Ctrl+A 然后按 X 退出"
echo "============================================="
echo -e "${NC}"

docker run --rm -it -v "$SCRIPT_DIR:/workspace" zephyros-builder \
    bash -c "cd /workspace && make run"
