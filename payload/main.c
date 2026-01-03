#include <stdint.h>
#include <stddef.h>

#include <debug.h>

struct bldr_command_handler {
    void *priv;
    uint32_t attr;
    void *cb;
};

typedef struct {
    uint32_t addr;
    uint32_t patch[2];
    const char *name;
} patch_t;

static inline void flush_dcache_range(unsigned long start, unsigned long size) {
    unsigned long end = start + size;
    for (unsigned long addr = start; addr < end; addr += 64) {
        __asm__ volatile("dc cvac, %0" :: "r"(addr) : "memory");
    }
    __asm__ volatile("dsb sy" ::: "memory");
}

static inline void invalidate_icache(void) {
    __asm__ volatile("ic ialluis" ::: "memory");
    __asm__ volatile("dsb sy" ::: "memory");
    __asm__ volatile("isb" ::: "memory");
}

static const patch_t patches[] = {
    { .addr = 0x02043f08, .patch = {0x529c401c, 0x00000000}, .name = "Increase handshake timeout" },
    { .addr = 0x0206c49c, .patch = {0x2a1f03e0, 0xd65f03c0}, .name = "Do not verify DA1" },
    { .addr = 0x0207235c, .patch = {0x2a1f03e0, 0xd65f03c0}, .name = "SBC off" },
    { .addr = 0x02072384, .patch = {0x2a1f03e0, 0xd65f03c0}, .name = "SLA off" },
    { .addr = 0x02072370, .patch = {0x2a1f03e0, 0xd65f03c0}, .name = "DAA off" },
};

static void apply_patches(void) {
    for (int i = 0; i < sizeof(patches) / sizeof(patches[0]); i++) {
        volatile uint32_t *addr = (volatile uint32_t *)(uintptr_t)patches[i].addr;
        
        uint32_t before = addr[0];
        
        addr[0] = patches[i].patch[0];
        if (patches[i].patch[1] != 0) {
            addr[1] = patches[i].patch[1];
        }
        
        flush_dcache_range((uint64_t)addr, 64);

        if (patches[i].patch[1] != 0) {
            printf("Applying patch '%s': 0x%x -> 0x%x 0x%x\n", 
                    patches[i].name, before, patches[i].patch[0], patches[i].patch[1]);
        } else {
            printf("Applying patch '%s': 0x%x -> 0x%x\n", 
                    patches[i].name, before, patches[i].patch[0]);
        }
    }
    
    invalidate_icache();
}

void main(void) {
    printf("\n");
    printf("           .--._.--.          \n");
    printf("          ( O     O )         \n");
    printf("          /   . .   \\         \n");
    printf("         .`._______.'.        \n");
    printf("        /(           )\\       \n");
    printf("      _/  \\  \\   /  /  \\_     \n");
    printf("   .~   `  \\  \\ /  /  '   ~.  \n");
    printf("  {    -.   \\  V  /   .-    } \n");
    printf("_ _`.    \\  |  |  |  /    .'_ _\n");
    printf(">_       _} |  |  | {_       _<\n");
    printf(" /. - ~ ,_-'  .^.  `-_, ~ - .\\ \n");
    printf("         '-'|/   \\|`-`        \n");
    printf("\n   Hello world from payload \n\n");
    
    apply_patches();
    
    struct bldr_command_handler handler = {
        .priv = NULL,
        .attr = 0,
        .cb = (void *)0x02048818
    };

    printf("\nAbout to handshake...\n\n");

    ((int (*)(struct bldr_command_handler *))(0x02048404))(&handler);
    
    while(1);
}