/*
 * Mainly by David Woodhouse, somewhat modified by Jordan Crouse
 *
 * Copyright © 2006-2007  Red Hat, Inc.
 * Copyright © 2006-2007  Advanced Micro Devices, Inc.
 * 
 * This program is free software.  You can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 */


#include <linux/kernel.h>
#include <linux/fb.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/i2c-id.h>
#include <linux/pci.h>
#include <linux/vt_kern.h>
#include <linux/pci_ids.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/backlight.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/ctype.h>
#include <linux/reboot.h>
#include <asm/tsc.h>
#include <asm/olpc.h>

#include "olpc_dcon.h"

/* Module definitions */

static int resumeline = 898;
module_param(resumeline, int, 0444);

static int noinit;
module_param(noinit, int, 0444);

/* Default off since it doesn't work on DCON ASIC in B-test OLPC board */
static int useaa = 1;
module_param(useaa, int, 0444);

/* I2C structures */

static struct i2c_driver dcon_driver;
static struct i2c_client *dcon_client;

/* Platform devices */
static struct platform_device *dcon_device;

/* Backlight device */
static struct backlight_device *dcon_bl_dev;

/* Base address of the GPIO registers */
static unsigned long gpio_base;

static struct fb_info *fbinfo;

/* Current source, initialized at probe time */
static int dcon_source;

/* Desired source */
static int dcon_pending;

/* Current output type */
static int dcon_output = DCON_OUTPUT_COLOR;

/* Current sleep status (not yet implemented) */
static int dcon_sleep_val = DCON_ACTIVE;

/* Shadow register for the DCON_REG_MODE register */
static unsigned short dcon_disp_mode;

/* Variables used during switches */
static int dcon_switched;

static DECLARE_WAIT_QUEUE_HEAD(dcon_wait_queue);

static unsigned short normal_i2c[] = { 0x0D, I2C_CLIENT_END };
I2C_CLIENT_INSMOD;

#define dcon_write(reg,val) i2c_smbus_write_word_data(dcon_client,reg,val)
#define dcon_read(reg) i2c_smbus_read_word_data(dcon_client,reg)

/* The current backlight value - this saves us some smbus traffic */
static int bl_val = -1;

/* ===== API functions - these are called by a variety of users ==== */

/* Backlight notes - turning off the backlight enable bit in the DCON
 * doesn't save us any power over just pushing the BL to zero, so we
 * don't use that bit in this code.
 */

static int dcon_get_backlight(void)
{
	if (dcon_client == NULL)
		return 0;

	if (bl_val == -1)
		bl_val = dcon_read(DCON_REG_BRIGHT) & 0x0F;

	return bl_val;
}

static void dcon_set_backlight(int level)
{
	if (dcon_client == NULL)
		return;

	if (bl_val == (level & 0x0F))
		return;

	bl_val = level & 0x0F;
	dcon_write(DCON_REG_BRIGHT, bl_val);

	/* Purposely turn off the backlight when we go to level 0 */

	if (bl_val == 0) {
	  dcon_disp_mode &= ~MODE_BL_ENABLE;
	  dcon_write(DCON_REG_MODE, dcon_disp_mode);
	}
	else if (!(dcon_disp_mode & MODE_BL_ENABLE)) {
	  dcon_disp_mode |= MODE_BL_ENABLE;
	  dcon_write(DCON_REG_MODE, dcon_disp_mode);
	}
}

/* Set the output type to either color or mono */

static int dcon_set_output(int arg)
{
	if (dcon_output == arg)
		return 0;

	dcon_output = arg;

	if (arg == DCON_OUTPUT_MONO) {
		dcon_disp_mode &= ~(MODE_CSWIZZLE | MODE_COL_AA);
		dcon_disp_mode |= MODE_MONO_LUMA;
	}
	else {
		dcon_disp_mode &= ~(MODE_MONO_LUMA);
		dcon_disp_mode |= MODE_CSWIZZLE;
		if (useaa)
			dcon_disp_mode |= MODE_COL_AA;
	}

	dcon_write(DCON_REG_MODE, dcon_disp_mode);
	return 0;
}

