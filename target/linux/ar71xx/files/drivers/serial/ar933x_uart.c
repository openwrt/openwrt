/*
 *  linux/drivers/serial/hornet_serial.c
 *
 *  Driver for hornet serial ports
 *
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *
 *  Copyright (C) 2010 Ryan Hsu.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  $Id$
 *
 * A note about mapbase / membase
 *
 *  mapbase is the physical address of the IO port.
 *  membase is an 'ioremapped' cookie.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_reg.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/nmi.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include <asm/mach-ar71xx/ar933x_uart_platform.h>

#include <asm/io.h>
#include <asm/irq.h>

#include "8250.h"
#define ar7240_reg_rmw_clear(_reg, _val)	do {} while (0)

#define DRIVER_NAME "ar933x-uart"

#define AR933X_UART_REGS_SIZE	20
#define AR933X_UART_FIFO_SIZE	16

/*
 * Uart block
 */
#define UARTDATA_UARTTXCSR_MSB			9
#define UARTDATA_UARTTXCSR_LSB			9
#define UARTDATA_UARTTXCSR_MASK			0x00000200
#define UARTDATA_UARTTXCSR_GET(x)		(((x) & UARTDATA_UARTTXCSR_MASK) >> UARTDATA_UARTTXCSR_LSB)
#define UARTDATA_UARTTXCSR_SET(x)		(((0 | (x)) << UARTDATA_UARTTXCSR_LSB) & UARTDATA_UARTTXCSR_MASK)
#define UARTDATA_UARTTXCSR_RESET		0
#define UARTDATA_UARTRXCSR_MSB			8
#define UARTDATA_UARTRXCSR_LSB			8
#define UARTDATA_UARTRXCSR_MASK			0x00000100
#define UARTDATA_UARTRXCSR_GET(x)		(((x) & UARTDATA_UARTRXCSR_MASK) >> UARTDATA_UARTRXCSR_LSB)
#define UARTDATA_UARTRXCSR_SET(x)		(((0 | (x)) << UARTDATA_UARTRXCSR_LSB) & UARTDATA_UARTRXCSR_MASK)
#define UARTDATA_UARTRXCSR_RESET		0
#define UARTDATA_UARTTXRXDATA_MSB		7
#define UARTDATA_UARTTXRXDATA_LSB		0
#define UARTDATA_UARTTXRXDATA_MASK		0x000000ff
#define UARTDATA_UARTTXRXDATA_GET(x)		(((x) & UARTDATA_UARTTXRXDATA_MASK) >> UARTDATA_UARTTXRXDATA_LSB)
#define UARTDATA_UARTTXRXDATA_SET(x)		(((0 | (x)) << UARTDATA_UARTTXRXDATA_LSB) & UARTDATA_UARTTXRXDATA_MASK)
#define UARTDATA_UARTTXRXDATA_RESET		0
#define UARTDATA_ADDRESS			0x0000
#define UARTDATA_HW_MASK			0x000003ff
#define UARTDATA_SW_MASK			0x000003ff
#define UARTDATA_RSTMASK			0x000003ff
#define UARTDATA_RESET				0x00000000

