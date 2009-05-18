/*
 * HDQ generic GPIO bitbang driver using FIQ
 *
 * (C) 2006-2007 by Openmoko, Inc.
 * Author: Andy Green <andy@openmoko.com>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/hdq.h>

#define HDQ_READ 0
#define HDQ_WRITE 0x80

enum hdq_bitbang_states {
	HDQB_IDLE = 0,
	HDQB_TX_BREAK,
	HDQB_TX_BREAK_RECOVERY,
	HDQB_ADS_CALC,
	HDQB_ADS_LOW,
	HDQB_ADS_HIGH,
	HDQB_WAIT_RX,
	HDQB_DATA_RX_LOW,
	HDQB_DATA_RX_HIGH,
	HDQB_WAIT_TX,
};

static struct hdq_priv {
	u8 hdq_probed; /* nonzero after HDQ driver probed */
	struct mutex hdq_lock; /* if you want to use hdq, you have to take lock */
	unsigned long hdq_gpio_pin; /* GTA02 = GPD14 which pin to meddle with */
	u8 hdq_ads; /* b7..b6 = register address, b0 = r/w */
	u8 hdq_tx_data; /* data to tx for write action */
	u8 hdq_rx_data; /* data received in read action */
	u8 hdq_request_ctr; /* incremented by "user" to request a transfer */
	u8 hdq_transaction_ctr; /* incremented after each transfer */
	u8 hdq_error; /* 0 = no error */
	u8 hdq_ctr;
	u8 hdq_ctr2;
	u8 hdq_bit;
	u8 hdq_shifter;
	u8 hdq_tx_data_done;
	enum hdq_bitbang_states hdq_state;
	int reported_error;

	struct hdq_platform_data *pdata;
} hdq_priv;


static void hdq_bad(void)
{
	if (!hdq_priv.reported_error)
		printk(KERN_ERR "HDQ error: %d\n", hdq_priv.hdq_error);
	hdq_priv.reported_error = 1;
}

static void hdq_good(void)
{
	if (hdq_priv.reported_error)
		printk(KERN_INFO "HDQ responds again\n");
	hdq_priv.reported_error = 0;
}

