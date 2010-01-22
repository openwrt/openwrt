/*
 * Realtek RTL8366 SMI interface driver
 *
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/spinlock.h>

#include "rtl8366_smi.h"

#define RTL8366_SMI_ACK_RETRY_COUNT         5
#define RTL8366_SMI_CLK_DELAY               10 /* nsec */

static inline void rtl8366_smi_clk_delay(struct rtl8366_smi *smi)
{
	ndelay(RTL8366_SMI_CLK_DELAY);
}

static void rtl8366_smi_start(struct rtl8366_smi *smi)
{
	unsigned int sda = smi->gpio_sda;
	unsigned int sck = smi->gpio_sck;

	/*
	 * Set GPIO pins to output mode, with initial state:
	 * SCK = 0, SDA = 1
	 */
	gpio_direction_output(sck, 0);
	gpio_direction_output(sda, 1);
	rtl8366_smi_clk_delay(smi);

	/* CLK 1: 0 -> 1, 1 -> 0 */
	gpio_set_value(sck, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8366_smi_clk_delay(smi);

	/* CLK 2: */
	gpio_set_value(sck, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sda, 0);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sda, 1);
}

static void rtl8366_smi_stop(struct rtl8366_smi *smi)
{
	unsigned int sda = smi->gpio_sda;
	unsigned int sck = smi->gpio_sck;

	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sda, 0);
	gpio_set_value(sck, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sda, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 1);

	/* add a click */
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 1);

	/* set GPIO pins to input mode */
	gpio_direction_input(sda);
	gpio_direction_input(sck);
}

static void rtl8366_smi_write_bits(struct rtl8366_smi *smi, u32 data, u32 len)
{
	unsigned int sda = smi->gpio_sda;
	unsigned int sck = smi->gpio_sck;

	for (; len > 0; len--) {
		rtl8366_smi_clk_delay(smi);

		/* prepare data */
		if ( data & ( 1 << (len - 1)) )
			gpio_set_value(sda, 1);
		else
			gpio_set_value(sda, 0);
		rtl8366_smi_clk_delay(smi);

		/* clocking */
		gpio_set_value(sck, 1);
		rtl8366_smi_clk_delay(smi);
		gpio_set_value(sck, 0);
	}
}

static void rtl8366_smi_read_bits(struct rtl8366_smi *smi, u32 len, u32 *data)
{
	unsigned int sda = smi->gpio_sda;
	unsigned int sck = smi->gpio_sck;

	gpio_direction_input(sda);

	for (*data = 0; len > 0; len--) {
		u32 u;

		rtl8366_smi_clk_delay(smi);

		/* clocking */
		gpio_set_value(sck, 1);
		rtl8366_smi_clk_delay(smi);
		u = gpio_get_value(sda);
		gpio_set_value(sck, 0);

		*data |= (u << (len - 1));
	}

	gpio_direction_output(sda, 0);
}

static int rtl8366_smi_wait_for_ack(struct rtl8366_smi *smi)
{
	int retry_cnt;

	retry_cnt = 0;
	do {
		u32 ack;

		rtl8366_smi_read_bits(smi, 1, &ack);
		if (ack == 0)
			break;

		if (++retry_cnt > RTL8366_SMI_ACK_RETRY_COUNT)
			return -EIO;
	} while (1);

	return 0;
}

static int rtl8366_smi_write_byte(struct rtl8366_smi *smi, u8 data)
{
	rtl8366_smi_write_bits(smi, data, 8);
	return rtl8366_smi_wait_for_ack(smi);
}

static int rtl8366_smi_read_byte0(struct rtl8366_smi *smi, u8 *data)
{
	u32 t;

	/* read data */
	rtl8366_smi_read_bits(smi, 8, &t);
	*data = (t & 0xff);

	/* send an ACK */
	rtl8366_smi_write_bits(smi, 0x00, 1);

	return 0;
}