// 0x0004 (UARTCS)
#define UARTCS_UARTRXBUSY_MSB			15
#define UARTCS_UARTRXBUSY_LSB			15
#define UARTCS_UARTRXBUSY_MASK			0x00008000
#define UARTCS_UARTRXBUSY_GET(x)		(((x) & UARTCS_UARTRXBUSY_MASK) >> UARTCS_UARTRXBUSY_LSB)
#define UARTCS_UARTRXBUSY_SET(x)		(((0 | (x)) << UARTCS_UARTRXBUSY_LSB) & UARTCS_UARTRXBUSY_MASK)
#define UARTCS_UARTRXBUSY_RESET			0
#define UARTCS_UARTTXBUSY_MSB			14
#define UARTCS_UARTTXBUSY_LSB			14
#define UARTCS_UARTTXBUSY_MASK			0x00004000
#define UARTCS_UARTTXBUSY_GET(x)		(((x) & UARTCS_UARTTXBUSY_MASK) >> UARTCS_UARTTXBUSY_LSB)
#define UARTCS_UARTTXBUSY_SET(x)		(((0 | (x)) << UARTCS_UARTTXBUSY_LSB) & UARTCS_UARTTXBUSY_MASK)
#define UARTCS_UARTTXBUSY_RESET			0
#define UARTCS_UARTHOSTINTEN_MSB		13
#define UARTCS_UARTHOSTINTEN_LSB		13
#define UARTCS_UARTHOSTINTEN_MASK		0x00002000
#define UARTCS_UARTHOSTINTEN_GET(x)		(((x) & UARTCS_UARTHOSTINTEN_MASK) >> UARTCS_UARTHOSTINTEN_LSB)
#define UARTCS_UARTHOSTINTEN_SET(x)		(((0 | (x)) << UARTCS_UARTHOSTINTEN_LSB) & UARTCS_UARTHOSTINTEN_MASK)
#define UARTCS_UARTHOSTINTEN_RESET		0
#define UARTCS_UARTHOSTINT_MSB			12
#define UARTCS_UARTHOSTINT_LSB			12
#define UARTCS_UARTHOSTINT_MASK			0x00001000
#define UARTCS_UARTHOSTINT_GET(x)		(((x) & UARTCS_UARTHOSTINT_MASK) >> UARTCS_UARTHOSTINT_LSB)
#define UARTCS_UARTHOSTINT_SET(x)		(((0 | (x)) << UARTCS_UARTHOSTINT_LSB) & UARTCS_UARTHOSTINT_MASK)
#define UARTCS_UARTHOSTINT_RESET		0
#define UARTCS_UARTTXBREAK_MSB			11
#define UARTCS_UARTTXBREAK_LSB			11
#define UARTCS_UARTTXBREAK_MASK			0x00000800
#define UARTCS_UARTTXBREAK_GET(x)		(((x) & UARTCS_UARTTXBREAK_MASK) >> UARTCS_UARTTXBREAK_LSB)
#define UARTCS_UARTTXBREAK_SET(x)		(((0 | (x)) << UARTCS_UARTTXBREAK_LSB) & UARTCS_UARTTXBREAK_MASK)
#define UARTCS_UARTTXBREAK_RESET		0
#define UARTCS_UARTRXBREAK_MSB			10
#define UARTCS_UARTRXBREAK_LSB			10
#define UARTCS_UARTRXBREAK_MASK			0x00000400
#define UARTCS_UARTRXBREAK_GET(x)		(((x) & UARTCS_UARTRXBREAK_MASK) >> UARTCS_UARTRXBREAK_LSB)
#define UARTCS_UARTRXBREAK_SET(x)		(((0 | (x)) << UARTCS_UARTRXBREAK_LSB) & UARTCS_UARTRXBREAK_MASK)
#define UARTCS_UARTRXBREAK_RESET		0
#define UARTCS_UARTSERIATXREADY_MSB		9
#define UARTCS_UARTSERIATXREADY_LSB		9
#define UARTCS_UARTSERIATXREADY_MASK		0x00000200
#define UARTCS_UARTSERIATXREADY_GET(x)		(((x) & UARTCS_UARTSERIATXREADY_MASK) >> UARTCS_UARTSERIATXREADY_LSB)
#define UARTCS_UARTSERIATXREADY_SET(x)		(((0 | (x)) << UARTCS_UARTSERIATXREADY_LSB) & UARTCS_UARTSERIATXREADY_MASK)
#define UARTCS_UARTSERIATXREADY_RESET		0
#define UARTCS_UARTTXREADYORIDE_MSB		8
#define UARTCS_UARTTXREADYORIDE_LSB		8
#define UARTCS_UARTTXREADYORIDE_MASK		0x00000100
#define UARTCS_UARTTXREADYORIDE_GET(x)		(((x) & UARTCS_UARTTXREADYORIDE_MASK) >> UARTCS_UARTTXREADYORIDE_LSB)
#define UARTCS_UARTTXREADYORIDE_SET(x)		(((0 | (x)) << UARTCS_UARTTXREADYORIDE_LSB) & UARTCS_UARTTXREADYORIDE_MASK)
#define UARTCS_UARTTXREADYORIDE_RESET		0
#define UARTCS_UARTRXREADYORIDE_MSB		7
#define UARTCS_UARTRXREADYORIDE_LSB		7
#define UARTCS_UARTRXREADYORIDE_MASK		0x00000080
#define UARTCS_UARTRXREADYORIDE_GET(x)		(((x) & UARTCS_UARTRXREADYORIDE_MASK) >> UARTCS_UARTRXREADYORIDE_LSB)
#define UARTCS_UARTRXREADYORIDE_SET(x)		(((0 | (x)) << UARTCS_UARTRXREADYORIDE_LSB) & UARTCS_UARTRXREADYORIDE_MASK)
#define UARTCS_UARTRXREADYORIDE_RESET		0
#define UARTCS_UARTDMAEN_MSB			6
#define UARTCS_UARTDMAEN_LSB			6
#define UARTCS_UARTDMAEN_MASK			0x00000040
#define UARTCS_UARTDMAEN_GET(x)			(((x) & UARTCS_UARTDMAEN_MASK) >> UARTCS_UARTDMAEN_LSB)
#define UARTCS_UARTDMAEN_SET(x)			(((0 | (x)) << UARTCS_UARTDMAEN_LSB) & UARTCS_UARTDMAEN_MASK)
#define UARTCS_UARTDMAEN_RESET			0
#define UARTCS_UARTFLOWCONTROLMODE_MSB		5
#define UARTCS_UARTFLOWCONTROLMODE_LSB		4
#define UARTCS_UARTFLOWCONTROLMODE_MASK		0x00000030
#define UARTCS_UARTFLOWCONTROLMODE_GET(x)	(((x) & UARTCS_UARTFLOWCONTROLMODE_MASK) >> UARTCS_UARTFLOWCONTROLMODE_LSB)
#define UARTCS_UARTFLOWCONTROLMODE_SET(x)	(((0 | (x)) << UARTCS_UARTFLOWCONTROLMODE_LSB) & UARTCS_UARTFLOWCONTROLMODE_MASK)
#define UARTCS_UARTFLOWCONTROLMODE_RESET	0
#define UARTCS_UARTINTERFACEMODE_MSB		3
#define UARTCS_UARTINTERFACEMODE_LSB		2
#define UARTCS_UARTINTERFACEMODE_MASK		0x0000000c
#define UARTCS_UARTINTERFACEMODE_GET(x)		(((x) & UARTCS_UARTINTERFACEMODE_MASK) >> UARTCS_UARTINTERFACEMODE_LSB)
#define UARTCS_UARTINTERFACEMODE_SET(x)		(((0 | (x)) << UARTCS_UARTINTERFACEMODE_LSB) & UARTCS_UARTINTERFACEMODE_MASK)
#define UARTCS_UARTINTERFACEMODE_RESET		0
#define UARTCS_UARTPARITYMODE_MSB		1
#define UARTCS_UARTPARITYMODE_LSB		0
#define UARTCS_UARTPARITYMODE_MASK		0x00000003
#define UARTCS_UARTPARITYMODE_GET(x)		(((x) & UARTCS_UARTPARITYMODE_MASK) >> UARTCS_UARTPARITYMODE_LSB)
#define UARTCS_UARTPARITYMODE_SET(x)		(((0 | (x)) << UARTCS_UARTPARITYMODE_LSB) & UARTCS_UARTPARITYMODE_MASK)
#define UARTCS_UARTPARITYMODE_RESET		0
#define UARTCS_ADDRESS				0x0004
#define UARTCS_HW_MASK				0x0000ffff
#define UARTCS_SW_MASK				0x0000ffff
#define UARTCS_RSTMASK				0x000029ff
#define UARTCS_RESET				0x00000000

// 0x0008 (UARTCLOCK)
#define UARTCLOCK_UARTCLOCKSCALE_MSB		23
#define UARTCLOCK_UARTCLOCKSCALE_LSB		16
#define UARTCLOCK_UARTCLOCKSCALE_MASK		0x00ff0000
#define UARTCLOCK_UARTCLOCKSCALE_GET(x)		(((x) & UARTCLOCK_UARTCLOCKSCALE_MASK) >> UARTCLOCK_UARTCLOCKSCALE_LSB)
#define UARTCLOCK_UARTCLOCKSCALE_SET(x)		(((0 | (x)) << UARTCLOCK_UARTCLOCKSCALE_LSB) & UARTCLOCK_UARTCLOCKSCALE_MASK)
#define UARTCLOCK_UARTCLOCKSCALE_RESET		0
#define UARTCLOCK_UARTCLOCKSTEP_MSB		15
#define UARTCLOCK_UARTCLOCKSTEP_LSB		0
#define UARTCLOCK_UARTCLOCKSTEP_MASK		0x0000ffff
#define UARTCLOCK_UARTCLOCKSTEP_GET(x)		(((x) & UARTCLOCK_UARTCLOCKSTEP_MASK) >> UARTCLOCK_UARTCLOCKSTEP_LSB)
#define UARTCLOCK_UARTCLOCKSTEP_SET(x)		(((0 | (x)) << UARTCLOCK_UARTCLOCKSTEP_LSB) & UARTCLOCK_UARTCLOCKSTEP_MASK)
#define UARTCLOCK_UARTCLOCKSTEP_RESET		0
#define UARTCLOCK_ADDRESS			0x0008
#define UARTCLOCK_HW_MASK			0x00ffffff
#define UARTCLOCK_SW_MASK			0x00ffffff
#define UARTCLOCK_RSTMASK			0x00ffffff
#define UARTCLOCK_RESET				0x00000000

