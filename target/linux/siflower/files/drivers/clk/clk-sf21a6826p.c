#include <dt-bindings/clock/sf21a6826p-clk.h>
#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/types.h>

#define TOP_CRM_BASE 	(0x0CE00400)
#define PLL_CMN_LOCK 	(TOP_CRM_BASE + 0xC8)
#define PLL_DDR_LOCK 	(TOP_CRM_BASE + 0xCC)
#define PLL_ETH_LOCK 	(TOP_CRM_BASE + 0xD0)
#define PLL_PCIE_LOCK 	(TOP_CRM_BASE + 0xD4)
#define PLL_LOAD 		(TOP_CRM_BASE + 0x100)

#define CLK_SEL0 		(TOP_CRM_BASE + 0x80)
#define CLK_SEL1 		(TOP_CRM_BASE + 0x84)

#define CLK_COUNT 		25
#define PBUS_CLK_COUNT 	7

static unsigned long PBUS_DEF_VAL[PBUS_CLK_COUNT];
static const unsigned long PLL_FREQ[] = {2400000000, 3200000000, 10000000000, 10000000000};
static const int PCIE_PLL_DEF_COF[] = { 4, 4, 4, 3, 4, 0 };
static int PLL_STAT[] = { 0, 0, 0, 0 };

static const int div_range[4][4] = {
	{16, 320, 63, 7},
	{32, 1000, 63, 7},
	{32, 1000, 63, 8},
	{32, 1000, 63, 8}};

typedef struct sf_clk_data {
	spinlock_t *lock;
	unsigned long clk_flags;
} sf_clk_data_t;

/* CM_CFG */

typedef enum cm_pll_t {
	CMN_PLL,
	DDR_PLL,
	ETH_PLL,
	PCIE_PLL,
} sf21a6826p_pll;

typedef enum cm_cfg_t {
	CPU_CFG,
	REALCPU_CFG,
	GIC_CFG,
	AXI_CFG,
	AHB_CFG,
	APB_CFG,
	SERDES_CSR_CFG,
	IRAM_CFG,
	IROM_CFG,
	NPU_CFG,
	DDR_PHY_CFG,
	DDR_BYP_CFG,
	ETH_PHY_REF_CFG_P,
	ETH_PHY_REF_CFG_N,
	ETH_TSU_CFG,
	GMAC_BYP_REF_CFG,
	PCIE_REF_CFG_P,
	PCIE_REF_CFG_N,
	UART_EXT_CFG,
	USBPHY_REF_CFG,
	USB_BUS_CFG,
	USB_SUS_CFG,
	CRYPTCSR_CFG,
	CRYPTAPP_CFG
} sf21a6826p_cfg;

typedef enum cm_pbus_t {
	I2C_PBUS,
	SPI_PBUS,
	UART_PBUS,
	PWM_PBUS,
	TIMER_PBUS,
	WDT_PBUS,
	GPIO_PBUS
} sf21a6826p_pbus;

static DEFINE_SPINLOCK(cm_cfg_lock);
static DEFINE_SPINLOCK(cm_cfg_lock_cpu);
static DEFINE_SPINLOCK(cm_pll_lock);
static DEFINE_SPINLOCK(cm_pll_lock_ddr);
static DEFINE_SPINLOCK(cm_pll_lock_pcie);
static DEFINE_SPINLOCK(cm_pll_lock_eth);

struct sf_clk_pll {
	struct clk_hw hw;
	void __iomem *base;
	int type;
	int div_shift;
	unsigned long div1;
	unsigned long div2;
	spinlock_t lock;
};

static sf_clk_data_t sf21a6826p_clk_data = {
	.lock = &cm_cfg_lock,
	.clk_flags = CLK_GET_RATE_NOCACHE,
};
static sf_clk_data_t sf21a6826p_clk_data_on = {
	.lock = &cm_cfg_lock,
	.clk_flags = CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED,
};
static sf_clk_data_t sf21a6826p_clk_data_cpu = {
	.lock = &cm_cfg_lock_cpu,
	.clk_flags = CLK_SET_RATE_NO_REPARENT | CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED,
};
static sf_clk_data_t sf21a6826p_clk_data_realcpu = {
	.lock = &cm_cfg_lock,
	.clk_flags = CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED | CLK_SET_RATE_PARENT,
};
static sf_clk_data_t sf21a6826p_pll_data_ddr = {
	.lock = &cm_pll_lock_ddr,
	.clk_flags = CLK_SET_RATE_NO_REPARENT | CLK_IGNORE_UNUSED,
};
static sf_clk_data_t sf21a6826p_pll_data_eth = {
	.lock = &cm_pll_lock_eth,
	.clk_flags = CLK_SET_RATE_NO_REPARENT | CLK_GET_RATE_NOCACHE | CLK_SET_RATE_GATE,
};
static sf_clk_data_t sf21a6826p_pll_data_pcie = {
	.lock = &cm_pll_lock_pcie,
	.clk_flags = CLK_SET_RATE_NO_REPARENT | CLK_GET_RATE_NOCACHE | CLK_SET_RATE_GATE,
};
static sf_clk_data_t sf21a6826p_pll_data_cmn = {
	.lock = &cm_pll_lock,
	.clk_flags = CLK_SET_RATE_NO_REPARENT | CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED,
};

