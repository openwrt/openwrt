/*
 *  Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ4720/JZ4740 SoC LCD framebuffer driver
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/jz4740_fb.h>

#include <linux/delay.h>
#include <linux/clk.h>

#include <asm/mach-jz4740/gpio.h>

#define JZ_REG_LCD_CFG		0x00
#define JZ_REG_LCD_VSYNC	0x04
#define JZ_REG_LCD_HSYNC	0x08
#define JZ_REG_LCD_VAT		0x0C
#define JZ_REG_LCD_DAH		0x10
#define JZ_REG_LCD_DAV		0x14
#define JZ_REG_LCD_PS		0x18
#define JZ_REG_LCD_CLS		0x1C
#define JZ_REG_LCD_SPL		0x20
#define JZ_REG_LCD_REV		0x24
#define JZ_REG_LCD_CTRL		0x30
#define JZ_REG_LCD_STATE	0x34
#define JZ_REG_LCD_IID		0x38
#define JZ_REG_LCD_DA0		0x40
#define JZ_REG_LCD_SA0		0x44
#define JZ_REG_LCD_FID0		0x48
#define JZ_REG_LCD_CMD0		0x4C
#define JZ_REG_LCD_DA1		0x50
#define JZ_REG_LCD_SA1		0x54
#define JZ_REG_LCD_FID1		0x58
#define JZ_REG_LCD_CMD1		0x5C

#define JZ_LCD_CFG_SLCD BIT(31)
#define JZ_LCD_CFG_PSM BIT(23)
#define JZ_LCD_CFG_CLSM BIT(22)
#define JZ_LCD_CFG_SPLM BIT(21)
#define JZ_LCD_CFG_REVM BIT(20)
#define JZ_LCD_CFG_HSYNCM BIT(19)
#define JZ_LCD_CFG_PCLKM BIT(18)
#define JZ_LCD_CFG_INV BIT(17)
#define JZ_LCD_CFG_SYNC_DIR BIT(16)
#define JZ_LCD_CFG_PSP BIT(15)
#define JZ_LCD_CFG_CLSP BIT(14)
#define JZ_LCD_CFG_SPLP BIT(13)
#define JZ_LCD_CFG_REVP BIT(12)
#define JZ_LCD_CFG_HSYNCP BIT(11)
#define JZ_LCD_CFG_PCLKP BIT(10)
#define JZ_LCD_CFG_DEP BIT(9)
#define JZ_LCD_CFG_VSYNCP BIT(8)
#define JZ_LCD_CFG_18_BIT BIT(7)
#define JZ_LCD_CFG_PDW BIT(5) | BIT(4)
#define JZ_LCD_CFG_MODE_MASK 0xf

#define JZ_LCD_CTRL_BURST_4 (0x0 << 28)
#define JZ_LCD_CTRL_BURST_8 (0x1 << 28)
#define JZ_LCD_CTRL_BURST_16 (0x2 << 28)
#define JZ_LCD_CTRL_RGB555 BIT(27)
#define JZ_LCD_CTRL_OFUP BIT(26)
#define JZ_LCD_CTRL_FRC_GRAYSCALE_16 (0x0 << 24)
#define JZ_LCD_CTRL_FRC_GRAYSCALE_4 (0x1 << 24)
#define JZ_LCD_CTRL_FRC_GRAYSCALE_2 (0x2 << 24)
#define JZ_LCD_CTRL_PDD_MASK (0xff << 16)
#define JZ_LCD_CTRL_EOF_IRQ BIT(13)
#define JZ_LCD_CTRL_SOF_IRQ BIT(12)
#define JZ_LCD_CTRL_OFU_IRQ BIT(11)
#define JZ_LCD_CTRL_IFU0_IRQ BIT(10)
#define JZ_LCD_CTRL_IFU1_IRQ BIT(9)
#define JZ_LCD_CTRL_DD_IRQ BIT(8)
#define JZ_LCD_CTRL_QDD_IRQ BIT(7)
#define JZ_LCD_CTRL_REVERSE_ENDIAN BIT(6)
#define JZ_LCD_CTRL_LSB_FISRT BIT(5)
#define JZ_LCD_CTRL_DISABLE BIT(4)
#define JZ_LCD_CTRL_ENABLE BIT(3)
#define JZ_LCD_CTRL_BPP_1 0x0
#define JZ_LCD_CTRL_BPP_2 0x1
#define JZ_LCD_CTRL_BPP_4 0x2
#define JZ_LCD_CTRL_BPP_8 0x3
#define JZ_LCD_CTRL_BPP_15_16 0x4
#define JZ_LCD_CTRL_BPP_18_24 0x5

#define JZ_LCD_CMD_SOF_IRQ BIT(15)
#define JZ_LCD_CMD_EOF_IRQ BIT(16)
#define JZ_LCD_CMD_ENABLE_PAL BIT(12)

#define JZ_LCD_SYNC_MASK 0x3ff

#define JZ_LCD_STATE_DISABLED BIT(0)

struct jzfb_framedesc {
	uint32_t next;
	uint32_t addr;
	uint32_t id;
	uint32_t cmd;
} __attribute__((packed));

struct jzfb {
	struct fb_info *fb;
	struct platform_device *pdev;
	void __iomem *base;
	struct resource *mem;
	struct jz4740_fb_platform_data *pdata;

	void *devmem;
	size_t devmem_size;
	dma_addr_t devmem_phys;
	void *vidmem;
	size_t vidmem_size;
	dma_addr_t vidmem_phys;
	struct jzfb_framedesc *framedesc;

	struct clk *ldclk;
	struct clk *lpclk;

	uint32_t pseudo_palette[16];
	unsigned is_enabled:1;
};

static struct fb_fix_screeninfo jzfb_fix __devinitdata = {
	.id =		"JZ4740 FB",
	.type =		FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_TRUECOLOR,
	.xpanstep =	0,
	.ypanstep =	0,
	.ywrapstep =	0,
	.accel =	FB_ACCEL_NONE,
};

const static struct jz_gpio_bulk_request jz_lcd_pins[] = {
	JZ_GPIO_BULK_PIN(LCD_PCLK),
	JZ_GPIO_BULK_PIN(LCD_HSYNC),
	JZ_GPIO_BULK_PIN(LCD_VSYNC),
	JZ_GPIO_BULK_PIN(LCD_DATA0),
	JZ_GPIO_BULK_PIN(LCD_DATA1),
	JZ_GPIO_BULK_PIN(LCD_DATA2),
	JZ_GPIO_BULK_PIN(LCD_DATA3),
	JZ_GPIO_BULK_PIN(LCD_DATA4),
	JZ_GPIO_BULK_PIN(LCD_DATA5),
	JZ_GPIO_BULK_PIN(LCD_DATA6),
	JZ_GPIO_BULK_PIN(LCD_DATA7),
};


int jzfb_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue,
			unsigned transp, struct fb_info *fb)
{
	((uint32_t*)fb->pseudo_palette)[regno] = red << 16 | green << 8 | blue;
	return 0;
}

static int jzfb_get_controller_bpp(struct jzfb *jzfb)
{
	switch(jzfb->pdata->bpp) {
	case 18:
	case 24:
		return 32;
	default:
		return jzfb->pdata->bpp;
	}
}

static int jzfb_check_var(struct fb_var_screeninfo *var, struct fb_info *fb)
{
	struct jzfb* jzfb = fb->par;
	struct fb_videomode *mode = jzfb->pdata->modes;
	int i;

	if (fb->var.bits_per_pixel != jzfb_get_controller_bpp(jzfb) &&
		fb->var.bits_per_pixel != jzfb->pdata->bpp)
		return -EINVAL;

	for (i = 0; i < jzfb->pdata->num_modes; ++i, ++mode) {
		if (mode->xres == fb->var.xres && mode->yres == fb->var.yres)
			break;
	}

	if (i == jzfb->pdata->num_modes)
		return -EINVAL;

	fb_videomode_to_var(&fb->var, fb->mode);

	switch (jzfb->pdata->bpp) {
	case 8:
		break;
	case 15:
		var->red.offset = 10;
		var->red.length = 5;
		var->green.offset = 6;
		var->green.length = 5;
		var->blue.offset = 0;
		var->blue.length = 5;
		break;
	case 16:
		var->red.offset = 11;
		var->red.length = 5;
		var->green.offset = 6;
		var->green.length = 6;
		var->blue.offset = 0;
		var->blue.length = 5;
		break;
	case 18:
		var->red.offset = 16;
		var->red.length = 6;
		var->green.offset = 8;
		var->green.length = 6;
		var->blue.offset = 0;
		var->blue.length = 6;
		fb->var.bits_per_pixel = 32;
		break;
	case 32:
	case 24:
		var->transp.offset = 24;
		var->transp.length = 8;
		var->red.offset = 16;
		var->red.length = 8;
		var->green.offset = 8;
		var->green.length = 8;
		var->blue.offset = 0;
		var->blue.length = 8;
		fb->var.bits_per_pixel = 32;
		break;
	default:
		break;
	}

	return 0;
}

static int jzfb_set_par(struct fb_info *info)
{
	struct jzfb* jzfb = info->par;
	struct fb_var_screeninfo *var = &info->var;
	uint16_t hds, vds;
	uint16_t hde, vde;
	uint16_t ht, vt;
	uint32_t ctrl;

	hds = var->hsync_len + var->left_margin;
	hde = hds + var->xres;
	ht = hde + var->right_margin;

	vds = var->vsync_len + var->upper_margin;
	vde = vds + var->yres;
	vt = vde + var->lower_margin;

	writel(var->hsync_len, jzfb->base + JZ_REG_LCD_HSYNC);
	writel(var->vsync_len, jzfb->base + JZ_REG_LCD_VSYNC);

	writel((ht << 16) | vt, jzfb->base + JZ_REG_LCD_VAT);

	writel((hds << 16) | hde, jzfb->base + JZ_REG_LCD_DAH);
	writel((vds << 16) | vde, jzfb->base + JZ_REG_LCD_DAV);

	ctrl = JZ_LCD_CTRL_OFUP | JZ_LCD_CTRL_BURST_16;
	ctrl |= JZ_LCD_CTRL_ENABLE;

	switch (jzfb->pdata->bpp) {
	case 1:
		ctrl |= JZ_LCD_CTRL_BPP_1;
		break;
	case 2:
		ctrl |= JZ_LCD_CTRL_BPP_2;
		break;
	case 4:
		ctrl |= JZ_LCD_CTRL_BPP_4;
		break;
	case 8:
		ctrl |= JZ_LCD_CTRL_BPP_8;
	break;
	case 15:
		ctrl |= JZ_LCD_CTRL_RGB555; /* Falltrough */
	case 16:
		ctrl |= JZ_LCD_CTRL_BPP_15_16;
		break;
	case 18:
	case 24:
	case 32:
		ctrl |= JZ_LCD_CTRL_BPP_18_24;
		break;
	default:
		break;
	}
	writel(ctrl, jzfb->base + JZ_REG_LCD_CTRL);

	return 0;
}

static int jzfb_blank(int blank_mode, struct fb_info *info)
{
	struct jzfb* jzfb = info->par;
	uint32_t ctrl;

	switch (blank_mode) {
	case FB_BLANK_UNBLANK:
		if (jzfb->is_enabled)
			return 0;

		jz_gpio_bulk_resume(jz_lcd_pins, ARRAY_SIZE(jz_lcd_pins));
		clk_enable(jzfb->lpclk);

		writel(0, jzfb->base + JZ_REG_LCD_STATE);

		writel(jzfb->framedesc->next, jzfb->base + JZ_REG_LCD_DA0);

		ctrl = readl(jzfb->base + JZ_REG_LCD_CTRL);
		ctrl |= JZ_LCD_CTRL_ENABLE;
		ctrl &= ~JZ_LCD_CTRL_DISABLE;
		writel(ctrl, jzfb->base + JZ_REG_LCD_CTRL);

		jzfb->is_enabled = 1;
		break;
	default:
		if (!jzfb->is_enabled)
			return 0;

		ctrl = readl(jzfb->base + JZ_REG_LCD_CTRL);
		ctrl |= JZ_LCD_CTRL_DISABLE;
		writel(ctrl, jzfb->base + JZ_REG_LCD_CTRL);
		do {
			ctrl = readl(jzfb->base + JZ_REG_LCD_STATE);
		} while (!(ctrl & JZ_LCD_STATE_DISABLED));

		clk_disable(jzfb->lpclk);
		jz_gpio_bulk_suspend(jz_lcd_pins, ARRAY_SIZE(jz_lcd_pins));
		jzfb->is_enabled = 0;
		break;
	}

	return 0;
}