// 0x000c (UARTINT)
#define UARTINT_UARTTXEMPTYINT_MSB		9
#define UARTINT_UARTTXEMPTYINT_LSB		9
#define UARTINT_UARTTXEMPTYINT_MASK		0x00000200
#define UARTINT_UARTTXEMPTYINT_GET(x)		(((x) & UARTINT_UARTTXEMPTYINT_MASK) >> UARTINT_UARTTXEMPTYINT_LSB)
#define UARTINT_UARTTXEMPTYINT_SET(x)		(((0 | (x)) << UARTINT_UARTTXEMPTYINT_LSB) & UARTINT_UARTTXEMPTYINT_MASK)
#define UARTINT_UARTTXEMPTYINT_RESET		0
#define UARTINT_UARTRXFULLINT_MSB		8
#define UARTINT_UARTRXFULLINT_LSB		8
#define UARTINT_UARTRXFULLINT_MASK		0x00000100
#define UARTINT_UARTRXFULLINT_GET(x)		(((x) & UARTINT_UARTRXFULLINT_MASK) >> UARTINT_UARTRXFULLINT_LSB)
#define UARTINT_UARTRXFULLINT_SET(x)		(((0 | (x)) << UARTINT_UARTRXFULLINT_LSB) & UARTINT_UARTRXFULLINT_MASK)
#define UARTINT_UARTRXFULLINT_RESET		0
#define UARTINT_UARTRXBREAKOFFINT_MSB		7
#define UARTINT_UARTRXBREAKOFFINT_LSB		7
#define UARTINT_UARTRXBREAKOFFINT_MASK		0x00000080
#define UARTINT_UARTRXBREAKOFFINT_GET(x)	(((x) & UARTINT_UARTRXBREAKOFFINT_MASK) >> UARTINT_UARTRXBREAKOFFINT_LSB)
#define UARTINT_UARTRXBREAKOFFINT_SET(x)	(((0 | (x)) << UARTINT_UARTRXBREAKOFFINT_LSB) & UARTINT_UARTRXBREAKOFFINT_MASK)
#define UARTINT_UARTRXBREAKOFFINT_RESET		0
#define UARTINT_UARTRXBREAKONINT_MSB		6
#define UARTINT_UARTRXBREAKONINT_LSB		6
#define UARTINT_UARTRXBREAKONINT_MASK		0x00000040
#define UARTINT_UARTRXBREAKONINT_GET(x)		(((x) & UARTINT_UARTRXBREAKONINT_MASK) >> UARTINT_UARTRXBREAKONINT_LSB)
#define UARTINT_UARTRXBREAKONINT_SET(x)		(((0 | (x)) << UARTINT_UARTRXBREAKONINT_LSB) & UARTINT_UARTRXBREAKONINT_MASK)
#define UARTINT_UARTRXBREAKONINT_RESET		0
#define UARTINT_UARTRXPARITYERRINT_MSB		5
#define UARTINT_UARTRXPARITYERRINT_LSB		5
#define UARTINT_UARTRXPARITYERRINT_MASK		0x00000020
#define UARTINT_UARTRXPARITYERRINT_GET(x)	(((x) & UARTINT_UARTRXPARITYERRINT_MASK) >> UARTINT_UARTRXPARITYERRINT_LSB)
#define UARTINT_UARTRXPARITYERRINT_SET(x)	(((0 | (x)) << UARTINT_UARTRXPARITYERRINT_LSB) & UARTINT_UARTRXPARITYERRINT_MASK)
#define UARTINT_UARTRXPARITYERRINT_RESET	0
#define UARTINT_UARTTXOFLOWERRINT_MSB		4
#define UARTINT_UARTTXOFLOWERRINT_LSB		4
#define UARTINT_UARTTXOFLOWERRINT_MASK		0x00000010
#define UARTINT_UARTTXOFLOWERRINT_GET(x)	(((x) & UARTINT_UARTTXOFLOWERRINT_MASK) >> UARTINT_UARTTXOFLOWERRINT_LSB)
#define UARTINT_UARTTXOFLOWERRINT_SET(x)	(((0 | (x)) << UARTINT_UARTTXOFLOWERRINT_LSB) & UARTINT_UARTTXOFLOWERRINT_MASK)
#define UARTINT_UARTTXOFLOWERRINT_RESET		0
#define UARTINT_UARTRXOFLOWERRINT_MSB		3
#define UARTINT_UARTRXOFLOWERRINT_LSB		3
#define UARTINT_UARTRXOFLOWERRINT_MASK		0x00000008
#define UARTINT_UARTRXOFLOWERRINT_GET(x)	(((x) & UARTINT_UARTRXOFLOWERRINT_MASK) >> UARTINT_UARTRXOFLOWERRINT_LSB)
#define UARTINT_UARTRXOFLOWERRINT_SET(x)	(((0 | (x)) << UARTINT_UARTRXOFLOWERRINT_LSB) & UARTINT_UARTRXOFLOWERRINT_MASK)
#define UARTINT_UARTRXOFLOWERRINT_RESET		0
#define UARTINT_UARTRXFRAMINGERRINT_MSB		2
#define UARTINT_UARTRXFRAMINGERRINT_LSB		2
#define UARTINT_UARTRXFRAMINGERRINT_MASK	0x00000004
#define UARTINT_UARTRXFRAMINGERRINT_GET(x)	(((x) & UARTINT_UARTRXFRAMINGERRINT_MASK) >> UARTINT_UARTRXFRAMINGERRINT_LSB)
#define UARTINT_UARTRXFRAMINGERRINT_SET(x)	(((0 | (x)) << UARTINT_UARTRXFRAMINGERRINT_LSB) & UARTINT_UARTRXFRAMINGERRINT_MASK)
#define UARTINT_UARTRXFRAMINGERRINT_RESET	0
#define UARTINT_UARTTXREADYINT_MSB		1
#define UARTINT_UARTTXREADYINT_LSB		1
#define UARTINT_UARTTXREADYINT_MASK		0x00000002
#define UARTINT_UARTTXREADYINT_GET(x)		(((x) & UARTINT_UARTTXREADYINT_MASK) >> UARTINT_UARTTXREADYINT_LSB)
#define UARTINT_UARTTXREADYINT_SET(x)		(((0 | (x)) << UARTINT_UARTTXREADYINT_LSB) & UARTINT_UARTTXREADYINT_MASK)
#define UARTINT_UARTTXREADYINT_RESET		0
#define UARTINT_UARTRXVALIDINT_MSB		0
#define UARTINT_UARTRXVALIDINT_LSB		0
#define UARTINT_UARTRXVALIDINT_MASK		0x00000001
#define UARTINT_UARTRXVALIDINT_GET(x)		(((x) & UARTINT_UARTRXVALIDINT_MASK) >> UARTINT_UARTRXVALIDINT_LSB)
#define UARTINT_UARTRXVALIDINT_SET(x)		(((0 | (x)) << UARTINT_UARTRXVALIDINT_LSB) & UARTINT_UARTRXVALIDINT_MASK)
#define UARTINT_UARTRXVALIDINT_RESET		0
#define UARTINT_ADDRESS				0x000c
#define UARTINT_HW_MASK				0x000003ff
#define UARTINT_SW_MASK				0x000003ff
#define UARTINT_RSTMASK				0x000003ff
#define UARTINT_RESET				0x00000000

