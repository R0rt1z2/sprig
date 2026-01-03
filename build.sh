#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf.tar.xz"
TOOLCHAIN_ARCHIVE="arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf.tar.xz"
TOOLCHAIN_DIR="arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf"
TOOLCHAIN_PATH="$(pwd)/$TOOLCHAIN_DIR/bin"

if [ ! -d "$TOOLCHAIN_PATH" ]; then
    echo "Downloading toolchain..."
    wget -O "$TOOLCHAIN_ARCHIVE" "$TOOLCHAIN_URL"
    
    echo "Extracting toolchain..."
    tar -xf "$TOOLCHAIN_ARCHIVE"
    rm -f "$TOOLCHAIN_ARCHIVE"
fi

export PATH="$TOOLCHAIN_PATH:$PATH"

cd payload
make clean
make