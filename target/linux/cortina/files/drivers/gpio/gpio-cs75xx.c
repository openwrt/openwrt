// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/gpio/driver.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#define DRV_NAME	"cs75xx-gpio"

#define GPIO_PIN_NUM	32

#define OFFS_CFG	0x00
#define OFFS_OUT	0x04
#define OFFS_IN		0x08
#define OFFS_LVL	0x0C
#define OFFS_EDGE	0x10
#define OFFS_IE		0x14
#define OFFS_INT	0x18

#define VAL_DIR_IN	0
#define VAL_DIR_OUT	1

struct cs75xx_gpio {
	int			id;
	void __iomem		*reg_base;
	void __iomem		*mux_reg;
	int			irq;
	struct gpio_chip	gpio_chip;
	spinlock_t		lock;
	u32			toggle_mask;
};

static int cs75xx_gpio_request(struct gpio_chip *chip, unsigned int offs)
{
	struct cs75xx_gpio *priv = gpiochip_get_data(chip);
	unsigned long flags;
	u32 regval;

	spin_lock_irqsave(&priv->lock, flags);
	regval = readl(priv->mux_reg);
	regval |= BIT(offs);
	writel(regval, priv->mux_reg);
	spin_unlock_irqrestore(&priv->lock, flags);
	return 0;
}

static void cs75xx_gpio_free(struct gpio_chip *chip, unsigned int offset)
{
	struct cs75xx_gpio *priv = gpiochip_get_data(chip);
	unsigned long flags;
	u32 regval;

	spin_lock_irqsave(&priv->lock, flags);
	regval = readl(priv->mux_reg);
	regval &= ~BIT(offset);
	writel(regval, priv->mux_reg);
	spin_unlock_irqrestore(&priv->lock, flags);
}

static void _gpio_set(struct cs75xx_gpio *priv, unsigned int offset, int val)
{
	u32 regval;

	regval = readl(priv->reg_base + OFFS_OUT);
	if (val)
		regval |= BIT(offset);
	else
		regval &= ~BIT(offset);
	writel(regval, priv->reg_base + OFFS_OUT);
}

static int cs75xx_gpio_set(struct gpio_chip *chip, unsigned int offset, int val)
{
	struct cs75xx_gpio *priv = gpiochip_get_data(chip);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	_gpio_set(priv, offset, val);
	spin_unlock_irqrestore(&priv->lock, flags);
	return 0;
}

static int cs75xx_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct cs75xx_gpio *priv = gpiochip_get_data(chip);

	return !!(readl(priv->reg_base + OFFS_IN) & BIT(offset));
}

static void _gpio_dir(struct cs75xx_gpio *priv, unsigned int offset, int dir)
{
	u32 regval;

	regval = readl(priv->reg_base + OFFS_CFG);
	if (dir == VAL_DIR_OUT)
		regval &= ~BIT(offset);
	else
		regval |= BIT(offset);
	writel(regval, priv->reg_base + OFFS_CFG);
}

static int cs75xx_gpio_get_direction(struct gpio_chip *chip, unsigned int offset)
{
	struct cs75xx_gpio *priv = gpiochip_get_data(chip);

	return (readl(priv->reg_base + OFFS_CFG) & BIT(offset)) ?
		GPIO_LINE_DIRECTION_IN : GPIO_LINE_DIRECTION_OUT;
}

static int cs75xx_gpio_dir_in(struct gpio_chip *chip, unsigned int offset)
{
	struct cs75xx_gpio *priv = gpiochip_get_data(chip);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	_gpio_dir(priv, offset, VAL_DIR_IN);
	spin_unlock_irqrestore(&priv->lock, flags);
	return 0;
}

static int cs75xx_gpio_dir_out(struct gpio_chip *chip, unsigned int offset, int val)
{
	struct cs75xx_gpio *priv = gpiochip_get_data(chip);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	_gpio_set(priv, offset, val);
	_gpio_dir(priv, offset, VAL_DIR_OUT);
	spin_unlock_irqrestore(&priv->lock, flags);
	return 0;
}

static void _toggle_irq_edge_trig(struct cs75xx_gpio *priv, unsigned int offs)
{
	u32 regval;

	regval = readl(priv->reg_base + OFFS_LVL);
	writel(regval ^ BIT(offs), priv->reg_base + OFFS_LVL);
}

static void cs75xx_gpio_irq_handler(struct irq_desc *desc)
{
	struct gpio_chip *gc = irq_desc_get_handler_data(desc);
	struct cs75xx_gpio *priv = gpiochip_get_data(gc);
	struct irq_chip *irqchip = irq_desc_get_chip(desc);
	unsigned long flags;
	u32 irq_stat;

	chained_irq_enter(irqchip, desc);

	spin_lock_irqsave(&priv->lock, flags);
	irq_stat = readl(priv->reg_base + OFFS_INT);
	irq_stat &= readl(priv->reg_base + OFFS_IE);
	spin_unlock_irqrestore(&priv->lock, flags);

	while (irq_stat) {
		unsigned int offset = __fls(irq_stat);

		if (priv->toggle_mask & BIT(offset)) {
			spin_lock_irqsave(&priv->lock, flags);
			_toggle_irq_edge_trig(priv, offset);
			spin_unlock_irqrestore(&priv->lock, flags);
		}
		generic_handle_domain_irq(gc->irq.domain, offset);
		irq_stat &= ~BIT(offset);
	}

	chained_irq_exit(irqchip, desc);
}

static void cs75xx_gpio_irq_ack(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct cs75xx_gpio *priv = gpiochip_get_data(gc);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	writel(BIT(d->hwirq), priv->reg_base + OFFS_INT);
	spin_unlock_irqrestore(&priv->lock, flags);
}

static void cs75xx_gpio_irq_mask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct cs75xx_gpio *priv = gpiochip_get_data(gc);
	unsigned long flags;
	u32 regval;

	spin_lock_irqsave(&priv->lock, flags);
	regval = readl(priv->reg_base + OFFS_IE);
	regval &= ~BIT(d->hwirq);
	writel(regval, priv->reg_base + OFFS_IE);
	spin_unlock_irqrestore(&priv->lock, flags);
	gpiochip_disable_irq(gc, irqd_to_hwirq(d));
}

static void cs75xx_gpio_irq_unmask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct cs75xx_gpio *priv = gpiochip_get_data(gc);
	unsigned long flags;
	u32 regval;

	gpiochip_enable_irq(gc, irqd_to_hwirq(d));
	spin_lock_irqsave(&priv->lock, flags);
	writel(BIT(d->hwirq), priv->reg_base + OFFS_INT);
	regval = readl(priv->reg_base + OFFS_IE);
	regval |= BIT(d->hwirq);
	writel(regval, priv->reg_base + OFFS_IE);
	spin_unlock_irqrestore(&priv->lock, flags);
}

static int cs75xx_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct cs75xx_gpio *priv = gpiochip_get_data(gc);
	unsigned long flags;
	u32 reg_lvl, reg_edge;
	u32 mask = BIT(d->hwirq);
	int res = 0;

	spin_lock_irqsave(&priv->lock, flags);

	priv->toggle_mask &= ~mask;

	reg_lvl = readl(priv->reg_base + OFFS_LVL);
	reg_edge = readl(priv->reg_base + OFFS_EDGE);

	switch (type) {
	case IRQ_TYPE_LEVEL_LOW:
		reg_lvl &= ~mask;
		reg_edge &= ~mask;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		reg_lvl |= mask;
		reg_edge &= ~mask;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		reg_lvl &= ~mask;
		reg_edge |= mask;
		break;
	case IRQ_TYPE_EDGE_RISING:
		reg_lvl |= mask;
		reg_edge |= mask;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		priv->toggle_mask |= mask;
		if (readl(priv->reg_base + OFFS_IN) & mask)
			reg_lvl &= ~mask;
		else
			reg_lvl |= mask;
		reg_edge |= mask;
		break;
	default:
		res = -EINVAL;
		goto unlock;
	}

	writel(reg_lvl, priv->reg_base + OFFS_LVL);
	writel(reg_edge, priv->reg_base + OFFS_EDGE);

unlock:
	spin_unlock_irqrestore(&priv->lock, flags);

	if (res == 0) {
		if (type & (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH))
			irq_set_handler_locked(d, handle_level_irq);
		else
			irq_set_handler_locked(d, handle_edge_irq);
	}

	return res;
}