// 0x0010 (UARTINTEN)
#define UARTINTEN_UARTTXEMPTYINTEN_MSB		9
#define UARTINTEN_UARTTXEMPTYINTEN_LSB		9
#define UARTINTEN_UARTTXEMPTYINTEN_MASK		0x00000200
#define UARTINTEN_UARTTXEMPTYINTEN_GET(x)	(((x) & UARTINTEN_UARTTXEMPTYINTEN_MASK) >> UARTINTEN_UARTTXEMPTYINTEN_LSB)
#define UARTINTEN_UARTTXEMPTYINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTTXEMPTYINTEN_LSB) & UARTINTEN_UARTTXEMPTYINTEN_MASK)
#define UARTINTEN_UARTTXEMPTYINTEN_RESET	0
#define UARTINTEN_UARTRXFULLINTEN_MSB		8
#define UARTINTEN_UARTRXFULLINTEN_LSB		8
#define UARTINTEN_UARTRXFULLINTEN_MASK		0x00000100
#define UARTINTEN_UARTRXFULLINTEN_GET(x)	(((x) & UARTINTEN_UARTRXFULLINTEN_MASK) >> UARTINTEN_UARTRXFULLINTEN_LSB)
#define UARTINTEN_UARTRXFULLINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTRXFULLINTEN_LSB) & UARTINTEN_UARTRXFULLINTEN_MASK)
#define UARTINTEN_UARTRXFULLINTEN_RESET		0
#define UARTINTEN_UARTRXBREAKOFFINTEN_MSB	7
#define UARTINTEN_UARTRXBREAKOFFINTEN_LSB	7
#define UARTINTEN_UARTRXBREAKOFFINTEN_MASK	0x00000080
#define UARTINTEN_UARTRXBREAKOFFINTEN_GET(x)	(((x) & UARTINTEN_UARTRXBREAKOFFINTEN_MASK) >> UARTINTEN_UARTRXBREAKOFFINTEN_LSB)
#define UARTINTEN_UARTRXBREAKOFFINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTRXBREAKOFFINTEN_LSB) & UARTINTEN_UARTRXBREAKOFFINTEN_MASK)
#define UARTINTEN_UARTRXBREAKOFFINTEN_RESET	0
#define UARTINTEN_UARTRXBREAKONINTEN_MSB	6
#define UARTINTEN_UARTRXBREAKONINTEN_LSB	6
#define UARTINTEN_UARTRXBREAKONINTEN_MASK	0x00000040
#define UARTINTEN_UARTRXBREAKONINTEN_GET(x)	(((x) & UARTINTEN_UARTRXBREAKONINTEN_MASK) >> UARTINTEN_UARTRXBREAKONINTEN_LSB)
#define UARTINTEN_UARTRXBREAKONINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTRXBREAKONINTEN_LSB) & UARTINTEN_UARTRXBREAKONINTEN_MASK)
#define UARTINTEN_UARTRXBREAKONINTEN_RESET	0
#define UARTINTEN_UARTRXPARITYERRINTEN_MSB	5
#define UARTINTEN_UARTRXPARITYERRINTEN_LSB	5
#define UARTINTEN_UARTRXPARITYERRINTEN_MASK	0x00000020
#define UARTINTEN_UARTRXPARITYERRINTEN_GET(x)	(((x) & UARTINTEN_UARTRXPARITYERRINTEN_MASK) >> UARTINTEN_UARTRXPARITYERRINTEN_LSB)
#define UARTINTEN_UARTRXPARITYERRINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTRXPARITYERRINTEN_LSB) & UARTINTEN_UARTRXPARITYERRINTEN_MASK)
#define UARTINTEN_UARTRXPARITYERRINTEN_RESET	0
#define UARTINTEN_UARTTXOFLOWERRINTEN_MSB	4
#define UARTINTEN_UARTTXOFLOWERRINTEN_LSB	4
#define UARTINTEN_UARTTXOFLOWERRINTEN_MASK	0x00000010
#define UARTINTEN_UARTTXOFLOWERRINTEN_GET(x)	(((x) & UARTINTEN_UARTTXOFLOWERRINTEN_MASK) >> UARTINTEN_UARTTXOFLOWERRINTEN_LSB)
#define UARTINTEN_UARTTXOFLOWERRINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTTXOFLOWERRINTEN_LSB) & UARTINTEN_UARTTXOFLOWERRINTEN_MASK)
#define UARTINTEN_UARTTXOFLOWERRINTEN_RESET	0
#define UARTINTEN_UARTRXOFLOWERRINTEN_MSB	3
#define UARTINTEN_UARTRXOFLOWERRINTEN_LSB	3
#define UARTINTEN_UARTRXOFLOWERRINTEN_MASK	0x00000008
#define UARTINTEN_UARTRXOFLOWERRINTEN_GET(x)	(((x) & UARTINTEN_UARTRXOFLOWERRINTEN_MASK) >> UARTINTEN_UARTRXOFLOWERRINTEN_LSB)
#define UARTINTEN_UARTRXOFLOWERRINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTRXOFLOWERRINTEN_LSB) & UARTINTEN_UARTRXOFLOWERRINTEN_MASK)
#define UARTINTEN_UARTRXOFLOWERRINTEN_RESET	0
#define UARTINTEN_UARTRXFRAMINGERRINTEN_MSB	2
#define UARTINTEN_UARTRXFRAMINGERRINTEN_LSB	2
#define UARTINTEN_UARTRXFRAMINGERRINTEN_MASK	0x00000004
#define UARTINTEN_UARTRXFRAMINGERRINTEN_GET(x)	(((x) & UARTINTEN_UARTRXFRAMINGERRINTEN_MASK) >> UARTINTEN_UARTRXFRAMINGERRINTEN_LSB)
#define UARTINTEN_UARTRXFRAMINGERRINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTRXFRAMINGERRINTEN_LSB) & UARTINTEN_UARTRXFRAMINGERRINTEN_MASK)
#define UARTINTEN_UARTRXFRAMINGERRINTEN_RESET	0
#define UARTINTEN_UARTTXREADYINTEN_MSB		1
#define UARTINTEN_UARTTXREADYINTEN_LSB		1
#define UARTINTEN_UARTTXREADYINTEN_MASK		0x00000002
#define UARTINTEN_UARTTXREADYINTEN_GET(x)	(((x) & UARTINTEN_UARTTXREADYINTEN_MASK) >> UARTINTEN_UARTTXREADYINTEN_LSB)
#define UARTINTEN_UARTTXREADYINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTTXREADYINTEN_LSB) & UARTINTEN_UARTTXREADYINTEN_MASK)
#define UARTINTEN_UARTTXREADYINTEN_RESET	0
#define UARTINTEN_UARTRXVALIDINTEN_MSB		0
#define UARTINTEN_UARTRXVALIDINTEN_LSB		0
#define UARTINTEN_UARTRXVALIDINTEN_MASK		0x00000001
#define UARTINTEN_UARTRXVALIDINTEN_GET(x)	(((x) & UARTINTEN_UARTRXVALIDINTEN_MASK) >> UARTINTEN_UARTRXVALIDINTEN_LSB)
#define UARTINTEN_UARTRXVALIDINTEN_SET(x)	(((0 | (x)) << UARTINTEN_UARTRXVALIDINTEN_LSB) & UARTINTEN_UARTRXVALIDINTEN_MASK)
#define UARTINTEN_UARTRXVALIDINTEN_RESET	0
#define UARTINTEN_ADDRESS			0x0010
#define UARTINTEN_HW_MASK			0x000003ff
#define UARTINTEN_SW_MASK			0x000003ff
#define UARTINTEN_RSTMASK			0x000003ff
#define UARTINTEN_RESET				0x00000000

/*
 * uncomment below to enable WAR for EV81847.
 */
//#define AR933X_EV81847_WAR

