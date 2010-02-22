/*
 * drivers/video/ubicom32vfb.c
 *	Ubicom32 virtual frame buffer driver
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
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DRIVER_NAME		"ubicom32vfb"
#define DRIVER_DESCRIPTION	"Ubicom32 virtual frame buffer driver"

#define PALETTE_ENTRIES_NO	16

/*
 * Option variables
 *
 * vram_size:	VRAM size in kilobytes, subject to alignment
 */
static int vram_size = 0;
module_param(vram_size, int, 0);
MODULE_PARM_DESC(vram_size, "VRAM size, in kilobytes to allocate, should be at least the size of one screen, subject to alignment");

static int xres = 320;
module_param(xres, int, 0);
MODULE_PARM_DESC(xres, "x (horizontal) resolution");

static int yres = 240;
module_param(yres, int, 0);
MODULE_PARM_DESC(yres, "y (vertical) resolution");

static int bgr = 0;
module_param(bgr, int, 0);
MODULE_PARM_DESC(bgr, "display is BGR (Blue is MSB)");

#define BITS_PER_PIXEL	16

/*
 * Buffer alignment, must not be 0
 */
#define UBICOM32VFB_ALIGNMENT 4

/*
 * fb_fix_screeninfo defines the non-changeable properties of the VDC, depending on what mode it is in.
 */
static struct fb_fix_screeninfo ubicom32vfb_fix = {
	.id =		"Ubicom32",
	.type =		FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_TRUECOLOR,
	.accel =	FB_ACCEL_UBICOM32_VFB,
};

/*
 * Filled in at probe time when we find out what the hardware supports
 */
static struct fb_var_screeninfo ubicom32vfb_var;

/*
 * Private data structure
 */
struct ubicom32vfb_drvdata {
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
};

static struct platform_device *ubicom32vfb_platform_device;

/*
 * ubicom32vfb_pan_display
 *	Pans the display to a given location.  Supports only y direction panning.
 */
static int ubicom32vfb_pan_display(struct fb_var_screeninfo *var, struct fb_info *fbi)
{
	struct ubicom32vfb_drvdata *ud = (struct ubicom32vfb_drvdata *)fbi->par;
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
 * ubicom32vfb_setcolreg
 *	Sets a color in our virtual palette
 */
static int ubicom32vfb_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp, struct fb_info *fbi)
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
 * ubicom32vfb_mmap
 */
static int ubicom32vfb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	struct ubicom32vfb_drvdata *ud = (struct ubicom32vfb_drvdata *)info->par;

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
 * ubicom32vfb_check_var
 *	Check the var, tweak it but don't change operational parameters.
 */
static int ubicom32vfb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct ubicom32vfb_drvdata *ud = (struct ubicom32vfb_drvdata *)info->par;
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
 * ubicom32vfb_set_par
 *	Set the video mode according to info->var
 */
static int ubicom32vfb_set_par(struct fb_info *info)
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
 * ubicom32vfb_ops
 *	List of supported operations
 */
static struct fb_ops ubicom32vfb_ops =
{
	.owner			= THIS_MODULE,
	.fb_pan_display		= ubicom32vfb_pan_display,
	.fb_setcolreg		= ubicom32vfb_setcolreg,
	.fb_mmap		= ubicom32vfb_mmap,
	.fb_check_var		= ubicom32vfb_check_var,
	.fb_set_par		= ubicom32vfb_set_par,
	.fb_fillrect		= cfb_fillrect,
	.fb_copyarea		= cfb_copyarea,
	.fb_imageblit		= cfb_imageblit,
};

/*
 * ubicom32vfb_release
 */
