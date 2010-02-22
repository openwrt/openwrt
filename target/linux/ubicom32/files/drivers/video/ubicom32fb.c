/*
 * drivers/video/ubicom32fb.c
 *	Ubicom32 frame buffer driver
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
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
#include <linux/interrupt.h>

#include <asm/io.h>
#include <asm/ip5000.h>
#include <asm/vdc_tio.h>
#include <asm/ubicom32fb.h>

#define DRIVER_NAME		"ubicom32fb"
#define DRIVER_DESCRIPTION	"Ubicom32 frame buffer driver"

#define PALETTE_ENTRIES_NO	16

/*
 * Option variables
 *
 * vram_size:	VRAM size in kilobytes, subject to alignment
 */
static int vram_size = 0;
module_param(vram_size, int, 0);
MODULE_PARM_DESC(vram, "VRAM size, in kilobytes to allocate, should be at least the size of one screen, subject to alignment");
static int init_value = 0;
module_param(init_value, int, 0);
MODULE_PARM_DESC(init, "Initial value of the framebuffer (16-bit number).");

/*
 * fb_fix_screeninfo defines the non-changeable properties of the VDC, depending on what mode it is in.
 */
static struct fb_fix_screeninfo ubicom32fb_fix = {
	.id =		"Ubicom32",
	.type =		FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_TRUECOLOR,
	.accel =	FB_ACCEL_UBICOM32,
};

/*
 * Filled in at probe time when we find out what the hardware supports
 */
static struct fb_var_screeninfo ubicom32fb_var;

/*
 * Private data structure
 */
struct ubicom32fb_drvdata {
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
	 * Interrupt to set when changing registers
	 */
	u32				vp_int;

	/*
	 * Optional: Interrupt used by TIO to signal us
	 */
	u32				rx_int;

	/*
	 * Base address of the regs for VDC_TIO
	 */
	volatile struct vdc_tio_vp_regs	*regs;

	/*
	 * non-zero if we are in yuv mode
	 */
	u8_t				is_yuv;

	/*
	 * Fake palette of 16 colors
	 */
	u32				pseudo_palette[PALETTE_ENTRIES_NO];

	/*
	 * Wait queue and lock used to block when we need to wait
	 * for something to happen.
	 */
	wait_queue_head_t		waitq;
	struct mutex			lock;

};

/*
 * ubicom32fb_set_next_frame
 *	Sets the next frame buffer to display
 *
 * if sync is TRUE then this function will block until the hardware
 * acknowledges the change
 */
static inline void ubicom32fb_set_next_frame(struct ubicom32fb_drvdata *ud, void *fb, u8_t sync)
{
	ud->regs->next_frame_flags = ud->is_yuv ? VDCTIO_NEXT_FRAME_FLAG_YUV : 0;
	ud->regs->next_frame = (void *)((u32_t)fb | 1);

	/*
	 * If we have interrupts, then we can wait on it
	 */
	if (ud->rx_int != -1) {
		DEFINE_WAIT(wait);
		unsigned long flags;

		spin_lock_irqsave(&ud->lock, flags);
		prepare_to_wait(&ud->waitq, &wait, TASK_INTERRUPTIBLE);
		spin_unlock_irqrestore(&ud->lock, flags);
		schedule();
		finish_wait(&ud->waitq, &wait);
		return;
	}

	/*
	 * No interrupt, we will just spin here
	 */
	while (sync && ((u32_t)ud->regs->next_frame & 1));
}

/*
 * ubicom32fb_send_command
 *	Sends a command/data pair to the VDC
 */
static inline void ubicom32fb_send_command(struct ubicom32fb_drvdata *ud, u16 command, u8_t block)
{
	ud->regs->command = command;
	ubicom32_set_interrupt(ud->vp_int);
	while (block && ud->regs->command);
}

/*
 * ubicom32fb_ioctl
 *	Handles any ioctls sent to us
 */
