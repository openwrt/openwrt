/*
 * drivers/video/ubicom32plio80.c
 *	Ubicom32 80 bus PLIO buffer driver
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */

/*
 * This driver was based on skeletonfb.c, Skeleton for a frame buffer device by
 * Geert Uytterhoeven.
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/plio.h>

#define DRIVER_NAME		"ubicom32plio80"
#define DRIVER_DESCRIPTION	"Ubicom32 80 bus PLIO frame buffer driver"

#define PALETTE_ENTRIES_NO	16

/*
 * Option variables
 *
 * vram_size:	VRAM size in kilobytes, subject to alignment
 */
static int vram_size = 0;
module_param(vram_size, int, 0);
MODULE_PARM_DESC(vram_size, "VRAM size, in kilobytes to allocate, should be at least the size of one screen, subject to alignment");

static int xres = 240;
module_param(xres, int, 0);
MODULE_PARM_DESC(xres, "x (horizontal) resolution");

static int yres = 320;
module_param(yres, int, 0);
MODULE_PARM_DESC(yres, "y (vertical) resolution");

static int bgr = 0;
module_param(bgr, int, 0);
MODULE_PARM_DESC(bgr, "display is BGR (Blue is MSB)");

#define BITS_PER_PIXEL	16

/*
 * Buffer alignment, must not be 0
 */
#define UBICOM32PLIO80_ALIGNMENT 4

/*
 * PLIO FSM
 *	16-bit data bus on port I
 *	CS on EXTCTL[6]
 *	WR on EXTCTL[4]
 */
static const plio_fctl_t plio_fctl = {
	.fctl0 = {
		.ptif_port_mode = PLIO_PORT_MODE_DI,
		.ptif_portd_cfg = 0,
		.ptif_porti_cfg = 3,
		.edif_ds = 6,
		.edif_cmp_mode = 1,
		.ecif_extclk_ena = 0, // enable clock output on PD7 table 2.65/p111 says extctl[0]?
		.icif_clk_src_sel = PLIO_CLK_IO,
	},
	.fctl2 = {
		.icif_eclk_div = 10,
		.icif_iclk_div = 10,
	},

	};

	static const plio_config_t plio_config = {
	.pfsm = {
		/*
		 * Table 12.63
		 */
		.grpsel[0] = {1,1,1,1,1,1,1,1,1,1},

		/*
		* Table 12.66 Counter load value
		*/
		.cs_lut[0] = {0,0,0,0,0,0,0,0},

		/*
		 * Table 2.75 PLIO PFSM Configuration Registers
		 */
		//                      3     2     1     0
		.extctl_o_lut[0] = {0x3f, 0x2f, 0x3f, 0x3f},
		//                      7     6     5     4
		.extctl_o_lut[1] = {0x3f, 0x3f, 0x3f, 0x2f},
	},
	.edif = {
		.odr_oe = 0xffff,
	},
	.ecif = {
		.output_ena = (1 << 6) | (1 << 4),
	},
};

static const u32_t ubicom32plio80_plio_fsm[] = {
	// 0-F
	0x00070007, 0x00070007,
	0x00070007, 0x00070007,
	0x00070007, 0x00070007,
	0x00070007, 0x00070007,

	0x16260806, 0x16260806,
	0x16260806, 0x16260806,
	0x16260806, 0x16260806,
	0x16260806, 0x16260806,

	// 10 - 1f
	0x22061806, 0x22061806,
	0x22061806, 0x22061806,
	0x22061806, 0x22061806,
	0x22061806, 0x22061806,

	0x22061806, 0x22061806,
	0x22061806, 0x22061806,
	0x22061806, 0x22061806,
	0x22061806, 0x22061806,

	// 20 - 2f
	0x00070806, 0x00070806,
	0x00070806, 0x00070806,
	0x00070806, 0x00070806,
	0x00070806, 0x00070806,

	0x00070806, 0x00070806,
	0x00070806, 0x00070806,
	0x00070806, 0x00070806,
	0x00070806, 0x00070806,
};

/*
 * fb_fix_screeninfo defines the non-changeable properties of the VDC, depending on what mode it is in.
 */
static struct fb_fix_screeninfo ubicom32plio80_fix = {
	.id =		"Ubicom32",
	.type =		FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_TRUECOLOR,
	.accel =	FB_ACCEL_UBICOM32_PLIO80,
};

/*
 * Filled in at probe time when we find out what the hardware supports
 */
static struct fb_var_screeninfo ubicom32plio80_var;

/*
 * Private data structure
 */
