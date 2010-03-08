/*
 *  Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ4740 platform GPIO support
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
#include <linux/module.h>
#include <linux/init.h>

#include <linux/spinlock.h>
#include <linux/sysdev.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>

#include <linux/debugfs.h>
#include <linux/seq_file.h>

#define JZ_GPIO_BASE_A (32*0)
#define JZ_GPIO_BASE_B (32*1)
#define JZ_GPIO_BASE_C (32*2)
#define JZ_GPIO_BASE_D (32*3)

#define JZ_GPIO_NUM_A 32
#define JZ_GPIO_NUM_B 32
#define JZ_GPIO_NUM_C 31
#define JZ_GPIO_NUM_D 32

#define JZ_IRQ_GPIO_BASE_A (JZ_IRQ_GPIO(0) + JZ_GPIO_BASE_A)
#define JZ_IRQ_GPIO_BASE_B (JZ_IRQ_GPIO(0) + JZ_GPIO_BASE_B)
#define JZ_IRQ_GPIO_BASE_C (JZ_IRQ_GPIO(0) + JZ_GPIO_BASE_C)
#define JZ_IRQ_GPIO_BASE_D (JZ_IRQ_GPIO(0) + JZ_GPIO_BASE_D)

#define JZ_IRQ_GPIO_A(num) (JZ_IRQ_GPIO_BASE_A + num)
#define JZ_IRQ_GPIO_B(num) (JZ_IRQ_GPIO_BASE_B + num)
#define JZ_IRQ_GPIO_C(num) (JZ_IRQ_GPIO_BASE_C + num)
#define JZ_IRQ_GPIO_D(num) (JZ_IRQ_GPIO_BASE_D + num)

#define JZ_REG_GPIO_PIN			0x00
#define JZ_REG_GPIO_DATA		0x10
#define JZ_REG_GPIO_DATA_SET		0x14
#define JZ_REG_GPIO_DATA_CLEAR		0x18
#define JZ_REG_GPIO_MASK		0x20
#define JZ_REG_GPIO_MASK_SET		0x24
#define JZ_REG_GPIO_MASK_CLEAR		0x28
#define JZ_REG_GPIO_PULL		0x30
#define JZ_REG_GPIO_PULL_SET		0x34
#define JZ_REG_GPIO_PULL_CLEAR		0x38
#define JZ_REG_GPIO_FUNC		0x40
#define JZ_REG_GPIO_FUNC_SET		0x44
#define JZ_REG_GPIO_FUNC_CLEAR		0x48
#define JZ_REG_GPIO_SELECT		0x50
#define JZ_REG_GPIO_SELECT_SET		0x54
#define JZ_REG_GPIO_SELECT_CLEAR	0x58
#define JZ_REG_GPIO_DIRECTION		0x60
#define JZ_REG_GPIO_DIRECTION_SET	0x64
#define JZ_REG_GPIO_DIRECTION_CLEAR	0x68
#define JZ_REG_GPIO_TRIGGER		0x70
#define JZ_REG_GPIO_TRIGGER_SET		0x74
#define JZ_REG_GPIO_TRIGGER_CLEAR	0x78
#define JZ_REG_GPIO_FLAG		0x80
#define JZ_REG_GPIO_FLAG_CLEAR		0x14

#define CHIP_TO_REG(chip, reg) (jz_gpio_base + (((chip)->base) << 3) + reg)

#define GPIO_TO_BIT(gpio) BIT(gpio & 0x1f)
#define GPIO_TO_REG(gpio, reg) (jz_gpio_base + ((gpio >> 5) << 8) + reg)

static void __iomem *jz_gpio_base;

struct jz_gpio_chip {
	unsigned int irq;
	unsigned int irq_base;
	uint32_t wakeup;
	uint32_t suspend_mask;
	uint32_t edge_trigger_both;
	spinlock_t lock;
	struct gpio_chip gpio_chip;
	struct irq_chip irq_chip;
};

static struct jz_gpio_chip *jz_irq_to_chip(unsigned int irq)
{
	return get_irq_chip_data(irq);
}

static inline void jz_gpio_write_bit(unsigned int gpio, unsigned int reg)
{
	writel(GPIO_TO_BIT(gpio), GPIO_TO_REG(gpio, reg));
}

int jz_gpio_set_function(int gpio, enum jz_gpio_function function)
{
	if (function == JZ_GPIO_FUNC_NONE) {
		jz_gpio_write_bit(gpio, JZ_REG_GPIO_FUNC_CLEAR);
		jz_gpio_write_bit(gpio, JZ_REG_GPIO_SELECT_CLEAR);
		jz_gpio_write_bit(gpio, JZ_REG_GPIO_TRIGGER_CLEAR);
	} else {
		jz_gpio_write_bit(gpio, JZ_REG_GPIO_FUNC_SET);
		jz_gpio_write_bit(gpio, JZ_REG_GPIO_TRIGGER_CLEAR);
		switch (function) {
		case JZ_GPIO_FUNC1:
			jz_gpio_write_bit(gpio, JZ_REG_GPIO_SELECT_CLEAR);
			break;
		case JZ_GPIO_FUNC3:
			jz_gpio_write_bit(gpio, JZ_REG_GPIO_TRIGGER_SET);
		case JZ_GPIO_FUNC2: /* Falltrough */
			jz_gpio_write_bit(gpio, JZ_REG_GPIO_SELECT_SET);
			break;
		default:
			BUG();
			break;
		}
	}

	return 0;
}
EXPORT_SYMBOL_GPL(jz_gpio_set_function);

