// SPDX-License-Identifier: GPL-2.0-only

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/types.h>

#include <soc/bcm/bcm3380-fpm.h>

#define BCM3380_FPM_MEM_BITMAP_OFFSET		0x7000

#define BCM3380_FPM_CTRLS			0x0000

// FpmCtrlFpmCtl.Bits.InitMem
#define BCM3380_FPM_CTRL		0x0000
#define BCM3380_FPM_CTRL_INIT_MEM		0x00000010
// FpmCtrlFpmCtl.Bits.Pool1Enable
#define BCM3380_FPM_CTRL_POOL1_ENABLE		0x10000

// FpmCtrlPoolCfg1.Bits.FpBufSize
#define BCM3380_FPM_CTRL_POOLCFG1	0x0040
#define BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_SHIFT	24
#define BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_MASK	0x07000000

// FpmCtrlPoolCfg2.Reg32
#define BCM3380_FPM_CTRL_POOLCFG2	0x0044

// FpmCtrlMemCtl.Reg32 and MemData1
#define BCM3380_FPM_MEMCTL		0x0100
#define BCM3380_FPM_MEMDATA1		0x0104

// Set FpmCtrlMemCtl.Bits.MemWr, FpmCtrlMemCtl.Bits.MemSel=2b11
#define BCM3380_FPM_MEMCTL_WRITE_TOKEN_LIMIT	0xb0000000

// Fpm.FpmPool starts after Fpm.FpmCtrl and Fpm.Pad0.
#define BCM3380_FPM_POOLS			0x0200
// FpmPoolRegs.Pool1AllocDealloc.Reg32
#define BCM3380_FPM_POOL1	(BCM3380_FPM_POOLS + 0x0000)

#define FPM_CTRL_FP_BUF_SIZE_BUF512		0x0
#define FPM_CTRL_FP_BUF_SIZE_BUF768		0x1
#define FPM_CTRL_FP_BUF_SIZE_BUF1024		0x2
#define FPM_CTRL_FP_BUF_SIZE_BUF1280		0x3
#define FPM_CTRL_FP_BUF_SIZE_BUF1536		0x4
#define FPM_CTRL_FP_BUF_SIZE_BUF1792		0x5
#define FPM_CTRL_FP_BUF_SIZE_BUF2048		0x6
#define FPM_CTRL_FP_BUF_SIZE_BUF2304		0x7

struct bcm3380_fpm {
	struct device *dev;
	void __iomem *base; // CPU's virtual address of the FPM registers
	resource_size_t phys; // Physical address of the FPM registers
	struct clk *clk;
	struct reset_control *reset;
	size_t mem_size; // Size of memory allocated for the FPM pool
	void *mem; // CPU's virtual address of the FPM memory
};

static bool fpm_ready(struct bcm3380_fpm *fpm)
{
	u32 ctl;

	if (!fpm || !fpm->mem)
		return false;

	ctl = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);

	return ctl & BCM3380_FPM_CTRL_POOL1_ENABLE;
}

static size_t fpm_buf_size_from_code(u32 code)
{
	switch (code) {
	case FPM_CTRL_FP_BUF_SIZE_BUF512:
		return 512;
	case FPM_CTRL_FP_BUF_SIZE_BUF768:
		return 768;
	case FPM_CTRL_FP_BUF_SIZE_BUF1024:
		return 1024;
	case FPM_CTRL_FP_BUF_SIZE_BUF1280:
		return 1280;
	case FPM_CTRL_FP_BUF_SIZE_BUF1536:
		return 1536;
	case FPM_CTRL_FP_BUF_SIZE_BUF1792:
		return 1792;
	case FPM_CTRL_FP_BUF_SIZE_BUF2048:
		return 2048;
	case FPM_CTRL_FP_BUF_SIZE_BUF2304:
		return 2304;
	default:
		return 0;
	}
}

