/* Smedia Glamo 336x/337x driver
 *
 * (C) 2007-2008 by Openmoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/console.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/mfd/glamo.h>

#include <asm/div64.h>

#ifdef CONFIG_PM
#include <linux/pm.h>
#endif

#include <linux/glamofb.h>

#include "glamo-regs.h"
#include "glamo-core.h"

static void glamofb_program_mode(struct glamofb_handle* glamo);

struct glamofb_handle {
	struct glamo_core *core;
	struct fb_info *fb;
	struct device *dev;
	struct resource *reg;
	struct resource *fb_res;
	char __iomem *base;
	struct glamo_fb_platform_data *mach_info;
	char __iomem *cursor_addr;
	int cursor_on;
	u_int32_t pseudo_pal[16];
	spinlock_t lock_cmd;
	int blank_mode;
	int mode_set; /* 0 if the current display mode hasn't been set on the glamo */
	int output_enabled; /* 0 if the video output is disabled */
};

static void glamo_output_enable(struct glamofb_handle *gfb) {
	struct glamo_core *gcore = gfb->core;

	if (gfb->output_enabled)
		return;

	/* enable the pixel clock if off */
    glamo_engine_enable(gcore, GLAMO_ENGINE_LCD);

	gfb->output_enabled = 1;
	if (!gfb->mode_set)
		glamofb_program_mode(gfb);
}

static void glamo_output_disable(struct glamofb_handle *gfb) {
	struct glamo_core *gcore = gfb->core;

	if (!gfb->output_enabled)
		return;

	/* enable the pixel clock if off */
    glamo_engine_suspend(gcore, GLAMO_ENGINE_LCD);

	gfb->output_enabled = 0;
}


static int reg_read(struct glamofb_handle *glamo,
			   u_int16_t reg)
{
	int i = 0;

	for (i = 0; i != 2; i++)
		nop();

	return readw(glamo->base + reg);
}

static void reg_write(struct glamofb_handle *glamo,
			     u_int16_t reg, u_int16_t val)
{
	int i = 0;

	for (i = 0; i != 2; i++)
		nop();

	writew(val, glamo->base + reg);
}

static struct glamo_script glamo_regs[] = {
	{ GLAMO_REG_LCD_MODE1, 0x0020 },
	/* no display rotation, no hardware cursor, no dither, no gamma,
	 * no retrace flip, vsync low-active, hsync low active,
	 * no TVCLK, no partial display, hw dest color from fb,
	 * no partial display mode, LCD1, software flip,  */
	{ GLAMO_REG_LCD_MODE2, 0x9020 },
	  /* video flip, no ptr, no ptr, dhclk off,
	   * normal mode,  no cpuif,
	   * res, serial msb first, single fb, no fr ctrl,
	   * cpu if bits all zero, no crc
	   * 0000 0000 0010  0000 */
	{ GLAMO_REG_LCD_MODE3, 0x0b40 },
	  /* src data rgb565, res, 18bit rgb666
	   * 000 01 011 0100 0000 */
	{ GLAMO_REG_LCD_POLARITY, 0x440c },
	  /* DE high active, no cpu/lcd if, cs0 force low, a0 low active,
	   * np cpu if, 9bit serial data, sclk rising edge latch data
	   * 01 00 0 100 0 000 01 0 0 */
	/* The following values assume 640*480@16bpp */
	{ GLAMO_REG_LCD_A_BASE1, 0x0000 }, /* display A base address 15:0 */
	{ GLAMO_REG_LCD_A_BASE2, 0x0000 }, /* display A base address 22:16 */
	{ GLAMO_REG_LCD_CURSOR_BASE1, 0xC000 }, /* cursor base address 15:0 */
	{ GLAMO_REG_LCD_CURSOR_BASE2, 0x0012 }, /* cursor base address 22:16 */
	{ GLAMO_REG_LCD_COMMAND2, 0x0000 }, /* display page A */
};

static int glamofb_run_script(struct glamofb_handle *glamo,
				struct glamo_script *script, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		struct glamo_script *line = &script[i];

		if (line->reg == 0xffff)
			return 0;
		else if (line->reg == 0xfffe)
			msleep(line->val);
		else
			reg_write(glamo, script[i].reg, script[i].val);
	}

	return 0;
}