int jz_gpio_bulk_request(const struct jz_gpio_bulk_request *request, size_t num)
{
	size_t i;
	int ret;

	for (i = 0; i < num; ++i, ++request) {
		ret = gpio_request(request->gpio, request->name);
		if (ret)
			goto err;
		jz_gpio_set_function(request->gpio, request->function);
	}

	return 0;
err:
	for (--request; i > 0; --i, --request)
		gpio_free(request->gpio);

	return ret;
}
EXPORT_SYMBOL_GPL(jz_gpio_bulk_request);

void jz_gpio_bulk_free(const struct jz_gpio_bulk_request *request, size_t num)
{
	size_t i;

	for (i = 0; i < num; ++i, ++request) {
		gpio_free(request->gpio);
		jz_gpio_set_function(request->gpio, JZ_GPIO_FUNC_NONE);
	}

}
EXPORT_SYMBOL_GPL(jz_gpio_bulk_free);

void jz_gpio_bulk_suspend(const struct jz_gpio_bulk_request *request, size_t num)
{
	size_t i;

	for (i = 0; i < num; ++i, ++request) {
		jz_gpio_set_function(request->gpio, JZ_GPIO_FUNC_NONE);
		jz_gpio_write_bit(request->gpio, JZ_REG_GPIO_DIRECTION_CLEAR);
		jz_gpio_write_bit(request->gpio, JZ_REG_GPIO_PULL_SET);
	}
}
EXPORT_SYMBOL_GPL(jz_gpio_bulk_suspend);

void jz_gpio_bulk_resume(const struct jz_gpio_bulk_request *request, size_t num)
{
	size_t i;

	for (i = 0; i < num; ++i, ++request) {
		jz_gpio_set_function(request->gpio, request->function);
	}
}
EXPORT_SYMBOL_GPL(jz_gpio_bulk_resume);

void jz_gpio_enable_pullup(unsigned gpio)
{
	jz_gpio_write_bit(gpio, JZ_REG_GPIO_PULL_CLEAR);
}
EXPORT_SYMBOL_GPL(jz_gpio_enable_pullup);

void jz_gpio_disable_pullup(unsigned gpio)
{
	jz_gpio_write_bit(gpio, JZ_REG_GPIO_PULL_SET);
}
EXPORT_SYMBOL_GPL(jz_gpio_disable_pullup);