static const struct of_device_id sf21a6826p_cfg_of_match[] = {
	{
		.compatible = "siflower,sf21a6826p-cmn-pll",
		.data = &sf21a6826p_pll_data_cmn,
	},
	{
		.compatible = "siflower,sf21a6826p-ddr-pll",
		.data = &sf21a6826p_pll_data_ddr,
	},
	{
		.compatible = "siflower,sf21a6826p-eth-pll",
		.data = &sf21a6826p_pll_data_eth,
	},
	{
		.compatible = "siflower,sf21a6826p-pcie-pll",
		.data = &sf21a6826p_pll_data_pcie,
	},
	{
		.compatible = "siflower,sf21a6826p-cpu-clk",
		.data = &sf21a6826p_clk_data_cpu,
	},
	{
		.compatible = "siflower,sf21a6826p-realcpu-clk",
		.data = &sf21a6826p_clk_data_realcpu,
	},
	{
		.compatible = "siflower,sf21a6826p-gic-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-axi-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-ahb-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-apb-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-serdescsr-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-iram-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-irom-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-npu-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-ddrphyref-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-ddrbyp-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-ethphyrefp-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-ethphyrefn-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-ethtsu-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-gmacbypref-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-pcierefp-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-pcierefn-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-uartext-clk",
		.data = &sf21a6826p_clk_data_on,
	},
	{
		.compatible = "siflower,sf21a6826p-usbphyref-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-usbbus-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-usbsus-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-pbus-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-cryptcsr-clk",
		.data = &sf21a6826p_clk_data,
	},
	{
		.compatible = "siflower,sf21a6826p-cryptapp-clk",
		.data = &sf21a6826p_clk_data,
	},
	{}
};

static const struct of_device_id *sf_get_match(int index,
											   struct device_node *np)
{
	return of_match_node(&sf21a6826p_cfg_of_match[index], np);
}

static const char **__init clk_mux_get_parents(struct device_node *np,
											   unsigned char *num_parents)
{
	const char **parents;
	int nparents, i;

	nparents = of_count_phandle_with_args(np, "clocks", "#clock-cells");
	if (WARN_ON(nparents <= 0))
		return ERR_PTR(-EINVAL);

	parents = kzalloc(nparents * sizeof(const char *), GFP_KERNEL);
	if (!parents)
		return ERR_PTR(-ENOMEM);

	for (i = 0; i < nparents; i++)
		parents[i] = of_clk_get_parent_name(np, i);

	*num_parents = nparents;
	return parents;
}

static void pbus_clk_endisable(struct clk_hw *hw, int enable)
{
	u32 tmp;
	struct clk_gate *gate = to_clk_gate(hw);
	unsigned long flags;
	if (gate->lock)
		spin_lock_irqsave(gate->lock, flags);
	else
		__acquire(gate->lock);

	tmp = readl(gate->reg);
	if (gate->bit_idx == DEFAULT_PBUS)
		tmp = PBUS_DEF_VAL[gate->flags];
	else {
		if (enable == 1)
			tmp |= BIT(gate->bit_idx - 1);
		else
			tmp &= ~BIT(gate->bit_idx - 1);
	}
	writel(tmp, gate->reg);
	if (gate->lock)
		spin_unlock_irqrestore(gate->lock, flags);
	else
		__release(gate->lock);
}

static int pbus_clk_enable(struct clk_hw *hw)
{
	pbus_clk_endisable(hw, 1);
	return 0;
}

static void pbus_clk_disable(struct clk_hw *hw)
{
	pbus_clk_endisable(hw, 0);
}

static int pbus_clk_is_enabled(struct clk_hw *hw)
{
	u32 reg;
	struct clk_gate *gate = to_clk_gate(hw);
	reg = readl(gate->reg);
	if (!gate->bit_idx)
		reg &= BIT(gate->bit_idx - 1);
	else
		return reg == 0 ? 1 : 0;
	return reg ? 1 : 0;
}

static struct clk_ops sf_pbus_clk_gate_ops = {
	.enable = pbus_clk_enable,
	.disable = pbus_clk_disable,
	.is_enabled = pbus_clk_is_enabled,
};

