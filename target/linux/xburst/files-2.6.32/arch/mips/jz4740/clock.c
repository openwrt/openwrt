/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ4740 SoC TCU support
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

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/err.h>

#define JZ_REG_CLOCK_CTRL	0x00
#define JZ_REG_CLOCK_PLL	0x10
#define JZ_REG_CLOCK_GATE	0x20
#define JZ_REG_CLOCK_I2S	0x60
#define JZ_REG_CLOCK_LCD	0x64
#define JZ_REG_CLOCK_MMC	0x68
#define JZ_REG_CLOCK_UHC	0x6C
#define JZ_REG_CLOCK_SPI	0x74

#define JZ_CLOCK_CTRL_I2S_SRC_PLL	BIT(31)
#define JZ_CLOCK_CTRL_KO_ENABLE		BIT(30)
#define JZ_CLOCK_CTRL_UDC_SRC_PLL	BIT(29)
#define JZ_CLOCK_CTRL_UDIV_MASK		0x1f800000
#define JZ_CLOCK_CTRL_CHANGE_ENABLE	BIT(22)
#define JZ_CLOCK_CTRL_PLL_HALF		BIT(21)
#define JZ_CLOCK_CTRL_LDIV_MASK		0x001f0000
#define JZ_CLOCK_CTRL_UDIV_OFFSET	23
#define JZ_CLOCK_CTRL_LDIV_OFFSET	16
#define JZ_CLOCK_CTRL_MDIV_OFFSET	12
#define JZ_CLOCK_CTRL_PDIV_OFFSET	 8
#define JZ_CLOCK_CTRL_HDIV_OFFSET	 4
#define JZ_CLOCK_CTRL_CDIV_OFFSET	 0

#define JZ_CLOCK_GATE_UART0	BIT(0)
#define JZ_CLOCK_GATE_TCU	BIT(1)
#define JZ_CLOCK_GATE_RTC	BIT(2)
#define JZ_CLOCK_GATE_I2C	BIT(3)
#define JZ_CLOCK_GATE_SPI	BIT(4)
#define JZ_CLOCK_GATE_AIC_PCLK	BIT(5)
#define JZ_CLOCK_GATE_AIC	BIT(6)
#define JZ_CLOCK_GATE_MMC	BIT(7)
#define JZ_CLOCK_GATE_ADC	BIT(8)
#define JZ_CLOCK_GATE_CIM	BIT(9)
#define JZ_CLOCK_GATE_LCD	BIT(10)
#define JZ_CLOCK_GATE_UDC	BIT(11)
#define JZ_CLOCK_GATE_DMAC	BIT(12)
#define JZ_CLOCK_GATE_IPU	BIT(13)
#define JZ_CLOCK_GATE_UHC	BIT(14)
#define JZ_CLOCK_GATE_UART1	BIT(15)

#define JZ_CLOCK_I2S_DIV_MASK		0x01ff

#define JZ_CLOCK_LCD_DIV_MASK		0x01ff

#define JZ_CLOCK_MMC_DIV_MASK		0x001f

#define JZ_CLOCK_UHC_DIV_MASK		0x000f

#define JZ_CLOCK_SPI_SRC_PLL		BIT(31)
#define JZ_CLOCK_SPI_DIV_MASK		0x000f

#define JZ_CLOCK_PLL_M_MASK		0x01ff
#define JZ_CLOCK_PLL_N_MASK		0x001f
#define JZ_CLOCK_PLL_OD_MASK		0x0003
#define JZ_CLOCK_PLL_STABLE		BIT(10)
#define JZ_CLOCK_PLL_BYPASS		BIT(9)
#define JZ_CLOCK_PLL_ENABLED		BIT(8)
#define JZ_CLOCK_PLL_STABLIZE_MASK	0x000f
#define JZ_CLOCK_PLL_M_OFFSET		23
#define JZ_CLOCK_PLL_N_OFFSET		18
#define JZ_CLOCK_PLL_OD_OFFSET		16

static void __iomem *jz_clock_base;
static spinlock_t jz_clock_lock;
static LIST_HEAD(jz_clocks);

struct clk {
	const char *name;
	struct clk* parent;

	uint32_t gate_bit;

