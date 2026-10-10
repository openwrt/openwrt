// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s clocks: the top CRM and the CRPMs of the two groups of low
 * speed peripherals
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 *
 * The boot loader sets up the PLLs and the CPU and bus clocks, and the driver
 * leaves them as they are: it reads them once and registers their rates. The
 * gates, and the muxes and dividers of the peripherals, are under its
 * control.
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/reset-controller.h>
#include <linux/reset/reset-simple.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include <dt-bindings/clock/zte,zx279128s-crm.h>

/* Top CRM */
#define TOP_CLK_SEL		0x0c
#define  TOP_MATRIX_SEL_SHIFT	15
#define  TOP_CPU_SEL_SHIFT	13
#define TOP_CLK_EN		0x14
#define TOP_PLL_A9		0x18
#define TOP_PLL_AUDIO		0x28
#define TOP_USB_EN		0x48
#define TOP_RST_LAST		0x4c	/* the last register with resets */

#define TOP_NUM_CLKS		(ZX279128S_TOP_USB_REF + 1)
#define TOP_NUM_RESETS		((TOP_RST_LAST / 4 + 1) * 32)

/*
 * A PLL has two registers. The output is
 * osc * (fbdiv + frac / 2^24) / pd1 / pd2.
 */
#define PLL_FBDIV		GENMASK(17, 6)
#define PLL_PD1			GENMASK(5, 3)
#define PLL_PD2			GENMASK(2, 0)
#define PLL_FRAC		GENMASK(23, 0)	/* second register */
#define PLL_FRAC_BITS		24

/* The LSP CRPMs: one register per peripheral */
#define LSP_PCLK_EN		0	/* bit of the register clock gate */
#define LSP_WCLK_EN		1	/* bit of the work clock gate */
#define LSP_WCLK_SEL_SHIFT	9
#define LSP_WCLK_DIV_SHIFT	11

/* Clock gates and reset bits share registers, so they need the same lock. */
static struct reset_simple_data zx_top_reset = {
	.lock = __SPIN_LOCK_UNLOCKED(zx_top_reset.lock),
};

/*
 * Register a PLL as a fixed factor of the crystal, from the setting the boot
 * loader left in it.
 */
static struct clk_hw * __init zx_top_pll(struct device_node *np,
					 const char *name, void __iomem *reg)
{
	u32 cfg0 = readl(reg), cfg1 = readl(reg + 4);
	unsigned int pd = FIELD_GET(PLL_PD1, cfg0) * FIELD_GET(PLL_PD2, cfg0);
	u64 mult = ((u64)FIELD_GET(PLL_FBDIV, cfg0) << PLL_FRAC_BITS) |
		   FIELD_GET(PLL_FRAC, cfg1);
	u64 div = (u64)pd << PLL_FRAC_BITS;
	unsigned int shift;

	if (!mult || !pd)
		return ERR_PTR(-EINVAL);

	/* Fit both into 32 bits: drop trailing zeroes, then fraction bits */
	shift = min(__ffs64(mult), PLL_FRAC_BITS);
	mult >>= shift;
	div >>= shift;
	while (mult > UINT_MAX) {
		mult >>= 1;
		div >>= 1;
	}

	return clk_hw_register_fixed_factor_fwname(NULL, np, name, "osc", 0,
						   mult, div);
}

static struct clk_hw * __init zx_top_gate(void __iomem *reg,
					  const char *name,
					  const struct clk_hw *parent, u8 bit,
					  unsigned long flags)
{
	return clk_hw_register_gate_parent_hw(NULL, name, parent, flags, reg,
					      bit, 0, &zx_top_reset.lock);
}

/*
 * Reset bits are cleared to hold the reset. The registers also hold other
 * controls, including clock gates. Share the clock register lock so reset
 * and gate updates cannot overwrite each other.
 */
