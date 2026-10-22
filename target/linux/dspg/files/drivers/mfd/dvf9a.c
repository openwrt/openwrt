// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSP Group
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/iopoll.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/mfd/dvf9a.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#define DAIF_REG_STRIDE		4
#define DAIF_MIRRORED_END	64
#define DAIF_WAPU_END		114
#define DAIF_ANA_START		128
#define DAIF_ANA_END		160

#define DAIF_RAPU		41
#define DAIF_WAPU		64
#define DAIF_ANA_DATA_WR_LSB	65
#define DAIF_ANA_DATA_WR_MSB	66
#define DAIF_ANA_ADDR_WR	67
#define DAIF_ANA_ADDR_RD	68
#define DAIF_ANA_DATA_RD_LSB	69
#define DAIF_ANA_DATA_RD_MSB	70
#define DAIF_MODE		117
#define DAIF_PEND		118
#define DAIF_PENDB0		119

#define DAIF_RAPU_NEW_VALUE	BIT(15)
#define DAIF_RAPU_ADDR		GENMASK(14, 8)
#define DAIF_RAPU_DATA		GENMASK(7, 0)
#define DAIF_WAPU_ADDR		GENMASK(14, 8)
#define DAIF_WAPU_DATA		GENMASK(7, 0)
#define DAIF_MODE_RAPU_PRIO	BIT(2)

#define DAIF_TIMEOUT_US		1000
#define DAIF_RAPU_RETRIES	3
#define DAIF_ANA_DELAY_US	10

#define DAIF_ICU_ANA_STAT_EN1	31
#define DAIF_ICU_INT_POL	32
#define DAIF_ICU_ANA_EN1	98
#define DAIF_ICU_PWR_EN		99
#define DAIF_ICU_ANA_EN2	100
#define DAIF_ICU_PWR_STAT	101
#define DAIF_ICU_ANA_STAT1	103
#define DAIF_ICU_ANA_STAT2	104
#define DAIF_ICU_ANA_SRC1	108
#define DAIF_ICU_PWR_STAT_EN	114

#define DVF9A_NUM_IRQS		13
#define DVF9A_IRQ_EN1_NUM	7
#define DVF9A_IRQ_EN1_MASK	GENMASK(DVF9A_IRQ_EN1_NUM - 1, 0)
#define DVF9A_IRQ_MISC		BIT(7)
#define DVF9A_IRQ_STAT_ALL	0xff
#define DVF9A_IRQ_NO_STAT_EN	-1

struct dvf9a {
	struct device *dev;
	void __iomem *base;
	struct regmap *regmap;
	struct mutex lock;
	dvf9a_acquire_t acquire;
	void *acquire_priv;
	bool owner;

	struct irq_domain *domain;
	struct mutex irq_lock;
	u32 irq_enable;
	u32 irq_enable_hw;
};

static const s8 dvf9a_irq_stat_en[DVF9A_NUM_IRQS] = {
	0, 1, 2, 3, 4, DVF9A_IRQ_NO_STAT_EN, 6, DVF9A_IRQ_NO_STAT_EN,
	DVF9A_IRQ_NO_STAT_EN, DVF9A_IRQ_NO_STAT_EN, DVF9A_IRQ_NO_STAT_EN, 7,
	DVF9A_IRQ_NO_STAT_EN,
};

static u16 daif_raw_read(struct dvf9a *dvf9a, unsigned int reg)
{
	return readw(dvf9a->base + reg * DAIF_REG_STRIDE);
}

static void daif_raw_write(struct dvf9a *dvf9a, unsigned int reg, u16 val)
{
	writew(val, dvf9a->base + reg * DAIF_REG_STRIDE);
}

static int daif_pend_wait(struct dvf9a *dvf9a)
{
	u16 stat;

	return readw_poll_timeout_atomic(dvf9a->base +
					 DAIF_PEND * DAIF_REG_STRIDE,
					 stat, !stat, 1, DAIF_TIMEOUT_US);
}

