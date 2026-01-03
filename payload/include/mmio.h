#pragma once

#include <stdint.h>

#define dmb(opt)    __asm__ volatile("dmb " #opt ::: "memory")
#define dsb(opt)    __asm__ volatile("dsb " #opt ::: "memory")
#define isb()       __asm__ volatile("isb" ::: "memory")

#define mb()        dsb(sy)
#define rmb()       dsb(ld)
#define wmb()       dsb(st)

#define smp_mb()    dmb(ish)
#define smp_rmb()   dmb(ishld)
#define smp_wmb()   dmb(ishst)

#define __raw_readb(addr) \
    ({ uint8_t __v = (*(volatile uint8_t *)(uintptr_t)(addr)); __v; })

#define __raw_readw(addr) \
    ({ uint16_t __v = (*(volatile uint16_t *)(uintptr_t)(addr)); __v; })

#define __raw_readl(addr) \
    ({ uint32_t __v = (*(volatile uint32_t *)(uintptr_t)(addr)); __v; })

#define __raw_readq(addr) \
    ({ uint64_t __v = (*(volatile uint64_t *)(uintptr_t)(addr)); __v; })

#define __raw_writeb(val, addr) \
    ({ (*(volatile uint8_t *)(uintptr_t)(addr)) = (val); })

#define __raw_writew(val, addr) \
    ({ (*(volatile uint16_t *)(uintptr_t)(addr)) = (val); })

#define __raw_writel(val, addr) \
    ({ (*(volatile uint32_t *)(uintptr_t)(addr)) = (val); })

#define __raw_writeq(val, addr) \
    ({ (*(volatile uint64_t *)(uintptr_t)(addr)) = (val); })

#define readb_relaxed(addr) \
    ({ uint8_t __v = __raw_readb(addr); __asm__ volatile("" ::: "memory"); __v; })

#define readw_relaxed(addr) \
    ({ uint16_t __v = __raw_readw(addr); __asm__ volatile("" ::: "memory"); __v; })

#define readl_relaxed(addr) \
    ({ uint32_t __v = __raw_readl(addr); __asm__ volatile("" ::: "memory"); __v; })

#define readq_relaxed(addr) \
    ({ uint64_t __v = __raw_readq(addr); __asm__ volatile("" ::: "memory"); __v; })

#define writeb_relaxed(val, addr) \
    ({ __asm__ volatile("" ::: "memory"); __raw_writeb(val, addr); })

#define writew_relaxed(val, addr) \
    ({ __asm__ volatile("" ::: "memory"); __raw_writew(val, addr); })

#define writel_relaxed(val, addr) \
    ({ __asm__ volatile("" ::: "memory"); __raw_writel(val, addr); })

#define writeq_relaxed(val, addr) \
    ({ __asm__ volatile("" ::: "memory"); __raw_writeq(val, addr); })

#define readb(addr) \
    ({ uint8_t __v = __raw_readb(addr); rmb(); __v; })

#define readw(addr) \
    ({ uint16_t __v = __raw_readw(addr); rmb(); __v; })

#define readl(addr) \
    ({ uint32_t __v = __raw_readl(addr); rmb(); __v; })

#define readq(addr) \
    ({ uint64_t __v = __raw_readq(addr); rmb(); __v; })

#define writeb(val, addr) \
    ({ wmb(); __raw_writeb(val, addr); })

#define writew(val, addr) \
    ({ wmb(); __raw_writew(val, addr); })

#define writel(val, addr) \
    ({ wmb(); __raw_writel(val, addr); })

#define writeq(val, addr) \
    ({ wmb(); __raw_writeq(val, addr); })

#define setbits_8(addr, set) \
    writeb_relaxed(readb_relaxed(addr) | (set), addr)

#define setbits_16(addr, set) \
    writew_relaxed(readw_relaxed(addr) | (set), addr)

#define setbits_32(addr, set) \
    writel_relaxed(readl_relaxed(addr) | (set), addr)

#define setbits_64(addr, set) \
    writeq_relaxed(readq_relaxed(addr) | (set), addr)

#define clrbits_8(addr, clr) \
    writeb_relaxed(readb_relaxed(addr) & ~(clr), addr)

#define clrbits_16(addr, clr) \
    writew_relaxed(readw_relaxed(addr) & ~(clr), addr)

#define clrbits_32(addr, clr) \
    writel_relaxed(readl_relaxed(addr) & ~(clr), addr)

#define clrbits_64(addr, clr) \
    writeq_relaxed(readq_relaxed(addr) & ~(clr), addr)

