#include <stdint.h>
#include <stddef.h>

#include "tss.h"

typedef struct
{
    uint16_t limit_low : 16;
    uint16_t base_low : 16;
    uint8_t base_mid : 8;
    uint8_t access : 8;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_high : 8;
} __attribute__((packed)) gdt_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdtp_t;

tss_t tss;
static gdt_t gdt[7];
static gdtp_t gdtr;

static void gdt_entry(size_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt_t *entry = gdt + index;

    entry->base_low = base & 0xffff;
    entry->base_mid = (base >> 16) & 0xff;
    entry->base_high = (base >> 24) & 0xff;

    entry->limit_low = limit & 0xffff;
    entry->limit_high = (limit >> 16) & 0xf;

    entry->access = access;
    entry->flags = flags & 0xf;
}

void gdt_initialize()
{
    gdtr.limit = 7 * sizeof(gdt_t) - 1;
    gdtr.base = (uint32_t)&gdt;

    gdt_entry(0, 0, 0, 0, 0);
    gdt_entry(1, 0, 0xfffff, 0x9a, 0xc);
    gdt_entry(2, 0, 0xfffff, 0x92, 0xc);
    gdt_entry(3, 0, 0xfffff, 0xfa, 0xc);
    gdt_entry(4, 0, 0xfffff, 0xf2, 0xc);
    gdt_entry(5, (uint32_t)&tss, sizeof(tss_t), 0xe9, 0);

    tss.ss0 = 0x10;
    tss.cs = 0x8 | 3;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x10 | 3;
    tss.iomap_base = sizeof(tss_t);

    asm volatile("lgdt %0\nmovw %%ax, %%ds\nmovw %%ax, %%es\nmovw %%ax, %%fs\nmovw %%ax, %%gs\nmovw %%ax, %%ss\nljmpl $0x8, $1f\n1:" ::"m"(gdtr), "a"(0x10));

    asm volatile("ltr %%ax" ::"a"(0x2b));
}

void gdt_set_kernel_stack(uintptr_t stack_top)
{
    tss.esp0 = stack_top;
}