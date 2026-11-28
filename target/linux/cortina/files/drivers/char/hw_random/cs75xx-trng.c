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

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/hw_random.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/string.h>
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

#define GLOBAL_JTAG_ID		0x00
#define GLOBAL_JTAG_ID_SOC	GENMASK(15, 12)
#define GLOBAL_JTAG_ID_CS7542_A1	8

#define GLOBAL_SCRATCH		0xc0
#define GLOBAL_SCRATCH_TRNG	BIT(5)

#define TRNG_TIMEOUT_US		10000

struct cs75xx_trng {
	struct hwrng rng;
	void __iomem *base;
	struct regmap *global;
	bool pending;
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
	int ret;

	ret = regmap_update_bits(t->global, GLOBAL_SCRATCH,
				 GLOBAL_SCRATCH_TRNG, GLOBAL_SCRATCH_TRNG);
	if (ret)
		return ret;
	writel(0, t->base + TRNG_IRQ_EN);
	writel(TRNG_IRQ_DONE, t->base + TRNG_IRQ_STAT);
	t->pending = false;
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

		if (!t->pending) {
			writel(TRNG_IRQ_DONE, t->base + TRNG_IRQ_STAT);
			writel(TRNG_CTRL_GEN_NEW, t->base + TRNG_CTRL);
			t->pending = true;
		}

		if (wait) {
			ret = cs75xx_trng_wait(t);
		} else {
			if (!(readl(t->base + TRNG_IRQ_STAT) & TRNG_IRQ_DONE))
				break;
			writel(TRNG_IRQ_DONE, t->base + TRNG_IRQ_STAT);
			ret = 0;
		}
		t->pending = false;
		if (ret) {
			memzero_explicit(data, sizeof(data));
			return done ? done : ret;
		}

		for (i = 0; i < TRNG_WORDS; i++)
			data[i] = readl(t->base + TRNG_DATA + 4 * i);
		memcpy((u8 *)buf + done, data, n);
		done += n;

		if (!wait)
			break;
	}

	memzero_explicit(data, sizeof(data));
	return done;
}

/* A0 ring reseeding is affected by vendor erratum 28891. Do not replace
 * the entropy source with the vendor's RTC/timer-derived nonce workaround.
 */
static int cs75xx_trng_check_revision(struct device *dev, struct regmap *global)
{
	u32 id;
	int ret;

	ret = regmap_read(global, GLOBAL_JTAG_ID, &id);
	if (ret)
		return dev_err_probe(dev, ret, "cannot read SoC revision\n");

	if (FIELD_GET(GLOBAL_JTAG_ID_SOC, id) != GLOBAL_JTAG_ID_CS7542_A1)
		return dev_err_probe(dev, -ENODEV,
				     "hardware reseeding requires CS7542 A1\n");

	return 0;
}

static int cs75xx_trng_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cs75xx_trng *t;
	int ret;

	t = devm_kzalloc(dev, sizeof(*t), GFP_KERNEL);
	if (!t)
		return -ENOMEM;

	t->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(t->base))
		return PTR_ERR(t->base);

	t->global = syscon_regmap_lookup_by_phandle(dev->of_node,
						    "cortina,syscon");
	if (IS_ERR(t->global))
		return dev_err_probe(dev, PTR_ERR(t->global),
				     "cannot get global control regmap\n");

	ret = cs75xx_trng_check_revision(dev, t->global);
	if (ret)
		return ret;

	t->rng.name = dev_name(dev);
	t->rng.init = cs75xx_trng_init;
	t->rng.read = cs75xx_trng_read;
	t->rng.quality = 512;

	return devm_hwrng_register(dev, &t->rng);
}

static const struct of_device_id cs75xx_trng_of_match[] = {
	{ .compatible = "cortina,cs7542-trng" },
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