static int glamofb_check_var(struct fb_var_screeninfo *var,
			     struct fb_info *info)
{
	struct glamofb_handle *glamo = info->par;

	if (var->bits_per_pixel != 16)
		var->bits_per_pixel = 16;

	var->height = glamo->mach_info->height;
	var->width = glamo->mach_info->width;

	/* FIXME: set rgb positions */
	switch (var->bits_per_pixel) {
	case 16:
		switch (reg_read(glamo, GLAMO_REG_LCD_MODE3) & 0xc000) {
		case GLAMO_LCD_SRC_RGB565:
			var->red.offset		= 11;
			var->green.offset	= 5;
			var->blue.offset	= 0;
			var->red.length		= 5;
			var->green.length	= 6;
			var->blue.length	= 5;
			var->transp.length	= 0;
			break;
		case GLAMO_LCD_SRC_ARGB1555:
			var->transp.offset	= 15;
			var->red.offset		= 10;
			var->green.offset	= 5;
			var->blue.offset	= 0;
			var->transp.length	= 1;
			var->red.length		= 5;
			var->green.length	= 5;
			var->blue.length	= 5;
			break;
		case GLAMO_LCD_SRC_ARGB4444:
			var->transp.offset	= 12;
			var->red.offset		= 8;
			var->green.offset	= 4;
			var->blue.offset	= 0;
			var->transp.length	= 4;
			var->red.length		= 4;
			var->green.length	= 4;
			var->blue.length	= 4;
			break;
		}
		break;
	case 24:
	case 32:
	default:
		/* The Smedia Glamo doesn't support anything but 16bit color */
		printk(KERN_ERR
		       "Smedia driver does not [yet?] support 24/32bpp\n");
		return -EINVAL;
	}

	return 0;
}

static void reg_set_bit_mask(struct glamofb_handle *glamo,
			     u_int16_t reg, u_int16_t mask,
			     u_int16_t val)
{
	u_int16_t tmp;

	val &= mask;

	tmp = reg_read(glamo, reg);
	tmp &= ~mask;
	tmp |= val;
	reg_write(glamo, reg, tmp);
}

#define GLAMO_LCD_WIDTH_MASK 0x03FF
#define GLAMO_LCD_HEIGHT_MASK 0x03FF
#define GLAMO_LCD_PITCH_MASK 0x07FE
#define GLAMO_LCD_HV_TOTAL_MASK 0x03FF
#define GLAMO_LCD_HV_RETR_START_MASK 0x03FF
#define GLAMO_LCD_HV_RETR_END_MASK 0x03FF
#define GLAMO_LCD_HV_RETR_DISP_START_MASK 0x03FF
#define GLAMO_LCD_HV_RETR_DISP_END_MASK 0x03FF

/* the caller has to enxure lock_cmd is held and we are in cmd mode */
static void __rotate_lcd(struct glamofb_handle *glamo, __u32 rotation)
{
	int glamo_rot;

	switch (rotation) {
	case FB_ROTATE_CW:
		glamo_rot = GLAMO_LCD_ROT_MODE_90;
		break;
	case FB_ROTATE_UD:
		glamo_rot = GLAMO_LCD_ROT_MODE_180;
		break;
	case FB_ROTATE_CCW:
		glamo_rot = GLAMO_LCD_ROT_MODE_270;
		break;
	default:
		glamo_rot = GLAMO_LCD_ROT_MODE_0;
		break;
	}

	reg_set_bit_mask(glamo,
			 GLAMO_REG_LCD_WIDTH,
			 GLAMO_LCD_ROT_MODE_MASK,
			 glamo_rot);
	reg_set_bit_mask(glamo,
			 GLAMO_REG_LCD_MODE1,
			 GLAMO_LCD_MODE1_ROTATE_EN,
			 (glamo_rot != GLAMO_LCD_ROT_MODE_0) ?
				 GLAMO_LCD_MODE1_ROTATE_EN : 0);
}