/* For now, this will be really stupid - we need to address how
 * DCONLOAD works in a sleep and account for it accordingly
 */

static void dcon_sleep(int state)
{
	/* Turn off the backlight and put the DCON to sleep */

	if (state == dcon_sleep_val)
		return;

	if (state == DCON_SLEEP) {
		dcon_disp_mode &= ~MODE_BL_ENABLE;
		dcon_disp_mode |= MODE_SLEEP;
	}
	else {
		/* Only re-enable the backlight if the backlight value is set */

		if (bl_val != 0)
			dcon_disp_mode |= MODE_BL_ENABLE;

		dcon_disp_mode &= ~MODE_SLEEP;
	}

	dcon_sleep_val = state;
	dcon_write(DCON_REG_MODE, dcon_disp_mode);

	/* We should turn off some stuff in the framebuffer - but what? */
}

/* Set the source of the display (CPU or DCON) */

static void dcon_source_switch(struct work_struct *work)
{
	DECLARE_WAITQUEUE(wait, current);
	int source = dcon_pending;

	if (dcon_source == source)
		return;

	dcon_switched = 0;

	switch (source) {
	case DCON_SOURCE_CPU:

		/* Enable the scanline interrupt bit */
		if (dcon_write(DCON_REG_MODE, dcon_disp_mode | MODE_SCAN_INT))
			printk(KERN_ERR "olpc-dcon:  couldn't enable scanline interrupt!\n");
		else {
			/* Wait up to one second for the scanline interrupt */
			wait_event_timeout(dcon_wait_queue, dcon_switched == 1, HZ);
		}

		if (!dcon_switched)
			printk(KERN_ERR "olpc-dcon:  Timeout entering CPU mode; expect a screen glitch.\n");

		/*
		 * Ideally we'd like to disable interrupts here so that the
		 * fb_powerup and DCON turn on happen at a known time value;
		 * however, we can't do that right now with fb_set_suspend
		 * messing with semaphores.
		 *
		 * For now, we just hope..
		 */
		if (fb_powerup(fbinfo)) {
			printk(KERN_ERR "olpc-dcon:  Failed to enter CPU mode\n");
			dcon_pending = DCON_SOURCE_DCON;
			return;
		}

		/* And turn off the DCON */
		outl(1<<11, gpio_base + GPIOx_OUT_VAL);

		/* Turn off the scanline interrupt */
		if (dcon_write(DCON_REG_MODE, dcon_disp_mode))
			printk(KERN_ERR "olpc-dcon:  couldn't disable scanline interrupt!\n");

		printk(KERN_INFO "olpc-dcon: The CPU has control\n");
		break;
	case DCON_SOURCE_DCON:
	{
		int t;

		add_wait_queue(&dcon_wait_queue, &wait);
		set_current_state(TASK_UNINTERRUPTIBLE);

		/* Clear GPIO11 (DCONLOAD) - this implies that the DCON is in
		   control */

		outl(1 << (11 + 16), gpio_base + GPIOx_OUT_VAL);

		t = schedule_timeout(HZ/2);
		remove_wait_queue(&dcon_wait_queue, &wait);
		set_current_state(TASK_RUNNING);

		if (!dcon_switched)
			printk(KERN_ERR "olpc-dcon: Timeout entering DCON mode; expect a screen glitch.\n");

		/* Turn off the graphics engine completely */
		fb_powerdown(fbinfo);

		printk(KERN_INFO "olpc-dcon: The DCON has control\n");
		break;
	}
	default:
		BUG();
	}

	dcon_source = source;
}

static DECLARE_WORK(dcon_work, dcon_source_switch);

static int dcon_set_source(int arg)
{
	if (arg != DCON_SOURCE_CPU && arg != DCON_SOURCE_DCON)
		return -EINVAL;

	if (dcon_pending == arg)
		return 0;

	dcon_pending = arg;
	if ((dcon_source != arg) && !work_pending(&dcon_work))
		schedule_work(&dcon_work);

	return 0;
}