static int jz_gpio_get_value(struct gpio_chip *chip, unsigned gpio)
{
	return !!(readl(CHIP_TO_REG(chip, JZ_REG_GPIO_PIN)) & BIT(gpio));
}

static void jz_gpio_set_value(struct gpio_chip *chip, unsigned gpio, int value)
{
	uint32_t __iomem *reg = CHIP_TO_REG(chip, JZ_REG_GPIO_DATA_SET);
	reg += !value;
	writel(BIT(gpio), reg);
}

static int jz_gpio_direction_output(struct gpio_chip *chip, unsigned gpio, int value)
{
	writel(BIT(gpio), CHIP_TO_REG(chip, JZ_REG_GPIO_DIRECTION_SET));
	jz_gpio_set_value(chip, gpio, value);

	return 0;
}

static int jz_gpio_direction_input(struct gpio_chip *chip, unsigned gpio)
{
	writel(BIT(gpio), CHIP_TO_REG(chip, JZ_REG_GPIO_DIRECTION_CLEAR));

	return 0;
}

int jz_gpio_port_direction_input(int port, uint32_t mask)
{
	writel(mask, GPIO_TO_REG(port, JZ_REG_GPIO_DIRECTION_CLEAR));

	return 0;
}
EXPORT_SYMBOL(jz_gpio_port_direction_input);

int jz_gpio_port_direction_output(int port, uint32_t mask)
{
	writel(mask, GPIO_TO_REG(port, JZ_REG_GPIO_DIRECTION_SET));

	return 0;
}
EXPORT_SYMBOL(jz_gpio_port_direction_output);

void jz_gpio_port_set_value(int port, uint32_t value, uint32_t mask)
{
	writel((~value) & mask, GPIO_TO_REG(port, JZ_REG_GPIO_DATA_CLEAR));
	writel(value & mask, GPIO_TO_REG(port, JZ_REG_GPIO_DATA_SET));
}
EXPORT_SYMBOL(jz_gpio_port_set_value);

uint32_t jz_gpio_port_get_value(int port, uint32_t mask)
{
	uint32_t value = readl(GPIO_TO_REG(port, JZ_REG_GPIO_PIN));

	return value & mask;
}
EXPORT_SYMBOL(jz_gpio_port_get_value);


#define IRQ_TO_GPIO(irq) (irq - JZ_IRQ_GPIO(0))
#define IRQ_TO_BIT(irq) BIT(IRQ_TO_GPIO(irq) & 0x1f)

#define IRQ_TO_REG(irq, reg)  GPIO_TO_REG(IRQ_TO_GPIO(irq), reg)

static void jz_gpio_irq_demux_handler(unsigned int irq, struct irq_desc *desc)
{
	uint32_t flag;
	unsigned int gpio_irq;
	unsigned int gpio_bank;
	struct jz_gpio_chip *chip = get_irq_desc_data(desc);

	gpio_bank = JZ_IRQ_GPIO0 - irq;

	flag = readl(jz_gpio_base + (gpio_bank << 8) + JZ_REG_GPIO_FLAG);

	gpio_irq = ffs(flag) - 1;

	if (chip->edge_trigger_both & BIT(gpio_irq)) {
		uint32_t value = readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_PIN));
		if (value & BIT(gpio_irq)) {
			writel(BIT(gpio_irq),
				CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_DIRECTION_CLEAR));
		} else {
			writel(BIT(gpio_irq),
				CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_DIRECTION_SET));
		}
	}

	gpio_irq += (gpio_bank << 5) + JZ_IRQ_GPIO(0);

	generic_handle_irq(gpio_irq);
};

static inline void jz_gpio_set_irq_bit(unsigned int irq, unsigned int reg)
{
	writel(IRQ_TO_BIT(irq), IRQ_TO_REG(irq, reg));
}

static void jz_gpio_irq_mask(unsigned int irq)
{
	jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_MASK_SET);
};

static void jz_gpio_irq_unmask(unsigned int irq)
{
	jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_MASK_CLEAR);
};


