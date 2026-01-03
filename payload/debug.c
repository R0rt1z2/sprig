#define NANOPRINTF_IMPLEMENTATION
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_ALT_FORM_FLAG 0
    
#include <nanoprintf.h>

void pl_log_store(int ch, void* /* ctx */) {
    ((void (*)(char))(0x02047A20))((char)ch);
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int ret = npf_vpprintf(&pl_log_store, NULL, fmt, args);

    va_end(args);
    return ret;
}