static void glamofb_program_mode(struct glamofb_handle* gfb) {
	int sync, bp, disp, fp, total;
	unsigned long flags;
	struct glamo_core *gcore = gfb->core;
	struct fb_var_screeninfo *var = &gfb->fb->var;

	dev_dbg(&gcore->pdev->dev,
			  "glamofb_program_mode spin_lock_irqsave\n");
	spin_lock_irqsave(&gfb->lock_cmd, flags);

	if (glamofb_cmd_mode(gfb, 1))
		goto out_unlock;

	if (var->pixclock)
		glamo_engine_reclock(gcore,
				     GLAMO_ENGINE_LCD,
				     (1000000000UL / gfb->fb->var.pixclock) * 1000);

	reg_set_bit_mask(gfb,
			 GLAMO_REG_LCD_WIDTH,
			 GLAMO_LCD_WIDTH_MASK,
			 var->xres);
	reg_set_bit_mask(gfb,
			 GLAMO_REG_LCD_HEIGHT,
			 GLAMO_LCD_HEIGHT_MASK,
			 var->yres);
	reg_set_bit_mask(gfb,
			 GLAMO_REG_LCD_PITCH,
			 GLAMO_LCD_PITCH_MASK,
			 gfb->fb->fix.line_length);

	/* honour the rotation request */
	__rotate_lcd(gfb, var->rotate);

	/* update scannout timings */
	sync = 0;
	bp = sync + var->hsync_len;
	disp = bp + var->left_margin;
	fp = disp + var->xres;
	total = fp + var->right_margin;

	reg_set_bit_mask(gfb, GLAMO_REG_LCD_HORIZ_TOTAL,
			 GLAMO_LCD_HV_TOTAL_MASK, total);
	reg_set_bit_mask(gfb, GLAMO_REG_LCD_HORIZ_RETR_START,
			 GLAMO_LCD_HV_RETR_START_MASK, sync);
	reg_set_bit_mask(gfb, GLAMO_REG_LCD_HORIZ_RETR_END,
			 GLAMO_LCD_HV_RETR_END_MASK, bp);
	reg_set_bit_mask(gfb, GLAMO_REG_LCD_HORIZ_DISP_START,
			  GLAMO_LCD_HV_RETR_DISP_START_MASK, disp);
	reg_set_bit_mask(gfb, GLAMO_REG_LCD_HORIZ_DISP_END,
			 GLAMO_LCD_HV_RETR_DISP_END_MASK, fp);

	sync = 0;
	bp = sync + var->vsync_len;
	disp = bp + var->upper_margin;
	fp = disp + var->yres;
	total = fp + var->lower_margin;

	reg_set_bit_mask(gfb, GLAMO_REG_LCD_VERT_TOTAL,
			 GLAMO_LCD_HV_TOTAL_MASK, total);
	reg_set_bit_mask(gfb, GLAMO_REG_LCD_VERT_RETR_START,
			  GLAMO_LCD_HV_RETR_START_MASK, sync);
	reg_set_bit_mask(gfb, GLAMO_REG_LCD_VERT_RETR_END,
			 GLAMO_LCD_HV_RETR_END_MASK, bp);
	reg_set_bit_mask(gfb, GLAMO_REG_LCD_VERT_DISP_START,
			 GLAMO_LCD_HV_RETR_DISP_START_MASK, disp);
	reg_set_bit_mask(gfb, GLAMO_REG_LCD_VERT_DISP_END,
			 GLAMO_LCD_HV_RETR_DISP_END_MASK, fp);

	glamofb_cmd_mode(gfb, 0);

	gfb->mode_set = 1;

out_unlock:
	dev_dbg(&gcore->pdev->dev,
		      "glamofb_program_mode spin_unlock_irqrestore\n");
	spin_unlock_irqrestore(&gfb->lock_cmd, flags);
}


static int glamofb_pan_display(struct fb_var_screeninfo *var,
		struct fb_info *info)
{
	return 0;
}

static struct fb_videomode *glamofb_find_mode(struct fb_info *info,
        struct fb_var_screeninfo *var) {
	struct glamofb_handle *glamo = info->par;
	struct glamo_fb_platform_data *mach_info = glamo->mach_info;
	struct fb_videomode *mode;
	int i;

	for(i = mach_info->num_modes, mode = mach_info->modes; i > 0; --i, ++mode) {
		if (mode->xres == var->xres &&
		    mode->yres == var->yres)
			return mode;
	}

	return NULL;
}