static void __init sf21a6826p_pbus_setup(struct device_node *np)
{
	unsigned char num_parents;
	const struct of_device_id *match;
	struct clk *clk;
	const char **parents;
	struct clk_gate *gate;
	void __iomem *reg = NULL;
	const char *clk_name[16];
	struct clk_onecell_data *clk_data;
	sf_clk_data_t *data;
	struct resource res;
	int rc, i, type;
	unsigned int tmp;

	/* of_match_node() will check device node. */
	match = sf_get_match((CLK_COUNT - 1), np);
	if (!match) {
		goto err;
	}

	if (of_property_read_u32(np, "gate-width", &tmp)) {
		goto err;
	}

	data = (sf_clk_data_t *)match->data;

	rc = of_address_to_resource(np, 0, &res);
	if (rc != 0) {
		goto err;
	}
	reg = of_iomap(np, 0);
	if (!reg) {
		goto err;
	}

	parents = clk_mux_get_parents(np, &num_parents);

	if (!parents) {
		goto err;
	}

	clk_data = kzalloc(sizeof(struct clk_onecell_data), GFP_KERNEL);
	if (!clk_data) {
		goto err_alloc_clk_data;
	}

	clk_data->clk_num = tmp + 1;
	if (of_property_read_string_array(np, "clock-output-names", clk_name, clk_data->clk_num) < 0) {
		goto err_alloc_clks;
	}

	clk_data->clks = kzalloc(clk_data->clk_num * sizeof(struct clk *),
							 GFP_KERNEL);
	if (!clk_data->clks) {
		goto err_alloc_clks;
	}

	if (0 == strcmp(clk_name[0], "i2cmclk0"))
		type = I2C_PBUS;
	else if (0 == strcmp(clk_name[0], "spimclk0"))
		type = SPI_PBUS;
	else if (0 == strcmp(clk_name[0], "uartmclk0"))
		type = UART_PBUS;
	else if (0 == strcmp(clk_name[0], "pwmmclk0"))
		type = PWM_PBUS;
	else if (0 == strcmp(clk_name[0], "timermclk0"))
		type = TIMER_PBUS;
	else if (0 == strcmp(clk_name[0], "wdtmclk0"))
		type = WDT_PBUS;
	else if (0 == strcmp(clk_name[0], "gpiomclk0"))
		type = GPIO_PBUS;

	for (i = 0; i < clk_data->clk_num; i++) {
		gate = kzalloc(sizeof(struct clk_gate), GFP_KERNEL);
		if (!gate) {
			goto err;
		}
		gate->reg = reg;
		gate->bit_idx = i;
		gate->flags = type;
		gate->lock = data->lock;

		clk = clk_register_composite(NULL, clk_name[i], parents, num_parents,
									 NULL, NULL,
									 NULL, NULL,
									 &gate->hw, &sf_pbus_clk_gate_ops,
									 data->clk_flags);
		if (IS_ERR(clk)) {
			kfree(gate);
			goto err_register;
		}
		clk_data->clks[i] = clk;
	}
	of_clk_add_provider(np, of_clk_src_onecell_get, clk_data);
	PBUS_DEF_VAL[type] = readl(gate->reg);
	kfree(parents);
	return;

err_register:
	kfree(clk_data->clks);
err_alloc_clks:
	kfree(clk_data);
err_alloc_clk_data:
	kfree(parents);
err:
	pr_err("pbus clk init failed!\n");
	return;
}

static long sf_clk_div_round_rate(struct clk_hw *hw, unsigned long rate,
								  unsigned long *prate)
{
	struct clk_divider *divider = to_clk_divider(hw);

	/* if read only, just return current value */
	if (divider->flags & CLK_DIVIDER_READ_ONLY) {
		u32 val;

		val = readl(divider->reg) >> divider->shift;
		val &= clk_div_mask(divider->width);

		return divider_ro_round_rate(hw, rate, prate, divider->table,
									 divider->width, divider->flags,
									 val);
	}

	return divider_round_rate(hw, rate, prate, divider->table,
							  divider->width, divider->flags);
}

static unsigned long sf_clk_div_recalc_rate(struct clk_hw *hw,
											unsigned long parent_rate)
{
	struct clk_divider *divider = to_clk_divider(hw);
	u32 val;

	val = readl(divider->reg) >> divider->shift;
	val &= clk_div_mask(divider->width);

	return divider_recalc_rate(hw, parent_rate, val, divider->table,
							   divider->flags, divider->width);
}

static int sf_clk_div_set_rate(struct clk_hw *hw, unsigned long rate,
							   unsigned long parent_rate)
{
	struct clk_divider *divider = to_clk_divider(hw);
	int value;
	unsigned long flags = 0;
	u32 val;
	void __iomem *load = ioremap(PLL_LOAD, 8);

	value = divider_get_val(rate, parent_rate, divider->table,
							divider->width, divider->flags);
	if (value < 0)
		return value;

	if (divider->lock)
		spin_lock_irqsave(divider->lock, flags);
	else
		__acquire(divider->lock);

	if (divider->flags & CLK_DIVIDER_HIWORD_MASK) {
		val = clk_div_mask(divider->width) << (divider->shift + 16);
	} else {
		val = readl(divider->reg);
		val &= ~(clk_div_mask(divider->width) << divider->shift);
	}
	val |= (u32)value << divider->shift;
	writel(val, divider->reg);
	writel(readl(load) | BIT(0), load);
	writel(readl(load) & ~BIT(0), load);

	if (divider->lock)
		spin_unlock_irqrestore(divider->lock, flags);
	else
		__release(divider->lock);
	iounmap(load);

	return 0;
}

static unsigned long sf_cpu_clk_recalc_rate(struct clk_hw *hw,
					    unsigned long parent_rate)
{
	struct clk_divider *divider = to_clk_divider(hw);
	int revise = (readl(divider->reg) & BIT(divider->shift)) ? 15 : 10;
	return DIV_ROUND_UP_ULL(parent_rate * 10, revise);
}