int hdq_fiq_handler(void)
{
	if (!hdq_priv.hdq_probed)
		return 0;

	switch (hdq_priv.hdq_state) {
	case HDQB_IDLE:
		if (hdq_priv.hdq_request_ctr == hdq_priv.hdq_transaction_ctr)
			break;
		hdq_priv.hdq_ctr = 250 / HDQ_SAMPLE_PERIOD_US;
		hdq_priv.pdata->gpio_set(0);
		hdq_priv.pdata->gpio_dir_out();
		hdq_priv.hdq_tx_data_done = 0;
		hdq_priv.hdq_state = HDQB_TX_BREAK;
		break;

	case HDQB_TX_BREAK: /* issue low for > 190us */
		if (--hdq_priv.hdq_ctr == 0) {
			hdq_priv.hdq_ctr = 60 / HDQ_SAMPLE_PERIOD_US;
			hdq_priv.hdq_state = HDQB_TX_BREAK_RECOVERY;
			hdq_priv.pdata->gpio_set(1);
		}
		break;

	case HDQB_TX_BREAK_RECOVERY: /* issue low for > 40us */
		if (--hdq_priv.hdq_ctr)
			break;
		hdq_priv.hdq_shifter = hdq_priv.hdq_ads;
		hdq_priv.hdq_bit = 8; /* 8 bits of ads / rw */
		hdq_priv.hdq_tx_data_done = 0; /* doing ads */
		/* fallthru on last one */
	case HDQB_ADS_CALC:
		if (hdq_priv.hdq_shifter & 1)
			hdq_priv.hdq_ctr = 50 / HDQ_SAMPLE_PERIOD_US;
		else
			hdq_priv.hdq_ctr = 120 / HDQ_SAMPLE_PERIOD_US;
		/* carefully precompute the other phase length */
		hdq_priv.hdq_ctr2 = (210 - (hdq_priv.hdq_ctr * HDQ_SAMPLE_PERIOD_US)) /
				HDQ_SAMPLE_PERIOD_US;
		hdq_priv.hdq_state = HDQB_ADS_LOW;
		hdq_priv.hdq_shifter >>= 1;
		hdq_priv.hdq_bit--;
		hdq_priv.pdata->gpio_set(0);
		break;

	case HDQB_ADS_LOW:
		if (--hdq_priv.hdq_ctr)
			break;
		hdq_priv.pdata->gpio_set(1);
		hdq_priv.hdq_state = HDQB_ADS_HIGH;
		break;

	case HDQB_ADS_HIGH:
		if (--hdq_priv.hdq_ctr2 > 1) /* account for HDQB_ADS_CALC */
			break;
		if (hdq_priv.hdq_bit) { /* more bits to do */
			hdq_priv.hdq_state = HDQB_ADS_CALC;
			break;
		}
		/* no more bits, wait it out until hdq_priv.hdq_ctr2 exhausted */
		if (hdq_priv.hdq_ctr2)
			break;
		/* ok no more bits and very last state */
		hdq_priv.hdq_ctr = 60 / HDQ_SAMPLE_PERIOD_US;
		/* FIXME 0 = read */
		if (hdq_priv.hdq_ads & 0x80) { /* write the byte out */
			 /* set delay before payload */
			hdq_priv.hdq_ctr = 300 / HDQ_SAMPLE_PERIOD_US;
 			/* already high, no need to write */
			hdq_priv.hdq_state = HDQB_WAIT_TX;
			break;
		}
		/* read the next byte */
		hdq_priv.hdq_bit = 8; /* 8 bits of data */
		hdq_priv.hdq_ctr = 2500 / HDQ_SAMPLE_PERIOD_US;
		hdq_priv.hdq_state = HDQB_WAIT_RX;
		hdq_priv.pdata->gpio_dir_in();
		break;

	case HDQB_WAIT_TX: /* issue low for > 40us */
		if (--hdq_priv.hdq_ctr)
			break;
		if (!hdq_priv.hdq_tx_data_done) { /* was that the data sent? */
			hdq_priv.hdq_tx_data_done++;
			hdq_priv.hdq_shifter = hdq_priv.hdq_tx_data;
			hdq_priv.hdq_bit = 8; /* 8 bits of data */
			hdq_priv.hdq_state = HDQB_ADS_CALC; /* start sending */
			break;
		}
		hdq_priv.hdq_error = 0;
		hdq_priv.hdq_transaction_ctr = hdq_priv.hdq_request_ctr;
		hdq_priv.hdq_state = HDQB_IDLE; /* all tx is done */
		/* idle in input mode, it's pulled up by 10K */
		hdq_priv.pdata->gpio_dir_in();
		break;

	case HDQB_WAIT_RX: /* wait for battery to talk to us */
		if (hdq_priv.pdata->gpio_get() == 0) {
			/* it talks to us! */
			hdq_priv.hdq_ctr2 = 1;
			hdq_priv.hdq_bit = 8; /* 8 bits of data */
			/* timeout */
			hdq_priv.hdq_ctr = 500 / HDQ_SAMPLE_PERIOD_US;
			hdq_priv.hdq_state = HDQB_DATA_RX_LOW;
			break;
		}
		if (--hdq_priv.hdq_ctr == 0) { /* timed out, error */
			hdq_priv.hdq_error = 1;
			hdq_priv.hdq_transaction_ctr = hdq_priv.hdq_request_ctr;
			hdq_priv.hdq_state = HDQB_IDLE; /* abort */
		}
		break;

	/*
	 * HDQ basically works by measuring the low time of the bit cell
	 * 32-50us --> '1', 80 - 145us --> '0'
	 */

	case HDQB_DATA_RX_LOW:
		if (hdq_priv.pdata->gpio_get()) {
			hdq_priv.hdq_rx_data >>= 1;
			if (hdq_priv.hdq_ctr2 <= (65 / HDQ_SAMPLE_PERIOD_US))
				hdq_priv.hdq_rx_data |= 0x80;

			if (--hdq_priv.hdq_bit == 0) {
				hdq_priv.hdq_error = 0;
				hdq_priv.hdq_transaction_ctr =
							hdq_priv.hdq_request_ctr;

				hdq_priv.hdq_state = HDQB_IDLE;
			} else
				hdq_priv.hdq_state = HDQB_DATA_RX_HIGH;
			/* timeout */
			hdq_priv.hdq_ctr = 1000 / HDQ_SAMPLE_PERIOD_US;
			hdq_priv.hdq_ctr2 = 1;
			break;
		}
		hdq_priv.hdq_ctr2++;
		if (--hdq_priv.hdq_ctr)
			break;
		 /* timed out, error */
		hdq_priv.hdq_error = 2;
		hdq_priv.hdq_transaction_ctr = hdq_priv.hdq_request_ctr;
		hdq_priv.hdq_state = HDQB_IDLE; /* abort */
		break;

	case HDQB_DATA_RX_HIGH:
		if (!hdq_priv.pdata->gpio_get()) {
			/* it talks to us! */
			hdq_priv.hdq_ctr2 = 1;
			/* timeout */
			hdq_priv.hdq_ctr = 400 / HDQ_SAMPLE_PERIOD_US;
			hdq_priv.hdq_state = HDQB_DATA_RX_LOW;
			break;
		}
		if (--hdq_priv.hdq_ctr)
			break;
		/* timed out, error */
		hdq_priv.hdq_error = 3;
		hdq_priv.hdq_transaction_ctr = hdq_priv.hdq_request_ctr;

		/* we're in input mode already */
		hdq_priv.hdq_state = HDQB_IDLE; /* abort */
		break;
	}

	/* Are we interested in keeping the FIQ source alive ? */
	if (hdq_priv.hdq_state != HDQB_IDLE)
		return 1;
	else
		return 0;
}
static int fiq_busy(void)
{
	int request = (volatile u8)hdq_priv.hdq_request_ctr;
	int transact = (volatile u8)hdq_priv.hdq_transaction_ctr;


	return (request != transact);
}