static struct uart_driver ar933x_uart_driver;

/*
 * Debugging.
 */
#if 0
#define DEBUG_AUTOCONF(fmt...)	printk(fmt)
#else
#define DEBUG_AUTOCONF(fmt...)	do { } while (0)
#endif

#if 0
#define DEBUG_INTR(fmt...)	printk(fmt)
#else
#define DEBUG_INTR(fmt...)	do { } while (0)
#endif

/*
 * We default to IRQ0 for the "no irq" hack.   Some
 * machine types want others as well - they're free
 * to redefine this in their header file.
 */
#define is_real_interrupt(irq)	((irq) != 0)

#include <asm/serial.h>

struct ar933x_uart_port {
	struct uart_port	port;
	struct timer_list	timer;		/* "no irq" timer */
	unsigned char		acr;
	unsigned char		ier;
	unsigned char		lcr;
	unsigned char		mcr;
};

static inline int ar933x_ev81847_war(void)
{
#if defined(AR933X_EV81847_WAR)
	return 1;
#else
	return 0;
#endif
}

static inline unsigned int ar933x_uart_read(struct ar933x_uart_port *up,
					    int offset)
{
	return readl(up->port.membase + offset);
}

static inline void ar933x_uart_write(struct ar933x_uart_port *up,
				     int offset, unsigned int value)
{
	writel(value, up->port.membase + offset);
}

static inline void ar933x_uart_rmw(struct ar933x_uart_port *up,
				  unsigned int offset,
				  unsigned int mask,
				  unsigned int val)
{
	unsigned int t;

	t = ar933x_uart_read(up, offset);
	t &= ~mask;
	t |= val;
	ar933x_uart_write(up, offset, t);
}

static inline void ar933x_uart_rmw_set(struct ar933x_uart_port *up,
				       unsigned int offset,
				       unsigned int val)
{
	ar933x_uart_rmw(up, offset, 0, val);
}

static inline void ar933x_uart_rmw_clear(struct ar933x_uart_port *up,
					 unsigned int offset,
					 unsigned int val)
{
	ar933x_uart_rmw(up, offset, val, 0);
}

static inline void ar933x_uart_start_tx_interrupt(struct ar933x_uart_port *up)
{
	ar933x_uart_rmw_set(up, UARTINTEN_ADDRESS,
			    UARTINTEN_UARTTXEMPTYINTEN_SET(1));
}

static inline void ar933x_uart_stop_tx_interrupt(struct ar933x_uart_port *up)
{
	if (up->ier & UART_IER_THRI) {
		up->ier &= ~UART_IER_THRI;

		/* FIXME: why this uses RXVALIDINTEN? */
		ar933x_uart_rmw_clear(up, UARTINTEN_ADDRESS,
				      UARTINTEN_UARTRXVALIDINTEN_SET(1));
	}
}

static unsigned int ar933x_uart_tx_empty(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned long flags;
	unsigned int rdata;

	spin_lock_irqsave(&up->port.lock, flags);
	rdata = ar933x_uart_read(up, UARTDATA_ADDRESS);
	spin_unlock_irqrestore(&up->port.lock, flags);

	return (rdata & UARTDATA_UARTTXCSR_MASK) ? 0 : TIOCSER_TEMT;
}

static unsigned int ar933x_uart_get_mctrl(struct uart_port *port)
{
	return TIOCM_CAR;
}

static void ar933x_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static void ar933x_uart_start_tx(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;

	ar933x_uart_start_tx_interrupt(up);
}

static void ar933x_uart_stop_tx(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;

	ar933x_uart_stop_tx_interrupt(up);
}

static void ar933x_uart_stop_rx(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;

	up->ier &= ~UART_IER_RLSI;
	up->port.read_status_mask &= ~UART_LSR_DR;

	ar933x_uart_rmw_clear(up, UARTINTEN_ADDRESS,
			      UARTINTEN_UARTRXVALIDINTEN_SET(1));
}

static void ar933x_uart_break_ctl(struct uart_port *port, int break_state)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned long flags;
	unsigned long rdata;

	spin_lock_irqsave(&up->port.lock, flags);

	if (break_state == -1)
		up->lcr |= UART_LCR_SBC;
	else
		up->lcr &= ~UART_LCR_SBC;

	rdata = ar933x_uart_read(up, UARTCS_ADDRESS);
	if (up->lcr & UART_LCR_SBC)
		rdata |= UARTCS_UARTTXBREAK_SET(1);
	else
		rdata &= ~UARTCS_UARTTXBREAK_SET(1);

	ar933x_uart_write(up, UARTCS_ADDRESS, rdata);

	spin_unlock_irqrestore(&up->port.lock, flags);
}

static void ar933x_uart_enable_ms(struct uart_port *port)
{
}

static inline unsigned int ar933x_uart_get_divisor(struct uart_port *port,
						   unsigned int baud)
{
	return (port->uartclk / (16 * baud)) - 1;
}

static void ar933x_uart_set_termios(struct uart_port *port,
				    struct ktermios *termios,
				    struct ktermios *old)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned char cval;
	unsigned long flags;
	unsigned int baud, quot;

	switch (termios->c_cflag & CSIZE) {
	case CS5:
		cval = UART_LCR_WLEN5;
		break;
	case CS6:
		cval = UART_LCR_WLEN6;
		break;
	case CS7:
		cval = UART_LCR_WLEN7;
		break;
	default:
	case CS8:
		cval = UART_LCR_WLEN8;
		break;
	}

	if (termios->c_cflag & CSTOPB)
		cval |= UART_LCR_STOP;
	if (termios->c_cflag & PARENB)
		cval |= UART_LCR_PARITY;
	if (!(termios->c_cflag & PARODD))
		cval |= UART_LCR_EPAR;
#ifdef CMSPAR
	if (termios->c_cflag & CMSPAR)
		cval |= UART_LCR_SPAR;
#endif

	/*
	 * Ask the core to calculate the divisor for us.
	 */
	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk / 16);
	quot = ar933x_uart_get_divisor(port, baud);

#if 0
	if (up->capabilities & UART_CAP_FIFO && up->port.fifosize > 1) {
		if (baud < 2400)
			fcr = UART_FCR_ENABLE_FIFO | UART_FCR_TRIGGER_1;
		else
			fcr = uart_config[up->port.type].fcr;
	}

	/*
	 * MCR-based auto flow control.  When AFE is enabled, RTS will be
	 * deasserted when the receive FIFO contains more characters than
	 * the trigger, or the MCR RTS bit is cleared.  In the case where
	 * the remote UART is not using CTS auto flow control, we must
	 * have sufficient FIFO entries for the latency of the remote
	 * UART to respond.  IOW, at least 32 bytes of FIFO.
	 */
	if (up->capabilities & UART_CAP_AFE && up->port.fifosize >= 32) {
		up->mcr &= ~UART_MCR_AFE;
		if (termios->c_cflag & CRTSCTS)
			up->mcr |= UART_MCR_AFE;
	}