static u32 fpm_buf_size_to_code(u32 size)
{
	switch (size) {
	case 512:
		return FPM_CTRL_FP_BUF_SIZE_BUF512;
	case 768:
		return FPM_CTRL_FP_BUF_SIZE_BUF768;
	case 1024:
		return FPM_CTRL_FP_BUF_SIZE_BUF1024;
	case 1280:
		return FPM_CTRL_FP_BUF_SIZE_BUF1280;
	case 1536:
		return FPM_CTRL_FP_BUF_SIZE_BUF1536;
	case 1792:
		return FPM_CTRL_FP_BUF_SIZE_BUF1792;
	case 2048:
		return FPM_CTRL_FP_BUF_SIZE_BUF2048;
	case 2304:
		return FPM_CTRL_FP_BUF_SIZE_BUF2304;
	default:
		return (u32)-1;
	}
}

static int fpm_wait_init_done(struct bcm3380_fpm *fpm)
{
	unsigned int i;

	for (i = 0; i < 10000; i++) {
		if (!(readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL) &
		      BCM3380_FPM_CTRL_INIT_MEM))
			return 0;

		udelay(1);
	}

	return -ETIMEDOUT;
}

static int fpm_write_token_limit(struct bcm3380_fpm *fpm, u32 limit)
{
	u32 bitmap[10];
	void __iomem *bitmap_regs = fpm->base + BCM3380_FPM_MEM_BITMAP_OFFSET;
	unsigned int i;

	if (limit > 0x03ffffff) {
		dev_err(fpm->dev, "FPM token limit is too large: %u\n", limit);
		return -EINVAL;
	}

	for (i = 0; i < ARRAY_SIZE(bitmap); i++)
		bitmap[i] = ~0U;

	for (i = 0; i < (limit >> 8); i++)
		bitmap[1 + (i >> 5)] &= ~BIT(i & 0x1f);

	for (i = 0; i < 8; i++) {
		if (bitmap[i + 1] != ~0U)
			bitmap[0] &= ~BIT(i);
	}

	writel_be(bitmap[0], bitmap_regs);
	for (i = 1; i < 9; i++)
		writel_be(bitmap[i], bitmap_regs + ((i + 1) * sizeof(u32)));

	writel_be(limit, fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_MEMDATA1);
	writel_be(BCM3380_FPM_MEMCTL_WRITE_TOKEN_LIMIT,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_MEMCTL);
	mdelay(10);

	return 0;
}

