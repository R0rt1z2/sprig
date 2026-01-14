#include <debug.h>
#include <mmio.h>
#include <patches.h>

static const patch_t patches[] = {
    PATCH(0x02043f08, 0x529c401c, "Increase handshake timeout"),
    PATCH_RET(0x0206c49c, 0, "Bypass DA1 verification"),
    PATCH_NOP(0x02057b34, "Don't disable UART logs"),
    PATCH_RET(0x0205c970, 0, "Forcefully disable AEE boot"),
    PATCH_RET(0x0207235c, 0, "Disable SBC"),
    PATCH_RET(0x02072384, 0, "Disable SLA"),
    PATCH_RET(0x02072370, 0, "Disable DAA"),
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void patch_apply_all(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(patches); i++) {
        const patch_t *p = &patches[i];
        uint32_t before = readl(p->addr);
        
        writel(p->insn[0], p->addr);
        
        if (p->insn[1] != 0) {
            writel(p->insn[1], p->addr + 4);
            printf("%-30s 0x%08x: 0x%08x -> 0x%08x 0x%08x\n",
                   p->name, p->addr, before, p->insn[0], p->insn[1]);
        } else {
            printf("%-30s 0x%08x: 0x%08x -> 0x%08x\n",
                   p->name, p->addr, before, p->insn[0]);
        }
        
        flush_dcache_range(p->addr, 64);
    }
    
    invalidate_icache();
}