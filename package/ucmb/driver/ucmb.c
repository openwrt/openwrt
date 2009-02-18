/*
 * Microcontroller Message Bus
 *
 * Copyright (c) 2009 Michael Buesch <mb@bu3sch.de>
 *
 * Licensed under the GNU/GPL. See COPYING for details.
 */

#include "ucmb.h"

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/gfp.h>
#include <linux/delay.h>
#include <linux/crc16.h>


#define PFX	"ucmb: "

#define DEBUG


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Microcontroller Message Bus");
MODULE_AUTHOR("Michael Buesch");


struct ucmb {
	unsigned int msg_delay_ms;

	/* Misc character device driver */
	struct miscdevice mdev;
	struct file_operations mdev_fops;

	/* SPI driver */
	struct spi_device *sdev;

	/* SPI-GPIO driver */
	struct spi_gpio_platform_data spi_gpio_pdata;
	struct platform_device spi_gpio_pdev;
};

struct ucmb_message_hdr {
	__le16 magic;		/* UCMB_MAGIC */
	__le16 len;		/* Payload length (excluding header) */
} __attribute__((packed));

struct ucmb_message_footer {
	__le16 crc;		/* CRC of the header + payload. */
} __attribute__((packed));

struct ucmb_status {
	__le16 magic;		/* UCMB_MAGIC */
	__le16 code;		/* enum ucmb_status_code */
} __attribute__((packed));

#define UCMB_MAGIC		0x1337
#define UCMB_MAX_MSG_LEN	0x200

enum ucmb_status_code {
	UCMB_STAT_OK = 0,
	UCMB_STAT_EPROTO,	/* Protocol format error */
	UCMB_STAT_ENOMEM,	/* Out of memory */
	UCMB_STAT_E2BIG,	/* Message too big */
	UCMB_STAT_ECRC,		/* CRC error */
};


static int ucmb_spi_busnum_count = 1337;


static struct ucmb_platform_data ucmb_list[] = {
	{ //FIXME don't define it here.
		.name		= "ucmb",
		.gpio_cs	= 3,
		.gpio_sck	= 0,
		.gpio_miso	= 1,
		.gpio_mosi	= 2,
		.mode		= SPI_MODE_0,
		.max_speed_hz	= 128000, /* Hz */
		.msg_delay_ms	= 1, /* mS */
	},
};


static int __devinit ucmb_spi_probe(struct spi_device *sdev)
{
	return 0;
}

static int __devexit ucmb_spi_remove(struct spi_device *sdev)
{
	return 0;
}

static struct spi_driver ucmb_spi_driver = {
	.driver		= {
		.name	= "ucmb",
		.bus	= &spi_bus_type,
		.owner	= THIS_MODULE,
	},
	.probe		= ucmb_spi_probe,
	.remove		= __devexit_p(ucmb_spi_remove),
};

static int ucmb_status_code_to_errno(enum ucmb_status_code code)
{
	switch (code) {
	case UCMB_STAT_OK:
		return 0;
	case UCMB_STAT_EPROTO:
		return -EPROTO;
	case UCMB_STAT_ENOMEM:
		return -ENOMEM;
	case UCMB_STAT_E2BIG:
		return -E2BIG;
	case UCMB_STAT_ECRC:
		return -EBADMSG;
	}
	return -EBUSY;
}

static inline struct ucmb * filp_to_ucmb(struct file *filp)
{
	return container_of(filp->f_op, struct ucmb, mdev_fops);
}

static ssize_t ucmb_read(struct file *filp, char __user *user_buf,
			 size_t size, loff_t *offp)
{
	struct ucmb *ucmb = filp_to_ucmb(filp);
	u8 *buf;
	int res, err;
	struct ucmb_message_hdr hdr;
	struct ucmb_message_footer footer;
	struct ucmb_status status = { .magic = cpu_to_le16(UCMB_MAGIC), };
	u16 crc = 0xFFFF;

	size = min_t(size_t, size, PAGE_SIZE);

	err = -ENOMEM;
	buf = (char *)__get_free_page(GFP_KERNEL);
	if (!buf)
		goto out;

	err = spi_read(ucmb->sdev, (u8 *)&hdr, sizeof(hdr));
	if (err)
		goto out_free;
#ifdef DEBUG
	printk(KERN_DEBUG PFX "Received message header 0x%04X 0x%04X\n",
	       le16_to_cpu(hdr.magic), le16_to_cpu(hdr.len));
#endif
	err = -EPROTO;
	if (hdr.magic != cpu_to_le16(UCMB_MAGIC))
		goto out_free;
	err = -ENOBUFS;
	if (size < le16_to_cpu(hdr.len))
		goto out_free;
	size = le16_to_cpu(hdr.len);
	err = spi_read(ucmb->sdev, buf, size);
	if (err)
		goto out_free;
	err = spi_read(ucmb->sdev, (u8 *)&footer, sizeof(footer));
	if (err)
		goto out_free;

	crc = crc16(crc, &hdr, sizeof(hdr));
	crc = crc16(crc, buf, size);
	crc ^= 0xFFFF;
	if (crc != le16_to_cpu(footer.crc)) {
		err = -EPROTO;
		status.code = UCMB_STAT_ECRC;
		goto out_send_status;
	}

	if (copy_to_user(user_buf, buf, size)) {
		err = -EFAULT;
		status.code = UCMB_STAT_ENOMEM;
		goto out_send_status;
	}

	status.code = UCMB_STAT_OK;
	err = 0;

out_send_status:
	res = spi_write(ucmb->sdev, (u8 *)&status, sizeof(status));
	if (res && !err)
		err = res;
out_free:
	free_page((unsigned long)buf);
out:
	return err ? err : size;
}

