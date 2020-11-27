// SPDX-License-Identifier: GPL-2.0-only

#include <linux/of_mdio.h>
#include <linux/of_platform.h>

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl83xx.h"

extern struct rtl83xx_soc_info soc_info;

extern const struct rtl838x_reg rtl838x_reg;
extern const struct rtl838x_reg rtl839x_reg;
extern const struct dsa_switch_ops rtl83xx_switch_ops;

DEFINE_MUTEX(smi_lock);


// TODO: unused
static void dump_fdb(struct rtl838x_switch_priv *priv)
{
	struct rtl838x_l2_entry e;
	int i;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < priv->fib_entries; i++) {
		priv->r->read_l2_entry_using_hash(i >> 2, i & 0x3, &e);

		if (!e.valid) /* Check for invalid entry */
			continue;

		pr_debug("-> port %02d: mac %pM, vid: %d, rvid: %d, MC: %d, %d\n",
			e.port, &e.mac[0], e.vid, e.rvid, e.is_ip_mc, e.is_ipv6_mc);
	}

	mutex_unlock(&priv->reg_mutex);
}

// TODO: unused
static void rtl83xx_port_get_stp_state(struct rtl838x_switch_priv *priv, int port)
{
	u32 cmd, msti = 0;
	u32 port_state[4];
	int index, bit, i;
	int pos = port;
	int n = priv->family_id == RTL8380_FAMILY_ID ? 2 : 4;

	/* CPU PORT can only be configured on RTL838x */
	if (port >= priv->cpu_port || port > 51)
		return;

	mutex_lock(&priv->reg_mutex);

	/* For the RTL839x, the bits are left-aligned in the 128 bit field */
	if (priv->family_id == RTL8390_FAMILY_ID)
		pos += 12;

	index = n - (pos >> 4) - 1;
	bit = (pos << 1) % 32;

	if (priv->family_id == RTL8380_FAMILY_ID) {
		cmd = BIT(15) /* Execute cmd */
			| BIT(14) /* Read */
			| 2 << 12 /* Table type 0b10 */
			| (msti & 0xfff);
	} else {
		cmd = BIT(16) /* Execute cmd */
			| 0 << 15 /* Read */
			| 5 << 12 /* Table type 0b101 */
			| (msti & 0xfff);
	}
	priv->r->exec_tbl0_cmd(cmd);

	for (i = 0; i < n; i++)
		port_state[i] = sw_r32(priv->r->tbl_access_data_0(i));

	mutex_unlock(&priv->reg_mutex);
}

int rtl83xx_dsa_phy_read(struct dsa_switch *ds, int phy_addr, int phy_reg)
{
	u32 val;
	u32 offset = 0;
	struct rtl838x_switch_priv *priv = ds->priv;

	if (phy_addr >= 24 && phy_addr <= 27
		&& priv->ports[24].phy == PHY_RTL838X_SDS) {
		if (phy_addr == 26)
			offset = 0x100;
		val = sw_r32(MAPLE_SDS4_FIB_REG0r + offset + (phy_reg << 2)) & 0xffff;
		return val;
	}

	if (soc_info.family == RTL8390_FAMILY_ID)
		rtl839x_read_phy(phy_addr, 0, phy_reg, &val);
	else
		rtl838x_read_phy(phy_addr, 0, phy_reg, &val);
	return val;
}

int rtl83xx_dsa_phy_write(struct dsa_switch *ds, int phy_addr, int phy_reg, u16 val)
{
	u32 offset = 0;
	struct rtl838x_switch_priv *priv = ds->priv;

	if (phy_addr >= 24 && phy_addr <= 27
	     && priv->ports[24].phy == PHY_RTL838X_SDS) {
		if (phy_addr == 26)
			offset = 0x100;
		sw_w32(val, MAPLE_SDS4_FIB_REG0r + offset + (phy_reg << 2));
		return 0;
	}
	if (soc_info.family == RTL8390_FAMILY_ID)
		return rtl839x_write_phy(phy_addr, 0, phy_reg, val);
	else
		return rtl838x_write_phy(phy_addr, 0, phy_reg, val);
}

static int rtl83xx_mdio_read(struct mii_bus *bus, int addr, int regnum)
{
	int ret;
	struct rtl838x_switch_priv *priv = bus->priv;

	ret = rtl83xx_dsa_phy_read(priv->ds, addr, regnum);
	return ret;
}

static int rtl83xx_mdio_write(struct mii_bus *bus, int addr, int regnum,
				 u16 val)
{
	struct rtl838x_switch_priv *priv = bus->priv;

	return rtl83xx_dsa_phy_write(priv->ds, addr, regnum, val);
}

