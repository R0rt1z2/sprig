TOOLCHAIN_DIR := ../arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf
TOOLCHAIN_PATH := $(shell pwd)/$(TOOLCHAIN_DIR)/bin
export PATH := $(TOOLCHAIN_PATH):$(PATH)

CC := aarch64-none-elf-gcc
LD := aarch64-none-elf-ld
OBJCOPY := aarch64-none-elf-objcopy

SRC_DIR := .
INC_DIR := include
BUILD_DIR := build

CFLAGS := -O2 -ffreestanding -nostdlib -fno-omit-frame-pointer
CFLAGS += -fno-builtin -fno-builtin-printf
CFLAGS += -mgeneral-regs-only -DWITH_NO_FP=1
CFLAGS += -I$(INC_DIR)

LDFLAGS := -T linker.ld -nostdlib

TARGET := payload.bin
ELF := $(BUILD_DIR)/payload.elf

C_SOURCES := main.c debug.c patches.c bldr.c
ASM_SOURCES := entry.S

C_OBJS := $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
ASM_OBJS := $(ASM_SOURCES:%.S=$(BUILD_DIR)/%.o)
OBJS := $(C_OBJS) $(ASM_OBJS)

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(ELF)
	$(OBJCOPY) -O binary $< $@
	@echo "Built: $(TARGET)"

$(ELF): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean