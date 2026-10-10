// SPDX-License-Identifier: GPL-2.0-only

#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <net/arp.h>
#include <net/nexthop.h>
#include <net/neighbour.h>
#include <net/netevent.h>
#include <linux/cleanup.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/inetdevice.h>
#include <linux/platform_device.h>
#include <linux/rhashtable.h>
#include <linux/of_net.h>
#include <asm/mach-rtl-otto/mach-rtl-otto.h>

#include "l2.h"
#include "l3.h"
#include "pie.h"
#include "rtl-otto.h"
#include "tc.h"
#include "stp.h"

/* Port register accessor functions for the RTL838x and RTL930X SoCs */
void rtl838x_mask_port_reg(u64 clear, u64 set, int reg)
{
	sw_w32_mask((u32)clear, (u32)set, reg);
}

void rtl838x_set_port_reg(u64 set, int reg)
{
	sw_w32((u32)set, reg);
}

u64 rtl838x_get_port_reg(int reg)
{
	return ((u64)sw_r32(reg));
}

/* Port register accessor functions for the RTL839x and RTL931X SoCs */
void rtl839x_mask_port_reg_be(u64 clear, u64 set, int reg)
{
	sw_w32_mask((u32)(clear >> 32), (u32)(set >> 32), reg);
	sw_w32_mask((u32)(clear & 0xffffffff), (u32)(set & 0xffffffff), reg + 4);
}

u64 rtl839x_get_port_reg_be(int reg)
{
	u64 v = sw_r32(reg);

	v <<= 32;
	v |= sw_r32(reg + 4);

	return v;
}

void rtl839x_set_port_reg_be(u64 set, int reg)
{
	sw_w32(set >> 32, reg);
	sw_w32(set & 0xffffffff, reg + 4);
}

void rtl839x_mask_port_reg_le(u64 clear, u64 set, int reg)
{
	sw_w32_mask((u32)clear, (u32)set, reg);
	sw_w32_mask((u32)(clear >> 32), (u32)(set >> 32), reg + 4);
}

void rtl839x_set_port_reg_le(u64 set, int reg)
{
	sw_w32(set, reg);
	sw_w32(set >> 32, reg + 4);
}

u64 rtl839x_get_port_reg_le(int reg)
{
	u64 v = sw_r32(reg + 4);

	v <<= 32;
	v |= sw_r32(reg);

	return v;
}

static bool rtldsa_phy_load_deferred(struct phy_device *phydev)
{
	struct device *d = &phydev->mdio.dev;

	if (d->driver)
		return false;

	return driver_deferred_probe_check_state(d) == -EPROBE_DEFER;
}

static bool rtldsa_phys_load_deferred(void)
{
	struct device_node *phy_node;
	struct phy_device *phydev;
	struct device_node *dn;
	bool deferred;

	for_each_node_by_name(dn, "port") {
		if (!of_device_is_available(dn))
			continue;

		phy_node = of_parse_phandle(dn, "phy-handle", 0);
		if (!phy_node)
			continue;

		phydev = of_phy_find_device(phy_node);
		of_node_put(phy_node);
		if (!phydev)
			continue;

		deferred = rtldsa_phy_load_deferred(phydev);
		put_device(&phydev->mdio.dev);

		if (deferred) {
			of_node_put(dn);
			return true;
		}
	}

	return false;
}

static int rtldsa_mdio_loaded(void)
{
	struct device_node *dn;

	/* Check if all buses of the Realtek MDIO controller are registered. */
	dn = of_find_compatible_node(NULL, NULL, "realtek,otto-mdio");
	if (!of_device_is_available(dn)) {
		of_node_put(dn);
		return -ENODEV;
	}

	for_each_child_of_node_scoped(dn, bn) {
		struct mii_bus *bus = of_mdio_find_bus(bn);

		if (!bus) {
			of_node_put(dn);
			return -EPROBE_DEFER;
		}
		put_device(&bus->dev);
	}
	of_node_put(dn);

	return 0;
}