static int cs75xx_gpio_irq_reqres(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	unsigned int hwirq = irqd_to_hwirq(d);

	cs75xx_gpio_dir_in(gc, hwirq);
	return gpiochip_reqres_irq(gc, hwirq);
}

static void cs75xx_gpio_irq_relres(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);

	gpiochip_relres_irq(gc, irqd_to_hwirq(d));
}

static const struct irq_chip cs75xx_gpio_irqchip = {
	.name			= "g2_gpio",
	.irq_ack		= cs75xx_gpio_irq_ack,
	.irq_mask		= cs75xx_gpio_irq_mask,
	.irq_unmask		= cs75xx_gpio_irq_unmask,
	.irq_set_type		= cs75xx_gpio_irq_set_type,
	.irq_request_resources	= cs75xx_gpio_irq_reqres,
	.irq_release_resources	= cs75xx_gpio_irq_relres,
	.flags			= IRQCHIP_MASK_ON_SUSPEND | IRQCHIP_IMMUTABLE,
};

static int cs75xx_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cs75xx_gpio *priv;
	struct gpio_chip *gc;
	struct gpio_irq_chip *girq;
	u32 regval;
	int irq, id, res;

	id = of_alias_get_id(dev->of_node, "gpio");
	if (id < 0) {
		dev_err(dev, "Couldn't get OF id\n");
		return id;
	}

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	platform_set_drvdata(pdev, priv);
	priv->id = id;

	priv->reg_base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->reg_base))
		return PTR_ERR(priv->reg_base);

	priv->mux_reg = devm_platform_ioremap_resource(pdev, 1);
	if (IS_ERR(priv->mux_reg))
		return PTR_ERR(priv->mux_reg);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;
	priv->irq = irq;

	if (of_property_read_u32(dev->of_node, "mux-initval", &regval) == 0) {
		writel(regval, priv->mux_reg);
		dev_info(dev, "initialize GPIO_MUX%d = 0x%08X\n", priv->id, regval);
	}

	spin_lock_init(&priv->lock);

	gc = &priv->gpio_chip;
	gc->label		= dev_name(dev);
	gc->owner		= THIS_MODULE;
	gc->parent		= dev;
	gc->request		= cs75xx_gpio_request;
	gc->free		= cs75xx_gpio_free;
	gc->set			= cs75xx_gpio_set;
	gc->get			= cs75xx_gpio_get;
	gc->get_direction	= cs75xx_gpio_get_direction;
	gc->direction_input	= cs75xx_gpio_dir_in;
	gc->direction_output	= cs75xx_gpio_dir_out;
	gc->base		= -1;
	gc->ngpio		= GPIO_PIN_NUM;

	writel(0, priv->reg_base + OFFS_IE);
	writel(0xFFFFFFFF, priv->reg_base + OFFS_INT);

	girq = &gc->irq;
	gpio_irq_chip_set_chip(girq, &cs75xx_gpio_irqchip);
	girq->parent_handler = cs75xx_gpio_irq_handler;
	girq->num_parents = 1;
	girq->parents = devm_kcalloc(dev, 1, sizeof(*girq->parents), GFP_KERNEL);
	if (!girq->parents)
		return -ENOMEM;
	girq->parents[0] = irq;
	girq->default_type = IRQ_TYPE_NONE;
	girq->handler = handle_level_irq;

	res = devm_gpiochip_add_data(dev, gc, priv);
	if (res) {
		dev_err(dev, "failed to add gpiochip (%d)\n", res);
		return res;
	}

	return 0;
}

static const struct of_device_id cs75xx_gpio_of_match[] = {
	{ .compatible = "cortina,cs75xx-gpio" },
	{},
};

static struct platform_driver cs75xx_gpio_driver = {
	.driver		= {
		.name		= DRV_NAME,
		.of_match_table = cs75xx_gpio_of_match,
	},
	.probe		= cs75xx_gpio_probe,
};

static int __init cs75xx_gpio_init(void)
{
	return platform_driver_register(&cs75xx_gpio_driver);
}
postcore_initcall(cs75xx_gpio_init);
