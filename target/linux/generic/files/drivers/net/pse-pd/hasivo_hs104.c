// SPDX-License-Identifier: GPL-2.0-only
/*
 * Hasivo HS104 PoE PSE controller driver
 *
 * The HS104PTI/HS104PBI are single-chip PoE PSE controllers managing 4
 * delivery channels, allowing them to supply 4 ports of 802.3af/at/bt power.
 * The HS104PTI can have 1x 802.3bt port and 3x 802.3at ports.
 * The HS104PBI can have 4x 802.3bt ports.
 *
 * Copyright (c) 2025 Bevan Weiss <bevan.weiss@gmail.com>
 * Copyright (c) 2026 Carlo Szelinsky <github@szelinsky.de>
 */

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pse-pd/pse.h>
#include <linux/regmap.h>

#define HS104_MAX_PORTS		4

/* Register map */
#define HS104_REG_PW_STATUS	0x01	/* Power delivery status */
#define HS104_REG_INPUT_V	0x02	/* Input voltage, 16-bit BE, 10mV */
#define HS104_REG_PORT0_I	0x04	/* Port current, 16-bit BE, 1mA (not yet implemented) */
#define HS104_REG_DEVID		0x0C	/* Device ID */
#define HS104_REG_PORT0_CLASS	0x0D	/* Port power class */
#define HS104_REG_PW_EN		0x14	/* Port enable control */
#define HS104_REG_PROTOCOL	0x19	/* Protocol per port (2 bits each) */
#define HS104_REG_TOTAL_POWER	0x1D	/* Total power, 16-bit BE, 10mW (not yet implemented) */
#define HS104_REG_PORT0_POWER	0x21	/* Port power, 16-bit BE, 10mW */

#define HS104_DEVICE_ID		0x91
#define HS104_EXECUTE		0x40	/* Execute bit for writes */

/*
 * BIT-field registers (PW_EN, PW_STATUS) use reversed bit ordering
 * relative to sequential PORT registers (PORT0_POWER, PORT0_CLASS).
 * Map PSE PI index to the hardware bit position.
 */
#define HS104_PORT_BIT(id)	BIT(HS104_MAX_PORTS - 1 - (id))

/* Protocol encoding (2 bits per port in PROTOCOL register) */
#define HS104_PROTO_MASK	0x3
#define HS104_PROTO_BT		0	/* 802.3bt - 60W */
#define HS104_PROTO_HIPO	1	/* Hi-PoE - 90W */
#define HS104_PROTO_AT		2	/* 802.3at - 30W */
#define HS104_PROTO_AF		3	/* 802.3af - 15.4W */

/* Power limits in mW */
#define HS104_PW_AF		15400
#define HS104_PW_AT		30000
#define HS104_PW_BT		60000
#define HS104_PW_HIPO		90000

/* Unit conversion steps */
#define HS104_UV_STEP		10000	/* 10mV -> uV */
#define HS104_UA_STEP		1000	/* 1mA -> uA */
#define HS104_MW_STEP		10	/* 10mW -> mW */

struct hs104_priv {
	struct regmap		*regmap;
	struct pse_controller_dev pcdev;
};

static inline struct hs104_priv *to_hs104(struct pse_controller_dev *pcdev)
{
	return container_of(pcdev, struct hs104_priv, pcdev);
}

/* Read 16-bit big-endian register and apply unit conversion */
static int hs104_read_be16(struct hs104_priv *priv, unsigned int reg,
			   unsigned int step)
{
	__be16 val;
	int ret;

	ret = regmap_bulk_read(priv->regmap, reg, &val, sizeof(val));
	if (ret)
		return ret;

	/* Hardware uses 14-bit data field, upper 2 bits are reserved */
	return (be16_to_cpu(val) & 0x3fff) * step;
}

/* Convert protocol code to power limit in mW */
static int hs104_proto_to_mw(unsigned int proto)
{
	switch (proto) {
	case HS104_PROTO_AF:	return HS104_PW_AF;
	case HS104_PROTO_AT:	return HS104_PW_AT;
	case HS104_PROTO_BT:	return HS104_PW_BT;
	case HS104_PROTO_HIPO:	return HS104_PW_HIPO;
	default:		return 0;
	}
}

/* PSE controller operations */