static int ubicom32fb_ioctl(struct fb_info *fbi, unsigned int cmd,
		       unsigned long arg)
{
	struct ubicom32fb_drvdata *ud = (struct ubicom32fb_drvdata *)fbi->par;
	void __user *argp = (void __user *)arg;
	int retval = -EFAULT;

	switch (cmd) {
	case UBICOM32FB_IOCTL_SET_NEXT_FRAME_SYNC:
		// check alignment, return -EINVAL if necessary
		ubicom32fb_set_next_frame(ud, argp, 1);
		retval = 0;
		break;

	case UBICOM32FB_IOCTL_SET_NEXT_FRAME:
		// check alignment, return -EINVAL if necessary
		ubicom32fb_set_next_frame(ud, argp, 0);
		retval = 0;
		break;

	case UBICOM32FB_IOCTL_SET_MODE:
		if (!(ud->regs->caps & VDCTIO_CAPS_SUPPORTS_SCALING)) {
			break;
		} else {
			struct ubicom32fb_mode mode;
			volatile struct vdc_tio_vp_regs *regs = ud->regs;
			u32_t flags = 0;

			if (copy_from_user(&mode, argp, sizeof(mode))) {
				break;
			}

			regs->x_in = mode.width;
			regs->y_in = mode.height;
			regs->x_out = regs->xres;
			regs->y_out = regs->yres;
			if (mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV_SCAN_ORDER) {
				flags |= VDCTIO_SCALE_FLAG_YUV_SCAN_ORDER;
			}
			if (mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV_BLOCK_ORDER) {
				flags |= VDCTIO_SCALE_FLAG_YUV_BLOCK_ORDER;
			}
			ud->is_yuv = mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV;
			if (ud->is_yuv) {
				flags |= VDCTIO_SCALE_FLAG_YUV;
			}
			if (mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_VRANGE_16_255) {
				flags |= VDCTIO_SCALE_FLAG_VRANGE_16_255;
			}
			if (mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_VRANGE_0_255) {
				flags |= VDCTIO_SCALE_FLAG_VRANGE_0_255;
			}
			if (mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_VSUB) {
				flags |= VDCTIO_SCALE_FLAG_VSUB;
			}
			if (mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_HSUB_2_1) {
				flags |= VDCTIO_SCALE_FLAG_HSUB_2_1;
			}
			if (mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_HSUB_1_1) {
				flags |= VDCTIO_SCALE_FLAG_HSUB_1_1;
			}
			if (mode.flags & UBICOM32FB_IOCTL_SET_MODE_FLAG_SCALE_ENABLE) {
				flags |= VDCTIO_SCALE_FLAG_ENABLE;
			}
			if (mode.next_frame) {
				flags |= VDCTIO_SCALE_FLAG_SET_FRAME_BUFFER;
				regs->next_frame = mode.next_frame;
			}

			regs->scale_flags = flags;
			ubicom32fb_send_command(ud, VDCTIO_COMMAND_SET_SCALE_MODE, 1);
			retval = 0;
			break;
		}

	default:
		retval = -ENOIOCTLCMD;
		break;
	}

	return retval;
}

/*
 * ubicom32fb_interrupt
 *	Called by the OS when the TIO has set the rx_int
 */
static irqreturn_t ubicom32fb_interrupt(int vec, void *appdata)
{
	struct ubicom32fb_drvdata *ud = (struct ubicom32fb_drvdata *)appdata;

	spin_lock(&ud->lock);
	if (waitqueue_active(&ud->waitq)) {
		wake_up(&ud->waitq);
	}
	spin_unlock(&ud->lock);

	return IRQ_HANDLED;
}

/*
 * ubicom32fb_pan_display
 *	Pans the display to a given location.  Supports only y direction panning.
 */
static int ubicom32fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *fbi)
{
	struct ubicom32fb_drvdata *ud = (struct ubicom32fb_drvdata *)fbi->par;
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

	/*
	 * Send down the command.  The buffer will switch at the next vertical blank
	 */
	ubicom32fb_set_next_frame(ud, (void *)new_addr, 0);

	return 0;
}

