/* SPDX-License-Identifier: GPL-2.0 */
#define IO_ADDRESS(x)		((x) + 0x07000000)

#define	UART0_PHYS	(0xf0070110)
#define	UART0_VIRT	IO_ADDRESS(UART0_PHYS)

#define	UART1_PHYS	(0xf0070140)
#define	UART1_VIRT	IO_ADDRESS(UART1_PHYS)

#define	LL_UART_PADDR	UART0_PHYS
#define	LL_UART_VADDR	UART0_VIRT

#define	UINFO_OFFS	(0xf0070128 - UART0_PHYS)
#define	UTX_DATA_OFFS	(0xf0070120 - UART0_PHYS)

		.macro	addruart, rp, rv, tmp
		ldr	\rp, =LL_UART_PADDR
		ldr	\rv, =LL_UART_VADDR
		.endm

		.macro	senduart, rd, rx
		ARM_BE8(rev \rd, \rd)
		str	\rd, [\rx, #UTX_DATA_OFFS]
		.endm

		.macro	waituart, rd, rx
1001:		ldr	\rd, [\rx, #UINFO_OFFS]
		ARM_BE8(rev \rd, \rd)
		tst	\rd, #1 << 2
		bne	1001b
		.endm

		.macro	busyuart, rd, rx
1002:		ldr	\rd, [\rx, #UINFO_OFFS]
		ARM_BE8(rev \rd, \rd)
		tst	\rd, #1 << 3
		beq	1002b
		.endm

		.macro	waituarttxrdy, rd, rx
1003:		ldr	\rd, [\rx, #UINFO_OFFS]
		ARM_BE8(rev \rd, \rd)
		tst	\rd, #1 << 3
		beq	1003b
		.endm