static int jzfb_alloc_vidmem(struct jzfb *jzfb)
{
	size_t devmem_size;
	int max_videosize = 0;
	struct fb_videomode *mode = jzfb->pdata->modes;
	struct jzfb_framedesc *framedesc;
	void *page;
	int i;

	for (i = 0; i < jzfb->pdata->num_modes; ++mode, ++i) {
		if (max_videosize < mode->xres * mode->yres)
			max_videosize = mode->xres * mode->yres;
	}

	max_videosize *= jzfb_get_controller_bpp(jzfb) >> 3;

	devmem_size = max_videosize + sizeof(struct jzfb_framedesc);

	jzfb->devmem_size = devmem_size;
	jzfb->devmem = dma_alloc_coherent(&jzfb->pdev->dev,
						PAGE_ALIGN(devmem_size),
						&jzfb->devmem_phys, GFP_KERNEL);

	if (!jzfb->devmem) {
		return -ENOMEM;
	}

	for (page = jzfb->vidmem;
		 page < jzfb->vidmem + PAGE_ALIGN(jzfb->vidmem_size);
		 page += PAGE_SIZE) {
		SetPageReserved(virt_to_page(page));
	}


	framedesc = jzfb->devmem  + max_videosize;
	jzfb->vidmem = jzfb->devmem;
	jzfb->vidmem_phys = jzfb->devmem_phys;

	framedesc->next = jzfb->devmem_phys + max_videosize;
	framedesc->addr = jzfb->devmem_phys;
	framedesc->id = 0;
	framedesc->cmd = 0;
	framedesc->cmd |= max_videosize / 4;

	jzfb->framedesc = framedesc;


	return 0;
}