#endif

	/*
	 * Ok, we're now changing the port state.  Do it with
	 * interrupts disabled.
	 */
	spin_lock_irqsave(&up->port.lock, flags);

	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	up->port.read_status_mask = UART_LSR_OE | UART_LSR_THRE | UART_LSR_DR;
	if (termios->c_iflag & INPCK)
		up->port.read_status_mask |= UART_LSR_FE | UART_LSR_PE;
	if (termios->c_iflag & (BRKINT | PARMRK))
		up->port.read_status_mask |= UART_LSR_BI;

	/*
	 * Characteres to ignore
	 */
	up->port.ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		up->port.ignore_status_mask |= UART_LSR_PE | UART_LSR_FE;
	if (termios->c_iflag & IGNBRK) {
		up->port.ignore_status_mask |= UART_LSR_BI;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			up->port.ignore_status_mask |= UART_LSR_OE;
	}

	/*
	 * ignore all characters if CREAD is not set
	 */
	if ((termios->c_cflag & CREAD) == 0)
		up->port.ignore_status_mask |= UART_LSR_DR;

	/*
	 * CTS flow control flag and modem status interrupts
	 */
	up->ier &= ~UART_IER_MSI;
	if (UART_ENABLE_MS(&up->port, termios->c_cflag))
		up->ier |= UART_IER_MSI;

	ar933x_uart_rmw_set(up, UARTCS_ADDRESS,
			    UARTCS_UARTHOSTINTEN_SET(1));

	/* Save LCR */
	up->lcr = cval;

	ar933x_uart_set_mctrl(&up->port, up->port.mctrl);
	spin_unlock_irqrestore(&up->port.lock, flags);
}

static void ar933x_uart_rx_chars(struct ar933x_uart_port *up, int *status)
{
	struct tty_struct *tty = up->port.state->port.tty;
	unsigned int lsr = *status;
	unsigned char ch;
	int max_count = 256;
	char flag;

	do {
		ch = (unsigned char)UARTDATA_UARTTXRXDATA_GET(lsr);

		flag = TTY_NORMAL;
		up->port.icount.rx++;

		lsr = UARTDATA_UARTRXCSR_SET(1);
		ar933x_uart_write(up, UARTDATA_ADDRESS, lsr);

		if (unlikely(lsr & (UART_LSR_BI | UART_LSR_PE |
				    UART_LSR_FE | UART_LSR_OE))) {
			/*
			 * For statistics only
			 */
			if (lsr & UART_LSR_BI) {
				lsr &= ~(UART_LSR_FE | UART_LSR_PE);
				up->port.icount.brk++;
				/*
				 * We do the SysRQ and SAK checking
				 * here because otherwise the break
				 * may get masked by ignore_status_mask
				 * or read_status_mask.
				 */
				if (uart_handle_break(&up->port))
					goto ignore_char;
			} else if (lsr & UART_LSR_PE)
				up->port.icount.parity++;
			else if (lsr & UART_LSR_FE)
				up->port.icount.frame++;
			if (lsr & UART_LSR_OE)
				up->port.icount.overrun++;

			/*
			 * Mask off conditions which should be ignored.
			 */
			lsr &= up->port.read_status_mask;

			if (lsr & UART_LSR_BI) {
				DEBUG_INTR("handling break....");
				flag = TTY_BREAK;
			} else if (lsr & UART_LSR_PE)
				flag = TTY_PARITY;
			else if (lsr & UART_LSR_FE)
				flag = TTY_FRAME;
		}

		if (uart_handle_sysrq_char(&up->port, ch))
			goto ignore_char;

		uart_insert_char(&up->port, lsr, UART_LSR_OE, ch, flag);

ignore_char:
		lsr = ar933x_uart_read(up, UARTDATA_ADDRESS);
	} while ((lsr & UARTDATA_UARTRXCSR_MASK) && (max_count-- > 0));

	spin_unlock(&up->port.lock);
	tty_flip_buffer_push(tty);
	spin_lock(&up->port.lock);

	*status = lsr;
}

static void ar933x_uart_tx_chars(struct ar933x_uart_port *up)
{
	struct circ_buf *xmit = &up->port.state->xmit;
	int count;
	unsigned int rdata;

	rdata = ar933x_uart_read(up, UARTDATA_ADDRESS);
	if (UARTDATA_UARTTXCSR_GET(rdata) == 0) {
		ar933x_uart_start_tx_interrupt(up);
		return;
	}

	if (up->port.x_char) {
		rdata = UARTDATA_UARTTXRXDATA_SET((unsigned int)(up->port.x_char));
		rdata |= UARTDATA_UARTTXCSR_SET(1);
		ar933x_uart_write(up, UARTDATA_ADDRESS, rdata);
		up->port.icount.tx++;
		up->port.x_char = 0;
		ar933x_uart_start_tx_interrupt(up);
		return;
	}

	if (uart_tx_stopped(&up->port)) {
		ar933x_uart_stop_tx(&up->port);
		return;
	}

	if (uart_circ_empty(xmit)) {
		ar933x_uart_stop_tx_interrupt(up);
		return;
	}

	count = up->port.fifosize / 4;
	do {
		rdata = ar933x_uart_read(up, UARTDATA_ADDRESS);
		if (UARTDATA_UARTTXCSR_GET(rdata) == 0) {
			ar933x_uart_start_tx_interrupt(up);
			return;
		}

		rdata = UARTDATA_UARTTXRXDATA_SET((unsigned int)(xmit->buf[xmit->tail]));
		rdata |= UARTDATA_UARTTXCSR_SET(1);
		ar933x_uart_write(up, UARTDATA_ADDRESS, rdata);

		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		up->port.icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	} while (--count > 0);

	rdata = ar933x_uart_read(up, UARTDATA_ADDRESS);
	if (UARTDATA_UARTTXCSR_GET(rdata) == 0) {
		ar933x_uart_start_tx_interrupt(up);
		return;
	}

	/* Re-enable TX Empty Interrupt to transmit pending chars */
	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
		uart_write_wakeup(&up->port);
		ar933x_uart_start_tx_interrupt(up);
	}

	DEBUG_INTR("THRE...");

	if (uart_circ_empty(xmit))
		ar933x_uart_stop_tx_interrupt(up);
	else
		ar933x_uart_start_tx_interrupt(up);
}

/*! Hornet's interrupt status is not read clear, so that we have to...
 * a. read out the interrupt status
 * b. clear the interrupt mask to reset the interrupt status
 * c. enable the interrupt to reactivate interrupt
 *
 * Disable and clear the interrupt status
 */
static inline void ar933x_uart_clear_int(struct ar933x_uart_port *up)
{
#define BIT3 (0x1>>3)

	/* 1. clear MISC interrupt mask */
	//ar7240_reg_rmw_clear(AR7240_MISC_INT_MASK, BIT3);

	/* 2. clear uartcs hostinten mask, bit13 */
	ar933x_uart_rmw_clear(up, UARTCS_ADDRESS,
			      UARTCS_UARTHOSTINTEN_SET(1));

	/* 3. clear rx uartint */
	ar933x_uart_write(up, UARTINT_ADDRESS, UARTINT_UARTRXVALIDINT_SET(1));

	/* 4. clear misc interrupt status  */
	ar7240_reg_rmw_clear(AR7240_MISC_INT_STATUS, BIT3);

	/* 5. clear rx uartinten*/
	ar933x_uart_rmw_clear(up, UARTINTEN_ADDRESS,
			      UARTINTEN_UARTRXVALIDINTEN_SET(1));

	/* 6. enable rx int*/
	ar933x_uart_rmw_set(up, UARTINTEN_ADDRESS,
			    UARTINTEN_UARTRXVALIDINTEN_SET(1));

	/* 7. set uartcs hostinten mask */
	ar933x_uart_rmw_set(up, UARTCS_ADDRESS,
			    UARTCS_UARTHOSTINTEN_SET(1));

	/* 8. set misc int mask */
	//ar7240_reg_wr(AR7240_MISC_INT_MASK, BIT3);
}

