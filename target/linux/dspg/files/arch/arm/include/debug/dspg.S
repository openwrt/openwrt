/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#define DSPG_UART1_PHYS		0x05a00000
#define DSPG_UART1_VIRT		0xf05a0000

#define DSPG_UART_TX_DATA	0x2c
#define DSPG_UART_STAT		0x34

#define DSPG_UART_STAT_TX_EMPTY	0x1
#define DSPG_UART_STAT_TX_FULL	0x2

		.macro	addruart, rp, rv, tmp
		ldr	\rp, =DSPG_UART1_PHYS
		ldr	\rv, =DSPG_UART1_VIRT
		.endm

		.macro	senduart, rd, rx
		strh	\rd, [\rx, #DSPG_UART_TX_DATA]
		.endm

		.macro	waituartcts, rd, rx
		.endm

		.macro	waituarttxrdy, rd, rx
1001:		ldrh	\rd, [\rx, #DSPG_UART_STAT]
		tst	\rd, #DSPG_UART_STAT_TX_FULL
		bne	1001b
		.endm

		.macro	busyuart, rd, rx
1002:		ldrh	\rd, [\rx, #DSPG_UART_STAT]
		tst	\rd, #DSPG_UART_STAT_TX_EMPTY
		beq	1002b
		.endm