#define clrsetbits_8(addr, clr, set) \
    writeb_relaxed((readb_relaxed(addr) & ~(clr)) | (set), addr)

#define clrsetbits_16(addr, clr, set) \
    writew_relaxed((readw_relaxed(addr) & ~(clr)) | (set), addr)

#define clrsetbits_32(addr, clr, set) \
    writel_relaxed((readl_relaxed(addr) & ~(clr)) | (set), addr)

#define clrsetbits_64(addr, clr, set) \
    writeq_relaxed((readq_relaxed(addr) & ~(clr)) | (set), addr)

#define setbits(addr, set)          setbits_32(addr, set)
#define clrbits(addr, clr)          clrbits_32(addr, clr)
#define clrsetbits(addr, clr, set)  clrsetbits_32(addr, clr, set)

#define readl_poll_timeout(addr, val, cond, timeout_us) \
({ \
    uint64_t __timeout = (timeout_us); \
    int __ret = -1; \
    for (uint64_t __i = 0; __i < __timeout; __i++) { \
        (val) = readl(addr); \
        if (cond) { \
            __ret = 0; \
            break; \
        } \
        for (volatile int __d = 0; __d < 100; __d++); \
    } \
    __ret; \
})

#define flush_dcache_range(start, size) \
do { \
    unsigned long __end = (unsigned long)(start) + (size); \
    for (unsigned long __addr = (unsigned long)(start); __addr < __end; __addr += 64) { \
        __asm__ volatile("dc cvac, %0" :: "r"(__addr) : "memory"); \
    } \
    dsb(sy); \
} while(0)

#define invalidate_dcache_range(start, size) \
do { \
    unsigned long __end = (unsigned long)(start) + (size); \
    for (unsigned long __addr = (unsigned long)(start); __addr < __end; __addr += 64) { \
        __asm__ volatile("dc ivac, %0" :: "r"(__addr) : "memory"); \
    } \
    dsb(sy); \
} while(0)

#define clean_invalidate_dcache_range(start, size) \
do { \
    unsigned long __end = (unsigned long)(start) + (size); \
    for (unsigned long __addr = (unsigned long)(start); __addr < __end; __addr += 64) { \
        __asm__ volatile("dc civac, %0" :: "r"(__addr) : "memory"); \
    } \
    dsb(sy); \
} while(0)

#define invalidate_icache() \
do { \
    __asm__ volatile("ic ialluis" ::: "memory"); \
    dsb(sy); \
    isb(); \
} while(0)

#define invalidate_icache_range(start, size) \
do { \
    flush_dcache_range(start, size); \
    invalidate_icache(); \
} while(0)

#define GENMASK(h, l) \
    (((~0ULL) << (l)) & (~0ULL >> (63 - (h))))

#define FIELD_PREP(mask, val) \
    (((val) << (__builtin_ctzll(mask))) & (mask))

#define FIELD_GET(mask, reg) \
    (((reg) & (mask)) >> (__builtin_ctzll(mask)))

#define BIT(n)              (1UL << (n))
#define BIT_ULL(n)          (1ULL << (n))

#define swab16(x) \
    ((uint16_t)( \
        (((uint16_t)(x) & 0x00ffU) << 8) | \
        (((uint16_t)(x) & 0xff00U) >> 8)))

#define swab32(x) \
    ((uint32_t)( \
        (((uint32_t)(x) & 0x000000ffUL) << 24) | \
        (((uint32_t)(x) & 0x0000ff00UL) <<  8) | \
        (((uint32_t)(x) & 0x00ff0000UL) >>  8) | \
        (((uint32_t)(x) & 0xff000000UL) >> 24)))

#define swab64(x) \
    ((uint64_t)( \
        (((uint64_t)(x) & 0x00000000000000ffULL) << 56) | \
        (((uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
        (((uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
        (((uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
        (((uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
        (((uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
        (((uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
        (((uint64_t)(x) & 0xff00000000000000ULL) >> 56)))

#define cpu_to_le16(x)  (x)
#define cpu_to_le32(x)  (x)
#define cpu_to_le64(x)  (x)
#define le16_to_cpu(x)  (x)
#define le32_to_cpu(x)  (x)
#define le64_to_cpu(x)  (x)

#define cpu_to_be16(x)  swab16(x)
#define cpu_to_be32(x)  swab32(x)
#define cpu_to_be64(x)  swab64(x)
#define be16_to_cpu(x)  swab16(x)
#define be32_to_cpu(x)  swab32(x)
#define be64_to_cpu(x)  swab64(x)