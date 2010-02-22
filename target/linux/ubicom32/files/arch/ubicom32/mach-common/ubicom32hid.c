/*
 * arch/ubicom32/mach-common/ubicom32hid.c
 *   I2C driver for HID coprocessor found on some DPF implementations.
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/backlight.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/input-polldev.h>

#include <asm/ubicom32hid.h>

#define DRIVER_NAME "ubicom32hid"

#ifdef DEBUG
static int ubicom32hid_debug;
#endif

static const struct i2c_device_id ubicom32hid_id[] = {
	{ DRIVER_NAME, },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ubicom32hid_id);

/*
 * Define this to make IR checking strict, in general, it's not needed
 */
#undef UBICOM32HID_STRICT_IR_CHECK

#define UBICOM32HID_CMD_SET_PWM		0x01
#define UBICOM32HID_CMD_SET_BL_EN	0x02
#define UBICOM32HID_BL_EN_LOW		0x00
#define UBICOM32HID_BL_EN_HIZ		0x01
#define UBICOM32HID_BL_EN_HI		0x02
#define UBICOM32HID_CMD_FLUSH		0x99
#define UBICOM32HID_CMD_RESET		0x99
#define UBICOM32HID_CMD_GET_IR_SWITCH	0xC0
#define UBICOM32HID_CMD_GET_REVISION	0xfd
#define UBICOM32HID_CMD_GET_DEVICE_ID	0xfe
#define UBICOM32HID_CMD_GET_VERSION	0xff
#define UBICOM32HID_DEVICE_ID		0x49

#define UBICOM32HID_MAX_BRIGHTNESS_PWM	255

/*
 * Data structure returned by the HID device
 */
struct ubicom32hid_input_data {
	uint32_t	ircmd;
	uint8_t		sw_state;
	uint8_t		sw_changed;
};

/*
 * Our private data
 */
struct ubicom32hid_data {
	/*
	 * Pointer to the platform data structure, we need the settings.
	 */
	const struct ubicom32hid_platform_data	*pdata;

	/*
	 * Backlight device
	 */
	struct backlight_device			*bldev;

	/*
	 * I2C client, for sending messages to the HID device
	 */
	struct i2c_client			*client;

	/*
	 * Current intensity, used for get_intensity.
	 */
	int					cur_intensity;

	/*
	 * Input subsystem
	 *	We won't register an input subsystem if there are no mappings.
	 */
	struct input_polled_dev			*poll_dev;
};


/*
 * ubicom32hid_set_intensity
 */
static int ubicom32hid_set_intensity(struct backlight_device *bd)
{
	struct ubicom32hid_data *ud =
		(struct ubicom32hid_data *)bl_get_data(bd);
	int intensity = bd->props.brightness;
	int reg;
	u8_t val;
	int ret;

	/*
	 * If we're blanked the the intensity doesn't matter.
	 */
	if ((bd->props.power != FB_BLANK_UNBLANK) ||
	    (bd->props.fb_blank != FB_BLANK_UNBLANK)) {
		intensity = 0;
	}

	/*
	 * Set the brightness based on the type of backlight
	 */
	if (ud->pdata->type == UBICOM32HID_BL_TYPE_BINARY) {
		reg = UBICOM32HID_CMD_SET_BL_EN;
		if (intensity) {
			val = ud->pdata->invert
				? UBICOM32HID_BL_EN_LOW : UBICOM32HID_BL_EN_HI;
		} else {
			val = ud->pdata->invert
				? UBICOM32HID_BL_EN_HI : UBICOM32HID_BL_EN_LOW;
		}
	} else {
		reg = UBICOM32HID_CMD_SET_PWM;
		val = ud->pdata->invert
			? (UBICOM32HID_MAX_BRIGHTNESS_PWM - intensity) :
			intensity;
	}

	/*
	 * Send the command
	 */
	ret = i2c_smbus_write_byte_data(ud->client, reg, val);
	if (ret < 0) {
		dev_warn(&ud->client->dev, "Unable to write backlight err=%d\n",
			 ret);
		return ret;
	}

	ud->cur_intensity = intensity;

	return 0;
}

