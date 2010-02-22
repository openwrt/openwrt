/*
 * arch/ubicom32/mach-common/ubicom32input_i2c.c
 *   Ubicom32 Input driver for I2C
 *	  Supports PCA953x and family
 *
 *   We hog the I2C device, turning it all to input.
 *
 *   Based on gpio-keys, pca953x
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
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/i2c.h>

#include <asm/ubicom32input_i2c.h>

#define UBICOM32INPUT_I2C_REG_INPUT	0
#define UBICOM32INPUT_I2C_REG_OUTPUT	1
#define UBICOM32INPUT_I2C_REG_INVERT	2
#define UBICOM32INPUT_I2C_REG_DIRECTION	3

static const struct i2c_device_id ubicom32input_i2c_id[] = {
	{ "ubicom32in_pca9534", 8, },
	{ "ubicom32in_pca9535", 16, },
	{ "ubicom32in_pca9536", 4, },
	{ "ubicom32in_pca9537", 4, },
	{ "ubicom32in_pca9538", 8, },
	{ "ubicom32in_pca9539", 16, },
	{ "ubicom32in_pca9554", 8, },
	{ "ubicom32in_pca9555", 16, },
	{ "ubicom32in_pca9557", 8, },
	{ "ubicom32in_max7310", 8, },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ubicom32input_i2c_id);

struct ubicom32input_i2c_data {
	struct ubicom32input_i2c_platform_data	*pdata;

	struct i2c_client			*client;

	struct input_polled_dev			*poll_dev;

	/*
	 * collection of previous states for buttons
	 */
	uint16_t				prev_state;

	uint8_t					ngpios;
};

/*
 * ubicom32input_i2c_write_reg
 *	writes a register to the I2C device.
 */
static int ubicom32input_i2c_write_reg(struct ubicom32input_i2c_data *ud,
				       int reg, uint16_t val)
{
	int ret;

	if (ud->ngpios <= 8) {
		ret = i2c_smbus_write_byte_data(ud->client, reg, val);
	} else {
		ret = i2c_smbus_write_word_data(ud->client, reg << 1, val);
	}

	if (ret < 0) {
		return ret;
	}

	return 0;
}

/*
 * ubicom32input_i2c_read_reg
 *	reads a register from the I2C device.
 */
static int ubicom32input_i2c_read_reg(struct ubicom32input_i2c_data *ud,
				      int reg, uint16_t *val)
{
	int ret;

	if (ud->ngpios <= 8) {
		ret = i2c_smbus_read_byte_data(ud->client, reg);
	} else {
		ret = i2c_smbus_read_word_data(ud->client, reg);
	}

	if (ret < 0) {
		return ret;
	}

	*val = (uint16_t)ret;

	return 0;
}

/*
 * ubicom32input_i2c_poll
 */
static void ubicom32input_i2c_poll(struct input_polled_dev *dev)
{
	struct ubicom32input_i2c_data *ud =
		(struct ubicom32input_i2c_data *)dev->private;
	struct ubicom32input_i2c_platform_data *pdata = ud->pdata;
	struct input_dev *id = dev->input;
	int i;
	int sync_needed = 0;
	uint16_t val;
	uint16_t change_mask;

	/*
	 * Try to get the input status, if we fail, bail out, maybe we can do it
	 * next time.
	 */
	if (ubicom32input_i2c_read_reg(ud, UBICOM32INPUT_I2C_REG_INPUT, &val)) {
		return;
	}

	/*
	 * see if anything changed by using XOR
	 */
	change_mask = ud->prev_state ^ val;
	ud->prev_state = val;

	for (i = 0; i < pdata->nbuttons; i++) {
		const struct ubicom32input_i2c_button *ub = &pdata->buttons[i];
		uint16_t mask = 1 << ub->bit;
		int state = val & mask;

		/*
		 * Check to see if the state changed from the last time we
		 * looked
		 */
		if (!(change_mask & mask)) {
			continue;
		}
		input_event(id, ub->type, ub->code, state);
		sync_needed = 1;
	}

	if (sync_needed) {
		input_sync(id);
	}
}

/*
 * ubicom32input_i2c_probe
 */
