// SPDX-License-Identifier: GPL-2.0-only
/*
 * Hasivo STC8 MFD driver with configurable write ORing for execute bit
 * I2C implementation
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/mfd/syscon.h>

struct stc8_mfd {
	struct device *dev;
	struct regmap *parent_regmap;
	struct regmap *child_regmap;
	u32 exec_bit;
	u32 *exec_regs;
	size_t num_exec_regs;
};

/* Check if register requires execute bit */
static bool is_exec_reg(struct stc8_mfd *mfd, unsigned int reg)
{
	for (size_t i = 0; i < mfd->num_exec_regs; i++) {
		if (mfd->exec_regs[i] == reg)
			return true;
	}
	return false;
}

/* Custom regmap write wrapper */
static int stc8_child_reg_write(void *context, unsigned int reg, unsigned int val)
{
	struct stc8_mfd *mfd = context;
	const unsigned int orig_val = val;

	/* Apply execute bit if this register is in the list */
	if (is_exec_reg(mfd, reg)) {
		val |= mfd->exec_bit;
        dev_dbg(mfd->dev, "Applying exec bit to reg 0x%02x, orig_val=0x%02x, new_val=0x%02x\n", reg, orig_val, val);
    }

	/* Forward to parent regmap (I2C bus) */
	return regmap_write(mfd->parent_regmap, reg, val);
}

/* Custom regmap read - transparent passthrough */
static int stc8_child_reg_read(void *context, unsigned int reg, unsigned int *val)
{
	struct stc8_mfd *mfd = context;

	return regmap_read(mfd->parent_regmap, reg, val);
}

static const struct regmap_config stc8_parent_regmap_config = {
	.name = "stc8-mfd-parent",
	.reg_bits = 8,
	.val_bits = 8,
};

static const struct regmap_config stc8_child_regmap_config = {
	.name = "stc8-mfd-child",
	.reg_bits = 8,
	.val_bits = 8,
	.reg_read = stc8_child_reg_read,
	.reg_write = stc8_child_reg_write,
};

static int stc8_parse_dt(struct stc8_mfd *mfd, struct device_node *np)
{
	int ret, count;

	/* Get execute bit value (default 0x40) */
    mfd->exec_bit = 0x40;
	ret = of_property_read_u32(np, "hasivo,execute-bit", &mfd->exec_bit);

	/* Get count of execute registers */
	count = of_property_count_u32_elems(np, "hasivo,execute-bit-registers");
	if (count <= 0) {
		mfd->num_exec_regs = 0;
	}
    else {
        mfd->num_exec_regs = count;
        mfd->exec_regs = devm_kcalloc(mfd->dev, count, sizeof(u32),
                        GFP_KERNEL);
        if (!mfd->exec_regs)
            return -ENOMEM;

        ret = of_property_read_u32_array(np, "hasivo,execute-bit-registers",
                        mfd->exec_regs, count);
        if (ret) {
            dev_err(mfd->dev, "Failed to read execute-bit-registers: %d\n", ret);
            return ret;
        }
    }

	dev_info(mfd->dev, "execute-bit=0x%02x, %zu execute-bit-registers\n",
		 mfd->exec_bit, mfd->num_exec_regs);

	return 0;
}

static int stc8_i2c_probe(struct i2c_client *client)
{
	struct stc8_mfd *mfd;
	int ret;

	dev_dbg(&client->dev, "Hasivo STC8 MFD driver probed started\n");

	mfd = devm_kzalloc(&client->dev, sizeof(struct stc8_mfd), GFP_KERNEL);
	if (!mfd)
		return -ENOMEM;

	mfd->dev = &client->dev;
	i2c_set_clientdata(client, mfd);

	/* Parse device tree properties */
	ret = stc8_parse_dt(mfd, mfd->dev->of_node);
	if (ret)
		return ret;

	/* Create parent regmap for direct I2C access */
	mfd->parent_regmap = devm_regmap_init_i2c(client,
						   &stc8_parent_regmap_config);
	if (IS_ERR(mfd->parent_regmap)) {
		dev_err(&client->dev, "Failed to init parent regmap\n");
		return PTR_ERR(mfd->parent_regmap);
	}

	/* Create child regmap with custom read/write for masking */
	mfd->child_regmap = devm_regmap_init(&client->dev, NULL, mfd,
					      &stc8_child_regmap_config);
	if (IS_ERR(mfd->child_regmap)) {
		dev_err(&client->dev, "Failed to init child regmap\n");
		return PTR_ERR(mfd->child_regmap);
	}
	/* Set the child regmap as the syscon regmap */
	ret = of_syscon_register_regmap(mfd->dev->of_node, mfd->child_regmap);
	if (ret)
		return ret;

	/* Automatically populate child devices from device tree */
	ret = of_platform_populate(client->dev.of_node, NULL, NULL,
				    &client->dev);
	if (ret) {
		dev_err(&client->dev, "Failed to add child devices: %d\n", ret);
		return ret;
	}

	dev_dbg(&client->dev, "Hasivo STC8 MFD driver probed successfully\n");

	return 0;
}

static void stc8_i2c_remove(struct i2c_client *client)
{
	of_platform_depopulate(&client->dev);
	dev_dbg(&client->dev, "Hasivo STC8 MFD driver removed\n");
}

static const struct of_device_id stc8_of_match[] = {
	{ .compatible = "hasivo,stc8-mfd" },
	{ }
};
MODULE_DEVICE_TABLE(of, stc8_of_match);

static struct i2c_driver stc8_i2c_driver = {
	.driver = {
		.name = "hasivo-stc8-mfd",
		.of_match_table = stc8_of_match,
	},
	.probe = stc8_i2c_probe,
	.remove = stc8_i2c_remove,
};
module_i2c_driver(stc8_i2c_driver);

MODULE_AUTHOR("Bevan Weiss <bevan.weiss@gmail.com>");
MODULE_DESCRIPTION("Hasivo STC8 MFD driver with configurable write masking");
MODULE_LICENSE("GPL");