static int glamofb_set_par(struct fb_info *info)
{
	struct glamofb_handle *glamo = info->par;
	struct fb_var_screeninfo *var = &info->var;
	struct fb_videomode *mode;

	mode = glamofb_find_mode(info, var);
	if (!mode)
		return -EINVAL;

	fb_videomode_to_var(var, mode);

	info->mode = mode;

	glamo->mode_set = 0;

	switch(var->rotate) {
	case FB_ROTATE_CW:
	case FB_ROTATE_CCW:
		info->fix.line_length = (var->yres * var->bits_per_pixel) / 8;
		/* FIXME: Limit pixelclock */
		var->pixclock *= 2;
		break;
	default:
		info->fix.line_length = (var->xres * var->bits_per_pixel) / 8;
		break;
	}

	if(glamo->output_enabled)
		glamofb_program_mode(glamo);

	return 0;
}

static int glamofb_blank(int blank_mode, struct fb_info *info)
{
	struct glamofb_handle *gfb = info->par;

	dev_dbg(gfb->dev, "glamofb_blank(%u)\n", blank_mode);

	switch (blank_mode) {
	case FB_BLANK_VSYNC_SUSPEND:
	case FB_BLANK_HSYNC_SUSPEND:
		/* FIXME: add pdata hook/flag to indicate whether
		 * we should already switch off pixel clock here */
		break;
	case FB_BLANK_POWERDOWN:
		/* disable the pixel clock */
		glamo_output_disable(gfb);
		gfb->blank_mode = blank_mode;
		break;
	case FB_BLANK_UNBLANK:
	case FB_BLANK_NORMAL:
		glamo_output_enable(gfb);
		gfb->blank_mode = blank_mode;
		break;
	}

	/* FIXME: once we have proper clock management in glamo-core,
	 * we can determine if other units need MCLK1 or the PLL, and
	 * disable it if not used. */
	return 0;
}

