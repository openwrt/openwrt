// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/math64.h>
#include <linux/notifier.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/reboot.h>
#include <linux/reset-controller.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include <dt-bindings/clock/dspg,dvf99-cmu.h>

#define CMU_PLL1CONTROL		0x00
#define CMU_PLL2CONTROL		0x04
#define CMU_PLL3CONTROL		0x08
#define CMU_CPUCLKCNTRL		0x14
#define CMU_COMCLKSEL		0x18
#define CMU_CLKSWCNTRL		0x1c
#define CMU_CLKDIVCNTRL1	0x20
#define CMU_CLKDIVCNTRL3	0x28
#define CMU_CLKDIVCNTRL4	0x2c
#define CMU_CLKDIVCNTRL5	0x30
#define CMU_SWSYSRST		0x40
#define CMU_RSTSR		0x48
#define CMU_SWCHWRST1		0x50
#define CMU_SWCHWRST2		0x54
#define CMU_SWCLKENR1		0x58
#define CMU_SWCLKENR2		0x5c

#define CMU_RSTSR_MAINCLK_SHIFT	1
#define CMU_RSTSR_OSC_25M	BIT(2)
#define CMU_RSTSR_RST_FLAGS	GENMASK(5, 3)

#define CMU_PLL_PREDIV		GENMASK(4, 0)
#define CMU_PLL_MULT		GENMASK(11, 5)
#define CMU_PLL_POSTDIV		GENMASK(13, 12)
#define CMU_PLL_REF_SHIFT	19

#define CMU_CPUCLK_HOSTCPU_SHIFT	0
#define CMU_CPUCLK_AHB_SHIFT		4
#define CMU_CPUCLK_ALTSYS_SHIFT		14
#define CMU_CLKSW_SYSCLK_SHIFT		1
#define CMU_CLKSW_SLOWCLK_SHIFT		4
#define CMU_CLKSW_SLOWCLK_WIDTH		11
#define CMU_CLKSW_SLOWCLK_EN		3
#define CMU_CLKSW_LCDC_SHIFT		17
#define CMU_CLKSW_QSPI_SHIFT		26
#define CMU_CLKSW_SDMMC_SHIFT		27
#define CMU_CLKDIV1_LCDC_SHIFT		8
#define CMU_CLKDIV1_LCDC_WIDTH		8
#define CMU_CLKDIV1_SDMMC_SHIFT		24
#define CMU_CLKDIV1_SDMMC_WIDTH		8
#define CMU_CLKDIV3_QSPI_SHIFT		8
#define CMU_CLKDIV3_QSPI_WIDTH		8
#define CMU_COMCLK_CSS_DIV_SHIFT	0
#define CMU_COMCLK_CSS_DIV_WIDTH	4
#define CMU_COMCLK_CSS_MUX_SHIFT	10
#define CMU_COMCLK_CSS_MUX_WIDTH	2
#define CMU_COMCLK_ADPCM_SHIFT		4
#define CMU_COMCLK_ADPCM_WIDTH		5
#define CMU_COMCLK_BMP_DIV_SHIFT	9
#define CMU_COMCLK_BMP_MUX_SHIFT	14
#define CMU_CPUCLK_AUDSRC_SHIFT		16
#define CMU_CLKSW_AUDCLK_SHIFT		2
#define CMU_CLKSW_APU_SHIFT		16
#define CMU_CLKSW_TDM1_SHIFT		19
#define CMU_CLKSW_TDM2_SHIFT		21
#define CMU_CLKSW_TDM_WIDTH		2
#define CMU_CLKDIV3_DCLASS_SHIFT	16
#define CMU_CLKDIV3_DCLASS_WIDTH	8
#define CMU_CLKDIV4_TDM1_SHIFT		0
#define CMU_CLKDIV4_TDM2_SHIFT		12
#define CMU_CLKDIV4_TDM_WIDTH		12
#define CMU_CLKDIV5_APU_SHIFT		24
#define CMU_CLKDIV5_APU_WIDTH		8
#define CMU_AUDCLK_DIV16		16
#define CMU_GATE_RETRIES		16