static int daif_pendb_wait(struct dvf9a *dvf9a, unsigned int reg)
{
	unsigned int pend_reg = DAIF_PENDB0 + reg / 16;
	u16 stat;

	return readw_poll_timeout_atomic(dvf9a->base +
					 pend_reg * DAIF_REG_STRIDE,
					 stat, !(stat & BIT(reg % 16)), 1,
					 DAIF_TIMEOUT_US);
}

static int daif_rapu_read(struct dvf9a *dvf9a, unsigned int reg, u8 *val)
{
	void __iomem *rapu = dvf9a->base + DAIF_RAPU * DAIF_REG_STRIDE;
	unsigned int retry;
	u16 data;
	int ret;

	for (retry = 0; retry < DAIF_RAPU_RETRIES; retry++) {
		daif_raw_write(dvf9a, DAIF_RAPU, reg);
		ret = readw_poll_timeout_atomic(rapu, data,
						data & DAIF_RAPU_NEW_VALUE, 1,
						DAIF_TIMEOUT_US);
		if (ret)
			return ret;

		if (FIELD_GET(DAIF_RAPU_ADDR, data) == reg) {
			*val = FIELD_GET(DAIF_RAPU_DATA, data);
			return 0;
		}
	}

	return -EIO;
}

static int daif_wapu_write(struct dvf9a *dvf9a, unsigned int reg, u8 val)
{
	daif_raw_write(dvf9a, DAIF_WAPU, FIELD_PREP(DAIF_WAPU_ADDR, reg) |
					 FIELD_PREP(DAIF_WAPU_DATA, val));

	return daif_pend_wait(dvf9a);
}

static int daif_ana_read(struct dvf9a *dvf9a, unsigned int reg,
			 unsigned int *val)
{
	u8 msb, lsb;
	int ret;

	ret = daif_wapu_write(dvf9a, DAIF_ANA_ADDR_RD,
			      (reg - DAIF_ANA_START) / 2);
	if (ret)
		return ret;

	if (daif_raw_read(dvf9a, DAIF_MODE) & DAIF_MODE_RAPU_PRIO) {
		ret = daif_pend_wait(dvf9a);
		if (ret)
			return ret;
	}

	ret = daif_rapu_read(dvf9a, DAIF_ANA_DATA_RD_MSB, &msb);
	if (ret)
		return ret;

	udelay(DAIF_ANA_DELAY_US);

	ret = daif_rapu_read(dvf9a, DAIF_ANA_DATA_RD_LSB, &lsb);
	if (ret)
		return ret;

	*val = msb << 8 | lsb;

	return 0;
}

static int daif_ana_write(struct dvf9a *dvf9a, unsigned int reg,
			  unsigned int val)
{
	int ret;

	ret = daif_wapu_write(dvf9a, DAIF_ANA_DATA_WR_MSB, val >> 8);
	if (ret)
		return ret;

	ret = daif_wapu_write(dvf9a, DAIF_ANA_DATA_WR_LSB, val & 0xff);
	if (ret)
		return ret;

	return daif_wapu_write(dvf9a, DAIF_ANA_ADDR_WR,
			       (reg - DAIF_ANA_START) / 2);
}

static void dvf9a_owner_get(struct dvf9a *dvf9a)
{
	mutex_lock(&dvf9a->lock);

	if (dvf9a->owner)
		return;

	if (dvf9a->acquire && dvf9a->acquire(dvf9a->acquire_priv))
		dev_warn_ratelimited(dvf9a->dev, "DAIF acquire failed\n");

	dvf9a->owner = true;
}

static void dvf9a_owner_put(struct dvf9a *dvf9a)
{
	mutex_unlock(&dvf9a->lock);
}

static int daif_digital_read(struct dvf9a *dvf9a, unsigned int reg,
			     unsigned int *val)
{
	u8 data;
	int ret;

	ret = daif_rapu_read(dvf9a, reg, &data);
	if (!ret)
		*val = data;

	return ret;
}

static int daif_digital_write(struct dvf9a *dvf9a, unsigned int reg,
			      unsigned int val)
{
	int ret;

	if (reg > DAIF_MIRRORED_END)
		return daif_wapu_write(dvf9a, reg, val);

	ret = daif_pendb_wait(dvf9a, reg);
	if (ret)
		return ret;

	daif_raw_write(dvf9a, reg, val);

	return daif_pendb_wait(dvf9a, reg);
}

