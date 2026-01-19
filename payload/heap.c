#include <heap.h>
#include <debug.h>

void heap_dump_freelist(void) {
    struct heap *theheap = (struct heap *)THEHEAP_BASE_ADDR;
    
    printf("theheap @ 0x%lx\n", (unsigned long)theheap);
    printf("  base      = 0x%lx\n", (unsigned long)theheap->base);
    printf("  len       = 0x%lx\n", (unsigned long)theheap->len);
    printf("  remaining = 0x%lx\n", (unsigned long)theheap->remaining);
    printf("  free_list.prev = 0x%lx\n", (unsigned long)theheap->free_list.prev);
    printf("  free_list.next = 0x%lx\n", (unsigned long)theheap->free_list.next);
}

void heap_dump_chunk_header(void *user_ptr) {
    if (!user_ptr) {
        printf("chunk: NULL\n");
        return;
    }
    
    struct alloc_struct_begin *as = (struct alloc_struct_begin *)user_ptr;
    as--;
    
    printf("chunk @ 0x%lx\n", (unsigned long)user_ptr);
    printf("  ptr  = 0x%lx\n", (unsigned long)as->ptr);
    printf("  size = 0x%lx\n", (unsigned long)as->size);

    printf("  raw:\n");
    hexdump((uint8_t *)as, 16, (uint64_t)as);

    printf("  data:\n");
    hexdump((uint8_t *)user_ptr, 256, (uint64_t)user_ptr);
}

void heap_dump_aio_state(void) {
    uint8_t *aio_buf = *(uint8_t **)G_EXT_ALL_IN_ONE_SIG;
    uint32_t aio_sz = *(uint32_t *)G_EXT_ALL_IN_ONE_SIG_SZ;
    
    printf("g_ext_all_in_one_sig = 0x%lx (sz=0x%x)\n", (unsigned long)aio_buf, aio_sz);
    
    if (aio_buf)
        heap_dump_chunk_header(aio_buf);
}

void heap_dump_dpc(void) {
    struct cmd_dpc *dpc = (struct cmd_dpc *)CMD_DPC_ADDR;
    
    printf("cmd_dpc @ 0x%lx\n", (unsigned long)dpc);
    printf("  key = 0x%lx", (unsigned long)dpc->key);
    if (dpc->key)
        printf(" (\"%s\")", dpc->key);
    printf("\n");
    printf("  cb  = 0x%lx\n", (unsigned long)dpc->cb);
    printf("  arg = 0x%lx\n", (unsigned long)dpc->arg);

    printf("  cb raw:\n");
    hexdump((uint8_t *)dpc->cb, 128, (uint64_t)dpc->cb);
}