#define CMU_SWSYSRST_KEY1	0x7496398b
#define CMU_SWSYSRST_KEY2	0xb5c9c674

#define CMU_RST1_RELEASE	0x0f7bfffd
#define CMU_RST2_RELEASE	0x003f0ff7
#define CMU_RST_NUM		64

#define DVF99_OSC_DECT_RATE	13824000
#define DVF99_OSC_12M_RATE	12000000
#define DVF99_OSC_MAIN_25M	25000000
#define DVF99_OSC_MAIN_50M	50000000

struct dvf99_pll {
	struct clk_hw hw;
	void __iomem *reg;
};

struct dvf99_gate_hw {
	struct clk_hw hw;
	void __iomem *reg;
	u32 mask;
};

struct dvf99_gate {
	unsigned int id;
	const char *name;
	const char *parent;
	unsigned int bit;
};

static const char * const dvf99_mainclk_parents[] = {
	"osc_main", "osc_dect",
};

static const char * const dvf99_root_parents[] = {
	"mainclk", "osc_dect", "osc_12m", "mainclk",
};

static const char * const dvf99_sysclk_parents[] = {
	"altsysclk", "pll1",
};

static const char * const dvf99_fast_parents[] = {
	"sysclk", "pll2",
};

static const char * const dvf99_css_parents[] = {
	"sysclk", "pll2", "audclk", "mainclk",
};

static const char * const dvf99_audclk_parents[] = {
	"audsrcclk", "pll3",
};

static const char * const dvf99_apu_parents[] = {
	"audclk_div16", "audclk",
};

static const char * const dvf99_bmp_parents[] = {
	"osc_dect", "apu",
};

static const char * const dvf99_tdm_parents[] = {
	"sysclk", "audclk", "pll2", "sysclk",
};

static const struct clk_div_table dvf99_ahb_div_table[] = {
	{ .val = 0, .div = 3 },
	{ .val = 1, .div = 2 },
	{ .val = 2, .div = 3 },
	{ .val = 3, .div = 4 },
	{ }
};

static const struct clk_div_table dvf99_bmp_div_table[] = {
	{ .val = 0, .div = 1 },
	{ .val = 1, .div = 12 },
	{ }
};

