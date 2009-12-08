/*
 * Platform driver for the Realtek RTL8366 ethernet switch
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <linux/skbuff.h>
#include <linux/phy.h>
#include <linux/rtl8366_smi.h>

//#define DEBUG	1

#define RTL8366_SMI_DRIVER_NAME	"rtl8366-smi"
#define RTL8366_SMI_DRIVER_DESC	"Realtek RTL8366 switch driver"
#define RTL8366_SMI_DRIVER_VER	"0.1.0"

#define RTL8366S_PHY_NO_MAX		4
#define RTL8366S_PHY_PAGE_MAX		7
#define RTL8366S_PHY_ADDR_MAX		31

#define RTL8366S_CHIP_VERSION_CTRL_REG	0x0104
#define RTL8366S_CHIP_VERSION_MASK	0xf
#define RTL8366S_CHIP_ID_REG		0x0105
#define RTL8366S_CHIP_ID_8366		0x8366

/* PHY registers control */
#define RTL8366S_PHY_ACCESS_CTRL_REG	0x8028
#define RTL8366S_PHY_ACCESS_DATA_REG	0x8029

#define RTL8366S_PHY_CTRL_READ		1
#define RTL8366S_PHY_CTRL_WRITE		0

#define RTL8366S_PHY_REG_MASK		0x1f
#define RTL8366S_PHY_PAGE_OFFSET	5
#define RTL8366S_PHY_PAGE_MASK		(0x7 << 5)
#define RTL8366S_PHY_NO_OFFSET		9
#define RTL8366S_PHY_NO_MASK		(0x1f << 9)

#define RTL8366_SMI_ACK_RETRY_COUNT	5
#define RTL8366_SMI_CLK_DELAY		10 /* nsec */

struct rtl8366_smi {
	struct platform_device			*pdev;
	struct rtl8366_smi_platform_data	*pdata;
	spinlock_t				lock;
	struct mii_bus				*mii_bus;
	int					mii_irq[PHY_MAX_ADDR];
};

static inline void rtl8366_smi_clk_delay(struct rtl8366_smi *smi)
{
	ndelay(RTL8366_SMI_CLK_DELAY);
}

