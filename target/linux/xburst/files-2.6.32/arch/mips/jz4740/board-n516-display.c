/*
 * board-n516-display.c -- Platform device for N516 display
 *
 * Copyright (C) 2009, Yauhen Kharuzhy <jekhor@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/jz4740_fb.h>

#include <asm/mach-jz4740/platform.h>
#include <asm/mach-jz4740/board-n516.h>

#include <video/metronomefb.h>
#include <linux/console.h>

extern struct platform_device jz_lcd_device;

static struct fb_videomode n516_fb_modes[] = {
	[0] = {
		.name		= "Metronome 800x600",
		.refresh	= 50,
		.xres		= 400,
		.yres		= 624,
		.hsync_len	= 31,
		.vsync_len	= 23,
		.right_margin	= 31,
		.left_margin	= 5,
		.upper_margin	= 1,
		.lower_margin	= 2,
		.sync = FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
	},
};

static struct jz4740_fb_platform_data n516_fb_pdata = {
	.num_modes	= ARRAY_SIZE(n516_fb_modes),
	.modes		= n516_fb_modes,
	.bpp		= 16,
	.lcd_type	= JZ_LCD_TYPE_GENERIC_16_BIT,
};

struct n516_board_info {
	uint8_t *metromem;
	size_t wfm_size;
	struct fb_info *host_fbinfo; /* the host LCD controller's fbi */
	unsigned int fw;
	unsigned int fh;
};

static struct platform_device *n516_device;
static struct n516_board_info n516_board_info;

static int metronome_gpios[] = {
	GPIO_DISPLAY_STBY,
	GPIO_DISPLAY_RST_L,
	GPIO_DISPLAY_RDY,
	GPIO_DISPLAY_ERR,
/*	GPIO_DISPLAY_OFF,*/
};

static const char *metronome_gpio_names[] = {
	"Metronome STDBY",
	"Metronome RST",
	"Metronome RDY",
	"Metronome ERR",
/*	"Metronone OFF",*/
};

static int n516_enable_hostfb(bool enable)
{
	int ret;
	int blank = enable ? FB_BLANK_UNBLANK : FB_BLANK_POWERDOWN;

	acquire_console_sem();
	ret = fb_blank(n516_board_info.host_fbinfo, blank);
	release_console_sem();

	return ret;
}

static int n516_init_metronome_gpios(struct metronomefb_par *par)
{
	int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(metronome_gpios); ++i) {
		ret = gpio_request(metronome_gpios[i], metronome_gpio_names[i]);
		if (ret)
			goto err;
	}

	gpio_direction_output(GPIO_DISPLAY_OFF, 0);
	gpio_direction_output(GPIO_DISPLAY_RST_L, 0);
	gpio_direction_output(GPIO_DISPLAY_STBY, 0);
	gpio_direction_input(GPIO_DISPLAY_RDY);
	gpio_direction_input(GPIO_DISPLAY_ERR);

	return 0;
err:
	for (--i; i >= 0; --i)
		gpio_free(metronome_gpios[i]);

	return ret;
}

static int n516_share_video_mem(struct fb_info *info)
{
	int ret;

	dev_dbg(&n516_device->dev, "ENTER %s\n", __func__);
	dev_dbg(&n516_device->dev, "%s, info->var.xres = %u, info->var.yres = %u\n", __func__, info->var.xres, info->var.yres);
	/* rough check if this is our desired fb and not something else */
	if ((info->var.xres != n516_fb_pdata.modes[0].xres)
		|| (info->var.yres != n516_fb_pdata.modes[0].yres))
		return 0;

	/* we've now been notified that we have our new fb */
	n516_board_info.metromem = info->screen_base;
	n516_board_info.host_fbinfo = info;

	n516_enable_hostfb(false);
	/* try to refcount host drv since we are the consumer after this */
	if (!try_module_get(info->fbops->owner))
		return -ENODEV;

	/* this _add binds metronomefb to n516. metronomefb refcounts n516 */
	ret = platform_device_add(n516_device);

	if (ret) {
		platform_device_put(n516_device);
		return ret;
	}

	/* request our platform independent driver */
	request_module("metronomefb");

	return 0;
}

