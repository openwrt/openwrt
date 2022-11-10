// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Realtek System-on-Chip info
 *
 * Copyright (c) 2017-2019 Andreas Färber
 * Copyright (c) 2019 Realtek Semiconductor Corp.
 */

#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#define REG_CHIP_ID	0x0
#define REG_CHIP_REV	0x4

#define REG_CHIP_INFO	0x28

struct soc_device {
	struct device dev;
	struct soc_device_attribute *attr;
	int soc_dev_num;
};

struct rtd_soc_revision {
	const char *name;
	u32 chip_rev;
};

static const struct rtd_soc_revision rtd1295_revisions[] = {
	{ "A00", 0x00000000 },
	{ "A01", 0x00010000 },
	{ "B00", 0x00020000 },
	{ "B01", 0x00030000 },
	{ }
};

static const struct rtd_soc_revision rtd1319_revisions[] = {
	{ "A00", 0x00000000},
	{ "B00", 0x00010000},
	{ "B01", 0x00020000},
	{ "B02", 0x00030000},
	{ }
};

static const struct rtd_soc_revision rtd161xb_revisions[] = {
	{ "A00", 0x00000000},
	{ }
};

struct rtd_soc {
	u32 chip_id;
	const char *(*get_name)(struct device *dev, const struct rtd_soc *s);
	const char *(*get_chip_type)(struct device *dev, const struct rtd_soc *s);
	const struct rtd_soc_revision *revisions;
	const char *codename;
};

static const char *rtd129x_name(struct device *dev, const struct rtd_soc *s)
{
	struct regmap *regmap;
	u32 chipinfo1;
	int ret;

	regmap = syscon_regmap_lookup_by_phandle(dev->of_node, "iso-syscon");
	if (IS_ERR(regmap)) {
		ret = PTR_ERR(regmap);
		if (ret == -EPROBE_DEFER)
			return ERR_PTR(ret);
		dev_warn(dev, "Could not check iso (%d)\n", ret);
	} else {
		ret = regmap_read(regmap, REG_CHIP_INFO, &chipinfo1);
		if (ret)
			dev_warn(dev, "Could not read chip_info1 (%d)\n", ret);
		else if (chipinfo1 & BIT(11)) {
			if (chipinfo1 & BIT(4))
				return "RTD1293";
			return "RTD1296";
		}
	}

	return "RTD1295";
}

static const char *rtd131x_name(struct device *dev, const struct rtd_soc *s)
{
	return "RTD1319";
}

static const char *rtd161xb_name(struct device *dev, const struct rtd_soc *s)
{
	return "RTD1619B";
}

static const struct rtd_soc rtd_soc_families[] = {
	{ 0x00006421, rtd129x_name,  NULL, rtd1295_revisions, "Kylin" },
	{ 0x00006570, rtd131x_name,  NULL, rtd1319_revisions, "Hank" },
	{ 0x00006698, rtd161xb_name, NULL, rtd161xb_revisions,"Stark" },
};

static const struct rtd_soc *rtd_soc_by_chip_id(u32 chip_id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(rtd_soc_families); i++) {
		const struct rtd_soc *family = &rtd_soc_families[i];

		if (family->chip_id == chip_id)
			return family;
	}
	return NULL;
}

static const char *rtd_soc_rev(const struct rtd_soc *family, u32 chip_rev)
{
	if (family) {
		const struct rtd_soc_revision *rev = family->revisions;

		while (rev && rev->name) {
			if (rev->chip_rev == chip_rev) {
				return rev->name;
			}
			rev++;
		}
	}
	return "unknown";
}

struct custom_device_attribute {
	const char *chip_type;
};

static ssize_t custom_info_get(struct device *dev,
			    struct device_attribute *attr,
			    char *buf);

static DEVICE_ATTR(chip_type,  S_IRUGO, custom_info_get,  NULL);

static umode_t custom_attribute_mode(struct kobject *kobj,
				struct attribute *attr,
				int index)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct soc_device *soc_dev = container_of(dev, struct soc_device, dev);

	if ((attr == &dev_attr_chip_type.attr)
	    && (soc_dev->attr->data)) {
		struct custom_device_attribute *custom_dev_attr;

		custom_dev_attr = (struct custom_device_attribute *)
			(soc_dev->attr->data);
		if (custom_dev_attr->chip_type)
			return attr->mode;
	}
	/* Unknown or unfilled attribute. */
	return 0;
}


