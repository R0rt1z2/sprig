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

/*
 * Hook for set_all_in_one_signature_buffer().
 * Dumps heap state before and after to analyze overflow corruption.
 */
static void set_aio_sig_hook(void) {
    register uint8_t *sig asm("x0");
    register uint32_t sz asm("w1");
    
    uint8_t *saved_sig = sig;
    uint32_t saved_sz = sz;
    
    printf("set_all_in_one_signature_buffer(0x%lx, 0x%x)\n", 
           (unsigned long)saved_sig, saved_sz);
    
    printf("BEFORE:\n");
    heap_dump_dpc();
    heap_dump_aio_state();
    heap_dump_freelist();
    
    ((void (*)(uint8_t *, uint32_t))0x4002cc90)(saved_sig, saved_sz);
    
    printf("AFTER:\n");
    heap_dump_dpc();
    heap_dump_aio_state();
    heap_dump_freelist();
}

/*
 * Hook for free() on abort path in cmd_security_set_all_in_one_signature.
 * This is called when fp_read_host_file fails (e.g., user aborts transfer).
 */
static void free_on_abort_hook(void) {
    register void *ptr asm("x0");
    
    void *saved_ptr = ptr;
    
    printf("free on abort(0x%lx)\n", (unsigned long)saved_ptr);
    
    printf("BEFORE free:\n");
    heap_dump_dpc();
    heap_dump_aio_state();
    if (saved_ptr)
        heap_dump_chunk_header(saved_ptr);
    heap_dump_freelist();
    
    ((void (*)(void *))0x40009eb8)(saved_ptr);
    
    printf("AFTER free:\n");
    heap_dump_dpc();
    heap_dump_aio_state();
    heap_dump_freelist();
}

static void dpc_call_hook(void) {
    printf("DPC call hook\n");
    heap_dump_dpc();
    heap_dump_aio_state();
    heap_dump_freelist();
}

/*
 * DA2 initialization hook.
 * Bypasses hash validation and installs protocol hooks.
 */
static void da2_init_hook(void) {
    printf("DA2 init hook\n");

    /* bypass DA2 hash validation */
    writel(0x52800000, 0x4026e048);  /* mov w0, #0 */
    writel(0xD65F03C0, 0x4026e04c);  /* ret */
    
    flush_dcache_range(0x4026e048, 8);
    invalidate_icache();

    /* hook set_all_in_one_signature_buffer (success path) */
    hook_install(&(hook_t)HOOK(0x40007464, 0x4000671C, set_aio_sig_hook, "set_aio_sig"));
    
    /* hook free on abort path */
    hook_install(&(hook_t)HOOK(0x4000749C, 0x400068CC, free_on_abort_hook, "free_on_abort"));

    /* hook DPC call */
    hook_install(&(hook_t)HOOK(0x4000987C, 0x4000687c, dpc_call_hook, "dpc_call"));
}

/*
 * DA1 initialization hook.
 * Enables debug logging and sets up DA2 hooks.
 */
static void da1_init_hook(void) {
    printf("DA1 init hook\n");

    /* force log level to DEBUG */
    writel(0x52800028, 0x40200EC4);
    
    flush_dcache_range(0x40200EC4, 4);
    invalidate_icache();

    /* hook DA2 init */
    hook_install(&(hook_t)HOOK(0x40200B50, 0x402010AC, da2_init_hook, "da2_init"));
}

static const hook_t hooks[] = {
    HOOK(0x02047F70, 0x0206C4FC, da1_init_hook, "da1_init"),
};

void hook_install_all(void) {
    for (size_t i = 0; i < ARRAY_SIZE(hooks); i++)
        hook_install(&hooks[i]);
}