static int dvf9a_reg_read(void *context, unsigned int reg, unsigned int *val)
{
	struct dvf9a *dvf9a = context;
	int ret;

	dvf9a_owner_get(dvf9a);
	if (reg >= DAIF_ANA_START)
		ret = daif_ana_read(dvf9a, reg, val);
	else
		ret = daif_digital_read(dvf9a, reg, val);
	dvf9a_owner_put(dvf9a);

	return ret;
}

static int dvf9a_reg_write(void *context, unsigned int reg, unsigned int val)
{
	struct dvf9a *dvf9a = context;
	int ret;

	dvf9a_owner_get(dvf9a);
	if (reg >= DAIF_ANA_START)
		ret = daif_ana_write(dvf9a, reg, val);
	else
		ret = daif_digital_write(dvf9a, reg, val);
	dvf9a_owner_put(dvf9a);

	return ret;
}

static bool dvf9a_reg_valid(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case 9:
	case 12 ... 13:
	case 27:
	case DAIF_RAPU:
	case 45:
	case 50:
	case 52 ... DAIF_ANA_DATA_RD_MSB:
	case 80:
	case 90:
	case 105:
	case DAIF_WAPU_END + 1 ... DAIF_ANA_START - 1:
		return false;
	}

	if (reg >= DAIF_ANA_START)
		return !(reg & 1);

	return true;
}

static const struct regmap_config dvf9a_regmap_config = {
	.reg_bits = 8,
	.val_bits = 16,
	.max_register = DAIF_ANA_END,
	.reg_read = dvf9a_reg_read,
	.reg_write = dvf9a_reg_write,
	.readable_reg = dvf9a_reg_valid,
	.writeable_reg = dvf9a_reg_valid,
	.cache_type = REGCACHE_NONE,
};

/**
 * dvf9a_acquire_hook_set() - set the callback that takes the DAIF back
 * @dvf9a: DVF9A core, the driver data of the DVF9A device
 * @acquire: callback that asks the CSS to hand back the DAIF and waits for
 *	     its answer, or NULL to remove the callback
 * @priv: argument for @acquire
 *
 * Removing the callback hands the DAIF to Linux.
 *
 * Return: 0 on success, -EBUSY if another callback is set.
 */
int dvf9a_acquire_hook_set(struct dvf9a *dvf9a, dvf9a_acquire_t acquire,
			   void *priv)
{
	int ret = 0;

	mutex_lock(&dvf9a->lock);

	if (acquire && dvf9a->acquire) {
		ret = -EBUSY;
		goto out;
	}

	dvf9a->acquire = acquire;
	dvf9a->acquire_priv = priv;
	if (!acquire)
		dvf9a->owner = true;
out:
	mutex_unlock(&dvf9a->lock);

	return ret;
}
EXPORT_SYMBOL_GPL(dvf9a_acquire_hook_set);

/**
 * dvf9a_owner_release() - hand the DAIF to the CSS
 * @dvf9a: DVF9A core, the driver data of the DVF9A device
 *
 * Waits until a running DAIF access ends. The next access calls the acquire
 * callback first. May sleep.
 */
void dvf9a_owner_release(struct dvf9a *dvf9a)
{
	mutex_lock(&dvf9a->lock);
	dvf9a->owner = false;
	mutex_unlock(&dvf9a->lock);
}
EXPORT_SYMBOL_GPL(dvf9a_owner_release);

static void dvf9a_irq_mask(struct irq_data *d)
{
	struct dvf9a *dvf9a = irq_data_get_irq_chip_data(d);

	dvf9a->irq_enable &= ~BIT(irqd_to_hwirq(d));
}

static void dvf9a_irq_unmask(struct irq_data *d)
{
	struct dvf9a *dvf9a = irq_data_get_irq_chip_data(d);

	dvf9a->irq_enable |= BIT(irqd_to_hwirq(d));
}

static void dvf9a_irq_bus_lock(struct irq_data *d)
{
	struct dvf9a *dvf9a = irq_data_get_irq_chip_data(d);

	mutex_lock(&dvf9a->irq_lock);
}

