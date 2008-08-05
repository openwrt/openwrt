/*
 *  $Id$
 *
 *  ADM5120 specific platform definitions
 *
 *  Copyright (C) 2007-2008 OpenWrt.org
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#ifndef _ADM5120_PLATFORM_H_
#define _ADM5120_PLATFORM_H_

#include <linux/device.h>
#include <linux/platform_device.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/nand.h>

#include <linux/input.h>
#include <linux/gpio_buttons.h>

#include <linux/amba/bus.h>
#include <linux/amba/serial.h>

struct adm5120_flash_platform_data {
	void			(*set_vpp)(struct map_info *, int);
	void			(*switch_bank)(unsigned);
	u32			window_size;
#ifdef CONFIG_MTD_PARTITIONS
	unsigned int		nr_parts;
	struct mtd_partition	*parts;
#endif
};

struct adm5120_switch_platform_data {
	/* TODO: not yet implemented */
};

struct adm5120_pci_irq {
	u8	slot;
	u8	func;
	u8	pin;
	unsigned irq;
};

#define PCIIRQ(s,f,p,i) {.slot = (s), .func = (f), .pin  = (p), .irq  = (i)}

#ifdef CONFIG_PCI
extern void adm5120_pci_set_irq_map(unsigned int nr_irqs,
		struct adm5120_pci_irq *map) __init;
#else
static inline void adm5120_pci_set_irq_map(unsigned int nr_irqs,
		struct adm5120_pci_irq *map)
{
}
#endif

#define ADM5120_NUM_BUTTONS	5

extern struct resource adm5120_gpiodev_resource;

extern struct adm5120_flash_platform_data adm5120_flash0_data;
extern struct adm5120_flash_platform_data adm5120_flash1_data;
extern struct platform_nand_data adm5120_nand_data;
extern struct adm5120_switch_platform_data adm5120_switch_data;
extern struct gpio_button adm5120_buttons[ADM5120_NUM_BUTTONS];
extern struct gpio_buttons_platform_data adm5120_buttons_data;
extern struct amba_pl010_data adm5120_uart0_data;
extern struct amba_pl010_data adm5120_uart1_data;

extern struct platform_device adm5120_flash0_device;
extern struct platform_device adm5120_flash1_device;
extern struct platform_device adm5120_nand_device;
extern struct platform_device adm5120_hcd_device;
extern struct platform_device adm5120_switch_device;
extern struct platform_device adm5120_buttons_device;
extern struct platform_device adm5120_gpiodev_device;
extern struct amba_device adm5120_uart0_device;
extern struct amba_device adm5120_uart1_device;

extern void adm5120_uart_set_mctrl(struct amba_device *dev, void __iomem *base,
		unsigned int mctrl);

extern void adm5120_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,
		unsigned int ctrl);
extern int adm5120_nand_ready(struct mtd_info *mtd);

#endif /* _ADM5120_PLATFORM_H_ */