struct ubicom32plio80_drvdata {
	struct fb_info			*fbinfo;
	bool				cmap_alloc;

	/*
	 * The address of the framebuffer in memory
	 */
	void				*fb;
	void				*fb_aligned;

	/*
	 * Total size of vram including alignment allowance
	 */
	u32				total_vram_size;

	/*
	 * Fake palette of 16 colors
	 */
	u32				pseudo_palette[PALETTE_ENTRIES_NO];

	int				irq_req;

	/*
	 * Current pointer and bytes left to transfer with the PLIO
	 */
	void				*xfer_ptr;
	u32				bytes_to_xfer;
	u32				busy;
};

static struct platform_device *ubicom32plio80_platform_device;

/*
 * ubicom32plio80_isr
 */
static int ubicom32plio80_isr(int irq, void *appdata)
{
	struct ubicom32plio80_drvdata *ud = (struct ubicom32plio80_drvdata *)appdata;

	if (!ud->bytes_to_xfer) {
		ubicom32_disable_interrupt(TX_FIFO_INT(PLIO_PORT));
		PLIO_NBR->intmask.txfifo_wm = 0;
		ud->busy = 0;
		return IRQ_HANDLED;
	}

	asm volatile (
		".rept 8				\n\t"
		"move.4	(%[fifo]), (%[data])4++		\n\t"
		".endr					\n\t"
		: [data] "+a" (ud->xfer_ptr)
		: [fifo] "a" (&PLIO_NBR->tx_lo)
	);

	ud->bytes_to_xfer -= 32;

	return IRQ_HANDLED;
}

/*
 * ubicom32plio80_update
 */
static void ubicom32plio80_update(struct ubicom32plio80_drvdata *ud, u32 *fb)
{
	struct ubicom32_io_port *ri = (struct ubicom32_io_port *)RI;
	struct ubicom32_io_port *rd = (struct ubicom32_io_port *)RD;

	ud->xfer_ptr = fb;
	ud->bytes_to_xfer = (xres * yres * 2) - 64;
	ud->busy = 1;

	ri->gpio_mask = 0;
	rd->gpio_mask &= ~((1 << 4) | (1 << 2));

	*(u32 *)(&PLIO_NBR->intclr) = ~0;
	PLIO_NBR->intmask.txfifo_wm = 1;
	PLIO_NBR->fifo_wm.tx = 8;
	ubicom32_enable_interrupt(TX_FIFO_INT(PLIO_PORT));

	asm volatile (
		".rept 16				\n\t"
		"move.4	(%[fifo]), (%[data])4++		\n\t"
		".endr					\n\t"
		: [data] "+a" (ud->xfer_ptr)
		: [fifo] "a" (&PLIO_NBR->tx_lo)
	);
}

/*
 * ubicom32plio80_pan_display
 *	Pans the display to a given location.  Supports only y direction panning.
 */
static int ubicom32plio80_pan_display(struct fb_var_screeninfo *var, struct fb_info *fbi)
{
	struct ubicom32plio80_drvdata *ud = (struct ubicom32plio80_drvdata *)fbi->par;
	void *new_addr;

	/*
	 * Get the last y line that would be displayed.  Since we don't support YWRAP,
	 * it must be less than our virtual y size.
	 */
	u32 lasty = var->yoffset + var->yres;
	if (lasty > fbi->var.yres_virtual) {
		/*
		 * We would fall off the end of our frame buffer if we panned here.
		 */
		return -EINVAL;
	}

	if (var->xoffset) {
		/*
		 * We don't support panning in the x direction
		 */
		return -EINVAL;
	}

	/*
	 * Everything looks sane, go ahead and pan
	 *
	 * We have to calculate a new address for the VDC to look at
	 */
	new_addr = ud->fb_aligned + (var->yoffset * fbi->fix.line_length);

	return 0;
}

/*
 * ubicom32plio80_setcolreg
 *	Sets a color in our virtual palette
 */
static int ubicom32plio80_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp, struct fb_info *fbi)
{
	u32 *palette = fbi->pseudo_palette;

	if (regno >= PALETTE_ENTRIES_NO) {
		return -EINVAL;
	}

	/*
	 * We only use 8 bits from each color
	 */
	red >>= 8;
	green >>= 8;
	blue >>= 8;

	/*
	 * Convert any grayscale values
	 */
	if (fbi->var.grayscale) {
		u16 gray = red + green + blue;
		gray += (gray >> 2) + (gray >> 3) - (gray >> 7);
		gray >>= 2;
		if (gray > 255) {
			gray = 255;
		}
		red = gray;
		blue = gray;
		green = gray;
	}

	palette[regno] = (red << fbi->var.red.offset) | (green << fbi->var.green.offset) |
			 (blue << fbi->var.blue.offset);

	return 0;
}

