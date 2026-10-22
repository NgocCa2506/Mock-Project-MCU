/*
******************************************************************************
**
** @file        : STM32F407VGTX_OTA_FLASH.ld
**
** @author      : CA
**
** @brief       : Linker script for the OTA image of STM32F407VGTx.
**
**                Produces a SINGLE binary that is flashed at 0x08080000.
**                The binary layout is:
**
**                  0x08080000 : OTA Header  (.ota_header section, 128 KB region)
**                  0x080A0000 : OTA Code    (.isr_vector + .text + ... )
**
**                The bootloader reads the header from OTA_HEADER_START_ADDRESS
**                (0x08080000) and the executable code from OTA_START_ADDRESS
**                (0x080A0000).  One single flash operation covers both.
**
******************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Minimum heap and stack sizes */
_Min_Heap_Size  = 0x200;
_Min_Stack_Size = 0x400;

/*
 * Memory map
 *   OTA_HEADER : 128 KB at 0x08080000 (Sector 8)
 *                Holds the app_header_t struct at the very first bytes.
 *   OTA_FLASH  : 384 KB at 0x080A0000 (Sectors 9-11)
 *                Holds the executable image (vector table, code, rodata ...).
 *   CCMRAM / RAM: same as the current-app linker script.
 */
MEMORY
{
  CCMRAM     (xrw) : ORIGIN = 0x10000000, LENGTH = 64K
  RAM        (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
  OTA_HEADER (r)   : ORIGIN = 0x08080000, LENGTH = 128K
  OTA_FLASH  (rx)  : ORIGIN = 0x080A0000, LENGTH = 384K
}

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Sections */
SECTIONS
{
  /* ------------------------------------------------------------------ */
  /* OTA header – placed at 0x08080000 (Sector 8).                       */
  /* The header struct is the ONLY content in this region; the rest of   */
  /* the 128 KB is left at 0xFF (erased flash).                          */
  /* ------------------------------------------------------------------ */
  .ota_header :
  {
    KEEP(*(.app_header))
  } > OTA_HEADER

  /* ------------------------------------------------------------------ */
  /* OTA executable image – placed at 0x080A0000 (Sectors 9-11).        */
  /* ------------------------------------------------------------------ */

  /* OTA vector table at 0x080A0000 */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > OTA_FLASH

  /* Code and read-only data */
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
  } > OTA_FLASH

  /* ARM exception-unwind tables */
  .ARM.extab :
  {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } > OTA_FLASH

  .ARM.exidx :
  {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } > OTA_FLASH

  /* C/C++ initialization arrays */
  .preinit_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
    . = ALIGN(4);
  } > OTA_FLASH

  .init_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
    . = ALIGN(4);
  } > OTA_FLASH

  .fini_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
    . = ALIGN(4);
  } > OTA_FLASH

  /* Constant data */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } > OTA_FLASH

  /* Exception frame header if generated */
  .eh_frame_hdr :
  {
    *(.eh_frame_hdr)
  } > OTA_FLASH

  /* Flash load address of initialized RAM data */
  _sidata = LOADADDR(.data);

  /* Initialized variables copied from OTA_FLASH to RAM */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    __data_start__ = .;

    *(.data)
    *(.data*)

    . = ALIGN(4);
    _edata = .;
    __data_end__ = .;
  } > RAM AT > OTA_FLASH

  /* Initialized variables copied from OTA_FLASH to CCMRAM */
  _siccmram = LOADADDR(.ccmram);

  .ccmram :
  {
    . = ALIGN(4);
    _sccmram = .;
    __ccmram_start__ = .;

    *(.ccmram)
    *(.ccmram*)

    . = ALIGN(4);
    _eccmram = .;
    __ccmram_end__ = .;
  } > CCMRAM AT > OTA_FLASH

  /* Uninitialized variables in RAM */
  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = .;

    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = .;
  } > RAM

  /* Uninitialized variables explicitly placed in CCMRAM */
  .ccmram_bss (NOLOAD) :
  {
    . = ALIGN(4);
    __ccmram_bss_start__ = .;

    *(.ccmram_bss)
    *(.ccmram_bss*)

    . = ALIGN(4);
    __ccmram_bss_end__ = .;
  } > CCMRAM

  /* Heap and stack reservation */
  ._user_heap_stack (NOLOAD) :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);

    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;

    . = ALIGN(8);
  } > RAM

  /* Remove unused standard-library metadata sections */
  /DISCARD/ :
  {
    libc.a   (*)
    libm.a   (*)
    libgcc.a (*)
  }

  /* ARM build attributes */
  .ARM.attributes 0 :
  {
    *(.ARM.attributes)
  }
}