	unsigned long (*get_rate)(struct clk* clk);
	unsigned long (*round_rate)(struct clk *clk, unsigned long rate);
	int (*set_rate)(struct clk* clk, unsigned long rate);
	int (*enable)(struct clk* clk);
	int (*disable)(struct clk* clk);

	int (*set_parent)(struct clk* clk, struct clk *parent);
	struct list_head list;
};

struct main_clk {
	struct clk clk;
	uint32_t div_offset;
};

struct divided_clk {
	struct clk clk;
	uint32_t reg;
	uint32_t mask;
};

struct static_clk {
	struct clk clk;
	unsigned long rate;
};

static uint32_t jz_clk_reg_read(int reg)
{
	return readl(jz_clock_base + reg);
}

static void jz_clk_reg_write_mask(int reg, uint32_t val, uint32_t mask)
{
	uint32_t val2;

	spin_lock(&jz_clock_lock);
	val2 = readl(jz_clock_base + reg);
	val2 &= ~mask;
	val2 |= val;
	writel(val2, jz_clock_base + reg);
	spin_unlock(&jz_clock_lock);
}

static void jz_clk_reg_set_bits(int reg, uint32_t mask)
{
	uint32_t val;

	spin_lock(&jz_clock_lock);
	val = readl(jz_clock_base + reg);
	val |= mask;
	writel(val, jz_clock_base + reg);
	spin_unlock(&jz_clock_lock);
}

static void jz_clk_reg_clear_bits(int reg, uint32_t mask)
{
	uint32_t val;

	spin_lock(&jz_clock_lock);
	val = readl(jz_clock_base + reg);
	val &= ~mask;
	writel(val, jz_clock_base + reg);
	spin_unlock(&jz_clock_lock);
}

static int jz_clk_enable_gating(struct clk *clk)
{
	jz_clk_reg_clear_bits(JZ_REG_CLOCK_GATE, clk->gate_bit);
	return 0;
}

static int jz_clk_disable_gating(struct clk *clk)
{
	jz_clk_reg_set_bits(JZ_REG_CLOCK_GATE, clk->gate_bit);
	return 0;
}

static unsigned long jz_clk_static_get_rate(struct clk *clk)
{
	return ((struct static_clk*)clk)->rate;
}

static int jz_clk_ko_enable(struct clk* clk)
{
	jz_clk_reg_set_bits(JZ_REG_CLOCK_CTRL, JZ_CLOCK_CTRL_KO_ENABLE);
	return 0;
}

static int jz_clk_ko_disable(struct clk* clk)
{
	jz_clk_reg_clear_bits(JZ_REG_CLOCK_CTRL, JZ_CLOCK_CTRL_KO_ENABLE);
	return 0;
}


static const int pllno[] = {1, 2, 2, 4};

static unsigned long jz_clk_pll_get_rate(struct clk *clk)
{
	uint32_t val;
	int m;
	int n;
	int od;

	val = jz_clk_reg_read(JZ_REG_CLOCK_PLL);

	if (val & JZ_CLOCK_PLL_BYPASS)
		return clk_get_rate(clk->parent);

	m = ((val >> 23) & 0x1ff) + 2;
	n = ((val >> 18) & 0x1f) + 2;
	od = (val >> 16) & 0x3;

	return clk_get_rate(clk->parent) * (m / n) / pllno[od];
}

static unsigned long jz_clk_pll_half_get_rate(struct clk *clk)
{
	uint32_t reg;

	reg = jz_clk_reg_read(JZ_REG_CLOCK_CTRL);
	if (reg & JZ_CLOCK_CTRL_PLL_HALF)
		return jz_clk_pll_get_rate(clk->parent);
	return jz_clk_pll_get_rate(clk->parent) >> 1;
}