static int __init zx_top_reset_init(struct device_node *np,
				    void __iomem *base)
{
	struct reset_simple_data *rst = &zx_top_reset;

	rst->membase = base;
	rst->active_low = true;
	rst->status_active_low = true;
	rst->rcdev.ops = &reset_simple_ops;
	rst->rcdev.owner = THIS_MODULE;
	rst->rcdev.nr_resets = TOP_NUM_RESETS;
	rst->rcdev.of_node = np;

	return reset_controller_register(&rst->rcdev);
}

static void __init zx279128s_topcrm_init(struct device_node *np)
{
	const struct clk_hw *cpu_parents[4], *matrix_parents[4];
	struct clk_hw *osc, *lsp, *lsp_500m, *lsp_250m, *lsp_125m, *lsp_100m;
	struct clk_hw *audio, *audio_32k, *matrix, *hclk, *pclk, *m200, *m20;
	struct clk_hw_onecell_data *data;
	struct clk_hw **hws;
	void __iomem *base, *en;
	int i;

	base = of_iomap(np, 0);
	if (!base) {
		pr_err("%pOF: no registers\n", np);
		return;
	}

	data = kzalloc(struct_size(data, hws, TOP_NUM_CLKS), GFP_KERNEL);
	if (!data)
		return;
	data->num = TOP_NUM_CLKS;
	hws = data->hws;

	hws[ZX279128S_TOP_PLL_A9] = zx_top_pll(np, "pll_a9", base + TOP_PLL_A9);
	if (IS_ERR(hws[ZX279128S_TOP_PLL_A9]))
		goto err;
	osc = clk_hw_get_parent(hws[ZX279128S_TOP_PLL_A9]);
	if (!osc)
		goto err;

	/*
	 * The formula above gives 200 MHz for the LSP PLL, but the vendor
	 * kernel has it at 1 GHz, with fixed taps for the bus (250 MHz) and
	 * the peripherals (100 MHz). Until that is measured, it is registered
	 * with the vendor's rate.
	 */
	lsp = clk_hw_register_fixed_factor_parent_hw(NULL, "pll_lsp", osc, 0,
						     40, 1);
	if (IS_ERR(lsp))
		goto err;
	hws[ZX279128S_TOP_PLL_LSP] = lsp;
	lsp_500m = clk_hw_register_fixed_factor_parent_hw(NULL, "lsp_500m", lsp,
							  0, 1, 2);
	lsp_250m = clk_hw_register_fixed_factor_parent_hw(NULL, "lsp_250m", lsp,
							  0, 1, 4);
	lsp_125m = clk_hw_register_fixed_factor_parent_hw(NULL, "lsp_125m", lsp,
							  0, 1, 8);
	lsp_100m = clk_hw_register_fixed_factor_parent_hw(NULL, "lsp_100m", lsp,
							  0, 1, 10);
	if (IS_ERR(lsp_500m) || IS_ERR(lsp_250m) || IS_ERR(lsp_125m) ||
	    IS_ERR(lsp_100m))
		goto err;

	/* 49.152 MHz, and the 32.768 kHz derived from it */
	audio = zx_top_pll(np, "pll_audio", base + TOP_PLL_AUDIO);
	if (IS_ERR(audio))
		goto err;
	audio_32k = clk_hw_register_fixed_factor_parent_hw(NULL, "audio_32k",
							   audio, 0, 1, 1500);
	if (IS_ERR(audio_32k))
		goto err;

	cpu_parents[0] = osc;
	cpu_parents[1] = lsp_250m;
	cpu_parents[2] = lsp_500m;
	cpu_parents[3] = hws[ZX279128S_TOP_PLL_A9];
	hws[ZX279128S_TOP_CPU] =
		clk_hw_register_mux_hws(NULL, "cpu", cpu_parents, 4, 0,
					base + TOP_CLK_SEL, TOP_CPU_SEL_SHIFT,
					2, CLK_MUX_READ_ONLY, &zx_top_reset.lock);
	if (IS_ERR(hws[ZX279128S_TOP_CPU]))
		goto err;
	hws[ZX279128S_TOP_A9_PERIPH] =
		clk_hw_register_fixed_factor_parent_hw(NULL, "a9_periph",
						       hws[ZX279128S_TOP_CPU],
						       0, 1, 2);

	/*
	 * A 200 MHz clock of unknown source; the vendor kernel declares it at
	 * this rate. It feeds the bus select and, divided, the USB reference.
	 */
	m200 = clk_hw_register_fixed_rate(NULL, "clk_200m", NULL, 0,
					  200000000);
	if (IS_ERR(m200))
		goto err;
	m20 = clk_hw_register_fixed_factor_parent_hw(NULL, "clk_20m", m200, 0,
						     1, 10);
	if (IS_ERR(m20))
		goto err;

	matrix_parents[0] = osc;
	matrix_parents[1] = m200;
	matrix_parents[2] = lsp_250m;
	matrix_parents[3] = lsp_125m;
	matrix = clk_hw_register_mux_hws(NULL, "matrix_aclk", matrix_parents,
					 4, 0, base + TOP_CLK_SEL,
					 TOP_MATRIX_SEL_SHIFT, 2,
					 CLK_MUX_READ_ONLY, &zx_top_reset.lock);
	if (IS_ERR(matrix))
		goto err;
	hws[ZX279128S_TOP_MATRIX_ACLK] = matrix;
	hclk = clk_hw_register_fixed_factor_parent_hw(NULL, "matrix_hclk",
						      matrix, 0, 1, 2);
	hws[ZX279128S_TOP_MATRIX_HCLK] = hclk;
	pclk = clk_hw_register_fixed_factor_parent_hw(NULL, "matrix_pclk",
						      matrix, 0, 1, 2);
	if (IS_ERR(pclk))
		goto err;
	hws[ZX279128S_TOP_MATRIX_PCLK] = pclk;

	en = base + TOP_CLK_EN;
	hws[ZX279128S_TOP_LSP0_100M] = zx_top_gate(en, "lsp0_100m", lsp_100m,
						   13, 0);
	hws[ZX279128S_TOP_LSP0_32K] = zx_top_gate(en, "lsp0_32k", audio_32k,
						  12, 0);
	hws[ZX279128S_TOP_LSP0_PCLK] = zx_top_gate(en, "lsp0_pclk", pclk, 11, 0);
	hws[ZX279128S_TOP_LSP0_25M] = zx_top_gate(en, "lsp0_25m", osc, 9, 0);
	hws[ZX279128S_TOP_LSP1_ACLK] = zx_top_gate(en, "lsp1_aclk", matrix, 8, 0);
	hws[ZX279128S_TOP_LSP1_100M] = zx_top_gate(en, "lsp1_100m", lsp_100m,
						   7, 0);
	hws[ZX279128S_TOP_LSP1_49M] = zx_top_gate(en, "lsp1_49m", audio, 6, 0);
	hws[ZX279128S_TOP_LSP1_PCLK] = zx_top_gate(en, "lsp1_pclk", pclk, 5, 0);
	hws[ZX279128S_TOP_LSP1_25M] = zx_top_gate(en, "lsp1_25m", osc, 4, 0);
	/* the parked second CPU waits in the IRAM */
	hws[ZX279128S_TOP_IRAM_ACLK] = zx_top_gate(en, "iram_aclk", matrix, 2,
						   CLK_IS_CRITICAL);
	hws[ZX279128S_TOP_IROM_ACLK] = zx_top_gate(en, "irom_aclk", matrix, 1,
						   CLK_IS_CRITICAL);
	hws[ZX279128S_TOP_SYS_CTRL_PCLK] = zx_top_gate(en, "sys_ctrl_pclk", pclk,
						       0, CLK_IS_CRITICAL);

	/* The USB 3.0 controller's bus, suspend and reference clocks */
	en = base + TOP_USB_EN;
	hws[ZX279128S_TOP_USB_ACLK] = zx_top_gate(en, "usb_aclk", matrix, 21, 0);
	hws[ZX279128S_TOP_USB_SUSPEND] = zx_top_gate(en, "usb_suspend",
						     audio_32k, 20, 0);
	hws[ZX279128S_TOP_USB_REF] = zx_top_gate(en, "usb_ref", m20, 19, 0);

	for (i = 0; i < TOP_NUM_CLKS; i++)
		if (IS_ERR_OR_NULL(hws[i]))
			goto err;

	if (of_clk_add_hw_provider(np, of_clk_hw_onecell_get, data))
		goto err;

	if (zx_top_reset_init(np, base))
		pr_err("%pOF: failed to register the resets\n", np);
	return;

err:
	/* Without its timer and bus clocks the system won't boot anyway */
	pr_err("%pOF: failed to register the clocks\n", np);
}

