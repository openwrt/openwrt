// SPDX-License-Identifier: GPL-2.0
/*
 * Cortina CS75xx (Goldengate) true random number generator
 *
 * The block is an Elliptic TRNG: control at 0x00, interrupt status at 0x04,
 * interrupt enable at 0x08 and four data words from 0x10. A new value is
 * requested with GEN_NEW and is ready when the DONE status bit is set.
 * The vendor driver also sets bit 5 of the GLOBAL_SCRATCH register before
 * using the block, so does this one.
 */

#include <linux/bits.h>
#include <linux/hw_random.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#define TRNG_CTRL		0x00
#define TRNG_CTRL_GEN_NEW	BIT(0)
#define TRNG_CTRL_RESEED	BIT(31)
#define TRNG_IRQ_STAT		0x04
#define TRNG_IRQ_EN		0x08
#define TRNG_IRQ_DONE		BIT(27)
#define TRNG_DATA		0x10
#define TRNG_WORDS		4

#define GLOBAL_SCRATCH_TRNG	BIT(5)

#define TRNG_TIMEOUT_US		10000

struct cs75xx_trng {
	struct hwrng rng;
	void __iomem *base;
	void __iomem *scratch;
};

static int cs75xx_trng_wait(struct cs75xx_trng *t)
{
	u32 val;
	int ret;

	ret = readl_poll_timeout(t->base + TRNG_IRQ_STAT, val,
				 val & TRNG_IRQ_DONE, 1, TRNG_TIMEOUT_US);
	writel(TRNG_IRQ_DONE, t->base + TRNG_IRQ_STAT);
	return ret;
}

static int cs75xx_trng_init(struct hwrng *rng)
{
	struct cs75xx_trng *t = container_of(rng, struct cs75xx_trng, rng);

	writel(readl(t->scratch) | GLOBAL_SCRATCH_TRNG, t->scratch);
	writel(0, t->base + TRNG_IRQ_EN);
	writel(TRNG_IRQ_DONE, t->base + TRNG_IRQ_STAT);
	writel(TRNG_CTRL_RESEED, t->base + TRNG_CTRL);
	return cs75xx_trng_wait(t);
}

static int cs75xx_trng_read(struct hwrng *rng, void *buf, size_t max, bool wait)
{
	struct cs75xx_trng *t = container_of(rng, struct cs75xx_trng, rng);
	u32 data[TRNG_WORDS];
	size_t done = 0;
	int i, ret;

	while (done < max) {
		size_t n = min(max - done, sizeof(data));

		writel(TRNG_CTRL_GEN_NEW, t->base + TRNG_CTRL);
		ret = cs75xx_trng_wait(t);
		if (ret)
			return done ? done : ret;

		for (i = 0; i < TRNG_WORDS; i++)
			data[i] = readl(t->base + TRNG_DATA + 4 * i);
		memcpy(buf + done, data, n);
		done += n;

		if (!wait)
			break;
	}

	memzero_explicit(data, sizeof(data));
	return done;
}

static int cs75xx_trng_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct resource *res;
	struct cs75xx_trng *t;

	t = devm_kzalloc(dev, sizeof(*t), GFP_KERNEL);
	if (!t)
		return -ENOMEM;

	t->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(t->base))
		return PTR_ERR(t->base);

	/* GLOBAL_SCRATCH is shared with other users of the global block */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!res)
		return -EINVAL;
	t->scratch = devm_ioremap(dev, res->start, resource_size(res));
	if (!t->scratch)
		return -ENOMEM;

	t->rng.name = dev_name(dev);
	t->rng.init = cs75xx_trng_init;
	t->rng.read = cs75xx_trng_read;
	t->rng.quality = 512;

	return devm_hwrng_register(dev, &t->rng);
}

static const struct of_device_id cs75xx_trng_of_match[] = {
	{ .compatible = "cortina,cs75xx-trng" },
	{ }
};
MODULE_DEVICE_TABLE(of, cs75xx_trng_of_match);

static struct platform_driver cs75xx_trng_driver = {
	.probe = cs75xx_trng_probe,
	.driver = {
		.name = "cs75xx-trng",
		.of_match_table = cs75xx_trng_of_match,
	},
};
module_platform_driver(cs75xx_trng_driver);

MODULE_DESCRIPTION("Cortina CS75xx TRNG driver");
MODULE_LICENSE("GPL");