static ssize_t custom_info_get(struct device *dev,
			    struct device_attribute *attr,
			    char *buf)
{
	struct soc_device *soc_dev = container_of(dev, struct soc_device, dev);

	if (attr == &dev_attr_chip_type &&
		soc_dev->attr->data) {
		struct custom_device_attribute *custom_dev_attr;

		custom_dev_attr = (struct custom_device_attribute *)
			(soc_dev->attr->data);
		if (custom_dev_attr->chip_type)
			return sprintf(buf, "%s\n", custom_dev_attr->chip_type);
	}

	return -EINVAL;
}

static struct attribute *custom_attr[] = {
	&dev_attr_chip_type.attr,
	NULL,
};

static const struct attribute_group custom_attr_group = {
	.attrs = custom_attr,
	.is_visible = custom_attribute_mode,
};

static int rtd_soc_probe(struct platform_device *pdev)
{
	const struct rtd_soc *s;
	struct soc_device_attribute *soc_dev_attr;
	struct soc_device *soc_dev;
	struct device_node *node;
	struct custom_device_attribute *custom_dev_attr;
	void __iomem *base;
	u32 chip_id, chip_rev;

	base = of_iomap(pdev->dev.of_node, 0);
	if (!base)
		return -ENODEV;

	soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
	if (!soc_dev_attr)
		return -ENOMEM;

	chip_id  = readl_relaxed(base + REG_CHIP_ID);
	chip_rev = readl_relaxed(base + REG_CHIP_REV);

	node = of_find_node_by_path("/");
	of_property_read_string(node, "model", &soc_dev_attr->machine);
	of_node_put(node);

	s = rtd_soc_by_chip_id(chip_id);

	soc_dev_attr->family = kasprintf(GFP_KERNEL, "Realtek %s",
		(s && s->codename) ? s->codename : "Digital Home Center");

	if (likely(s && s->get_name))
		soc_dev_attr->soc_id = s->get_name(&pdev->dev, s);
	else
		soc_dev_attr->soc_id = "unknown";

	soc_dev_attr->revision = rtd_soc_rev(s, chip_rev);

	custom_dev_attr = kzalloc(sizeof(*custom_dev_attr), GFP_KERNEL);
	if (!custom_dev_attr)
		return -ENOMEM;

	if (likely(s && s->get_chip_type))
		custom_dev_attr->chip_type = s->get_chip_type(&pdev->dev, s);
	else
		custom_dev_attr->chip_type = NULL;

	soc_dev_attr->data = (void *)custom_dev_attr;

	soc_dev_attr->custom_attr_group = &custom_attr_group;

	soc_dev = soc_device_register(soc_dev_attr);
	if (IS_ERR(soc_dev)) {
		kfree(soc_dev_attr->family);
		kfree(soc_dev_attr);
		return PTR_ERR(soc_dev);
	}

	platform_set_drvdata(pdev, soc_dev);

	pr_info("%s %s (0x%08x) rev %s (0x%08x) detected\n",
		soc_dev_attr->family, soc_dev_attr->soc_id, chip_id,
		soc_dev_attr->revision, chip_rev);

	return 0;
}

static int rtd_soc_remove(struct platform_device *pdev)
{
	struct soc_device *soc_dev = platform_get_drvdata(pdev);

	soc_device_unregister(soc_dev);

	return 0;
}

static const struct of_device_id rtd_soc_dt_ids[] = {
	 { .compatible = "realtek,soc-chip" },
	 { }
};

static struct platform_driver rtd_soc_driver = {
	.probe = rtd_soc_probe,
	.remove = rtd_soc_remove,
	.driver = {
		.name = "rtk-soc",
		.of_match_table	= rtd_soc_dt_ids,
	},
};

static int __init rtd_soc_driver_init(void)
{
	return platform_driver_register(&(rtd_soc_driver));
}
subsys_initcall_sync(rtd_soc_driver_init);

static void __exit rtd_soc_driver_exit(void)
{
	platform_driver_unregister(&(rtd_soc_driver));
}

MODULE_DESCRIPTION("Realtek SoC identification");
MODULE_LICENSE("GPL");