static const struct dvf99_gate dvf99_gates[] = {
	{ DVF99_CLK_GDMAC,	"gdmac",	"ahb",		0 },
	{ DVF99_CLK_RTC,	"rtc",		"slowclk",	2 },
	{ DVF99_CLK_ETHMAC,	"ethmac",	"ahb",		3 },
	{ DVF99_CLK_UART1,	"uart1",	"ahb",		6 },
	{ DVF99_CLK_UART2,	"uart2",	"ahb",		7 },
	{ DVF99_CLK_CSS_DRT,	"css_drt",	"bmp",		8 },
	{ DVF99_CLK_TDM1,	"tdm1",		"tdm1_div",	9 },
	{ DVF99_CLK_TDM2,	"tdm2",		"tdm2_div",	10 },
	{ DVF99_CLK_QSPI,	"qspi",		"qspi_div",	11 },
	{ DVF99_CLK_SDMMC,	"sdmmc",	"sdmmc_div",	13 },
	{ DVF99_CLK_NFC,	"nfc",		"ahb",		14 },
	{ DVF99_CLK_LCDC,	"lcdc",		"lcdc_div",	21 },
	{ DVF99_CLK_USB20_MAC,	"usb20_mac",	"usb20_phy",	16 },
	{ DVF99_CLK_USB20_PHY,	"usb20_phy",	"osc_12m",	17 },
	{ DVF99_CLK_CSS_ARM,	"css_arm",	"css",		1 },
	{ DVF99_CLK_DCLASS,	"dclass",	"dclass_div",	18 },
	{ DVF99_CLK_PMU,	"pmu",		"ahb",		20 },
	{ DVF99_CLK_SEC,	"sec",		"ahb",		22 },
	{ DVF99_CLK_CSS,	"css",		"css_div",	23 },
	{ DVF99_CLK_RFAPU,	"rfapu",	"ahb",		25 },
	{ DVF99_CLK_APU,	"apu",		"apu_div",	26 },
	{ DVF99_CLK_CSS_ADPCM,	"css_adpcm",	"css_adpcm_div", 29 },
	{ DVF99_CLK_BMP,	"bmp",		"bmp_div",	30 },
	{ DVF99_CLK_CSS_ETM,	"css_etm",	"css",		31 },
	{ DVF99_CLK_TIMER1,	"timer1",	"ahb",		32 },
	{ DVF99_CLK_TIMER2,	"timer2",	"ahb",		33 },
	{ DVF99_CLK_TIMER3,	"timer3",	"ahb",		34 },
	{ DVF99_CLK_TIMER4,	"timer4",	"ahb",		35 },
	{ DVF99_CLK_I2C1,	"i2c1",		"ahb",		36 },
	{ DVF99_CLK_I2C2,	"i2c2",		"ahb",		37 },
	{ DVF99_CLK_SPI1,	"spi1",		"ahb",		38 },
	{ DVF99_CLK_CSS_TIMER1,	"css_timer1",	"css_ahb",	45 },
	{ DVF99_CLK_CSS_TIMER2,	"css_timer2",	"css_ahb",	46 },
	{ DVF99_CLK_CSS_GDMAC,	"css_gdmac",	"css_ahb",	47 },
};

static DEFINE_SPINLOCK(dvf99_cmu_lock);

static struct dvf99_gate_hw *to_dvf99_gate(struct clk_hw *hw)
{
	return container_of(hw, struct dvf99_gate_hw, hw);
}

static int dvf99_gate_set(struct clk_hw *hw, bool on)
{
	struct dvf99_gate_hw *gate = to_dvf99_gate(hw);
	unsigned int retry = CMU_GATE_RETRIES;
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&dvf99_cmu_lock, flags);
	do {
		val = readl(gate->reg);
		if (on)
			val |= gate->mask;
		else
			val &= ~gate->mask;
		writel(val, gate->reg);
	} while (readl(gate->reg) != val && --retry);
	spin_unlock_irqrestore(&dvf99_cmu_lock, flags);

	return retry ? 0 : -EIO;
}

static int dvf99_gate_enable(struct clk_hw *hw)
{
	return dvf99_gate_set(hw, true);
}

static void dvf99_gate_disable(struct clk_hw *hw)
{
	dvf99_gate_set(hw, false);
}

static int dvf99_gate_is_enabled(struct clk_hw *hw)
{
	struct dvf99_gate_hw *gate = to_dvf99_gate(hw);

	return !!(readl(gate->reg) & gate->mask);
}

static const struct clk_ops dvf99_gate_ops = {
	.enable = dvf99_gate_enable,
	.disable = dvf99_gate_disable,
	.is_enabled = dvf99_gate_is_enabled,
};

static struct clk_hw *dvf99_gate_register(const struct dvf99_gate *desc,
					  void __iomem *base)
{
	struct clk_init_data init = { };
	struct dvf99_gate_hw *gate;
	unsigned int bit = desc->bit;
	int ret;

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		return ERR_PTR(-ENOMEM);

	gate->reg = base + (bit < 32 ? CMU_SWCLKENR1 : CMU_SWCLKENR2);
	gate->mask = BIT(bit % 32);

	init.name = desc->name;
	init.ops = &dvf99_gate_ops;
	init.parent_names = desc->parent ? &desc->parent : NULL;
	init.num_parents = desc->parent ? 1 : 0;
	init.flags = CLK_IGNORE_UNUSED | CLK_SET_RATE_PARENT;
	gate->hw.init = &init;

	ret = clk_hw_register(NULL, &gate->hw);
	if (ret) {
		kfree(gate);
		return ERR_PTR(ret);
	}

	return &gate->hw;
}

