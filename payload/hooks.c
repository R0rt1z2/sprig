#include <hooks.h>
#include <mmio.h>
#include <heap.h>
#include <debug.h>

enum {
    TRAMP_STR_LR  = 0xF81F0FFE,
    TRAMP_LDR_X16 = 0x58000090,
    TRAMP_BLR_X16 = 0xD63F0200,
    TRAMP_LDR_LR  = 0xF84107FE,
    TRAMP_RET     = 0xD65F03C0,
};

// variable that will hold the address of the chunk we send the second payload to (through USB)
static uintptr_t usb_malloced_chunk_addr = 0;

// variable that will hold the address of the chunk that comes after the one we overflow (USB chunk -> NEXT chunk)
static uintptr_t usb_overflowed_chunk_addr = 0;

// variable that will hold the address of the mxml_escape chunk
static uintptr_t mxml_escape_chunk_addr = 0;

// variable that will hold the address of the chunk that comes after the mxml_escape chunk
static uintptr_t mxml_escape_overflowed_chunk_addr = 0;

bool hook_install(const hook_t *hook) {
    if (!hook || !hook->handler)
        return false;

    uint32_t bl_insn = encode_bl(hook->site, hook->trampoline);
    if (bl_insn == 0) {
        printf("%-30s out of range\n", hook->name);
        return false;
    }

    uintptr_t target = (uintptr_t)hook->handler;

    writel(TRAMP_STR_LR,                    hook->trampoline + 0x00);
    writel(TRAMP_LDR_X16,                   hook->trampoline + 0x04);
    writel(TRAMP_BLR_X16,                   hook->trampoline + 0x08);
    writel(TRAMP_LDR_LR,                    hook->trampoline + 0x0C);
    writel(TRAMP_RET,                       hook->trampoline + 0x10);
    writel((uint32_t)(target & 0xFFFFFFFF), hook->trampoline + 0x14);
    writel((uint32_t)(target >> 32),        hook->trampoline + 0x18);

    writel(bl_insn, hook->site);

    flush_dcache_range(hook->trampoline, HOOK_TRAMPOLINE_SIZE);
    flush_dcache_range(hook->site, sizeof(uint32_t));
    invalidate_icache();

    printf("%-30s 0x%08x -> 0x%08x\n", hook->name, hook->site, hook->trampoline);
    return true;
}

static void *mxml_escape_malloc_hook(void) {
    register uint64_t size asm("x0");
    uint64_t saved_size = size;
    
    // this is the buffer used by mxml_escape to store escaped data. It has a size of
    // 0x200 bytes, but it can be overflown if we pass a lot of special characters so
    // they get expanded.
    void *result = ((void *(*)(uint64_t))0x40009A48)(saved_size);
    
    printf("mxml_escape_malloc(0x%lx) = 0x%lx\n", saved_size, (unsigned long)result);
    heap_dump_chunk_header(result);

    // save the chunk address for later analysis
    mxml_escape_chunk_addr = (uintptr_t)((uintptr_t)result - 0x10);
    mxml_escape_overflowed_chunk_addr = mxml_escape_chunk_addr + saved_size + 0x10;

    // dump the chunk that comes after this one
    struct free_heap_chunk *next_chunk = (struct free_heap_chunk *)mxml_escape_overflowed_chunk_addr;
    printf("next chunk at 0x%lx\n", (unsigned long)next_chunk);
    printf("  prev=0x%lx next=0x%lx len=0x%lx\n", // walk down..
        (unsigned long)next_chunk->node.prev,
        (unsigned long)next_chunk->node.next,
        (unsigned long)next_chunk->len);
    
    return result;
}

static void *mxml_escape_hook(void) {
    register char *src asm("x0");
    register uint32_t len asm("w1");
    
    char *saved_src = src;
    uint32_t saved_len = len;
    
    char *result = ((char *(*)(char *, uint32_t))0x4002A520)(saved_src, saved_len);
    
    printf("mxml_escape(\"%s\", %u) = 0x%lx\n", saved_src, saved_len, (unsigned long)result);
    printf("  expanded: \"%s\"\n", result);
    
    // dump the mxml_escape buffer to see if we overflowed
    if (mxml_escape_chunk_addr != 0) {
        printf("mxml_escape buffer at 0x%lx:\n", mxml_escape_chunk_addr + 0x10);
        hexdump((void *)(mxml_escape_chunk_addr + 0x10), 0x220, mxml_escape_chunk_addr + 0x10);
        
        // check what's after the 0x200 buffer
        printf("chunk after mxml_escape buffer:\n");
        hexdump((void *)mxml_escape_overflowed_chunk_addr, 0x40, mxml_escape_overflowed_chunk_addr);
    }
    
    return result;
}