static inline unsigned int chan_to_field(unsigned int chan,
					 struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static int glamofb_setcolreg(unsigned regno,
			     unsigned red, unsigned green, unsigned blue,
			     unsigned transp, struct fb_info *info)
{
	struct glamofb_handle *glamo = info->par;
	unsigned int val;

	switch (glamo->fb->fix.visual) {
	case FB_VISUAL_TRUECOLOR:
	case FB_VISUAL_DIRECTCOLOR:
		/* true-colour, use pseuo-palette */

		if (regno < 16) {
			u32 *pal = glamo->fb->pseudo_palette;

			val  = chan_to_field(red, &glamo->fb->var.red);
			val |= chan_to_field(green, &glamo->fb->var.green);
			val |= chan_to_field(blue, &glamo->fb->var.blue);

			pal[regno] = val;
		};
		break;
	default:
		return 1; /* unknown type */
	}

	return 0;
}

static int glamofb_ioctl(struct fb_info *info, unsigned int cmd,
                         unsigned long arg) {
	struct glamofb_handle *gfb = (struct glamofb_handle*)info->par;
	struct glamo_core *gcore = gfb->core;
	int retval = -ENOTTY;

	switch (cmd) {
	case GLAMOFB_ENGINE_ENABLE:
		retval = glamo_engine_enable(gcore, arg);
		break;
	case GLAMOFB_ENGINE_DISABLE:
		retval = glamo_engine_disable(gcore, arg);
		break;
	case GLAMOFB_ENGINE_RESET:
		glamo_engine_reset(gcore, arg);
		retval = 0;
		break;
	default:
		break;
	}

	return retval;
}


#ifdef CONFIG_MFD_GLAMO_HWACCEL
static inline void glamofb_vsync_wait(struct glamofb_handle *glamo,
		int line, int size, int range)
{
	int count[2];

	do {
		count[0] = reg_read(glamo, GLAMO_REG_LCD_STATUS2) & 0x3ff;
		count[1] = reg_read(glamo, GLAMO_REG_LCD_STATUS2) & 0x3ff;
	} while (count[0] != count[1] ||
			(line < count[0] + range &&
			 size > count[0] - range) ||
			count[0] < range * 2);
}

/*
 * Enable/disable the hardware cursor mode altogether
 * (for blinking and such, use glamofb_cursor()).
 */
static void glamofb_cursor_onoff(struct glamofb_handle *glamo, int on)
{
	int y, size;

	if (glamo->cursor_on) {
		y = reg_read(glamo, GLAMO_REG_LCD_CURSOR_Y_POS);
		size = reg_read(glamo, GLAMO_REG_LCD_CURSOR_Y_SIZE);

		glamofb_vsync_wait(glamo, y, size, 30);
	}

	reg_set_bit_mask(glamo, GLAMO_REG_LCD_MODE1,
			GLAMO_LCD_MODE1_CURSOR_EN,
			on ? GLAMO_LCD_MODE1_CURSOR_EN : 0);
	glamo->cursor_on = on;

	/* Hide the cursor by default */
	reg_write(glamo, GLAMO_REG_LCD_CURSOR_X_SIZE, 0);
}

static int glamofb_cursor(struct fb_info *info, struct fb_cursor *cursor)
{
	struct glamofb_handle *glamo = info->par;
	unsigned long flags;

	spin_lock_irqsave(&glamo->lock_cmd, flags);

	reg_write(glamo, GLAMO_REG_LCD_CURSOR_X_SIZE,
			cursor->enable ? cursor->image.width : 0);

	if (cursor->set & FB_CUR_SETPOS) {
		reg_write(glamo, GLAMO_REG_LCD_CURSOR_X_POS,
			  cursor->image.dx);
		reg_write(glamo, GLAMO_REG_LCD_CURSOR_Y_POS,
			  cursor->image.dy);
	}

	if (cursor->set & FB_CUR_SETCMAP) {
		uint16_t fg = glamo->pseudo_pal[cursor->image.fg_color];
		uint16_t bg = glamo->pseudo_pal[cursor->image.bg_color];

		reg_write(glamo, GLAMO_REG_LCD_CURSOR_FG_COLOR, fg);
		reg_write(glamo, GLAMO_REG_LCD_CURSOR_BG_COLOR, bg);
		reg_write(glamo, GLAMO_REG_LCD_CURSOR_DST_COLOR, fg);
	}

	if (cursor->set & FB_CUR_SETHOT)
		reg_write(glamo, GLAMO_REG_LCD_CURSOR_PRESET,
				(cursor->hot.x << 8) | cursor->hot.y);

	if ((cursor->set & FB_CUR_SETSIZE) ||
	    (cursor->set & (FB_CUR_SETIMAGE | FB_CUR_SETSHAPE))) {
		int x, y, pitch, op;
		const uint8_t *pcol = cursor->image.data;
		const uint8_t *pmsk = cursor->mask;
		uint8_t __iomem *dst = glamo->cursor_addr;
		uint8_t dcol = 0;
		uint8_t dmsk = 0;
		uint8_t byte = 0;

		if (cursor->image.depth > 1) {
			spin_unlock_irqrestore(&glamo->lock_cmd, flags);
			return -EINVAL;
		}

		pitch = ((cursor->image.width + 7) >> 2) & ~1;
		reg_write(glamo, GLAMO_REG_LCD_CURSOR_PITCH,
			pitch);
		reg_write(glamo, GLAMO_REG_LCD_CURSOR_Y_SIZE,
			cursor->image.height);

		for (y = 0; y < cursor->image.height; y++) {
			byte = 0;
			for (x = 0; x < cursor->image.width; x++) {
				if ((x % 8) == 0) {
					dcol = *pcol++;
					dmsk = *pmsk++;
				} else {
					dcol >>= 1;
					dmsk >>= 1;
				}

				if (cursor->rop == ROP_COPY)
					op = (dmsk & 1) ?
						(dcol & 1) ? 1 : 3 : 0;
				else
					op = ((dmsk & 1) << 1) |
						((dcol & 1) << 0);
				byte |= op << ((x & 3) << 1);

				if (x % 4 == 3) {
					writeb(byte, dst + x / 4);
					byte = 0;
				}
			}
			if (x % 4) {
				writeb(byte, dst + x / 4);
				byte = 0;
			}

			dst += pitch;
		}
	}

	spin_unlock_irqrestore(&glamo->lock_cmd, flags);

	return 0;
}
#endif

static inline int glamofb_cmdq_empty(struct glamofb_handle *gfb)
{
	/* DGCMdQempty -- 1 == command queue is empty */
	return reg_read(gfb, GLAMO_REG_LCD_STATUS1) & (1 << 15);
}

/* call holding gfb->lock_cmd  when locking, until you unlock */
int glamofb_cmd_mode(struct glamofb_handle *gfb, int on)
{
	int timeout = 2000000;

	dev_dbg(gfb->dev, "glamofb_cmd_mode(gfb=%p, on=%d)\n", gfb, on);
	if (on) {
		dev_dbg(gfb->dev, "%s: waiting for cmdq empty: ",
			__func__);
		while ((!glamofb_cmdq_empty(gfb)) && (timeout--))
			cpu_relax();
		if (timeout < 0) {
			printk(KERN_ERR"*************"
				       "glamofb cmd_queue never got empty"
				       "*************\n");
			return -EIO;
		}
		dev_dbg(gfb->dev, "empty!\n");

		/* display the entire frame then switch to command */
		reg_write(gfb, GLAMO_REG_LCD_COMMAND1,
			  GLAMO_LCD_CMD_TYPE_DISP |
			  GLAMO_LCD_CMD_DATA_FIRE_VSYNC);

		/* wait until lcd idle */
		dev_dbg(gfb->dev, "waiting for lcd idle: ");
		timeout = 2000000;
		while ((!reg_read(gfb, GLAMO_REG_LCD_STATUS2) & (1 << 12)) &&
		      (timeout--))
			cpu_relax();
		if (timeout < 0) {
			printk(KERN_ERR"*************"
				       "glamofb lcd never idle"
				       "*************\n");
			return -EIO;
		}

		mdelay(100);

		dev_dbg(gfb->dev, "cmd mode entered\n");

	} else {
		/* RGB interface needs vsync/hsync */
		if (reg_read(gfb, GLAMO_REG_LCD_MODE3) & GLAMO_LCD_MODE3_RGB)
			reg_write(gfb, GLAMO_REG_LCD_COMMAND1,
				  GLAMO_LCD_CMD_TYPE_DISP |
				  GLAMO_LCD_CMD_DATA_DISP_SYNC);

		reg_write(gfb, GLAMO_REG_LCD_COMMAND1,
			  GLAMO_LCD_CMD_TYPE_DISP |
			  GLAMO_LCD_CMD_DATA_DISP_FIRE);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(glamofb_cmd_mode);


int glamofb_cmd_write(struct glamofb_handle *gfb, u_int16_t val)
{
	int timeout = 200000;

	dev_dbg(gfb->dev, "%s: waiting for cmdq empty\n", __func__);
	while ((!glamofb_cmdq_empty(gfb)) && (timeout--))
		yield();
	if (timeout < 0) {
		printk(KERN_ERR"*************"
				"glamofb cmd_queue never got empty"
				"*************\n");
		return 1;
	}
	dev_dbg(gfb->dev, "idle, writing 0x%04x\n", val);

	reg_write(gfb, GLAMO_REG_LCD_COMMAND1, val);

	return 0;
}
EXPORT_SYMBOL_GPL(glamofb_cmd_write);

static struct fb_ops glamofb_ops = {
	.owner		= THIS_MODULE,
	.fb_check_var	= glamofb_check_var,
	.fb_pan_display	= glamofb_pan_display,
	.fb_set_par	= glamofb_set_par,
	.fb_blank	= glamofb_blank,
	.fb_setcolreg	= glamofb_setcolreg,
	.fb_ioctl = glamofb_ioctl,
#ifdef CONFIG_MFD_GLAMO_HWACCEL
	.fb_cursor	= glamofb_cursor,
#endif
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};

static int glamofb_init_regs(struct glamofb_handle *glamo)
{
	struct fb_info *info = glamo->fb;

	glamofb_check_var(&info->var, info);
	glamofb_run_script(glamo, glamo_regs, ARRAY_SIZE(glamo_regs));
	glamofb_set_par(info);

	return 0;
}

static int __init glamofb_probe(struct platform_device *pdev)
{
	int rc = -EIO;
	struct fb_info *fbinfo;
	struct glamofb_handle *glamofb;
	struct glamo_core *core = dev_get_drvdata(pdev->dev.parent);
	struct glamo_fb_platform_data *mach_info;

	printk(KERN_INFO "SMEDIA Glamo frame buffer driver (C) 2007 "
		"Openmoko, Inc.\n");

	if (!core->pdata || !core->pdata->fb_data)
		return -ENOENT;


	fbinfo = framebuffer_alloc(sizeof(struct glamofb_handle), &pdev->dev);
	if (!fbinfo)
		return -ENOMEM;


	glamofb = fbinfo->par;
	glamofb->fb = fbinfo;
	glamofb->dev = &pdev->dev;

	glamofb->blank_mode = FB_BLANK_POWERDOWN;

	strcpy(fbinfo->fix.id, "SMedia Glamo");

	glamofb->reg = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						    "glamo-fb-regs");
	if (!glamofb->reg) {
		dev_err(&pdev->dev, "platform device with no registers?\n");
		rc = -ENOENT;
		goto out_free;
	}

	glamofb->fb_res = platform_get_resource_byname(pdev, IORESOURCE_MEM,
							"glamo-fb-mem");
	if (!glamofb->fb_res) {
		dev_err(&pdev->dev, "platform device with no memory ?\n");
		rc = -ENOENT;
		goto out_free;
	}

	glamofb->reg = request_mem_region(glamofb->reg->start,
					  resource_size(glamofb->reg), pdev->name);
	if (!glamofb->reg) {
		dev_err(&pdev->dev, "failed to request mmio region\n");
		goto out_free;
	}

	glamofb->fb_res = request_mem_region(glamofb->fb_res->start,
					     resource_size(glamofb->fb_res),
					     pdev->name);
	if (!glamofb->fb_res) {
		dev_err(&pdev->dev, "failed to request vram region\n");
		goto out_release_reg;
	}

	/* we want to remap only the registers required for this core
	 * driver. */
	glamofb->base = ioremap_nocache(glamofb->reg->start, resource_size(glamofb->reg));
	if (!glamofb->base) {
		dev_err(&pdev->dev, "failed to ioremap() mmio memory\n");
		goto out_release_fb;
	}

	fbinfo->fix.smem_start = (unsigned long) glamofb->fb_res->start;
	fbinfo->fix.smem_len = (__u32) resource_size(glamofb->fb_res);

	fbinfo->screen_base = ioremap(glamofb->fb_res->start,
				       resource_size(glamofb->fb_res));
	if (!fbinfo->screen_base) {
		dev_err(&pdev->dev, "failed to ioremap() vram memory\n");
		goto out_release_fb;
	}
	glamofb->cursor_addr = fbinfo->screen_base + 0x12C000;

	platform_set_drvdata(pdev, glamofb);

	mach_info = core->pdata->fb_data;
	glamofb->core = core;
	glamofb->mach_info = mach_info;

	fbinfo->fix.visual = FB_VISUAL_TRUECOLOR;
	fbinfo->fix.type = FB_TYPE_PACKED_PIXELS;
	fbinfo->fix.type_aux = 0;
	fbinfo->fix.xpanstep = 0;
	fbinfo->fix.ypanstep = 0;
	fbinfo->fix.ywrapstep = 0;
	fbinfo->fix.accel = FB_ACCEL_GLAMO;


	fbinfo->fbops = &glamofb_ops;
	fbinfo->flags = FBINFO_FLAG_DEFAULT;
	fbinfo->pseudo_palette = &glamofb->pseudo_pal;

	fbinfo->mode = mach_info->modes;
	fb_videomode_to_var(&fbinfo->var, fbinfo->mode);
	fbinfo->var.bits_per_pixel = 16;
	fbinfo->var.nonstd = 0;
	fbinfo->var.activate = FB_ACTIVATE_NOW;
	fbinfo->var.height = mach_info->height;
	fbinfo->var.width = mach_info->width;
	fbinfo->var.accel_flags = 0;
	fbinfo->var.vmode = FB_VMODE_NONINTERLACED;

	glamo_engine_enable(core, GLAMO_ENGINE_LCD);
	glamo_engine_reset(core, GLAMO_ENGINE_LCD);
	glamofb->output_enabled = 1;
	glamofb->mode_set = 1;

	dev_info(&pdev->dev, "spin_lock_init\n");
	spin_lock_init(&glamofb->lock_cmd);
	glamofb_init_regs(glamofb);
#ifdef CONFIG_MFD_GLAMO_HWACCEL
	glamofb_cursor_onoff(glamofb, 1);
#endif

	fb_videomode_to_modelist(mach_info->modes, mach_info->num_modes,
	                         &fbinfo->modelist);

	rc = register_framebuffer(fbinfo);
	if (rc < 0) {
		dev_err(&pdev->dev, "failed to register framebuffer\n");
		goto out_unmap_fb;
	}

	printk(KERN_INFO "fb%d: %s frame buffer device\n",
		fbinfo->node, fbinfo->fix.id);

	return 0;

out_unmap_fb:
	iounmap(fbinfo->screen_base);
	iounmap(glamofb->base);
out_release_fb:
	release_mem_region(glamofb->fb_res->start, resource_size(glamofb->fb_res));
out_release_reg:
	release_mem_region(glamofb->reg->start, resource_size(glamofb->reg));
out_free:
	framebuffer_release(fbinfo);
	return rc;
}

static int glamofb_remove(struct platform_device *pdev)
{
	struct glamofb_handle *glamofb = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);
	iounmap(glamofb->base);
	release_mem_region(glamofb->reg->start, resource_size(glamofb->reg));
	kfree(glamofb);

	return 0;
}

#ifdef CONFIG_PM

static int glamofb_suspend(struct device *dev)
{
	struct glamofb_handle *gfb = dev_get_drvdata(dev);

    acquire_console_sem();
	fb_set_suspend(gfb->fb, 1);
	release_console_sem();

	/* seriously -- nobody is allowed to touch glamo memory when we
	 * are suspended or we lock on nWAIT
	 */
	/* iounmap(gfb->fb->screen_base); */

	return 0;
}

static int glamofb_resume(struct device *dev)
{
	struct glamofb_handle *gfb = dev_get_drvdata(dev);

	/* OK let's allow framebuffer ops again */
	/* gfb->fb->screen_base = ioremap(gfb->fb_res->start,
				       resource_size(gfb->fb_res)); */
	glamo_engine_enable(gfb->core, GLAMO_ENGINE_LCD);
	glamo_engine_reset(gfb->core, GLAMO_ENGINE_LCD);

	glamofb_init_regs(gfb);
#ifdef CONFIG_MFD_GLAMO_HWACCEL
	glamofb_cursor_onoff(gfb, 1);
#endif

    acquire_console_sem();
	fb_set_suspend(gfb->fb, 0);
	release_console_sem();

	return 0;
}

static struct dev_pm_ops glamofb_pm_ops = {
	.suspend = glamofb_suspend,
	.resume = glamofb_resume,
};

#define GLAMOFB_PM_OPS (&glamofb_pm_ops)

#else
#define GLAMOFB_PM_OPS NULL
#endif

static struct platform_driver glamofb_driver = {
	.probe		= glamofb_probe,
	.remove		= glamofb_remove,
	.driver		= {
		.name	= "glamo-fb",
		.owner	= THIS_MODULE,
		.pm     = GLAMOFB_PM_OPS
	},
};

static int __devinit glamofb_init(void)
{
	return platform_driver_register(&glamofb_driver);
}

static void __exit glamofb_cleanup(void)
{
	platform_driver_unregister(&glamofb_driver);
}

module_init(glamofb_init);
module_exit(glamofb_cleanup);

MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");
MODULE_DESCRIPTION("Smedia Glamo 336x/337x framebuffer driver");
MODULE_LICENSE("GPL");