static int rtldsa_ports_probe(struct rtl838x_switch_priv *priv)
{
	struct device_node *dn, *phy_node, *led_node;
	u32 pn;

	dn = of_find_compatible_node(NULL, NULL, "realtek,otto-switch");
	if (!dn) {
		dev_err(priv->dev, "No RTL switch node in DTS\n");
		return -ENODEV;
	}

	led_node = of_find_compatible_node(NULL, NULL, "realtek,rtl9300-leds");

	for_each_node_by_name(dn, "port") {
		u32 led_set;
		char led_set_str[16] = {0};

		if (!of_device_is_available(dn))
			continue;

		if (of_property_read_u32(dn, "reg", &pn))
			continue;

		phy_node = of_parse_phandle(dn, "phy-handle", 0);
		priv->ports[pn].has_pcs = fwnode_property_present(of_fwnode_handle(dn),
								  "pcs-handle");
		if (pn != priv->r->cpu_port && !phy_node && !priv->ports[pn].has_pcs) {
			dev_err(priv->dev, "Port node %d has neither pcs-handle nor phy-handle\n", pn);
			continue;
		}

		priv->ports[pn].leds_on_this_port = 0;
		if (led_node) {
			if (of_property_read_u32(dn, "led-set", &led_set))
				led_set = 0;
			priv->ports[pn].led_set = led_set;
			sprintf(led_set_str, "led_set%d", led_set);
			priv->ports[pn].leds_on_this_port = of_property_count_u32_elems(led_node, led_set_str);
			if (priv->ports[pn].leds_on_this_port > 4) {
				of_node_put(dn);
				dev_err(priv->dev, "led_set %d for port %d configuration is invalid\n", led_set, pn);
				return -ENODEV;
			}
		}

		priv->ports[pn].phy = !!phy_node;
	}

	return 0;
}

static int rtl83xx_get_l2aging(struct rtl838x_switch_priv *priv)
{
	int t = sw_r32(priv->r->l2_ctrl_1);

	/* RTL838x uses a high-resolution 23-bit AGE_UNIT where one unit is
	 * 204.8 ms. RTL839x and RTL93xx use a 21-bit AGE_UNIT where one unit
	 * is 600 ms. An AGE_UNIT value of 0 disables dynamic address aging.
	 */
	if (priv->r->high_res_l2_age) {
		t &= GENMASK(22, 0);
		t = t * 128 / 625;
	} else {
		t &= GENMASK(20, 0);
		t = t * 3 / 5;
	}

	pr_debug("L2 AGING time: %d sec\n", t);
	pr_debug("Dynamic aging for ports: %x\n", sw_r32(priv->r->l2_port_aging_out));

	return t;
}

// Currently Unused
// /* Allocate a 64 bit octet counter located in the LOG HW table */
// static int rtl83xx_octet_cntr_alloc(struct rtl838x_switch_priv *priv)
// {
// 	int idx;

// 	mutex_lock(&priv->reg_mutex);

// 	idx = find_first_zero_bit(priv->octet_cntr_use_bm, MAX_COUNTERS);
// 	if (idx >= priv->r->n_counters) {
// 		mutex_unlock(&priv->reg_mutex);
// 		return -1;
// 	}

// 	set_bit(idx, priv->octet_cntr_use_bm);
// 	mutex_unlock(&priv->reg_mutex);

// 	return idx;
// }

/*
 * Packet counters share hardware memory with octet counters (2 packet counters
 * per 1 octet block). Allocation relies on two complementary bitmaps:
 *
 *   octet_cntr_use_bm:  0 = free block, 1 = used (or split into packet counters)
 *   packet_cntr_use_bm: 1 = free standalone counter, 0 = unavailable
 *
 * Allocation strategy:
 * 1. Look for a free standalone counter from an already split block (bit = 1).
 * 2. If none are free, claim a new octet block 'j', use counter index (2 * j),
 *    and mark counter (2 * j + 1) as available for future allocations.
 */

/**
 * rtldsa_packet_cntr_alloc - Allocate a hardware packet counter.
 * @priv: Switch driver private structure.
 *
 * Return: Counter index (>= 0) on success, or -1 if full.
 */
