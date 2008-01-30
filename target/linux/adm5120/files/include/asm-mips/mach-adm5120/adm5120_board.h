/*
 *  $Id$
 *
 *  ADM5120 board definitions
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#ifndef _ADM5120_BOARD_H_
#define _ADM5120_BOARD_H_

#include <linux/init.h>
#include <linux/list.h>

#define ADM5120_BOARD_NAMELEN	64

struct adm5120_board {
	unsigned long		mach_type;
	char			name[ADM5120_BOARD_NAMELEN];

	void			(*board_setup)(void);
	void			(*board_reset)(void);

	unsigned int		eth_num_ports;
	unsigned char		*eth_vlans;
	unsigned int		num_devices;
	struct platform_device	**devices;
	unsigned int		pci_nr_irqs;
	struct adm5120_pci_irq	*pci_irq_map;

	struct list_head	list;
};

extern void adm5120_board_register(struct adm5120_board *) __init;

#define ADM5120_BOARD_START(_type, _name)				\
static struct adm5120_board adm5120_board_##_type __initdata;		\
									\
static __init int adm5120_board_##_type##_register(void)		\
{									\
	adm5120_board_register(&adm5120_board_##_type);			\
	return 0;							\
}									\
pure_initcall(adm5120_board_##_type##_register);			\
									\
static struct adm5120_board adm5120_board_##_type __initdata = {	\
	.mach_type	= MACH_ADM5120_##_type,				\
	.name		= _name,

#define ADM5120_BOARD_END						\
};

#endif /* _ADM5120_BOARD_H_ */