static int hs104_pi_enable(struct pse_controller_dev *pcdev, int id)
{
	struct hs104_priv *priv = to_hs104(pcdev);
	unsigned int val;
	int ret;

	ret = regmap_read(priv->regmap, HS104_REG_PW_EN, &val);
	if (ret)
		return ret;

	val = (val | HS104_PORT_BIT(id)) | HS104_EXECUTE;

	dev_dbg(pcdev->dev, "PI %d: enable (0x%02x)\n", id, val);
	return regmap_write(priv->regmap, HS104_REG_PW_EN, val);
}

static int hs104_pi_disable(struct pse_controller_dev *pcdev, int id)
{
	struct hs104_priv *priv = to_hs104(pcdev);
	unsigned int val;
	int ret;

	ret = regmap_read(priv->regmap, HS104_REG_PW_EN, &val);
	if (ret)
		return ret;

	val = (val & ~HS104_PORT_BIT(id)) | HS104_EXECUTE;

	dev_dbg(pcdev->dev, "PI %d: disable (0x%02x)\n", id, val);
	return regmap_write(priv->regmap, HS104_REG_PW_EN, val);
}

static int hs104_pi_get_voltage(struct pse_controller_dev *pcdev, int id)
{
	struct hs104_priv *priv = to_hs104(pcdev);

	/* Input voltage is shared across all PIs */
	return hs104_read_be16(priv, HS104_REG_INPUT_V, HS104_UV_STEP);
}

static int hs104_pi_get_pw_limit(struct pse_controller_dev *pcdev, int id)
{
	struct hs104_priv *priv = to_hs104(pcdev);
	unsigned int val, proto;
	int ret;

	ret = regmap_read(priv->regmap, HS104_REG_PROTOCOL, &val);
	if (ret)
		return ret;

	proto = (val >> (id * 2)) & HS104_PROTO_MASK;
	return hs104_proto_to_mw(proto) ?: -ENODATA;
}

static int hs104_pi_set_pw_limit(struct pse_controller_dev *pcdev,
				 int id, int max_mw)
{
	struct hs104_priv *priv = to_hs104(pcdev);
	unsigned int proto, mask;

	if (max_mw <= HS104_PW_AF)
		proto = HS104_PROTO_AF;
	else if (max_mw <= HS104_PW_AT)
		proto = HS104_PROTO_AT;
	else if (max_mw <= HS104_PW_BT)
		proto = HS104_PROTO_BT;
	else if (max_mw <= HS104_PW_HIPO)
		proto = HS104_PROTO_HIPO;
	else
		return -EINVAL;

	mask = HS104_PROTO_MASK << (id * 2);
	proto <<= (id * 2);

	return regmap_update_bits(priv->regmap, HS104_REG_PROTOCOL, mask, proto);
}

static int hs104_pi_get_admin_state(struct pse_controller_dev *pcdev, int id,
				    struct pse_admin_state *admin_state)
{
	struct hs104_priv *priv = to_hs104(pcdev);
	unsigned int val;
	int ret;

	ret = regmap_read(priv->regmap, HS104_REG_PW_EN, &val);
	if (ret) {
		admin_state->c33_admin_state =
			ETHTOOL_C33_PSE_ADMIN_STATE_UNKNOWN;
		return ret;
	}

	if (val & HS104_PORT_BIT(id))
		admin_state->c33_admin_state = ETHTOOL_C33_PSE_ADMIN_STATE_ENABLED;
	else
		admin_state->c33_admin_state = ETHTOOL_C33_PSE_ADMIN_STATE_DISABLED;

	return 0;
}

static int hs104_pi_get_pw_status(struct pse_controller_dev *pcdev, int id,
				  struct pse_pw_status *pw_status)
{
	struct hs104_priv *priv = to_hs104(pcdev);
	unsigned int val;
	int ret;

	ret = regmap_read(priv->regmap, HS104_REG_PW_STATUS, &val);
	if (ret) {
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_UNKNOWN;
		return ret;
	}

	if (val & HS104_PORT_BIT(id))
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_DELIVERING;
	else
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_DISABLED;

	return 0;
}

static int hs104_pi_get_pw_class(struct pse_controller_dev *pcdev, int id)
{
	struct hs104_priv *priv = to_hs104(pcdev);
	unsigned int val;
	int ret;

	ret = regmap_read(priv->regmap, HS104_REG_PORT0_CLASS + id, &val);
	if (ret)
		return ret;

	return val;
}

static int hs104_pi_get_actual_pw(struct pse_controller_dev *pcdev, int id)
{
	struct hs104_priv *priv = to_hs104(pcdev);

	return hs104_read_be16(priv, HS104_REG_PORT0_POWER + id * 2,
			       HS104_MW_STEP);
}

