#!/bin/bash
# setup.sh - Install dependencies and build ZephyrOS
# For Ubuntu 20.04 / 22.04 / WSL2

set -e

echo "=== ZephyrOS Build Setup ==="

# Install RISC-V toolchain and QEMU
echo "[1/3] Installing RISC-V toolchain and QEMU..."
sudo apt-get update
sudo apt-get install -y gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu qemu-system-misc

# Verify
echo "[2/3] Verifying toolchain..."
riscv64-linux-gnu-gcc --version
qemu-system-riscv64 --version

# Build
echo "[3/3] Building ZephyrOS kernel..."
make clean
make

echo ""
echo "=== Build complete! ==="
echo "Run with:  make run"
echo "Debug with: make debug  (then connect GDB with 'make gdb')"