CLK_OF_DECLARE(zx279128s_topcrm, "zte,zx279128s-topcrm",
	       zx279128s_topcrm_init);

/*
 * A peripheral of an LSP group: its work clock, optionally behind a mux of
 * two inputs or a divider, and its register clock.
 */
struct zx_lsp_periph {
	const char *name;
	unsigned int reg;
	int wclk;			/* index of the work clock, or -1 */
	int pclk;			/* index of the register clock */
	const char *parents[2];		/* the mux inputs, or one parent */
	u8 div_width;			/* 0: no divider */
};

struct zx_lsp_data {
	const struct zx_lsp_periph *periphs;
	unsigned int num_periphs;
	unsigned int num_clks;
	bool aclk;			/* the group has an AXI clock */
};

/* The work clock: an optional mux or divider, then the gate */
static struct clk_hw *zx_lsp_wclk(struct device *dev, void __iomem *reg,
				  spinlock_t *lock,
				  const struct zx_lsp_periph *p)
{
	struct clk_parent_data pd[2] = {
		{ .fw_name = p->parents[0] },
		{ .fw_name = p->parents[1] },
	};
	struct clk_hw *src = NULL;
	const char *name;

	if (p->parents[1]) {
		name = devm_kasprintf(dev, GFP_KERNEL, "%s_wsel", p->name);
		if (!name)
			return ERR_PTR(-ENOMEM);
		src = devm_clk_hw_register_mux_parent_data_table(dev, name, pd,
								 2, 0, reg,
								 LSP_WCLK_SEL_SHIFT,
								 1, 0, NULL,
								 lock);
	} else if (p->div_width) {
		name = devm_kasprintf(dev, GFP_KERNEL, "%s_wdiv", p->name);
		if (!name)
			return ERR_PTR(-ENOMEM);
		src = __devm_clk_hw_register_divider(dev, NULL, name, NULL,
						     NULL, pd, 0, reg,
						     LSP_WCLK_DIV_SHIFT,
						     p->div_width, 0, NULL,
						     lock);
	}
	if (IS_ERR(src))
		return src;

	name = devm_kasprintf(dev, GFP_KERNEL, "%s_wclk", p->name);
	if (!name)
		return ERR_PTR(-ENOMEM);
	if (src)
		return devm_clk_hw_register_gate_parent_hw(dev, name, src,
							   CLK_SET_RATE_PARENT,
							   reg, LSP_WCLK_EN, 0,
							   lock);
	return devm_clk_hw_register_gate_parent_data(dev, name, pd, 0, reg,
						     LSP_WCLK_EN, 0, lock);
}