static int fpm_init_pool1(struct bcm3380_fpm *fpm)
{
	struct device *dev = fpm->dev;
	u32 buffer_size;

	int ret = of_property_read_u32(dev->of_node, "brcm,pool-buffer-size",
				   &buffer_size);
	if (ret) {
		dev_err(dev, "missing brcm,pool-buffer-size\n");
		return ret;
	}

	u32 token_limit;
	ret = of_property_read_u32(dev->of_node, "brcm,pool-token-limit",
				   &token_limit);
	if (ret) {
		dev_err(dev, "missing brcm,pool-token-limit\n");
		return ret;
	}

	u32 buffer_size_code = fpm_buf_size_to_code(buffer_size);
	if (buffer_size_code == (u32)-1) {
		dev_err(dev, "unsupported FPM pool buffer size: %u\n",
			buffer_size);
		return -EINVAL;
	}

	if (!token_limit || token_limit > 0x03ffffff) {
		dev_err(dev, "invalid FPM pool token limit: %u\n", token_limit);
		return -EINVAL;
	}

	if ((u8)token_limit) {
		token_limit = (token_limit + 255) & 0xffffff00;
		dev_warn(dev,
			 "FPM token limit must be a multiple of 256, rounded to %u\n",
			 token_limit);
	}

	if (token_limit > 0x03ffffff) {
		dev_err(dev, "rounded FPM pool token limit is too large: %u\n",
			token_limit);
		return -EINVAL;
	}

	if (token_limit > SIZE_MAX / buffer_size) {
		dev_err(dev,
			"FPM pool memory size would overflow: token_limit=%u buffer_size=%u\n",
			token_limit, buffer_size);
		return -EINVAL;
	}

	dma_addr_t dma;
	fpm->mem_size = (size_t)buffer_size * token_limit;
	fpm->mem = dma_alloc_coherent(dev, fpm->mem_size, &dma, GFP_KERNEL);
	if (!fpm->mem)
		return -ENOMEM;

	writel_be(BCM3380_FPM_CTRL_INIT_MEM,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);
	ret = fpm_wait_init_done(fpm);
	if (ret) {
		dev_err(dev, "FPM InitMem did not complete\n");
		goto free_pool_mem;
	}

	writel_be(buffer_size_code << BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_SHIFT,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG1);
	writel_be((u32)dma, fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG2);

	ret = fpm_write_token_limit(fpm, token_limit);
	if (ret)
		goto free_pool_mem;

	writel_be(BCM3380_FPM_CTRL_POOL1_ENABLE,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);

	dev_info(dev,
		 "FPM pool1 ready: dma=%pad cpu=%p size=%zu token_limit=%u alloc_bus=0x%08x\n",
		 &dma, fpm->mem, fpm->mem_size, token_limit,
		 fpm_alloc_free_bus_addr(fpm));

	return 0;

free_pool_mem:
	dma_free_coherent(dev, fpm->mem_size, fpm->mem, dma);
	fpm->mem = NULL;
	return ret;
}

static void fpm_disable_pool1(struct bcm3380_fpm *fpm)
{
	dma_addr_t dma = fpm_buffer_base_dma(fpm);
	u32 ctrl = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);

	writel_be(ctrl & ~BCM3380_FPM_CTRL_POOL1_ENABLE,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);

	if (fpm->mem) {
		dma_free_coherent(fpm->dev, fpm->mem_size, fpm->mem,
				  dma);
		fpm->mem = NULL;
	}
}

int fpm_get(struct device *consumer, struct bcm3380_fpm **fpm)
{
	struct platform_device *pdev;
	struct device_node *np;
	struct bcm3380_fpm *provider;

	if (!consumer || !consumer->of_node || !fpm)
		return -EINVAL;

	np = of_parse_phandle(consumer->of_node, "fpm", 0);
	if (!np)
		return -ENODEV;

	pdev = of_find_device_by_node(np);
	of_node_put(np);
	if (!pdev)
		return -EPROBE_DEFER;

	provider = platform_get_drvdata(pdev);
	if (!provider) {
		put_device(&pdev->dev);
		return -EPROBE_DEFER;
	}

	if (!fpm_ready(provider)) {
		put_device(&pdev->dev);
		return -EINVAL;
	}

	*fpm = provider;

	return 0;
}
EXPORT_SYMBOL_GPL(fpm_get);

void fpm_put(struct bcm3380_fpm *fpm)
{
	if (fpm && fpm->dev)
		put_device(fpm->dev);
}
EXPORT_SYMBOL_GPL(fpm_put);

dma_addr_t fpm_buffer_base_dma(struct bcm3380_fpm *fpm)
{
	if (!fpm_ready(fpm))
		return 0;

	return readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG2);
}
EXPORT_SYMBOL_GPL(fpm_buffer_base_dma);

u32 fpm_buffer_size_code(struct bcm3380_fpm *fpm)
{
	u32 cfg1;

	if (!fpm_ready(fpm))
		return 0;

	cfg1 = readl_be(fpm->base + BCM3380_FPM_CTRLS +
			BCM3380_FPM_CTRL_POOLCFG1);

	return (cfg1 & BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_MASK) >>
	       BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_SHIFT;
}
EXPORT_SYMBOL_GPL(fpm_buffer_size_code);