static void rtl8366_smi_start(struct rtl8366_smi *smi)
{
	unsigned int sda = smi->pdata->gpio_sda;
	unsigned int sck = smi->pdata->gpio_sck;

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
	unsigned int sda = smi->pdata->gpio_sda;
	unsigned int sck = smi->pdata->gpio_sck;

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
	unsigned int sda = smi->pdata->gpio_sda;
	unsigned int sck = smi->pdata->gpio_sck;

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
	unsigned int sda = smi->pdata->gpio_sda;
	unsigned int sck = smi->pdata->gpio_sck;

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

static int rtl8366_smi_read_reg(struct rtl8366_smi *smi, u32 addr, u32 *data)
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

static int rtl8366_smi_write_reg(struct rtl8366_smi *smi, u32 addr, u32 data)
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

static int rtl8366_smi_read_phy_reg(struct rtl8366_smi *smi,
				    u32 phy_no, u32 page, u32 addr, u32 *data)
{
	u32 reg;
	int ret;

	if (phy_no > RTL8366S_PHY_NO_MAX)
		return -EINVAL;

	if (page > RTL8366S_PHY_PAGE_MAX)
		return -EINVAL;

	if (addr > RTL8366S_PHY_ADDR_MAX)
		return -EINVAL;

	ret = rtl8366_smi_write_reg(smi, RTL8366S_PHY_ACCESS_CTRL_REG,
				    RTL8366S_PHY_CTRL_READ);
	if (ret)
		return ret;

	reg = 0x8000 | (1 << (phy_no + RTL8366S_PHY_NO_OFFSET)) |
	      ((page << RTL8366S_PHY_PAGE_OFFSET) & RTL8366S_PHY_PAGE_MASK) |
	      (addr & RTL8366S_PHY_REG_MASK);

	ret = rtl8366_smi_write_reg(smi, reg, 0);
	if (ret)
		return ret;

	ret = rtl8366_smi_read_reg(smi, RTL8366S_PHY_ACCESS_DATA_REG, data);
	if (ret)
		return ret;

	return 0;
}

static int rtl8366_smi_write_phy_reg(struct rtl8366_smi *smi,
				     u32 phy_no, u32 page, u32 addr, u32 data)
{
	u32 reg;
	int ret;

	if (phy_no > RTL8366S_PHY_NO_MAX)
		return -EINVAL;

	if (page > RTL8366S_PHY_PAGE_MAX)
		return -EINVAL;

	if (addr > RTL8366S_PHY_ADDR_MAX)
		return -EINVAL;

	ret = rtl8366_smi_write_reg(smi, RTL8366S_PHY_ACCESS_CTRL_REG,
				    RTL8366S_PHY_CTRL_WRITE);
	if (ret)
		return ret;

	reg = 0x8000 | (1 << (phy_no + RTL8366S_PHY_NO_OFFSET)) |
	      ((page << RTL8366S_PHY_PAGE_OFFSET) & RTL8366S_PHY_PAGE_MASK) |
	      (addr & RTL8366S_PHY_REG_MASK);

	ret = rtl8366_smi_write_reg(smi, reg, data);
	if (ret)
		return ret;

	return 0;
}

#ifdef DEBUG
static void rtl8366_smi_dump_regs(struct rtl8366_smi *smi)
{
	u32 t;
	int err;
	int i;

	for (i = 0; i < 0x200; i++) {
		err = rtl8366_smi_read_reg(smi, i, &t);
		if (err) {
			dev_err(&smi->pdev->dev,
				"unable to read register %04x\n", i);
			return;
		}
		dev_info(&smi->pdev->dev, "reg %04x: %04x\n", i, t);
	}

	for (i = 0; i <= RTL8366S_PHY_NO_MAX; i++) {
		int j;

		for (j = 0; j <= RTL8366S_PHY_ADDR_MAX; j++) {
			err = rtl8366_smi_read_phy_reg(smi, i, 0, j, &t);
			if (err) {
				dev_err(&smi->pdev->dev,
					"unable to read PHY%u:%02x register\n",
					i, j);
				return;
			}
			dev_info(&smi->pdev->dev,
				 "PHY%u:%02x: %04x\n", i, j, t);
		}
	}
}
#else
static inline void rtl8366_smi_dump_regs(struct rtl8366_smi *smi) {}
#endif

static int rtl8366_smi_mii_read(struct mii_bus *bus, int addr, int reg)
{
	struct rtl8366_smi *smi = bus->priv;
	u32 val = 0;
	int err;

	err = rtl8366_smi_read_phy_reg(smi, addr, 0, reg, &val);
	if (err)
		return 0xffff;

	return val;
}

static int rtl8366_smi_mii_write(struct mii_bus *bus, int addr, int reg,
				     u16 val)
{
	struct rtl8366_smi *smi = bus->priv;
	u32 t;
	int err;

	err = rtl8366_smi_write_phy_reg(smi, addr, 0, reg, val);
	/* flush write */
	(void) rtl8366_smi_read_phy_reg(smi, addr, 0, reg, &t);

	return err;
}

static int rtl8366_smi_mii_init(struct rtl8366_smi *smi)
{
	int ret;
	int i;

	smi->mii_bus = mdiobus_alloc();
	if (smi->mii_bus == NULL) {
		ret = -ENOMEM;
		goto err;
	}

	spin_lock_init(&smi->lock);
	smi->mii_bus->priv = (void *) smi;
	smi->mii_bus->name = "rtl8366-smi";
	smi->mii_bus->read = rtl8366_smi_mii_read;
	smi->mii_bus->write = rtl8366_smi_mii_write;
	snprintf(smi->mii_bus->id, MII_BUS_ID_SIZE, "%s",
			dev_name(&smi->pdev->dev));
	smi->mii_bus->parent = &smi->pdev->dev;
	smi->mii_bus->phy_mask = ~(0x1f);
	smi->mii_bus->irq = smi->mii_irq;
	for (i = 0; i < PHY_MAX_ADDR; i++)
		smi->mii_irq[i] = PHY_POLL;

	rtl8366_smi_dump_regs(smi);

	ret = mdiobus_register(smi->mii_bus);
	if (ret)
		goto err_free;

	rtl8366_smi_dump_regs(smi);

	return 0;

 err_free:
	mdiobus_free(smi->mii_bus);
 err:
	return ret;
}

static void rtl8366_smi_mii_cleanup(struct rtl8366_smi *smi)
{
	mdiobus_unregister(smi->mii_bus);
	mdiobus_free(smi->mii_bus);
}

static int rtl8366_smi_setup(struct rtl8366_smi *smi)
{
	u32 chip_id = 0;
	u32 chip_ver = 0;
	int ret;

	ret = rtl8366_smi_read_reg(smi, RTL8366S_CHIP_ID_REG, &chip_id);
	if (ret) {
		dev_err(&smi->pdev->dev, "unable to read chip id\n");
		return ret;
	}

	switch (chip_id) {
	case RTL8366S_CHIP_ID_8366:
		break;
	default:
		dev_err(&smi->pdev->dev, "unknown chip id (%04x)\n", chip_id);
		return -ENODEV;
	}

	ret = rtl8366_smi_read_reg(smi, RTL8366S_CHIP_VERSION_CTRL_REG,
				   &chip_ver);
	if (ret) {
		dev_err(&smi->pdev->dev, "unable to read chip version\n");
		return ret;
	}

	dev_info(&smi->pdev->dev, "RTL%04x ver. %u chip found\n",
		 chip_id, chip_ver & RTL8366S_CHIP_VERSION_MASK);

	return 0;
}

static int __init rtl8366_smi_probe(struct platform_device *pdev)
{
	static int rtl8366_smi_version_printed;
	struct rtl8366_smi_platform_data *pdata;
	struct rtl8366_smi *smi;
	int err;

	if (!rtl8366_smi_version_printed++)
		printk(KERN_NOTICE RTL8366_SMI_DRIVER_DESC
		       " version " RTL8366_SMI_DRIVER_VER"\n");

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data specified\n");
		err = -EINVAL;
		goto err_out;
	}

	smi = kzalloc(sizeof(struct rtl8366_smi), GFP_KERNEL);
	if (!smi) {
		dev_err(&pdev->dev, "no memory for private data\n");
		err = -ENOMEM;
		goto err_out;
	}

	err = gpio_request(pdata->gpio_sda, dev_name(&pdev->dev));
	if (err) {
		dev_err(&pdev->dev, "gpio_request failed for %u, err=%d\n",
			pdata->gpio_sda, err);
		goto err_free_smi;
	}

	err = gpio_request(pdata->gpio_sck, dev_name(&pdev->dev));
	if (err) {
		dev_err(&pdev->dev, "gpio_request failed for %u, err=%d\n",
			pdata->gpio_sck, err);
		goto err_free_sda;
	}

	smi->pdev = pdev;
	smi->pdata = pdata;
	spin_lock_init(&smi->lock);

	platform_set_drvdata(pdev, smi);

	dev_info(&pdev->dev, "using GPIO pins %u (SDA) and %u (SCK)\n",
		 pdata->gpio_sda, pdata->gpio_sck);

	err = rtl8366_smi_setup(smi);
	if (err)
		goto err_clear_drvdata;

	err = rtl8366_smi_mii_init(smi);
	if (err)
		goto err_clear_drvdata;

	return 0;

 err_clear_drvdata:
	platform_set_drvdata(pdev, NULL);
	gpio_free(pdata->gpio_sck);
 err_free_sda:
	gpio_free(pdata->gpio_sda);
 err_free_smi:
	kfree(smi);
 err_out:
	return err;
}

