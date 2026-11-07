/* SPDX-License-Identifier: MIT
 * RTX830 U-Boot CANZ entry. No DDR, pinmux, flash or FPGA writes.
 * Cache geometry walk adapted from the MIT-licensed old project linux_shim.S
 * at d2b99c6146d2791b74a7c14c31b4d5da86fd96b3; no Linux ABI is retained.
 */
.syntax unified
.arch armv7-a
.arm
.section .text.start, "ax"
.global _start
_start:
    cpsid if
    mrc p15, 0, r0, c1, c0, 0
    tst r0, #1
    bne halted                 /* Unknown VA mapping: do not touch MMIO. */
    cps #0x13
    mrc p15, 0, r0, c0, c0, 5
    and r0, r0, #0xff
    cmp r0, #0
    bne halted
    /* Stop only an already-enabled CPU0 private watchdog. */
    ldr r1, =0xf100c600
    ldr r0, [r1, #0x28]
    tst r0, #1
    beq watchdog_stopped
    ldr r0, =0x12345678
    str r0, [r1, #0x34]
    ldr r0, =0x87654321
    str r0, [r1, #0x34]
    mov r0, #0
    str r0, [r1, #0x28]
    dsb sy
watchdog_stopped:
    ldr r0, [r1, #0x28]
    tst r0, #1
    bne halted
    ldr r0, =banner
    bl puts
    ldr r1, =0xf1020808
    ldr r0, [r1]
    tst r0, #1
    beq unsafe_cpu1
    /* Measured entry has PL310 off. An enabled outer cache is unsupported. */
    ldr r1, =0xf1008100
    ldr r0, [r1]
    tst r0, #1
    bne unsafe_l2
    bl clean_inner
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #4
    bic r0, r0, #0x800
    bic r0, r0, #0x1000
    mcr p15, 0, r0, c1, c0, 0
    dsb sy
    isb sy
    /* Relocate the embedded linked image; no cacheable stores from here. */
    ldr r0, =uboot_start
    ldr r1, =0x02000000
    ldr r2, =uboot_end
copy_words:
    cmp r0, r2
    ldrlo r3, [r0], #4
    strlo r3, [r1], #4
    blo copy_words
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c5, 6
    mcr p15, 0, r0, c8, c7, 0
    dsb sy
    isb sy
    ldr r0, =jump_banner
    bl puts
    ldr r1, =0xf1012000
wait_tx:
    ldr r0, [r1, #0x14]
    tst r0, #0x40
    beq wait_tx
    dsb sy
    ldr r3, =0x02000000
    bx r3
unsafe_cpu1:
    ldr r0, =cpu1_message
    bl puts
    b halted
unsafe_l2:
    ldr r0, =l2_message
    bl puts
halted:
    b halted
puts:
    mov r1, r0
    ldr r2, =0xf1012000
put_next:
    ldrb r0, [r1], #1
    cmp r0, #0
    bxeq lr
put_wait:
    ldr r3, [r2, #0x14]
    tst r3, #0x20
    beq put_wait
    strb r0, [r2]
    b put_next
clean_inner:
    dsb sy
    mrc p15, 1, r0, c0, c0, 1
    ubfx r1, r0, #24, #3
    mov r2, #0
2:  cmp r2, r1
    bhs 6f
    add r3, r2, r2, lsl #1
    mov r3, r0, lsr r3
    and r3, r3, #7
    cmp r3, #2
    blo 5f
    mov r3, r2, lsl #1
    mcr p15, 2, r3, c0, c0, 0
    isb sy
    mrc p15, 1, r4, c0, c0, 0
    and r5, r4, #7
    add r5, r5, #4
    ubfx r6, r4, #3, #10
    clz r7, r6
    ubfx r9, r4, #13, #15
3:  mov r10, r9
4:  mov r11, r10, lsl r5
    orr r11, r11, r6, lsl r7
    orr r11, r11, r3
    mcr p15, 0, r11, c7, c14, 2
    subs r10, r10, #1
    bpl 4b
    subs r6, r6, #1
    bpl 3b
    dsb sy
5:  add r2, r2, #1
    b 2b
6:  mov r0, #0
    mcr p15, 2, r0, c0, c0, 0
    dsb sy
    isb sy
    bx lr


.ltorg
banner: .asciz "\r\nRTX830 U-BOOT CANZ ENTRY v1\r\n"
jump_banner: .asciz "U-Boot @02000000; UART 9600; DDR inherited\r\n"
cpu1_message: .asciz "CPU1 not held in reset; halted.\r\n"
l2_message: .asciz "Outer cache enabled; unsupported entry; halted.\r\n"
.balign 4
uboot_start:
.incbin "u-boot.bin"
.balign 4
uboot_end:
