// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Sercomm MSP430G2513 LEDs.
 *
 * Copyright 2023 Álvaro Fernández Rojas <noltari@gmail.com>
 */

#include <linux/delay.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/spi/spi.h>
#include "leds.h"

/*
 * MSP430G2513 SPI protocol description:
 * +----+----+----+----+----+----+
 * | b1 | b2 | b3 | b4 | b5 | b6 |
 * +----+----+----+----+----+----+
 * 6 bytes TX & RX per transaction.
 *
 * LEDs:
 * MSP430G2513 can control up to 9 LEDs.
 * b1: LED ID [1,9]
 * b2: LED function
 * b3-b6: LED function parameters
 *
 * LED functions:
 * [0] Off
 * [1] On
 * [2] Flash
 *   - b4: delay (x 6ms)
 *   - b5: repeat (0 = infinite)
 * [3] Pulse
 *   - b3: delay (x 6ms)
 *   - b4: blink while pulsing? (unknown)
 *   - b5: repeat (0 = infinite)
 * [4] Pulse On
 *   - b3: delay (x 6ms)
 *   - b4: blink while pulsing? (unknown)
 *   - b5: repeat (0 = infinite)
 * [5] Pulse Off
 *   - b3: delay (x 6ms)
 *   - b4: blink while pulsing? (unknown)
 *   - b5: repeat (0 = infinite)
 * [6] Level
 *   - b3: brightness [0,4]
 *
 * MCU Commands (b1 = 0x55):
 * [0x0a] FW upgrade data
 *   - b3: Data size (usually 0x40), which is appended to TX & RX.
 * [0x31] Get MCU version? (unknown)
 * [0x68] Get MCU work mode
 * [0xa5] Start FW upgrade
 * [0xf0] End FW upgrade
 */

#define MSP430_CMD_BYTES		6
#define MSP430_CMD_MCU			0x55
#define MSP430_MCU_WM			0x68

#define MSP430_LED_MIN_ID		1
#define MSP430_LED_MAX_ID		9

#define MSP430_LED_OFF			0
#define MSP430_LED_ON			1
#define MSP430_LED_FLASH		2
#define MSP430_LED_PULSE		3
#define MSP430_LED_PULSE_ON		4
#define MSP430_LED_PULSE_OFF		5
#define MSP430_LED_LEVEL		6

#define MSP430_LED_BLINK_DEF		500
#define MSP430_LED_BLINK_MASK		0xff
#define MSP430_LED_BLINK_MS		6
#define MSP430_LED_BLINK_MAX		(MSP430_LED_BLINK_MS * \
					 MSP430_LED_BLINK_MASK)

#define MSP430_LED_BRIGHTNESS_MAX	5
#define MSP430_LED_REPEAT_MAX		0xff

/**
 * struct msp430_led - state container for Sercomm MSP430 based LEDs
 * @cdev: LED class device for this LED
 * @spi: spi resource
 * @id: LED ID
 */
struct msp430_led {
	struct led_classdev cdev;
	struct spi_device *spi;
	u8 id;
};

static inline int msp430_cmd(struct spi_device *spi, u8 tx[MSP430_CMD_BYTES],
			     u8 rx[MSP430_CMD_BYTES])
{
	struct device *dev = &spi->dev;
	int rc;

	memset(rx, 0, MSP430_CMD_BYTES);

	rc = spi_write_then_read(spi, tx, MSP430_CMD_BYTES,
				  rx, MSP430_CMD_BYTES);
	if (rc)
		dev_err(dev, "spi error\n");

	dev_dbg(dev, "msp430_cmd: [%02x %02x %02x %02x %02x %02x]"
		" -> [%02x %02x %02x %02x %02x %02x]",
		tx[0], tx[1], tx[2], tx[3], tx[4], tx[5],
		rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);

	return rc;
}

static unsigned long msp430_blink_delay(unsigned long delay)
{
	unsigned long msp430_delay;

	msp430_delay = delay + MSP430_LED_BLINK_MS / 2;
	msp430_delay = msp430_delay / MSP430_LED_BLINK_MS;
	if (msp430_delay == 0)
		msp430_delay = 1;

	return msp430_delay;
}