static void jzfb_free_devmem(struct jzfb *jzfb)
{
	dma_free_coherent(&jzfb->pdev->dev, jzfb->devmem_size, jzfb->devmem,
			  jzfb->devmem_phys);
}

static struct  fb_ops jzfb_ops = {
	.owner = THIS_MODULE,
	.fb_check_var = jzfb_check_var,
	.fb_set_par = jzfb_set_par,
	.fb_blank = jzfb_blank,
	.fb_fillrect	= sys_fillrect,
	.fb_copyarea	= sys_copyarea,
	.fb_imageblit	= sys_imageblit,
	.fb_setcolreg = jzfb_setcolreg,
};

static int __devinit jzfb_probe(struct platform_device *pdev)
{
	int ret;
	struct jzfb *jzfb;
	struct fb_info *fb;
	struct jz4740_fb_platform_data *pdata = pdev->dev.platform_data;
	struct resource *mem;

	if (!pdata) {
		dev_err(&pdev->dev, "Missing platform data\n");
		return -ENOENT;
	}

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (!mem) {
		dev_err(&pdev->dev, "Failed to get register memory resource\n");
		return -ENOENT;
	}

	mem = request_mem_region(mem->start, resource_size(mem), pdev->name);

	if (!mem) {
		dev_err(&pdev->dev, "Failed to request register memory region\n");
		return -EBUSY;
	}


	fb = framebuffer_alloc(sizeof(struct jzfb), &pdev->dev);

	if (!fb) {
		dev_err(&pdev->dev, "Failed to allocate framebuffer device\n");
		ret = -ENOMEM;
		goto err_release_mem_region;
	}

	fb->fbops = &jzfb_ops;
	fb->flags = FBINFO_DEFAULT;

	jzfb = fb->par;
	jzfb->pdev = pdev;
	jzfb->pdata = pdata;
	jzfb->mem = mem;

	jzfb->ldclk = clk_get(&pdev->dev, "lcd");
	jzfb->lpclk = clk_get(&pdev->dev, "lcd_pclk");

	jzfb->is_enabled = 1;

	if (IS_ERR(jzfb->ldclk)) {
		ret = PTR_ERR(jzfb->ldclk);
		dev_err(&pdev->dev, "Faild to get device clock: %d\n", ret);
		goto err_framebuffer_release;
	}

	if (IS_ERR(jzfb->lpclk)) {
		ret = PTR_ERR(jzfb->ldclk);
		dev_err(&pdev->dev, "Faild to get pixel clock: %d\n", ret);
		goto err_framebuffer_release;
	}


	jzfb->base = ioremap(mem->start, resource_size(mem));

	if (!jzfb->base) {
		dev_err(&pdev->dev, "Failed to ioremap register memory region\n");
		ret = -EBUSY;
		goto err_framebuffer_release;
	}

	platform_set_drvdata(pdev, jzfb);

	fb_videomode_to_modelist(pdata->modes, pdata->num_modes,
				 &fb->modelist);
	fb->mode = pdata->modes;

	fb_videomode_to_var(&fb->var, fb->mode);
	fb->var.bits_per_pixel = pdata->bpp;
	jzfb_check_var(&fb->var, fb);

	ret = jzfb_alloc_vidmem(jzfb);
	if (ret) {
		dev_err(&pdev->dev, "Failed to allocate video memory\n");
		goto err_iounmap;
	}

	fb->fix = jzfb_fix;
	fb->fix.line_length = fb->var.bits_per_pixel * fb->var.xres / 8;
	fb->fix.mmio_start = mem->start;
	fb->fix.mmio_len = resource_size(mem);
	fb->fix.smem_start = jzfb->vidmem_phys;
	fb->fix.smem_len =  fb->fix.line_length * fb->var.yres;
	fb->screen_base = jzfb->vidmem;
	fb->pseudo_palette = jzfb->pseudo_palette;

	fb_alloc_cmap(&fb->cmap, 256, 0);

	clk_enable(jzfb->ldclk);

	jzfb_set_par(fb);
	writel(jzfb->framedesc->next, jzfb->base + JZ_REG_LCD_DA0);

	jz_gpio_bulk_request(jz_lcd_pins, ARRAY_SIZE(jz_lcd_pins));

	ret = register_framebuffer(fb);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register framebuffer: %d\n", ret);
		goto err_free_devmem;
	}

	return 0;
