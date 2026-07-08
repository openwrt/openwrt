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

#define BCM3380_FPM_NUM_POOLS			4
#define BCM3380_FPM_MEM_BITMAP_OFFSET		0x7000

#define BCM3380_FPM_CTRLS			0x0000

// FpmCtrlFpmCtl.Bits.InitMem
#define BCM3380_FPM_CTRL		0x0000
#define BCM3380_FPM_CTRL_INIT_MEM		0x00000010
// FpmCtrlFpmCtl.Bits.Pool1Enable..Pool4Enable
#define BCM3380_FPM_CTRL_POOL_ENABLE_SHIFT	16
#define BCM3380_FPM_CTRL_POOL_ENABLE_MASK	0x000f0000

// FpmCtrlFpmCfg1.Reg32
#define BCM3380_FPM_CTRL_CFG1		0x0004
#define BCM3380_FPM_CTRL_CFG1_SEARCH_MODE_SHIFT	0
#define BCM3380_FPM_CTRL_CFG1_CACHE_BYPASS_SHIFT	4
#define BCM3380_FPM_CTRL_CFG1_POOL_BITS_MASK	0x000000ff

// FpmCtrlPoolCfg1.Bits.FpBufSize
#define BCM3380_FPM_CTRL_POOLCFG1	0x0040
#define BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_SHIFT	24
#define BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_MASK	0x07000000

// FpmCtrlPoolCfg2.Reg32
#define BCM3380_FPM_CTRL_POOLCFG2	0x0044

// FpmCtrlFpmXonXoffCfg.Reg32
#define BCM3380_FPM_CTRL_XON_XOFF	0x00c0
#define BCM3380_FPM_CTRL_XON_SHIFT	16
#define BCM3380_FPM_CTRL_XON_XOFF_THRESHOLD_MAX	0xffff

// FpmCtrlMemCtl.Reg32 and MemData1
#define BCM3380_FPM_MEMCTL		0x0100
#define BCM3380_FPM_MEMDATA1		0x0104

// Set FpmCtrlMemCtl.Bits.MemWr, FpmCtrlMemCtl.Bits.MemSel=2b11
#define BCM3380_FPM_MEMCTL_WRITE_TOKEN_LIMIT	0xb0000000

// Fpm.FpmPool starts after Fpm.FpmCtrl and Fpm.Pad0.
#define BCM3380_FPM_POOLS			0x0200
// FpmPoolRegs.Pool1AllocDealloc.Reg32..Pool4AllocDealloc.Reg32
#define BCM3380_FPM_POOL_ALLOC_STRIDE		0x0004

#define FPM_CTRL_FP_BUF_SIZE_BUF512		0x0
#define FPM_CTRL_FP_BUF_SIZE_BUF768		0x1
#define FPM_CTRL_FP_BUF_SIZE_BUF1024		0x2
#define FPM_CTRL_FP_BUF_SIZE_BUF1280		0x3
#define FPM_CTRL_FP_BUF_SIZE_BUF1536		0x4
#define FPM_CTRL_FP_BUF_SIZE_BUF1792		0x5
#define FPM_CTRL_FP_BUF_SIZE_BUF2048		0x6
#define FPM_CTRL_FP_BUF_SIZE_BUF2304		0x7

struct bcm3380_fpm;

struct bcm3380_fpm_pool {
	struct bcm3380_fpm *fpm;
	unsigned int id;
	bool configured;
	bool enabled;
	u32 cache_bypass;
	u32 search_mode;
};

struct bcm3380_fpm {
	struct device *dev;
	void __iomem *base; // CPU's virtual address of the FPM registers
	resource_size_t phys; // Physical address of the FPM registers
	struct clk *clk;
	struct reset_control *reset;
	struct bcm3380_fpm_pool pools[BCM3380_FPM_NUM_POOLS];
	size_t mem_size; // Total memory allocated for all enabled FPM pools
	void *mem; // CPU's virtual address of the FPM memory
	dma_addr_t mem_dma;
};

static u32 fpm_pool_enable_bit(unsigned int id)
{
	return BIT(BCM3380_FPM_CTRL_POOL_ENABLE_SHIFT + id);
}

static u32 fpm_pool_alloc_reg(unsigned int id)
{
	return BCM3380_FPM_POOLS + id * BCM3380_FPM_POOL_ALLOC_STRIDE;
}