/*
 * ubicom32hid_get_intensity
 *	Return the current intensity of the backlight.
 */
static int ubicom32hid_get_intensity(struct backlight_device *bd)
{
	struct ubicom32hid_data *ud =
		(struct ubicom32hid_data *)bl_get_data(bd);

	return ud->cur_intensity;
}

/*
 * ubicom32hid_verify_data
 *	Verify the data to see if there is any action to be taken
 *
 * Returns 0 if no action is to be taken, non-zero otherwise
 */
static int ubicom32hid_verify_data(struct ubicom32hid_data *ud,
				   struct ubicom32hid_input_data *data)
{
	uint8_t *ircmd = (uint8_t *)&(data->ircmd);

	/*
	 * ircmd == DEADBEEF means ir queue is empty.  Since this is a
	 * meaningful code, that means the rest of the message is most likely
	 * correct, so only process the data if the switch state has changed.
	 */
	if (data->ircmd == 0xDEADBEEF) {
		return data->sw_changed != 0;
	}

	/*
	 * We have an ircmd which is not empty:
	 *      Data[1] should be the complement of Data[0]
	 */
	if (ircmd[0] != (u8_t)~ircmd[1]) {
		return 0;
	}

#ifdef UBICOM32HID_STRICT_IR_CHECK
	/*
	 * It seems that some remote controls don't follow the NEC protocol
	 * properly, so only do this check if the remote does indeed follow the
	 * spec.  Data[3] should be the complement of Data[2]
	 */
	if (ircmd[2] == (u8_t)~ircmd[3]) {
		return 1;
	}

	/*
	 * For non-compliant remotes, check the system code according to what
	 * they send.
	 */
	if ((ircmd[2] != UBICOM32HID_IR_SYSTEM_CODE_CHECK) ||
	    (ircmd[3] != UBICOM32HID_IR_SYSTEM_CODE)) {
		return 0;
	}
#endif

	/*
	 * Data checks out, process
	 */
	return 1;
}

/*
 * ubicom32hid_poll_input
 *	Poll the input from the HID device.
 */
static void ubicom32hid_poll_input(struct input_polled_dev *dev)
{
	struct ubicom32hid_data *ud = (struct ubicom32hid_data *)dev->private;
	const struct ubicom32hid_platform_data *pdata = ud->pdata;
	struct ubicom32hid_input_data data;
	struct input_dev *id = dev->input;
	int i;
	int sync_needed = 0;
	uint8_t cmd;
	int ret;

	/*
	 * Flush the queue
	 */
	cmd = UBICOM32HID_CMD_FLUSH;
	ret = i2c_master_send(ud->client, &cmd, 1);
	if (ret < 0) {
		return;
	}

	ret = i2c_smbus_read_i2c_block_data(
		ud->client, UBICOM32HID_CMD_GET_IR_SWITCH, 6, (void *)&data);
	if (ret < 0) {
		return;
	}

	/*
	 * Verify the data to see if there is any action to be taken
	 */
	if (!ubicom32hid_verify_data(ud, &data)) {
		return;
	}

#ifdef DEBUG
	if (ubicom32hid_debug) {
		printk("Polled ircmd=%8x swstate=%2x swchanged=%2x\n",
		       data.ircmd, data.sw_state, data.sw_changed);
	}
#endif

	/*
	 * Process changed switches
	 */
	if (data.sw_changed) {
		const struct ubicom32hid_button *ub = pdata->buttons;
		for (i = 0; i < pdata->nbuttons; i++, ub++) {
			uint8_t mask = (1 << ub->bit);
			if (!(data.sw_changed & mask)) {
				continue;
			}

			sync_needed = 1;
			input_event(id, ub->type, ub->code,
				    (data.sw_state & mask) ? 1 : 0);
		}
	}
	if (sync_needed) {
		input_sync(id);
	}

	/*
	 * Process ir codes
	 */
	if (data.ircmd != 0xDEADBEEF) {
		const struct ubicom32hid_ir *ui = pdata->ircodes;
		for (i = 0; i < pdata->nircodes; i++, ui++) {
			if (ui->ir_code == data.ircmd) {
				/*
				 * Simulate a up/down event
				 */
				input_event(id, ui->type, ui->code, 1);
				input_sync(id);
				input_event(id, ui->type, ui->code, 0);
				input_sync(id);
			}
		}
	}
}