/*
 * ubicom32fb_setcolreg
 *	Sets a color in our virtual palette
 */
static int ubicom32fb_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp, struct fb_info *fbi)
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
 * ubicom32fb_mmap
 */
static int ubicom32fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	struct ubicom32fb_drvdata *drvdata = (struct ubicom32fb_drvdata *)info->par;

	vma->vm_start = (unsigned long)(drvdata->fb_aligned);

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
 * ubicom32fb_blank
 */
static int ubicom32fb_blank(int blank_mode, struct fb_info *fbi)
{
	return 0;
#if 0
	struct ubicom32fb_drvdata *drvdata = to_ubicom32fb_drvdata(fbi);

	switch (blank_mode) {
	case FB_BLANK_UNBLANK:
		/* turn on panel */
		ubicom32fb_out_be32(drvdata, REG_CTRL, drvdata->reg_ctrl_default);
		break;

	case FB_BLANK_NORMAL:
	case FB_BLANK_VSYNC_SUSPEND:
	case FB_BLANK_HSYNC_SUSPEND:
	case FB_BLANK_POWERDOWN:
		/* turn off panel */
		ubicom32fb_out_be32(drvdata, REG_CTRL, 0);
	default:
		break;

	}
	return 0; /* success */
#endif
}

static struct fb_ops ubicom32fb_ops =
{
	.owner			= THIS_MODULE,
	.fb_pan_display		= ubicom32fb_pan_display,
	.fb_setcolreg		= ubicom32fb_setcolreg,
	.fb_blank		= ubicom32fb_blank,
	.fb_mmap		= ubicom32fb_mmap,
	.fb_ioctl		= ubicom32fb_ioctl,
	.fb_fillrect		= cfb_fillrect,
	.fb_copyarea		= cfb_copyarea,
	.fb_imageblit		= cfb_imageblit,
};

/*
 * ubicom32fb_release
 */
static int ubicom32fb_release(struct device *dev)
{
	struct ubicom32fb_drvdata *ud = dev_get_drvdata(dev);

#if !defined(CONFIG_FRAMEBUFFER_CONSOLE) && defined(CONFIG_LOGO)
	//ubicom32fb_blank(VESA_POWERDOWN, &drvdata->info);
#endif

	unregister_framebuffer(ud->fbinfo);

	if (ud->cmap_alloc) {
		fb_dealloc_cmap(&ud->fbinfo->cmap);
	}

	if (ud->fb) {
		kfree(ud->fb);
	}

	if (ud->rx_int != -1) {
		free_irq(ud->rx_int, ud);
	}

	/*
	 * Turn off the display
	 */
	//ubicom32fb_out_be32(drvdata, REG_CTRL, 0);
	//iounmap(drvdata->regs);

	framebuffer_release(ud->fbinfo);
	dev_set_drvdata(dev, NULL);

	return 0;
}

/*
 * ubicom32fb_platform_probe
 */
