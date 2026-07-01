/*
 * Lantiq VRX200 NAND Bootstrap
 *
 * This small bootstrap program is loaded by the VRX200 BootROM from the
 * first NAND page. It reads the SPL from subsequent NAND pages into SRAM
 * and transfers control to the SPL.
 *
 * Memory map:
 *   0xBE220000 - Bootstrap (this code, loaded by BootROM)
 *   0xBE220800 - SPL (loaded by this bootstrap from NAND pages 1+)
 *
 * NAND layout:
 *   Page 0 (0x000-0x7FF): NVB header + this bootstrap
 *   Pages 1-N (0x800+):   SPL binary
 *   0x4000+:              Compressed U-Boot (read by SPL)
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <asm/regdef.h>

#define EBU_BASE		0xBE100000
#define EBU_NAND_CON		0x53B0
#define EBU_NAND_WAIT		0x53B4
#define EBU_NAND_DATA_BASE	0xB4000000
#define EBU_NAND_DATA		0x10
#define EBU_NAND_ALE		0x14
#define EBU_NAND_CMD		0x18

#define SRAM_BASE		0xBE220000
#define SPL_LOAD_ADDR		(SRAM_BASE + 0x800)

#define NAND_CON_INIT		0x00F405F3
#define NAND_CON_CLE_BIT	0x00040000
#define NAND_CON_CLE_MASK	0xFFFBFFFF
#define NAND_CON_DISABLE_MASK	0xFFEFFFFF

#define NAND_CMD_READ0		0x00
#define NAND_CMD_READSTART	0x30

#define NAND_PAGE_SIZE		2048
#define NAND_PAGES_TO_READ	9
#define NAND_ADDR_CYCLES	3	/* 3 row address bytes = 24 bits */

	.set	noreorder
	.globl	_start

_start:
	/* Initialize EBU NAND controller */
	lui	v1, %hi(NAND_CON_INIT)
	addiu	v1, v1, %lo(NAND_CON_INIT)
	lui	v0, %hi(EBU_BASE)
	lui	a0, %hi(SRAM_BASE)
	lui	t2, %hi(NAND_CON_CLE_MASK)
	sw	v1, EBU_NAND_CON(v0)

	/* Set destination address for SPL */
	addiu	a0, a0, %lo(SPL_LOAD_ADDR)
	move	a1, zero			/* page counter = 0 */

	/* Load constants */
	lui	v1, %hi(EBU_NAND_DATA_BASE)
	lui	t0, %hi(NAND_CON_CLE_BIT)
	li	t1, (NAND_ADDR_CYCLES * 8)	/* 24 bits for row address */
	ori	t2, t2, %lo(NAND_CON_CLE_MASK)
	li	t3, NAND_CMD_READSTART
	li	t4, NAND_PAGES_TO_READ

page_loop:
	addiu	a1, a1, 1			/* next page */

	/* Send READ command (0x00) */
	sb	zero, EBU_NAND_CMD(v1)

	/* Wait for controller ready */
1:	lw	a3, EBU_NAND_WAIT(v0)
	andi	a3, a3, 0x8
	beqz	a3, 1b
	nop

	/* Enable CLE for address phase */
	lw	a3, EBU_NAND_CON(v0)
	or	a3, a3, t0
	sw	a3, EBU_NAND_CON(v0)

	/* Send column address byte 0 (always 0) */
	sb	zero, EBU_NAND_ALE(v1)
2:	lw	a2, EBU_NAND_WAIT(v0)
	andi	a2, a2, 0x8
	beqz	a2, 2b
	nop

	/* Send column address byte 1 (always 0) */
	sb	zero, EBU_NAND_ALE(v1)
3:	lw	a2, EBU_NAND_WAIT(v0)
	andi	a2, a2, 0x8
	beqz	a2, 3b
	move	a2, zero			/* bit shift = 0 */

	/* Send row address (page number, 3 bytes) */
addr_loop:
	srlv	a3, a1, a2
	andi	a3, a3, 0xFF
	sb	a3, EBU_NAND_ALE(v1)
4:	lw	t5, EBU_NAND_WAIT(v0)
	andi	t5, t5, 0x8
	beqz	t5, 4b
	nop
	addiu	a2, a2, 8
	bne	a2, t1, addr_loop
	srlv	a3, a1, a2			/* delay slot */

	/* Disable CLE, send READ confirm (0x30) */
	lw	a2, EBU_NAND_CON(v0)
	and	a2, a2, t2
	sw	a2, EBU_NAND_CON(v0)
	sb	t3, EBU_NAND_CMD(v1)

	/* Wait for controller ready */
5:	lw	a2, EBU_NAND_WAIT(v0)
	andi	a2, a2, 0x8
	beqz	a2, 5b
	nop

	/* Wait for data ready */
6:	lw	a2, EBU_NAND_WAIT(v0)
	andi	a2, a2, 0x1
	beqz	a2, 6b
	addiu	a3, a0, NAND_PAGE_SIZE		/* end address = dest + 2048 */

	/* Read page data (2048 bytes) */
read_loop:
	lbu	a2, EBU_NAND_DATA(v1)
	addiu	a0, a0, 1
	bne	a0, a3, read_loop
	sb	a2, -1(a0)			/* delay slot: store byte */

	/* Wait for ready before next page */
	lui	a2, %hi(EBU_BASE)
7:	lw	a0, EBU_NAND_WAIT(a2)
	andi	a0, a0, 0x1
	beqz	a0, 7b
	nop

	/* Loop for all pages */
	bne	a1, t4, page_loop
	move	a0, a3				/* delay slot: next dest addr */

	/* Disable NAND controller and jump to SPL */
	lw	v1, EBU_NAND_CON(a2)
	lui	v0, %hi(NAND_CON_DISABLE_MASK)
	ori	v0, v0, %lo(NAND_CON_DISABLE_MASK)
	and	v0, v1, v0
	lui	t9, %hi(SPL_LOAD_ADDR)
	sw	v0, EBU_NAND_CON(a2)
	addiu	t9, t9, %lo(SPL_LOAD_ADDR)
	jr	t9
	nop