struct dvf99_div_hw {
	struct clk_hw hw;
	void __iomem *reg;
	u8 shift;
	u8 width;
};

static struct dvf99_div_hw *to_dvf99_div(struct clk_hw *hw)
{
	return container_of(hw, struct dvf99_div_hw, hw);
}

static unsigned long dvf99_div_recalc_rate(struct clk_hw *hw,
					   unsigned long parent_rate)
{
	struct dvf99_div_hw *div = to_dvf99_div(hw);
	u32 val = (readl(div->reg) >> div->shift) & GENMASK(div->width - 1, 0);

	return parent_rate / (val + 1);
}

static int dvf99_div_determine_rate(struct clk_hw *hw,
				    struct clk_rate_request *req)
{
	struct dvf99_div_hw *div = to_dvf99_div(hw);
	unsigned long in = req->best_parent_rate;
	unsigned long d;

	if (!req->rate)
		return -EINVAL;

	d = (in + req->rate / 2) / req->rate;
	d = clamp(d, 1UL, BIT(div->width));
	req->rate = in / d;

	return 0;
}

static int dvf99_div_set_rate(struct clk_hw *hw, unsigned long rate,
			      unsigned long parent_rate)
{
	struct dvf99_div_hw *div = to_dvf99_div(hw);
	u32 mask = GENMASK(div->width - 1, 0) << div->shift;
	unsigned long flags;
	unsigned long d;
	u32 val;

	if (!rate)
		return -EINVAL;

	d = parent_rate / rate;
	if (!d || d > BIT(div->width) || parent_rate / d != rate)
		return -EINVAL;

	spin_lock_irqsave(&dvf99_cmu_lock, flags);
	val = readl(div->reg) & ~mask;
	writel(val | ((d - 1) << div->shift), div->reg);
	spin_unlock_irqrestore(&dvf99_cmu_lock, flags);

	return 0;
}

static const struct clk_ops dvf99_div_ops = {
	.recalc_rate = dvf99_div_recalc_rate,
	.determine_rate = dvf99_div_determine_rate,
	.set_rate = dvf99_div_set_rate,
};

static struct clk_hw *dvf99_div_register(const char *name, const char *parent,
					 void __iomem *reg, u8 shift, u8 width)
{
	struct clk_init_data init = { };
	struct dvf99_div_hw *div;
	int ret;

	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		return ERR_PTR(-ENOMEM);

	div->reg = reg;
	div->shift = shift;
	div->width = width;

	init.name = name;
	init.ops = &dvf99_div_ops;
	init.parent_names = &parent;
	init.num_parents = 1;
	div->hw.init = &init;

	ret = clk_hw_register(NULL, &div->hw);
	if (ret) {
		kfree(div);
		return ERR_PTR(ret);
	}

	return &div->hw;
}

struct dvf99_rst {
	struct reset_controller_dev rcdev;
	void __iomem *base;
};

static void __iomem *dvf99_rst_reg(struct dvf99_rst *rst, unsigned long id)
{
	return rst->base + (id < 32 ? CMU_SWCHWRST1 : CMU_SWCHWRST2);
}

static int dvf99_rst_update(struct reset_controller_dev *rcdev,
			    unsigned long id, bool assert)
{
	struct dvf99_rst *rst = container_of(rcdev, struct dvf99_rst, rcdev);
	void __iomem *reg = dvf99_rst_reg(rst, id);
	u32 mask = BIT(id % 32);
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&dvf99_cmu_lock, flags);
	do {
		val = readl(reg);
		if (assert)
			val |= mask;
		else
			val &= ~mask;
		writel(val, reg);
	} while (readl(reg) != val);
	spin_unlock_irqrestore(&dvf99_cmu_lock, flags);

	return 0;
}