static bool fpm_pool_ready(struct bcm3380_fpm_pool *pool)
{
	struct bcm3380_fpm *fpm;
	u32 ctl;

	if (!pool || !pool->enabled || !pool->fpm)
		return false;

	fpm = pool->fpm;
	if (!fpm->mem)
		return false;

	ctl = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);

	return ctl & fpm_pool_enable_bit(pool->id);
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

static u32 fpm_ctrl_cfg1_from_pools(struct bcm3380_fpm *fpm)
{
	u32 cfg1 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_CFG1);
	unsigned int i;

	cfg1 &= ~BCM3380_FPM_CTRL_CFG1_POOL_BITS_MASK;

	for (i = 0; i < BCM3380_FPM_NUM_POOLS; i++) {
		struct bcm3380_fpm_pool *pool = &fpm->pools[i];

		if (!pool->configured)
			continue;

		if (pool->search_mode)
			cfg1 |= BIT(BCM3380_FPM_CTRL_CFG1_SEARCH_MODE_SHIFT + i);

		if (pool->cache_bypass)
			cfg1 |= BIT(BCM3380_FPM_CTRL_CFG1_CACHE_BYPASS_SHIFT + i);
	}

	return cfg1;
}

static int fpm_init_pools(struct bcm3380_fpm *fpm)
{
	struct device *dev = fpm->dev;
	u32 buffer_size;

	int ret = of_property_read_u32(dev->of_node, "brcm,pool-buffer-size",
				   &buffer_size);
	if (ret) {
		dev_err(dev, "missing brcm,pool-buffer-size\n");
		return ret;
	}

	u32 per_pool_token_limit;
	ret = of_property_read_u32(dev->of_node, "brcm,pool-token-limit",
				   &per_pool_token_limit);
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

	if (!per_pool_token_limit || per_pool_token_limit > 0x03ffffff) {
		dev_err(dev, "invalid FPM pool token limit: %u\n",
			per_pool_token_limit);
		return -EINVAL;
	}

	if ((u8)per_pool_token_limit) {
		per_pool_token_limit = (per_pool_token_limit + 255) & 0xffffff00;
		dev_warn(dev,
			 "FPM token limit must be a multiple of 256, rounded to %u\n",
			 per_pool_token_limit);
	}

	if (per_pool_token_limit > 0x03ffffff) {
		dev_err(dev, "rounded FPM pool token limit is too large: %u\n",
			per_pool_token_limit);
		return -EINVAL;
	}

	if (per_pool_token_limit > SIZE_MAX / buffer_size) {
		dev_err(dev,
			"FPM pool memory size would overflow: token_limit=%u buffer_size=%u\n",
			per_pool_token_limit, buffer_size);
		return -EINVAL;
	}

	unsigned int enabled_pool_count = 0;
	u32 enabled_pool_mask = 0;
	for (unsigned int i = 0; i < BCM3380_FPM_NUM_POOLS; i++) {
		struct bcm3380_fpm_pool *pool = &fpm->pools[i];

		if (!pool->enabled)
			continue;

		enabled_pool_count++;
		enabled_pool_mask |= fpm_pool_enable_bit(pool->id);
	}

	if (!enabled_pool_count) {
		dev_err(dev, "no enabled FPM pools\n");
		return -EINVAL;
	}

	if (enabled_pool_count > 0x03ffffff / per_pool_token_limit) {
		dev_err(dev,
			"FPM total token limit would exceed hardware limit: enabled_pools=%u per_pool_token_limit=%u\n",
			enabled_pool_count, per_pool_token_limit);
		return -EINVAL;
	}
	u32 total_token_limit = per_pool_token_limit * enabled_pool_count;

	size_t pool_mem_size = (size_t)buffer_size * per_pool_token_limit;
	if (enabled_pool_count > SIZE_MAX / pool_mem_size) {
		dev_err(dev,
			"FPM total memory size would overflow: enabled_pools=%u pool_mem_size=%zu\n",
			enabled_pool_count, pool_mem_size);
		return -EINVAL;
	}

	fpm->mem_size = pool_mem_size * enabled_pool_count;
	fpm->mem = dma_alloc_coherent(dev, fpm->mem_size, &fpm->mem_dma, GFP_KERNEL);
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
	writel_be((u32)fpm->mem_dma, fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG2);
	writel_be(fpm_ctrl_cfg1_from_pools(fpm),
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_CFG1);

	ret = fpm_write_token_limit(fpm, total_token_limit);
	if (ret)
		goto free_pool_mem;

	writel_be((readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL) & ~BCM3380_FPM_CTRL_POOL_ENABLE_MASK) | enabled_pool_mask,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);

	dev_info(dev,
		 "FPM ready: dma=%pad cpu=%p total_size=%zu pool_size=%zu enabled_pools=%u per_pool_token_limit=%u total_token_limit=%u\n",
		 &fpm->mem_dma, fpm->mem, fpm->mem_size, pool_mem_size,
		 enabled_pool_count, per_pool_token_limit, total_token_limit);

	return 0;