static long sf_cpu_clk_round_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long *prate)
{
	unsigned long rem, max_rate = 1200000000, min_rate = 3125000;
	struct clk_hw *pclk = clk_hw_get_parent(hw);
	if (rate > max_rate || rate < min_rate)
		return -EINVAL;

	if (rate == (*prate) || (rate * 15) == ((*prate) * 10))
		return rate;

	rem = ((*prate) * 10) % (rate * 15);
	if (!rem) {
		(*prate) = clk_hw_round_rate(pclk,
					     DIV_ROUND_UP_ULL((rate * 15), 10));
		return rate;
	}
	rem = max_rate % (rate);
	if (!rem) {
		(*prate) = clk_hw_round_rate(pclk, rate);
		return rate;
	}
	rem = (max_rate * 10) % (rate * 15);
	if (!rem) {
		(*prate) = clk_hw_round_rate(pclk,
					     DIV_ROUND_UP_ULL((rate * 15), 10));
		return rate;
	}
	return -EINVAL;
}

int sf_cpu_clk_set_rate(struct clk_hw *hw, unsigned long rate,
			unsigned long parent_rate)
{
	struct clk_divider *divider = to_clk_divider(hw);
	int revise = readl(divider->reg);
	if (rate == parent_rate) {
		writel(revise & (~BIT(divider->shift)), divider->reg);
	} else if ((rate * 15) == (parent_rate * 10)) {
		writel(revise | BIT(divider->shift), divider->reg);
	} else {
		return -EINVAL;
	}
	return 0;
}

static struct clk_ops sf_real_cpu_clk_divider_ops = {
	.recalc_rate = sf_cpu_clk_recalc_rate,
	.round_rate = sf_cpu_clk_round_rate,
	.set_rate = sf_cpu_clk_set_rate,
};

static struct clk_ops sf_clk_divider_ops = {
	.recalc_rate = sf_clk_div_recalc_rate,
	.round_rate = sf_clk_div_round_rate,
	.set_rate = sf_clk_div_set_rate,
};

static void __init sf21a6826p_cfg_setup(struct device_node *np, int cfg_type)
{
	unsigned char num_parents = '\0';
	const struct of_device_id *match;
	struct clk *clk;
	const char **parents;
	struct clk_gate *gate = NULL;
	struct clk_divider *div = NULL;
	void __iomem *reg = NULL, *div_reg = NULL, *gate_reg = NULL;
	const char *clk_name;
	struct clk_onecell_data *clk_data;
	sf_clk_data_t *data;
	struct resource res;
	int i, tmp;

	/* of_match_node() will check device node. */
	match = sf_get_match(cfg_type + 4, np);
	if (!match)
		goto err;

	data = (sf_clk_data_t *)match->data;

	for (i = 0; i < 2; i++) {
		if (of_address_to_resource(np, i, &res)) {
			if (!i)
				goto err;
			break;
		}
		reg = of_iomap(np, i);
		if (!reg)
			goto err;
		if (!strcmp(res.name, "div-reg"))
			div_reg = reg;
		else if (!strcmp(res.name, "gate-reg"))
			gate_reg = reg;
	}

	if (of_property_read_string(np, "clock-output-names", &clk_name))
		goto err;

	parents = clk_mux_get_parents(np, &num_parents);

	if (!parents)
		goto err;

	clk_data = kzalloc(sizeof(struct clk_onecell_data), GFP_KERNEL);
	if (!clk_data)
		goto err_alloc_clk_data;

	clk_data->clk_num = 1;
	clk_data->clks = kzalloc(clk_data->clk_num * sizeof(struct clk *),
							 GFP_KERNEL);

	if (!clk_data->clks)
		goto err_alloc_clks;

	if (gate_reg) {
		gate = kzalloc(sizeof(struct clk_gate), GFP_KERNEL);
		if (!gate)
			goto err_alloc_gate;

		gate->reg = gate_reg;
		of_property_read_u32(np, "gate-bit", &tmp);
		gate->bit_idx = tmp;
		gate->flags = 0;
		gate->lock = data->lock;
	}

	if (div_reg) {
		div = kzalloc(sizeof(struct clk_divider), GFP_KERNEL);
		if (!div) {
			goto err_alloc_div;
		}
		div->reg = div_reg;
		of_property_read_u32(np, "div-shift", &tmp);
		div->shift = tmp;
		div->width = 8;
		if (cfg_type == REALCPU_CFG)
			div->width = 1;
		div->lock = data->lock;
		div->flags = CLK_DIVIDER_ROUND_CLOSEST | CLK_DIVIDER_ALLOW_ZERO;
		if (gate) {
			if (cfg_type == REALCPU_CFG) {
				clk = clk_register_composite(
					NULL, clk_name, parents, num_parents,
					NULL, NULL, &div->hw,
					&sf_real_cpu_clk_divider_ops, &gate->hw,
					&clk_gate_ops, data->clk_flags);
			} else {
				clk = clk_register_composite(
					NULL, clk_name, parents, num_parents,
					NULL, NULL, &div->hw,
					&sf_clk_divider_ops, &gate->hw,
					&clk_gate_ops, data->clk_flags);
			}

		} else {
			clk = clk_register_composite(NULL, clk_name, parents,
						     num_parents, NULL, NULL,
						     &div->hw,
						     &sf_clk_divider_ops, NULL,
						     NULL, data->clk_flags);
		}
	} else {
		clk = clk_register_composite(NULL, clk_name, parents, num_parents,
									 NULL, NULL,
									 NULL, NULL,
									 &gate->hw, &clk_gate_ops,
									 data->clk_flags);
	}
	if (IS_ERR(clk))
		goto err_register;

	clk_data->clks[0] = clk;
	of_clk_add_provider(np, of_clk_src_onecell_get, clk_data);
	if ((data->clk_flags & CLK_IGNORE_UNUSED) && !clk_gate_is_enabled(&gate->hw))
		clk_prepare_enable(clk);
	kfree(parents);
	return;
err_register:
	kfree(div);
err_alloc_div:
	kfree(gate);
err_alloc_gate:
	kfree(clk_data->clks);
err_alloc_clks:
	kfree(clk_data);
err_alloc_clk_data:
	kfree(parents);
err:
	pr_err("clk %d init failed!\n", cfg_type);
	return;
}