static inline void ar933x_uart_handle_port(struct ar933x_uart_port *up)
{
	unsigned int status;
	unsigned int int_status;
	unsigned int en_status;
	unsigned long flags;

	status = ar933x_uart_read(up, UARTDATA_ADDRESS);
	int_status = ar933x_uart_read(up, UARTINT_ADDRESS);
	en_status = ar933x_uart_read(up, UARTINTEN_ADDRESS);

	spin_lock_irqsave(&up->port.lock, flags);

	if( (int_status & en_status) & UARTINT_UARTRXVALIDINT_MASK )
		ar933x_uart_rx_chars(up, &status);

	if (((int_status & en_status) & UARTINT_UARTTXEMPTYINT_MASK)) {
		/* clear TX empty interrupts */
		ar933x_uart_write(up, UARTINT_ADDRESS,
				  UARTINT_UARTTXEMPTYINT_SET(1));

		/* disable TX empty interrupts */
		ar933x_uart_rmw_clear(up, UARTINTEN_ADDRESS,
				      UARTINTEN_UARTTXEMPTYINTEN_SET(1));

		if (!uart_circ_empty(&up->port.state->xmit))
			ar933x_uart_tx_chars(up);
	}

	spin_unlock_irqrestore(&up->port.lock, flags);
}

static irqreturn_t ar933x_uart_interrupt(int irq, void *dev_id)
{
	struct ar933x_uart_port *up;
	unsigned int iir;

	up = (struct ar933x_uart_port *) dev_id;

	iir = ar933x_uart_read(up, UARTCS_ADDRESS);
	if ((iir & UARTCS_UARTHOSTINT_MASK) == 0)
		return IRQ_NONE;

	DEBUG_INTR("ar933x_uart_interrupt(%d)...", irq);

	spin_lock(&up->port.lock);
	ar933x_uart_handle_port(up);
	ar933x_uart_clear_int(up);
	spin_unlock(&up->port.lock);

	DEBUG_INTR("end.\n");

	return IRQ_HANDLED;
}

static void ar933x_uart_timer(unsigned long data)
{
	struct uart_port *port = (void *)data;
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned int iir;

	if (ar933x_ev81847_war()) {
		struct circ_buf *xmit = &up->port.state->xmit;
		unsigned long flags;

		if (!uart_circ_empty(xmit)) {
			spin_lock_irqsave(&up->port.lock, flags);
			ar933x_uart_tx_chars(up);
			spin_unlock_irqrestore(&up->port.lock, flags);
		}
	} else {
		iir = ar933x_uart_read(up, UARTCS_ADDRESS);
		if (iir & UARTCS_UARTHOSTINT_MASK) {
			spin_lock(&up->port.lock);
			ar933x_uart_handle_port(up);
			spin_unlock(&up->port.lock);
		}
	}

	mod_timer(&up->timer, jiffies + uart_poll_timeout(port));
}

static int ar933x_uart_startup(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned long flags;
	int ret;

	ret = request_irq(up->port.irq, ar933x_uart_interrupt,
			  up->port.irqflags, dev_name(up->port.dev), up);
	if (ret)
		return ret;

	up->mcr = 0;

	/*
	 * Clear the interrupt registers.
	 */
	ar933x_uart_read(up, UARTCS_ADDRESS);
	ar933x_uart_read(up, UARTINT_ADDRESS);

	if (!is_real_interrupt(up->port.irq) || ar933x_ev81847_war()) {
		setup_timer(&up->timer, ar933x_uart_timer, (unsigned long)port);
		mod_timer(&up->timer, jiffies + uart_poll_timeout(port));
		return 0;
	}

	spin_lock_irqsave(&up->port.lock, flags);

	/*
	 * Enable host interrupts
	 */
	ar933x_uart_rmw_set(up, UARTCS_ADDRESS,
			    UARTCS_UARTHOSTINTEN_SET(1));

	/*
	 * Enable RX interrupts
	 */
	up->ier = UART_IER_RLSI | UART_IER_RDI;
	ar933x_uart_write(up, UARTINTEN_ADDRESS,
			  UARTINTEN_UARTRXVALIDINTEN_SET(1));

	/*
	 * And clear the interrupt registers again for luck.
	 */
	ar933x_uart_read(up, UARTINT_ADDRESS);

	spin_unlock_irqrestore(&up->port.lock, flags);

	return 0;
}

static void ar933x_uart_shutdown(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned long flags;

	/*
	 * Disable all interrupts from this port
	 */
	up->ier = 0;
	ar933x_uart_write(up, UARTINTEN_ADDRESS, 0);

	spin_lock_irqsave(&up->port.lock, flags);
	up->port.mctrl &= ~TIOCM_OUT2;
	ar933x_uart_set_mctrl(&up->port, up->port.mctrl);
	spin_unlock_irqrestore(&up->port.lock, flags);

	/*
	 * Disable break condition
	 */
	ar933x_uart_rmw_clear(up, UARTCS_ADDRESS,
			      UARTCS_UARTTXBREAK_SET(1));

	if (!is_real_interrupt(up->port.irq) ||
	    ar933x_ev81847_war())
		del_timer_sync(&up->timer);

	free_irq(up->port.irq, up);
}

static const char *ar933x_uart_type(struct uart_port *port)
{
	return (port->type == PORT_AR933X) ? "AR933X UART" : NULL;
}

static void ar933x_uart_release_port(struct uart_port *port)
{
	/* Nothing to release ... */
}

static int ar933x_uart_request_port(struct uart_port *port)
{
	/* UARTs always present */
	return 0;
}

static void ar933x_uart_config_port(struct uart_port *port, int flags)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;

	port->type = PORT_AR933X;

	/* Clear mask, so no surprise interrupts. */
	ar933x_uart_read(up, UARTCS_ADDRESS);
	/* Clear interrupts status register */
	ar933x_uart_read(up, UARTINT_ADDRESS);
}

static int ar933x_uart_verify_port(struct uart_port *port,
				   struct serial_struct *ser)
{
	return -EINVAL;
}

static struct uart_ops ar933x_uart_ops = {
	.tx_empty	= ar933x_uart_tx_empty,
	.set_mctrl	= ar933x_uart_set_mctrl,
	.get_mctrl	= ar933x_uart_get_mctrl,
	.stop_tx	= ar933x_uart_stop_tx,
	.start_tx	= ar933x_uart_start_tx,
	.stop_rx	= ar933x_uart_stop_rx,
	.enable_ms	= ar933x_uart_enable_ms,
	.break_ctl	= ar933x_uart_break_ctl,
	.startup	= ar933x_uart_startup,
	.shutdown	= ar933x_uart_shutdown,
	.set_termios	= ar933x_uart_set_termios,
	.type		= ar933x_uart_type,
	.release_port	= ar933x_uart_release_port,
	.request_port	= ar933x_uart_request_port,
	.config_port	= ar933x_uart_config_port,
	.verify_port	= ar933x_uart_verify_port,
};