static void rtl8380_sds_rst(int mac)
{
	u32 offset = (mac == 24) ? 0 : 0x100;

	sw_w32_mask(1 << 11, 0, RTL8380_SDS4_FIB_REG0 + offset);
	sw_w32_mask(0x3, 0, RTL838X_SDS4_REG28 + offset);
	sw_w32_mask(0x3, 0x3, RTL838X_SDS4_REG28 + offset);
	sw_w32_mask(0, 0x1 << 6, RTL838X_SDS4_DUMMY0 + offset);
	sw_w32_mask(0x1 << 6, 0, RTL838X_SDS4_DUMMY0 + offset);
	pr_debug("SERDES reset: %d\n", mac);
}

static int __init rtl8380_sds_power(int mac, int val)
{
	u32 mode = (val == 1) ? 0x4 : 0x9;
	u32 offset = (mac == 24) ? 5 : 0;

	if ((mac != 24) && (mac != 26)) {
		pr_err("%s: not a fibre port: %d\n", __func__, mac);
		return -1;
	}

	sw_w32_mask(0x1f << offset, mode << offset, RTL838X_SDS_MODE_SEL);

	rtl8380_sds_rst(mac);

	return 0;
}

static int __init rtl83xx_mdio_probe(struct rtl838x_switch_priv *priv)
{
	struct device *dev = priv->dev;
	struct device_node *dn, *mii_np = dev->of_node;
	struct mii_bus *bus;
	int ret;
	u32 pn;

	pr_debug("In %s\n", __func__);
	mii_np = of_find_compatible_node(NULL, NULL, "realtek,rtl838x-mdio");
	if (mii_np) {
		pr_debug("Found compatible MDIO node!\n");
	} else {
		dev_err(priv->dev, "no %s child node found", "mdio-bus");
		return -ENODEV;
	}

	priv->mii_bus = of_mdio_find_bus(mii_np);
	if (!priv->mii_bus) {
		pr_debug("Deferring probe of mdio bus\n");
		return -EPROBE_DEFER;
	}
	if (!of_device_is_available(mii_np))
		ret = -ENODEV;

	bus = devm_mdiobus_alloc(priv->ds->dev);
	if (!bus)
		return -ENOMEM;

	bus->name = "rtl838x slave mii";
	bus->read = &rtl83xx_mdio_read;
	bus->write = &rtl83xx_mdio_write;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%s-%d", bus->name, dev->id);
	bus->parent = dev;
	priv->ds->slave_mii_bus = bus;
	priv->ds->slave_mii_bus->priv = priv;

	ret = mdiobus_register(priv->ds->slave_mii_bus);
	if (ret && mii_np) {
		of_node_put(dn);
		return ret;
	}

	dn = mii_np;
	for_each_node_by_name(dn, "ethernet-phy") {
		if (of_property_read_u32(dn, "reg", &pn))
			continue;

		priv->ports[pn].dp = dsa_to_port(priv->ds, pn);

		// Check for the integrated SerDes of the RTL8380M first
		if (of_property_read_bool(dn, "phy-is-integrated")
			&& priv->id == 0x8380 && pn >= 24) {
			pr_debug("----> FÓUND A SERDES\n");
			priv->ports[pn].phy = PHY_RTL838X_SDS;
			continue;
		}

		if (of_property_read_bool(dn, "phy-is-integrated")
			&& !of_property_read_bool(dn, "sfp")) {
			priv->ports[pn].phy = PHY_RTL8218B_INT;
			continue;
		}

		if (!of_property_read_bool(dn, "phy-is-integrated")
			&& of_property_read_bool(dn, "sfp")) {
			priv->ports[pn].phy = PHY_RTL8214FC;
			continue;
		}

		if (!of_property_read_bool(dn, "phy-is-integrated")
			&& !of_property_read_bool(dn, "sfp")) {
			priv->ports[pn].phy = PHY_RTL8218B_EXT;
			continue;
		}
	}

	/* Disable MAC polling the PHY so that we can start configuration */
	priv->r->set_port_reg_le(0ULL, priv->r->smi_poll_ctrl);

	/* Enable PHY control via SoC */
	if (priv->family_id == RTL8380_FAMILY_ID) {
		/* Enable PHY control via SoC */
		sw_w32_mask(0, BIT(15), RTL838X_SMI_GLB_CTRL);
	} else {
		/* Disable PHY polling via SoC */
		sw_w32_mask(BIT(7), 0, RTL839X_SMI_GLB_CTRL);
	}

	/* Power on fibre ports and reset them if necessary */
	if (priv->ports[24].phy == PHY_RTL838X_SDS) {
		pr_debug("Powering on fibre ports & reset\n");
		rtl8380_sds_power(24, 1);
		rtl8380_sds_power(26, 1);
	}

	pr_debug("%s done\n", __func__);
	return 0;
}

