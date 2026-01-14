#pragma once

#include <stdarg.h>
#include <debug.h>

enum {
    LOG_OFF = 0,
    LOG_ON  = 1,
};

int printf(const char* fmt, ...);
void set_log_switch(int enable);