static int __init ubicom32fb_platform_probe(struct platform_device *pdev)
{
	struct ubicom32fb_drvdata *ud;
	struct resource *irq_resource_rx;
	struct resource *irq_resource_tx;
	struct resource *mem_resource;
	struct fb_info *fbinfo;
	int rc;
	size_t fbsize;
	struct device *dev = &pdev->dev;
	int offset;
	struct vdc_tio_vp_regs *regs;

	/*
	 * Get our resources
	 */
	irq_resource_tx = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!irq_resource_tx) {
		dev_err(dev, "No tx IRQ resource assigned\n");
		return -ENODEV;
	}

	irq_resource_rx = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
	if (!irq_resource_rx) {
		dev_err(dev, "No rx IRQ resource assigned\n");
		return -ENODEV;
	}

	mem_resource = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem_resource || !mem_resource->start) {
		dev_err(dev, "No mem resource assigned\n");
		return -ENODEV;
	}
	regs = (struct vdc_tio_vp_regs *)mem_resource->start;
	if (regs->version != VDCTIO_VP_VERSION) {
		dev_err(dev, "VDCTIO is not compatible with this driver tio:%x drv:%x\n",
			regs->version, VDCTIO_VP_VERSION);
		return -ENODEV;
	}

	/*
	 * This is the minimum VRAM size
	 */
	fbsize = regs->xres * regs->yres * (regs->bpp / 8);
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
	fbinfo = framebuffer_alloc(sizeof(struct ubicom32fb_drvdata), &pdev->dev);
	if (!fbinfo) {
		dev_err(dev, "Not enough memory to allocate instance.\n");
		return -ENOMEM;
	}

	/*
	 * Fill in our private data.
	 */
	ud = (struct ubicom32fb_drvdata *)fbinfo->par;
	ud->fbinfo = fbinfo;
	ud->regs = (struct vdc_tio_vp_regs *)(mem_resource->start);
	dev_set_drvdata(dev, ud);

	ud->vp_int = irq_resource_tx->start;

	/*
	 * If we were provided an rx_irq then we need to init the appropriate
	 * queues, locks, and functions.
	 */
	ud->rx_int = -1;
	if (irq_resource_rx->start != DEVTREE_IRQ_NONE) {
		init_waitqueue_head(&ud->waitq);
		mutex_init(&ud->lock);
		if (request_irq(ud->rx_int, ubicom32fb_interrupt, IRQF_SHARED, "ubicom32fb_rx", ud)) {
			dev_err(dev, "Couldn't request rx IRQ\n");
			rc = -ENOMEM;
			goto fail;
		}
		ud->rx_int = irq_resource_rx->start;
	}

	/*
	 * Allocate and align the requested amount of VRAM
	 */
	ud->total_vram_size = (vram_size * 1024) + regs->fb_align;
	ud->fb = kmalloc(ud->total_vram_size, GFP_KERNEL);
	if (ud->fb == NULL) {
		dev_err(dev, "Couldn't allocate VRAM\n");
		rc = -ENOMEM;
		goto fail;
	}

	offset = (u32_t)ud->fb & (regs->fb_align - 1);
	if (!offset) {
		ud->fb_aligned = ud->fb;
	} else {
		offset = regs->fb_align - offset;
		ud->fb_aligned = ud->fb + offset;
	}

	/*
	 * Clear the entire frame buffer
	 */
	if (!init_value) {
		memset(ud->fb_aligned, 0, vram_size * 1024);
	} else {
		unsigned short *p = ud->fb_aligned;
		int i;
		for (i = 0; i < ((vram_size * 1024) / sizeof(u16_t)); i++) {
			*p++ = init_value;
		}
	}

	/*
	 * Fill in the fb_var_screeninfo structure
	 */
	memset(&ubicom32fb_var, 0, sizeof(ubicom32fb_var));
	ubicom32fb_var.bits_per_pixel = regs->bpp;
	ubicom32fb_var.red.offset = regs->rshift;
	ubicom32fb_var.green.offset = regs->gshift;
	ubicom32fb_var.blue.offset = regs->bshift;
	ubicom32fb_var.red.length = regs->rbits;
	ubicom32fb_var.green.length = regs->gbits;
	ubicom32fb_var.blue.length = regs->bbits;
	ubicom32fb_var.activate = FB_ACTIVATE_NOW;

#if 0
	/*
	 * Turn on the display
	 */
	ud->reg_ctrl_default = REG_CTRL_ENABLE;
	if (regs->rotate_screen)
		ud->reg_ctrl_default |= REG_CTRL_ROTATE;
	ubicom32fb_out_be32(ud, REG_CTRL, ud->reg_ctrl_default);