static int __init rtl83xx_get_l2aging(struct rtl838x_switch_priv *priv)
{
	int t = sw_r32(priv->r->l2_ctrl_1);

	t &= priv->family_id == RTL8380_FAMILY_ID ? 0x7fffff : 0x1FFFFF;

	if (priv->family_id == RTL8380_FAMILY_ID)
		t = t * 128 / 625; /* Aging time in seconds. 0: L2 aging disabled */
	else
		t = (t * 3) / 5;

	pr_debug("L2 AGING time: %d sec\n", t);
	pr_debug("Dynamic aging for ports: %x\n", sw_r32(priv->r->l2_port_aging_out));
	return t;
}

static int __init rtl83xx_sw_probe(struct platform_device *pdev)
{
	int err = 0, i;
	struct rtl838x_switch_priv *priv;
	struct device *dev = &pdev->dev;
	u64 irq_mask;

	pr_debug("Probing RTL838X switch device\n");
	if (!pdev->dev.of_node) {
		dev_err(dev, "No DT found\n");
		return -EINVAL;
	}

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->ds = dsa_switch_alloc(dev, DSA_MAX_PORTS);

	if (!priv->ds)
		return -ENOMEM;
	priv->ds->dev = dev;
	priv->ds->priv = priv;
	priv->ds->ops = &rtl83xx_switch_ops;
	priv->dev = dev;

	priv->family_id = soc_info.family;
	priv->id = soc_info.id;
	if (soc_info.family == RTL8380_FAMILY_ID) {
		priv->cpu_port = RTL838X_CPU_PORT;
		priv->port_mask = 0x1f;
		priv->r = &rtl838x_reg;
		priv->ds->num_ports = 30;
		priv->fib_entries = 8192;
		rtl8380_get_version(priv);
	} else {
		priv->cpu_port = RTL839X_CPU_PORT;
		priv->port_mask = 0x3f;
		priv->r = &rtl839x_reg;
		priv->ds->num_ports = 53;
		priv->fib_entries = 16384;
		rtl8390_get_version(priv);
	}
	pr_debug("Chip version %c\n", priv->version);

	err = rtl83xx_mdio_probe(priv);
	if (err) {
		/* Probing fails the 1st time because of missing ethernet driver
		 * initialization. Use this to disable traffic in case the bootloader left if on
		 */
		return err;
	}
	err = dsa_register_switch(priv->ds);
	if (err) {
		dev_err(dev, "Error registering switch: %d\n", err);
		return err;
	}

	/* Enable link and media change interrupts. Are the SERDES masks needed? */
	sw_w32_mask(0, 3, priv->r->isr_glb_src);
	/* ... for all ports */
	irq_mask = soc_info.family == RTL8380_FAMILY_ID ? 0x0FFFFFFF : 0xFFFFFFFFFFFFFULL;
	priv->r->set_port_reg_le(irq_mask, priv->r->isr_port_link_sts_chg);
	priv->r->set_port_reg_le(irq_mask, priv->r->imr_port_link_sts_chg);

	priv->link_state_irq = platform_get_irq(pdev, 0);;
	if (priv->family_id == RTL8380_FAMILY_ID) {
		err = request_irq(priv->link_state_irq, rtl838x_switch_irq,
				IRQF_SHARED, "rtl838x-link-state", priv->ds);
	} else {
		err = request_irq(priv->link_state_irq, rtl839x_switch_irq,
				IRQF_SHARED, "rtl839x-link-state", priv->ds);
	}
	if (err) {
		dev_err(dev, "Error setting up switch interrupt.\n");
		/* Need to free allocated switch here */
	}

	/* Enable interrupts for switch */
	sw_w32(0x1, priv->r->imr_glb);

	rtl83xx_get_l2aging(priv);

/*
	if (priv->family_id == RTL8380_FAMILY_ID)
		rtl83xx_storm_control_init(priv);
*/

	/* Clear all destination ports for mirror groups */
	for (i = 0; i < 4; i++)
		priv->mirror_group_ports[i] = -1;

	rtl838x_dbgfs_init(priv);

	return err;
}

static int rtl83xx_sw_remove(struct platform_device *pdev)
{
	// TODO:
	pr_debug("Removing platform driver for rtl83xx-sw\n");
	return 0;
}

static const struct of_device_id rtl83xx_switch_of_ids[] = {
	{ .compatible = "realtek,rtl83xx-switch"},
	{ /* sentinel */ }
};


MODULE_DEVICE_TABLE(of, rtl83xx_switch_of_ids);

static struct platform_driver rtl83xx_switch_driver = {
	.probe = rtl83xx_sw_probe,
	.remove = rtl83xx_sw_remove,
	.driver = {
		.name = "rtl83xx-switch",
		.pm = NULL,
		.of_match_table = rtl83xx_switch_of_ids,
	},
};

module_platform_driver(rtl83xx_switch_driver);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL83XX SoC Switch Driver");
MODULE_LICENSE("GPL");