u32 fpm_alloc_free_bus_addr(struct bcm3380_fpm *fpm)
{
	if (!fpm)
		return 0;

	return fpm->phys + BCM3380_FPM_POOL1;
}
EXPORT_SYMBOL_GPL(fpm_alloc_free_bus_addr);

u32 fpm_alloc_token(struct bcm3380_fpm *fpm)
{
	if (!fpm_ready(fpm))
		return 0;

	return readl_be(fpm->base + BCM3380_FPM_POOL1);
}
EXPORT_SYMBOL_GPL(fpm_alloc_token);

void fpm_free_token(struct bcm3380_fpm *fpm, u32 token)
{
	if (!fpm_ready(fpm))
		return;

	writel_be(token, fpm->base + BCM3380_FPM_POOL1);
}
EXPORT_SYMBOL_GPL(fpm_free_token);

void *fpm_token_to_virt(struct bcm3380_fpm *fpm, u32 token)
{
	if (!fpm_ready(fpm))
		return NULL;

	if (!fpm_token_valid(token))
		return NULL;

	u32 cfg1 = readl_be(fpm->base + BCM3380_FPM_CTRLS +
			BCM3380_FPM_CTRL_POOLCFG1);
	size_t buf_size = fpm_buf_size_from_code((cfg1 &
					   BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_MASK) >>
					  BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_SHIFT);
	if (!buf_size)
		return NULL;

	size_t offset = fpm_token_index(token) * buf_size;
	if (offset >= fpm->mem_size)
		return NULL;

	return fpm->mem + offset;
}
EXPORT_SYMBOL_GPL(fpm_token_to_virt);

static int fpm_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	int ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret)
		return ret;

	struct bcm3380_fpm *fpm = devm_kzalloc(dev, sizeof(*fpm), GFP_KERNEL);
	if (!fpm)
		return -ENOMEM;

	fpm->dev = dev;

	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	fpm->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(fpm->base))
		return PTR_ERR(fpm->base);
	fpm->phys = res->start;

	fpm->clk = devm_clk_get_optional(dev, "fpm");
	if (IS_ERR(fpm->clk))
		return PTR_ERR(fpm->clk);

	if (fpm->clk) {
		ret = clk_prepare_enable(fpm->clk);
		if (ret)
			return ret;
	}

	fpm->reset = devm_reset_control_get_optional_exclusive(dev, "fpm");
	if (IS_ERR(fpm->reset)) {
		ret = PTR_ERR(fpm->reset);
		goto disable_clk;
	}

	if (fpm->reset) {
		ret = reset_control_reset(fpm->reset);
		if (ret)
			goto disable_clk;
	}

	ret = fpm_init_pool1(fpm);
	if (ret)
		goto assert_reset;

	platform_set_drvdata(pdev, fpm);

	return 0;

assert_reset:
	if (fpm->reset)
		reset_control_assert(fpm->reset);
disable_clk:
	if (fpm->clk)
		clk_disable_unprepare(fpm->clk);
	return ret;
}

static void fpm_remove(struct platform_device *pdev)
{
	struct bcm3380_fpm *fpm = platform_get_drvdata(pdev);

	fpm_disable_pool1(fpm);

	if (fpm->reset)
		reset_control_assert(fpm->reset);

	if (fpm->clk)
		clk_disable_unprepare(fpm->clk);
}

static const struct of_device_id bcm3380_fpm_of_match[] = {
	{ .compatible = "brcm,bcm3380-fpm" },
	{ }
};
MODULE_DEVICE_TABLE(of, bcm3380_fpm_of_match);

static struct platform_driver bcm3380_fpm_driver = {
	.probe = fpm_probe,
	.remove = fpm_remove,
	.driver = {
		.name = "bcm3380-fpm",
		.of_match_table = bcm3380_fpm_of_match,
	},
};
module_platform_driver(bcm3380_fpm_driver);

MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_DESCRIPTION("BCM3380 Free Pool Manager driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:bcm3380-fpm");