static int __devexit rtl8366_smi_remove(struct platform_device *pdev)
{
	struct rtl8366_smi *smi = platform_get_drvdata(pdev);

	if (smi) {
		struct rtl8366_smi_platform_data *pdata;

		pdata = pdev->dev.platform_data;

		rtl8366_smi_mii_cleanup(smi);
		platform_set_drvdata(pdev, NULL);
		gpio_free(pdata->gpio_sck);
		gpio_free(pdata->gpio_sda);
		kfree(smi);
	}

	return 0;
}

int rtl8366_phy_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static struct platform_driver rtl8366_smi_driver = {
	.driver = {
		.name		= RTL8366_SMI_DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe		= rtl8366_smi_probe,
	.remove		= __devexit_p(rtl8366_smi_remove),
};

static struct phy_driver rtl8366_smi_phy_driver = {
	.phy_id		= 0x001cc960,
	.name		= "Realtek RTL8366",
	.phy_id_mask	= 0x1ffffff0,
	.features	= PHY_GBIT_FEATURES,
	.config_aneg	= rtl8366_phy_config_aneg,
	.read_status	= genphy_read_status,
	.driver		= {
		.owner = THIS_MODULE,
	},
};

static int __init rtl8366_smi_init(void)
{
	int ret;

	ret = phy_driver_register(&rtl8366_smi_phy_driver);
	if (ret)
		return ret;

	ret = platform_driver_register(&rtl8366_smi_driver);
	if (ret)
		goto err_phy_unregister;

	return 0;

 err_phy_unregister:
	phy_driver_unregister(&rtl8366_smi_phy_driver);
	return ret;
}
module_init(rtl8366_smi_init);

static void __exit rtl8366_smi_exit(void)
{
	platform_driver_unregister(&rtl8366_smi_driver);
	phy_driver_unregister(&rtl8366_smi_phy_driver);
}
module_exit(rtl8366_smi_exit);

MODULE_DESCRIPTION(RTL8366_SMI_DRIVER_DESC);
MODULE_VERSION(RTL8366_SMI_DRIVER_VER);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" RTL8366_SMI_DRIVER_NAME);
