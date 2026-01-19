#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <debug.h>

enum {
    LOG_OFF = 0,
    LOG_ON  = 1,
};

int printf(const char* fmt, ...);
void hexdump(const void *data, size_t size, uint64_t base_addr);
void set_log_switch(int enable);