static int n516_unshare_video_mem(struct fb_info *info)
{
	dev_dbg(&n516_device->dev, "ENTER %s\n", __func__);

	if (info != n516_board_info.host_fbinfo)
		return 0;

	module_put(n516_board_info.host_fbinfo->fbops->owner);
	return 0;
}

static int n516_fb_notifier_callback(struct notifier_block *self,
				 unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	struct fb_info *info = evdata->info;

	dev_dbg(&n516_device->dev, "ENTER %s\n", __func__);

	if (event == FB_EVENT_FB_REGISTERED)
		return n516_share_video_mem(info);
	else if (event == FB_EVENT_FB_UNREGISTERED)
		return n516_unshare_video_mem(info);

	return 0;
}

static struct notifier_block n516_fb_notif = {
	.notifier_call = n516_fb_notifier_callback,
};

/* this gets called as part of our init. these steps must be done now so
 * that we can use set_pxa_fb_info */
static void __init n516_presetup_fb(void)
{
	int padding_size;
	int totalsize;

	/* the frame buffer is divided as follows:
	command | CRC | padding
	16kb waveform data | CRC | padding
	image data | CRC
	*/

	n516_board_info.fw = 800;
	n516_board_info.fh = 624;

	/* waveform must be 16k + 2 for checksum */
	n516_board_info.wfm_size = roundup(16*1024 + 2, n516_board_info.fw);

	padding_size = PAGE_SIZE + (4 * n516_board_info.fw);

	/* total is 1 cmd , 1 wfm, padding and image */
	totalsize = n516_board_info.fw + n516_board_info.wfm_size;
	totalsize += padding_size + (n516_board_info.fw*n516_board_info.fh);

	/* save this off because we're manipulating fw after this and
	 * we'll need it when we're ready to setup the framebuffer */

	/* the reason we do this adjustment is because we want to acquire
	 * more framebuffer memory without imposing custom awareness on the
	 * underlying driver */
	n516_fb_pdata.modes[0].yres = DIV_ROUND_UP(totalsize, n516_board_info.fw);

	jz4740_framebuffer_device.dev.platform_data = &n516_fb_pdata;
	platform_device_register(&jz4740_framebuffer_device);
}

/* this gets called by metronomefb as part of its init, in our case, we
 * have already completed initial framebuffer init in presetup_fb so we
 * can just setup the fb access pointers */
static int n516_setup_fb(struct metronomefb_par *par)
{
	/* metromem was set up by the notifier in share_video_mem so now
	 * we can use its value to calculate the other entries */
	par->metromem_cmd = (struct metromem_cmd *) n516_board_info.metromem;
	par->metromem_wfm = n516_board_info.metromem + n516_board_info.fw;
	par->metromem_img = par->metromem_wfm + n516_board_info.wfm_size;
	par->metromem_img_csum = (u16 *) (par->metromem_img + (n516_board_info.fw * n516_board_info.fh));
	par->metromem_dma = n516_board_info.host_fbinfo->fix.smem_start;

	return 0;
}

static int n516_get_panel_type(void)
{
	return 5;
}

static irqreturn_t n516_handle_irq(int irq, void *dev_id)
{
	struct metronomefb_par *par = dev_id;

	dev_dbg(&par->pdev->dev, "Metronome IRQ! RDY=%d\n", 	gpio_get_value(GPIO_DISPLAY_RDY));
	wake_up_all(&par->waitq);

	return IRQ_HANDLED;
}

static void n516_power_ctl(struct metronomefb_par *par, int cmd)
{
	switch (cmd) {
	case METRONOME_POWER_OFF:
		gpio_set_value(GPIO_DISPLAY_OFF, 1);
		n516_enable_hostfb(false);
		break;
	case METRONOME_POWER_ON:
		gpio_set_value(GPIO_DISPLAY_OFF, 0);
		n516_enable_hostfb(true);
		break;
	}
}

static int n516_get_rdy(struct metronomefb_par *par)
{
	return gpio_get_value(GPIO_DISPLAY_RDY);
}

static int n516_get_err(struct metronomefb_par *par)
{
	return gpio_get_value(GPIO_DISPLAY_ERR);
}

static int n516_setup_irq(struct fb_info *info)
{
	int ret;

	dev_dbg(&n516_device->dev, "ENTER %s\n", __func__);

	ret = request_irq(gpio_to_irq(GPIO_DISPLAY_RDY), n516_handle_irq,
				IRQF_TRIGGER_RISING,
				"n516", info->par);
	if (ret)
		dev_err(&n516_device->dev, "request_irq failed: %d\n", ret);

	return ret;
}

