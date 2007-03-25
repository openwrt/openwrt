/*
 * $Id$
 * 
 * Copyright (C) 2006, 2007 OpenWrt.org
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __AR7_H__
#define __AR7_H__

#include <asm/addrspace.h>
#include <linux/delay.h>

#define AR7_REGS_BASE 0x08610000

#define AR7_REGS_MAC0   (AR7_REGS_BASE + 0x0000)
#define AR7_REGS_EMIF   (AR7_REGS_BASE + 0x0800)
#define AR7_REGS_GPIO   (AR7_REGS_BASE + 0x0900)
#define AR7_REGS_POWER  (AR7_REGS_BASE + 0x0a00)
#define AR7_REGS_WDT    (AR7_REGS_BASE + 0x0b00)
#define AR7_REGS_TIMER0 (AR7_REGS_BASE + 0x0c00)
#define AR7_REGS_TIMER1 (AR7_REGS_BASE + 0x0d00)
#define AR7_REGS_UART0  (AR7_REGS_BASE + 0x0e00)
#define AR7_REGS_UART1  (AR7_REGS_BASE + 0x0f00)
#define AR7_REGS_I2C    (AR7_REGS_BASE + 0x1000)
#define AR7_REGS_USB    (AR7_REGS_BASE + 0x1200)
#define AR7_REGS_DMA    (AR7_REGS_BASE + 0x1400)
#define AR7_REGS_RESET  (AR7_REGS_BASE + 0x1600)
#define AR7_REGS_BIST   (AR7_REGS_BASE + 0x1700)
#define AR7_REGS_VLYNQ0 (AR7_REGS_BASE + 0x1800)
#define AR7_REGS_DCL    (AR7_REGS_BASE + 0x1A00)
#define AR7_REGS_VLYNQ1 (AR7_REGS_BASE + 0x1C00)
#define AR7_REGS_MDIO   (AR7_REGS_BASE + 0x1E00)
#define AR7_REGS_FSER   (AR7_REGS_BASE + 0x2000)
#define AR7_REGS_IRQ    (AR7_REGS_BASE + 0x2400)
#define AR7_REGS_MAC1   (AR7_REGS_BASE + 0x2800)

#define  AR7_RESET_PEREPHERIAL 0x0
#define  AR7_RESET_SOFTWARE    0x4
#define  AR7_RESET_STATUS      0x8

#define AR7_RESET_BIT_MDIO   22

/* GPIO control registers */
#define  AR7_GPIO_INPUT  0x0
#define  AR7_GPIO_OUTPUT 0x4
#define  AR7_GPIO_DIR    0x8
#define  AR7_GPIO_ENABLE 0xC

#define AR7_GPIO_BIT_STATUS_LED   8

#define AR7_CHIP_7100 0x18
#define AR7_CHIP_7200 0x2b
#define AR7_CHIP_7300 0x05

/* Interrupts */
#define AR7_IRQ_UART0  15
#define AR7_IRQ_UART1  16

struct plat_cpmac_data {
	int reset_bit;
	int power_bit;
	u32 phy_mask;
	char dev_addr[6];
};

struct plat_dsl_data {
	int reset_bit_dsl;
	int reset_bit_sar;
};

extern char *prom_getenv(char *envname);

extern int ar7_cpu_clock, ar7_bus_clock, ar7_dsp_clock;

static inline u16 ar7_chip_id(void)
{
	return readl((void *)KSEG1ADDR(AR7_REGS_GPIO + 0x14)) & 0xffff;
}

static inline u8 ar7_chip_rev(void)
{
	return (readl((void *)KSEG1ADDR(AR7_REGS_GPIO + 0x14)) >> 16) & 0xff;
}

static inline int ar7_cpu_freq(void)
{
	return ar7_cpu_clock;
}

static inline int ar7_bus_freq(void)
{
	return ar7_bus_clock;
}

static inline int ar7_vbus_freq(void)
{
	return ar7_bus_clock / 2;
}
#define ar7_cpmac_freq ar7_vbus_freq

static inline int ar7_dsp_freq(void)
{
	return ar7_dsp_clock;
}

static inline int ar7_has_high_cpmac(void)
{
	u16 chip_id = ar7_chip_id();
	switch (chip_id) {
	case AR7_CHIP_7100:
	case AR7_CHIP_7200:
		return 0;
	default:
		return 1;
	}
}
#define ar7_has_high_vlynq ar7_has_high_cpmac

static inline void ar7_device_enable(u32 bit)
{
	void *reset_reg = (void *)KSEG1ADDR(AR7_REGS_RESET + AR7_RESET_PEREPHERIAL);
	writel(readl(reset_reg) | (1 << bit), reset_reg);
	mdelay(20);
}

static inline void ar7_device_disable(u32 bit)
{
	void *reset_reg = (void *)KSEG1ADDR(AR7_REGS_RESET + AR7_RESET_PEREPHERIAL);
	writel(readl(reset_reg) & ~(1 << bit), reset_reg);
	mdelay(20);
}

static inline void ar7_device_reset(u32 bit)
{
	ar7_device_disable(bit);
	ar7_device_enable(bit);
}

static inline void ar7_device_on(u32 bit)
{
	void *power_reg = (void *)KSEG1ADDR(AR7_REGS_POWER);
	writel(readl(power_reg) | (1 << bit), power_reg);
	mdelay(20);
}

static inline void ar7_device_off(u32 bit)
{
	void *power_reg = (void *)KSEG1ADDR(AR7_REGS_POWER);
	writel(readl(power_reg) & ~(1 << bit), power_reg);
	mdelay(20);
}

#endif