err_free_devmem:
	jzfb_free_devmem(jzfb);
err_iounmap:
	iounmap(jzfb->base);
err_framebuffer_release:
	framebuffer_release(fb);
err_release_mem_region:
	release_mem_region(mem->start, resource_size(mem));
	return ret;
}

static int __devexit jzfb_remove(struct platform_device *pdev)
{
	struct jzfb *jzfb = platform_get_drvdata(pdev);

	jz_gpio_bulk_free(jz_lcd_pins, ARRAY_SIZE(jz_lcd_pins));
	iounmap(jzfb->base);
	release_mem_region(jzfb->mem->start, resource_size(jzfb->mem));
	jzfb_free_devmem(jzfb);
	platform_set_drvdata(pdev, NULL);
	framebuffer_release(jzfb->fb);
	return 0;
}

#ifdef CONFIG_PM

static int jzfb_suspend(struct device *dev)
{
	struct jzfb *jzfb = dev_get_drvdata(dev);
	clk_disable(jzfb->ldclk);

	return 0;
}

static int jzfb_resume(struct device *dev)
{
	struct jzfb *jzfb = dev_get_drvdata(dev);
	clk_enable(jzfb->ldclk);

	return 0;
}

static const struct dev_pm_ops jzfb_pm_ops = {
	.suspend	= jzfb_suspend,
	.resume		= jzfb_resume,
	.poweroff	= jzfb_suspend,
	.restore	= jzfb_resume,
};

#define JZFB_PM_OPS (&jzfb_pm_ops)

#else
#define JZFB_PM_OPS NULL
#endif

static struct platform_driver jzfb_driver = {
	.probe = jzfb_probe,
	.remove = __devexit_p(jzfb_remove),

	.driver = {
		.name = "jz4740-fb",
		.pm = JZFB_PM_OPS,
	},
};

int __init jzfb_init(void)
{
	return platform_driver_register(&jzfb_driver);
}
module_init(jzfb_init);

void __exit jzfb_exit(void)
{
	platform_driver_unregister(&jzfb_driver);
}
module_exit(jzfb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("JZ4720/JZ4740 SoC LCD framebuffer driver");
MODULE_ALIAS("platform:jz4740-fb");
MODULE_ALIAS("platform:jz4720-fb");
