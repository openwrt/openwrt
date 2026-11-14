/* SPDX-License-Identifier: MIT */
ENTRY(_start)
SECTIONS {
  . = 0x00030100;
  .text : { *(.text.start) *(.text*) *(.rodata*) }
  /DISCARD/ : { *(.ARM.attributes) *(.comment) *(.note*) }
  ASSERT(SIZEOF(.text) <= 0x00200000, "entry plus U-Boot exceeds 2 MiB")
}
