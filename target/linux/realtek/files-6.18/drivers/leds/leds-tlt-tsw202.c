// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/bitmap.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/gpio/consumer.h>

#define DRV_NAME      "leds-tlt-tsw202"
#define DEF_DELAY_ON  500
#define DEF_DELAY_OFF 500

/*
 * Register access macros
 */

#define RTL838X_SW_BASE		     ((volatile void *)0xBB000000)
#define sw_r32(reg)		     readl(RTL838X_SW_BASE + reg)
#define sw_w32(val, reg)	     writel(val, RTL838X_SW_BASE + reg)
#define sw_w32_mask(clear, set, reg) sw_w32((sw_r32(reg) & ~(clear)) | (set), reg)

#define MAX_LEDS_IN_GROUP 28
#define RTL_ETH_LEDS	  24

#define REG_LED0_SW_P_EN_CTRL 0xA010
#define REG_LED_SW_P_CTRL     0xA01C

#define MASK_LED_SW_P_CTRL 0x07
#define VAL_LED_SW_P_ON	   0x05
#define VAL_LED_SW_P_OFF   0x0

#define MAX_LEDS_PER_PORT 3
#define MAX_RTL_LEDS	  (MAX_LEDS_IN_GROUP * MAX_LEDS_PER_PORT)

struct gpio_array_led_data {
	struct led_classdev master;
	struct led_classdev *rtl_cdevs;

	DECLARE_BITMAP(rtl_visible, MAX_RTL_LEDS);
	DECLARE_BITMAP(rtl_green, MAX_RTL_LEDS);
	DECLARE_BITMAP(rtl_used, MAX_RTL_LEDS);
	DECLARE_BITMAP(rtl_except, MAX_RTL_LEDS);

	struct mutex lock;
	bool opposed_leds;
};

static struct gpio_array_led_data g_leds = { 0 };

static int rtl_led_set(struct led_classdev *cdev, enum led_brightness value)
{
	int base, pin, i;
	u32 val;
	int index = cdev - g_leds.rtl_cdevs;

	base = index / MAX_LEDS_IN_GROUP;
	pin  = index % MAX_LEDS_IN_GROUP;

	mutex_lock(&g_leds.lock);
	if (g_leds.opposed_leds) {
		for (i = 0; i < MAX_LEDS_PER_PORT; i++) {
			val = value && (i == base) ? VAL_LED_SW_P_ON : VAL_LED_SW_P_OFF;
			sw_w32_mask(0, (1 << pin), REG_LED0_SW_P_EN_CTRL + i * 4);
			sw_w32_mask((MASK_LED_SW_P_CTRL << (i * 3)), val << (i * 3),
				    REG_LED_SW_P_CTRL + pin * 4);
		}
	} else {
		val = value ? VAL_LED_SW_P_ON : VAL_LED_SW_P_OFF;
		sw_w32_mask(0, (1 << pin), REG_LED0_SW_P_EN_CTRL + base * 4);
		sw_w32_mask((MASK_LED_SW_P_CTRL << (base * 3)), val << (base * 3),
			    REG_LED_SW_P_CTRL + pin * 4);
	}
	mutex_unlock(&g_leds.lock);

	return 0;
}

static int master_led_set(struct led_classdev *cdev, enum led_brightness value)
{
	int i;

	for (i = 0; i < MAX_RTL_LEDS; i++) {
		int base, pin;
		u32 val;

		/* pass only used port leds */
		if (!test_bit(i, g_leds.rtl_used) || test_bit(i, g_leds.rtl_except)) {
			continue;
		}

		base = i / MAX_LEDS_IN_GROUP;
		pin  = i % MAX_LEDS_IN_GROUP;

		val = value && test_bit(i, g_leds.rtl_visible) ? VAL_LED_SW_P_ON : VAL_LED_SW_P_OFF;
		/* These values are written more often then it has to be. But it is done quickly.
		 * So.. leave them be */
		mutex_lock(&g_leds.lock);
		sw_w32_mask(0, (1 << pin), REG_LED0_SW_P_EN_CTRL + base * 4);
		sw_w32_mask((MASK_LED_SW_P_CTRL << (base * 3)), val << (base * 3),
			    REG_LED_SW_P_CTRL + pin * 4);
		mutex_unlock(&g_leds.lock);
	}

	return 0;
}

static int master_led_blink_set(struct led_classdev *cdev, unsigned long *delay_on, unsigned long *delay_off)
{
	cdev->blink_delay_on  = *delay_on ? *delay_on : DEF_DELAY_ON;
	cdev->blink_delay_off = *delay_off ? *delay_off : DEF_DELAY_OFF;

	set_bit(LED_BLINK_SW, &cdev->work_flags);
	mod_timer(&cdev->blink_timer, jiffies + 1);

	return 0;
}


