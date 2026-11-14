// SPDX-License-Identifier: GPL-2.0-only
/* MediaTek MT7620 SoC identification. */

#define pr_fmt(fmt) "mt7620-socinfo: " fmt

#include <linux/bitfield.h>
#include <linux/init.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/printk.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>

#define MT7620_CHIP_REV		0x0c
#define MT7620_CHIP_REV_PKG	BIT(16)
#define MT7620_CHIP_REV_VER	GENMASK(11, 8)
#define MT7620_CHIP_REV_ECO	GENMASK(3, 0)

static int __init mt7620_socinfo_init(void)
{
	struct soc_device_attribute *attr;
	struct soc_device *soc_dev;
	struct device_node *np;
	struct regmap *sysc;
	u32 rev;
	int ret;

	np = of_find_compatible_node(NULL, NULL, "ralink,mt7620-sysc");
	if (!np)
		return 0;

	/* The clock/reset driver owns the syscon platform device. */
	sysc = syscon_node_to_regmap(np);
	of_node_put(np);
	if (IS_ERR(sysc)) {
		ret = PTR_ERR(sysc);
		goto err;
	}

	ret = regmap_read(sysc, MT7620_CHIP_REV, &rev);
	if (ret)
		goto err;

	attr = kzalloc(sizeof(*attr), GFP_KERNEL);
	if (!attr) {
		ret = -ENOMEM;
		goto err;
	}

	attr->family = "Ralink";
	attr->soc_id = rev & MT7620_CHIP_REV_PKG ? "mt7620a" : "mt7620n";
	/* Decimal silicon version and ECO, as in the early boot banner. */
	attr->revision = kasprintf(GFP_KERNEL, "%lu.%lu",
				   FIELD_GET(MT7620_CHIP_REV_VER, rev),
				   FIELD_GET(MT7620_CHIP_REV_ECO, rev));
	if (!attr->revision) {
		ret = -ENOMEM;
		goto free_attr;
	}

	soc_dev = soc_device_register(attr);
	if (IS_ERR(soc_dev)) {
		ret = PTR_ERR(soc_dev);
		kfree(attr->revision);
		goto free_attr;
	}

	return 0;

free_attr:
	kfree(attr);
err:
	pr_err("failed to register SoC information: %d\n", ret);
	return ret;
}

/* Supply identity before device_initcall consumers, including built-in PHYs. */
subsys_initcall(mt7620_socinfo_init);