static int dcon_set_source_sync(int arg)
{
	int ret = dcon_set_source(arg);
	if (!ret)
		flush_scheduled_work();
	return ret;
}

static int dconbl_set(struct backlight_device *dev) {

	int level = dev->props.brightness;

	if (dev->props.power != FB_BLANK_UNBLANK)
		level = 0;

	dcon_set_backlight(level);
	return 0;
}

static int dconbl_get(struct backlight_device *dev) {
	return dcon_get_backlight();
}

static ssize_t dcon_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%4.4X\n", dcon_disp_mode);
}

static ssize_t dcon_sleep_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", dcon_sleep_val);
}

static ssize_t /* __deprecated */ dcon_source_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	printk(KERN_WARNING "olpc-dcon:  using deprecated sysfs 'source' interface; use 'freeze' instead!\n");
	return sprintf(buf, "%d\n", dcon_source);
}

static ssize_t dcon_freeze_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", dcon_source == DCON_SOURCE_DCON ? 1 : 0);
}

static ssize_t dcon_output_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", dcon_output);
}

static ssize_t dcon_resumeline_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", resumeline);
}

static int _strtoul(const char *buf, int len, unsigned int *val)
{

	char *endp;
	unsigned int output = simple_strtoul(buf, &endp, 0);
	int size = endp - buf;

	if (*endp && isspace(*endp))
		size++;

	if (size != len)
		return -EINVAL;

	*val = output;
	return 0;
}

static ssize_t dcon_output_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int output;
	int rc = -EINVAL;

	if (_strtoul(buf, count, &output))
		return -EINVAL;

	if (output == DCON_OUTPUT_COLOR || output == DCON_OUTPUT_MONO) {
		dcon_set_output(output);
		rc = count;
	}

	return rc;
}

static ssize_t /* __deprecated */ dcon_source_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int output;
	int rc = -EINVAL;

	printk(KERN_WARNING "olpc-dcon:  using deprecated sysfs 'source' interface; use 'freeze' instead!\n");
	if (_strtoul(buf, count, &output))
		return -EINVAL;

	dcon_set_source(output);
	rc = count;

	return rc;
}

static ssize_t dcon_freeze_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int output;
	int rc = -EINVAL;

	if (_strtoul(buf, count, &output))
		return rc;

	dcon_set_source(output ? DCON_SOURCE_DCON : DCON_SOURCE_CPU);
	rc = count;

	return rc;
}

static ssize_t dcon_resumeline_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int rl;
	int rc = -EINVAL;

	if (_strtoul(buf, count, &rl))
		return rc;

	resumeline = rl;
	dcon_write(DCON_REG_SCAN_INT, resumeline);
	rc = count;

	return rc;
}

static ssize_t dcon_sleep_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int output;

	if (_strtoul(buf, count, &output))
		return -EINVAL;

	dcon_sleep(output ? DCON_SLEEP : DCON_ACTIVE);
	return count;
}

static struct device_attribute dcon_device_files[] = {
	__ATTR(mode, 0444, dcon_mode_show, NULL),
	__ATTR(sleep, 0644, dcon_sleep_show, dcon_sleep_store),
	__ATTR(source, 0644, dcon_source_show, dcon_source_store),
	__ATTR(freeze, 0644, dcon_freeze_show, dcon_freeze_store),
	__ATTR(output, 0644, dcon_output_show, dcon_output_store),
	__ATTR(resumeline, 0644, dcon_resumeline_show, dcon_resumeline_store),
};

static struct backlight_ops dcon_bl_ops = {
	.get_brightness = dconbl_get,
	.update_status = dconbl_set
};

/* List of GPIOs that we care about:
   (in)  GPIO12   -- DCONBLNK
   (in)  GPIO[56] -- DCONSTAT[01]
   (out) GPIO11   -- DCONLOAD
*/

#define IN_GPIOS ((1<<5) | (1<<6) | (1<<7) | (1<<12))
#define OUT_GPIOS (1<<11)

static irqreturn_t dcon_interrupt(int, void *);