/* TODO: Check if function is gpio */
static unsigned int jz_gpio_irq_startup(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);

	jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_SELECT_SET);

	jz_gpio_irq_unmask(irq);
	desc->status &= ~IRQ_MASKED;

	return 0;
}

static void jz_gpio_irq_shutdown(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);

	jz_gpio_irq_mask(irq);
	desc->status |= IRQ_MASKED;

	/* Set direction to input */
	jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_DIRECTION_CLEAR);
	jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_SELECT_CLEAR);
}

static void jz_gpio_irq_ack(unsigned int irq)
{
	jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_FLAG_CLEAR);
};

static int jz_gpio_irq_set_type(unsigned int irq, unsigned int flow_type)
{
	struct jz_gpio_chip *chip = jz_irq_to_chip(irq);
	struct irq_desc *desc = irq_to_desc(irq);

	jz_gpio_irq_mask(irq);

	if (flow_type == IRQ_TYPE_EDGE_BOTH) {
		uint32_t value = readl(IRQ_TO_REG(irq, JZ_REG_GPIO_PIN));
		if (value & IRQ_TO_BIT(irq))
			flow_type = IRQ_TYPE_EDGE_FALLING;
		else
			flow_type = IRQ_TYPE_EDGE_RISING;
		chip->edge_trigger_both |= IRQ_TO_BIT(irq);
	} else {
		chip->edge_trigger_both &= ~IRQ_TO_BIT(irq);
	}

	switch(flow_type) {
	case IRQ_TYPE_EDGE_RISING:
		jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_DIRECTION_SET);
		jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_TRIGGER_SET);
		break;
	case IRQ_TYPE_EDGE_FALLING:
		jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_DIRECTION_CLEAR);
		jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_TRIGGER_SET);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_DIRECTION_SET);
		jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_TRIGGER_CLEAR);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_DIRECTION_CLEAR);
		jz_gpio_set_irq_bit(irq, JZ_REG_GPIO_TRIGGER_CLEAR);
		break;
	default:
		return -EINVAL;
	}

	if (!(desc->status & IRQ_MASKED))
		jz_gpio_irq_unmask(irq);

	return 0;
}

static int jz_gpio_irq_set_wake(unsigned int irq, unsigned int on)
{
	struct jz_gpio_chip *chip = jz_irq_to_chip(irq);
	spin_lock(&chip->lock);
	if (on)
		chip->wakeup |= IRQ_TO_BIT(irq);
	else
		chip->wakeup &= ~IRQ_TO_BIT(irq);
	spin_unlock(&chip->lock);

	set_irq_wake(chip->irq, !!(chip->wakeup));
	return 0;
}

int gpio_to_irq(unsigned gpio)
{
	return JZ_IRQ_GPIO(0) + gpio;
}
EXPORT_SYMBOL_GPL(gpio_to_irq);

int irq_to_gpio(unsigned gpio)
{
	return IRQ_TO_GPIO(gpio);
}
EXPORT_SYMBOL_GPL(irq_to_gpio);

#define JZ_GPIO_CHIP(_bank) { \
	.irq_base = JZ_IRQ_GPIO_BASE_ ## _bank, \
	.gpio_chip = { \
		.label = "Bank " # _bank, \
		.owner = THIS_MODULE, \
		.set = jz_gpio_set_value, \
		.get = jz_gpio_get_value, \
		.direction_output = jz_gpio_direction_output, \
		.direction_input = jz_gpio_direction_input, \
		.base = JZ_GPIO_BASE_ ## _bank, \
		.ngpio = JZ_GPIO_NUM_ ## _bank, \
	}, \
	.irq_chip =  { \
		.name = "GPIO Bank " # _bank, \
		.mask = jz_gpio_irq_mask, \
		.unmask = jz_gpio_irq_unmask, \
		.ack = jz_gpio_irq_ack, \
		.startup = jz_gpio_irq_startup, \
		.shutdown = jz_gpio_irq_shutdown, \
		.set_type = jz_gpio_irq_set_type, \
		.set_wake = jz_gpio_irq_set_wake, \
	}, \
}