static void dvf9a_irq_bus_sync_unlock(struct irq_data *d)
{
	struct dvf9a *dvf9a = irq_data_get_irq_chip_data(d);
	u32 changed = dvf9a->irq_enable ^ dvf9a->irq_enable_hw;
	unsigned int stat_en = 0;
	unsigned int hwirq;

	if (!changed)
		goto out;

	for (hwirq = 0; hwirq < DVF9A_NUM_IRQS; hwirq++)
		if (dvf9a_irq_stat_en[hwirq] != DVF9A_IRQ_NO_STAT_EN &&
		    dvf9a->irq_enable & BIT(hwirq))
			stat_en |= BIT(dvf9a_irq_stat_en[hwirq]);

	regmap_write(dvf9a->regmap, DAIF_ICU_ANA_EN1,
		     dvf9a->irq_enable & DVF9A_IRQ_EN1_MASK);
	regmap_write(dvf9a->regmap, DAIF_ICU_ANA_STAT_EN1, stat_en);
	regmap_write(dvf9a->regmap, DAIF_ICU_ANA_EN2,
		     dvf9a->irq_enable >> DVF9A_IRQ_EN1_NUM);

	dvf9a->irq_enable_hw = dvf9a->irq_enable;
out:
	mutex_unlock(&dvf9a->irq_lock);
}

static const struct irq_chip dvf9a_irq_chip = {
	.name = "dvf9a",
	.irq_mask = dvf9a_irq_mask,
	.irq_unmask = dvf9a_irq_unmask,
	.irq_bus_lock = dvf9a_irq_bus_lock,
	.irq_bus_sync_unlock = dvf9a_irq_bus_sync_unlock,
	.flags = IRQCHIP_SKIP_SET_WAKE,
};

static int dvf9a_irq_map(struct irq_domain *domain, unsigned int virq,
			 irq_hw_number_t hwirq)
{
	struct dvf9a *dvf9a = domain->host_data;

	irq_set_chip_data(virq, dvf9a);
	irq_set_chip_and_handler(virq, &dvf9a_irq_chip, handle_simple_irq);
	irq_set_nested_thread(virq, true);
	irq_set_noprobe(virq);

	return 0;
}

static const struct irq_domain_ops dvf9a_irq_domain_ops = {
	.map = dvf9a_irq_map,
	.xlate = irq_domain_xlate_onecell,
};

static irqreturn_t dvf9a_irq_thread(int irq, void *data)
{
	struct dvf9a *dvf9a = data;
	unsigned int src, misc = 0;
	unsigned long pending;
	unsigned int hwirq;

	if (regmap_read(dvf9a->regmap, DAIF_ICU_ANA_SRC1, &src))
		return IRQ_NONE;

	if (!src)
		src = DVF9A_IRQ_MISC;

	if (src & DVF9A_IRQ_MISC) {
		if (regmap_read(dvf9a->regmap, DAIF_ICU_ANA_STAT2, &misc))
			return IRQ_NONE;
		regmap_write(dvf9a->regmap, DAIF_ICU_ANA_STAT2, misc);
	}

	src &= DVF9A_IRQ_EN1_MASK;
	if (src)
		regmap_write(dvf9a->regmap, DAIF_ICU_ANA_STAT1, src);

	pending = src | misc << DVF9A_IRQ_EN1_NUM;
	pending &= dvf9a->irq_enable_hw;
	if (!pending)
		return IRQ_NONE;

	for_each_set_bit(hwirq, &pending, DVF9A_NUM_IRQS)
		handle_nested_irq(irq_find_mapping(dvf9a->domain, hwirq));

	return IRQ_HANDLED;
}

static void dvf9a_irq_domain_remove(void *data)
{
	irq_domain_remove(data);
}