static int msp430_blink_set(struct led_classdev *led_cdev,
			    unsigned long *delay_on,
			    unsigned long *delay_off)
{
	struct msp430_led *led =
		container_of(led_cdev, struct msp430_led, cdev);
	u8 tx[MSP430_CMD_BYTES] = {led->id, MSP430_LED_FLASH, 0, 0, 0, 0};
	u8 rx[MSP430_CMD_BYTES];
	unsigned long delay;

	if (!*delay_on)
		*delay_on = MSP430_LED_BLINK_DEF;
	if (!*delay_off)
		*delay_off = MSP430_LED_BLINK_DEF;

	delay = msp430_blink_delay(*delay_on);
	if (delay != msp430_blink_delay(*delay_off)) {
		dev_dbg(led_cdev->dev,
			"fallback to soft blinking (delay_on != delay_off)\n");
		return -EINVAL;
	}

	if (delay > MSP430_LED_BLINK_MASK) {
		dev_dbg(led_cdev->dev,
			"fallback to soft blinking (delay > %ums)\n",
			MSP430_LED_BLINK_MAX);
		return -EINVAL;
	}

	tx[3] = delay;

	return msp430_cmd(led->spi, tx, rx);
}

static int msp430_brightness_set(struct led_classdev *led_cdev,
				 enum led_brightness brightness)
{
	struct msp430_led *led =
		container_of(led_cdev, struct msp430_led, cdev);
	u8 tx[MSP430_CMD_BYTES] = {led->id, 0, 0, 0, 0, 0};
	u8 rx[MSP430_CMD_BYTES];
	u8 val = (u8) brightness;

	switch (val)
	{
	case LED_OFF:
		tx[1] = MSP430_LED_OFF;
		break;
	case MSP430_LED_BRIGHTNESS_MAX:
		tx[1] = MSP430_LED_ON;
		break;
	default:
		tx[1] = MSP430_LED_LEVEL;
		tx[2] = val - 1;
		break;
	}

	return msp430_cmd(led->spi, tx, rx);
}

static int msp430_pattern_clear(struct led_classdev *ldev)
{
	msp430_brightness_set(ldev, LED_OFF);

	return 0;
}

static int msp430_pattern_set(struct led_classdev *led_cdev,
			      struct led_pattern *pattern,
			      u32 len, int repeat)
{
	struct msp430_led *led =
		container_of(led_cdev, struct msp430_led, cdev);
	u8 tx[MSP430_CMD_BYTES] = {led->id, 0, 0, 0, 0, 0};
	u8 rx[MSP430_CMD_BYTES];
	unsigned long delay0;
	unsigned long delay1;
	int rc;

	if (len != 2 ||
	    repeat > MSP430_LED_REPEAT_MAX ||
	    pattern[0].delta_t > MSP430_LED_BLINK_MAX ||
	    pattern[1].delta_t > MSP430_LED_BLINK_MAX)
		return -EINVAL;

	delay0 = msp430_blink_delay(pattern[0].delta_t);
	delay1 = msp430_blink_delay(pattern[1].delta_t);

	/* Infinite pattern */
	if (repeat < 0)
		repeat = 0;

	/* Pulse: <off> <delay> <max> <delay> */
	if (delay0 == delay1 &&
	    pattern[0].brightness == LED_OFF &&
	    pattern[1].brightness == MSP430_LED_BRIGHTNESS_MAX)
	{
		tx[1] = MSP430_LED_PULSE;
		tx[2] = delay0;
		tx[4] = (u8) repeat;
	}

	/* Pulse On: <off> <delay> <max> <0ms> */
	if (pattern[0].delta_t != 0 &&
	    pattern[1].delta_t == 0 &&
	    pattern[0].brightness == LED_OFF &&
	    pattern[1].brightness == MSP430_LED_BRIGHTNESS_MAX) {
		tx[1] = MSP430_LED_PULSE_ON;
		tx[2] = delay0;
		tx[4] = (u8) repeat;
	}

	/* Pulse Off: <max> <delay> <off> <0ms> */
	if (pattern[0].delta_t != 0 &&
	    pattern[1].delta_t == 0 &&
	    pattern[0].brightness == MSP430_LED_BRIGHTNESS_MAX &&
	    pattern[1].brightness == LED_OFF) {
		tx[1] = MSP430_LED_PULSE_OFF;
		tx[2] = delay0;
		tx[4] = (u8) repeat;
	}

	if (!tx[1])
		return -EINVAL;

	rc = msp430_cmd(led->spi, tx, rx);
	if (rc)
		return rc;

	return 0;
}

