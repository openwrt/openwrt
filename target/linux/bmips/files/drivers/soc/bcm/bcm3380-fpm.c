// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3380 FPM(Free Pool Manager) Driver
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/types.h>

#include <soc/bcm/bcm3380-fpm.h>

#define BCM3380_FPM_NUM_POOLS			4
#define BCM3380_FPM_MEM_BITMAP_OFFSET		0x7000
#define BCM3380_FPM_TOKEN_LIMIT_BITMAP_LEAF_OFFSET	0x0020
#define BCM3380_FPM_TOKEN_LIMIT_BITMAP_LEAF_WORDS	8
#define BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUP_TOKENS	256
#define BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUPS_PER_WORD	32
#define BCM3380_FPM_TOKEN_LIMIT_BITMAP_MAX_TOKENS	\
	(BCM3380_FPM_TOKEN_LIMIT_BITMAP_LEAF_WORDS *	\
	 BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUPS_PER_WORD * \
	 BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUP_TOKENS)

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

// FpmCtrlPoolStat1.Reg32
#define BCM3380_FPM_CTRL_POOLSTAT1	0x0050
#define BCM3380_FPM_CTRL_POOLSTAT1_OVRFL_SHIFT	16
#define BCM3380_FPM_CTRL_POOLSTAT1_UNDRFL_MASK	GENMASK(15, 0)

// FpmCtrlPoolStat2.Reg32
#define BCM3380_FPM_CTRL_POOLSTAT2	0x0054
#define BCM3380_FPM_CTRL_POOLSTAT2_POOL_FULL	BIT(31)
#define BCM3380_FPM_CTRL_POOLSTAT2_FREE_FIFO_FULL	BIT(29)
#define BCM3380_FPM_CTRL_POOLSTAT2_FREE_FIFO_EMPTY	BIT(28)
#define BCM3380_FPM_CTRL_POOLSTAT2_ALLOC_FIFO_FULL	BIT(27)
#define BCM3380_FPM_CTRL_POOLSTAT2_ALLOC_FIFO_EMPTY	BIT(26)
#define BCM3380_FPM_CTRL_POOLSTAT2_TOKEN_AVAIL_MASK	GENMASK(17, 0)

// FpmCtrlPoolStat3.Reg32
#define BCM3380_FPM_CTRL_POOLSTAT3	0x0058
#define BCM3380_FPM_CTRL_POOLSTAT3_INVALID_FREE_MASK	GENMASK(17, 0)

// FpmCtrlPoolStat4.Reg32
#define BCM3380_FPM_CTRL_POOLSTAT4	0x005c
#define BCM3380_FPM_CTRL_POOLSTAT4_INVALID_MULTI_MASK	GENMASK(17, 0)

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

struct variant_data {
	u32 pool_alloc_stride;
	u32 token_index_mask;
	bool multi_size_alloc_regs;
	bool poolcfg1_has_buffer_size;
	bool token_limit_bitmap;
};

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
	const struct variant_data *variant;
	struct clk *clk;
	struct reset_control *reset;
	struct bcm3380_fpm_pool pools[BCM3380_FPM_NUM_POOLS];
	size_t token_stride;
	size_t mem_size; // Total memory allocated for all enabled FPM pools
	void *mem; // CPU's virtual address of the FPM memory
	dma_addr_t mem_dma;
};

static const struct variant_data bcm3380_fpm_data = {
	.pool_alloc_stride = 4,
	.token_index_mask = 0x0003ffff,
	.poolcfg1_has_buffer_size = true,
	.token_limit_bitmap = true,
};

static const struct variant_data bcm3383_fpm_data = {
	.pool_alloc_stride = 8,
	.token_index_mask = 0x0000ffff,
	.multi_size_alloc_regs = true,
	/*
	 * BCM3383 GPL headers place FpmMulti at offset 0x7000, where BCM3380
	 * uses the token-limit bitmap.  The BCM3383 bootloader does not write
	 * that bitmap path during Ethernet FPM init.
	 */
	.token_limit_bitmap = false,
};

