/*
 * STM32F407ZGTx Linker Script
 * Target: STM32F407ZGT6 (1MB Flash, 192KB SRAM)
 * 
 * Memory Layout:
 *   FLASH: 0x08000000 - 0x080FFFFF (1024KB)
 *   RAM:   0x20000000 - 0x2002FFFF (192KB)
 *   CCM:   0x10000000 - 0x1000FFFF (64KB) - Core Coupled Memory
 */

/* Entry point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Generate a link error if heap and stack don't fit into RAM */
__min_heap_size = 0x200;
__min_stack_size = 0x400;

/* Memory regions */
MEMORY
{
    /* Flash memory */
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
    
    /* SRAM - Main RAM */
    RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 128K
    
    /* CCM RAM - Core Coupled Memory (not accessible by DMA) */
    CCM (rwx)       : ORIGIN = 0x10000000, LENGTH = 64K
}

/* Sections */
SECTIONS
{
    /* Vector table - must be at start of flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* Program code */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)
        
        KEEP(*(.init))
        KEEP(*(.fini))
        
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* Constant data */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    /* ARM exception unwinding section */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    /* ARM index table */
    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    /* Pre-initialized data - loaded at end of flash, copied to RAM at startup */
    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        
        . = ALIGN(4);
        _edata = .;
    } > RAM AT> FLASH

    /* Uninitialized data - zeroed at startup */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    /* User heap section */
    ._user_heap :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        PROVIDE(_end = .);
        . = . + __min_heap_size;
        . = ALIGN(8);
    } > RAM

    /* Stack at end of RAM */
    __stack_bottom = _estack - __min_stack_size;

    /* Remove debug information from the executable */
    /DISCARD/ :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }

    /* ARM debug sections */
    .ARM.attributes 0 : { *(.ARM.attributes) }
}