static void *malloc_for_file_hook(void) {
    // we only care about the second call
    static int call_count = 0;
    call_count++;

    register uint64_t size asm("x0");
    uint64_t saved_size = size;
    
    void *result = ((void *(*)(uint64_t))0x40009A48)(saved_size);
    
    // The host tells the DA we'll receive X bytes, so this hook
    // will receive X + 4 as the size parameter (da does this internally).
    printf("\n\n\nmalloc_for_file(0x%lx) = 0x%lx \n\n",
            saved_size, (unsigned long)result);
    
    // Malloc gives us a pointer to the chunk data, not to the pointer header. 
    // In addition to that, the header size will be 0x10 bytes bigger than what
    // we requested because of the chunk metadata:
    //    void *ptr; // 8 bytes
    //    size_t size; // 8 bytes
    heap_dump_chunk_header(result);

    // if we're in the error path..
    if (call_count == 2) {
        void *chunk_base = (void *)((uintptr_t)result - 0x10);
        size_t chunk_size = saved_size + 0x10;
        struct free_heap_chunk *next_chunk = (struct free_heap_chunk *)((uintptr_t)chunk_base + chunk_size);
        // we still haven't overflowed yet!!
        printf("next chunk at 0x%lx\n", (unsigned long)next_chunk);
        printf("  prev=0x%lx next=0x%lx len=0x%lx\n", // walk down..
            (unsigned long)next_chunk->node.prev,
            (unsigned long)next_chunk->node.next,
            (unsigned long)next_chunk->len);
        
        usb_malloced_chunk_addr = (uintptr_t)chunk_base;
        usb_overflowed_chunk_addr = (uintptr_t)next_chunk;

        // apparently the next chunk has been overflowed already by the XML expansion
        // payload?
        hexdump((void *)usb_overflowed_chunk_addr, 0x100, usb_overflowed_chunk_addr);
    }

    heap_dump_freelist();
    
    return result;
}

static void free_on_abort_hook(void) {
    register void *ptr asm("x0");
    void *saved_ptr = ptr;
    
    printf("\n\n\nfree_on_abort\n\n");
    heap_dump_chunk_header(saved_ptr);
    heap_dump_freelist();
    
    ((void (*)(void *))0x40009EB8)(saved_ptr);

    heap_dump_freelist();

    // DPC is at 0x40070028, it should've been ovewritten by now...
    printf("DPC region:\n");
    hexdump((void *)0x40070020, 0x40, 0x40070020);
    printf("returned from original free_on_abort\n\n\n");   
}

static void error_path_hook(void) {
    register void *msg asm("x0");
    void *saved_msg = msg;

    printf("\n\n\nerror_path: %s\n\n", (char *)saved_msg);

    // usb_malloced_chunk_addr holds the malicious data we sent that overflows
    // we dump the data of this chunk (not the header) to see what we wrote
    if (usb_malloced_chunk_addr != 0) {
        printf("malicious chunk data at: 0x%lx\n", (unsigned long)usb_malloced_chunk_addr);
        hexdump((void *)(usb_malloced_chunk_addr + 0x10), 0x40,
                (uint64_t)(usb_malloced_chunk_addr + 0x10));
    }

    // at this point, we overflowed the chunk that comes after the one
    // we previously allocated to store the file data
    //
    // allegedly, the chunk after that is a freelist chunk, which we
    // overflowed the header of
    if (usb_overflowed_chunk_addr != 0) {
        printf("overflowed chunk at: 0x%lx\n", (unsigned long)usb_overflowed_chunk_addr);
        hexdump((void *)usb_overflowed_chunk_addr, 0x40,
                (uint64_t)usb_overflowed_chunk_addr);
        heap_dump_chunk_header((void *)(usb_overflowed_chunk_addr));
    }
    heap_dump_freelist();

    ((void (*)(void *))0x400090E8)(saved_msg);
}

static void dpc_call_hook(void) {
    printf("DPC call hook\n");

    // If everything went well, the DPC region should now have a pointer to
    // our shellcode, which was sent in the first AIO packet.
    hexdump((void *)0x5A000000-0x100, 0x200, 0x5A000000-0x100);
    heap_dump_dpc();
}

static void mxml_free_hook(void) {
    register void *node asm("x0");
    void *saved_node = node;
    
    printf("mxml_free: 0x%lx\n", (unsigned long)saved_node);
    ((void (*)(void *))0x4000FBFC)(saved_node);
}

static void da2_init_hook(void) {
    printf("DA2 init hook\n");

    hook_install(&(hook_t)HOOK(0x4000749C, 0x400066AC, free_on_abort_hook, "free_on_abort"));
    hook_install(&(hook_t)HOOK(0x4002AAB0, 0x4000687C, malloc_for_file_hook, "malloc_for_file"));
    hook_install(&(hook_t)HOOK(0x4002A9F0, 0x400067BC, error_path_hook, "error_path_1"));
    hook_install(&(hook_t)HOOK(0x4002AA88, 0x400068CC, error_path_hook, "error_path_2"));
    hook_install(&(hook_t)HOOK(0x4000987C, 0x4000671C, dpc_call_hook, "dpc_call"));
    hook_install(&(hook_t)HOOK(0x4000FBE0, 0x4000693C, mxml_free_hook, "mxml_free"));
    hook_install(&(hook_t)HOOK(0x4002A554, 0x40006B9C, mxml_escape_malloc_hook, "mxml_escape_malloc"));
    hook_install(&(hook_t)HOOK(0x4002A8F4, 0x40006D6C, mxml_escape_hook, "mxml_escape"));
}

static void da1_init_hook(void) {
    printf("DA1 init hook\n");

    /* force log level to DEBUG */
    writel(0x52800028, 0x40200EC4);
    flush_dcache_range(0x40200EC4, 4);
    invalidate_icache();

    hook_install(&(hook_t)HOOK(0x40200B50, 0x402010AC, da2_init_hook, "da2_init"));
}

static const hook_t hooks[] = {
    HOOK(0x02047F70, 0x0206C4FC, da1_init_hook, "da1_init"),
};

void hook_install_all(void) {
    for (size_t i = 0; i < ARRAY_SIZE(hooks); i++)
        hook_install(&hooks[i]);
}