static int dcon_request_irq(void)
{
	unsigned long lo, hi;
	unsigned char lob;

	rdmsr(MSR_LBAR_GPIO, lo, hi);

	/* Check the mask and whether GPIO is enabled (sanity check) */
	if (hi != 0x0000f001) {
		printk(KERN_ERR "GPIO not enabled -- cannot use DCON\n");
		return -ENODEV;
	}

	/* Mask off the IO base address */
	gpio_base = lo & 0x0000ff00;

	/* Turn off the event enable for GPIO7 just to be safe */
	outl(1 << (16+7), gpio_base + GPIOx_EVNT_EN);

	/* Set the directions for the GPIO pins */
	outl(OUT_GPIOS | (IN_GPIOS << 16), gpio_base + GPIOx_OUT_EN);
	outl(IN_GPIOS | (OUT_GPIOS << 16), gpio_base + GPIOx_IN_EN);

	/* Set up the interrupt mappings */

	/* Set the IRQ to pair 2 */
	geode_gpio_event_irq(OLPC_GPIO_DCON_IRQ, 2);

	/* Enable group 2 to trigger the DCON interrupt */
	geode_gpio_set_irq(2, DCON_IRQ);

	/* Select edge level for interrupt (in PIC) */

	lob = inb(0x4d0);
	lob &= ~(1 << DCON_IRQ);
	outb(lob, 0x4d0);

	/* Register the interupt handler */
	if (request_irq(DCON_IRQ, &dcon_interrupt, 0, "DCON", &dcon_driver))
		return -EIO;

	/* Clear INV_EN for GPIO7 (DCONIRQ) */
	outl((1<<(16+7)), gpio_base + GPIOx_INV_EN);

	/* Enable filter for GPIO12 (DCONBLANK) */
	outl(1<<(12), gpio_base + GPIOx_IN_FLTR_EN);

	/* Disable filter for GPIO7 */
	outl(1<<(16+7), gpio_base + GPIOx_IN_FLTR_EN);

	/* Disable event counter for GPIO7 (DCONIRQ) and GPIO12 (DCONBLANK) */

	outl(1<<(16+7), gpio_base + GPIOx_EVNTCNT_EN);
	outl(1<<(16+12), gpio_base + GPIOx_EVNTCNT_EN);

	/* Add GPIO12 to the Filter Event Pair #7 */
	outb(12, gpio_base + GPIO_FE7_SEL);

	/* Turn off negative Edge Enable for GPIO12 */
	outl(1<<(16+12), gpio_base + GPIOx_NEGEDGE_EN);

	/* Enable negative Edge Enable for GPIO7 */
	outl(1<<7, gpio_base + GPIOx_NEGEDGE_EN);

	/* Zero the filter amount for Filter Event Pair #7 */
	outw(0, gpio_base + GPIO_FLT7_AMNT);

	/* Clear the negative edge status for GPIO7 and GPIO12 */
	outl((1<<7) | (1<<12), gpio_base+0x4c);

	/* FIXME:  Clear the posiitive status as well, just to be sure */
	outl((1<<7) | (1<<12), gpio_base+0x48);

	/* Enable events for GPIO7 (DCONIRQ) and GPIO12 (DCONBLANK) */
	outl((1<<(7))|(1<<12), gpio_base + GPIOx_EVNT_EN);

	/* Determine the current state by reading the GPIO bit */
	/* Earlier stages of the boot process have established the state */
	dcon_source = inl(gpio_base + GPIOx_OUT_VAL) & (1<<11)
		? DCON_SOURCE_CPU
		: DCON_SOURCE_DCON;
	dcon_pending = dcon_source;

	return 0;
}

static int dcon_reboot_notify(struct notifier_block *nb, unsigned long foo, void *bar)
{
	if (dcon_client == NULL)
		return 0;

	/* Turn off the DCON. Entirely. */
	dcon_write(DCON_REG_MODE, 0x39);
	dcon_write(DCON_REG_MODE, 0x32);
	return 0;
}

static int dcon_conswitch_notify(struct notifier_block *nb,
				 unsigned long mode, void *dummy)
{
	if (mode == CONSOLE_EVENT_SWITCH_TEXT)
		dcon_sleep(DCON_ACTIVE);

	return 0;
}