free_pool_mem:
	dma_free_coherent(dev, fpm->mem_size, fpm->mem, fpm->mem_dma);
	fpm->mem = NULL;
	fpm->mem_dma = 0;
	return ret;
}

static void fpm_disable_pools(struct bcm3380_fpm *fpm)
{
	u32 ctrl = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);

	writel_be(ctrl & ~BCM3380_FPM_CTRL_POOL_ENABLE_MASK,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);

	if (fpm->mem) {
		dma_free_coherent(fpm->dev, fpm->mem_size, fpm->mem,
				  fpm->mem_dma);
		fpm->mem = NULL;
		fpm->mem_dma = 0;
	}
}

static int fpm_parse_pools(struct bcm3380_fpm *fpm)
{
	struct device_node *child;
	unsigned int child_count = 0;

	for (unsigned int i = 0; i < BCM3380_FPM_NUM_POOLS; i++) {
		fpm->pools[i].fpm = fpm;
		fpm->pools[i].id = i;
	}

	for_each_child_of_node(fpm->dev->of_node, child) {
		child_count++;
		u32 reg;
		int ret = of_property_read_u32(child, "reg", &reg);
		if (ret) {
			dev_err(fpm->dev, "%pOF: missing reg\n", child);
			of_node_put(child);
			return ret;
		}

		if (reg < 1 || reg > BCM3380_FPM_NUM_POOLS) {
			dev_err(fpm->dev, "%pOF: invalid pool reg %u\n", child,
				reg);
			of_node_put(child);
			return -EINVAL;
		}

		struct bcm3380_fpm_pool *pool = &fpm->pools[reg - 1];
		if (pool->configured) {
			dev_err(fpm->dev, "%pOF: duplicate FPM pool %u\n",
				child, reg);
			of_node_put(child);
			return -EINVAL;
		}

		u32 cache_bypass;
		ret = of_property_read_u32(child, "brcm,cache-bypass",
					   &cache_bypass);
		if (ret) {
			dev_err(fpm->dev, "%pOF: missing brcm,cache-bypass\n",
				child);
			of_node_put(child);
			return ret;
		}

		u32 search_mode;
		ret = of_property_read_u32(child, "brcm,search-mode",
					   &search_mode);
		if (ret) {
			dev_err(fpm->dev, "%pOF: missing brcm,search-mode\n",
				child);
			of_node_put(child);
			return ret;
		}

		if (cache_bypass > 1 || search_mode > 1) {
			dev_err(fpm->dev, "%pOF: invalid cache/search config cache_bypass=%u search_mode=%u\n",
				child, cache_bypass, search_mode);
			of_node_put(child);
			return -EINVAL;
		}

		pool->configured = true;
		pool->enabled = of_device_is_available(child);
		pool->cache_bypass = cache_bypass;
		pool->search_mode = search_mode;
	}

	if (!child_count) {
		dev_err(fpm->dev, "missing FPM pool child nodes\n");
		return -EINVAL;
	}

	return 0;
}