int rtldsa_packet_cntr_alloc(struct rtl838x_switch_priv *priv)
{
	int idx, j, base = 0;

	/* On SoCs where a PIE rule implicitly logs into the LOG table entry
	 * with its own rule ID (RTL930x), the tc cls_flower offload consumes
	 * one LOG counter per PIE rule and does not pass through this
	 * allocator. Keep that range reserved so a route counter handed out
	 * here cannot alias a flow's LOG entry.
	 *
	 * The offload is ingress-only, so PIE rule IDs currently populate
	 * only the lower half of that range, and a route counter's LOG entry
	 * (idx / 2) never dips below the upper half either. If egress PIE
	 * rules are ever wired up, this reservation needs to grow so their
	 * LOG entries stay clear of route counters too.
	 */
	if (priv->r->pie_rule_id_is_log_counter)
		base = priv->r->n_pie_blocks * PIE_BLOCK_SIZE;

	scoped_guard(mutex, &priv->reg_mutex) {
		idx = find_next_bit(priv->packet_cntr_use_bm, priv->r->n_counters * 2, base);
		if (idx >= priv->r->n_counters * 2) {
			j = find_next_zero_bit(priv->octet_cntr_use_bm, priv->r->n_counters,
					       base / 2);
			if (j >= priv->r->n_counters)
				return -1;

			__set_bit(j, priv->octet_cntr_use_bm);
			idx = j * 2;
			__set_bit(j * 2 + 1, priv->packet_cntr_use_bm);
		} else {
			__clear_bit(idx, priv->packet_cntr_use_bm);
		}
	}

	return idx;
}

/**
 * rtldsa_packet_cntr_free - Release a packet counter from rtldsa_packet_cntr_alloc().
 * @priv: Switch driver private structure.
 * @idx: Packet counter index to free; a negative id is ignored.
 *
 * Marks the counter free again and, once both halves of its octet block are
 * free, returns the whole block to the octet counter pool.
 */
void rtldsa_packet_cntr_free(struct rtl838x_switch_priv *priv, int idx)
{
	int j;

	if (idx < 0 || idx >= priv->r->n_counters * 2)
		return;

	scoped_guard(mutex, &priv->reg_mutex) {
		/* already free - guard against a double release */
		if (test_bit(idx, priv->packet_cntr_use_bm))
			return;

		__set_bit(idx, priv->packet_cntr_use_bm);

		j = idx / 2;
		if (test_bit(j, priv->octet_cntr_use_bm) &&
		    test_bit(idx ^ 1, priv->packet_cntr_use_bm)) {
			__clear_bit(idx, priv->packet_cntr_use_bm);
			__clear_bit(idx ^ 1, priv->packet_cntr_use_bm);
			__clear_bit(j, priv->octet_cntr_use_bm);
		}
	}
}

int rtl83xx_port_is_under(const struct net_device *dev, struct rtl838x_switch_priv *priv)
{
	/* Is the lower network device a DSA user network device of our RTL930X-switch?
	 * Unfortunately we cannot just follow dev->dsa_prt as this is only set for the
	 * DSA conduit device. TODO: since 6.12:
	 *
	 * if(!dsa_user_dev_check(dev)) {
	 *   netdev_info(dev, "%s: not a DSA device.\n", __func__);
	 *   return -EINVAL;
	 * }
	 */

	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (!priv->ports[i].dp)
			continue;
		if (priv->ports[i].dp->user == dev)
			return i;
	}

	return -EINVAL;
}

static irqreturn_t rtldsa_switch_irq(int irq, void *dev_id)
{
	struct rtl838x_switch_priv *priv;
	struct dsa_switch *ds = dev_id;
	u64 link, ports;

	priv = ds->priv;
	ports = priv->r->get_port_reg_le(priv->r->isr_port_link_sts_chg);
	priv->r->set_port_reg_le(ports, priv->r->isr_port_link_sts_chg);

	/* read latched */
	link = priv->r->get_port_reg_le(priv->r->mac_link_sts);
	link = priv->r->get_port_reg_le(priv->r->mac_link_sts);

	for (int port = 0; port < priv->r->cpu_port; port++)
		if (ports & BIT_ULL(port))
			dsa_port_phylink_mac_change(ds, port, link & BIT_ULL(port));

	return IRQ_HANDLED;
}