static void n516_set_rst(struct metronomefb_par *par, int state)
{
	dev_dbg(&n516_device->dev, "ENTER %s, RDY=%d\n", __func__, gpio_get_value(GPIO_DISPLAY_RDY));
	if (state)
		gpio_set_value(GPIO_DISPLAY_RST_L, 1);
	else
		gpio_set_value(GPIO_DISPLAY_RST_L, 0);
}

static void n516_set_stdby(struct metronomefb_par *par, int state)
{
	dev_dbg(&n516_device->dev, "ENTER %s, RDY=%d\n", __func__, gpio_get_value(GPIO_DISPLAY_RDY));
	if (state)
		gpio_set_value(GPIO_DISPLAY_STBY, 1);
	else
		gpio_set_value(GPIO_DISPLAY_STBY, 0);
}

static int n516_wait_event(struct metronomefb_par *par)
{
	unsigned long timeout = jiffies + HZ / 20;

	dev_dbg(&n516_device->dev, "ENTER1 %s, RDY=%d\n",
			__func__, gpio_get_value(GPIO_DISPLAY_RDY));
	while (n516_get_rdy(par) && time_before(jiffies, timeout))
		schedule();

	dev_dbg(&n516_device->dev, "ENTER2 %s, RDY=%d\n",
			__func__, gpio_get_value(GPIO_DISPLAY_RDY));
	return wait_event_timeout(par->waitq,
			n516_get_rdy(par), HZ * 2) ? 0 : -EIO;
}

static int n516_wait_event_intr(struct metronomefb_par *par)
{
	unsigned long timeout = jiffies + HZ/20;

	dev_dbg(&n516_device->dev, "ENTER1 %s, RDY=%d\n",
			__func__, gpio_get_value(GPIO_DISPLAY_RDY));
	while (n516_get_rdy(par) && time_before(jiffies, timeout))
		schedule();

	dev_dbg(&n516_device->dev, "ENTER2 %s, RDY=%d\n",
			__func__, gpio_get_value(GPIO_DISPLAY_RDY));
	return wait_event_interruptible_timeout(par->waitq,
					n516_get_rdy(par), HZ * 2) ? 0 : -EIO;
}

static void n516_cleanup(struct metronomefb_par *par)
{
	int i;

	free_irq(gpio_to_irq(GPIO_DISPLAY_RDY), par);
	for (i = 0; i < ARRAY_SIZE(metronome_gpios); ++i)
		gpio_free(metronome_gpios[i]);
}

static struct metronome_board n516_board __initdata = {
	.owner			= THIS_MODULE,
	.power_ctl		= n516_power_ctl,
	.setup_irq		= n516_setup_irq,
	.setup_io		= n516_init_metronome_gpios,
	.setup_fb		= n516_setup_fb,
	.set_rst		= n516_set_rst,
	.get_err		= n516_get_err,
	.get_rdy		= n516_get_rdy,
	.set_stdby		= n516_set_stdby,
	.met_wait_event		= n516_wait_event,
	.met_wait_event_intr	= n516_wait_event_intr,
	.get_panel_type		= n516_get_panel_type,
	.cleanup		= n516_cleanup,
};

static int __init n516_init(void)
{
	int ret;

	/* Keep the metronome off, until its driver is loaded */
	ret = gpio_request(GPIO_DISPLAY_OFF, "Display off");
	if (ret)
		return ret;

	gpio_direction_output(GPIO_DISPLAY_OFF, 1);

	/* before anything else, we request notification for any fb
	 * creation events */
	fb_register_client(&n516_fb_notif);

	n516_device = platform_device_alloc("metronomefb", -1);
	if (!n516_device)
		return -ENOMEM;

	/* the n516_board that will be seen by metronomefb is a copy */
	platform_device_add_data(n516_device, &n516_board,
					sizeof(n516_board));

	n516_presetup_fb();

	return 0;
}
module_init(n516_init);

MODULE_DESCRIPTION("board driver for n516 display");
MODULE_AUTHOR("Yauhen Kharuzhy");
MODULE_LICENSE("GPL");