/* addr value mask
		cfg = 
			<0x1004 0x00000000 0x00000003>,
			<0xa000 0x0000001b 0x000001bf>,
			<0xa004 0x000000e0 0x0fffffff>,
			<0xa008 0x0f00ff00 0x0fffffff>,
			<0xa010 0x0500ff00 0x0fffffff>,
			<0xa014 0x0800ff00 0x0fffffff>,
			<0xa018 0x00000000 0x0fffffff>;

		rtl-used = <0x0500ff00 0x00800ff0 0x00000000>;
		rtl-visible = <0x0500ff00 0x00000ff0 0x00000000>;
		rtl-green = <0x00000000 0x00000ff0 0x00000000>;
		rtl-except = <0x00000000 0x00800000 0x00000000>;
		except-55 = "green:profinet";

*/

static int rtl8380_led_conf(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node;
	int i;
	u32 *cfg;

	// Hardcoded configuration for TSW202 device
	//						Register   Value       Mask
	u32 cfg_entries[] = {	0x1004, 0x00000000, 0x00000003,
							0xa000, 0x0000001b, 0x000001bf,
							0xa004, 0x000000e0, 0x0fffffff,
							0xa008, 0x0f00ff00, 0x0fffffff,
							0xa010, 0x0500ff00, 0x0fffffff,
							0xa014, 0x0800ff00, 0x0fffffff,
							0xa018, 0x00000000, 0x0fffffff
						};
	u32 rtl_used[] = { 0x0500ff00, 0x00800ff0, 0x00000000 };
	u32 rtl_visible[] = { 0x0500ff00, 0x00000ff0, 0x00000000 };
	u32 rtl_green[] = { 0x00000000, 0x00000ff0, 0x00000000 };
	u32 rtl_except[] = { 0x00000000, 0x00800000, 0x00000000 };

	node = dev->of_node;
	if (!node) {
		dev_err(dev, "missing DTS info\n");
		return -EINVAL;
	}

	bitmap_from_arr32(g_leds.rtl_used, rtl_used, MAX_RTL_LEDS);

	bitmap_from_arr32(g_leds.rtl_visible, rtl_visible, MAX_RTL_LEDS);

	bitmap_from_arr32(g_leds.rtl_green, rtl_green, MAX_RTL_LEDS);

	bitmap_from_arr32(g_leds.rtl_except, rtl_except, MAX_RTL_LEDS);


	mutex_lock(&g_leds.lock);
	for (i = 0; i < ARRAY_SIZE(cfg_entries) / 3; i++) {
		cfg = &cfg_entries[3 * i];
		sw_w32_mask(cfg[2], cfg[1], cfg[0]);
	}
	mutex_unlock(&g_leds.lock);

	return 0;
}

static int rtl8380_led_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int ret, i, cnt = 0, y_cnt = 0, g_cnt = 0, sfp_cnt = 0;

	struct device_node *node;

	node = dev->of_node;
	if (!node) {
		dev_err(dev, "missing DTS info\n");
		return -EINVAL;
	}

	ret = rtl8380_led_conf(pdev);
	if (ret) {
		dev_err(dev, "Failed to get led configuration: %d", ret);
		return ret;
	}

	g_leds.rtl_cdevs = devm_kcalloc(dev, MAX_RTL_LEDS, sizeof(struct led_classdev), GFP_KERNEL);
	if (!g_leds.rtl_cdevs)
		return -ENOMEM;

	for (i = 0; i < MAX_RTL_LEDS; i++) {
		struct led_classdev *cdev = &g_leds.rtl_cdevs[i];
		char *name;
		int base, pin;

		if (!test_bit(i, g_leds.rtl_used)) {
			pr_debug("%s:%d skiping %d bit\n", __func__, __LINE__, i);
			continue;
		}

		base = i / MAX_LEDS_IN_GROUP;
		pin  = i % MAX_LEDS_IN_GROUP;

		if (test_bit(i, g_leds.rtl_except)) {
			// except-55 = "green:profinet"
			name = devm_kasprintf(dev, GFP_KERNEL, "green:profinet");

		} else if (pin < RTL_ETH_LEDS) {
			name = devm_kasprintf(dev, GFP_KERNEL, "eth_%s_%d",
					      test_bit(i, g_leds.rtl_green) ? "g" : "y",
					      test_bit(i, g_leds.rtl_green) ? ++g_cnt : ++y_cnt);
		} else {
			name = devm_kasprintf(dev, GFP_KERNEL, "sfp_y_%d", ++sfp_cnt);
			set_bit(i, g_leds.rtl_visible);
		}
		if (!name)
			return -ENOMEM;

		cdev->name		       = name;
		cdev->brightness_set_blocking  = rtl_led_set;
		cdev->max_brightness	       = 1;

		ret = devm_led_classdev_register(dev, cdev);
		if (ret)
			dev_err(dev, "Failed to register LED %d (err=%d)\n", i, ret);

		cnt++;
	}

	dev_info(dev, "Register %d RTL leds\n", cnt);

	return 0;
}