/*
 * Backlight ops
 */
static struct backlight_ops ubicom32hid_blops = {
	.get_brightness = ubicom32hid_get_intensity,
	.update_status  = ubicom32hid_set_intensity,
};

/*
 * ubicom32hid_probe
 */
static int ubicom32hid_probe(struct i2c_client *client,
			     const struct i2c_device_id *id)
{
	struct ubicom32hid_platform_data *pdata;
	struct ubicom32hid_data *ud;
	int ret;
	int i;
	u8 version[2];
	char buf[1];

	pdata = client->dev.platform_data;
	if (pdata == NULL) {
		return -ENODEV;
	}

	/*
	 * See if we even have a device available before allocating memory.
	 *
	 * Hard reset the device
	 */
	ret = gpio_request(pdata->gpio_reset, "ubicom32hid-reset");
	if (ret < 0) {
		return ret;
	}
	gpio_direction_output(pdata->gpio_reset, pdata->gpio_reset_polarity);
	udelay(100);
	gpio_set_value(pdata->gpio_reset, !pdata->gpio_reset_polarity);
	udelay(100);

	/*
	 * soft reset the device.  It sometimes takes a while to do this.
	 */
	for (i = 0; i < 50; i++) {
		buf[0] = UBICOM32HID_CMD_RESET;
		ret = i2c_master_send(client, buf, 1);
		if (ret > 0) {
			break;
		}
		udelay(10000);
	}
	if (i == 50) {
		dev_warn(&client->dev, "Unable to reset device\n");
		goto fail;
	}

	ret = i2c_smbus_read_byte_data(client, UBICOM32HID_CMD_GET_DEVICE_ID);
	if (ret != UBICOM32HID_DEVICE_ID) {
		dev_warn(&client->dev, "Incorrect device id %02x\n", buf[0]);
		ret = -ENODEV;
		goto fail;
	}

	ret = i2c_smbus_read_byte_data(client, UBICOM32HID_CMD_GET_VERSION);
	if (ret < 0) {
		dev_warn(&client->dev, "Unable to get version\n");
		goto fail;
	}
	version[0] = ret;

	ret = i2c_smbus_read_byte_data(client, UBICOM32HID_CMD_GET_REVISION);
	if (ret < 0) {
		dev_warn(&client->dev, "Unable to get revision\n");
		goto fail;
	}
	version[1] = ret;

	/*
	 * Allocate our private data
	 */
	ud = kzalloc(sizeof(struct ubicom32hid_data), GFP_KERNEL);
	if (!ud) {
		ret = -ENOMEM;
		goto fail;
	}
	ud->pdata = pdata;
	ud->client = client;

	/*
	 * Register our backlight device
	 */
	ud->bldev = backlight_device_register(DRIVER_NAME, &client->dev,
					      ud, &ubicom32hid_blops);
	if (IS_ERR(ud->bldev)) {
		ret = PTR_ERR(ud->bldev);
		goto fail2;
	}
	platform_set_drvdata(client, ud);

	/*
	 * Start up the backlight with the requested intensity
	 */
	ud->bldev->props.power = FB_BLANK_UNBLANK;
	ud->bldev->props.max_brightness =
		(pdata->type == UBICOM32HID_BL_TYPE_PWM) ?
		UBICOM32HID_MAX_BRIGHTNESS_PWM : 1;
	if (pdata->default_intensity < ud->bldev->props.max_brightness) {
		ud->bldev->props.brightness = pdata->default_intensity;
	} else {
		dev_warn(&client->dev, "Default brightness out of range, "
			 "setting to max\n");
		ud->bldev->props.brightness = ud->bldev->props.max_brightness;
	}

	ubicom32hid_set_intensity(ud->bldev);

	/*
	 * Check to see if we have any inputs
	 */
	if (!pdata->nbuttons && !pdata->nircodes) {
		goto done;
	}

	/*
	 * We have buttons or codes, we must register an input device
	 */
	ud->poll_dev = input_allocate_polled_device();
	if (!ud->poll_dev) {
		ret = -ENOMEM;
		goto fail3;
	}

	/*
	 * Setup the polling to default to 100ms
	 */
	ud->poll_dev->poll = ubicom32hid_poll_input;
	ud->poll_dev->poll_interval =
		pdata->poll_interval ? pdata->poll_interval : 100;
	ud->poll_dev->private = ud;

	ud->poll_dev->input->name =
		pdata->input_name ? pdata->input_name : "Ubicom32HID";
	ud->poll_dev->input->phys = "ubicom32hid/input0";
	ud->poll_dev->input->dev.parent = &client->dev;
	ud->poll_dev->input->id.bustype = BUS_I2C;

	/*
	 * Set the capabilities by running through the buttons and ir codes
	 */
	for (i = 0; i < pdata->nbuttons; i++) {
		const struct ubicom32hid_button *ub = &pdata->buttons[i];

		input_set_capability(ud->poll_dev->input,
				     ub->type ? ub->type : EV_KEY, ub->code);
	}

	for (i = 0; i < pdata->nircodes; i++) {
		const struct ubicom32hid_ir *ui = &pdata->ircodes[i];

		input_set_capability(ud->poll_dev->input,
				     ui->type ? ui->type : EV_KEY, ui->code);
	}

	ret = input_register_polled_device(ud->poll_dev);
	if (ret) {
		goto fail3;
	}

done:
	printk(KERN_INFO DRIVER_NAME ": enabled, version=%02x.%02x\n",
	       version[0], version[1]);

	return 0;

fail3:
	gpio_free(ud->pdata->gpio_reset);
	backlight_device_unregister(ud->bldev);
fail2:
	kfree(ud);
fail:
	gpio_free(pdata->gpio_reset);
	return ret;
}