static int ubicom32vfb_release(struct device *dev)
{
	struct ubicom32vfb_drvdata *ud = dev_get_drvdata(dev);

	unregister_framebuffer(ud->fbinfo);

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
 * ubicom32vfb_platform_probe
 */
static int __init ubicom32vfb_platform_probe(struct platform_device *pdev)
{
	struct ubicom32vfb_drvdata *ud;
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
	fbinfo = framebuffer_alloc(sizeof(struct ubicom32vfb_drvdata), &pdev->dev);
	if (!fbinfo) {
		dev_err(dev, "Not enough memory to allocate instance.\n");
		return -ENOMEM;
	}

	/*
	 * Fill in our private data.
	 */
	ud = (struct ubicom32vfb_drvdata *)fbinfo->par;
	ud->fbinfo = fbinfo;
	dev_set_drvdata(dev, ud);

	/*
	 * Allocate and align the requested amount of VRAM
	 */
	ud->total_vram_size = (vram_size * 1024) + UBICOM32VFB_ALIGNMENT;
	ud->fb = kmalloc(ud->total_vram_size, GFP_KERNEL);
	if (ud->fb == NULL) {
		dev_err(dev, "Couldn't allocate VRAM\n");
		rc = -ENOMEM;
		goto fail;
	}

	offset = (u32_t)ud->fb & (UBICOM32VFB_ALIGNMENT - 1);
	if (!offset) {
		ud->fb_aligned = ud->fb;
	} else {
		offset =  UBICOM32VFB_ALIGNMENT - offset;
		ud->fb_aligned = ud->fb + offset;
	}

	/*
	 * Clear the entire frame buffer
	 */
	memset(ud->fb_aligned, 0, vram_size * 1024);

	/*
	 * Fill in the fb_var_screeninfo structure
	 */
	memset(&ubicom32vfb_var, 0, sizeof(ubicom32vfb_var));
	ubicom32vfb_var.bits_per_pixel = BITS_PER_PIXEL;
	ubicom32vfb_var.red.length = 5;
	ubicom32vfb_var.green.length = 6;
	ubicom32vfb_var.green.offset = 5;
	ubicom32vfb_var.blue.length = 5;
	ubicom32vfb_var.activate = FB_ACTIVATE_NOW;

	if (bgr) {
		ubicom32vfb_var.red.offset = 0;
		ubicom32vfb_var.blue.offset = 11;
	} else {
		ubicom32vfb_var.red.offset = 11;
		ubicom32vfb_var.blue.offset = 0;
	}

	/*
	 * Fill in the fb_info structure
	 */
	ud->fbinfo->device = dev;
	ud->fbinfo->screen_base = (void *)ud->fb_aligned;
	ud->fbinfo->fbops = &ubicom32vfb_ops;
	ud->fbinfo->fix = ubicom32vfb_fix;
	ud->fbinfo->fix.smem_start = (u32)ud->fb_aligned;
	ud->fbinfo->fix.smem_len = vram_size * 1024;
	ud->fbinfo->fix.line_length = xres * 2;
	ud->fbinfo->fix.mmio_start = (u32)ud;
	ud->fbinfo->fix.mmio_len = sizeof(struct ubicom32vfb_drvdata);

	/*
	 * We support panning in the y direction only
	 */
	ud->fbinfo->fix.xpanstep = 0;
	ud->fbinfo->fix.ypanstep = 1;

	ud->fbinfo->pseudo_palette = ud->pseudo_palette;
	ud->fbinfo->flags = FBINFO_DEFAULT;
	ud->fbinfo->var = ubicom32vfb_var;
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
	ubicom32vfb_release(dev);
	return rc;
}

/*
 * ubicom32vfb_platform_remove
 */
static int ubicom32vfb_platform_remove(struct platform_device *pdev)
{
	dev_info(&(pdev->dev), "Ubicom32 FB Driver Remove\n");
	return ubicom32vfb_release(&pdev->dev);
}

static struct platform_driver ubicom32vfb_platform_driver = {
	.probe		= ubicom32vfb_platform_probe,
	.remove		= ubicom32vfb_platform_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

#ifndef MODULE
/*
 * ubicom32vfb_setup
 *	Process kernel boot options
 */
static int __init ubicom32vfb_setup(char *options)
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
 * ubicom32vfb_init
 */
static int __devinit ubicom32vfb_init(void)
{
	int ret;

#ifndef MODULE
	/*
	 * Get kernel boot options (in 'video=ubicom32vfb:<options>')
	 */
	char *option = NULL;

	if (fb_get_options(DRIVER_NAME, &option)) {
		return -ENODEV;
	}
	ubicom32vfb_setup(option);
#endif /* MODULE */

	ret = platform_driver_register(&ubicom32vfb_platform_driver);

#ifdef CONFIG_FB_UBICOM32_VIRTUAL_NOAUTO
	return ret;
#else
	if (!ret) {
		ubicom32vfb_platform_device = platform_device_alloc(DRIVER_NAME, 0);

		if (ubicom32vfb_platform_device)
			ret = platform_device_add(ubicom32vfb_platform_device);
		else
			ret = -ENOMEM;

		if (ret) {
			platform_device_put(ubicom32vfb_platform_device);
			platform_driver_unregister(&ubicom32vfb_platform_driver);
		}
	}

	return ret;
#endif
}
module_init(ubicom32vfb_init);

/*
 * ubicom32vfb_exit
 */
static void __exit ubicom32vfb_exit(void)
{
	platform_device_unregister(ubicom32vfb_platform_device);
	platform_driver_unregister(&ubicom32vfb_platform_driver);
}
module_exit(ubicom32vfb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Tjin <@ubicom.com>");
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