static int dvf9a_irq_init(struct dvf9a *dvf9a, int irq)
{
	struct device *dev = dvf9a->dev;
	static const struct reg_sequence init[] = {
		{ DAIF_ICU_INT_POL, 0 },
		{ DAIF_ICU_ANA_EN1, 0 },
		{ DAIF_ICU_ANA_EN2, 0 },
		{ DAIF_ICU_ANA_STAT_EN1, 0 },
		{ DAIF_ICU_ANA_STAT1, DVF9A_IRQ_STAT_ALL },
		{ DAIF_ICU_ANA_STAT2, DVF9A_IRQ_STAT_ALL },
		{ DAIF_ICU_PWR_EN, 0 },
		{ DAIF_ICU_PWR_STAT_EN, 0 },
		{ DAIF_ICU_PWR_STAT, DVF9A_IRQ_STAT_ALL },
	};
	int ret;

	ret = devm_mutex_init(dev, &dvf9a->irq_lock);
	if (ret)
		return ret;

	ret = regmap_multi_reg_write(dvf9a->regmap, init, ARRAY_SIZE(init));
	if (ret)
		return ret;

	dvf9a->domain = irq_domain_create_linear(dev_fwnode(dev),
						 DVF9A_NUM_IRQS,
						 &dvf9a_irq_domain_ops, dvf9a);
	if (!dvf9a->domain)
		return -ENOMEM;

	ret = devm_add_action_or_reset(dev, dvf9a_irq_domain_remove,
				       dvf9a->domain);
	if (ret)
		return ret;

	return devm_request_threaded_irq(dev, irq, NULL, dvf9a_irq_thread,
					 IRQF_ONESHOT, dev_name(dev), dvf9a);
}

/**
 * dvf9a_irq_get() - get the Linux interrupt of a DVF9A interrupt line
 * @dvf9a: DVF9A core, the driver data of the DVF9A device
 * @hwirq: DVF9A interrupt line, 0 to 12
 *
 * Return: the Linux interrupt number, or a negative error code.
 */
int dvf9a_irq_get(struct dvf9a *dvf9a, unsigned int hwirq)
{
	unsigned int virq;

	if (!dvf9a->domain)
		return -ENXIO;

	if (hwirq >= DVF9A_NUM_IRQS)
		return -EINVAL;

	virq = irq_create_mapping(dvf9a->domain, hwirq);

	return virq ? virq : -ENXIO;
}
EXPORT_SYMBOL_GPL(dvf9a_irq_get);

static int dvf9a_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dvf9a *dvf9a;
	struct clk *clk;
	int irq, ret;

	dvf9a = devm_kzalloc(dev, sizeof(*dvf9a), GFP_KERNEL);
	if (!dvf9a)
		return -ENOMEM;

	dvf9a->dev = dev;
	dvf9a->owner = true;
	platform_set_drvdata(pdev, dvf9a);

	ret = devm_mutex_init(dev, &dvf9a->lock);
	if (ret)
		return ret;

	dvf9a->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(dvf9a->base))
		return PTR_ERR(dvf9a->base);

	clk = devm_clk_get_enabled(dev, "pmu");
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "no pmu clock\n");

	clk = devm_clk_get_enabled(dev, "apu");
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "no apu clock\n");

	clk = devm_clk_get_enabled(dev, "rfapu");
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "no rfapu clock\n");

	dvf9a->regmap = devm_regmap_init(dev, NULL, dvf9a,
					 &dvf9a_regmap_config);
	if (IS_ERR(dvf9a->regmap))
		return PTR_ERR(dvf9a->regmap);

	irq = platform_get_irq_optional(pdev, 0);
	if (irq > 0) {
		ret = dvf9a_irq_init(dvf9a, irq);
		if (ret)
			return dev_err_probe(dev, ret, "irq setup failed\n");
	} else if (irq != -ENXIO) {
		return irq;
	}

	return devm_of_platform_populate(dev);
}

static const struct of_device_id dvf9a_of_match[] = {
	{ .compatible = "dspg,dvf9a" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf9a_of_match);

static struct platform_driver dvf9a_driver = {
	.probe = dvf9a_probe,
	.driver = {
		.name = "dvf9a",
		.of_match_table = dvf9a_of_match,
	},
};
module_platform_driver(dvf9a_driver);

MODULE_DESCRIPTION("DSPG DVF9A core driver");
MODULE_LICENSE("GPL");