/*
 * ubicom32hid_remove
 */
static int ubicom32hid_remove(struct i2c_client *client)
{
	struct ubicom32hid_data *ud =
		(struct ubicom32hid_data *)platform_get_drvdata(client);

	gpio_free(ud->pdata->gpio_reset);

	backlight_device_unregister(ud->bldev);

	if (ud->poll_dev) {
		input_unregister_polled_device(ud->poll_dev);
		input_free_polled_device(ud->poll_dev);
	}

	platform_set_drvdata(client, NULL);

	kfree(ud);

	return 0;
}

static struct i2c_driver ubicom32hid_driver = {
	.driver = {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= ubicom32hid_probe,
	.remove		= __exit_p(ubicom32hid_remove),
	.id_table	= ubicom32hid_id,
};

/*
 * ubicom32hid_init
 */
static int __init ubicom32hid_init(void)
{
	return i2c_add_driver(&ubicom32hid_driver);
}
module_init(ubicom32hid_init);

/*
 * ubicom32hid_exit
 */
static void __exit ubicom32hid_exit(void)
{
	i2c_del_driver(&ubicom32hid_driver);
}
module_exit(ubicom32hid_exit);

MODULE_AUTHOR("Pat Tjin <@ubicom.com>")
MODULE_DESCRIPTION("Ubicom HID driver");
MODULE_LICENSE("GPL");