int fpm_pool_get(struct device *consumer, struct bcm3380_fpm_pool **pool)
{
	if (!consumer || !consumer->of_node || !pool)
		return -EINVAL;

	struct device_node * pool_np = of_parse_phandle(consumer->of_node, "brcm,fpm_pool", 0);
	if (!pool_np)
		return -ENODEV;

	u32 reg;
	int ret = of_property_read_u32(pool_np, "reg", &reg);
	if (ret)
		goto put_pool_node;

	if (reg < 1 || reg > BCM3380_FPM_NUM_POOLS) {
		ret = -EINVAL;
		goto put_pool_node;
	}

	struct device_node *fpm_np = of_get_parent(pool_np);
	if (!fpm_np) {
		ret = -EINVAL;
		goto put_pool_node;
	}

	struct platform_device *pdev = of_find_device_by_node(fpm_np);
	of_node_put(fpm_np);
	if (!pdev) {
		ret = -EPROBE_DEFER;
		goto put_pool_node;
	}

	struct bcm3380_fpm *provider = platform_get_drvdata(pdev);
	if (!provider) {
		put_device(&pdev->dev);
		ret = -EPROBE_DEFER;
		goto put_pool_node;
	}

	if (!provider->pools[reg - 1].enabled ||
	    !fpm_pool_ready(&provider->pools[reg - 1])) {
		put_device(&pdev->dev);
		ret = -EINVAL;
		goto put_pool_node;
	}

	*pool = &provider->pools[reg - 1];
	of_node_put(pool_np);
	return 0;

put_pool_node:
	of_node_put(pool_np);
	return ret;
}
EXPORT_SYMBOL_GPL(fpm_pool_get);

void fpm_pool_put(struct bcm3380_fpm_pool *pool)
{
	if (pool && pool->fpm && pool->fpm->dev)
		put_device(pool->fpm->dev);
}
EXPORT_SYMBOL_GPL(fpm_pool_put);

dma_addr_t fpm_buffer_base_dma(struct bcm3380_fpm_pool *pool)
{
	if (!fpm_pool_ready(pool))
		return 0;

	struct bcm3380_fpm * fpm = pool->fpm;

	return readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG2);
}
EXPORT_SYMBOL_GPL(fpm_buffer_base_dma);

u32 fpm_buffer_size_code(struct bcm3380_fpm_pool *pool)
{
	if (!fpm_pool_ready(pool))
		return 0;

	struct bcm3380_fpm * fpm = pool->fpm;
	u32 cfg1 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG1);

	return (cfg1 & BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_MASK) >>
	       BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_SHIFT;
}
EXPORT_SYMBOL_GPL(fpm_buffer_size_code);

u32 fpm_alloc_free_bus_addr(struct bcm3380_fpm_pool *pool)
{
	if (!fpm_pool_ready(pool))
		return 0;

	return pool->fpm->phys + fpm_pool_alloc_reg(pool->id);
}
EXPORT_SYMBOL_GPL(fpm_alloc_free_bus_addr);

u32 fpm_borrow_token(struct bcm3380_fpm_pool *pool)
{
	if (!fpm_pool_ready(pool))
		return 0;

	return readl_be(pool->fpm->base + fpm_pool_alloc_reg(pool->id));
}
EXPORT_SYMBOL_GPL(fpm_borrow_token);

void fpm_return_token(struct bcm3380_fpm_pool *pool, u32 token)
{
	if (!fpm_pool_ready(pool))
		return;

	writel_be(token, pool->fpm->base + fpm_pool_alloc_reg(pool->id));
}
EXPORT_SYMBOL_GPL(fpm_return_token);

void *fpm_token_to_virt(struct bcm3380_fpm_pool *pool, u32 token)
{
	if (!fpm_pool_ready(pool))
		return NULL;

	if (!fpm_token_valid(token))
		return NULL;

	struct bcm3380_fpm * fpm = pool->fpm;
	u32 cfg1 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG1);
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

	ret = fpm_parse_pools(fpm);
	if (ret)
		goto assert_reset;

	u32 pool_xon;
	ret = of_property_read_u32(dev->of_node, "brcm,pool-xon", &pool_xon);
	if (ret) {
		dev_err(dev, "missing brcm,pool-xon\n");
		goto assert_reset;
	}

	u32 pool_xoff;
	ret = of_property_read_u32(dev->of_node, "brcm,pool-xoff", &pool_xoff);
	if (ret) {
		dev_err(dev, "missing brcm,pool-xoff\n");
		goto assert_reset;
	}

	if (pool_xon > BCM3380_FPM_CTRL_XON_XOFF_THRESHOLD_MAX ||
	    pool_xoff > BCM3380_FPM_CTRL_XON_XOFF_THRESHOLD_MAX) {
		dev_err(dev, "invalid FPM pool XON/XOFF thresholds: xon=%u xoff=%u\n",
			pool_xon, pool_xoff);
		ret = -EINVAL;
		goto assert_reset;
	}

	writel_be((pool_xon << BCM3380_FPM_CTRL_XON_SHIFT) | pool_xoff,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_XON_XOFF);

	ret = fpm_init_pools(fpm);
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

	fpm_disable_pools(fpm);

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