static int master_led_probe(struct platform_device *pdev)
{
	int ret;

	g_leds.master.name		      = "master";
	g_leds.master.brightness_set_blocking = master_led_set;
	g_leds.master.blink_set		      = master_led_blink_set;
	g_leds.master.max_brightness	      = 1;
	g_leds.master.default_trigger	      = "hw";
	g_leds.master.blink_brightness	      = LED_FULL;
	g_leds.master.blink_delay_on	      = DEF_DELAY_ON;
	g_leds.master.blink_delay_off	      = DEF_DELAY_OFF;

	ret = devm_led_classdev_register(&pdev->dev, &g_leds.master);
	return ret;
}

static int hw_ctl_activate(struct led_classdev *cdev)
{
	int base, pin, i;

	if (!cdev || !g_leds.rtl_cdevs ||
	    (cdev != &g_leds.master &&
	     (cdev < g_leds.rtl_cdevs || cdev >= g_leds.rtl_cdevs + MAX_RTL_LEDS))) {
		pr_err("This trigger cannot be used on %s\n", cdev ? cdev->name : "(null)");
		return -ENXIO;
	}

	/* Prevent calling brightness_set_blocking after hw control is setup.
	 * I.e. prevent reenabling sw control */
	cancel_work_sync(&cdev->set_brightness_work);

	/* does led_stop_software_blink from led-core.c */
	timer_delete_sync(&cdev->blink_timer);
	cdev->blink_delay_on  = 0;
	cdev->blink_delay_off = 0;
	clear_bit(LED_BLINK_SW, &cdev->work_flags);

	if (cdev->brightness_set_blocking)
		cdev->brightness_set_blocking(cdev, LED_OFF);

	if (cdev == &g_leds.master) {
		for (i = 0; i < MAX_RTL_LEDS; i++) {
			if (!test_bit(i, g_leds.rtl_used) || test_bit(i, g_leds.rtl_except)) {
				continue;
			}

			base = i / MAX_LEDS_IN_GROUP;
			pin  = i % MAX_LEDS_IN_GROUP;

			mutex_lock(&g_leds.lock);
			sw_w32_mask((1 << pin), 0, REG_LED0_SW_P_EN_CTRL + base * 4);
			mutex_unlock(&g_leds.lock);
		}

	} else {
		i    = cdev - g_leds.rtl_cdevs;
		base = i / MAX_LEDS_IN_GROUP;
		pin  = i % MAX_LEDS_IN_GROUP;
		mutex_lock(&g_leds.lock);
		sw_w32_mask((1 << pin), 0, REG_LED0_SW_P_EN_CTRL + base * 4);
		mutex_unlock(&g_leds.lock);
	}

	return 0;
}

static struct led_trigger hw_trigger = {
	.name	  = "hw",
	.activate = hw_ctl_activate,
};

static int tlt_port_leds_probe(struct platform_device *pdev)
{
	int ret;
	struct device *dev = &pdev->dev;
	struct device_node *node;

	node = dev->of_node;
	if (!node) {
		dev_err(dev, "missing DTS info\n");
		return -EINVAL;
	}

	mutex_init(&g_leds.lock);

	g_leds.opposed_leds = of_property_read_bool(node, "opposed-leds");

	ret = rtl8380_led_probe(pdev);
	if (ret) {
		dev_err(dev, "Unrecoverable error while probing RTL port leds %d\n", ret);
		return ret;
	}

	ret = devm_led_trigger_register(dev, &hw_trigger);
	if (ret) {
		dev_err(dev, "Failed to register HW control trigger (err=%d)\n", ret);
		return ret;
	}

	ret = master_led_probe(pdev);
	if (ret) {
		dev_err(dev, "Failed to register master LED (err=%d)\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, &g_leds);
	dev_info(dev, "Probed %s\n", DRV_NAME);

	return 0;
}

static struct of_device_id gpio_array_led_of_match[] = { { .compatible = DRV_NAME }, {} };

MODULE_DEVICE_TABLE(of, gpio_array_led_of_match);

static struct platform_driver gpio_array_led_driver = {
	.driver = {
		.name = DRV_NAME,
		.of_match_table = gpio_array_led_of_match,
	},
	.probe = tlt_port_leds_probe,
};
module_platform_driver(gpio_array_led_driver);

MODULE_AUTHOR("Linas Perkauskas <linas.perkauskas@teltonika.lt>");
MODULE_DESCRIPTION("Module for ETH/SFP port led management for TSW202 devices");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