#endif

	/*
	 * Fill in the fb_info structure
	 */
	ud->fbinfo->device = dev;
	ud->fbinfo->screen_base = (void *)ud->fb_aligned;
	ud->fbinfo->fbops = &ubicom32fb_ops;
	ud->fbinfo->fix = ubicom32fb_fix;
	ud->fbinfo->fix.smem_start = (u32)ud->fb_aligned;
	ud->fbinfo->fix.smem_len = vram_size * 1024;
	ud->fbinfo->fix.line_length = regs->xres * (regs->bpp / 8);
	ud->fbinfo->fix.mmio_start = (u32)regs;
	ud->fbinfo->fix.mmio_len = sizeof(struct vdc_tio_vp_regs);

	/*
	 * We support panning in the y direction only
	 */
	ud->fbinfo->fix.xpanstep = 0;
	ud->fbinfo->fix.ypanstep = 1;

	ud->fbinfo->pseudo_palette = ud->pseudo_palette;
	ud->fbinfo->flags = FBINFO_DEFAULT;
	ud->fbinfo->var = ubicom32fb_var;
	ud->fbinfo->var.xres = regs->xres;
	ud->fbinfo->var.yres = regs->yres;

	/*
	 * We cannot pan in the X direction, so xres_virtual is regs->xres
	 * We can pan in the Y direction, so yres_virtual is vram_size / ud->fbinfo->fix.line_length
	 */
	ud->fbinfo->var.xres_virtual = regs->xres;
	ud->fbinfo->var.yres_virtual = (vram_size * 1024) / ud->fbinfo->fix.line_length;

	//ud->fbinfo->var.height = regs->height_mm;
	//ud->fbinfo->var.width = regs->width_mm;

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
	 * Start up the VDC
	 */
	ud->regs->next_frame = ud->fb;
	ubicom32fb_send_command(ud, VDCTIO_COMMAND_START, 0);

	/*
	 * Tell the log we are here
	 */
	dev_info(dev, "fbaddr=%p align=%p, size=%uKB screen(%ux%u) virt(%ux%u), regs=%p irqtx=%u irqrx=%u\n",
		ud->fb, ud->fb_aligned, vram_size, ud->fbinfo->var.xres, ud->fbinfo->var.yres,
		ud->fbinfo->var.xres_virtual, ud->fbinfo->var.yres_virtual, ud->regs,
		irq_resource_tx->start, irq_resource_rx->start);

	/*
	 * Success
	 */
	return 0;

fail:
	ubicom32fb_release(dev);
	return rc;
}

/*
 * ubicom32fb_platform_remove
 */
static int ubicom32fb_platform_remove(struct platform_device *pdev)
{
	dev_info(&(pdev->dev), "Ubicom32 FB Driver Remove\n");
	return ubicom32fb_release(&pdev->dev);
}

static struct platform_driver ubicom32fb_platform_driver = {
	.probe		= ubicom32fb_platform_probe,
	.remove		= ubicom32fb_platform_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

#ifndef MODULE
/*
 * ubicom32fb_setup
 *	Process kernel boot options
 */
static int __init ubicom32fb_setup(char *options)
{
	char *this_opt;

	if (!options || !*options) {
		return 0;
	}

	while ((this_opt = strsep(&options, ",")) != NULL) {
		if (!*this_opt) {
			continue;
		}

		if (!strncmp(this_opt, "init_value=", 10)) {
			init_value = simple_strtoul(this_opt + 11, NULL, 0);
			continue;
		}

		if (!strncmp(this_opt, "vram_size=", 10)) {
			vram_size = simple_strtoul(this_opt + 10, NULL, 0);
			continue;
		}
	}
	return 0;
}
#endif /* MODULE */

/*
 * ubicom32fb_init
 */
static int __devinit ubicom32fb_init(void)
{
#ifndef MODULE
	/*
	 * Get kernel boot options (in 'video=ubicom32fb:<options>')
	 */
	char *option = NULL;

	if (fb_get_options(DRIVER_NAME, &option)) {
		return -ENODEV;
	}
	ubicom32fb_setup(option);
#endif /* MODULE */

	return platform_driver_register(&ubicom32fb_platform_driver);
}
module_init(ubicom32fb_init);

/*
 * ubicom32fb_exit
 */
static void __exit ubicom32fb_exit(void)
{
	platform_driver_unregister(&ubicom32fb_platform_driver);
}
module_exit(ubicom32fb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Tjin <@ubicom.com>");
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
