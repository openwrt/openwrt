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


#define PFX	"ucmb: "


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Microcontroller Message Bus");
MODULE_AUTHOR("Michael Buesch");


struct ucmb {
	/* Misc character device driver */
	struct miscdevice mdev;
	struct file_operations mdev_fops;

	/* SPI driver */
	struct spi_device *sdev;

	/* SPI-GPIO driver */
	struct spi_gpio_platform_data spi_gpio_pdata;
	struct platform_device spi_gpio_pdev;
};

static int ucmb_spi_busnum_count = 1337;


static struct ucmb_platform_data ucmb_list[] = {
	{ //FIXME don't define it here.
		.name		= "ucmb",
		.gpio_sck	= 0,
		.gpio_miso	= 1,
		.gpio_mosi	= 2,
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

static inline struct ucmb * filp_to_ucmb(struct file *filp)
{
	return container_of(filp->f_op, struct ucmb, mdev_fops);
}

/* FIXME offp */
static ssize_t ucmb_read(struct file *filp, char __user *user_buf,
			 size_t size, loff_t *offp)
{
	struct ucmb *ucmb = filp_to_ucmb(filp);
	u8 *buf;
	int err;

	err = -E2BIG;
	if (size > PAGE_SIZE)
		goto out;

	err = -ENOMEM;
	buf = (char *)__get_free_page(GFP_KERNEL);
	if (!buf)
		goto out;

	err = spi_read(ucmb->sdev, buf, size);
	if (err)
		goto out_free;

	err = -EFAULT;
	if (copy_to_user(user_buf, buf, size))
		goto out_free;

	err = 0;

out_free:
	__free_page((unsigned long)buf);
out:
	return err ? err : size;
}

/* FIXME offp */
static ssize_t ucmb_write(struct file *filp, const char __user *user_buf,
			  size_t size, loff_t *offp)
{
	struct ucmb *ucmb = filp_to_ucmb(filp);
	u8 *buf;
	int err;

	err = -ENOMEM;
	buf = (char *)__get_free_page(GFP_KERNEL);
	if (!buf)
		goto out;

	size = min(PAGE_SIZE, size);
	err = -EFAULT;
	if (copy_from_user(buf, user_buf, size))
		goto out_free;

	err = spi_write(ucmb->sdev, buf, size);

out_free:
	__free_page((unsigned long)buf);
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
	ucmb->sdev->max_speed_hz = 5000000;
	ucmb->sdev->chip_select = 0;
	ucmb->sdev->mode = SPI_MODE_0;
	strlcpy(ucmb->sdev->modalias, "ucmb", /* We are the SPI driver. */
		sizeof(ucmb->sdev->modalias));
	ucmb->sdev->controller_data = (void *)SPI_GPIO_NO_CHIPSELECT;
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