#ifdef CONFIG_SERIAL_AR933X_CONSOLE

static struct ar933x_uart_port *ar933x_console_ports[CONFIG_SERIAL_AR933X_NR_UARTS];

static void ar933x_uart_wait_xmitr(struct ar933x_uart_port *up)
{
	unsigned int status;
	unsigned int timeout = 60000;

	/* Wait up to 60ms for the character(s) to be sent. */
	do {
		status = ar933x_uart_read(up, UARTDATA_ADDRESS);
		if (--timeout == 0)
			break;
		udelay(1);
	} while (UARTDATA_UARTTXCSR_GET(status) == 0);
}

static void ar933x_uart_console_putchar(struct uart_port *port, int ch)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned int rdata;

	ar933x_uart_wait_xmitr(up);

	rdata = UARTDATA_UARTTXRXDATA_SET(ch) |
	        UARTDATA_UARTTXCSR_SET(1);
	ar933x_uart_write(up, UARTDATA_ADDRESS, rdata);
}

static void ar933x_uart_console_write(struct console *co, const char *s,
				      unsigned int count)
{
	struct ar933x_uart_port *up = ar933x_console_ports[co->index];
	unsigned long flags;
	unsigned int ier;
	int locked = 1;

	local_irq_save(flags);

	if (up->port.sysrq) {
		locked = 0;
	} else if (oops_in_progress) {
		locked = spin_trylock(&up->port.lock);
	} else
		spin_lock(&up->port.lock);

	/*
	 * First save the IER then disable the interrupts
	 */
	ier = ar933x_uart_read(up, UARTINTEN_ADDRESS);
	ar933x_uart_write(up, UARTINTEN_ADDRESS, 0);

	uart_console_write(&up->port, s, count, ar933x_uart_console_putchar);

	/*
	 * Finally, wait for transmitter to become empty
	 * and restore the IER
	 */
	ar933x_uart_wait_xmitr(up);

	ar933x_uart_write(up, UARTINTEN_ADDRESS, ier);
	ar933x_uart_write(up, UARTINT_ADDRESS, UARTINT_RSTMASK);

	if (locked)
		spin_unlock(&up->port.lock);

	local_irq_restore(flags);
}

static int ar933x_uart_console_setup(struct console *co, char *options)
{
	struct ar933x_uart_port *up;
	int baud = 115200;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (co->index < 0 || co->index >= CONFIG_SERIAL_AR933X_NR_UARTS)
		return -EINVAL;

	up = ar933x_console_ports[co->index];
	if (!up)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(&up->port, co, baud, parity, bits, flow);
}

static struct console ar933x_uart_console = {
	.name		= "ttyATH",
	.write		= ar933x_uart_console_write,
	.device		= uart_console_device,
	.setup		= ar933x_uart_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &ar933x_uart_driver,
};

static int __init ar933x_uart_console_init(void)
{
	register_console(&ar933x_uart_console);
	return 0;
}
console_initcall(ar933x_uart_console_init);

static void ar933x_uart_add_console_port(struct ar933x_uart_port *up)
{
	ar933x_console_ports[up->port.line] = up;
}

#define AR933X_SERIAL_CONSOLE	&ar933x_uart_console

#else

static inline void ar933x_uart_add_console_port(struct ar933x_uart_port *up) {}

#define AR933X_SERIAL_CONSOLE	NULL

#endif /* CONFIG_SERIAL_AR933X_CONSOLE */

static struct uart_driver ar933x_uart_driver = {
	.owner		= THIS_MODULE,
	.driver_name	= DRIVER_NAME,
	.dev_name	= "ttyATH",
	.nr		= CONFIG_SERIAL_AR933X_NR_UARTS,
	.cons		= AR933X_SERIAL_CONSOLE,
};

static int __devinit ar933x_uart_probe(struct platform_device *pdev)
{
	struct ar933x_uart_platform_data *pdata;
	struct ar933x_uart_port *up;
	struct uart_port *port;
	struct resource *mem_res;
	struct resource *irq_res;
	int id;
	int ret;

	pdata = pdev->dev.platform_data;
	if (!pdata)
		return -EINVAL;

	id = pdev->id;
	if (id == -1)
		id = 0;

	if (id > CONFIG_SERIAL_AR933X_NR_UARTS)
		return -EINVAL;

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem_res) {
		dev_err(&pdev->dev, "no MEM resource\n");
		return -EINVAL;
	}

	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!irq_res) {
		dev_err(&pdev->dev, "no IRQ resource\n");
		return -EINVAL;
	}

	up = kzalloc(sizeof(struct ar933x_uart_port), GFP_KERNEL);
	if (!up)
		return -ENOMEM;

	port = &up->port;
	port->mapbase = mem_res->start;

	port->membase = ioremap(mem_res->start, AR933X_UART_REGS_SIZE);
	if (!port->membase) {
		ret = -ENOMEM;
		goto err_free_up;
	}

	port->line = id;
	port->irq = irq_res->start;
	port->dev = &pdev->dev;
	port->type = PORT_AR933X;
	port->iotype = UPIO_MEM32;
	port->uartclk = pdata->uartclk;

	port->regshift = 2;
	port->fifosize = AR933X_UART_FIFO_SIZE;
	port->ops = &ar933x_uart_ops;

	ar933x_uart_add_console_port(up);

	ret = uart_add_one_port(&ar933x_uart_driver, &up->port);
	if (ret)
		goto err_unmap;

	platform_set_drvdata(pdev, up);
	return 0;

err_unmap:
	iounmap(up->port.membase);
err_free_up:
	kfree(up);
	return ret;
}

static int __devexit ar933x_uart_remove(struct platform_device *pdev)
{
	struct ar933x_uart_port *up;

	up = platform_get_drvdata(pdev);
	platform_set_drvdata(pdev, NULL);

	if (up) {
		uart_remove_one_port(&ar933x_uart_driver, &up->port);
		iounmap(up->port.membase);
		kfree(up);
	}

	return 0;
}

static struct platform_driver ar933x_uart_platform_driver = {
	.probe		= ar933x_uart_probe,
	.remove		= __devexit_p(ar933x_uart_remove),
	.driver		= {
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
};

static int __init ar933x_uart_init(void)
{
	int ret;

	ar933x_uart_driver.nr = CONFIG_SERIAL_AR933X_NR_UARTS;
	ret = uart_register_driver(&ar933x_uart_driver);
	if (ret)
		goto err_out;

	ret = platform_driver_register(&ar933x_uart_platform_driver);
	if (ret)
		goto err_unregister_uart_driver;

	return 0;

err_unregister_uart_driver:
	uart_unregister_driver(&ar933x_uart_driver);
err_out:
	return ret;
}

static void __exit ar933x_uart_exit(void)
{
	platform_driver_unregister(&ar933x_uart_platform_driver);
	uart_unregister_driver(&ar933x_uart_driver);
}

module_init(ar933x_uart_init);
module_exit(ar933x_uart_exit);

MODULE_DESCRIPTION("Atheros AR933X UART driver");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