static int sf_pll_enable(struct clk_hw *hw)
{
	struct sf_clk_pll *pll = container_of(hw, struct sf_clk_pll, hw);
	u32 enable;
	enable = readl(pll->base);
	switch (pll->type) {
	case CMN_PLL:
		writel(enable & ~BIT(26), pll->base);
		break;
	case DDR_PLL:
		writel(enable | BIT(22), pll->base);
		break;
	case ETH_PLL:
	case PCIE_PLL:
		writel(enable | BIT(31), pll->base);
		break;
	default:
		return -EINVAL;
	}
	PLL_STAT[pll->type]++;
	return 0;
}

static void sf_pll_disable(struct clk_hw *hw)
{
	struct sf_clk_pll *pll = container_of(hw, struct sf_clk_pll, hw);
	u32 enable;
	PLL_STAT[pll->type]--;
	if (PLL_STAT[pll->type])
		return;
	enable = readl(pll->base);
	switch (pll->type) {
	case CMN_PLL:
		writel(enable | BIT(26), pll->base);
		break;
	case DDR_PLL:
		writel(enable & ~BIT(22), pll->base);
		break;
	case ETH_PLL:
	case PCIE_PLL:
		writel(enable & ~BIT(31), pll->base);
		break;
	default:
		return;
	}
	return;
}

static int sf_pll_is_enabled(struct clk_hw *hw)
{
	struct sf_clk_pll *pll = container_of(hw, struct sf_clk_pll, hw);
	u32 enable = 0;
	switch (pll->type) {
	case CMN_PLL:
		enable = !((readl(pll->base) >> 26) & 0x1);
		break;
	case DDR_PLL:
		enable = (readl(pll->base) >> 22) & 0x1;
		break;
	case ETH_PLL:
	case PCIE_PLL:
		enable = (readl(pll->base) >> 31) & 0x1;
		break;
	default:
		return -EINVAL;
	}
	return enable;
}

static void sf_pll_set_all_params(struct sf_clk_pll *pll)
{
	u32 reg;

	if (pll->type == CMN_PLL)
		reg = readl(pll->base);
	else
		reg = readl(pll->base + 0x4);
	reg &= ~(0x3F << pll->div_shift);
	reg |= pll->div2 << pll->div_shift;
	reg |= pll->div1 << (pll->div_shift + 3);
	if (pll->type == CMN_PLL)
		writel(reg, pll->base);
	else
		writel(reg, pll->base + 0x4);
	return;
}

static void sf_pll_get_all_params(struct sf_clk_pll *pll)
{
	u32 reg;
	if (pll->type == CMN_PLL) {
		reg = readl(pll->base);
	} else {
		reg = readl(pll->base + 0x4);
	}
	pll->div2 = (reg >> pll->div_shift) & 0x7;
	pll->div1 = (reg >> (pll->div_shift + 3)) & 0x7;
	return;
}

static unsigned long sf_pll_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	unsigned long rate = 0;
	struct sf_clk_pll *pll = container_of(hw, struct sf_clk_pll, hw);
	sf_pll_get_all_params(pll);
	parent_rate = PLL_FREQ[pll->type];
	switch (pll->type) {
	case CMN_PLL:
	case DDR_PLL:
		rate = parent_rate / (pll->div1 * pll->div2);
		break;
	case ETH_PLL:
	case PCIE_PLL:
		rate = parent_rate / (4 * (pll->div1 + 1) * (pll->div2 + 1));
		break;
	default:
		return -EINVAL;
	}
	return rate;
}

static long sf_pll_round_rate(struct clk_hw *hw, unsigned long rate,
							  unsigned long *parent_rate)
{
	struct sf_clk_pll *pll = container_of(hw, struct sf_clk_pll, hw);
	unsigned long min_freq = 0, max_freq = 0;

	sf_pll_get_all_params(pll);
	switch (pll->type) {
	case CMN_PLL:
	case DDR_PLL:
		min_freq = PLL_FREQ[pll->type] / 49;
		max_freq = PLL_FREQ[pll->type];
		break;
	case ETH_PLL:
	case PCIE_PLL:
		min_freq = PLL_FREQ[pll->type] / (4 * 81);
		max_freq = PLL_FREQ[pll->type] / 4;
		break;
	default:
		return -EINVAL;
	}
	if ((rate < min_freq) || (rate > max_freq))
		return -EINVAL;

	return (long)rate;
}