static int rtl8366_smi_read_byte1(struct rtl8366_smi *smi, u8 *data)
{
	u32 t;

	/* read data */
	rtl8366_smi_read_bits(smi, 8, &t);
	*data = (t & 0xff);

	/* send an ACK */
	rtl8366_smi_write_bits(smi, 0x01, 1);

	return 0;
}

int rtl8366_smi_read_reg(struct rtl8366_smi *smi, u32 addr, u32 *data)
{
	unsigned long flags;
	u8 lo = 0;
	u8 hi = 0;
	int ret;

	spin_lock_irqsave(&smi->lock, flags);

	rtl8366_smi_start(smi);

	/* send READ command */
	ret = rtl8366_smi_write_byte(smi, 0x0a << 4 | 0x04 << 1 | 0x01);
	if (ret)
		goto out;

	/* set ADDR[7:0] */
	ret = rtl8366_smi_write_byte(smi, addr & 0xff);
	if (ret)
		goto out;

	/* set ADDR[15:8] */
	ret = rtl8366_smi_write_byte(smi, addr >> 8);
	if (ret)
		goto out;

	/* read DATA[7:0] */
	rtl8366_smi_read_byte0(smi, &lo);
	/* read DATA[15:8] */
	rtl8366_smi_read_byte1(smi, &hi);

	*data = ((u32) lo) | (((u32) hi) << 8);

	ret = 0;

 out:
	rtl8366_smi_stop(smi);
	spin_unlock_irqrestore(&smi->lock, flags);

	return ret;
}
EXPORT_SYMBOL_GPL(rtl8366_smi_read_reg);

int rtl8366_smi_write_reg(struct rtl8366_smi *smi, u32 addr, u32 data)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&smi->lock, flags);

	rtl8366_smi_start(smi);

	/* send WRITE command */
	ret = rtl8366_smi_write_byte(smi, 0x0a << 4 | 0x04 << 1 | 0x00);
	if (ret)
		goto out;

	/* set ADDR[7:0] */
	ret = rtl8366_smi_write_byte(smi, addr & 0xff);
	if (ret)
		goto out;

	/* set ADDR[15:8] */
	ret = rtl8366_smi_write_byte(smi, addr >> 8);
	if (ret)
		goto out;

	/* write DATA[7:0] */
	ret = rtl8366_smi_write_byte(smi, data & 0xff);
	if (ret)
		goto out;

	/* write DATA[15:8] */
	ret = rtl8366_smi_write_byte(smi, data >> 8);
	if (ret)
		goto out;

	ret = 0;

 out:
	rtl8366_smi_stop(smi);
	spin_unlock_irqrestore(&smi->lock, flags);

	return ret;
}
EXPORT_SYMBOL_GPL(rtl8366_smi_write_reg);

int rtl8366_smi_init(struct rtl8366_smi *smi)
{
	int err;

	if (!smi->parent)
		return -EINVAL;

	err = gpio_request(smi->gpio_sda, dev_name(smi->parent));
	if (err) {
		dev_err(smi->parent, "gpio_request failed for %u, err=%d\n",
			smi->gpio_sda, err);
		goto err_out;
	}

	err = gpio_request(smi->gpio_sck, dev_name(smi->parent));
	if (err) {
		dev_err(smi->parent, "gpio_request failed for %u, err=%d\n",
			smi->gpio_sck, err);
		goto err_free_sda;
	}

	spin_lock_init(&smi->lock);

	dev_info(smi->parent, "using GPIO pins %u (SDA) and %u (SCK)\n",
		 smi->gpio_sda, smi->gpio_sck);

	return 0;

 err_free_sda:
	gpio_free(smi->gpio_sda);
 err_out:
	return err;
}
EXPORT_SYMBOL_GPL(rtl8366_smi_init);

void rtl8366_smi_cleanup(struct rtl8366_smi *smi)
{
	gpio_free(smi->gpio_sck);
	gpio_free(smi->gpio_sda);
}
EXPORT_SYMBOL_GPL(rtl8366_smi_cleanup);

MODULE_DESCRIPTION("Realtek RTL8366 SMI interface driver");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");