static ssize_t ucmb_write(struct file *filp, const char __user *user_buf,
			  size_t size, loff_t *offp)
{
	struct ucmb *ucmb = filp_to_ucmb(filp);
	u8 *buf;
	int err;
	struct ucmb_message_hdr hdr = { .magic = cpu_to_le16(UCMB_MAGIC), };
	struct ucmb_message_footer footer = { .crc = 0xFFFF, };
	struct ucmb_status status;
	struct spi_transfer spi_hdr_xfer;
	struct spi_transfer spi_footer_xfer;
	struct spi_transfer spi_data_xfer;
	struct spi_message spi_msg;

	err = -ENOMEM;
	buf = (char *)__get_free_page(GFP_KERNEL);
	if (!buf)
		goto out;

	size = min_t(size_t, PAGE_SIZE, size);
	size = min_t(size_t, UCMB_MAX_MSG_LEN, size);
	err = -EFAULT;
	if (copy_from_user(buf, user_buf, size))
		goto out_free;
	hdr.len = cpu_to_le16(size);

	footer.crc = crc16(footer.crc, &hdr, sizeof(hdr));
	footer.crc = crc16(footer.crc, buf, size);
	footer.crc ^= 0xFFFF;

	spi_message_init(&spi_msg);

	memset(&spi_hdr_xfer, 0, sizeof(spi_hdr_xfer));
	spi_hdr_xfer.tx_buf = &hdr;
	spi_hdr_xfer.len = sizeof(hdr);
	spi_message_add_tail(&spi_hdr_xfer, &spi_msg);

	memset(&spi_data_xfer, 0, sizeof(spi_data_xfer));
	spi_data_xfer.tx_buf = buf;
	spi_data_xfer.len = size;
	spi_message_add_tail(&spi_data_xfer, &spi_msg);

	memset(&spi_footer_xfer, 0, sizeof(spi_footer_xfer));
	spi_footer_xfer.tx_buf = &footer;
	spi_footer_xfer.len = sizeof(footer);
	spi_message_add_tail(&spi_footer_xfer, &spi_msg);

	/* Send the message, including header. */
	err = spi_sync(ucmb->sdev, &spi_msg);
	if (err)
		goto out_free;

	/* The microcontroller deserves some time to process the message. */
	if (ucmb->msg_delay_ms)
		msleep(ucmb->msg_delay_ms);

	/* Get the status code. */
	err = spi_read(ucmb->sdev, (u8 *)&status, sizeof(status));
	if (err)
		goto out_free;
#ifdef DEBUG
	printk(KERN_DEBUG PFX "Sent message. Status report: 0x%04X 0x%04X\n",
	       le16_to_cpu(status.magic), le16_to_cpu(status.code));
#endif
	err = -EPROTO;
	if (status.magic != cpu_to_le16(UCMB_MAGIC))
		goto out_free;
	err = ucmb_status_code_to_errno(le16_to_cpu(status.code));
	if (err)
		goto out_free;

out_free:
	free_page((unsigned long)buf);
out:
	return err ? err : size;
}