static const int jz_clk_main_divs[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

static unsigned long jz_clk_main_round_rate(struct clk *clk, unsigned long rate)
{
	unsigned long parent_rate = jz_clk_pll_get_rate(clk->parent);
	int div;

	div = parent_rate / rate;
	if (div > 32)
		return parent_rate / 32;
	else if (div < 1)
		return parent_rate;

	div &= (0x3 << (ffs(div) - 1));

	return parent_rate / div;
}

static unsigned long jz_clk_main_get_rate(struct clk *clk) {
	struct main_clk *mclk = (struct main_clk*)clk;
	uint32_t div;

	div = jz_clk_reg_read(JZ_REG_CLOCK_CTRL);

	div >>= mclk->div_offset;
	div &= 0xf;

	if (div >= ARRAY_SIZE(jz_clk_main_divs))
		div = ARRAY_SIZE(jz_clk_main_divs) - 1;

	return jz_clk_pll_get_rate(clk->parent) / jz_clk_main_divs[div];
}

static int jz_clk_main_set_rate(struct clk *clk, unsigned long rate)
{
	struct main_clk *mclk = (struct main_clk*)clk;
	int i;
	int div;
	unsigned long parent_rate = jz_clk_pll_get_rate(clk->parent);

	rate = jz_clk_main_round_rate(clk, rate);

	div = parent_rate / rate;

	i = (ffs(div) - 1) << 1;
	if (i > 0 && !(div & BIT(i-1)))
		i -= 1;

	jz_clk_reg_write_mask(JZ_REG_CLOCK_CTRL, i << mclk->div_offset,
				0xf << mclk->div_offset);

	return 0;
}


static struct static_clk jz_clk_ext = {
	.clk = {
		.name = "ext",
		.get_rate = jz_clk_static_get_rate,
	},
};

static struct clk jz_clk_pll = {
	.name = "pll",
	.parent = &jz_clk_ext.clk,
	.get_rate = jz_clk_pll_get_rate,
};

static struct clk jz_clk_pll_half = {
	.name = "pll half",
	.parent = &jz_clk_pll,
	.get_rate = jz_clk_pll_half_get_rate,
};

static struct main_clk jz_clk_cpu = {
	.clk = {
		.name = "cclk",
		.parent = &jz_clk_pll,
		.get_rate = jz_clk_main_get_rate,
		.set_rate = jz_clk_main_set_rate,
		.round_rate = jz_clk_main_round_rate,
	},
	.div_offset = JZ_CLOCK_CTRL_CDIV_OFFSET,
};

static struct main_clk jz_clk_memory = {
	.clk = {
		.name = "mclk",
		.parent = &jz_clk_pll,
		.get_rate = jz_clk_main_get_rate,
		.set_rate = jz_clk_main_set_rate,
		.round_rate = jz_clk_main_round_rate,
	},
	.div_offset = JZ_CLOCK_CTRL_MDIV_OFFSET,
};

static struct main_clk jz_clk_high_speed_peripheral = {
	.clk = {
		.name = "hclk",
		.parent = &jz_clk_pll,
		.get_rate = jz_clk_main_get_rate,
		.set_rate = jz_clk_main_set_rate,
		.round_rate = jz_clk_main_round_rate,
	},
	.div_offset = JZ_CLOCK_CTRL_HDIV_OFFSET,
};


static struct main_clk jz_clk_low_speed_peripheral = {
	.clk = {
		.name = "pclk",
		.parent = &jz_clk_pll,
		.get_rate = jz_clk_main_get_rate,
		.set_rate = jz_clk_main_set_rate,
	},
	.div_offset = JZ_CLOCK_CTRL_PDIV_OFFSET,
};

static struct clk jz_clk_ko = {
	.name = "cko",
	.parent = &jz_clk_memory.clk,
	.enable = jz_clk_ko_enable,
	.disable = jz_clk_ko_disable,
};

static int jz_clk_spi_set_parent(struct clk *clk, struct clk *parent)
{
	if (parent == &jz_clk_pll)
		jz_clk_reg_set_bits(JZ_CLOCK_SPI_SRC_PLL, JZ_REG_CLOCK_SPI);
	else if(parent == &jz_clk_ext.clk)
		jz_clk_reg_clear_bits(JZ_CLOCK_SPI_SRC_PLL, JZ_REG_CLOCK_SPI);
	else
		return -EINVAL;

	clk->parent = parent;

	return 0;
}

static int jz_clk_i2s_set_parent(struct clk *clk, struct clk *parent)
{
	if (parent == &jz_clk_pll_half)
		jz_clk_reg_set_bits(JZ_REG_CLOCK_CTRL, JZ_CLOCK_CTRL_I2S_SRC_PLL);
	else if(parent == &jz_clk_ext.clk)
		jz_clk_reg_clear_bits(JZ_REG_CLOCK_CTRL, JZ_CLOCK_CTRL_I2S_SRC_PLL);
	else
		return -EINVAL;

	clk->parent = parent;

	return 0;
}

static int jz_clk_udc_set_parent(struct clk *clk, struct clk *parent)
{
	if (parent == &jz_clk_pll_half)
		jz_clk_reg_set_bits(JZ_REG_CLOCK_CTRL, JZ_CLOCK_CTRL_UDC_SRC_PLL);
	else if(parent == &jz_clk_ext.clk)
		jz_clk_reg_clear_bits(JZ_REG_CLOCK_CTRL, JZ_CLOCK_CTRL_UDC_SRC_PLL);
	else
		return -EINVAL;

	clk->parent = parent;

	return 0;
}

static int jz_clk_udc_set_rate(struct clk *clk, unsigned long rate)
{
	int div;

	if (clk->parent == &jz_clk_ext.clk)
		return -EINVAL;

	div = clk_get_rate(clk->parent) / rate - 1;

	if (div < 0)
		div = 0;
	else if (div > 63)
		div = 63;

	jz_clk_reg_write_mask(JZ_REG_CLOCK_CTRL, div << JZ_CLOCK_CTRL_UDIV_OFFSET,
				JZ_CLOCK_CTRL_UDIV_MASK);
	return 0;
}

static unsigned long jz_clk_udc_get_rate(struct clk *clk)
{
	int div;

	if (clk->parent == &jz_clk_ext.clk)
		return clk_get_rate(clk->parent);

	div = (jz_clk_reg_read(JZ_REG_CLOCK_CTRL) & JZ_CLOCK_CTRL_UDIV_MASK);
	div >>= JZ_CLOCK_CTRL_UDIV_OFFSET;
	div += 1;

	return clk_get_rate(clk->parent) / div;
}

static unsigned long jz_clk_divided_get_rate(struct clk *clk)
{
	struct divided_clk *dclk = (struct divided_clk*)clk;
	int div;

	if (clk->parent == &jz_clk_ext.clk)
		return clk_get_rate(clk->parent);

	div = (jz_clk_reg_read(dclk->reg) & dclk->mask) + 1;

	return clk_get_rate(clk->parent) / div;
}

static int jz_clk_divided_set_rate(struct clk *clk, unsigned long rate)
{
	struct divided_clk *dclk = (struct divided_clk*)clk;
	int div;

	if (clk->parent == &jz_clk_ext.clk)
		return -EINVAL;

	div = clk_get_rate(clk->parent) / rate - 1;

	if (div < 0)
		div = 0;
	else if(div > dclk->mask)
		div = dclk->mask;

	jz_clk_reg_write_mask(dclk->reg, div, dclk->mask);

	return 0;
}

static unsigned long jz_clk_ldclk_round_rate(struct clk *clk, unsigned long rate)
{
	int div;
	unsigned long parent_rate = jz_clk_pll_half_get_rate(clk->parent);

	if (rate > 150000000)
		return 150000000;

	div = parent_rate / rate;
	if (div < 1)
		div = 1;
	else if(div > 32)
		div = 32;

	return parent_rate / div;
}

static int jz_clk_ldclk_set_rate(struct clk *clk, unsigned long rate)
{
	int div;

	if (rate > 150000000)
		return -EINVAL;

	div = jz_clk_pll_half_get_rate(clk->parent) / rate - 1;
	if (div < 0)
		div = 0;
	else if(div > 31)
		div = 31;

	jz_clk_reg_write_mask(JZ_REG_CLOCK_CTRL, div << JZ_CLOCK_CTRL_LDIV_OFFSET,
				JZ_CLOCK_CTRL_LDIV_MASK);

	return 0;
}

static unsigned long jz_clk_ldclk_get_rate(struct clk *clk)
{
	int div;

	div = jz_clk_reg_read(JZ_REG_CLOCK_CTRL) & JZ_CLOCK_CTRL_LDIV_MASK;
	div >>= JZ_CLOCK_CTRL_LDIV_OFFSET;

	return jz_clk_pll_half_get_rate(clk->parent) / (div + 1);
}

static struct clk jz_clk_ld = {
	.name = "lcd",
	.parent = &jz_clk_pll_half,
	.set_rate = jz_clk_ldclk_set_rate,
	.get_rate = jz_clk_ldclk_get_rate,
	.round_rate = jz_clk_ldclk_round_rate,
};

static struct divided_clk jz_clk_lp = {
	.clk = {
		.name = "lcd_pclk",
		.parent = &jz_clk_pll_half,
	},
	.reg = JZ_REG_CLOCK_LCD,
	.mask = JZ_CLOCK_LCD_DIV_MASK,
};

static struct clk jz_clk_cim_mclk = {
	.name = "cim_mclk",
	.parent = &jz_clk_high_speed_peripheral.clk,
};

static struct static_clk jz_clk_cim_pclk = {
	.clk = {
		.name = "cim_pclk",
		.gate_bit = JZ_CLOCK_GATE_CIM,
		.get_rate = jz_clk_static_get_rate,
		.enable = jz_clk_enable_gating,
		.disable = jz_clk_disable_gating,
	},
};

static struct divided_clk jz_clk_i2s = {
	.clk = {
		.name = "i2s",
		.parent = &jz_clk_ext.clk,
		.gate_bit = JZ_CLOCK_GATE_AIC,
		.set_rate = jz_clk_divided_set_rate,
		.get_rate = jz_clk_divided_get_rate,
		.enable = jz_clk_enable_gating,
		.disable = jz_clk_disable_gating,
		.set_parent = jz_clk_i2s_set_parent,
	},
	.reg = JZ_REG_CLOCK_I2S,
	.mask = JZ_CLOCK_I2S_DIV_MASK,
};

static struct  divided_clk jz_clk_mmc = {
	.clk = {
		.name = "mmc",
		.parent = &jz_clk_pll_half,
		.gate_bit = JZ_CLOCK_GATE_MMC,
		.set_rate = jz_clk_divided_set_rate,
		.get_rate = jz_clk_divided_get_rate,
		.enable = jz_clk_enable_gating,
		.disable = jz_clk_disable_gating,
	},
	.reg = JZ_REG_CLOCK_MMC,
	.mask = JZ_CLOCK_MMC_DIV_MASK,
};

static struct divided_clk jz_clk_uhc = {
	.clk = {
		.name = "uhc",
		.parent = &jz_clk_pll_half,
		.gate_bit = JZ_CLOCK_GATE_UHC,
		.set_rate = jz_clk_divided_set_rate,
		.get_rate = jz_clk_divided_get_rate,
		.enable = jz_clk_enable_gating,
		.disable = jz_clk_disable_gating,
	},
	.reg = JZ_REG_CLOCK_UHC,
	.mask = JZ_CLOCK_UHC_DIV_MASK,
};

static struct clk jz_clk_udc = {
	.name = "udc",
	.parent = &jz_clk_ext.clk,
	.set_parent = jz_clk_udc_set_parent,
	.set_rate = jz_clk_udc_set_rate,
	.get_rate = jz_clk_udc_get_rate,
};

static struct divided_clk jz_clk_spi = {
	.clk = {
		.name = "spi",
		.parent = &jz_clk_ext.clk,
		.gate_bit = JZ_CLOCK_GATE_SPI,
		.set_rate = jz_clk_divided_set_rate,
		.get_rate = jz_clk_divided_get_rate,
		.enable = jz_clk_enable_gating,
		.disable = jz_clk_disable_gating,
		.set_parent = jz_clk_spi_set_parent,
	},
	.reg = JZ_REG_CLOCK_SPI,
	.mask = JZ_CLOCK_SPI_DIV_MASK,
};

static struct clk jz_clk_uart0 = {
	.name = "uart0",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE_UART0,
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
};

static struct clk jz_clk_uart1 = {
	.name = "uart1",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE_UART1,
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
};

static struct clk jz_clk_dma = {
	.name = "dma",
	.parent = &jz_clk_high_speed_peripheral.clk,
	.gate_bit = JZ_CLOCK_GATE_UART0,
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
};

static struct clk jz_clk_ipu = {
	.name = "ipu",
	.parent = &jz_clk_high_speed_peripheral.clk,
	.gate_bit = JZ_CLOCK_GATE_IPU,
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
};

static struct clk jz_clk_adc = {
	.name = "adc",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE_ADC,
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
};

static struct clk jz_clk_i2c = {
	.name = "i2c",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE_I2C,
	.enable = jz_clk_enable_gating,
	.disable = jz_clk_disable_gating,
};

static struct static_clk jz_clk_rtc = {
	.clk = {
		.name = "rtc",
		.gate_bit = JZ_CLOCK_GATE_RTC,
		.enable = jz_clk_enable_gating,
		.disable = jz_clk_disable_gating,
	},
	.rate = 32768,
};

int clk_enable(struct clk *clk)
{
	if (!clk->enable)
		return -EINVAL;

	return clk->enable(clk);
}
EXPORT_SYMBOL_GPL(clk_enable);

void clk_disable(struct clk *clk)
{
	if (clk->disable)
		clk->disable(clk);
}
EXPORT_SYMBOL_GPL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
	if (clk->get_rate)
		return clk->get_rate(clk);
	if (clk->parent)
		return clk_get_rate(clk->parent);

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(clk_get_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	if (!clk->set_rate)
		return -EINVAL;
	return clk->set_rate(clk, rate);
}
EXPORT_SYMBOL_GPL(clk_set_rate);

long clk_round_rate(struct clk *clk, unsigned long rate)
{
	if (clk->round_rate)
		return clk->round_rate(clk, rate);

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(clk_round_rate);

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	int ret;

	if (!clk->set_parent)
		return -EINVAL;

	clk->disable(clk);
	ret = clk->set_parent(clk, parent);
	clk->enable(clk);

	return ret;
}
EXPORT_SYMBOL_GPL(clk_set_parent);


struct clk *clk_get(struct device *dev, const char *name)
{
	struct clk *clk;

	list_for_each_entry(clk, &jz_clocks, list) {
	    if (strcmp(clk->name, name) == 0)
			return clk;
	}
	return ERR_PTR(-ENOENT);
}
EXPORT_SYMBOL_GPL(clk_get);

void clk_put(struct clk *clk)
{
}
EXPORT_SYMBOL_GPL(clk_put);

inline static void clk_add(struct clk *clk)
{
    list_add_tail(&clk->list, &jz_clocks);
}

static void clk_register_clks(void)
{
    clk_add(&jz_clk_ext.clk);
    clk_add(&jz_clk_pll);
    clk_add(&jz_clk_pll_half);
    clk_add(&jz_clk_cpu.clk);
    clk_add(&jz_clk_high_speed_peripheral.clk);
    clk_add(&jz_clk_low_speed_peripheral.clk);
    clk_add(&jz_clk_ko);
    clk_add(&jz_clk_ld);
    clk_add(&jz_clk_lp.clk);
    clk_add(&jz_clk_cim_mclk);
    clk_add(&jz_clk_cim_pclk.clk);
    clk_add(&jz_clk_i2s.clk);
    clk_add(&jz_clk_mmc.clk);
    clk_add(&jz_clk_uhc.clk);
    clk_add(&jz_clk_udc);
    clk_add(&jz_clk_uart0);
    clk_add(&jz_clk_uart1);
    clk_add(&jz_clk_dma);
    clk_add(&jz_clk_ipu);
    clk_add(&jz_clk_adc);
    clk_add(&jz_clk_i2c);
    clk_add(&jz_clk_rtc.clk);
}

int jz_init_clocks(unsigned long ext_rate)
{
	uint32_t val;

	jz_clock_base = ioremap(0x10000000, 0x100);
	if (!jz_clock_base)
		return -EBUSY;

	jz_clk_ext.rate = ext_rate;

	val = jz_clk_reg_read(JZ_REG_CLOCK_SPI);

	if (val & JZ_CLOCK_SPI_SRC_PLL)
		jz_clk_spi.clk.parent = &jz_clk_pll_half;

	val = jz_clk_reg_read(JZ_REG_CLOCK_CTRL);

	if (val & JZ_CLOCK_CTRL_I2S_SRC_PLL)
		jz_clk_i2s.clk.parent = &jz_clk_pll_half;

	if (val & JZ_CLOCK_CTRL_UDC_SRC_PLL)
		jz_clk_udc.parent = &jz_clk_pll_half;

	clk_register_clks();

	return 0;
}
EXPORT_SYMBOL_GPL(jz_init_clocks);