static void
get_params(bool is_vco, int pll_type, unsigned long *div_first,
		   unsigned long *div_sec, unsigned long frac, unsigned long parent_rate, unsigned long goal_rate)
{
	unsigned long cur_rate, div1, div2, new1, new2, diff, dsm;
	div1 = *div_first;
	div2 = *div_sec;
	if (is_vco) {
		dsm = 16777216;
		div1 *= dsm;
		div2 = (div2 * dsm + frac);
	}
	new1 = div1;
	new2 = div2;
	if (is_vco) {
		if (goal_rate % parent_rate == 0) {
			new2 = goal_rate / parent_rate;
			new1 = 1;
			goto done;
		}
		cur_rate = parent_rate / div1 * div2;
		goal_rate = parent_rate / new1 * new2;
		if (cur_rate % goal_rate == 0) {
			diff = cur_rate / goal_rate;
			if (div2 % diff == 0)
				new2 = div2 / diff;
			else if ((div1 * diff) <= div_range[pll_type][2])
				new1 = div1 * diff;
			else
				goto try;
		} else if (goal_rate % cur_rate == 0) {
			diff = goal_rate / cur_rate;
			if ((div2 * diff) <= div_range[pll_type][1])
				new2 = div2 * diff;
			else if (div1 % diff == 0)
				new1 = div1 / diff;
			else
				goto try;
		}
		try:
			for (new1 = 1; new1 <= div_range[pll_type][2]; new1++) {
				for (new2 = div_range[pll_type][0]; new2 <= div_range[pll_type][1]; new2++) {
					if ((goal_rate * new1) == (parent_rate * new2))
						goto done;
				}
			}
	} else {
		for (new1 = 1; new1 <= div_range[pll_type][3]; new1++) {
			for (new2 = 1; new2 <= new1; new2++) {
				if (pll_type < 2)
					cur_rate = parent_rate / (new1 * new2);
				else
					cur_rate = parent_rate / (4 * (new1 + 1) * (new2 + 1));
				if ((parent_rate / (new1 * new2)) == goal_rate)
					goto done;
			}
		}
	}
done:
	*div_first = new1;
	*div_sec = new2;
	if (is_vco) {
		*div_first /= dsm;
		*div_sec = (new2 - frac) / dsm;
	}
	return;
}
static unsigned long ref_clk_switch[4][2] = {
	{0b10001000101010101, 0x8000},
	{0x140000, 0},
	{0x15000000, 0},
	{0x200000, 0x3}
};

static void change_to_ref_clk(bool yes, int type)
{
	u32 reg;
	void __iomem *sel0, *sel1;
	sel0 = ioremap(CLK_SEL0, 8);
	sel1 = ioremap(CLK_SEL1, 8);
	reg = readl(sel0);
	if (yes)
		reg |= ref_clk_switch[type][0];
	else
		reg &= ~ref_clk_switch[type][0];
	writel(reg, sel0);
	reg = readl(sel1);
	if (yes)
		reg |= ref_clk_switch[type][1];
	else
		reg &= ~ref_clk_switch[type][1];
	writel(reg, sel1);
	iounmap(sel0);
	iounmap(sel1);
}

static int sf_pll_set_rate(struct clk_hw *hw, unsigned long rate,
						   unsigned long parent_rate)
{
	struct sf_clk_pll *pll = container_of(hw, struct sf_clk_pll, hw);
	unsigned int para_load;
	unsigned long flags;
	void __iomem *pll_lock, *load;
	u32 ret;
	load = ioremap(PLL_LOAD, 8);
	parent_rate = PLL_FREQ[pll->type];

	spin_lock_irqsave(&pll->lock, flags);
	change_to_ref_clk(true, pll->type);
	sf_pll_disable(hw);
	if (rate == sf_pll_recalc_rate(hw, parent_rate))
		return 0;

	switch (pll->type) {
	case CMN_PLL:
		pll_lock = ioremap(PLL_CMN_LOCK, 8);
		para_load = BIT(1);
		break;
	case DDR_PLL:
		pll_lock = ioremap(PLL_DDR_LOCK, 8);
		para_load = BIT(2);
		break;
	case ETH_PLL:
		pll_lock = ioremap(PLL_ETH_LOCK, 8);
		para_load = BIT(3);
		break;
	case PCIE_PLL:
		pll_lock = ioremap(PLL_PCIE_LOCK, 8);
		para_load = BIT(4);
		break;
	default:
		return -EINVAL;
	}
	writel(readl(load) | para_load, load);
	writel(readl(load) & ~para_load, load);
	get_params(false, pll->type, &pll->div1, &pll->div2, 0, parent_rate,
		   	   rate);
	sf_pll_set_all_params(pll);
	writel(readl(load) | para_load, load);
	writel(readl(load) & ~para_load, load);

	sf_pll_enable(hw);
	writel(readl(load) | para_load, load);
	writel(readl(load) & ~para_load, load);
	do {
		ret = readl(pll_lock) & 0x1;
	} while (!ret);
	change_to_ref_clk(false, pll->type);
	iounmap(load);
	iounmap(pll_lock);
	spin_unlock_irqrestore(&pll->lock, flags);
	return 0;
}

static struct clk_ops sf_clk_pll_ops = {
	.enable = sf_pll_enable,
	.disable = sf_pll_disable,
	.is_enabled = sf_pll_is_enabled,
	.recalc_rate = sf_pll_recalc_rate,
	.round_rate = sf_pll_round_rate,
	.set_rate = sf_pll_set_rate,
};

