/*
 * drivers/video/ubicom32lcd.c
 *	LCD initilization code
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/ip5000.h>
#include <asm/gpio.h>
#include <asm/ubicom32lcd.h>

#include "ubicom32lcd.h"

#define DRIVER_NAME			"ubicom32lcd"

struct ubicom32lcd_data {
	const struct ubicom32lcd_panel	*panel;

	int				pin_cs;
	int				pin_rd;
	int				pin_rs;
	int				pin_wr;
	int				pin_reset;
	struct ubicom32_io_port		*port_data;
	int				data_shift;
};

/*
 * ubicom32lcd_write
 *	Performs a write cycle on the bus (assumes CS asserted, RD & WR set)
 */
static void ubicom32lcd_write(struct ubicom32lcd_data *ud, int command, u16 data)
{
	if (command) {
		UBICOM32_GPIO_SET_PIN_LOW(ud->pin_rs);
	} else {
		UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_rs);
	}

	asm volatile (
		"or.4	4(%[port]), 4(%[port]), %[mask]	\n\t"
		"not.4	%[mask], %[mask]		\n\t"
		"and.4	8(%[port]), 8(%[port]), %[mask]	\n\t"
		"or.4	8(%[port]), 8(%[port]), %[cmd]	\n\t"
		:
		: [port] "a" (ud->port_data),
		  [mask] "d" (0xFFFF << ud->data_shift),
		  [cmd] "d" (data << ud->data_shift)
		: "cc"
	);

	UBICOM32_GPIO_SET_PIN_LOW(ud->pin_wr);

	//ndelay(50);
	udelay(1);

	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_wr);

	udelay(1);
	//ndelay(50);
}

/*
 * ubicom32lcd_read_data
 *	Performs a read cycle on the bus (assumes CS asserted, RD & WR set)
 */
static u16 ubicom32lcd_read_data(struct ubicom32lcd_data *ud)
{
	u32_t data;

	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_rs);

	asm volatile (
		"and.4	4(%[port]), 4(%[port]), %[mask]\n\t"
		:
		: [port] "a" (ud->port_data),
		  [mask] "d" (~(0xFFFF << ud->data_shift))
		: "cc"
	);

	UBICOM32_GPIO_SET_PIN_LOW(ud->pin_rd);

	ndelay(300);

	asm volatile (
		"lsr.4	%[data], 12(%[port]), %[shamt]	\n\t"
		"and.4	%[data], %[data], %[mask]	\n\t"
		: [data] "=d" (data)
		: [port] "a" (ud->port_data),
		  [mask] "d" (0xFFFF),
		  [shamt] "d" (ud->data_shift)
		: "cc"
	);

	ndelay(200);

	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_rd);

	ndelay(500);

	return data;
}

/*
 * ubicom32lcd_execute
 *	Executes a script for performing operations on the LCD (assumes CS set)
 */
static void ubicom32lcd_execute(struct ubicom32lcd_data *ud, const struct ubicom32lcd_step *script)
{
	while (1) {
		switch (script->op) {
		case LCD_STEP_CMD:
			ubicom32lcd_write(ud, 1, script->cmd);
			break;

		case LCD_STEP_DATA:
			ubicom32lcd_write(ud, 0, script->data);
			break;

		case LCD_STEP_CMD_DATA:
			ubicom32lcd_write(ud, 1, script->cmd);
			ubicom32lcd_write(ud, 0, script->data);
			break;

		case LCD_STEP_SLEEP:
			udelay(script->data);
			break;

		case LCD_STEP_DONE:
			return;
		}
		script++;
	}
}

/*
 * ubicom32lcd_goto
 *	Places the gram pointer at a specific X, Y address
 */
static void ubicom32lcd_goto(struct ubicom32lcd_data *ud, int x, int y)
{
	ubicom32lcd_write(ud, 1, ud->panel->horz_reg);
	ubicom32lcd_write(ud, 0, x);
	ubicom32lcd_write(ud, 1, ud->panel->vert_reg);
	ubicom32lcd_write(ud, 0, y);
	ubicom32lcd_write(ud, 1, ud->panel->gram_reg);
}

/*
 * ubicom32lcd_panel_init
 *	Initializes the lcd panel.
 */