/*
 * ubicom32plio80_mmap
 */
static int ubicom32plio80_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	struct ubicom32plio80_drvdata *ud = (struct ubicom32plio80_drvdata *)info->par;

	vma->vm_start = (unsigned long)(ud->fb_aligned);

	vma->vm_end = vma->vm_start + info->fix.smem_len;

	/* For those who don't understand how mmap works, go read
	 *   Documentation/nommu-mmap.txt.
	 * For those that do, you will know that the VM_MAYSHARE flag
	 * must be set in the vma->vm_flags structure on noMMU
	 *   Other flags can be set, and are documented in
	 *   include/linux/mm.h
	 */

	vma->vm_flags |=  VM_MAYSHARE | VM_SHARED;

	return 0;
}

/*
 * ubicom32plio80_check_var
 *	Check the var, tweak it but don't change operational parameters.
 */
static int ubicom32plio80_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct ubicom32plio80_drvdata *ud = (struct ubicom32plio80_drvdata *)info->par;
	u32 line_size = var->xres * (BITS_PER_PIXEL / 8);

	/*
	 * See if we can handle this bpp
	 */
	if (var->bits_per_pixel > BITS_PER_PIXEL) {
		return -EINVAL;
	}
	var->bits_per_pixel = BITS_PER_PIXEL;

	/*
	 * See if we have enough memory to handle this resolution
	 */
	if ((line_size * var->yres * BITS_PER_PIXEL / 8) > ud->total_vram_size) {
		return -EINVAL;
	}

	var->xres_virtual = var->xres;
	var->yres_virtual = ud->total_vram_size / line_size;

	var->red.length = 5;
	var->green.length = 6;
	var->green.offset = 5;
	var->blue.length = 5;
	var->transp.offset = var->transp.length = 0;

	if (bgr) {
		var->red.offset = 0;
		var->blue.offset = 11;
	} else {
		var->red.offset = 11;
		var->blue.offset = 0;
	}

	var->nonstd = 0;
	var->height = -1;
	var->width = -1;
	var->vmode = FB_VMODE_NONINTERLACED;
	var->sync = 0;

	return 0;
}

/*
 * ubicom32plio80_set_par
 *	Set the video mode according to info->var
 */
static int ubicom32plio80_set_par(struct fb_info *info)
{
	/*
	 * Anything changed?
	 */
	if ((xres == info->var.xres) && (yres == info->var.yres)) {
		return 0;
	}

	/*
	 * Implement changes
	 */
	xres = info->var.xres;
	yres = info->var.yres;
	info->fix.visual = FB_VISUAL_TRUECOLOR;
	info->fix.xpanstep = 0;
	info->fix.ypanstep = 1;
	info->fix.line_length = xres * (BITS_PER_PIXEL / 8);

	return 0;
}

/*
 * ubicom32plio80_ops
 *	List of supported operations
 */
static struct fb_ops ubicom32plio80_ops =
{
	.owner			= THIS_MODULE,
	.fb_pan_display		= ubicom32plio80_pan_display,
	.fb_setcolreg		= ubicom32plio80_setcolreg,
	.fb_mmap		= ubicom32plio80_mmap,
	.fb_check_var		= ubicom32plio80_check_var,
	.fb_set_par		= ubicom32plio80_set_par,
	.fb_fillrect		= cfb_fillrect,
	.fb_copyarea		= cfb_copyarea,
	.fb_imageblit		= cfb_imageblit,
};

/*
 * ubicom32plio80_release
 */
static int ubicom32plio80_release(struct device *dev)
{
	struct ubicom32plio80_drvdata *ud = dev_get_drvdata(dev);

	unregister_framebuffer(ud->fbinfo);

	if (ud->irq_req) {
		free_irq(TX_FIFO_INT(PLIO_PORT), ud);
	}
	if (ud->cmap_alloc) {
		fb_dealloc_cmap(&ud->fbinfo->cmap);
	}

	if (ud->fb) {
		kfree(ud->fb);
	}

	framebuffer_release(ud->fbinfo);
	dev_set_drvdata(dev, NULL);

	return 0;
}

/*
 * ubicom32plio80_platform_probe
 */