static unsigned long get_oscclk_rate(struct clk_hw *hw)
{
	struct clk *osc_clk = clk_get_parent(hw->clk);
	return clk_get_rate(osc_clk);
}

static void set_all_vco(struct sf_clk_pll *pll, unsigned long osc_rate)
{
	unsigned long refdiv, fbdiv, frac, dsm, reg, cur_pll;
	dsm = 16777216;
	frac = 0;
	switch (pll->type) {
	case CMN_PLL:
		reg = readl(pll->base);
		refdiv = GENMASK(5, 0) & reg;
		fbdiv = (GENMASK(25, 14) & reg) >> 14;
		break;
	case DDR_PLL:
		reg = readl(pll->base + 0x8);
		refdiv = GENMASK(5, 0) & reg;
		fbdiv = (GENMASK(17, 6) & reg) >> 6;
		if (readl(pll->base) & BIT(19))
			frac = readl(pll->base + 0x4) & GENMASK(23, 0);
		break;
	case ETH_PLL:
	case PCIE_PLL:
		reg = readl(pll->base);
		refdiv = (GENMASK(29, 24) & reg) >> 24;
		reg = readl(pll->base + 0x8);
		fbdiv = GENMASK(11, 0) & reg;
		if (reg & BIT(29))
			frac = readl(pll->base) & GENMASK(23, 0);
		break;
	}
	cur_pll = osc_rate * (fbdiv * dsm + frac) / (refdiv * dsm);
	if (cur_pll == PLL_FREQ[pll->type] || pll->type == DDR_PLL) {
		return;
	} else
		get_params(true, pll->type, &refdiv, &fbdiv, frac, osc_rate,
			       PLL_FREQ[pll->type]);
	switch (pll->type) {
	case CMN_PLL:
		reg = readl(pll->base);
		reg &= ~GENMASK(5, 0);
		reg |= FIELD_PREP(GENMASK(5, 0), refdiv);
		reg &= ~GENMASK(25, 14);
		reg |= FIELD_PREP(GENMASK(25, 14), fbdiv);
		writel(reg, pll->base);
		return;
	case DDR_PLL:
		reg = readl(pll->base + 0x8);
		reg &= ~GENMASK(5, 0);
		reg |= FIELD_PREP(GENMASK(5, 0), refdiv);
		reg &= ~GENMASK(17, 6);
		reg |= FIELD_PREP(GENMASK(17, 6), fbdiv);
		writel(reg, pll->base + 0x8);
		return;
	case PCIE_PLL:
		/*setting default config*/
		reg = readl(pll->base + 0x4);
		reg &= ~GENMASK(17, 0);
		reg |= FIELD_PREP(GENMASK(17, 15), PCIE_PLL_DEF_COF[0]);
		reg |= FIELD_PREP(GENMASK(14, 12), PCIE_PLL_DEF_COF[1]);
		reg |= FIELD_PREP(GENMASK(11, 9), PCIE_PLL_DEF_COF[2]);
		reg |= FIELD_PREP(GENMASK(8, 6), PCIE_PLL_DEF_COF[3]);
		reg |= FIELD_PREP(GENMASK(5, 3), PCIE_PLL_DEF_COF[4]);
		reg |= FIELD_PREP(GENMASK(2, 0), PCIE_PLL_DEF_COF[5]);
		writel(reg, pll->base + 0x4);
		reg = readl(pll->base);
		reg &= ~GENMASK(29, 24);
		reg |= FIELD_PREP(GENMASK(29, 24), refdiv);
		writel(reg, pll->base);
		reg = readl(pll->base + 0x8);
		reg &= ~GENMASK(11, 0);
		reg |= FIELD_PREP(GENMASK(11, 0), fbdiv);
		writel(reg, pll->base + 0x8);
		return;
	}
}

static void __init sf21a6826p_pll_setup(struct device_node *np, int pll_type)
{
	struct clk *clk;
	struct clk_init_data init = {};
	struct sf_clk_pll *pll;
	struct clk_onecell_data *clk_data;
	const struct of_device_id *match;
	const char *pll_name = NULL;
	sf_clk_data_t *data;

	pll = kzalloc(sizeof(struct sf_clk_pll), GFP_KERNEL);
	if (!pll)
		goto out_of_mem;

	match = sf_get_match(pll_type, np);
	if (!match)
		goto err;

	data = (sf_clk_data_t *)match->data;

	if (of_property_read_string(np, "clock-output-names", &pll_name))
		goto err;

	pll->base = of_iomap(np, 0);
	if (!pll->base)
		goto err;

	pll->type = pll_type;

	if (of_property_read_u32(np, "div-shift", &pll->div_shift))
		goto err;

	spin_lock_init(&pll->lock);

	init.name = pll_name;
	init.flags = data->clk_flags;
	init.ops = &sf_clk_pll_ops;
	init.parent_names = clk_mux_get_parents(np, &init.num_parents);

	pll->hw.init = &init;
	clk = clk_register(NULL, &pll->hw);
	kfree(init.parent_names);
	if (IS_ERR(clk)) {
		goto err;
	} else {
		clk_data = kzalloc(sizeof(struct clk_onecell_data), GFP_KERNEL);
		if (!clk_data)
			goto err;
		clk_data->clk_num = 1;
		clk_data->clks = kzalloc(clk_data->clk_num * sizeof(struct clk *),
								 GFP_KERNEL);
		if (!clk_data->clks) {
			kfree(clk_data);
			goto err;
		}
		clk_data->clks[0] = clk;
		of_clk_add_provider(np, of_clk_src_onecell_get, clk_data);
		/*init pll vco freq*/
		set_all_vco(pll, get_oscclk_rate(&pll->hw));
		if ((data->clk_flags & CLK_IGNORE_UNUSED) && !sf_pll_is_enabled(&pll->hw))
			sf_pll_enable(&pll->hw);
		return;
	}
err:
	kfree(pll);
out_of_mem:
	pr_err("%s failed!\nType is %d!\n", __func__, pll_type);
	return;
}