static struct notifier_block dcon_nb = {
	.notifier_call = dcon_reboot_notify,
	.priority = -1,
};

static struct notifier_block dcon_console_nb = {
	.notifier_call = dcon_conswitch_notify,
	.priority = -1,
};

static int dcon_probe(struct i2c_adapter *adap, int addr, int kind)
{
	struct i2c_client *client;
	uint16_t ver;
	int rc, i;

	if (!olpc_has_dcon()) {
		printk("olpc-dcon:  No DCON is attached.\n");
		return -ENODEV;
	}

	if (num_registered_fb >= 1)
		fbinfo = registered_fb[0];

	if (adap->id != I2C_HW_SMBUS_SCX200) {
		printk(KERN_ERR "olpc-dcon: Invalid I2C bus (%d not %d)\n",
		       adap->id, I2C_HW_SMBUS_SCX200);
		return -ENXIO;
	}

	client = kzalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == NULL)
		return -ENOMEM;

	strncpy(client->name, "OLPC-DCON", I2C_NAME_SIZE);
	client->addr = addr;
	client->adapter = adap;
	client->driver = &dcon_driver;

	if ((rc = i2c_attach_client(client)) != 0) {
		printk(KERN_ERR "olpc-dcon: Unable to attach the I2C client.\n");
		goto eclient;
	}

	ver = i2c_smbus_read_word_data(client, DCON_REG_ID);

	if ((ver >> 8) != 0xDC) {
		printk(KERN_ERR "olpc-dcon: DCON ID not 0xDCxx: 0x%04x instead.\n", ver);
		rc = -ENXIO;
		goto ei2c;
	}

	if ((rc = dcon_request_irq())) {
		printk(KERN_ERR "olpc-dcon: Unable to grab IRQ.\n");
		goto ei2c;
	}

	if (ver < 0xdc02 && !noinit) {
		/* Initialize the DCON registers */

		/* Start with work-arounds for DCON ASIC */
		i2c_smbus_write_word_data(client, 0x4b, 0x00cc);
		i2c_smbus_write_word_data(client, 0x4b, 0x00cc);
		i2c_smbus_write_word_data(client, 0x4b, 0x00cc);
		i2c_smbus_write_word_data(client, 0x0b, 0x007a);
		i2c_smbus_write_word_data(client, 0x36, 0x025c);
		i2c_smbus_write_word_data(client, 0x37, 0x025e);
		
		/* Initialise SDRAM */

		i2c_smbus_write_word_data(client, 0x3b, 0x002b);
		i2c_smbus_write_word_data(client, 0x41, 0x0101);
		i2c_smbus_write_word_data(client, 0x42, 0x0101);
	}

	/* Colour swizzle, AA, no passthrough, backlight */

	dcon_disp_mode = MODE_PASSTHRU | MODE_BL_ENABLE | MODE_CSWIZZLE;
	if (useaa)
		dcon_disp_mode |= MODE_COL_AA;
	i2c_smbus_write_word_data(client, DCON_REG_MODE, dcon_disp_mode);


	/* Set the scanline to interrupt on during resume */

	i2c_smbus_write_word_data(client, DCON_REG_SCAN_INT, resumeline);

	/* Add the DCON device */

	dcon_device = platform_device_alloc("dcon", -1);

	if (dcon_device == NULL) {
		printk(KERN_ERR "dcon:  Unable to create the DCON device\n");
		rc = -ENOMEM;
		goto eirq;
	}

	if ((rc = platform_device_add(dcon_device))) {
		printk(KERN_ERR "dcon:  Unable to add the DCON device\n");
		goto edev;
	}

	for(i = 0; i < ARRAY_SIZE(dcon_device_files); i++)
		device_create_file(&dcon_device->dev, &dcon_device_files[i]);

	/* Add the backlight device for the DCON */

	dcon_client = client;

	dcon_bl_dev = backlight_device_register("dcon-bl", &dcon_device->dev,
		NULL, &dcon_bl_ops);

	if (IS_ERR(dcon_bl_dev)) {
		printk(KERN_INFO "Could not register the backlight device for the DCON (%ld)\n", PTR_ERR(dcon_bl_dev));
		dcon_bl_dev = NULL;
	}
	else {
		dcon_bl_dev->props.max_brightness = 15;
		dcon_bl_dev->props.power = FB_BLANK_UNBLANK;
		dcon_bl_dev->props.brightness = dcon_get_backlight();

		backlight_update_status(dcon_bl_dev);
	}

	register_reboot_notifier(&dcon_nb);
	console_event_register(&dcon_console_nb);

	printk(KERN_INFO "olpc-dcon: Discovered DCON version %x\n", ver & 0xFF);

	return 0;

 edev:
	platform_device_unregister(dcon_device);
	dcon_device = NULL;
 eirq:
	free_irq(DCON_IRQ, &dcon_driver);
 ei2c:
	i2c_detach_client(client);
 eclient:
	kfree(client);

	return rc;
}

