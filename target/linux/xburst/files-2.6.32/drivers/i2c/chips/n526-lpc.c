/*
 * Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#include <linux/workqueue.h>

#include <asm/mach-jz4740/irq.h>
#include <asm/mach-jz4740/gpio.h>

struct n526_lpc {
	struct i2c_client *client;
	struct input_dev *input;

	struct work_struct work;
};

static const unsigned int n526_lpc_keymap[] = {
	[0x01] = KEY_PAGEUP,
	[0x02] = KEY_PAGEDOWN,
	[0x03] = KEY_VOLUMEUP,
	[0x04] = KEY_VOLUMEDOWN,
	[0x06] = KEY_1,
	[0x07] = KEY_Q,
	[0x08] = KEY_A,
	[0x09] = KEY_Z,
	[0x0a] = KEY_LEFTSHIFT,
	[0x0b] = KEY_2,
	[0x0c] = KEY_W,
	[0x0d] = KEY_S,
	[0x0e] = KEY_X,
	[0x0f] = KEY_REFRESH,
	[0x10] = KEY_3,
	[0x11] = KEY_E,
	[0x12] = KEY_D,
	[0x13] = KEY_C,
	[0x14] = KEY_DOCUMENTS,
	[0x15] = KEY_4,
	[0x16] = KEY_R,
	[0x17] = KEY_F,
	[0x18] = KEY_V,
	[0x19] = KEY_MUTE,
	[0x1a] = KEY_5,
	[0x1b] = KEY_T,
	[0x1c] = KEY_G,
	[0x1d] = KEY_B,
	[0x1e] = KEY_DELETE,
	[0x1f] = KEY_6,
	[0x20] = KEY_Y,
	[0x21] = KEY_H,
	[0x22] = KEY_N,
	[0x23] = KEY_SPACE,
	[0x24] = KEY_7,
	[0x25] = KEY_U,
	[0x26] = KEY_J,
	[0x27] = KEY_M,
/*	[0x28] = KEY_SYM, */
	[0x29] = KEY_8,
	[0x2a] = KEY_I,
	[0x2b] = KEY_K,
	[0x2c] = KEY_MENU,
	[0x2d] = KEY_LEFT,
	[0x2e] = KEY_9,
	[0x2f] = KEY_O,
	[0x30] = KEY_L,
	[0x31] = KEY_UP,
	[0x32] = KEY_DOWN,
	[0x33] = KEY_0,
	[0x34] = KEY_P,
	[0x35] = KEY_BACKSPACE,
	[0x36] = KEY_ENTER,
	[0x37] = KEY_RIGHT,
};

static void n526_lpc_irq_work(struct work_struct *work)
{
	int ret;
	struct n526_lpc *n526_lpc = container_of(work, struct n526_lpc, work);
	struct i2c_client *client = n526_lpc->client;
	unsigned char raw_msg;
	struct i2c_msg msg = {client->addr, client->flags | I2C_M_RD, 1, &raw_msg};
	unsigned char keycode;


	ret = i2c_transfer(client->adapter, &msg, 1);

	if (ret != 1) {
		dev_err(&client->dev, "Failed to read lpc status\n");
	}

	keycode = raw_msg & 0x7f;

	if (keycode < ARRAY_SIZE(n526_lpc_keymap)) {
		input_report_key(n526_lpc->input, n526_lpc_keymap[keycode],
		                 !(raw_msg & 0x80));
		input_sync(n526_lpc->input);
	}
}

static irqreturn_t n526_lpc_irq(int irq, void *dev_id)
{
	struct n526_lpc *n526_lpc = dev_id;

	schedule_work(&n526_lpc->work);
	return IRQ_HANDLED;
}

static int __devinit n526_lpc_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int ret;
	size_t i;
	struct n526_lpc *n526_lpc;
	struct input_dev *input;

	n526_lpc = kmalloc(sizeof(*n526_lpc), GFP_KERNEL);

	if (!n526_lpc) {
		dev_err(&client->dev, "Failed to allocate device structure\n");
		return -ENOMEM;
	}

	input = input_allocate_device();
	if (!input) {
		dev_err(&client->dev, "Failed to allocate input device\n");
		ret = -ENOMEM;
		goto err_free;
	}

	input->name = "n526-keys";
	input->phys = "n526-keys/input0";
	input->dev.parent = &client->dev;
	input->id.bustype = BUS_I2C;
	input->id.vendor  = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0001;

	__set_bit(EV_KEY, input->evbit);

	for (i = 0; i < ARRAY_SIZE(n526_lpc_keymap); ++i) {
		if (n526_lpc_keymap[i] != 0)
			__set_bit(n526_lpc_keymap[i], input->keybit);
	}

	ret = input_register_device(input);

	if (ret) {
		dev_err(&client->dev, "Failed to register input device: %d\n", ret);
		goto err_free_input;
	}

	n526_lpc->client = client;
	n526_lpc->input  = input;
	INIT_WORK(&n526_lpc->work, n526_lpc_irq_work);

	ret = request_irq(client->irq, n526_lpc_irq, IRQF_TRIGGER_FALLING,
	                  "n526-lpc", n526_lpc);
	if (ret) {
		dev_err(&client->dev, "Failed to request irq: %d\n", ret);
		goto err_unregister_input;
	}

	i2c_set_clientdata(client, n526_lpc);

	return 0;

err_unregister_input:
	input_unregister_device(input);
err_free_input:
	input_free_device(input);
err_free:
	kfree(n526_lpc);

	return ret;
}

static int n526_lpc_remove(struct i2c_client *client)
{
	struct n526_lpc *n526_lpc = i2c_get_clientdata(client);

	free_irq(client->irq, n526_lpc);

	i2c_set_clientdata(client, NULL);
   	input_unregister_device(n526_lpc->input);
	input_free_device(n526_lpc->input);
	kfree(n526_lpc);

 return 0;
}

static const struct i2c_device_id n526_lpc_id[] = {
	{ "n526-lpc", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, n526_lpc_id);

static struct i2c_driver n526_lpc_driver = {
	.driver = {
		.name = "n526-lpc",
		.owner = THIS_MODULE,
	},
	.probe = n526_lpc_probe,
	.remove = n526_lpc_remove,
	.id_table = n526_lpc_id,
};

static int __init n526_lpc_init(void)
{
	return i2c_add_driver(&n526_lpc_driver);
}
module_init(n526_lpc_init);

static void __exit n526_lpc_exit(void)
{
	i2c_del_driver(&n526_lpc_driver);
}
module_exit(n526_lpc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars-Peter Clausen");
MODULE_DESCRIPTION("n526 keypad driver");
MODULE_ALIAS("i2c:n526-keys");