static int ubicom32lcd_panel_init(struct ubicom32lcd_data *ud)
{
	u16 id;

	UBICOM32_GPIO_SET_PIN_LOW(ud->pin_reset);
	UBICOM32_GPIO_SET_PIN_OUTPUT(ud->pin_reset);
	UBICOM32_GPIO_ENABLE(ud->pin_reset);

	asm volatile (
		"or.4	0x50(%[port]), 0x50(%[port]), %[mask]	\n\t"
		"not.4	%[mask], %[mask]			\n\t"
		"and.4	0x04(%[port]), 0x04(%[port]), %[mask]	\n\t"
		:
		: [port] "a" (ud->port_data),
		  [mask] "d" (0xFFFF << ud->data_shift)
		: "cc"
	);

	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_rs);
	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_rd);
	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_wr);
	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_cs);

	UBICOM32_GPIO_SET_PIN_OUTPUT(ud->pin_rs);
	UBICOM32_GPIO_SET_PIN_OUTPUT(ud->pin_rd);
	UBICOM32_GPIO_SET_PIN_OUTPUT(ud->pin_wr);
	UBICOM32_GPIO_SET_PIN_OUTPUT(ud->pin_cs);

	UBICOM32_GPIO_ENABLE(ud->pin_rs);
	UBICOM32_GPIO_ENABLE(ud->pin_rd);
	UBICOM32_GPIO_ENABLE(ud->pin_wr);
	UBICOM32_GPIO_ENABLE(ud->pin_cs);

	udelay(20);

	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_reset);

	udelay(20);

	UBICOM32_GPIO_SET_PIN_LOW(ud->pin_cs);

	id = ubicom32lcd_read_data(ud);

	/*
	 * We will try to figure out what kind of panel we have if we were not told.
	 */
	if (!ud->panel) {
		const struct ubicom32lcd_panel **p = ubicom32lcd_panels;
		while (*p) {
			if ((*p)->id && ((*p)->id == id)) {
				break;
			}
			p++;
		}
		if (!*p) {
			printk(KERN_WARNING DRIVER_NAME ":Could not find compatible panel, id=%x\n", id);
			return -ENODEV;
		}
		ud->panel = *p;
	}

	/*
	 * Make sure panel ID matches if we were supplied a panel type
	 */
	if (ud->panel->id && (ud->panel->id != id)) {
		UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_cs);

		return -ENODEV;
	}

	ubicom32lcd_execute(ud, ud->panel->init_seq);

	ubicom32lcd_goto(ud, 0, 0);

	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_cs);
	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_rd);
	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_wr);
	UBICOM32_GPIO_SET_PIN_HIGH(ud->pin_rs);

	printk(KERN_INFO DRIVER_NAME ": Initialized panel %s\n", ud->panel->desc);

	return 0;
}

/*
 * ubicom32lcd_probe
 */
static int ubicom32lcd_probe(struct platform_device *pdev)
{
	const struct ubicom32lcd_platform_data *pdata = pdev->dev.platform_data;
	struct ubicom32lcd_data *ud;
	int retval;

	/*
	 * Allocate our private data
	 */
	ud = kzalloc(sizeof(struct ubicom32lcd_data), GFP_KERNEL);
	if (!ud) {
		return -ENOMEM;
	}

	if (pdata) {
		ud->pin_cs = pdata->pin_cs;
		ud->pin_rd = pdata->pin_rd;
		ud->pin_wr = pdata->pin_wr;
		ud->pin_rs = pdata->pin_rs;
		ud->pin_reset = pdata->pin_reset;
		ud->port_data = pdata->port_data;
		ud->data_shift = pdata->data_shift;
	} else {
		/*
		 * Defaults
		 */
		ud->pin_cs = GPIO_RD_4;
		ud->pin_rd = GPIO_RD_5;
		ud->pin_rs = GPIO_RD_3;
		ud->pin_wr = GPIO_RD_2;
		ud->pin_reset = GPIO_RD_7;
		ud->port_data = (struct ubicom32_io_port *)RI;
		ud->data_shift = 0;
	}

	/*
	 * Initialize the display
	 */
	retval = ubicom32lcd_panel_init(ud);
	if (retval) {
		kfree(ud);
		return retval;
	}

	printk(KERN_INFO DRIVER_NAME ": LCD initialized\n");

	return 0;
}

/*
 * ubicom32lcd_remove
 */
static int __exit ubicom32lcd_remove(struct platform_device *pdev)
{
	struct ubicom32lcd_data *ud = platform_get_drvdata(pdev);

	kfree(ud);

	return 0;
}

static struct platform_driver ubicom32lcd_driver = {
	.probe          = ubicom32lcd_probe,
	.remove         = ubicom32lcd_remove,

	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},

	.remove = __exit_p(ubicom32lcd_remove),
};

static struct platform_device *ubicom32lcd_device;

/*
 * ubicom32lcd_init
 */
static int __init ubicom32lcd_init(void)
{
	int res;

	res = platform_driver_register(&ubicom32lcd_driver);
	if (res == 0) {
		ubicom32lcd_device = platform_device_alloc(DRIVER_NAME, 0);
		if (ubicom32lcd_device) {
			res = platform_device_add(ubicom32lcd_device);
		} else {
			res = -ENOMEM;
		}
		if (res) {
			platform_device_put(ubicom32lcd_device);
			platform_driver_unregister(&ubicom32lcd_driver);
		}
	}
	return res;
}
module_init(ubicom32lcd_init);

/*
 * ubicom32lcd_exit
 */
static void __exit ubicom32lcd_exit(void)
{
	platform_device_unregister(ubicom32lcd_device);
	platform_driver_unregister(&ubicom32lcd_driver);
}
module_exit(ubicom32lcd_exit);

MODULE_AUTHOR("Patrick Tjin <@ubicom.com>");
MODULE_DESCRIPTION("Ubicom32 LCD driver");
MODULE_LICENSE("GPL");