static struct jz_gpio_chip jz_gpio_chips[] = {
	JZ_GPIO_CHIP(A),
	JZ_GPIO_CHIP(B),
	JZ_GPIO_CHIP(C),
	JZ_GPIO_CHIP(D),
};

int jz_gpio_suspend(void)
{
	struct jz_gpio_chip *chip = jz_gpio_chips;
	int i, gpio;

	for (i = 0; i < ARRAY_SIZE(jz_gpio_chips); ++i, ++chip) {
		gpio = chip->gpio_chip.base;
		chip->suspend_mask = readl(GPIO_TO_REG(gpio, JZ_REG_GPIO_MASK));
		writel(~(chip->wakeup), GPIO_TO_REG(gpio, JZ_REG_GPIO_MASK_SET));
	}

	chip = jz_gpio_chips;

	return 0;
}

int jz_gpio_resume(void)
{
	struct jz_gpio_chip *chip = jz_gpio_chips;
	int i;

	for (i = 0; i < ARRAY_SIZE(jz_gpio_chips); ++i, ++chip) {
		writel(~(chip->suspend_mask), GPIO_TO_REG(chip->gpio_chip.base, JZ_REG_GPIO_MASK_CLEAR));
	}

	return 0;
}

int __init jz_gpiolib_init(void)
{
	struct jz_gpio_chip *chip = jz_gpio_chips;
	int i, irq;

	jz_gpio_base = ioremap(0x10010000, 0x400);

	for (i = 0; i < ARRAY_SIZE(jz_gpio_chips); ++i, ++chip) {
		gpiochip_add(&chip->gpio_chip);
		spin_lock_init(&chip->lock);
		chip->irq = JZ_IRQ_INTC_GPIO(i);
		set_irq_data(chip->irq, chip);
		set_irq_chained_handler(chip->irq, jz_gpio_irq_demux_handler);
		for (irq = chip->irq_base; irq < chip->irq_base + chip->gpio_chip.ngpio; ++irq) {
			set_irq_chip_and_handler(irq, &chip->irq_chip, handle_level_irq);
			set_irq_chip_data(irq, chip);
		}
	}

	printk("JZ GPIO initalized\n");

	return 0;
}

#ifdef CONFIG_DEBUG_FS

static int gpio_regs_show(struct seq_file *s, void *unused)
{
	struct jz_gpio_chip *chip = jz_gpio_chips;
	int i;

	for (i = 0; i < ARRAY_SIZE(jz_gpio_chips); ++i, ++chip) {
		seq_printf(s, "GPIO %d: \n", i);
		seq_printf(s, "\tPin: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_PIN)));
		seq_printf(s, "\tData: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_DATA)));
		seq_printf(s, "\tMask: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_MASK)));
		seq_printf(s, "\tData: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_DATA)));
		seq_printf(s, "\tPull: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_PULL)));
		seq_printf(s, "\tFunc: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_FUNC)));
		seq_printf(s, "\tSelect: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_SELECT)));
		seq_printf(s, "\tDirection: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_DIRECTION)));
		seq_printf(s, "\tTrigger: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_TRIGGER)));
		seq_printf(s, "\tFlag: %.8x\n", readl(CHIP_TO_REG(&chip->gpio_chip, JZ_REG_GPIO_FLAG)));
	}

	return 0;
}

static int gpio_regs_open(struct inode *inode, struct file *file)
{
	return single_open(file, gpio_regs_show, NULL);
}

static const struct file_operations gpio_regs_operations = {
	.open		= gpio_regs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init gpio_debugfs_init(void)
{
	(void) debugfs_create_file("jz_regs_gpio", S_IFREG | S_IRUGO,
				NULL, NULL, &gpio_regs_operations);
	return 0;
}
subsys_initcall(gpio_debugfs_init);

#endif