static int __devinit ucmb_probe(struct platform_device *pdev)
{
	struct ucmb_platform_data *pdata;
	struct ucmb *ucmb;
	int err;
	const int bus_num = ucmb_spi_busnum_count++;
	struct spi_bitbang *bb;

	pdata = pdev->dev.platform_data;
	if (!pdata)
		return -ENXIO;

	ucmb = kzalloc(sizeof(struct ucmb), GFP_KERNEL);
	if (!ucmb)
		return -ENOMEM;
	ucmb->msg_delay_ms = pdata->msg_delay_ms;

	/* Create the SPI GPIO bus master. */

#ifdef CONFIG_SPI_GPIO_MODULE
	err = request_module("spi_gpio");
	if (err)
		printk(KERN_WARNING PFX "Failed to request spi_gpio module\n");
#endif /* CONFIG_SPI_GPIO_MODULE */

	ucmb->spi_gpio_pdata.sck = pdata->gpio_sck;
	ucmb->spi_gpio_pdata.mosi = pdata->gpio_mosi;
	ucmb->spi_gpio_pdata.miso = pdata->gpio_miso;
	ucmb->spi_gpio_pdata.num_chipselect = 1;

	ucmb->spi_gpio_pdev.name = "spi_gpio";
	ucmb->spi_gpio_pdev.id = bus_num;
	ucmb->spi_gpio_pdev.dev.platform_data = &ucmb->spi_gpio_pdata;

	err = platform_device_register(&ucmb->spi_gpio_pdev);
	if (err) {
		printk(KERN_ERR PFX "Failed to register SPI-GPIO platform device\n");
		goto err_free_ucmb;
	}
	bb = platform_get_drvdata(&ucmb->spi_gpio_pdev);
	if (!bb || !bb->master) {
		printk(KERN_ERR PFX "No bitbanged master device found.\n");
		goto err_unreg_spi_gpio_pdev;
	}

	/* Create the SPI device. */

	ucmb->sdev = spi_alloc_device(bb->master);
	if (!ucmb->sdev) {
		printk(KERN_ERR PFX "Failed to allocate SPI device\n");
		goto err_unreg_spi_gpio_pdev;
	}
	ucmb->sdev->max_speed_hz = pdata->max_speed_hz;
	ucmb->sdev->chip_select = 0;
	ucmb->sdev->mode = pdata->mode;
	strlcpy(ucmb->sdev->modalias, "ucmb", /* We are the SPI driver. */
		sizeof(ucmb->sdev->modalias));
	ucmb->sdev->controller_data = (void *)pdata->gpio_cs;
	err = spi_add_device(ucmb->sdev);
	if (err) {
		printk(KERN_ERR PFX "Failed to add SPI device\n");
		goto err_free_spi_device;
	}

	/* Create the Misc char device. */

	ucmb->mdev.minor = MISC_DYNAMIC_MINOR;
	ucmb->mdev.name = pdata->name;
	ucmb->mdev.parent = &pdev->dev;
	ucmb->mdev_fops.read = ucmb_read;
	ucmb->mdev_fops.write = ucmb_write;
	ucmb->mdev.fops = &ucmb->mdev_fops;

	err = misc_register(&ucmb->mdev);
	if (err) {
		printk(KERN_ERR PFX "Failed to register miscdev %s\n",
		       ucmb->mdev.name);
		goto err_unreg_spi_device;
	}

	platform_set_drvdata(pdev, ucmb);

	printk(KERN_INFO PFX "Registered message bus \"%s\"\n", pdata->name);

	return 0;

err_unreg_spi_device:
	spi_unregister_device(ucmb->sdev);
err_free_spi_device:
	spi_dev_put(ucmb->sdev);
err_unreg_spi_gpio_pdev:
	platform_device_unregister(&ucmb->spi_gpio_pdev);
err_free_ucmb:
	kfree(ucmb);

	return err;
}

static int __devexit ucmb_remove(struct platform_device *pdev)
{
	struct ucmb *ucmb = platform_get_drvdata(pdev);
	int err;

	err = misc_deregister(&ucmb->mdev);
	if (err) {
		printk(KERN_ERR PFX "Failed to unregister miscdev %s\n",
		       ucmb->mdev.name);
	}
	spi_unregister_device(ucmb->sdev);
	spi_dev_put(ucmb->sdev);
	platform_device_unregister(&ucmb->spi_gpio_pdev);

	kfree(ucmb);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver ucmb_driver = {
	.driver		= {
		.name	= "ucmb",
		.owner	= THIS_MODULE,
	},
	.probe		= ucmb_probe,
	.remove		= __devexit_p(ucmb_probe),
};

static int ucmb_modinit(void)
{
	struct ucmb_platform_data *pdata;
	struct platform_device *pdev;
	int err, i;

	printk(KERN_INFO "Microcontroller message bus driver\n");

	err = platform_driver_register(&ucmb_driver);
	if (err) {
		printk(KERN_ERR PFX "Failed to register platform driver\n");
		return err;
	}
	err = spi_register_driver(&ucmb_spi_driver);
	if (err) {
		printk(KERN_ERR PFX "Failed to register SPI driver\n");
		platform_driver_unregister(&ucmb_driver);
		return err;
	}

	for (i = 0; i < ARRAY_SIZE(ucmb_list); i++) {
		pdata = &ucmb_list[i];

		pdev = platform_device_alloc("ucmb", i);
		if (!pdev) {
			printk(KERN_ERR PFX "Failed to allocate platform device.\n");
			break;
		}
		err = platform_device_add_data(pdev, pdata, sizeof(*pdata));
		if (err) {
			printk(KERN_ERR PFX "Failed to add platform data.\n");
			platform_device_put(pdev);
			break;
		}
		err = platform_device_add(pdev);
		if (err) {
			printk(KERN_ERR PFX "Failed to register platform device.\n");
			platform_device_put(pdev);
			break;
		}
		pdata->pdev = pdev;
	}

	return 0;
}
module_init(ucmb_modinit);

static void ucmb_modexit(void)
{
	struct ucmb_platform_data *pdata;
	int i;

	for (i = 0; i < ARRAY_SIZE(ucmb_list); i++) {
		pdata = &ucmb_list[i];

		if (pdata->pdev) {
			platform_device_unregister(pdata->pdev);
			platform_device_put(pdata->pdev);
		}
	}
	spi_unregister_driver(&ucmb_spi_driver);
	platform_driver_unregister(&ucmb_driver);
}
module_exit(ucmb_modexit);