static int dvf99_rst_assert(struct reset_controller_dev *rcdev,
			    unsigned long id)
{
	return dvf99_rst_update(rcdev, id, true);
}

static int dvf99_rst_deassert(struct reset_controller_dev *rcdev,
			      unsigned long id)
{
	return dvf99_rst_update(rcdev, id, false);
}

static int dvf99_rst_status(struct reset_controller_dev *rcdev,
			    unsigned long id)
{
	struct dvf99_rst *rst = container_of(rcdev, struct dvf99_rst, rcdev);

	return !!(readl(dvf99_rst_reg(rst, id)) & BIT(id % 32));
}

static const struct reset_control_ops dvf99_rst_ops = {
	.assert = dvf99_rst_assert,
	.deassert = dvf99_rst_deassert,
	.status = dvf99_rst_status,
};

static void __init dvf99_rst_register(struct device_node *np,
				      void __iomem *base)
{
	struct dvf99_rst *rst;
	int ret;

	rst = kzalloc(sizeof(*rst), GFP_KERNEL);
	if (!rst)
		return;

	rst->base = base;
	rst->rcdev.ops = &dvf99_rst_ops;
	rst->rcdev.of_node = np;
	rst->rcdev.nr_resets = CMU_RST_NUM;

	ret = reset_controller_register(&rst->rcdev);
	if (ret) {
		pr_err("%pOF: failed to register reset controller: %d\n",
		       np, ret);
		kfree(rst);
	}
}

static unsigned long dvf99_pll_recalc_rate(struct clk_hw *hw,
					   unsigned long parent_rate)
{
	struct dvf99_pll *pll = container_of(hw, struct dvf99_pll, hw);
	u32 val = readl(pll->reg);
	u64 rate;

	rate = (u64)parent_rate * (FIELD_GET(CMU_PLL_MULT, val) + 1);
	rate = div_u64(rate, FIELD_GET(CMU_PLL_PREDIV, val) + 1);

	return rate >> FIELD_GET(CMU_PLL_POSTDIV, val);
}

static const struct clk_ops dvf99_pll_ops = {
	.recalc_rate = dvf99_pll_recalc_rate,
};

static struct clk_hw *dvf99_pll_register(const char *name, const char *parent,
					 void __iomem *reg)
{
	struct clk_init_data init = { };
	struct dvf99_pll *pll;
	int ret;

	pll = kzalloc(sizeof(*pll), GFP_KERNEL);
	if (!pll)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &dvf99_pll_ops;
	init.parent_names = &parent;
	init.num_parents = 1;

	pll->reg = reg;
	pll->hw.init = &init;

	ret = clk_hw_register(NULL, &pll->hw);
	if (ret) {
		kfree(pll);
		return ERR_PTR(ret);
	}

	return &pll->hw;
}

static int dvf99_restart(struct sys_off_data *data)
{
	void __iomem *base = data->cb_data;

	writel(readl(base + CMU_RSTSR) | CMU_RSTSR_RST_FLAGS, base + CMU_RSTSR);
	writel(CMU_SWSYSRST_KEY1, base + CMU_SWSYSRST);
	writel(CMU_SWSYSRST_KEY2, base + CMU_SWSYSRST);

	return NOTIFY_DONE;
}

static void dvf99_resets_release(void __iomem *base)
{
	writel(readl(base + CMU_SWCHWRST1) & ~CMU_RST1_RELEASE,
	       base + CMU_SWCHWRST1);
	writel(readl(base + CMU_SWCHWRST2) & ~CMU_RST2_RELEASE,
	       base + CMU_SWCHWRST2);
}