/* The register clock */
static struct clk_hw *zx_lsp_pclk(struct device *dev, void __iomem *reg,
				  spinlock_t *lock,
				  const struct zx_lsp_periph *p)
{
	struct clk_parent_data pclk = { .fw_name = "pclk" };
	const char *name;

	name = devm_kasprintf(dev, GFP_KERNEL, "%s_pclk", p->name);
	if (!name)
		return ERR_PTR(-ENOMEM);
	return devm_clk_hw_register_gate_parent_data(dev, name, &pclk, 0, reg,
						     LSP_PCLK_EN, 0, lock);
}

static int zx279128s_lsp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct zx_lsp_data *match = device_get_match_data(dev);
	struct clk_hw_onecell_data *data;
	void __iomem *base;
	spinlock_t *lock;	/* serializes the group's clock registers */
	struct clk *clk;
	unsigned int i;

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base))
		return PTR_ERR(base);

	/* The group's own registers need these */
	clk = devm_clk_get_enabled(dev, "pclk");
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "no register clock\n");
	if (match->aclk) {
		clk = devm_clk_get_enabled(dev, "aclk");
		if (IS_ERR(clk))
			return dev_err_probe(dev, PTR_ERR(clk),
					     "no AXI clock\n");
	}

	lock = devm_kzalloc(dev, sizeof(*lock), GFP_KERNEL);
	data = devm_kzalloc(dev, struct_size(data, hws, match->num_clks),
			    GFP_KERNEL);
	if (!lock || !data)
		return -ENOMEM;
	spin_lock_init(lock);
	data->num = match->num_clks;

	for (i = 0; i < match->num_periphs; i++) {
		const struct zx_lsp_periph *p = &match->periphs[i];
		void __iomem *reg = base + p->reg;
		struct clk_hw *hw;

		if (p->wclk >= 0) {
			hw = zx_lsp_wclk(dev, reg, lock, p);
			if (IS_ERR(hw))
				return dev_err_probe(dev, PTR_ERR(hw),
						     "failed to register %s\n",
						     p->name);
			data->hws[p->wclk] = hw;
		}

		hw = zx_lsp_pclk(dev, reg, lock, p);
		if (IS_ERR(hw))
			return dev_err_probe(dev, PTR_ERR(hw),
					     "failed to register %s\n", p->name);
		data->hws[p->pclk] = hw;
	}

	return devm_of_clk_add_hw_provider(dev, of_clk_hw_onecell_get, data);
}