/*setup*/
#define SF_CLK_DECLARE(name, compat, type)                              \
	static void __init sf21a6826p_##name##_setup(struct device_node *np) \
	{                                                                   \
		sf21a6826p_cfg_setup(np, type);                                  \
		return;                                                         \
	}                                                                   \
	CLK_OF_DECLARE(name, compat, sf21a6826p_##name##_setup)

#define SF_PLL_DECLARE(name, compat, type)                              \
	static void __init sf21a6826p_##name##_setup(struct device_node *np) \
	{                                                                   \
		sf21a6826p_pll_setup(np, type);                                  \
		return;                                                         \
	}                                                                   \
	CLK_OF_DECLARE(name, compat, sf21a6826p_##name##_setup)

CLK_OF_DECLARE(sf21a6826p_pbus_clk, "siflower,sf21a6826p-pbus-clk", sf21a6826p_pbus_setup);
SF_CLK_DECLARE(sf21a6826p_cpu_clk, "siflower,sf21a6826p-cpu-clk", CPU_CFG);
SF_CLK_DECLARE(sf21a6826p_realcpu_clk, "siflower,sf21a6826p-realcpu-clk", REALCPU_CFG);
SF_CLK_DECLARE(sf21a6826p_gic_clk, "siflower,sf21a6826p-gic-clk", GIC_CFG);
SF_CLK_DECLARE(sf21a6826p_axi_clk, "siflower,sf21a6826p-axi-clk", AXI_CFG);
SF_CLK_DECLARE(sf21a6826p_ahb_clk, "siflower,sf21a6826p-ahb-clk", AHB_CFG);
SF_CLK_DECLARE(sf21a6826p_apb_clk, "siflower,sf21a6826p-apb-clk", APB_CFG);
SF_CLK_DECLARE(sf21a6826p_serdescsr_clk, "siflower,sf21a6826p-serdescsr-clk", SERDES_CSR_CFG);
SF_CLK_DECLARE(sf21a6826p_iram_clk, "siflower,sf21a6826p-iram-clk", IRAM_CFG);
SF_CLK_DECLARE(sf21a6826p_irom_clk, "siflower,sf21a6826p-irom-clk", IROM_CFG);
SF_CLK_DECLARE(sf21a6826p_ethrefclkp_clk, "siflower,sf21a6826p-ethphyrefp-clk", ETH_PHY_REF_CFG_P);
SF_CLK_DECLARE(sf21a6826p_cryptcsr_clk, "siflower,sf21a6826p-cryptcsr-clk", CRYPTCSR_CFG);
SF_CLK_DECLARE(sf21a6826p_cryptapp_clk, "siflower,sf21a6826p-cryptapp-clk", CRYPTAPP_CFG);
SF_CLK_DECLARE(sf21a6826p_npu_clk, "siflower,sf21a6826p-npu-clk", NPU_CFG);
SF_CLK_DECLARE(sf21a6826p_ddrphyref_clk, "siflower,sf21a6826p-ddrphyref-clk", DDR_PHY_CFG);
SF_CLK_DECLARE(sf21a6826p_ddrbyp_clk, "siflower,sf21a6826p-ddrbyp-clk", DDR_BYP_CFG);
SF_CLK_DECLARE(sf21a6826p_ethtsu_clk, "siflower,sf21a6826p-ethtsu-clk", ETH_TSU_CFG);
SF_CLK_DECLARE(sf21a6826p_gmacbypref_clk, "siflower,sf21a6826p-gmacbypref-clk", GMAC_BYP_REF_CFG);
SF_CLK_DECLARE(sf21a6826p_pcierefp_clk, "siflower,sf21a6826p-pcierefp-clk", PCIE_REF_CFG_P);
SF_CLK_DECLARE(sf21a6826p_pcierefn_clk, "siflower,sf21a6826p-pcierefn-clk", PCIE_REF_CFG_N);
SF_CLK_DECLARE(sf21a6826p_uartext_clk, "siflower,sf21a6826p-uartext-clk", UART_EXT_CFG);
SF_PLL_DECLARE(sf21a6826p_cmn_pll, "siflower,sf21a6826p-cmn-pll", CMN_PLL);
SF_PLL_DECLARE(sf21a6826p_ddr_pll, "siflower,sf21a6826p-ddr-pll", DDR_PLL);
SF_PLL_DECLARE(sf21a6826p_pcie_pll, "siflower,sf21a6826p-pcie-pll", PCIE_PLL);