static int __init ubicom32plio80_platform_probe(struct platform_device *pdev)
{
	struct ubicom32plio80_drvdata *ud;
	struct fb_info *fbinfo;
	int rc;
	size_t fbsize;
	struct device *dev = &pdev->dev;
	int offset;

	/*
	 * This is the minimum VRAM size
	 */
	fbsize = xres * yres * 2;
	if (!vram_size) {
		vram_size = (fbsize + 1023) / 1024;
	} else {
		if (fbsize > (vram_size * 1024)) {
			dev_err(dev, "Not enough VRAM for display, need >= %u bytes\n", fbsize);
			return -ENOMEM; // should be ebadparam?
		}
	}

	/*
	 * Allocate the framebuffer instance + our private data
	 */
	fbinfo = framebuffer_alloc(sizeof(struct ubicom32plio80_drvdata), &pdev->dev);
	if (!fbinfo) {
		dev_err(dev, "Not enough memory to allocate instance.\n");
		return -ENOMEM;
	}

	/*
	 * Fill in our private data.
	 */
	ud = (struct ubicom32plio80_drvdata *)fbinfo->par;
	ud->fbinfo = fbinfo;
	dev_set_drvdata(dev, ud);

	/*
	 * Allocate and align the requested amount of VRAM
	 */
	ud->total_vram_size = (vram_size * 1024) + UBICOM32PLIO80_ALIGNMENT;
	ud->fb = kmalloc(ud->total_vram_size, GFP_KERNEL);
	if (ud->fb == NULL) {
		dev_err(dev, "Couldn't allocate VRAM\n");
		rc = -ENOMEM;
		goto fail;
	}

	offset = (u32_t)ud->fb & (UBICOM32PLIO80_ALIGNMENT - 1);
	if (!offset) {
		ud->fb_aligned = ud->fb;
	} else {
		offset =  UBICOM32PLIO80_ALIGNMENT - offset;
		ud->fb_aligned = ud->fb + offset;
	}

	/*
	 * Clear the entire frame buffer
	 */
	memset(ud->fb_aligned, 0, vram_size * 1024);

	/*
	 * Fill in the fb_var_screeninfo structure
	 */
	memset(&ubicom32plio80_var, 0, sizeof(ubicom32plio80_var));
	ubicom32plio80_var.bits_per_pixel = BITS_PER_PIXEL;
	ubicom32plio80_var.red.length = 5;
	ubicom32plio80_var.green.length = 6;
	ubicom32plio80_var.green.offset = 5;
	ubicom32plio80_var.blue.length = 5;
	ubicom32plio80_var.activate = FB_ACTIVATE_NOW;

	if (bgr) {
		ubicom32plio80_var.red.offset = 0;
		ubicom32plio80_var.blue.offset = 11;
	} else {
		ubicom32plio80_var.red.offset = 11;
		ubicom32plio80_var.blue.offset = 0;
	}

	/*
	 * Fill in the fb_info structure
	 */
	ud->fbinfo->device = dev;
	ud->fbinfo->screen_base = (void *)ud->fb_aligned;
	ud->fbinfo->fbops = &ubicom32plio80_ops;
	ud->fbinfo->fix = ubicom32plio80_fix;
	ud->fbinfo->fix.smem_start = (u32)ud->fb_aligned;
	ud->fbinfo->fix.smem_len = vram_size * 1024;
	ud->fbinfo->fix.line_length = xres * 2;
	ud->fbinfo->fix.mmio_start = (u32)ud;
	ud->fbinfo->fix.mmio_len = sizeof(struct ubicom32plio80_drvdata);

	/*
	 * We support panning in the y direction only
	 */
	ud->fbinfo->fix.xpanstep = 0;
	ud->fbinfo->fix.ypanstep = 1;

	ud->fbinfo->pseudo_palette = ud->pseudo_palette;
	ud->fbinfo->flags = FBINFO_DEFAULT;
	ud->fbinfo->var = ubicom32plio80_var;
	ud->fbinfo->var.xres = xres;
	ud->fbinfo->var.yres = yres;

	/*
	 * We cannot pan in the X direction, so xres_virtual is xres
	 * We can pan in the Y direction, so yres_virtual is vram_size / ud->fbinfo->fix.line_length
	 */
	ud->fbinfo->var.xres_virtual = xres;
	ud->fbinfo->var.yres_virtual = (vram_size * 1024) / ud->fbinfo->fix.line_length;

	/*
	 * Allocate a color map
	 */
	rc = fb_alloc_cmap(&ud->fbinfo->cmap, PALETTE_ENTRIES_NO, 0);
	if (rc) {
		dev_err(dev, "Fail to allocate colormap (%d entries)\n",
			PALETTE_ENTRIES_NO);
		goto fail;
	}
	ud->cmap_alloc = true;

	/*
	 * Register new frame buffer
	 */
	rc = register_framebuffer(ud->fbinfo);
	if (rc) {
		dev_err(dev, "Could not register frame buffer\n");
		goto fail;
	}

	/*
	 * request the PLIO IRQ
	 */
	rc = request_irq(TX_FIFO_INT(PLIO_PORT), ubicom32plio80_isr, IRQF_DISABLED, "ubicom32plio80", ud);
	if (rc) {
		dev_err(dev, "Could not request IRQ\n");
		goto fail;
	}
	ud->irq_req = 1;

	/*
	 * Clear any garbage out of the TX FIFOs (idif_txfifo_flush)
	 *
	 * cast through ubicom32_io_port to make sure the compiler does a word write
	 */
	((struct ubicom32_io_port *)PLIO_NBR)->int_set = (1 << 18);

	/*
	 * Start up the state machine
	 */
	plio_init(&plio_fctl, &plio_config, (plio_sram_t *)ubicom32plio80_plio_fsm, sizeof(ubicom32plio80_plio_fsm));
	PLIO_NBR->fctl0.pfsm_cmd = 0;

	ubicom32plio80_update(ud, ud->fb_aligned);

	/*
	 * Tell the log we are here
	 */
	dev_info(dev, "fbaddr=%p align=%p, size=%uKB screen(%ux%u) virt(%ux%u)\n",
		ud->fb, ud->fb_aligned, vram_size, ud->fbinfo->var.xres, ud->fbinfo->var.yres,
		ud->fbinfo->var.xres_virtual, ud->fbinfo->var.yres_virtual);

	/*
	 * Success
	 */
	return 0;

fail:
	ubicom32plio80_release(dev);
	return rc;
}