static const struct ethtool_c33_pse_pw_limit_range hs104_pw_ranges[] = {
	{ .min = HS104_PW_AF,   .max = HS104_PW_AF },
	{ .min = HS104_PW_AT,   .max = HS104_PW_AT },
	{ .min = HS104_PW_BT,   .max = HS104_PW_BT },
	{ .min = HS104_PW_HIPO, .max = HS104_PW_HIPO },
};

static int hs104_pi_get_pw_limit_ranges(struct pse_controller_dev *pcdev,
					int id,
					struct pse_pw_limit_ranges *pw_limit_ranges)
{
	struct ethtool_c33_pse_pw_limit_range *c33_pw_limit_ranges;

	c33_pw_limit_ranges = kmemdup(hs104_pw_ranges, sizeof(hs104_pw_ranges),
				      GFP_KERNEL);
	if (!c33_pw_limit_ranges)
		return -ENOMEM;

	pw_limit_ranges->c33_pw_limit_ranges = c33_pw_limit_ranges;

	/* Return number of ranges */
	return ARRAY_SIZE(hs104_pw_ranges);
}

static const struct pse_controller_ops hs104_ops = {
	.pi_enable		= hs104_pi_enable,
	.pi_disable		= hs104_pi_disable,
	.pi_get_admin_state	= hs104_pi_get_admin_state,
	.pi_get_pw_status	= hs104_pi_get_pw_status,
	.pi_get_pw_class	= hs104_pi_get_pw_class,
	.pi_get_actual_pw	= hs104_pi_get_actual_pw,
	.pi_get_voltage		= hs104_pi_get_voltage,
	.pi_get_pw_limit	= hs104_pi_get_pw_limit,
	.pi_set_pw_limit	= hs104_pi_set_pw_limit,
	.pi_get_pw_limit_ranges	= hs104_pi_get_pw_limit_ranges,
};

/* Driver initialization */

static const struct regmap_config hs104_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static int hs104_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct hs104_priv *priv;
	unsigned int devid;
	int ret;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENXIO;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->regmap = devm_regmap_init_i2c(client, &hs104_regmap_config);
	if (IS_ERR(priv->regmap))
		return PTR_ERR(priv->regmap);

	/* Verify device ID */
	ret = regmap_read(priv->regmap, HS104_REG_DEVID, &devid);
	if (ret)
		return ret;

	if (devid != HS104_DEVICE_ID) {
		dev_err(dev, "Unknown device ID: 0x%02x\n", devid);
		return -ENODEV;
	}

	/*
	 * Enable all ports before registering the PSE controller, so that
	 * pse_pi_is_hw_enabled() sees the correct state when consumers
	 * request PSE controls. HS104 only delivers power to valid PDs.
	 */
	ret = regmap_write(priv->regmap, HS104_REG_PW_EN,
			   HS104_EXECUTE | GENMASK(HS104_MAX_PORTS - 1, 0));
	if (ret)
		dev_warn(dev, "Failed to enable ports: %d\n", ret);

	/* Register PSE controller */
	priv->pcdev.ops = &hs104_ops;
	priv->pcdev.dev = dev;
	priv->pcdev.owner = THIS_MODULE;
	priv->pcdev.nr_lines = HS104_MAX_PORTS;
	priv->pcdev.of_pse_n_cells = 1;
	priv->pcdev.types = ETHTOOL_PSE_C33;

	ret = devm_pse_controller_register(dev, &priv->pcdev);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to register PSE controller\n");

	dev_info(dev, "HS104 PSE controller initialized\n");
	return 0;
}

static const struct of_device_id hs104_of_match[] = {
	{ .compatible = "hasivo,hs104" },
	{ .compatible = "hasivo,hs104pti" },
	{ .compatible = "hasivo,hs104pbi" },
	{ }
};
MODULE_DEVICE_TABLE(of, hs104_of_match);

static struct i2c_driver hs104_driver = {
	.driver = {
		.name		= "hasivo-hs104",
		.of_match_table	= hs104_of_match,
	},
	.probe	= hs104_probe,
};
module_i2c_driver(hs104_driver);

MODULE_AUTHOR("Bevan Weiss <bevan.weiss@gmail.com>");
MODULE_AUTHOR("Carlo Szelinsky <github@szelinsky.de>");
MODULE_DESCRIPTION("Hasivo HS104 PoE PSE Controller");
MODULE_LICENSE("GPL");