static int __devinit ubicom32input_i2c_probe(struct i2c_client *client,
					     const struct i2c_device_id *id)
{
	int i;
	struct ubicom32input_i2c_data *ud;
	struct input_polled_dev *poll_dev;
	struct input_dev *input_dev;
	struct ubicom32input_i2c_platform_data *pdata;
	int ret;
	uint16_t invert_mask = 0;

	pdata = client->dev.platform_data;
	if (!pdata) {
		return -EINVAL;
	}

	ud = kzalloc(sizeof(struct ubicom32input_i2c_data), GFP_KERNEL);
	if (!ud) {
		return -ENOMEM;
	}
	ud->pdata = pdata;
	ud->client = client;
	ud->ngpios = id->driver_data;

	poll_dev = input_allocate_polled_device();
	if (!poll_dev) {
		ret = -ENOMEM;
		goto fail;
	}

	ud->poll_dev = poll_dev;
	poll_dev->private = ud;
	poll_dev->poll = ubicom32input_i2c_poll;

	/*
	 * Set the poll interval requested, default to 100 msec
	 */
	if (pdata->poll_interval) {
		poll_dev->poll_interval = pdata->poll_interval;
	} else {
		poll_dev->poll_interval = 100;
	}

	/*
	 * Setup the input device
	 */
	input_dev = poll_dev->input;
	input_dev->name = pdata->name ? pdata->name : "Ubicom32 Input I2C";
	input_dev->phys = "ubicom32input_i2c/input0";
	input_dev->dev.parent = &client->dev;
	input_dev->id.bustype = BUS_I2C;

	/*
	 * Set the capabilities
	 */
	for (i = 0; i < pdata->nbuttons; i++) {
		const struct ubicom32input_i2c_button *ub = &pdata->buttons[i];

		if (ub->active_low) {
			invert_mask |= (1 << ub->bit);
		}

		input_set_capability(input_dev,
				     ub->type ? ub->type : EV_KEY, ub->code);
	}

	/*
	 * Setup the device (all inputs)
	 */
	ret = ubicom32input_i2c_write_reg(ud, UBICOM32INPUT_I2C_REG_DIRECTION,
					  0xFFFF);
	if (ret < 0) {
		goto fail;
	}

	ret = ubicom32input_i2c_write_reg(ud, UBICOM32INPUT_I2C_REG_INVERT,
					  invert_mask);
	if (ret < 0) {
		goto fail;
	}

	/*
	 * Register
	 */
	ret = input_register_polled_device(ud->poll_dev);
	if (ret) {
		goto fail;
	}

	i2c_set_clientdata(client, ud);

	return 0;

fail:
	printk(KERN_ERR "ubicom32input_i2c: Failed to register driver %d\n",
	       ret);
	input_free_polled_device(poll_dev);
	kfree(ud);
	return ret;
}

/*
 * ubicom32input_i2c_remove
 */
static int __devexit ubicom32input_i2c_remove(struct i2c_client *client)
{
	struct ubicom32input_i2c_data *ud =
		(struct ubicom32input_i2c_data *)i2c_get_clientdata(client);

	i2c_set_clientdata(client, NULL);
	input_unregister_polled_device(ud->poll_dev);
	input_free_polled_device(ud->poll_dev);

	kfree(ud);

	return 0;
}

static struct i2c_driver ubicom32input_i2c_driver = {
	.driver         = {
		.name   = "ubicom32input_i2c",
		.owner  = THIS_MODULE,
	},
	.remove         = __devexit_p(ubicom32input_i2c_remove),
	.id_table	= ubicom32input_i2c_id,
	.probe          = ubicom32input_i2c_probe,
};

/*
 * ubicom32input_i2c_init
 */
static int __devinit ubicom32input_i2c_init(void)
{
	return i2c_add_driver(&ubicom32input_i2c_driver);
}

/*
 * ubicom32input_i2c_exit
 */
static void __exit ubicom32input_i2c_exit(void)
{
	i2c_del_driver(&ubicom32input_i2c_driver);
}

module_init(ubicom32input_i2c_init);
module_exit(ubicom32input_i2c_exit);

MODULE_AUTHOR("Pat Tjin <pattjin@ubicom.com>");
MODULE_DESCRIPTION("Ubicom32 Input Driver I2C");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:ubicom32-input");