static const struct zx_lsp_periph zx_lsp0_periphs[] = {
	{ "uart0", 0x10, ZX279128S_LSP0_UART0_WCLK, ZX279128S_LSP0_UART0_PCLK,
	  { "wclk25m", "wclk100m" } },
	{ "uart1", 0x14, ZX279128S_LSP0_UART1_WCLK, ZX279128S_LSP0_UART1_PCLK,
	  { "wclk25m", "wclk100m" } },
	{ "spi", 0x18, ZX279128S_LSP0_SPI_WCLK, ZX279128S_LSP0_SPI_PCLK,
	  { "wclk100m" }, 6 },
	{ "gpio", 0x1c, -1, ZX279128S_LSP0_GPIO_PCLK },
};

static const struct zx_lsp_data zx_lsp0_data = {
	.periphs = zx_lsp0_periphs,
	.num_periphs = ARRAY_SIZE(zx_lsp0_periphs),
	.num_clks = ZX279128S_LSP0_GPIO_PCLK + 1,
};

static const struct zx_lsp_periph zx_lsp1_periphs[] = {
	{ "mdio", 0x04, ZX279128S_LSP1_MDIO_WCLK, ZX279128S_LSP1_MDIO_PCLK,
	  { "wclk100m" }, 7 },
};

static const struct zx_lsp_data zx_lsp1_data = {
	.periphs = zx_lsp1_periphs,
	.num_periphs = ARRAY_SIZE(zx_lsp1_periphs),
	.num_clks = ZX279128S_LSP1_MDIO_PCLK + 1,
	.aclk = true,
};

static const struct of_device_id zx279128s_lsp_of_match[] = {
	{ .compatible = "zte,zx279128s-lsp0crpm", .data = &zx_lsp0_data },
	{ .compatible = "zte,zx279128s-lsp1crpm", .data = &zx_lsp1_data },
	{ }
};

static struct platform_driver zx279128s_lsp_driver = {
	.probe = zx279128s_lsp_probe,
	.driver = {
		.name = "zx279128s-lspcrpm",
		.of_match_table = zx279128s_lsp_of_match,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(zx279128s_lsp_driver);
