/*
 *  ADM5120 generic board code
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <asm/bootinfo.h>

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_board.h>
#include <asm/mach-adm5120/adm5120_platform.h>

#define PFX	"ADM5120: "

static struct list_head adm5120_boards __initdata =
			LIST_HEAD_INIT(adm5120_boards);

static char adm5120_board_name[ADM5120_BOARD_NAMELEN] = "Unknown board";

const char *get_system_type(void)
{
	return adm5120_board_name;
}

static struct adm5120_board * __init adm5120_board_find(unsigned long machtype)
{
	struct list_head *this;

	list_for_each(this, &adm5120_boards) {
		struct adm5120_board *board;

		board = list_entry(this, struct adm5120_board, list);
		if (board->mach_type == machtype)
			return board;
	}

	return NULL;
}

static int __init adm5120_board_setup(void)
{
	struct adm5120_board *board;

	board = adm5120_board_find(mips_machtype);
	if (board == NULL)
		panic(PFX "no board registered for machtype %lu\n",
			mips_machtype);

	if (board->name[0])
		strlcpy(adm5120_board_name, board->name, ADM5120_BOARD_NAMELEN);

	printk(KERN_INFO PFX "board is '%s'\n", adm5120_board_name);

	adm5120_gpio_init();

	if (board->board_setup)
		board->board_setup();

	return 0;
}
arch_initcall(adm5120_board_setup);

void __init adm5120_board_register(struct adm5120_board *board)
{
	list_add_tail(&board->list, &adm5120_boards);
}

static void __init adm5120_generic_board_setup(void)
{
	adm5120_add_device_uart(0);
	adm5120_add_device_uart(1);

	adm5120_add_device_flash(0);
	adm5120_add_device_switch(6, NULL);
}

ADM5120_BOARD(MACH_ADM5120_GENERIC, "Generic ADM5120 board",
		adm5120_generic_board_setup);
