/*
 *  ADM5120 board definitions
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#ifndef _ASM_MIPS_MACH_ADM5120_BOARD_H
#define _ASM_MIPS_MACH_ADM5120_BOARD_H

#include <linux/init.h>
#include <linux/list.h>

#define ADM5120_BOARD_NAMELEN	64

struct adm5120_board {
	unsigned long		mach_type;
	char			name[ADM5120_BOARD_NAMELEN];

	void			(*board_setup)(void);
	struct list_head	list;
};

extern void adm5120_board_register(struct adm5120_board *) __init;

#define ADM5120_BOARD(_type, _name, _setup)				\
static struct adm5120_board adm5120_board_##_type __initdata = {	\
	.mach_type	= _type,					\
	.name		= _name,					\
	.board_setup	= _setup,					\
};									\
									\
static __init int adm5120_board_##_type##_register(void)		\
{									\
	adm5120_board_register(&adm5120_board_##_type);			\
	return 0;							\
}									\
pure_initcall(adm5120_board_##_type##_register)

#endif /* _ASM_MIPS_MACH_ADM5120_BOARD_H */