static void __init dvf99_cmu_init(struct device_node *np)
{
	struct clk_hw_onecell_data *data;
	unsigned long osc_main_rate;
	struct clk_hw **hws;
	void __iomem *base;
	int i;

	base = of_iomap(np, 0);
	if (!base) {
		pr_err("%pOF: unable to map registers\n", np);
		return;
	}

	data = kzalloc(struct_size(data, hws, DVF99_CLK_NUM), GFP_KERNEL);
	if (!data)
		return;

	data->num = DVF99_CLK_NUM;
	hws = data->hws;

	dvf99_resets_release(base);

	osc_main_rate = readl(base + CMU_RSTSR) & CMU_RSTSR_OSC_25M ?
			DVF99_OSC_MAIN_25M : DVF99_OSC_MAIN_50M;

	hws[DVF99_CLK_OSC_MAIN] =
		clk_hw_register_fixed_rate(NULL, "osc_main", NULL, 0,
					   osc_main_rate);
	hws[DVF99_CLK_OSC_12M] =
		clk_hw_register_fixed_rate(NULL, "osc_12m", NULL, 0,
					   DVF99_OSC_12M_RATE);
	hws[DVF99_CLK_OSC_DECT] =
		clk_hw_register_fixed_rate(NULL, "osc_dect", NULL, 0,
					   DVF99_OSC_DECT_RATE);

	hws[DVF99_CLK_MAINCLK] =
		clk_hw_register_mux(NULL, "mainclk", dvf99_mainclk_parents,
				    ARRAY_SIZE(dvf99_mainclk_parents), 0,
				    base + CMU_RSTSR, CMU_RSTSR_MAINCLK_SHIFT, 1,
				    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	clk_hw_register_divider(NULL, "slowclk_div", "mainclk", 0,
				base + CMU_CLKSWCNTRL, CMU_CLKSW_SLOWCLK_SHIFT,
				CMU_CLKSW_SLOWCLK_WIDTH, CLK_DIVIDER_READ_ONLY,
				&dvf99_cmu_lock);

	hws[DVF99_CLK_SLOWCLK] =
		clk_hw_register_gate(NULL, "slowclk", "slowclk_div",
				     CLK_IGNORE_UNUSED, base + CMU_CLKSWCNTRL,
				     CMU_CLKSW_SLOWCLK_EN, 0, &dvf99_cmu_lock);

	clk_hw_register_mux(NULL, "pll1_ref", dvf99_root_parents,
			    ARRAY_SIZE(dvf99_root_parents), 0,
			    base + CMU_PLL1CONTROL, CMU_PLL_REF_SHIFT, 2,
			    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	hws[DVF99_CLK_PLL1] = dvf99_pll_register("pll1", "pll1_ref",
						 base + CMU_PLL1CONTROL);

	clk_hw_register_mux(NULL, "altsysclk", dvf99_root_parents,
			    ARRAY_SIZE(dvf99_root_parents), 0,
			    base + CMU_CPUCLKCNTRL, CMU_CPUCLK_ALTSYS_SHIFT, 2,
			    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	hws[DVF99_CLK_SYSCLK] =
		clk_hw_register_mux(NULL, "sysclk", dvf99_sysclk_parents,
				    ARRAY_SIZE(dvf99_sysclk_parents), 0,
				    base + CMU_CLKSWCNTRL, CMU_CLKSW_SYSCLK_SHIFT,
				    1, CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	hws[DVF99_CLK_HOSTCPU] =
		clk_hw_register_divider(NULL, "hostcpu", "sysclk", 0,
					base + CMU_CPUCLKCNTRL,
					CMU_CPUCLK_HOSTCPU_SHIFT, 4,
					CLK_DIVIDER_READ_ONLY, &dvf99_cmu_lock);

	hws[DVF99_CLK_AHB] =
		clk_hw_register_divider_table(NULL, "ahb", "hostcpu", 0,
					      base + CMU_CPUCLKCNTRL,
					      CMU_CPUCLK_AHB_SHIFT, 2,
					      CLK_DIVIDER_READ_ONLY,
					      dvf99_ahb_div_table,
					      &dvf99_cmu_lock);

	hws[DVF99_CLK_PLL2] = dvf99_pll_register("pll2", "mainclk",
						 base + CMU_PLL2CONTROL);

	clk_hw_register_mux(NULL, "lcdc_mux", dvf99_fast_parents,
			    ARRAY_SIZE(dvf99_fast_parents), 0,
			    base + CMU_CLKSWCNTRL, CMU_CLKSW_LCDC_SHIFT, 1,
			    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	clk_hw_register_divider(NULL, "lcdc_div", "lcdc_mux", 0,
				base + CMU_CLKDIVCNTRL1, CMU_CLKDIV1_LCDC_SHIFT,
				CMU_CLKDIV1_LCDC_WIDTH, CLK_DIVIDER_ROUND_CLOSEST,
				&dvf99_cmu_lock);

	clk_hw_register_mux(NULL, "sdmmc_mux", dvf99_fast_parents,
			    ARRAY_SIZE(dvf99_fast_parents), 0,
			    base + CMU_CLKSWCNTRL, CMU_CLKSW_SDMMC_SHIFT, 1,
			    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	clk_hw_register_divider(NULL, "sdmmc_div", "sdmmc_mux", 0,
				base + CMU_CLKDIVCNTRL1, CMU_CLKDIV1_SDMMC_SHIFT,
				CMU_CLKDIV1_SDMMC_WIDTH, CLK_DIVIDER_ROUND_CLOSEST,
				&dvf99_cmu_lock);

	clk_hw_register_mux(NULL, "qspi_mux", dvf99_fast_parents,
			    ARRAY_SIZE(dvf99_fast_parents), 0,
			    base + CMU_CLKSWCNTRL, CMU_CLKSW_QSPI_SHIFT, 1,
			    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	clk_hw_register_divider(NULL, "qspi_div", "qspi_mux", 0,
				base + CMU_CLKDIVCNTRL3, CMU_CLKDIV3_QSPI_SHIFT,
				CMU_CLKDIV3_QSPI_WIDTH, CLK_DIVIDER_ROUND_CLOSEST,
				&dvf99_cmu_lock);

	clk_hw_register_mux(NULL, "css_mux", dvf99_css_parents,
			    ARRAY_SIZE(dvf99_css_parents), 0,
			    base + CMU_COMCLKSEL, CMU_COMCLK_CSS_MUX_SHIFT,
			    CMU_COMCLK_CSS_MUX_WIDTH, CLK_MUX_READ_ONLY,
			    &dvf99_cmu_lock);

	clk_hw_register_divider(NULL, "css_div", "css_mux", 0,
				base + CMU_COMCLKSEL, CMU_COMCLK_CSS_DIV_SHIFT,
				CMU_COMCLK_CSS_DIV_WIDTH, 0, &dvf99_cmu_lock);

	clk_hw_register_mux(NULL, "audsrcclk", dvf99_root_parents,
			    ARRAY_SIZE(dvf99_root_parents), 0,
			    base + CMU_CPUCLKCNTRL, CMU_CPUCLK_AUDSRC_SHIFT, 2,
			    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	hws[DVF99_CLK_PLL3] = dvf99_pll_register("pll3", "audsrcclk",
						 base + CMU_PLL3CONTROL);

	hws[DVF99_CLK_AUDCLK] =
		clk_hw_register_mux(NULL, "audclk", dvf99_audclk_parents,
				    ARRAY_SIZE(dvf99_audclk_parents), 0,
				    base + CMU_CLKSWCNTRL, CMU_CLKSW_AUDCLK_SHIFT,
				    1, CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	clk_hw_register_fixed_factor(NULL, "audclk_div16", "audclk", 0, 1,
				     CMU_AUDCLK_DIV16);

	clk_hw_register_mux(NULL, "apu_mux", dvf99_apu_parents,
			    ARRAY_SIZE(dvf99_apu_parents), 0,
			    base + CMU_CLKSWCNTRL, CMU_CLKSW_APU_SHIFT, 1,
			    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	clk_hw_register_divider(NULL, "apu_div", "apu_mux", 0,
				base + CMU_CLKDIVCNTRL5, CMU_CLKDIV5_APU_SHIFT,
				CMU_CLKDIV5_APU_WIDTH, CLK_DIVIDER_READ_ONLY,
				&dvf99_cmu_lock);

	dvf99_div_register("css_adpcm_div", "css_ahb", base + CMU_COMCLKSEL,
			   CMU_COMCLK_ADPCM_SHIFT, CMU_COMCLK_ADPCM_WIDTH);

	clk_hw_register_mux(NULL, "bmp_mux", dvf99_bmp_parents,
			    ARRAY_SIZE(dvf99_bmp_parents), 0,
			    base + CMU_COMCLKSEL, CMU_COMCLK_BMP_MUX_SHIFT, 1,
			    CLK_MUX_READ_ONLY, &dvf99_cmu_lock);

	clk_hw_register_divider_table(NULL, "bmp_div", "bmp_mux", 0,
				      base + CMU_COMCLKSEL,
				      CMU_COMCLK_BMP_DIV_SHIFT, 1, 0,
				      dvf99_bmp_div_table, &dvf99_cmu_lock);

	dvf99_div_register("dclass_div", "audclk", base + CMU_CLKDIVCNTRL3,
			   CMU_CLKDIV3_DCLASS_SHIFT, CMU_CLKDIV3_DCLASS_WIDTH);

	clk_hw_register_mux(NULL, "tdm1_mux", dvf99_tdm_parents,
			    ARRAY_SIZE(dvf99_tdm_parents), 0,
			    base + CMU_CLKSWCNTRL, CMU_CLKSW_TDM1_SHIFT,
			    CMU_CLKSW_TDM_WIDTH, CLK_MUX_READ_ONLY,
			    &dvf99_cmu_lock);

	dvf99_div_register("tdm1_div", "tdm1_mux", base + CMU_CLKDIVCNTRL4,
			   CMU_CLKDIV4_TDM1_SHIFT, CMU_CLKDIV4_TDM_WIDTH);

	clk_hw_register_mux(NULL, "tdm2_mux", dvf99_tdm_parents,
			    ARRAY_SIZE(dvf99_tdm_parents), 0,
			    base + CMU_CLKSWCNTRL, CMU_CLKSW_TDM2_SHIFT,
			    CMU_CLKSW_TDM_WIDTH, CLK_MUX_READ_ONLY,
			    &dvf99_cmu_lock);

	dvf99_div_register("tdm2_div", "tdm2_mux", base + CMU_CLKDIVCNTRL4,
			   CMU_CLKDIV4_TDM2_SHIFT, CMU_CLKDIV4_TDM_WIDTH);

	for (i = 0; i < ARRAY_SIZE(dvf99_gates); i++)
		hws[dvf99_gates[i].id] = dvf99_gate_register(&dvf99_gates[i],
							     base);

	hws[DVF99_CLK_CSS_AHB] =
		clk_hw_register_fixed_factor(NULL, "css_ahb", "css", 0, 1, 2);

	for (i = 0; i < DVF99_CLK_NUM; i++)
		if (IS_ERR_OR_NULL(hws[i]))
			pr_err("%pOF: clock %d failed to register: %ld\n",
			       np, i, PTR_ERR(hws[i]));

	of_clk_add_hw_provider(np, of_clk_hw_onecell_get, data);

	dvf99_rst_register(np, base);

	register_sys_off_handler(SYS_OFF_MODE_RESTART, SYS_OFF_PRIO_DEFAULT,
				 dvf99_restart, base);
}
CLK_OF_DECLARE(dvf99_cmu, "dspg,dvf99-cmu", dvf99_cmu_init);
