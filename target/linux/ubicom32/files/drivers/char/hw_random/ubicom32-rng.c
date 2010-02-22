/*
 * drivers/net/ubi32-eth.c
 *   Ubicom32 hardware random number generator driver.
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hw_random.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/ip5000.h>

#define MODULE_NAME "ubicom32_rng"

static int ubicom32_rng_data_present(struct hwrng *rng, int wait)
{
	int data, i;

	for (i = 0; i < 20; i++) {
		data = *(int *)(TIMER_BASE + TIMER_TRN);
		if (data || !wait)
			break;
		udelay(10);
	}
	return data;
}

static int ubicom32_rng_data_read(struct hwrng *rng, u32 *data)
{
	*data = *(int *)(TIMER_BASE + TIMER_TRN);
	return 4;
}

static int ubicom32_rng_init(struct hwrng *rng)
{
	printk(KERN_INFO "ubicom32 rng init\n");
	*(int *)(TIMER_BASE + TIMER_TRN_CFG) = TIMER_TRN_CFG_ENABLE_OSC;
	return 0;
}

static void ubicom32_rng_cleanup(struct hwrng *rng)
{
	printk(KERN_INFO "ubicom32 rng cleanup\n");
	*(int *)(TIMER_BASE + TIMER_TRN_CFG) = 0;
}

static struct hwrng ubicom32_rng = {
	.name		= MODULE_NAME,
	.init		= ubicom32_rng_init,
	.cleanup	= ubicom32_rng_cleanup,
	.data_present	= ubicom32_rng_data_present,
	.data_read	= ubicom32_rng_data_read,
	.priv		= 0,
};

static int __init mod_init(void)
{
	int err;

	printk(KERN_INFO "ubicom32 rng started\n");
	err = hwrng_register(&ubicom32_rng);
	if (err) {
		printk(KERN_ERR "ubicom32 rng register failed (%d)\n",
			err);
	}

	return err;
}

static void __exit mod_exit(void)
{
	printk(KERN_INFO "ubicom32 rng stopped\n");
	hwrng_unregister(&ubicom32_rng);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ubicom, Inc.");
MODULE_DESCRIPTION("H/W rng driver for ubicom32 processor");
MODULE_VERSION("1:1.0.a");