int hdq_initialized(void)
{
	return hdq_priv.hdq_probed;
}
EXPORT_SYMBOL_GPL(hdq_initialized);

int hdq_read(int address)
{
	int count_sleeps = 5;
	int ret = -ETIME;

	if (!hdq_priv.hdq_probed)
		return -EINVAL;

	mutex_lock(&hdq_priv.hdq_lock);

	hdq_priv.hdq_error = 0;
	hdq_priv.hdq_ads = address | HDQ_READ;
	hdq_priv.hdq_request_ctr++;
	hdq_priv.pdata->kick_fiq();
	/*
	 * FIQ takes care of it while we block our calling process
	 * But we're not spinning -- other processes run normally while
	 * we wait for the result
	 */
	while (count_sleeps--) {
		msleep(10); /* valid transaction always completes in < 10ms */

		if (fiq_busy())
			continue;

		if (hdq_priv.hdq_error) {
			hdq_bad();
			goto done; /* didn't see a response in good time */
		}
		hdq_good();

		ret = hdq_priv.hdq_rx_data;
		goto done;
	}

done:
	mutex_unlock(&hdq_priv.hdq_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(hdq_read);

int hdq_write(int address, u8 data)
{
	int count_sleeps = 5;
	int ret = -ETIME;

	if (!hdq_priv.hdq_probed)
		return -EINVAL;

	mutex_lock(&hdq_priv.hdq_lock);

	hdq_priv.hdq_error = 0;
	hdq_priv.hdq_ads = address | HDQ_WRITE;
	hdq_priv.hdq_tx_data = data;
	hdq_priv.hdq_request_ctr++;
	hdq_priv.pdata->kick_fiq();
	/*
	 * FIQ takes care of it while we block our calling process
	 * But we're not spinning -- other processes run normally while
	 * we wait for the result
	 */
	while (count_sleeps--) {
		msleep(10); /* valid transaction always completes in < 10ms */

		if (fiq_busy())
			continue; /* something bad with FIQ */

		if (hdq_priv.hdq_error) {
			hdq_bad();
			goto done; /* didn't see a response in good time */
		}
		hdq_good();

		ret = 0;
		goto done;
	}

done:
	mutex_unlock(&hdq_priv.hdq_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(hdq_write);

/* sysfs */

static ssize_t hdq_sysfs_dump(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	int n;
	int v;
	u8 u8a[128]; /* whole address space for HDQ */
	char *end = buf;

	if (!hdq_priv.hdq_probed)
		return -EINVAL;

	/* the dump does not take care about 16 bit regs, because at this
	 * bus level we don't know about the chip details
	 */
	for (n = 0; n < sizeof(u8a); n++) {
		v = hdq_read(n);
		if (v < 0)
			goto bail;
		u8a[n] = v;
	}

	for (n = 0; n < sizeof(u8a); n += 16) {
		hex_dump_to_buffer(u8a + n, sizeof(u8a), 16, 1, end, 4096, 0);
		end += strlen(end);
		*end++ = '\n';
		*end = '\0';
	}
	return (end - buf);

bail:
	return sprintf(buf, "ERROR %d\n", v);
}

/* you write by <address> <data>, eg, "34 128" */

#define atoi(str) simple_strtoul(((str != NULL) ? str : ""), NULL, 0)

static ssize_t hdq_sysfs_write(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	const char *end = buf + count;
	int address = atoi(buf);

	if (!hdq_priv.hdq_probed)
		return -EINVAL;

	while ((buf != end) && (*buf != ' '))
		buf++;
	if (buf >= end)
		return 0;
	while ((buf < end) && (*buf == ' '))
		buf++;
	if (buf >= end)
		return 0;

	hdq_write(address, atoi(buf));

	return count;
}

static DEVICE_ATTR(dump, 0400, hdq_sysfs_dump, NULL);
static DEVICE_ATTR(write, 0600, NULL, hdq_sysfs_write);

static struct attribute *hdq_sysfs_entries[] = {
	&dev_attr_dump.attr,
	&dev_attr_write.attr,
	NULL
};

static struct attribute_group hdq_attr_group = {
	.name	= "hdq",
	.attrs	= hdq_sysfs_entries,
};


#ifdef CONFIG_PM
static int hdq_suspend(struct platform_device *pdev, pm_message_t state)
{
	/* after 18s of this, the battery monitor will also go to sleep */
	hdq_priv.pdata->gpio_dir_in();
	hdq_priv.pdata->disable_fiq();
	return 0;
}

static int hdq_resume(struct platform_device *pdev)
{
	hdq_priv.pdata->gpio_set(1);
	hdq_priv.pdata->gpio_dir_out();
	hdq_priv.pdata->enable_fiq();
	return 0;
}
#endif

static int __init hdq_probe(struct platform_device *pdev)
{
	struct resource *r = platform_get_resource(pdev, 0, 0);
	int ret;
	struct hdq_platform_data *pdata = pdev->dev.platform_data;

	if (!r || !pdata)
		return -EINVAL;

	platform_set_drvdata(pdev, NULL);

	mutex_init(&hdq_priv.hdq_lock);

	/* set our HDQ comms pin from the platform data */
	hdq_priv.hdq_gpio_pin = r->start;
	hdq_priv.pdata = pdata;

	hdq_priv.pdata->gpio_set(1);
	hdq_priv.pdata->gpio_dir_out();

	/* Initialize FIQ */
	if (hdq_priv.pdata->enable_fiq() < 0) {
		dev_err(&pdev->dev, "Could not enable FIQ source\n");
		return -EINVAL;
	}

	ret = sysfs_create_group(&pdev->dev.kobj, &hdq_attr_group);
	if (ret)
		return ret;

	hdq_priv.hdq_probed = 1; /* we are ready to do stuff now */

	/*
	 * if wanted, users can defer registration of devices
	 * that depend on HDQ until after we register, and can use our
	 * device as parent so suspend-resume ordering is correct
	 */
	if (pdata->attach_child_devices)
		(pdata->attach_child_devices)(&pdev->dev);

	hdq_priv.pdata = pdata;

	return 0;
}

static int hdq_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &hdq_attr_group);
	return 0;
}

static struct platform_driver hdq_driver = {
	.probe		= hdq_probe,
	.remove		= hdq_remove,
#ifdef CONFIG_PM
	.suspend	= hdq_suspend,
	.resume		= hdq_resume,
#endif
	.driver		= {
		.name		= "hdq",
	},
};

static int __init hdq_init(void)
{
	return platform_driver_register(&hdq_driver);
}

static void __exit hdq_exit(void)
{
 	platform_driver_unregister(&hdq_driver);
}

module_init(hdq_init);
module_exit(hdq_exit);

MODULE_AUTHOR("Andy Green <andy@openmoko.com>");
MODULE_DESCRIPTION("HDQ driver");
