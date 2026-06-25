FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Install RISC-V cross-compiler toolchain, QEMU, and build tools
RUN sudo apt-get update && sudo apt-get install -y \
    gcc-riscv64-linux-gnu \
    binutils-riscv64-linux-gnu \
    qemu-system-misc \
    gcc make gdb \
    libc6-dev \
    && sudo rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspaces/ZephyrOS