/*
 * ubicom32plio80_platform_remove
 */
static int ubicom32plio80_platform_remove(struct platform_device *pdev)
{
	dev_info(&(pdev->dev), "Ubicom32 FB Driver Remove\n");
	return ubicom32plio80_release(&pdev->dev);
}

static struct platform_driver ubicom32plio80_platform_driver = {
	.probe		= ubicom32plio80_platform_probe,
	.remove		= ubicom32plio80_platform_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

#ifndef MODULE
/*
 * ubicom32plio80_setup
 *	Process kernel boot options
 */
static int __init ubicom32plio80_setup(char *options)
{
	char *this_opt;

	if (!options || !*options) {
		return 0;
	}

	while ((this_opt = strsep(&options, ",")) != NULL) {
		if (!*this_opt) {
			continue;
		}

		if (!strncmp(this_opt, "vram_size=", 10)) {
			vram_size = simple_strtoul(this_opt + 10, NULL, 0);
			continue;
		}

		if (!strncmp(this_opt, "bgr=", 4)) {
			bgr = simple_strtoul(this_opt + 4, NULL, 0);
			continue;
		}

		if (!strncmp(this_opt, "xres=", 5)) {
			xres = simple_strtoul(this_opt + 5, NULL, 0);
			continue;
		}

		if (!strncmp(this_opt, "yres=", 5)) {
			yres = simple_strtoul(this_opt + 5, NULL, 0);
			continue;
		}
	}
	return 0;
}
#endif /* MODULE */

/*
 * ubicom32plio80_init
 */
static int __devinit ubicom32plio80_init(void)
{
	int ret;

#ifndef MODULE
	/*
	 * Get kernel boot options (in 'video=ubicom32plio80:<options>')
	 */
	char *option = NULL;

	if (fb_get_options(DRIVER_NAME, &option)) {
		return -ENODEV;
	}
	ubicom32plio80_setup(option);
#endif /* MODULE */

	ret = platform_driver_register(&ubicom32plio80_platform_driver);

	if (!ret) {
		ubicom32plio80_platform_device = platform_device_alloc(DRIVER_NAME, 0);

		if (ubicom32plio80_platform_device)
			ret = platform_device_add(ubicom32plio80_platform_device);
		else
			ret = -ENOMEM;

		if (ret) {
			platform_device_put(ubicom32plio80_platform_device);
			platform_driver_unregister(&ubicom32plio80_platform_driver);
		}
	}

	return ret;
}
module_init(ubicom32plio80_init);

/*
 * ubicom32plio80_exit
 */
static void __exit ubicom32plio80_exit(void)
{
	platform_device_unregister(ubicom32plio80_platform_device);
	platform_driver_unregister(&ubicom32plio80_platform_driver);
}
module_exit(ubicom32plio80_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Tjin <@ubicom.com>");
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
