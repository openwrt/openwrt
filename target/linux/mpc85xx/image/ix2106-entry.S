/* SPDX-License-Identifier: MIT */
/* Bootstrap RAM entry at 0x20000; preserve its DDR/TLB configuration.
 */
#ifndef KERNEL_ENTRY
#define KERNEL_ENTRY 0x02000000
#endif
.section .text.start,"ax",@progbits
.globl _start
_start:
	wrteei 0
	mfspr 5,340
	lis 6,0xc81e
	andc 5,5,6
	mtspr 340,5
	lis 5,0xc000
	mtspr 336,5
	isync
	lis 4,0xfd00
	ori 4,4,0x4500
	lis 7,message@ha
	addi 7,7,message@l
1:  lbz 3,0(7)
	cmpwi 3,0
	beq 3f
2:  lbz 5,0x10(4)
	andi. 5,5,2
	bne 2b
	stb 3,0(4)
	sync
	addi 7,7,1
	b 1b
3:  li 3,0
	li 4,0
	li 5,0
	lis 12,(KERNEL_ENTRY >> 16)
	ori 12,12,(KERNEL_ENTRY & 0xffff)
	mtctr 12
	sync
	isync
	bctr
message:
	.asciz "IX2106: entering Linux wrapper\r\n"
