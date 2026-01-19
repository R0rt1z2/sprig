#pragma once

#include <stdint.h>
#include <stddef.h>

#define THEHEAP_BASE_ADDR       0x40070108
#define HEAP_START              0x4007F100
#define HEAP_END                0x7207F100

#define G_EXT_ALL_IN_ONE_SIG    0x400769e8
#define G_EXT_ALL_IN_ONE_SIG_SZ 0x400769e0

#define CMD_DPC_ADDR 0x40070030

struct cmd_dpc {
    const char *key;
    void (*cb)(void *);
    void *arg;
};

struct list_node {
    struct list_node *prev;
    struct list_node *next;
};

struct heap {
    void *base;
    size_t len;
    size_t remaining;
    size_t low_watermark;
    uint64_t lock;
    struct list_node free_list;
};

struct alloc_struct_begin {
    void *ptr;
    size_t size;
};

struct free_heap_chunk {
    struct list_node node;
    size_t len;
};

void heap_dump_freelist(void);
void heap_dump_chunk_header(void *user_ptr);
void heap_dump_aio_state(void);
void heap_dump_dpc(void);