static int dcon_attach(struct i2c_adapter *adap)
{
	int ret;

	ret = i2c_probe(adap, &addr_data, dcon_probe);

	if (dcon_client == NULL)
		printk(KERN_ERR "olpc-dcon: No DCON found on SMBus\n");

	return ret;
}

static int dcon_detach(struct i2c_client *client)
{
	int rc;
	dcon_client = NULL;

	unregister_reboot_notifier(&dcon_nb);
	console_event_unregister(&dcon_console_nb);

	free_irq(DCON_IRQ, &dcon_driver);

	if ((rc = i2c_detach_client(client)) == 0)
		kfree(i2c_get_clientdata(client));

	if (dcon_bl_dev != NULL)
		backlight_device_unregister(dcon_bl_dev);

	if (dcon_device != NULL)
		platform_device_unregister(dcon_device);
	cancel_work_sync(&dcon_work);

	return rc;
}


#ifdef CONFIG_PM
static int dcon_suspend(struct i2c_client *client, pm_message_t state)
{
	if (dcon_sleep_val != DCON_ACTIVE)
		return 0;

	/* Set up the DCON to have the source */
	return dcon_set_source_sync(DCON_SOURCE_DCON);
}

static int dcon_resume(struct i2c_client *client)
{
	int x;
	if (dcon_sleep_val != DCON_ACTIVE)
		return 0;

	/* HACK: ensure the bus is stable */
	do {
		x = dcon_read(DCON_REG_ID);
	} while (x < 0);

	return dcon_set_source(DCON_SOURCE_CPU);
}

#endif

static irqreturn_t dcon_interrupt(int irq, void *id)
{
	int status = inl(gpio_base + GPIOx_READ_BACK) >> 5;

	/* Clear the negative edge status for GPIO7 */
	outl(1 << 7, gpio_base + GPIOx_NEGEDGE_STS);

	switch (status & 3) {
	case 3:
		printk(KERN_DEBUG "olpc-dcon: DCONLOAD_MISSED interrupt\n");
		break;
	case 2:	/* switch to DCON mode */
	case 1: /* switch to CPU mode */
		dcon_switched = 1;
		wake_up(&dcon_wait_queue);
		break;
	case 0:
		printk(KERN_DEBUG "olpc-dcon: scanline interrupt w/CPU\n");
	}

	return IRQ_HANDLED;
}

static struct i2c_driver dcon_driver = {
	.driver = {
		.name	= "OLPC-DCON",
	},
	.id = I2C_DRIVERID_DCON,
	.attach_adapter = dcon_attach,
	.detach_client = dcon_detach,
#ifdef CONFIG_PM
	.suspend = dcon_suspend,
	.resume = dcon_resume,
#endif
};


static int __init olpc_dcon_init(void)
{
	i2c_add_driver(&dcon_driver);
	return 0;
}

static void __exit olpc_dcon_exit(void)
{
	i2c_del_driver(&dcon_driver);
}

module_init(olpc_dcon_init);
module_exit(olpc_dcon_exit);

MODULE_LICENSE("GPL");