/*
 * TODO: This check is usually built into the DSA initialization functions. After carving
 * out the mdio driver from the ethernet driver, there are two drivers that must be loaded
 * before the DSA setup can start. This driver has severe issues with handling of deferred
 * probing. For now provide this function for early dependency checks.
 */
static int rtldsa_ethernet_loaded(struct platform_device *pdev)
{
	struct platform_device *eth_pdev;
	struct device_node *port_np;
	struct device_node *eth_np;
	int ret = -EPROBE_DEFER;

	for_each_node_with_property(port_np, "ethernet") {
		eth_np = of_parse_phandle(port_np, "ethernet", 0);
		if (!eth_np)
			continue;

		eth_pdev = of_find_device_by_node(eth_np);
		of_node_put(eth_np);

		if (!eth_pdev)
			continue;

		if (eth_pdev->dev.driver)
			ret = 0;

		put_device(&eth_pdev->dev);
	}

	return ret;
}

static int rtl83xx_sw_probe(struct platform_device *pdev)
{
	struct rtl838x_switch_priv *priv;
	struct device *dev = &pdev->dev;
	const struct rtldsa_config *r;
	u64 bpdu_mask;
	int err = 0;

	pr_debug("Probing RTL838X switch device\n");
	if (!pdev->dev.of_node) {
		dev_err(dev, "No DT found\n");
		return -EINVAL;
	}

	if (rtldsa_phys_load_deferred())
		return -EPROBE_DEFER;

	err = rtldsa_ethernet_loaded(pdev);
	if (err)
		return err;

	err = otto_table_loaded();
	if (err)
		return dev_err_probe(dev, err, "no switch table access\n");

	r = device_get_match_data(&pdev->dev);
	priv = devm_kzalloc(dev, struct_size(priv, msts, r->n_mst - 1), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->r = r;

	priv->l2_uc_map = devm_kcalloc(dev, r->fib_entries, sizeof(*priv->l2_uc_map),
				       GFP_KERNEL);
	if (!priv->l2_uc_map)
		return -ENOMEM;

	priv->ds = devm_kzalloc(dev, sizeof(*priv->ds), GFP_KERNEL);
	if (!priv->ds)
		return -ENOMEM;

	priv->ds->dev = dev;
	priv->ds->priv = priv;
	priv->ds->needs_standalone_vlan_filtering = true;
	priv->ds->ops = priv->r->switch_ops;
	priv->ds->phylink_mac_ops = priv->r->phylink_mac_ops;
	priv->ds->num_lag_ids = priv->r->num_lag_ids;
	priv->ds->num_ports = priv->r->cpu_port + 1;

	priv->dev = dev;
	dev_set_drvdata(dev, priv);

	err = devm_mutex_init(dev, &priv->reg_mutex);
	if (err)
		return err;

	err = devm_mutex_init(dev, &priv->counters_lock);
	if (err)
		return err;

	priv->family_id = soc_info.family;
	priv->r->stp_init();
	priv->irq_mask = GENMASK_ULL(priv->r->cpu_port - 1, 0);

	err = rtldsa_mdio_loaded();
	if (err)
		return err;

	err = rtldsa_ports_probe(priv);
	if (err)
		return err;

	priv->wq = create_singlethread_workqueue("rtl83xx");
	if (!priv->wq) {
		dev_err(dev, "Error creating workqueue: %d\n", err);
		return -ENOMEM;
	}

	err = dsa_register_switch(priv->ds);
	if (err) {
		dev_err(dev, "Error registering switch: %d\n", err);
		goto err_register_switch;
	}

	/* dsa_to_port returns dsa_port from the port list in
	 * dsa_switch_tree, the tree is built when the switch
	 * is registered by dsa_register_switch
	 */
	for (int i = 0; i <= priv->r->cpu_port; i++)
		priv->ports[i].dp = dsa_to_port(priv->ds, i);

	/* Enable link and media change interrupts. Are the SERDES masks needed? */
	sw_w32_mask(0, 3, priv->r->isr_glb_src);

	priv->r->set_port_reg_le(priv->irq_mask, priv->r->isr_port_link_sts_chg);
	priv->r->set_port_reg_le(priv->irq_mask, priv->r->imr_port_link_sts_chg);

	priv->link_state_irq = platform_get_irq(pdev, 0);
	pr_info("LINK state irq: %d\n", priv->link_state_irq);
	err = request_irq(priv->link_state_irq, rtldsa_switch_irq,
			  IRQF_SHARED, "rtldsa-link-state", priv->ds);
	if (err) {
		dev_err(dev, "Error setting up switch interrupt.\n");
		/* Need to free allocated switch here */
	}

	/* Enable interrupts for switch, on RTL931x, the IRQ is always on globally */
	if (soc_info.family != RTL9310_FAMILY_ID)
		sw_w32(0x1, priv->r->imr_glb);

	rtl83xx_get_l2aging(priv);

	if (priv->r->qos_init)
		priv->r->qos_init(priv);

	/* Clear all destination ports for mirror groups */
	for (int i = 0; i < 4; i++)
		priv->mirror_group_ports[i] = -1;

	err = otto_l3_probe(dev, priv);
	if (err)
		goto err_register_l3;

	/* TODO: put this into l2_setup() */
	switch (soc_info.family) {
	default:
		/* Flood BPDUs to all ports including cpu-port */
		bpdu_mask = soc_info.family == RTL8380_FAMILY_ID ? 0x1FFFFFFF : 0x1FFFFFFFFFFFFF;
		priv->r->set_port_reg_be(bpdu_mask, priv->r->rma_bpdu_fld_pmask);

		/* TRAP 802.1X frames (EAPOL) to the CPU-Port, bypass STP and VLANs */
		sw_w32(7, priv->r->spcl_trap_eapol_ctrl);

		rtl838x_dbgfs_init(priv);
		break;
	case RTL9300_FAMILY_ID:
	case RTL9310_FAMILY_ID:
		rtl930x_dbgfs_init(priv);
		break;
	}

	otto_l2_dbgfs_init(priv);

	if (priv->r->lag_switch_init)
		priv->r->lag_switch_init(priv);

	return 0;

err_register_l3:
	dsa_switch_shutdown(priv->ds);
err_register_switch:
	rtldsa_tc_cleanup(priv);
	destroy_workqueue(priv->wq);

	return err;
}

static void rtl83xx_sw_remove(struct platform_device *pdev)
{
	struct rtl838x_switch_priv *priv = platform_get_drvdata(pdev);

	if (!priv)
		return;

	/* TODO: */
	pr_debug("Removing platform driver for rtl83xx-sw\n");

	/* unregister notifiers which will create workqueue entries with
	 * references to the switch structures. Also stop self-arming delayed
	 * work items to avoid them still accessing the DSA structures
	 * when they are getting shut down.
	 */
	otto_l3_remove(priv);
	cancel_delayed_work_sync(&priv->counters_work);

	dsa_switch_shutdown(priv->ds);

	rtldsa_tc_cleanup(priv);

	destroy_workqueue(priv->wq);

	dev_set_drvdata(&pdev->dev, NULL);
}

static const struct of_device_id rtl83xx_switch_of_ids[] = {
	{
		.compatible = "realtek,rtl8380-switch",
		.data = &rtldsa_838x_cfg,
	},
	{
		.compatible = "realtek,rtl8392-switch",
		.data = &rtldsa_839x_cfg,
	},
	{
		.compatible = "realtek,rtl9301-switch",
		.data = &rtldsa_930x_cfg,
	},
	{
		.compatible = "realtek,rtl9311-switch",
		.data = &rtldsa_931x_cfg,
	},
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, rtl83xx_switch_of_ids);

static struct platform_driver rtl83xx_switch_driver = {
	.probe  = rtl83xx_sw_probe,
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