static u32 fpm_pool_enable_bit(unsigned int id)
{
	return BIT(BCM3380_FPM_CTRL_POOL_ENABLE_SHIFT + id);
}

static u32 fpm_pool_alloc_reg(struct bcm3380_fpm *fpm, unsigned int id)
{
	return BCM3380_FPM_POOLS + id * fpm->variant->pool_alloc_stride;
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

static unsigned int fpm_bcm3383_alloc_reg_id_for_size(size_t size)
{
	switch (size) {
	case 2048:
		return 0;
	case 1024:
		return 1;
	case 512:
		return 2;
	case 256:
		return 3;
	default:
		return BCM3380_FPM_NUM_POOLS;
	}
}

static u32 fpm_buf_size_to_code(u32 size)
{
	switch (size) {
	case 512:
		return 0x0;
	case 768:
		return 0x1;
	case 1024:
		return 0x2;
	case 1280:
		return 0x3;
	case 1536:
		return 0x4;
	case 1792:
		return 0x5;
	case 2048:
		return 0x6;
	case 2304:
		return 0x7;
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
	void __iomem *bitmap_regs = fpm->base + BCM3380_FPM_MEM_BITMAP_OFFSET;
	u32 summary = ~0U;
	u32 leaf[BCM3380_FPM_TOKEN_LIMIT_BITMAP_LEAF_WORDS];

	if (limit > BCM3380_FPM_TOKEN_LIMIT_BITMAP_MAX_TOKENS) {
		dev_err(fpm->dev, "FPM token limit is too large: %u\n", limit);
		return -EINVAL;
	}

	if (limit % BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUP_TOKENS) {
		dev_err(fpm->dev, "FPM token limit must be a multiple of %u: %u\n",
			BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUP_TOKENS, limit);
		return -EINVAL;
	}

	for (unsigned int i = 0; i < ARRAY_SIZE(leaf); i++)
		leaf[i] = ~0U;

	for (unsigned int i = 0;
	     i < limit / BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUP_TOKENS; i++)
		leaf[i / BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUPS_PER_WORD] &=
			~BIT(i % BCM3380_FPM_TOKEN_LIMIT_BITMAP_GROUPS_PER_WORD);

	for (unsigned int i = 0; i < ARRAY_SIZE(leaf); i++)
		if (leaf[i] != ~0U)
			summary &= ~BIT(i);

	writel_be(summary, bitmap_regs);
	for (unsigned int i = 0; i < ARRAY_SIZE(leaf); i++)
		writel_be(leaf[i],
			  bitmap_regs + BCM3380_FPM_TOKEN_LIMIT_BITMAP_LEAF_OFFSET + i * sizeof(u32));

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

	u32 buffer_size_code = 0;
	if (fpm->variant->poolcfg1_has_buffer_size) {
		buffer_size_code = fpm_buf_size_to_code(buffer_size);
		if (buffer_size_code == (u32)-1) {
			dev_err(dev, "unsupported FPM pool buffer size: %u\n",
				buffer_size);
			return -EINVAL;
		}
	} else if (buffer_size != 256) {
		dev_err(dev, "unsupported BCM3383 FPM base token size: %u\n",
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
	if (total_token_limit > BCM3380_FPM_TOKEN_LIMIT_BITMAP_MAX_TOKENS) {
		dev_err(dev,
			"FPM total token limit exceeds bitmap capacity: enabled_pools=%u per_pool_token_limit=%u total_token_limit=%u max=%u\n",
			enabled_pool_count, per_pool_token_limit, total_token_limit,
			BCM3380_FPM_TOKEN_LIMIT_BITMAP_MAX_TOKENS);
		return -EINVAL;
	}

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
	fpm->token_stride = buffer_size;

	writel_be(BCM3380_FPM_CTRL_INIT_MEM,
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);
	ret = fpm_wait_init_done(fpm);
	if (ret) {
		dev_err(dev, "FPM InitMem did not complete\n");
		goto free_pool_mem;
	}

	if (fpm->variant->poolcfg1_has_buffer_size)
		writel_be(buffer_size_code << BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_SHIFT,
			  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG1);
	writel_be((u32)fpm->mem_dma, fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG2);
	writel_be(fpm_ctrl_cfg1_from_pools(fpm),
		  fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_CFG1);

	if (fpm->variant->token_limit_bitmap) {
		ret = fpm_write_token_limit(fpm, total_token_limit);
		if (ret)
			goto free_pool_mem;
	}

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

	return pool->fpm->phys + fpm_pool_alloc_reg(pool->fpm, pool->id);
}
EXPORT_SYMBOL_GPL(fpm_alloc_free_bus_addr);

u32 fpm_alloc_free_bus_addr_for_size(struct bcm3380_fpm_pool *pool,
				     size_t size)
{
	if (!fpm_pool_ready(pool))
		return 0;

	struct bcm3380_fpm *fpm = pool->fpm;
	if (!fpm->variant->multi_size_alloc_regs)
		return fpm->phys + fpm_pool_alloc_reg(fpm, pool->id);

	unsigned int id = fpm_bcm3383_alloc_reg_id_for_size(size);
	if (id >= BCM3380_FPM_NUM_POOLS)
		return 0;

	return fpm->phys + fpm_pool_alloc_reg(fpm, id);
}
EXPORT_SYMBOL_GPL(fpm_alloc_free_bus_addr_for_size);

u32 fpm_borrow_token(struct bcm3380_fpm_pool *pool)
{
	if (!fpm_pool_ready(pool))
		return 0;

	struct bcm3380_fpm *fpm = pool->fpm;
	unsigned int id = pool->id;

	if (fpm->variant->multi_size_alloc_regs) {
		/*
		 * On BCM3383 the alloc/dealloc registers select token size.
		 * Ethernet uses the 2048-byte source at the first register.
		 */
		id = 0;
	}

	return readl_be(fpm->base + fpm_pool_alloc_reg(fpm, id));
}
EXPORT_SYMBOL_GPL(fpm_borrow_token);

void fpm_return_token(struct bcm3380_fpm_pool *pool, u32 token)
{
	if (!fpm_pool_ready(pool))
		return;

	struct bcm3380_fpm *fpm = pool->fpm;
	unsigned int id = pool->id;

	if (fpm->variant->multi_size_alloc_regs) {
		/*
		 * BCM3383 bootloader always returns borrowed/RX tokens through
		 * FpmPool.Pool1AllocDealloc.  The other three alloc/dealloc
		 * registers are only used as size-specific token sources for
		 * MBDMA Tokenaddress256/512/1k/2k.
		 */
		id = 0;
	}

	writel_be(token, fpm->base + fpm_pool_alloc_reg(fpm, id));
}
EXPORT_SYMBOL_GPL(fpm_return_token);

void *fpm_token_to_virt(struct bcm3380_fpm_pool *pool, u32 token)
{
	if (!fpm_pool_ready(pool))
		return NULL;

	if (!fpm_token_valid(token))
		return NULL;

	struct bcm3380_fpm * fpm = pool->fpm;
	if (!fpm->token_stride)
		return NULL;

	size_t offset = ((token >> 12) & fpm->variant->token_index_mask) *
			fpm->token_stride;
	if (offset >= fpm->mem_size)
		return NULL;

	return fpm->mem + offset;
}
EXPORT_SYMBOL_GPL(fpm_token_to_virt);

u32 fpm_tokens_available(struct bcm3380_fpm_pool *pool)
{
	if (!fpm_pool_ready(pool))
		return 0;

	struct bcm3380_fpm *fpm = pool->fpm;
	u32 stat2 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLSTAT2);

	return stat2 & BCM3380_FPM_CTRL_POOLSTAT2_TOKEN_AVAIL_MASK;
}
EXPORT_SYMBOL_GPL(fpm_tokens_available);

static unsigned int fpm_enabled_pool_count(struct bcm3380_fpm *fpm)
{
	unsigned int count = 0;

	for (unsigned int i = 0; i < BCM3380_FPM_NUM_POOLS; i++) {
		if (fpm->pools[i].enabled)
			count++;
	}

	return count;
}

static u32 fpm_enabled_pool_id_mask(struct bcm3380_fpm *fpm)
{
	u32 mask = 0;

	for (unsigned int i = 0; i < BCM3380_FPM_NUM_POOLS; i++) {
		if (fpm->pools[i].enabled)
			mask |= BIT(i);
	}

	return mask;
}

static ssize_t status_show(struct device *dev,
			   struct device_attribute *attr, char *buf)
{
	struct bcm3380_fpm *fpm = dev_get_drvdata(dev);
	ssize_t len = 0;

	if (!fpm || !fpm->mem)
		return sysfs_emit(buf, "not ready\n");

	u32 ctrl = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL);
	u32 cfg1 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_CFG1);
	u32 pool_cfg1 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG1);
	u32 pool_cfg2 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLCFG2);
	u32 stat1 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLSTAT1);
	u32 stat2 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLSTAT2);
	u32 stat3 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLSTAT3);
	u32 stat4 = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_POOLSTAT4);
	u32 xon_xoff = readl_be(fpm->base + BCM3380_FPM_CTRLS + BCM3380_FPM_CTRL_XON_XOFF);
	u32 buffer_size_code = (u32)-1;
	if (fpm->variant->poolcfg1_has_buffer_size)
		buffer_size_code = (pool_cfg1 & BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_MASK) >>
				   BCM3380_FPM_CTRL_POOLCFG1_BUF_SIZE_SHIFT;
	size_t buffer_size = fpm->token_stride;
	unsigned int enabled_pool_count = fpm_enabled_pool_count(fpm);
	size_t pool_mem_size = enabled_pool_count ? fpm->mem_size / enabled_pool_count : 0;
	size_t total_token_limit = buffer_size ? fpm->mem_size / buffer_size : 0;
	size_t per_pool_token_limit = enabled_pool_count ? total_token_limit / enabled_pool_count : 0;

	len += sysfs_emit_at(buf, len, "Free Pool Manager Configuration\n");
	len += sysfs_emit_at(buf, len, "buffer_size_bytes: %zu\n", buffer_size);
	len += sysfs_emit_at(buf, len, "buffer_size_code: 0x%08x\n", buffer_size_code);
	len += sysfs_emit_at(buf, len, "token_limit_total: %zu\n", total_token_limit);
	len += sysfs_emit_at(buf, len, "token_limit_per_pool: %zu\n", per_pool_token_limit);
	len += sysfs_emit_at(buf, len, "enabled_pool_count: %u\n", enabled_pool_count);
	len += sysfs_emit_at(buf, len, "enabled_pool_mask: 0x%08x\n", fpm_enabled_pool_id_mask(fpm));
	len += sysfs_emit_at(buf, len, "fpm_register_phys_base: 0x%08x\n", (u32)fpm->phys);
	len += sysfs_emit_at(buf, len, "fpm_pool_dma_base: 0x%08x\n", pool_cfg2);
	len += sysfs_emit_at(buf, len, "total_configured_memory_size: %zu\n", fpm->mem_size);
	len += sysfs_emit_at(buf, len, "per_pool_memory_size: %zu\n", pool_mem_size);
	len += sysfs_emit_at(buf, len, "free_fifo_full: %u\n", !!(stat2 & BCM3380_FPM_CTRL_POOLSTAT2_FREE_FIFO_FULL));
	len += sysfs_emit_at(buf, len, "free_fifo_empty: %u\n", !!(stat2 & BCM3380_FPM_CTRL_POOLSTAT2_FREE_FIFO_EMPTY));
	len += sysfs_emit_at(buf, len, "alloc_fifo_full: %u\n", !!(stat2 & BCM3380_FPM_CTRL_POOLSTAT2_ALLOC_FIFO_FULL));
	len += sysfs_emit_at(buf, len, "alloc_fifo_empty: %u\n", !!(stat2 & BCM3380_FPM_CTRL_POOLSTAT2_ALLOC_FIFO_EMPTY));
	len += sysfs_emit_at(buf, len, "pool_full: %u\n", !!(stat2 & BCM3380_FPM_CTRL_POOLSTAT2_POOL_FULL));
	len += sysfs_emit_at(buf, len, "tokens_available: %u\n", (u32)(stat2 & BCM3380_FPM_CTRL_POOLSTAT2_TOKEN_AVAIL_MASK));
	len += sysfs_emit_at(buf, len, "not_valid_token_frees: %u\n", (u32)(stat3 & BCM3380_FPM_CTRL_POOLSTAT3_INVALID_FREE_MASK));
	len += sysfs_emit_at(buf, len, "not_valid_token_multi: %u\n", (u32)(stat4 & BCM3380_FPM_CTRL_POOLSTAT4_INVALID_MULTI_MASK));
	len += sysfs_emit_at(buf, len, "overflow_count: %u\n", stat1 >> BCM3380_FPM_CTRL_POOLSTAT1_OVRFL_SHIFT);
	len += sysfs_emit_at(buf, len, "underflow_count: %u\n", (u32)(stat1 & BCM3380_FPM_CTRL_POOLSTAT1_UNDRFL_MASK));
	len += sysfs_emit_at(buf, len, "xon_threshold: %u\n", xon_xoff >> BCM3380_FPM_CTRL_XON_SHIFT);
	len += sysfs_emit_at(buf, len, "xoff_threshold: %u\n", xon_xoff & BCM3380_FPM_CTRL_XON_XOFF_THRESHOLD_MAX);
	len += sysfs_emit_at(buf, len, "ctrl: 0x%08x\n", ctrl);
	len += sysfs_emit_at(buf, len, "cfg1: 0x%08x\n", cfg1);
	len += sysfs_emit_at(buf, len, "pool_cfg1: 0x%08x\n", pool_cfg1);
	len += sysfs_emit_at(buf, len, "pool_cfg2: 0x%08x\n", pool_cfg2);
	len += sysfs_emit_at(buf, len, "pool_stat1: 0x%08x\n", stat1);
	len += sysfs_emit_at(buf, len, "pool_stat2: 0x%08x\n", stat2);
	len += sysfs_emit_at(buf, len, "pool_stat3: 0x%08x\n", stat3);
	len += sysfs_emit_at(buf, len, "pool_stat4: 0x%08x\n", stat4);

	for (unsigned int i = 0; i < BCM3380_FPM_NUM_POOLS; i++) {
		struct bcm3380_fpm_pool *pool = &fpm->pools[i];

		if (!pool->configured)
			continue;

		len += sysfs_emit_at(buf, len,
				     "pool%u: status=%s cache_bypass=%u search_mode=%u alloc_free_bus_addr=0x%08x\n",
				     pool->id + 1, pool->enabled ? "okay" : "disabled",
				     pool->cache_bypass, pool->search_mode,
				     pool->enabled ? (u32)(fpm->phys + fpm_pool_alloc_reg(fpm, pool->id)) : 0);
	}

	return len;
}

static DEVICE_ATTR_RO(status);

static struct attribute *fpm_attrs[] = {
	&dev_attr_status.attr,
	NULL,
};

static const struct attribute_group fpm_attr_group = {
	.name = "fpm",
	.attrs = fpm_attrs,
};

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
	fpm->variant = of_device_get_match_data(dev);
	if (!fpm->variant)
		fpm->variant = &bcm3380_fpm_data;

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

	ret = sysfs_create_group(&dev->kobj, &fpm_attr_group);
	if (ret)
		goto disable_pools;

	return 0;

disable_pools:
	fpm_disable_pools(fpm);
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

	sysfs_remove_group(&pdev->dev.kobj, &fpm_attr_group);
	fpm_disable_pools(fpm);

	if (fpm->reset)
		reset_control_assert(fpm->reset);

	if (fpm->clk)
		clk_disable_unprepare(fpm->clk);
}

static const struct of_device_id bcm3380_fpm_of_match[] = {
	{ .compatible = "brcm,bcm3383-fpm", .data = &bcm3383_fpm_data },
	{ .compatible = "brcm,bcm3380-fpm", .data = &bcm3380_fpm_data },
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