static int msp430_led(struct spi_device *spi, struct device_node *nc, u8 id)
{
	struct device *dev = &spi->dev;
	struct led_init_data init_data = {};
	struct msp430_led *led;
	enum led_default_state state;
	int rc;

	led = devm_kzalloc(dev, sizeof(*led), GFP_KERNEL);
	if (!led)
		return -ENOMEM;

	led->id = id;
	led->spi = spi;

	init_data.fwnode = of_fwnode_handle(nc);

	state = led_init_default_state_get(init_data.fwnode);
	switch (state) {
	case LEDS_DEFSTATE_ON:
		led->cdev.brightness = MSP430_LED_BRIGHTNESS_MAX;
		break;
	default:
		led->cdev.brightness = LED_OFF;
		break;
	}

	msp430_brightness_set(&led->cdev, led->cdev.brightness);

	led->cdev.blink_set = msp430_blink_set;
	led->cdev.brightness_set_blocking = msp430_brightness_set;
	led->cdev.max_brightness = MSP430_LED_BRIGHTNESS_MAX;
	led->cdev.pattern_clear = msp430_pattern_clear;
	led->cdev.pattern_set = msp430_pattern_set;

	rc = devm_led_classdev_register_ext(dev, &led->cdev, &init_data);
	if (rc < 0)
		return rc;

	dev_dbg(dev, "registered LED %s\n", led->cdev.name);

	return 0;
}

static inline int msp430_check_workmode(struct spi_device *spi)
{
	struct device *dev = &spi->dev;
	u8 tx[MSP430_CMD_BYTES] = {MSP430_CMD_MCU, MSP430_MCU_WM, 0, 0, 0, 0};
	u8 rx[MSP430_CMD_BYTES];
	int rc;

	rc = msp430_cmd(spi, tx, rx);
	if (rc)
		return rc;

	if ((rx[3] == 0xA5 && rx[4] == 'Z') ||
	    (rx[4] == 0xA5 && rx[5] == 'Z') ||
	    (rx[4] == '\b' && rx[5] == '\n')) {
		dev_err(dev, "invalid workmode: "
			"[%02x %02x %02x %02x %02x %02x]\n",
			rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);
		return -EINVAL;
	}

	return 0;
}

static int msp430_leds_probe(struct spi_device *spi)
{
	struct device *dev = &spi->dev;
	struct device_node *np = dev_of_node(dev);
	struct device_node *child;
	int rc;

	rc = msp430_check_workmode(spi);
	if (rc)
		return rc;

	for_each_available_child_of_node(np, child) {
		u32 reg;

		if (of_property_read_u32(child, "reg", &reg))
			continue;

		if (reg < MSP430_LED_MIN_ID || reg > MSP430_LED_MAX_ID) {
			dev_err(dev, "invalid LED (%u) [%d, %d]\n", reg,
				MSP430_LED_MIN_ID, MSP430_LED_MAX_ID);
			continue;
		}

		rc = msp430_led(spi, child, reg);
		if (rc < 0) {
			of_node_put(child);
			return rc;
		}
	}

	return 0;
}

static const struct of_device_id msp430_leds_of_match[] = {
	{ .compatible = "sercomm,msp430-leds", },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, msp430_leds_of_match);

static const struct spi_device_id msp430_leds_id_table[] = {
	{ "msp430-leds", 0 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(spi, msp430_leds_id_table);

static struct spi_driver msp430_leds_driver = {
	.driver = {
		.name = KBUILD_MODNAME,
		.of_match_table = msp430_leds_of_match,
	},
	.id_table = msp430_leds_id_table,
	.probe = msp430_leds_probe,
};
module_spi_driver(msp430_leds_driver);

MODULE_AUTHOR("Álvaro Fernández Rojas <noltari@gmail.com>");
MODULE_DESCRIPTION("LED driver for Sercomm MSP430 controllers");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:leds-sercomm-msp430");
