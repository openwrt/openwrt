// SPDX-License-Identifier: GPL-2.0
/*
 * Based upon the Maxlinear SDK driver
 *
 * Copyright (C) 2024 MaxLinear Inc.
 * Copyright (C) 2025 John Crispin <john@phrozen.org>
 * Copyright (C) 2025 Daniel Golle <daniel@makrotopia.org>
 */

#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of_device.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <net/dsa.h>
#include <linux/dsa/8021q.h>
#include <linux/stddef.h>
#include <linux/version.h>
#include <linux/gpio/consumer.h>

#include "mxl862xx.h"
#include "mxl862xx-api.h"
#include "mxl862xx-cmd.h"
#include "mxl862xx-host.h"
#include "mxl862xx-firmware.h"


#define MXL862XX_API_WRITE(dev, cmd, data) \
	mxl862xx_api_wrap(dev, cmd, &(data), sizeof((data)), false)
#define MXL862XX_API_READ(dev, cmd, data) \
	mxl862xx_api_wrap(dev, cmd, &(data), sizeof((data)), true)

/* mainline driver does not hide port 0 (microcontroller), first usable port is index 1 */
#define DSA_MXL_PORT(port) ((port))

#define MXL862XX_SDMA_PCTRLP(p) (0xBC0 + ((p) * 0x6))
#define MXL862XX_SDMA_PCTRL_EN BIT(0) /* SDMA Port Enable */

/* Ethernet Switch Fetch DMA Port Control Register */
#define MXL862XX_FDMA_PCTRLP(p) (0xA80 + ((p) * 0x6))
#define MXL862XX_FDMA_PCTRL_EN BIT(0) /* FDMA Port Enable */

#define MAX_VLAN_ENTRIES (1024)
#define IDX_INVAL (-1)

#define INGRESS_FINAL_RULES 5
#define INGRESS_VID_RULES VID_RULES
#define EGRESS_FINAL_RULES 3
#define EGRESS_VID_RULES VID_RULES
/* It's only the size of the array for storing VLAN info.
 * The real number of simultaneous VLANS is lower
 * and depends on the number of filtering rules and ports.
 * It is calculated dynamically at runtime.
 */
#define MAX_RULES_RECYCLED MAX_VLANS

struct mxl862xx_mib_desc {
	unsigned int size;
	unsigned int offset;
	const char *name;
};

#define MIB_DESC(_size, _name, _element)						\
{										\
	.size = _size,								\
	.name = _name,								\
	.offset = offsetof(struct mxl862xx_rmon_port_cnt, _element)	\
}

static const struct mxl862xx_mib_desc mxl862xx_mib[] = {
	MIB_DESC(1, "TxGoodPkts", tx_good_pkts),
	MIB_DESC(1, "TxUnicastPkts", tx_unicast_pkts),
	MIB_DESC(1, "TxBroadcastPkts", tx_broadcast_pkts),
	MIB_DESC(1, "TxMulticastPkts", tx_multicast_pkts),
	MIB_DESC(1, "Tx64BytePkts", tx64byte_pkts),
	MIB_DESC(1, "Tx127BytePkts", tx127byte_pkts),
	MIB_DESC(1, "Tx255BytePkts", tx255byte_pkts),
	MIB_DESC(1, "Tx511BytePkts", tx511byte_pkts),
	MIB_DESC(1, "Tx1023BytePkts", tx1023byte_pkts),
	MIB_DESC(1, "TxMaxBytePkts", tx_max_byte_pkts),
	MIB_DESC(1, "TxDroppedPkts", tx_dropped_pkts),
	MIB_DESC(1, "TxAcmDroppedPkts", tx_acm_dropped_pkts),
	MIB_DESC(2, "TxGoodBytes", tx_good_bytes),
	MIB_DESC(1, "TxSingleCollCount", tx_single_coll_count),
	MIB_DESC(1, "TxMultCollCount", tx_mult_coll_count),
	MIB_DESC(1, "TxLateCollCount", tx_late_coll_count),
	MIB_DESC(1, "TxExcessCollCount", tx_excess_coll_count),
	MIB_DESC(1, "TxCollCount", tx_coll_count),
	MIB_DESC(1, "TxPauseCount", tx_pause_count),
	MIB_DESC(1, "RxGoodPkts", rx_good_pkts),
	MIB_DESC(1, "RxUnicastPkts", rx_unicast_pkts),
	MIB_DESC(1, "RxBroadcastPkts", rx_broadcast_pkts),
	MIB_DESC(1, "RxMulticastPkts", rx_multicast_pkts),
	MIB_DESC(1, "RxFCSErrorPkts", rx_fcserror_pkts),
	MIB_DESC(1, "RxUnderSizeGoodPkts", rx_under_size_good_pkts),
	MIB_DESC(1, "RxOversizeGoodPkts", rx_oversize_error_pkts),
	MIB_DESC(1, "RxUnderSizeErrorPkts", rx_under_size_error_pkts),
	MIB_DESC(1, "RxOversizeErrorPkts", rx_oversize_error_pkts),
	MIB_DESC(1, "RxFilteredPkts", rx_filtered_pkts),
	MIB_DESC(1, "Rx64BytePkts", rx64byte_pkts),
	MIB_DESC(1, "Rx127BytePkts", rx127byte_pkts),
	MIB_DESC(1, "Rx255BytePkts", rx255byte_pkts),
	MIB_DESC(1, "Rx511BytePkts", rx511byte_pkts),
	MIB_DESC(1, "Rx1023BytePkts", rx1023byte_pkts),
	MIB_DESC(1, "RxMaxBytePkts", rx_max_byte_pkts),
	MIB_DESC(1, "RxDroppedPkts", rx_dropped_pkts),
	MIB_DESC(1, "RxExtendedVlanDiscardPkts", rx_extended_vlan_discard_pkts),
	MIB_DESC(1, "MtuExceedDiscardPkts", mtu_exceed_discard_pkts),
	MIB_DESC(2, "RxGoodBytes", rx_good_bytes),
	MIB_DESC(2, "RxBadBytes", rx_bad_bytes),
	MIB_DESC(1, "RxGoodPausePkts", rx_good_pause_pkts),
	MIB_DESC(1, "RxAlignErrorPkts", rx_align_error_pkts),
};

static int mxl862xx_phy_read_mmd(struct mxl862xx_priv *priv, int port, int devadd, int reg)
{
	struct mdio_relay_data param = {
		.phy = port,
		.mmd = devadd,
		.reg = reg & 0xffff,
	};
	int ret;

	ret = MXL862XX_API_READ(priv, INT_GPHY_READ, param);
	if (ret) {
		pr_err("mxl862xx: failed to read mmd on port %d\n", port);
		return ret;
	}

	return param.data;
}

static int mxl862xx_phy_write_mmd(struct mxl862xx_priv *priv, int port,
				  int devadd, int reg, u16 data)
{
	struct mdio_relay_data param = {
		.phy = port,
		.mmd = devadd,
		.reg = reg,
		.data = data,
	};
	int ret;

	ret = MXL862XX_API_WRITE(priv, INT_GPHY_WRITE, param);
	if (ret)
		pr_err("mxl862xx: failed to write mmd on port %d\n", port);

	return ret;
}

static int mxl862xx_phy_read(struct dsa_switch *ds, int port, int reg)
{
	return mxl862xx_phy_read_mmd(ds->priv, port, 0, reg);
}

static int mxl862xx_phy_write(struct dsa_switch *ds, int port, int reg,
			      u16 data)
{
	return mxl862xx_phy_write_mmd(ds->priv, port, 0, reg, data);
}

static int mxl862xx_update_bridge_conf_port(struct dsa_switch *ds, u8 port,
					    struct net_device *bridge, int action)
{
	struct mxl862xx_priv *priv = ds->priv;
	u8 max_ports = priv->hw_info->max_ports;
	u8 cpu_port = priv->cpu_port;
	u8 i;
	bool vlan_sp_tag = (priv->port_info[cpu_port].tag_protocol == DSA_TAG_PROTO_MXL862_8021Q);

	/*struct dsa_port *dp = dsa_to_port(priv->ds, port), *other_dp;
	 * struct mxl862xx_port_info *other_p;
	 *int other_port;
	 */
	int ret;

	/* Update local bridge port map */
	for (i = 0; i < max_ports; i++) {
		int bridge_id = priv->port_info[i].bridge_id;

		if (dsa_is_unused_port(ds, i))
			continue;

		/* CPU port is assigned to all bridges and cannot be modified  */
		if ((dsa_is_cpu_port(ds, i)))
			continue;

		/* Skip if bridge does not match, except the self port assignment  */
		if ((dsa_port_bridge_dev_get(dsa_to_port(ds, i)) != bridge) && (i != port))
			continue;

		/* Case for standalone bridges assigned only to single user and CPU ports.
		 * Used only for initial ports isolation */
		if ((bridge == NULL) && (i != port))
			continue;

		if (action)
			priv->bridge_portmap[bridge_id] |= BIT(DSA_MXL_PORT(port));
		else
			priv->bridge_portmap[bridge_id] &= ~BIT(DSA_MXL_PORT(port));
	}

	/* Update switch according to local bridge port map */
	/* Add this port to the port maps of other ports skiping it's own map */
	for (i = 0; i < max_ports; i++) {
		struct mxl862xx_bridge_port_config br_port_cfg = { };
		int bridge_id = priv->port_info[i].bridge_id;

		if (!(dsa_is_user_port(ds, i)))
			continue;

		/* Case for standalone bridges assigned only to single user and CPU ports.
		 * Used only for initial ports isolation
		 */
		if (!bridge && i != port)
			continue;

		/* Do not reconfigure any standalone bridge if this is bridge join scenario */
		if (bridge && !priv->port_info[i].bridge)
			continue;

		br_port_cfg.bridge_port_id = DSA_MXL_PORT(i);
		br_port_cfg.mask |=
			MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP |
			MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID;

		ret = MXL862XX_API_READ(priv, MXL862XX_BRIDGEPORT_CONFIGGET, br_port_cfg);
		if (ret) {
			dev_err(ds->dev, "failed to set bridge port configuration on port %d\n",
				port);
			return ret;
		}

		/* Skip port map update if for the existing bridge the port
		 * is not assigned there.
		 */
		if (i != port && (br_port_cfg.bridge_id != bridge_id ||
				  br_port_cfg.bridge_id == 0))
			continue;

		br_port_cfg.mask |=
			MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP |
			MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID |
			MXL862XX_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING |
			MXL862XX_BRIDGE_PORT_CONFIG_MASK_VLAN_BASED_MAC_LEARNING;

		/* Skip the port itself in it's own portmap */
		br_port_cfg.bridge_port_map[0] =
			priv->bridge_portmap[bridge_id] & ~(BIT(DSA_MXL_PORT(i)));

		if (action) {
			br_port_cfg.src_mac_learning_disable = !bridge;
			br_port_cfg.vlan_src_mac_vid_enable = br_port_cfg.vlan_dst_mac_vid_enable =
				(vlan_sp_tag) ? false : (bridge != NULL);
			br_port_cfg.bridge_id = bridge_id;
		} else {
			/* When port is removed from the bridge, assign it back to the default
			 * bridge 0
			 */
			br_port_cfg.src_mac_learning_disable = true;
			br_port_cfg.vlan_src_mac_vid_enable = br_port_cfg.vlan_dst_mac_vid_enable = false;
			/* Cleanup the port own map leaving only the CPU port mapping. */
			if (i == port) {
				br_port_cfg.bridge_port_map[0] = BIT(DSA_MXL_PORT(cpu_port));
				br_port_cfg.bridge_id = 0;
			}
		}

		ret = MXL862XX_API_WRITE(priv, MXL862XX_BRIDGEPORT_CONFIGSET, br_port_cfg);
		if (ret) {
			dev_err(ds->dev, "failed to set bridge port configuration on port %d\n", port);
			return ret;
		}
	}

	/* Configure additional bridge port for VLAN based tagging */
	if (vlan_sp_tag) {
		int bridge_id = priv->port_info[port].bridge_id;
		uint16_t bridge_port_cpu = DSA_MXL_PORT(port) + 16;
		struct mxl862xx_bridge_port_alloc bpa_param = { };
		struct mxl862xx_bridge_port_config br_port_cfg = { };

		bpa_param.bridge_port_id = bridge_port_cpu;

		if (action) {
			/* add */
			ret = MXL862XX_API_READ(priv, MXL862XX_BRIDGEPORT_ALLOC, bpa_param);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to prepare associated bridge port\n",
					__func__, port);
				return ret;
			}

			br_port_cfg.mask |=
				MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP |
				MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID |
				MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING |
				MXL862XX_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING;
			br_port_cfg.bridge_id = bridge_id;
			br_port_cfg.bridge_port_id = bridge_port_cpu;
			br_port_cfg.bridge_port_map[0] = BIT(DSA_MXL_PORT(port));
			br_port_cfg.dest_logical_port_id = DSA_MXL_PORT(cpu_port);
			br_port_cfg.src_mac_learning_disable = true;

			ret = MXL862XX_API_WRITE(priv, MXL862XX_BRIDGEPORT_CONFIGSET, br_port_cfg);
			if (ret) {
				dev_err(ds->dev,
				"%s: Configuration of cpu bridge port:%d  for port:%d on bridge:%d failed with ret=%d\n",
				__func__, bridge_port_cpu, port, bridge_id, ret);
				return ret;
			}

			/* Add bridge cpu port to portmap */
			priv->bridge_portmap[bridge_id] |= BIT(bridge_port_cpu);
			priv->port_info[port].bridge_port_cpu = bridge_port_cpu;
		} else {
			/* remove */
			ret = MXL862XX_API_WRITE(priv, MXL862XX_BRIDGEPORT_FREE, bpa_param);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to free associated bridge port\n",
					__func__, port);
				return ret;
			}
			/* Remove bridge cpu port from portmap */
			priv->bridge_portmap[bridge_id] &= ~BIT(bridge_port_cpu);
			priv->port_info[port].bridge_port_cpu = 0;
		}
	}

	return 0;
}

static enum dsa_tag_protocol mxl862_parse_tag_proto(struct dsa_switch *ds, uint8_t port)
{
	/* Default value if no dt entry found */
	enum dsa_tag_protocol tag_proto = DSA_TAG_PROTO_MXL862_8021Q;
	struct dsa_port *dp = (struct dsa_port *)dsa_to_port(ds, port);
	const char *user_protocol = NULL;

	if (dp != NULL)
		user_protocol = of_get_property(dp->dn, "dsa-tag-protocol", NULL);
	if (user_protocol != NULL) {
		if (strcmp("mxl862_8021q", user_protocol) == 0)
			tag_proto = DSA_TAG_PROTO_MXL862_8021Q;
	}
	return tag_proto;
}

static int mxl862xx_port_state(struct dsa_switch *ds, int port, bool enable)
{
	struct mxl862xx_register_mod sdma_param = {
		.addr = MXL862XX_SDMA_PCTRLP(DSA_MXL_PORT(port)),
		.data = enable ? MXL862XX_SDMA_PCTRL_EN : 0,
		.mask = MXL862XX_SDMA_PCTRL_EN,
	};
	struct mxl862xx_register_mod fdma_param = {
		.addr = MXL862XX_FDMA_PCTRLP(DSA_MXL_PORT(port)),
		.data = enable ? MXL862XX_FDMA_PCTRL_EN : 0,
		.mask = MXL862XX_FDMA_PCTRL_EN,
	};
	struct mxl862xx_priv *priv = ds->priv;
	int ret;

	if (!dsa_is_user_port(ds, port))
		return 0;

	if (enable && dsa_is_cpu_port(ds, port))
		return 0;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_COMMON_REGISTERMOD, sdma_param);
	if (ret) {
		dev_err(ds->dev, "failed to %s SDMA on port %d\n", (enable)?"enable":"disable", port);
		return ret;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_COMMON_REGISTERMOD, fdma_param);
	if (ret) {
		dev_err(ds->dev, "failed to %s FDMA on port %d\n", (enable)?"enable":"disable", port);
		return ret;
	}

	priv->port_info[port].port_enabled = enable;

	return 0;
}

static int mxl862xx_port_enable(struct dsa_switch *ds, int port, struct phy_device *phydev)
{
	return mxl862xx_port_state(ds, port, true);
}

static void mxl862xx_port_disable(struct dsa_switch *ds, int port)
{
	mxl862xx_port_state(ds, port, false);
}

static void mxl862xx_port_fast_age(struct dsa_switch *ds, int port)
{
	struct mxl862xx_mac_table_clear param = {
		.type = MXL862XX_MAC_CLEAR_PHY_PORT,
		.port_id = DSA_MXL_PORT(port),
	};

	if (MXL862XX_API_WRITE(ds->priv, MXL862XX_MAC_TABLECLEARCOND, param))
		dev_err(ds->dev, "failed to clear fdb on port %d\n", port);
}

static int get_vlan_vid_filters_idx(struct mxl862xx_priv *priv, uint8_t port, bool ingress,
		uint16_t vid, int *f_0, int *f_1, uint16_t *vlan_idx)
{
	int ret = -EINVAL;
	int x, i = 0;
	/* negative values if not found */
	int filter_0 = -1;
	int filter_1 = -1;
	struct mxl862xx_extended_vlan_block_info *block_info;

	if (ingress)
		block_info = &(priv->port_info[port].vlan.ingress_vlan_block_info);
	else
		block_info = &(priv->port_info[port].vlan.egress_vlan_block_info);

	/* Check if there's active entry for the requested VLAN. If found, overwrite it. */
	if (filter_0 < 0 && filter_1 < 0) {
		for (i = 0; i < MAX_VLANS; i++) {
			if (block_info->vlans[i].vid == vid) {
				filter_0 = block_info->vlans[i].filters_idx[0];
				filter_1 = block_info->vlans[i].filters_idx[1];
				ret = 0;
				break;
			}
		}
	}

	/* If there are no matching active VLAN entries, check in recycled */
	if (filter_0 < 0 && filter_1 < 0) {
	/* check if there are recycled filter entries for use */
		for (x = 0; x < MAX_VLANS; x++) {
			if (block_info->filter_entries_recycled[x].valid) {
				filter_0 = block_info->filter_entries_recycled[x].filters_idx[0];
				filter_1 = block_info->filter_entries_recycled[x].filters_idx[1];
				/* remove filter entries from recycled inventory */
				block_info->filter_entries_recycled[x].valid = false;
				ret = 0;
				break;
			}
		}

		/* find empty slot for storing ID's of vlan filtering rules */
		for (i = 0; i < MAX_VLANS; i++) {
			if (!(block_info->vlans[i].used)) {
				ret = 0;
				break;
			}
			if (i == priv->max_vlans - 1) {
				dev_err(priv->dev,
					"%s: Port:%d reached max number of defined VLAN's: %d\n",
					__func__, port, priv->max_vlans);
				return -ENOSPC;
			}
		}
	}

	if (f_0 != NULL)
		*f_0 = filter_0;
	if (f_1 != NULL)
		*f_1 = filter_1;
	if (vlan_idx != NULL)
		*vlan_idx = i;

	return ret;
}

static int deactivate_vlan_filter_entry(struct dsa_switch *ds, u16 block_id, u16 entry_idx)
{
	struct mxl862xx_extendedvlan_config vlan_cfg = { };
	struct mxl862xx_priv *priv = ds->priv;
	int ret;

	/* Set default reset values as it makes the rule transparent */
	vlan_cfg.extended_vlan_block_id = block_id;
	vlan_cfg.entry_index = entry_idx;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.priority_enable = true;
	vlan_cfg.filter.outer_vlan.priority_val = 0;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = 0;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.priority_enable = true;
	vlan_cfg.filter.inner_vlan.priority_val = 0;
	vlan_cfg.filter.inner_vlan.vid_enable = true;
	vlan_cfg.filter.inner_vlan.vid_val = 0;
	vlan_cfg.treatment.add_outer_vlan = true;
	vlan_cfg.treatment.outer_vlan.priority_mode = MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.outer_vlan.priority_val = 0;
	vlan_cfg.treatment.add_inner_vlan = true;
	vlan_cfg.treatment.inner_vlan.priority_mode = MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.inner_vlan.priority_val = 0;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		pr_err("%s: failed to deactivate entry:%d for VLAN block ID:%d\n",
			__func__, vlan_cfg.entry_index, vlan_cfg.extended_vlan_block_id);

	return ret;
}

static int mxl862xx_allocate_extended_vlan_block(struct mxl862xx_priv *priv,
						 struct mxl862xx_extended_vlan_block_info *block_info)
{
	int ret;

	if (!block_info->allocated) {
		struct mxl862xx_extendedvlan_alloc vlan_alloc = { };

		vlan_alloc.number_of_entries = block_info->filters_max;
		ret = MXL862XX_API_READ(priv, MXL862XX_EXTENDEDVLAN_ALLOC, vlan_alloc);
		if (ret)
			return ret;

		block_info->allocated = true;
		block_info->block_id = vlan_alloc.extended_vlan_block_id;
	}

	return 0;
}

static int prepare_vlan_egress_filters_off_sp_tag_no_vid(struct dsa_switch *ds, uint8_t port)
{
	struct mxl862xx_extendedvlan_config vlan_cfg = { };
	struct mxl862xx_priv *priv = ds->priv;
	int ret = -EINVAL;

	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[port].vlan.egress_vlan_block_info);
	if (ret)
		return ret;

	// Static entry :  Outer and iner tag.
	// Remove outer tag  one as it must be sp_tag. Transparent for inner tag.
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.egress_vlan_block_info.filters_max - 2;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	/* remove  sp tag */
	vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	// Last entry :  Only outer tag. Remove it as it must be sp_tag
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.egress_vlan_block_info.filters_max - 1;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	/* remove  sp tag */
	vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	return 0;
}

static int prepare_vlan_egress_filters_off_sp_tag(struct dsa_switch *ds, uint8_t port, uint16_t vid, bool untagged)
{
	int ret = -EINVAL;
	uint16_t idx = 0;
	/* negative values if not found */
	int filter_0 = -1;
	int filter_1 = -1;
	struct mxl862xx_priv *priv = ds->priv;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[port].vlan.egress_vlan_block_info);
	if (ret)
		return ret;

	/* VID specific entries must be processed before the final entries,
	 * so putting them at the beginnig of the block */

	ret = get_vlan_vid_filters_idx(priv, port, false, vid, &filter_0, &filter_1, &idx);
	dev_dbg(priv->dev, "%s: Port:%d  vid:%d f_0:%d f_1:%d idx:%d\n", __func__, port, vid, filter_0, filter_1, idx);
	if (ret)
		return ret;

	// Entry 0 :  Outer and Inner tags are present. Inner tag matching vid.
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_0 >= 0 ?
			filter_0 :
			priv->port_info[port]
				.vlan.egress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.vid_enable = true;
	vlan_cfg.filter.inner_vlan.vid_val = vid;

	if (untagged) {
		/* remove both sp_tag(outer) and vid (inner) */
		vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG;
	} else {
		/* remove only sp tag */
		vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[port]
		.vlan.egress_vlan_block_info.vlans[idx]
		.filters_idx[0] = vlan_cfg.entry_index;

	priv->port_info[port]
		.vlan.egress_vlan_block_info.vlans[idx]
		.filters_idx[1] = IDX_INVAL;

	// Static entry :  Outer and iner tag, not matching vid. Remove outer tag  one as it must be sp_tag
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.egress_vlan_block_info.filters_max - 2;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	/* remove  sp tag */
	vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	// Last entry :  Only outer tag. Remove it as it must be sp_tag
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.egress_vlan_block_info.filters_max - 2;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	/* remove  sp tag */
	vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	// Last entry :  Only outer tag. Remove it as it must be sp_tag
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.egress_vlan_block_info.filters_max - 1;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	/* remove  sp tag */
	vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index ;


	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].vid = vid;
	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].used = true;
	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].untagged = untagged;

	return 0;
}

static int prepare_vlan_egress_filters_off(struct mxl862xx_priv *priv, uint8_t port, uint16_t vid, bool untagged)
{
	int ret = -EINVAL;
	uint16_t idx = 0;
	/* negative values if not found */
	int filter_0 = -1;
	int filter_1 = -1;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[port].vlan.egress_vlan_block_info);
	if (ret)
		return ret;

	/* VID specific entries must be processed before the final entries,
	 * so putting them at the beginnig of the block */

	ret = get_vlan_vid_filters_idx(priv, port, false, vid, &filter_0, &filter_1, &idx);
	if (ret)
		return ret;

	// Entry 0 : ACCEPT VLAN tags that are matching  VID. Outer and Inner tags are present
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_0 >= 0 ?
			filter_0 :
			priv->port_info[port]
				.vlan.egress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.vid_enable = true;
	vlan_cfg.filter.inner_vlan.vid_val = vid;
	if (untagged)
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
	else
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[port]
		.vlan.egress_vlan_block_info.vlans[idx]
		.filters_idx[0] = vlan_cfg.entry_index;

	//	 Entry 1 : ACCEPT VLAN tags that are matching PVID or port VID. Only the outer tags are present
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_1 >= 0 ?
			filter_1 :
			priv->port_info[port]
				.vlan.egress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.filter.inner_vlan.vid_enable = true;
	vlan_cfg.filter.inner_vlan.vid_val = vid;
	if (untagged)
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
	else
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[port]
		.vlan.egress_vlan_block_info.vlans[idx]
		.filters_idx[1] = vlan_cfg.entry_index;

	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].vid = vid;
	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].used = true;
	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].untagged = untagged;


	return ret;
}


static int prepare_vlan_ingress_filters_off_sp_tag_no_vid(struct dsa_switch *ds, uint8_t port)
{
	struct mxl862xx_priv *priv = ds->priv;
	struct dsa_port *dp = dsa_to_port(ds, port);
	int ret = -EINVAL;
	struct mxl862xx_extended_vlan_block_info *block_info =
		&priv->port_info[port].vlan.ingress_vlan_block_info;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv, block_info);
	if (ret)
		return ret;

	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;

	//Static rules. No tags, add SP tag
	vlan_cfg.entry_index = block_info->filters_max - 3;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.add_outer_vlan = true;
	vlan_cfg.treatment.outer_vlan.vid_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
	vlan_cfg.treatment.outer_vlan.tpid =
		MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
	vlan_cfg.treatment.outer_vlan.priority_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.outer_vlan.priority_val = 0;
	vlan_cfg.treatment.outer_vlan.dei =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
	vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	block_info->final_filters_idx = vlan_cfg.entry_index;

	// Static rules
	// Single tag. Use transparent mode. Add sp tag
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	vlan_cfg.entry_index = block_info->filters_max - 2;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.add_outer_vlan = true;
	vlan_cfg.treatment.outer_vlan.vid_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
	vlan_cfg.treatment.outer_vlan.tpid =
		MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
	vlan_cfg.treatment.outer_vlan.priority_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.outer_vlan.priority_val = 0;
	vlan_cfg.treatment.outer_vlan.dei =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
	vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	block_info->final_filters_idx = vlan_cfg.entry_index;

	// Two tags. Use transparent mode. Do not apply vid as this is tagged pkt
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	vlan_cfg.entry_index = block_info->filters_max - 1;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.treatment.add_outer_vlan = true;
	vlan_cfg.treatment.outer_vlan.vid_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
	vlan_cfg.treatment.outer_vlan.tpid =
		MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
	vlan_cfg.treatment.outer_vlan.priority_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.outer_vlan.priority_val = 0;
	vlan_cfg.treatment.outer_vlan.dei =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
	vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	block_info->final_filters_idx = vlan_cfg.entry_index;

	return ret;
}

static int prepare_vlan_ingress_filters_off_sp_tag(struct dsa_switch *ds, uint8_t port, uint16_t vid)
{
	struct mxl862xx_priv *priv = ds->priv;
	struct dsa_port *dp = dsa_to_port(ds, port);
	int ret = -EINVAL;
	//there's possible only one rule for single pvid, so it always uses idx 0
	uint16_t idx = 0;
	struct mxl862xx_extended_vlan_block_info *block_info =
		&priv->port_info[port].vlan.ingress_vlan_block_info;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv, block_info);
	if (ret)
		return ret;

	/*  If port has pvid then add vid dependand dynamic rule.
	 *  It's done that way because it's required for proper handling of
	 *  vlan delete scenario. If no pvid configured, create 'static' rule */
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;

	// vid dynamic rule
	if (priv->port_info[port].vlan.pvid) {
		/* As there's only one  pvid  per port possible, always overwrite the rule at position 0 */
		vlan_cfg.entry_index = 0;
		vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		vlan_cfg.treatment.add_outer_vlan = true;
		vlan_cfg.treatment.outer_vlan.vid_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vlan_cfg.treatment.outer_vlan.tpid =
			MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
		vlan_cfg.treatment.outer_vlan.priority_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vlan_cfg.treatment.outer_vlan.priority_val = 0;
		vlan_cfg.treatment.outer_vlan.dei =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
		vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);
		vlan_cfg.treatment.add_inner_vlan = true;
		vlan_cfg.treatment.inner_vlan.vid_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vlan_cfg.treatment.inner_vlan.vid_val =
			priv->port_info[port].vlan.pvid;
		vlan_cfg.treatment.inner_vlan.tpid =
			MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
		vlan_cfg.treatment.inner_vlan.priority_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vlan_cfg.treatment.inner_vlan.priority_val = 0;
		vlan_cfg.treatment.inner_vlan.dei =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;

		ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
		if (ret) {
			dev_err(priv->dev,
				"%s: Port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
				__func__, port, vlan_cfg.entry_index,
				vlan_cfg.extended_vlan_block_id);
			return ret;
		}

		block_info->vlans[idx].filters_idx[0] = vlan_cfg.entry_index;
		block_info->vlans[idx].filters_idx[1] = IDX_INVAL;

		block_info->vlans[idx].vid = vid;
		block_info->vlans[idx].used = true;

	}
	// no pvid, static rule
	else {
		// deactivate possible dynamic rule if there's no pvid
		if (block_info->vlans[idx].vid) {
			ret = deactivate_vlan_filter_entry(ds, block_info->block_id, block_info->vlans[idx].filters_idx[0]);
			if (ret)
				return ret;
			block_info->vlans[idx].vid = 0;
			block_info->vlans[idx].used = false;
		}

		vlan_cfg.entry_index = block_info->filters_max - 3;
		vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		vlan_cfg.treatment.add_outer_vlan = true;
		vlan_cfg.treatment.outer_vlan.vid_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vlan_cfg.treatment.outer_vlan.tpid =
			MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
		vlan_cfg.treatment.outer_vlan.priority_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vlan_cfg.treatment.outer_vlan.priority_val = 0;
		vlan_cfg.treatment.outer_vlan.dei =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
		vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);

		ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
		if (ret) {
			dev_err(priv->dev,
				"%s: Port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
				__func__, port, vlan_cfg.entry_index, vlan_cfg.extended_vlan_block_id);
			return ret;
		}

		block_info->final_filters_idx = vlan_cfg.entry_index;
	}

	// Static rules
	// Single tag. Use transparent mode. Do not apply PVID as this is the tagged traffic
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	vlan_cfg.entry_index = block_info->filters_max - 2;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.add_outer_vlan = true;
	vlan_cfg.treatment.outer_vlan.vid_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
	vlan_cfg.treatment.outer_vlan.tpid =
		MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
	vlan_cfg.treatment.outer_vlan.priority_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.outer_vlan.priority_val = 0;
	vlan_cfg.treatment.outer_vlan.dei =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
	vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	block_info->final_filters_idx = vlan_cfg.entry_index;

	// Two tags. Use transparent mode. Do not apply vid as this is tagged pkt
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	vlan_cfg.entry_index = block_info->filters_max - 1;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.treatment.add_outer_vlan = true;
	vlan_cfg.treatment.outer_vlan.vid_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
	vlan_cfg.treatment.outer_vlan.tpid =
		MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
	vlan_cfg.treatment.outer_vlan.priority_mode =
		MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.outer_vlan.priority_val = 0;
	vlan_cfg.treatment.outer_vlan.dei =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
	vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	block_info->final_filters_idx = vlan_cfg.entry_index;

	return ret;
}

static int prepare_vlan_ingress_filters_off(struct mxl862xx_priv *priv, uint8_t port, uint16_t vid)
{
	int ret = -EINVAL;
	uint16_t idx = 0;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[port].vlan.ingress_vlan_block_info);
	if (ret)
		return ret;

	// Entry 4  untagged pkts. If there's PVID accept and add PVID tag
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	/* for cpu port this entry is fixed and always put at the end of the block */
	if (port == priv->cpu_port)
		vlan_cfg.entry_index =  priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 1;
	else {
		/* for vlan filter off, this entry is fixed and always put at the end of the block */
		vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 1;
		priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;
	}
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;

	if (priv->port_info[port].vlan.pvid) {
		vlan_cfg.treatment.add_outer_vlan = true;
		vlan_cfg.treatment.outer_vlan.vid_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vlan_cfg.treatment.outer_vlan.vid_val =
			priv->port_info[port].vlan.pvid;
		vlan_cfg.treatment.outer_vlan.tpid =
			MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
		vlan_cfg.treatment.outer_vlan.priority_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vlan_cfg.treatment.outer_vlan.priority_val = 0;
		vlan_cfg.treatment.outer_vlan.dei =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	ret = get_vlan_vid_filters_idx(priv, port, true, vid, NULL, NULL, &idx);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d couldn't get idx for VID:%d and  block ID:%d\n",
			__func__, port, vid, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.ingress_vlan_block_info.vlans[idx].vid = vid;
	priv->port_info[port].vlan.ingress_vlan_block_info.vlans[idx].used = true;

	return ret;
}

static int mxl862xx_port_vlan_filtering(struct dsa_switch *ds, int port,
					bool vlan_filtering,
					struct netlink_ext_ack *extack)
{
	int ret = 0;
	struct mxl862xx_priv *priv = ds->priv;
	struct dsa_port *dsa_port = dsa_to_port(ds, port);
	struct net_device *bridge = dsa_port_bridge_dev_get(dsa_port);

	/* Prevent dynamic setting of the vlan_filtering. */
	if (bridge && priv->port_info[port].vlan.filtering_mode_locked) {
		ret = -ENOTSUPP;
		dev_err(ds->dev, "%s: Change of vlan_filtering mode is not allowed while port:%d is joined to a bridge\n",
				__func__, port);
				NL_SET_ERR_MSG_MOD(extack, "Change of vlan_filtering mode is not allowedwhile port is joind to a bridge.");

	} else {
		priv->port_info[port].vlan.filtering = vlan_filtering;
		/* Do not lock if port is isolated. */
		if (!priv->port_info[port].isolated)
			priv->port_info[port].vlan.filtering_mode_locked = true;
	}

	return ret;
}

static int prepare_vlan_egress_filters(struct dsa_switch *ds, uint8_t port, uint16_t vid, bool untagged)
{
	int ret = -EINVAL;
	uint16_t idx = 0;
	/* negative values if not found */
	int filter_0 = -1;
	int filter_1 = -1;
	struct mxl862xx_priv *priv = ds->priv;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };
	struct mxl862xx_extended_vlan_block_info *block_info =
		&priv->port_info[port].vlan.egress_vlan_block_info;

	ret = mxl862xx_allocate_extended_vlan_block(priv, block_info);
	if (ret)
		return ret;

	/* First populate the block with set of rules which should be executed finally after
	 * VID specific filtering. The final rules (not related to VID) are placed on the end of the block. The number of
	 * rules is fixed  per port. Order of execution  is important. To avoid static reservations they are
	 * stored in reversed order starting from the end of the block */

	//Entry 4: no outer/inner tag, no PVID  DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	vlan_cfg.entry_index = block_info->filters_max - 1;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	block_info->final_filters_idx = vlan_cfg.entry_index;

	//Entry 3: Only Outer tag present. Discard if VID is not matching the previous rules
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	vlan_cfg.entry_index = block_info->filters_max - 2;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	block_info->final_filters_idx = vlan_cfg.entry_index;

	//Entry 2: Outer and Inner tags are present. Discard if VID is not matching the previous rules
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	vlan_cfg.entry_index = block_info->filters_max - 3;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	block_info->final_filters_idx = vlan_cfg.entry_index;

	/* VID specific entries must be processed before the final entries,
	 * so putting them at the beginnig of the block */
	ret = get_vlan_vid_filters_idx(priv, port, false, vid, &filter_0, &filter_1, &idx);
	if (ret)
		return ret;

	// Entry 0 : ACCEPT VLAN tags that are matching  VID. Outer and Inner tags are present
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_0 >= 0 ? filter_0 : block_info->vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = vid;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;

	if (untagged)
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
	else
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	block_info->vlans[idx].filters_idx[0] = vlan_cfg.entry_index;

	//	 Entry 1 : ACCEPT VLAN tags that are matching PVID or port VID. Only the outer tags are present
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_1 >= 0 ? filter_1 : block_info->vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = vid;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	if (untagged)
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
	else
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	block_info->vlans[idx].filters_idx[1] = vlan_cfg.entry_index;

	block_info->vlans[idx].vid = vid;
	block_info->vlans[idx].used = true;
	block_info->vlans[idx].untagged = untagged;

	return ret;
}

static int prepare_vlan_egress_filters_sp_tag(struct dsa_switch *ds, uint8_t port, uint16_t vid, bool untagged)
{
	int ret = -EINVAL;
	uint16_t idx = 0;
	/* negative values if not found */
	int filter_0 = IDX_INVAL;
	int filter_1 = IDX_INVAL;
	struct mxl862xx_priv *priv = ds->priv;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[port].vlan.egress_vlan_block_info);
	if (ret)
		return ret;

	/* First populate the fixed block of rules which should be executed finally after
	 * VID specific filtering. The final rules (not related to VID)
	 * are placed at the end of the block. */

	//Entry 4: only outer tag (SP tag), no PVID  DISCARD
	if (untagged) {
		memset(&vlan_cfg, 0, sizeof(vlan_cfg));
		vlan_cfg.extended_vlan_block_id =
			priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	   vlan_cfg.entry_index = priv->port_info[port].vlan.egress_vlan_block_info.filters_max - 1;
		vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

		ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
		if (ret) {
			dev_err(priv->dev,
				"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
				__func__, port, vlan_cfg.entry_index,
				vlan_cfg.extended_vlan_block_id);
			return ret;
		}

		priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx =
			vlan_cfg.entry_index;
	}

	//Entry 3: there is any other inner tag -> discard upstream traffic
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.egress_vlan_block_info.filters_max - 2;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.remove_tag =	MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx =
		vlan_cfg.entry_index;

	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.egress_vlan_block_info.filters_max - 3;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	/* VID specific entries must be processed before the final entries,
	 * so putting them at the beginnig of the block */
	ret = get_vlan_vid_filters_idx(priv, port, false, vid, &filter_0, &filter_1, &idx);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d couldn't get idx for VID specific filters for VID:%d and  block ID:%d\n",
			__func__, port, vid, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	/* Set of dynamic rules that depend on VID.
	 * The number of rules depends on the number of handled vlans */

	// Entry 0 : ACCEPT VLAN tags that are matching  VID. Outer and Inner tags are present
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_0 >= 0 ?
			filter_0 :
			priv->port_info[port]
				.vlan.egress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.vid_enable = true;
	vlan_cfg.filter.inner_vlan.vid_val = vid;

	if (untagged) {
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG;
	} else {
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[port]
		.vlan.egress_vlan_block_info.vlans[idx]
		.filters_idx[0] = vlan_cfg.entry_index;

	/* mark as unused */
	priv->port_info[port]
		.vlan.egress_vlan_block_info.vlans[idx]
		.filters_idx[1] = IDX_INVAL;

	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].vid = vid;
	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].used = true;
	priv->port_info[port].vlan.egress_vlan_block_info.vlans[idx].untagged = untagged;

	return ret;
}


static int prepare_vlan_egress_filters_off_sp_tag_cpu(struct dsa_switch *ds, uint8_t cpu_port)
{
	int ret = -EINVAL;
	struct mxl862xx_priv *priv = ds->priv;
	struct mxl862xx_extendedvlan_config vlan_cfg = { };
	struct mxl862xx_extended_vlan_block_info *block_info =
		&priv->port_info[cpu_port].vlan.egress_vlan_block_info;

	ret = mxl862xx_allocate_extended_vlan_block(priv, block_info);
	if (ret)
		return ret;

	// Entry last - 1  : Outer and Inner tags are present.
	// Transparent mode, no tag modifications
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;

	vlan_cfg.entry_index = block_info->filters_max - 2;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, cpu_port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	block_info->final_filters_idx = vlan_cfg.entry_index;

	// Entry last : Outer tag is present.
	// Transparent mode, no tag modifications
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id = block_info->block_id;

	vlan_cfg.entry_index = block_info->filters_max - 1;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, cpu_port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	block_info->final_filters_idx = vlan_cfg.entry_index;

	return ret;
}

static int prepare_vlan_egress_filters_sp_tag_cpu(struct dsa_switch *ds, uint8_t cpu_port, uint16_t vid, bool untagged)
{
	int ret = -EINVAL;
	struct mxl862xx_priv *priv = ds->priv;
	int filter_0 = -1;
	int filter_1 = -1;
	uint16_t idx = 0;
	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[cpu_port].vlan.egress_vlan_block_info);
	if (ret)
		return ret;

	/* Populate the  block of rules related to VID */

	/* VID specific entries must be processed before the final entries,
	 * so putting them at the beginnig of the block */
	ret = get_vlan_vid_filters_idx(priv, cpu_port, false, vid, &filter_0, &filter_1, &idx);

	if (ret)
		return ret;

	// Entry 0 : Outer and Inner tags are present. If user port is untagged
	// remove inner tag if the outer tag is matching the user port
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[cpu_port].vlan.egress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_0 >= 0 ?
			filter_0 :
			priv->port_info[cpu_port]
				.vlan.egress_vlan_block_info.vid_filters_idx++;

	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.vid_enable = true;
	vlan_cfg.filter.inner_vlan.vid_val = vid;

	if (untagged) {
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG;
		vlan_cfg.treatment.add_outer_vlan = true;
		vlan_cfg.treatment.outer_vlan.vid_mode = MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_VID;
		vlan_cfg.treatment.outer_vlan.tpid = MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
		vlan_cfg.treatment.outer_vlan.priority_mode = MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY;
		vlan_cfg.treatment.outer_vlan.dei = MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_DEI;
	}	else {
		vlan_cfg.treatment.remove_tag = MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for egress extended VLAN block ID:%d\n",
			__func__, cpu_port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[cpu_port]
		.vlan.egress_vlan_block_info.vlans[idx]
		.filters_idx[0] = vlan_cfg.entry_index;

   /* mark as unused */
	priv->port_info[cpu_port]
		.vlan.egress_vlan_block_info.vlans[idx]
		.filters_idx[1] = IDX_INVAL;

	priv->port_info[cpu_port].vlan.egress_vlan_block_info.vlans[idx].vid = vid;
	priv->port_info[cpu_port].vlan.egress_vlan_block_info.vlans[idx].untagged = untagged;
	priv->port_info[cpu_port].vlan.egress_vlan_block_info.vlans[idx].used = true;

	return ret;

}

static int prepare_vlan_ingress_filters_sp_tag(struct dsa_switch *ds, uint8_t port, uint16_t vid)
{
	struct dsa_port *dp = dsa_to_port(ds, port);
	int ret = -EINVAL;
	uint16_t idx = 0;
	/* negative values if not found */
	int filter_0 = -1;
	int filter_1 = -1;
	struct mxl862xx_priv *priv = ds->priv;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[port].vlan.ingress_vlan_block_info);
	if (ret)
		return ret;

	/* First populate the fixed block of rules which should be executed finally after
	 * VID specific filtering. The final rules (not related to VID)
	 * are placed at the end of the block. */

	//Entry 6 no other rule applies Outer tag default Inner tag  not present DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 1;
	vlan_cfg.filter.outer_vlan.type =
		MXL862XX_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	//Entry 5 no other rule applies Outer tag default Inner tag  present DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 2;
	vlan_cfg.filter.outer_vlan.type =
		MXL862XX_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	// Entry 4  untagged pkts. If there's PVID accept and add PVID tag, otherwise reject
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 3;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	if (!priv->port_info[port].vlan.pvid) {
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;
	} else {
		vlan_cfg.treatment.add_outer_vlan = true;
		vlan_cfg.treatment.outer_vlan.vid_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vlan_cfg.treatment.outer_vlan.tpid =
			MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
		vlan_cfg.treatment.outer_vlan.priority_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vlan_cfg.treatment.outer_vlan.priority_val = 0;
		vlan_cfg.treatment.outer_vlan.dei =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
		vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);
		vlan_cfg.treatment.add_inner_vlan = true;
		vlan_cfg.treatment.inner_vlan.vid_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vlan_cfg.treatment.inner_vlan.vid_val =
			priv->port_info[port].vlan.pvid;
		vlan_cfg.treatment.inner_vlan.tpid =
			MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
		vlan_cfg.treatment.inner_vlan.priority_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vlan_cfg.treatment.inner_vlan.priority_val = 0;
		vlan_cfg.treatment.inner_vlan.dei =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add PVID entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx =
		vlan_cfg.entry_index;

	// Entry 3 : Only Outer tag present : not matching  DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 4;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add DISCARD entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx =
		vlan_cfg.entry_index;

	// Entry 2 : Outer and Inner VLAN tag present : not matching  DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 5;

	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add DISCARD entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	/* VID specific filtering rules which should be executed first before final ones.
	 * Storing starts at the beginning of the block. */

	ret = get_vlan_vid_filters_idx(priv, port, true, vid, &filter_0, &filter_1, &idx);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d couldn't get idx for VID specific filters for VID:%d and  block ID:%d\n",
			__func__, port, vid, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	// Entry 0 : Outer and Inner VLAN tag present :  matching  ACCEPT
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_0 >= 0 ?
			filter_0 :
			priv->port_info[port]
				.vlan.ingress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = vid;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.add_outer_vlan = true;
	vlan_cfg.treatment.outer_vlan.vid_mode = MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
	vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);
	vlan_cfg.treatment.outer_vlan.tpid = MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
	vlan_cfg.treatment.outer_vlan.priority_mode = MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.outer_vlan.priority_val = 0;
	vlan_cfg.treatment.outer_vlan.dei = MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for block_id:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[port]
		.vlan.ingress_vlan_block_info.vlans[idx]
		.filters_idx[0] = vlan_cfg.entry_index;

	// Entry 1 : Only Outer tags is present : matching  ACCEPT
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_1 >= 0 ?
			filter_1 :
			priv->port_info[port]
				.vlan.ingress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = vid;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.add_outer_vlan = true;
	vlan_cfg.treatment.outer_vlan.vid_mode = MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
	vlan_cfg.treatment.outer_vlan.vid_val = dsa_tag_8021q_standalone_vid(dp);
	vlan_cfg.treatment.outer_vlan.tpid = MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
	vlan_cfg.treatment.outer_vlan.priority_mode = MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
	vlan_cfg.treatment.outer_vlan.priority_val = 0;
	vlan_cfg.treatment.outer_vlan.dei = MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add entry:%d for block_id:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[port]
		.vlan.ingress_vlan_block_info.vlans[idx]
		.filters_idx[1] = vlan_cfg.entry_index;

	priv->port_info[port].vlan.ingress_vlan_block_info.vlans[idx].vid = vid;
	priv->port_info[port].vlan.ingress_vlan_block_info.vlans[idx].used = true;

	return ret;
}


static int prepare_vlan_ingress_filters_sp_tag_cpu(struct dsa_switch *ds, uint8_t port, uint8_t cpu_port)
{
	int ret = -EINVAL;
	uint16_t idx = 0;
	/* negative values if not found */
	int filter_0 = -1;
	int filter_1 = -1;
	struct mxl862xx_priv *priv = ds->priv;
	uint16_t bridge_port_cpu = priv->port_info[port].bridge_port_cpu;
	struct dsa_port *dp = dsa_to_port(ds, port);
	uint16_t vid = dsa_tag_8021q_standalone_vid(dp);

	struct mxl862xx_extendedvlan_config vlan_cfg = { };

	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[cpu_port].vlan.ingress_vlan_block_info);
	if (ret)
		return ret;

	/* VID specific filtering rules which should be executed first before final ones.
	 * Storing starts at the beginning of the block. */
	ret = get_vlan_vid_filters_idx(priv, cpu_port, true, vid, &filter_0, &filter_1, &idx);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d couldn't get idx for VID specific filters for VID:%d and  block ID:%d\n",
			__func__, cpu_port, vid, vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	// Entry 0 : Outer and Inner VLAN tag present
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[cpu_port].vlan.ingress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_0 >= 0 ?
			filter_0 :
			priv->port_info[cpu_port]
				.vlan.ingress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = vid;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.reassign_bridge_port = true;
	vlan_cfg.treatment.new_bridge_port_id = bridge_port_cpu;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: cpu_port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, cpu_port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[cpu_port]
		.vlan.ingress_vlan_block_info.vlans[idx]
		.filters_idx[0] = vlan_cfg.entry_index;

	// Entry 1 : Only Outer tags is present
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[cpu_port].vlan.ingress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_1 >= 0 ?
			filter_1 :
			priv->port_info[cpu_port]
				.vlan.ingress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = vid;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.reassign_bridge_port = true;
	vlan_cfg.treatment.new_bridge_port_id = bridge_port_cpu;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: cpu_port:%d failed to add entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, cpu_port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[cpu_port]
		.vlan.ingress_vlan_block_info.vlans[idx]
		.filters_idx[1] = vlan_cfg.entry_index;

	priv->port_info[cpu_port].vlan.ingress_vlan_block_info.vlans[idx].vid = vid;
	priv->port_info[cpu_port].vlan.ingress_vlan_block_info.vlans[idx].used = true;

	return ret;
}

static int prepare_vlan_ingress_filters(struct dsa_switch *ds, uint8_t port, uint16_t vid)
{
	int ret = -EINVAL;
	uint16_t idx = 0;
	/* negative values if not found */
	int filter_0 = -1;
	int filter_1 = -1;
	struct mxl862xx_priv *priv = ds->priv;

	struct mxl862xx_extendedvlan_config vlan_cfg = { };
	ret = mxl862xx_allocate_extended_vlan_block(priv,
		&priv->port_info[port].vlan.ingress_vlan_block_info);
	if (ret)
		return ret;

	/* First populate the block with set of rules which should be executed finally after
	 * VID specific filtering. The final rules (not related to VID) are placed on the end of the block. The number of
	 * rules is fixed  per port. Order of execution  is important. To avoid static reservations they are
	 * stored in reversed order starting from the end of the block */

	//Entry 6 no other rule applies Outer tag default Inner tag  not present DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 1;
	vlan_cfg.filter.outer_vlan.type =
		MXL862XX_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add default DISCARD entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	//Entry 5 no other rule applies Outer tag default Inner tag  present DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 2;
	vlan_cfg.filter.outer_vlan.type =
		MXL862XX_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret) {
		dev_err(priv->dev,
			"%s: Port:%d failed to add default DISCARD entry:%d for ingress extended VLAN block ID:%d\n",
			__func__, port, vlan_cfg.entry_index,
			vlan_cfg.extended_vlan_block_id);
		return ret;
	}

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	// Entry 4  untagged pkts. If there's PVID accept and add PVID tag, otherwise reject
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 3;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	if (!priv->port_info[port].vlan.pvid) {
		vlan_cfg.treatment.remove_tag =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;
	} else {
		vlan_cfg.treatment.add_outer_vlan = true;
		vlan_cfg.treatment.outer_vlan.vid_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vlan_cfg.treatment.outer_vlan.tpid =
			MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q;
		vlan_cfg.treatment.outer_vlan.priority_mode =
			MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vlan_cfg.treatment.outer_vlan.priority_val = 0;
		vlan_cfg.treatment.outer_vlan.dei =
			MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0;
		vlan_cfg.treatment.outer_vlan.vid_val =
			priv->port_info[port].vlan.pvid;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	// Entry 3 : Only Outer tag present : not matching  DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 4;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	// Entry 2 : Outer and Inner VLAN tag present : not matching  DISCARD
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	vlan_cfg.entry_index = priv->port_info[port].vlan.ingress_vlan_block_info.filters_max - 5;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx = vlan_cfg.entry_index;

	/* VID specific filtering rules which should be executed first before final ones.
	 * Storing starts at the beginning of the block. */

	ret = get_vlan_vid_filters_idx(priv, port, true, vid, &filter_0, &filter_1, &idx);
	if (ret)
		return ret;

	// Entry 0 : Outer and Inner VLAN tag present :  matching  ACCEPT
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_0 >= 0 ?
			filter_0 :
			priv->port_info[port]
				.vlan.ingress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = vid;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[port]
		.vlan.ingress_vlan_block_info.vlans[idx]
		.filters_idx[0] = vlan_cfg.entry_index;

	// Entry 1 : Only Outer tag is present : matching  ACCEPT
	memset(&vlan_cfg, 0, sizeof(vlan_cfg));
	vlan_cfg.extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;
	/* if found recycled entry reuse it, otherwise create new one */
	vlan_cfg.entry_index =
		filter_1 >= 0 ?
			filter_1 :
			priv->port_info[port]
				.vlan.ingress_vlan_block_info.vid_filters_idx++;
	vlan_cfg.filter.outer_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
	vlan_cfg.filter.outer_vlan.vid_enable = true;
	vlan_cfg.filter.outer_vlan.vid_val = vid;
	vlan_cfg.filter.inner_vlan.type = MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vlan_cfg.treatment.remove_tag =
		MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_EXTENDEDVLAN_SET, vlan_cfg);
	if (ret)
		return ret;

	/* store VLAN filtering rules ID's (for VLAN delete, if needed) */
	priv->port_info[port]
		.vlan.ingress_vlan_block_info.vlans[idx]
		.filters_idx[1] = vlan_cfg.entry_index;

	priv->port_info[port].vlan.ingress_vlan_block_info.vlans[idx].vid = vid;
	priv->port_info[port].vlan.ingress_vlan_block_info.vlans[idx].used = true;

	return ret;
}

static int mxl862xx_port_vlan_add(struct dsa_switch *ds, int port,
                                  const struct switchdev_obj_port_vlan *vlan,
                                  struct netlink_ext_ack *extack)
{
	int ret = -EINVAL;
	struct mxl862xx_priv *priv = ds->priv;
	struct mxl862xx_bridge_port_config br_port_cfg = { };
	bool untagged = vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED;
	bool pvid = vlan->flags & BRIDGE_VLAN_INFO_PVID;
	uint8_t cpu_port = priv->cpu_port;
	bool vlan_sp_tag = (priv->port_info[cpu_port].tag_protocol == DSA_TAG_PROTO_MXL862_8021Q);
	bool standalone_port = false;
	uint16_t vid = vlan->vid;

	if (port < 0 || port >= priv->hw_info->max_ports) {
		dev_err(priv->dev, "invalid port: %d\n", port);
		NL_SET_ERR_MSG_MOD(extack, "Port out of range");
		return ret;
	}

	if (!((struct dsa_port *)dsa_to_port(ds, port))) {
		dev_err(ds->dev, "%s:  port:%d is out of DSA domain\n", __func__, port);
		NL_SET_ERR_MSG_MOD(extack, "Port out of DSA domain");
		return ret;
	}

	/* standalone port */
	if ((priv->port_info[port].bridge == NULL) && (!dsa_is_cpu_port(ds, port)))
		standalone_port = true;

	if (vid == 0)
		return ret;

	/* If this is request to set pvid, just overwrite it as there may be
	 * only one pid per port */
	if (pvid)
		priv->port_info[port].vlan.pvid = vid;
	/* If this is pvid disable request, check if there's already matching vid
	 * and only then disable it. If vid doesn't match active pvid, don't touch it */
	else {
		if (priv->port_info[port].vlan.pvid == vid)
			priv->port_info[port].vlan.pvid = 0;
	}

	/* Check if there's enough room for ingress and egress rules */
	if ((priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx -
			priv->port_info[port].vlan.ingress_vlan_block_info.vid_filters_idx) <
			(priv->port_info[port].vlan.ingress_vlan_block_info.entries_per_vlan)) {

		dev_err(ds->dev,
			"%s: Port:%d vlan:%d. Number of avaliable ingress entries too low. Required:%d  ff_idx:%d vf_idx:%d .\n",
			__func__, port, vid,
			priv->port_info[port].vlan.ingress_vlan_block_info.entries_per_vlan,
			priv->port_info[port].vlan.ingress_vlan_block_info.final_filters_idx,
			priv->port_info[port].vlan.ingress_vlan_block_info.vid_filters_idx);

		ret = -ENOSPC;
		NL_SET_ERR_MSG_MOD(extack, "Reached max number of VLAN ingress filter entries per port");
		return ret;
	}

	if ((priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx -
			priv->port_info[port].vlan.egress_vlan_block_info.vid_filters_idx) <
			(priv->port_info[port].vlan.egress_vlan_block_info.entries_per_vlan)) {

		dev_err(ds->dev,
			"%s: Port:%d vlan:%d. Number of avaliable egress entries too low. Required:%d  ff_idx:%d vf_idx:%d .\n",
			__func__, port, vid,
			priv->port_info[port].vlan.egress_vlan_block_info.entries_per_vlan,
			priv->port_info[port].vlan.egress_vlan_block_info.final_filters_idx,
			priv->port_info[port].vlan.egress_vlan_block_info.vid_filters_idx);

		ret = -ENOSPC;
		NL_SET_ERR_MSG_MOD(extack, "Reached max number of VLAN egress filter entries per port");
		return ret;
	}

	/* Although 4-byte vlan special tagging handling is similar to 8 byte MxL tagging,
	 * keep VLAN rules separate for better readibility */
	if (vlan_sp_tag) {
		if (!dsa_is_cpu_port(ds, port)) {
			/* Special rules for CPU port based on user port id */
			ret = prepare_vlan_ingress_filters_sp_tag_cpu(ds, port, cpu_port);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to prepare ingress filters for VLAN:%d with vlan_filtering disabled\n",
					__func__, port, vid);
				NL_SET_ERR_MSG_MOD(extack, "Failed to prepare ingress filters with vlan_filtering disabled");
				return ret;
			}
			ret = prepare_vlan_egress_filters_sp_tag_cpu(ds, cpu_port, vid, untagged);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to prepare egress filters for VLAN:%d with vlan_filtering disabled\n",
					__func__, cpu_port, vid);
				NL_SET_ERR_MSG_MOD(extack, "Failed to prepare egress filters with vlan_filtering disabled");
				return ret;
			}
			/* vlan_filtering disabled */
			/* skiping this configuration for vlan_sp_tag/cpu port as it requires special rules defined above */
			if (!priv->port_info[port].vlan.filtering) {
				dev_dbg(ds->dev,
					"%s: port:%d setting VLAN:%d with vlan_filtering disabled\n",
					__func__, port, vid);
				ret = prepare_vlan_ingress_filters_off_sp_tag(ds, port, vid);
				if (ret) {
					dev_err(ds->dev,
						"%s: Port:%d failed to prepare ingress filters for VLAN:%d with vlan_filtering disabled\n",
						__func__, port, vid);
					NL_SET_ERR_MSG_MOD(extack, "Failed to prepare ingress filters with vlan_filtering disabled");
					return ret;
				}

				ret = prepare_vlan_egress_filters_off_sp_tag(ds, port, vid, untagged);
				if (ret) {
					dev_err(ds->dev,
						"%s: Port:%d failed to prepare egress filters for VLAN:%d with vlan_filtering disabled\n",
						__func__, port, vid);
					NL_SET_ERR_MSG_MOD(extack, "Failed to prepare egress filters with vlan_filtering disabled");
					return ret;
				}
			}
			/* vlan_filtering enabled */
			else {
				/* special rules for the CPU port are already defined,
				 * so define only the rules for user ports */
				ret = prepare_vlan_ingress_filters_sp_tag(ds, port, vid);
				if (ret) {
					dev_err(ds->dev,
						"%s: Port:%d failed to prepare ingress filters for VLAN:%d\n",
						__func__, port, vid);
					NL_SET_ERR_MSG_MOD(extack, "Failed to prepare ingress filters for VLAN");
					return ret;
				}

				ret = prepare_vlan_egress_filters_sp_tag(ds, port, vid, untagged);
				if (ret) {
					dev_err(ds->dev,
						"%s: Port:%d failed to prepare egress filters for VLAN:%d\n",
						__func__, port, vid);
					NL_SET_ERR_MSG_MOD(extack, "Failed to prepare egress filters for VLAN");
					return ret;
				}
			}
		} else {
			/* CPU port. This else block handles explicit request for adding
			 * VLAN to CPU port. Only egress rule requires reconfiguration.*/
			ret = prepare_vlan_egress_filters_sp_tag_cpu(ds, cpu_port, vid, untagged);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to prepare egress filters for VLAN:%d with vlan_filtering disabled\n",
					__func__, cpu_port, vid);
				NL_SET_ERR_MSG_MOD(extack, "Failed to prepare egress filters with vlan_filtering disabled");
				return ret;
			}
		}

		/* CPU port is explicitely added by the DSA framework to the new vlans.
		   Apply block_id with filtering rules defined while processing user ports
			For 8021q special tag mode cpu port rules may change because of new ports added,
			so they need to be reloaded */
		{
			struct mxl862xx_ctp_port_config ctp_param = { };

			ctp_param.logical_port_id = DSA_MXL_PORT(cpu_port);
			ctp_param.mask = MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_VLAN |
					     MXL862XX_CTP_PORT_CONFIG_MASK_INGRESS_VLAN;
			ctp_param.egress_extended_vlan_enable = true;
			ctp_param.egress_extended_vlan_block_id =
			priv->port_info[cpu_port].vlan.egress_vlan_block_info.block_id;
			ctp_param.ingress_extended_vlan_enable = true;
			ctp_param.ingress_extended_vlan_block_id =
				priv->port_info[cpu_port].vlan.ingress_vlan_block_info.block_id;

			ret = MXL862XX_API_WRITE(priv, MXL862XX_CTP_PORTCONFIGSET, ctp_param);
			if (ret) {
				dev_err(ds->dev,
					"%s: CTP port %d config failed on port config set with %d\n",
					__func__, cpu_port, ret);
				NL_SET_ERR_MSG_MOD(extack, "Failed to configure VLAN for cpu port");
				return ret;
			}
		}
	} else {
		/* VLAN rules for 8 byte MxL tagging*/
		/* vlan_filtering disabled */
		/* skiping this configuration for vlan_sp_tag/cpu port as it requires special rules defined above */
		if (!priv->port_info[port].vlan.filtering) {
			ret = prepare_vlan_ingress_filters_off(priv, port, vid);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to prepare ingress filters for VLAN:%d with vlan_filtering disabled\n",
					__func__, port, vid);
				NL_SET_ERR_MSG_MOD(extack, "Failed to prepare ingress filters with vlan_filtering disabled");
				return ret;
			}

			ret = prepare_vlan_egress_filters_off(priv, port, vid, untagged);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to prepare egress filters for VLAN:%d with vlan_filtering disabled\n",
					__func__, port, vid);
				NL_SET_ERR_MSG_MOD(extack, "Failed to prepare egress filters with vlan_filtering disabled");
				return ret;
			}
		}
		/* vlan_filtering enabled */
		else {
			ret = prepare_vlan_ingress_filters(ds, port, vid);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to prepare ingress filters for VLAN:%d\n",
					__func__, port, vid);
				NL_SET_ERR_MSG_MOD(extack, "Failed to prepare ingress filters for VLAN");
				return ret;
			}
			ret = prepare_vlan_egress_filters(ds, port, vid, untagged);
			if (ret) {
				dev_err(ds->dev,
					"%s: Port:%d failed to prepare egress filters for VLAN:%d\n",
					__func__, port, vid);
				NL_SET_ERR_MSG_MOD(extack, "Failed to prepare egress filters for VLAN");
				return ret;
			}
		}

		/* CPU port is explicitely added by the DSA framework to new vlans */
		if (dsa_is_cpu_port(ds, port)) {
			struct mxl862xx_ctp_port_config ctp_param = { };

			ctp_param.logical_port_id = DSA_MXL_PORT(port);
			ctp_param.mask = MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_VLAN |
					     MXL862XX_CTP_PORT_CONFIG_MASK_INGRESS_VLAN;
			ctp_param.egress_extended_vlan_enable = true;
			ctp_param.egress_extended_vlan_block_id =
				priv->port_info[port].vlan.egress_vlan_block_info.block_id;
			ctp_param.ingress_extended_vlan_enable = true;
			ctp_param.ingress_extended_vlan_block_id =
				priv->port_info[port].vlan.ingress_vlan_block_info.block_id;

			ret = MXL862XX_API_WRITE(priv, MXL862XX_CTP_PORTCONFIGSET, ctp_param);
			if (ret) {
				dev_err(ds->dev,
					"%s: CTP port %d config failed on port config set with %d\n",
					__func__, port, ret);
				NL_SET_ERR_MSG_MOD(extack, "Failed to configure VLAN for cpu port");
				return ret;
			}

			return ret;
		}
	}

	/* Update bridge port */
	br_port_cfg.bridge_port_id = DSA_MXL_PORT(port);
	br_port_cfg.mask |= MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN |
			    MXL862XX_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN |
				MXL862XX_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING |
				MXL862XX_BRIDGE_PORT_CONFIG_MASK_VLAN_BASED_MAC_LEARNING;
	br_port_cfg.egress_extended_vlan_enable = true;
	br_port_cfg.egress_extended_vlan_block_id =
		priv->port_info[port].vlan.egress_vlan_block_info.block_id;
	br_port_cfg.ingress_extended_vlan_enable = true;
	br_port_cfg.ingress_extended_vlan_block_id =
		priv->port_info[port].vlan.ingress_vlan_block_info.block_id;

	/* Disable MAC learning for standalone ports. */
	br_port_cfg.src_mac_learning_disable = (standalone_port) ? true : false;
	br_port_cfg.vlan_src_mac_vid_enable = br_port_cfg.vlan_dst_mac_vid_enable =
		(vlan_sp_tag) ? false : !standalone_port;

	ret = MXL862XX_API_WRITE(priv, MXL862XX_BRIDGEPORT_CONFIGSET, br_port_cfg);
	if (ret) {
		dev_err(ds->dev,
			"%s: Bridge port configuration for port %d failed with %d\n",
			__func__, port, ret);
		NL_SET_ERR_MSG_MOD(extack, "Bridge port configuration for VLAN failed");
		return ret;
	}

	return ret;
}


static int mxl862xx_port_vlan_del(struct dsa_switch *ds, int port,
				  const struct switchdev_obj_port_vlan *vlan)
{
	int ret = -EINVAL;
	int dir;
	struct mxl862xx_priv *priv = ds->priv;
	uint16_t vid = vlan->vid;

	for (dir = 0 ; dir < 2 ; dir++) {
		struct mxl862xx_extended_vlan_block_info *block_info = (dir == 0)
			? &priv->port_info[port].vlan.ingress_vlan_block_info
			: &priv->port_info[port].vlan.egress_vlan_block_info;
		char *dir_txt = (dir == 0)	? "ingress" : "egress";
		int16_t entry_idx;
		int vlan_idx, x;
		u16 block_id = block_info->block_id;
		/* Indicator of the last dynamic vid related entry being processed.
		 * Required for cleanup of static rules at the end of the block. */
		bool last_vlan = false;
		bool vlan_found = false;

		/* check if vlan is present */
		for (vlan_idx = 0; vlan_idx < MAX_VLANS; vlan_idx++) {
			if ((block_info->vlans[vlan_idx].vid == vid)
					&& block_info->vlans[vlan_idx].used)
				vlan_found = true;

			if (vlan_idx == MAX_VLANS - 1)
				last_vlan = true;

			if (vlan_found)
				break;
		}

		if (!vlan_found) {
			dev_err(ds->dev, "%s: Port:%d VLAN:%d not found (%s)\n", __func__, port, vid, dir_txt);
			goto static_rules_cleanup;
		}

		/* cleanup */
		for (x = 0; x < VID_RULES ; x++) {
			entry_idx = block_info->vlans[vlan_idx].filters_idx[x];
			if (entry_idx != IDX_INVAL) {
				ret = deactivate_vlan_filter_entry(ds, block_id, entry_idx);
				if (ret)
					return ret;
			}
		}

		/* cleanup of the vlan record in the port vlan inventory */
		block_info->vlans[vlan_idx].vid = 0;
		block_info->vlans[vlan_idx].used = false;

		/* find the first free slot for storing recycled filter entries */
		for (x = 0; x < MAX_VLANS; x++) {
			if (!(block_info->filter_entries_recycled[x].valid)) {
				block_info->filter_entries_recycled[x].filters_idx[0] = block_info->vlans[vlan_idx].filters_idx[0];
				block_info->filter_entries_recycled[x].filters_idx[1] = block_info->vlans[vlan_idx].filters_idx[1];
				block_info->filter_entries_recycled[x].valid = true;
				block_info->vlans[vlan_idx].filters_idx[0] = IDX_INVAL;
				block_info->vlans[vlan_idx].filters_idx[1] = IDX_INVAL;
				break;
			}

			if (x == MAX_VLANS - 1) {
				ret = -ENOSPC;
				dev_err(ds->dev,
					"%s: Port:%d no free slots for recycled %s filter entries\n",
					__func__, port, dir_txt);
				return ret;
			}
		}

static_rules_cleanup:
		/* If this is the last vlan entry or no entries left,
		 * remove static entries (placed at the end of the block) */
		if (last_vlan && block_id != 0xffff) {
			for (entry_idx = block_info->final_filters_idx; entry_idx < block_info->filters_max ; entry_idx++) {
				ret = deactivate_vlan_filter_entry(ds, block_id, entry_idx);
				if (ret)
					return ret;
			}
			/* Entries cleared, so point out to the end */
			block_info->final_filters_idx = block_info->filters_max-1;
		}
	}

	return 0;
}


static int mxl862xx_isolate_port(struct dsa_switch *ds, int port)
{
	struct mxl862xx_bridge_alloc param = {};
	struct mxl862xx_priv *priv = ds->priv;
	uint8_t cpu_port = priv->cpu_port;
	bool vlan_sp_tag = (priv->port_info[cpu_port].tag_protocol == DSA_TAG_PROTO_MXL862_8021Q);
	int ret;

	if (priv->port_info[port].isolated)
		return 0;

	ret = MXL862XX_API_READ(priv, MXL862XX_BRIDGE_ALLOC, param);
	if (ret) {
		dev_err(ds->dev, "failed to allocate a bridge for port %d\n", port);
		return ret;
	}

	priv->port_info[port].bridge_id = param.bridge_id;
	priv->port_info[port].bridge = NULL;
	ret = mxl862xx_update_bridge_conf_port(ds, port, NULL, 1);
	if (ret) {
		dev_err(ds->dev, "failed to add port %d to bridge %d\n", port, param.bridge_id);
		return ret;
	}

	/* for VLAN special tagging mode add port to vlan 1 to apply also
	 * the special tag handling filters */
	if (vlan_sp_tag) {
		/* set port vlan 1 untagged */
		struct switchdev_obj_port_vlan vlan;
		uint16_t vid = 1;
		bool filtering_prev = priv->port_info[port].vlan.filtering;
		priv->port_info[port].vlan.filtering = true;
		vlan.flags = BRIDGE_VLAN_INFO_UNTAGGED | BRIDGE_VLAN_INFO_PVID;
		vlan.vid = vid;
		ret = mxl862xx_port_vlan_add(ds, port, &vlan, NULL);
		if (ret) {
			dev_err(ds->dev,
				"%s: adding port %d, to vlan:%d failed with ret %d\n",
				__func__, port, vlan.vid, ret);
		}
		priv->port_info[port].vlan.filtering = filtering_prev;
	}

	if (!ret)
		priv->port_info[port].isolated = true;

	return ret;
}

static void mxl862xx_deisolate_port(struct dsa_switch *ds, u8 port)
{
	struct mxl862xx_bridge_alloc bridge_alloc = { };
	struct mxl862xx_priv *priv = ds->priv;
	uint8_t cpu_port = priv->cpu_port;
	bool vlan_sp_tag = (priv->port_info[cpu_port].tag_protocol == DSA_TAG_PROTO_MXL862_8021Q);
	int ret;

	if (!priv->port_info[port].isolated)
		return;

	ret = mxl862xx_update_bridge_conf_port(ds, port, NULL, 0);
	if (ret) {
		dev_err(ds->dev, "failed to remove port %d from bridge\n", port);
		return;
	}

	bridge_alloc.bridge_id = priv->port_info[port].bridge_id;
	ret = MXL862XX_API_WRITE(priv, MXL862XX_BRIDGE_FREE, bridge_alloc);
	if (ret) {
		dev_err(ds->dev, "failed to free bridge %d\n", bridge_alloc.bridge_id);
		return;
	}

	/* For VLAN special tagging mode isolated port is assigned to VLAN 1
	 * to apply also the special tag handling filters. Now for deisolation
	 * VLAN 1 must be unassigned */
	if (vlan_sp_tag) {
		struct switchdev_obj_port_vlan vlan;
		uint16_t vid = 1;
		int i;

		vlan.flags = BRIDGE_VLAN_INFO_UNTAGGED | BRIDGE_VLAN_INFO_PVID;
		vlan.vid = vid;
		/* Removes vid dependant 'dynamic' rules */
		ret = mxl862xx_port_vlan_del(ds, port, &vlan);
		if (ret) {
			dev_err(ds->dev,
				"%s: deleting port %d, from vlan:%d failed with ret %d\n",
				__func__, port, vid, ret);
		}

		/* Clear/deactivate 'static' set of filtering rules, placed at the end of the block */
		for (i = 0 ; i < 2 ; i++) {
			uint16_t j, start_idx, stop_idx, block_id;
			struct mxl862xx_extended_vlan_block_info *block_info = (i == 0)
				? &priv->port_info[port].vlan.ingress_vlan_block_info
				: &priv->port_info[port].vlan.egress_vlan_block_info;

			block_id = block_info->block_id;
			stop_idx = block_info->filters_max;
			start_idx = block_info->final_filters_idx;

			for (j = start_idx ; j < stop_idx ; j++) {
				ret = deactivate_vlan_filter_entry(ds, block_id, j);
				if (ret)
					return;
			}
			/* Entries cleared, so point out to the end */
			block_info->final_filters_idx = j;
		}
	}

	priv->port_info[port].isolated = false;
}

static int mxl862xx_find_bridge_id(struct dsa_switch *ds, struct net_device *bridge)
{
	struct mxl862xx_priv *priv = ds->priv;
	u8 i;

	if (bridge) {
		for (i = 0; i < priv->hw_info->max_ports; i++) {
			if (dsa_is_unused_port(ds, i))
				continue;

			if (priv->port_info[i].bridge == bridge)
				return priv->port_info[i].bridge_id;
		}
	}

	return 0;
}

static int mxl862xx_mac_learning(struct dsa_switch *ds, int port, bool enable)
{
	struct mxl862xx_bridge_port_config param = {
		.mask = MXL862XX_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING |
				MXL862XX_BRIDGE_PORT_CONFIG_MASK_VLAN_BASED_MAC_LEARNING,
		.bridge_port_id = DSA_MXL_PORT(port),
		.src_mac_learning_disable = !enable,
	};
	int ret;
	struct mxl862xx_priv *priv = ds->priv;
	u8 cpu_port = priv->cpu_port;
	bool vlan_sp_tag = (priv->port_info[cpu_port].tag_protocol == DSA_TAG_PROTO_MXL862_8021Q);

	param.vlan_src_mac_vid_enable = param.vlan_dst_mac_vid_enable =
		(vlan_sp_tag) ? false : enable;
	ret = MXL862XX_API_WRITE(ds->priv, MXL862XX_BRIDGEPORT_CONFIGSET, param);
	if (ret)
		dev_err(ds->dev, "failed to %s MAC learning on port %d\n",
			enable ? "enable" : "disable", port);
	return ret;
}

static void mxl862xx_set_vlan_filter_limits(struct dsa_switch *ds)
{
	u8 i;
	u16 cpu_ingress_entries;
	u16 cpu_egress_entries;
	u16 user_ingress_entries;
	u16 user_egress_entries;
	struct mxl862xx_priv *priv = ds->priv;
	u8 cpu_port = priv->cpu_port;
	struct mxl862xx_port_vlan_info *vlan = &priv->port_info[cpu_port].vlan;

	/* Set limits and indexes required for processing VLAN rules for CPU port */

	/* The calculation of the max number of simultaneously supported VLANS (priv->max_vlans)
	 * comes from the equation:
	 *
	 * MAX_VLAN_ENTRIES = phy_ports * (EGRESS_FINAL_RULES + EGRESS_VID_RULES * priv->max_vlans)
	 *  + phy_ports * (INGRESS_FINAL_RULES + INGRESS_VID_RULES * priv-> max_vlans)
	 *  + cpu_ingress_entries + cpu_egress_entries
	 */
	if (priv->port_info[cpu_port].tag_protocol == DSA_TAG_PROTO_MXL862_8021Q) {
		priv->max_vlans = (MAX_VLAN_ENTRIES - priv->user_pnum *
				   (EGRESS_FINAL_RULES + INGRESS_FINAL_RULES + 2) - 3) /
				  (priv->user_pnum *
				   (EGRESS_VID_RULES + INGRESS_VID_RULES) + 2);
		/* 2 entries per port and 1 entry for fixed rule */
		cpu_ingress_entries = priv->user_pnum * 2 + 1;
		/* 2 entries per each vlan and 2 entries for fixed rules */
		cpu_egress_entries = priv->max_vlans * 2 + 2;

		vlan->ingress_vlan_block_info.entries_per_vlan = 0;
		vlan->ingress_vlan_block_info.filters_max = cpu_ingress_entries;
		vlan->egress_vlan_block_info.entries_per_vlan = 2;
		vlan->egress_vlan_block_info.filters_max = cpu_egress_entries;

		user_ingress_entries = INGRESS_FINAL_RULES + INGRESS_VID_RULES * priv->max_vlans;
		user_egress_entries = EGRESS_FINAL_RULES + EGRESS_VID_RULES * priv->max_vlans;
	} else {
		priv->max_vlans = (MAX_VLAN_ENTRIES - priv->user_pnum *
				(EGRESS_FINAL_RULES + INGRESS_FINAL_RULES) - 1) /
			(priv->user_pnum * (EGRESS_VID_RULES + INGRESS_VID_RULES) + 2);
		/* 1 entry for fixed rule */
		cpu_ingress_entries =  1;
		/* 2 entries per each vlan  */
		cpu_egress_entries = priv->max_vlans * 2;
		vlan->ingress_vlan_block_info.entries_per_vlan = 0;
		vlan->ingress_vlan_block_info.filters_max = cpu_ingress_entries;
		vlan->egress_vlan_block_info.entries_per_vlan = 2;
		vlan->egress_vlan_block_info.filters_max = cpu_egress_entries;

		user_ingress_entries = INGRESS_FINAL_RULES + INGRESS_VID_RULES * priv->max_vlans;
		user_egress_entries = EGRESS_FINAL_RULES + EGRESS_VID_RULES * priv->max_vlans;
	}

	/* This index is counted backwards */
	vlan->ingress_vlan_block_info.final_filters_idx =
		vlan->ingress_vlan_block_info.filters_max - 1;
	vlan->egress_vlan_block_info.final_filters_idx =
		vlan->egress_vlan_block_info.filters_max - 1;

	/* block_id uninitialized */
	vlan->ingress_vlan_block_info.block_id = 0xffff;
	vlan->egress_vlan_block_info.block_id = 0xffff;

	/* Set limits and indexes required for processing VLAN rules for user ports */
	for (i = 0; i < priv->hw_info->max_ports; i++) {
		if (dsa_is_unused_port(ds, i))
			continue;
		if (dsa_is_cpu_port(ds, i))
			continue;

		vlan = &priv->port_info[i].vlan;
		vlan->ingress_vlan_block_info.entries_per_vlan = INGRESS_VID_RULES;
		vlan->ingress_vlan_block_info.filters_max = user_ingress_entries;
		vlan->egress_vlan_block_info.entries_per_vlan = EGRESS_VID_RULES;
		vlan->egress_vlan_block_info.filters_max = user_egress_entries;
		/* This index is counted backwards */
		vlan->ingress_vlan_block_info.final_filters_idx =
			vlan->ingress_vlan_block_info.filters_max - 1;
		vlan->egress_vlan_block_info.final_filters_idx =
			vlan->egress_vlan_block_info.filters_max - 1;
	}
	dev_info(ds->dev, "%s: user_pnum:%d, priv->max_vlans: %d, cpu_ingress_entries: %d, "
		 "cpu_egress_entries: %d, user_ingress_entries: %d, user_egress_entries: %d\n",
		 __func__, priv->user_pnum, priv->max_vlans, cpu_ingress_entries,
		 cpu_egress_entries, user_ingress_entries, user_egress_entries);
}

static int mxl862xx_set_ageing_time(struct dsa_switch *ds, unsigned int msecs)
{
	struct mxl862xx_cfg param;
	int ret;

	ret = MXL862XX_API_READ(ds->priv, MXL862XX_COMMON_CFGGET, param);
	if (ret) {
		dev_err(ds->dev, "failed to read switch config\n");
		return ret;
	}

	param.mac_table_age_timer = MXL862XX_AGETIMER_CUSTOM;
	param.age_timer = msecs / 1000;
	ret = MXL862XX_API_WRITE(ds->priv, MXL862XX_COMMON_CFGSET, param);
	if (ret)
		dev_err(ds->dev, "failed to set ageing\n");

	return ret;
}

static int mxl862xx_port_bridge_join(struct dsa_switch *ds, int port, struct dsa_bridge bridge,
				     bool *tx_fwd_offload, struct netlink_ext_ack *extack)

{
	struct mxl862xx_priv *priv = ds->priv;
	uint8_t cpu_port = priv->cpu_port;
	bool vlan_sp_tag = (priv->port_info[cpu_port].tag_protocol == DSA_TAG_PROTO_MXL862_8021Q);
	int bridge_id;
	int ret;

	if (priv->force_isolate) {
		dev_info(priv->dev, "ignore bridge join due to force isolate\n");
		return 0;
	}

	mxl862xx_deisolate_port(ds, port);

	bridge_id = mxl862xx_find_bridge_id(ds, bridge.dev);

	if (bridge_id == 0) {
		struct mxl862xx_bridge_alloc bridge_alloc = { };

		ret = MXL862XX_API_READ(priv, MXL862XX_BRIDGE_ALLOC, bridge_alloc);
		if (ret) {
			dev_err(ds->dev, "failed to allocate new bridge\n");
			return ret;
		}
		priv->port_info[port].bridge_id = bridge_alloc.bridge_id;
		priv->port_info[port].bridge = bridge.dev;
	} else {
		priv->port_info[port].bridge_id = bridge_id;
		priv->port_info[port].bridge = bridge.dev;
	}

	ret = mxl862xx_update_bridge_conf_port(ds, port, bridge.dev, 1);
	if (ret) {
		dev_err(ds->dev,
			"%s: bridge port adding failed for port %d, ret %d\n",
			__func__, port, ret);
		return ret;
	}

	if (vlan_sp_tag) {
		struct mxl862xx_ctp_port_config ctp_param = { };
		struct mxl862xx_bridge_port_config br_port_cfg = { };

		ret = prepare_vlan_ingress_filters_sp_tag_cpu(ds, port, cpu_port);
		if (ret)
			return ret;
		ret = prepare_vlan_egress_filters_off_sp_tag_cpu(ds, cpu_port);
		if (ret)
			return ret;
		ret = prepare_vlan_ingress_filters_off_sp_tag_no_vid(ds, port);
		if (ret)
			return ret;
		ret = prepare_vlan_egress_filters_off_sp_tag_no_vid(ds, port);
		if (ret)
			return ret;

		/* update cpu port */
		ctp_param.logical_port_id = DSA_MXL_PORT(cpu_port);
		ctp_param.mask = MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_VLAN |
				     MXL862XX_CTP_PORT_CONFIG_MASK_INGRESS_VLAN;
		ctp_param.egress_extended_vlan_enable = true;
		ctp_param.egress_extended_vlan_block_id =
		priv->port_info[cpu_port].vlan.egress_vlan_block_info.block_id;
		ctp_param.ingress_extended_vlan_enable = true;
		ctp_param.ingress_extended_vlan_block_id =
			priv->port_info[cpu_port].vlan.ingress_vlan_block_info.block_id;

		ret = MXL862XX_API_WRITE(priv, MXL862XX_CTP_PORTCONFIGSET, ctp_param);
		if (ret) {
			dev_err(ds->dev,
				"%s: CTP port %d config failed on port config set with %d\n",
				__func__, cpu_port, ret);
			NL_SET_ERR_MSG_MOD(extack, "Failed to configure VLAN for cpu port");
			return ret;
		}

		/* Update bridge port */
		br_port_cfg.bridge_port_id = DSA_MXL_PORT(port);
		br_port_cfg.mask |= MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN |
			     MXL862XX_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN;
		br_port_cfg.egress_extended_vlan_enable = true;
		br_port_cfg.egress_extended_vlan_block_id =
			priv->port_info[port].vlan.egress_vlan_block_info.block_id;
		br_port_cfg.ingress_extended_vlan_enable = true;
		br_port_cfg.ingress_extended_vlan_block_id =
			priv->port_info[port].vlan.ingress_vlan_block_info.block_id;

		ret = MXL862XX_API_WRITE(priv, MXL862XX_BRIDGEPORT_CONFIGSET, br_port_cfg);
		if (ret) {
			dev_err(ds->dev,
				"%s: Bridge port configuration for port %d failed with %d\n",
				__func__, port, ret);
			NL_SET_ERR_MSG_MOD(extack, "Bridge port configuration for VLAN failed");
		}
	}

	return 0;
}

static void mxl862xx_port_bridge_leave(struct dsa_switch *ds, int port,
				       struct dsa_bridge bridge)
{
	struct mxl862xx_priv *priv = ds->priv;
	struct mxl862xx_bridge_alloc bridge_alloc = {
		.bridge_id = priv->port_info[port].bridge_id,
	};
	unsigned int cpu_port = priv->cpu_port;
	int bridge_id;
	int ret;

	bridge_id = mxl862xx_find_bridge_id(ds, bridge.dev);
	ret = mxl862xx_update_bridge_conf_port(ds, port, bridge.dev, 0);
	if (ret) {
		dev_err(ds->dev, "failed to remove port %d from bridge\n", port);
		return;
	}

	if (priv->bridge_portmap[bridge_id] == BIT(DSA_MXL_PORT(cpu_port))) {
		ret = MXL862XX_API_WRITE(priv, MXL862XX_BRIDGE_FREE, bridge_alloc);
		if (ret) {
			dev_err(ds->dev, "failed to free bridge %d\n", bridge_alloc.bridge_id);
			return;
		}
	}

	mxl862xx_isolate_port(ds, port);
	priv->port_info[port].vlan.filtering_mode_locked = false;
}

static int mxl862xx_phy_read_mii_bus(struct mii_bus *bus, int port,
				     int regnum)
{
	return mxl862xx_phy_read_mmd(bus->priv, port, 0, regnum);
}

static int mxl862xx_phy_write_mii_bus(struct mii_bus *bus, int port,
				      int regnum, u16 val)
{
	return mxl862xx_phy_write_mmd(bus->priv, port, 0, regnum, val);
}

static int mxl862xx_phy_read_c45_mii_bus(struct mii_bus *bus, int port,
					 int devadd, int regnum)
{
	return mxl862xx_phy_read_mmd(bus->priv, port, devadd, regnum);
}

static int mxl862xx_phy_write_c45_mii_bus(struct mii_bus *bus, int port,
					  int devadd, int regnum, u16 val)
{
	return mxl862xx_phy_write_mmd(bus->priv, port, devadd, regnum, val);
}

static int mxl862xx_setup_mdio(struct dsa_switch *ds)
{
	struct mxl862xx_priv *priv = ds->priv;
	struct device *dev = ds->dev;
	struct mii_bus *bus;
	static int idx;
	int ret;

	bus = devm_mdiobus_alloc(dev);
	if (!bus)
		return -ENOMEM;
	bus->priv = ds->priv;

	ds->user_mii_bus = bus;
	bus->name = KBUILD_MODNAME "-mii";
	snprintf(bus->id, MII_BUS_ID_SIZE, KBUILD_MODNAME "-%d", idx++);
	bus->read_c45 = mxl862xx_phy_read_c45_mii_bus;
	bus->write_c45 = mxl862xx_phy_write_c45_mii_bus;
	bus->read = mxl862xx_phy_read_mii_bus;
	bus->write = mxl862xx_phy_write_mii_bus;
	bus->parent = dev;
	bus->phy_mask = ~ds->phys_mii_mask;

	/* 10G ports do not support slave MDIO bus yet */
	if (priv->hw_info->ext_ports <= 2)
		 bus->phy_mask |= 0xff00;

#if IS_ENABLED(CONFIG_OF_MDIO)
	struct device_node *mdio_np;

	mdio_np = of_get_child_by_name(dev->of_node, "mdio");
	if (!mdio_np) {
		dev_err(dev, "no MDIO bus node\n");
		of_node_put(mdio_np);
		return -ENODEV;
	}
	ret = devm_of_mdiobus_register(dev, bus, mdio_np);
	of_node_put(mdio_np);
#else
	ret = devm_mdiobus_register(dev, bus);
#endif
	if (ret)
		dev_err(dev, "failed to register MDIO bus: %d\n", ret);

	return ret;
}

static struct mxl862xx_pcs *pcs_to_mxl862xx_pcs(struct phylink_pcs *pcs)
{
	return container_of(pcs, struct mxl862xx_pcs, pcs);
}

static void mxl862xx_pcs_get_state(struct phylink_pcs *pcs,
				 struct phylink_link_state *state)
{
	struct mxl862xx_priv *priv = pcs_to_mxl862xx_pcs(pcs)->priv;
	int port = pcs_to_mxl862xx_pcs(pcs)->port;

	struct mxl862xx_port_link_cfg port_link_cfg = {
		.port_id = port,
	};
	struct mxl862xx_port_cfg port_cfg = {
		.port_id = port,
	};
	int ret;

	ret = MXL862XX_API_READ(priv, MXL862XX_COMMON_PORTLINKCFGGET, port_link_cfg);
	if (ret) {
		dev_err(priv->dev, "failed to read link configuration on port %d\n", port);
		return;
	}
	ret = MXL862XX_API_READ(priv, MXL862XX_COMMON_PORTCFGGET, port_cfg);
	if (ret) {
		dev_err(priv->dev, "failed to read configuration on port %d\n", port);
		return;
	}

	if (port_link_cfg.link == MXL862XX_PORT_LINK_UP)
		state->link = 1;
	else
		state->link = 0;
	state->an_complete = state->link;

	switch (port_link_cfg.speed) {
	case MXL862XX_PORT_SPEED_10:
		state->speed = SPEED_10;
		break;
	case MXL862XX_PORT_SPEED_100:
		state->speed = SPEED_100;
		break;
	case MXL862XX_PORT_SPEED_1000:
		state->speed = SPEED_1000;
		break;
	case MXL862XX_PORT_SPEED_2500:
		state->speed = SPEED_2500;
		break;
	case MXL862XX_PORT_SPEED_5000:
		state->speed = SPEED_5000;
		break;
	case MXL862XX_PORT_SPEED_10000:
		state->speed = SPEED_10000;
		break;
	default:
		state->speed = SPEED_UNKNOWN;
		dev_err(priv->dev, "unsupported links speed on port %d\n", port);
		break;
	}

	switch (port_link_cfg.duplex) {
	case MXL862XX_DUPLEX_HALF:
		state->duplex = DUPLEX_HALF;
		break;
	case MXL862XX_DUPLEX_FULL:
		state->duplex = DUPLEX_FULL;
		break;
	default:
		state->duplex = DUPLEX_UNKNOWN;
		break;
	}

	state->pause &= ~(MLO_PAUSE_RX | MLO_PAUSE_TX);
	switch (port_cfg.flow_ctrl) {
	case MXL862XX_FLOW_RXTX:
		state->pause |= MLO_PAUSE_TXRX_MASK;
		break;
	case MXL862XX_FLOW_TX:
		state->pause |= MLO_PAUSE_TX;
		break;
	case MXL862XX_FLOW_RX:
		state->pause |= MLO_PAUSE_RX;
		break;
	case MXL862XX_FLOW_OFF:
	default:
		break;
	}
}

static int mxl862xx_pcs_config(struct phylink_pcs *pcs, unsigned int neg_mode,
			     phy_interface_t interface,
			     const unsigned long *advertising,
			     bool permit_pause_to_mac)
{
	return 0;
}

static const struct phylink_pcs_ops mxl862xx_pcs_ops = {
	.pcs_get_state = mxl862xx_pcs_get_state,
	.pcs_config = mxl862xx_pcs_config,
};

static void mxl862xx_setup_pcs(struct mxl862xx_priv *priv, struct mxl862xx_pcs *pcs,
			    int port)
{
	pcs->pcs.ops = &mxl862xx_pcs_ops;

	/* poll link changes */
	pcs->pcs.poll = true;
	pcs->priv = priv;
	pcs->port = port;
}

#define MXL862XX_READY_TIMEOUT_MS	10000
#define MXL862XX_READY_POLL_MS		100

static int mxl862xx_wait_ready(struct mxl862xx_priv *priv)
{
	struct mxl862xx_sys_fw_image_version ver = {};
	struct mxl862xx_cfg cfg = {};
	unsigned long start = jiffies;
	unsigned long timeout = start + msecs_to_jiffies(MXL862XX_READY_TIMEOUT_MS);
	int ret;

	do {
		ret = MXL862XX_API_READ(priv, SYS_MISC_FW_VERSION, ver);
		if (ret == 0 && (ver.iv_major || ver.iv_minor)) {
			ret = MXL862XX_API_READ(priv, MXL862XX_COMMON_CFGGET, cfg);
			if (ret == 0) {
				dev_info(priv->dev, "switch ready after %u ms, firmware %u.%u.%u (build %u)\n",
					 jiffies_to_msecs(jiffies - start),
					 ver.iv_major, ver.iv_minor,
					 le16_to_cpu(ver.iv_revision),
					 le32_to_cpu(ver.iv_build_num));
				return 0;
			}
		}
		msleep(MXL862XX_READY_POLL_MS);
	} while (time_before(jiffies, timeout));

	dev_err(priv->dev, "switch not responding after reset\n");
	return -ETIMEDOUT;
}

static int mxl862xx_setup(struct dsa_switch *ds)
{
	struct mxl862xx_priv *priv = ds->priv;
	unsigned int cpu_port, j;
	int ret;
	u8 i;
	struct mxl862xx_bridge_port_config br_port_cfg = {
		.mask = MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP,
	};

	gpiod_set_value_cansleep(priv->reset, 1);
	usleep_range(5000, 5100);
	gpiod_set_value_cansleep(priv->reset, 0);

	ret = mxl862xx_wait_ready(priv);
	if (ret)
		return ret;

	ret = mxl862xx_firmware_load(priv);
	if (ret)
		dev_warn(ds->dev, "firmware load failed: %d, continuing\n", ret);

	priv->user_pnum = 0;
	for (j = 0; j < ds->num_ports; j++) {
		if (dsa_is_user_port(ds, j))
			priv->user_pnum++;
		else if (dsa_is_cpu_port(ds, j))
			priv->cpu_port = cpu_port = j;

		if (DSA_MXL_PORT(j) == 13) {
			mxl862xx_setup_pcs(priv, &priv->pcs_port_1, 13);
		}
	}
	dev_info(ds->dev, "\tMxl862xx CPU Port %u, User Port number %u\n",
		 cpu_port, priv->user_pnum);

	ret = mxl862xx_setup_mdio(ds);
	if (ret) {
		dev_err(ds->dev, "failed to setup the mdio bus\n");
		return ret;
	}

	priv->port_info[priv->cpu_port].tag_protocol = mxl862_parse_tag_proto(ds, priv->cpu_port);

	mxl862xx_mac_learning(ds, cpu_port, true);

	for (i = 1; i < MAX_BRIDGES; i++)
		priv->bridge_portmap[i] = BIT(DSA_MXL_PORT(cpu_port));

	mxl862xx_set_vlan_filter_limits(ds);
	for (i = 0; i < MAX_VLANS; i++)
		priv->port_info[cpu_port].vlan.egress_vlan_block_info.vlans[i].untagged = true;

	for (i = 0; i < priv->hw_info->max_ports; i++) {
		if (dsa_is_unused_port(ds, i))
			continue;

		priv->port_info[i].vlan.filtering_mode_locked = false;
		priv->port_info[i].isolated = false;

		if (dsa_is_cpu_port(ds, i)) {
			continue;
		}

		mxl862xx_port_state(ds, i, false);
		mxl862xx_isolate_port(ds, i);
		mxl862xx_port_fast_age(ds, i);
		priv->bridge_portmap[0] |= BIT(DSA_MXL_PORT(i));
	}

	/* Update CPU bridge port */
	br_port_cfg.bridge_port_id = DSA_MXL_PORT(cpu_port),
	br_port_cfg.bridge_port_map[0] = priv->bridge_portmap[0];
	ret = MXL862XX_API_WRITE(priv, MXL862XX_BRIDGEPORT_CONFIGSET, br_port_cfg);
	if (ret) {
		dev_err(ds->dev, "failed to set the cpu portmap\n");
		return ret;
	}

	mxl862xx_port_fast_age(ds, cpu_port);

	return 0;
}

static void mxl862xx_port_stp_state_set(struct dsa_switch *ds, int port,
					u8 state)
{
	struct mxl862xx_stp_port_cfg param = {
		.port_id = DSA_MXL_PORT(port),
	};
	struct mxl862xx_priv *priv = ds->priv;
	int ret;

	switch (state) {
	case BR_STATE_DISABLED:
		param.port_state = MXL862XX_STP_PORT_STATE_DISABLE;
		return;
	case BR_STATE_BLOCKING:
	case BR_STATE_LISTENING:
		param.port_state = MXL862XX_STP_PORT_STATE_BLOCKING;
		break;
	case BR_STATE_LEARNING:
		param.port_state = MXL862XX_STP_PORT_STATE_LEARNING;
		break;
	case BR_STATE_FORWARDING:
		param.port_state = MXL862XX_STP_PORT_STATE_FORWARD;
		break;
	default:
		dev_err(priv->dev, "invalid STP state: %d\n", state);
		return;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_STP_PORTCFGSET, param);
	if (ret)
		dev_err(ds->dev, "failed to set STP state on port %d\n", port);

	if (!priv->port_info[port].bridge || dsa_is_cpu_port(ds, port))
		mxl862xx_mac_learning(ds, port, false);
}

static void mxl862xx_phylink_get_caps(struct dsa_switch *ds, int port,
				      struct phylink_config *config)
{
	struct mxl862xx_priv *priv = ds->priv;

	config->mac_capabilities = MAC_ASYM_PAUSE | MAC_SYM_PAUSE | MAC_10 |
				   MAC_100 | MAC_1000 | MAC_2500FD;

	if ((port >= 0 && port < priv->hw_info->phy_ports) ||
	    (port >= 8 && priv->hw_info->ext_ports >= 7)) {
		__set_bit(PHY_INTERFACE_MODE_INTERNAL, config->supported_interfaces);
	} else if (port >= 8 && priv->hw_info->ext_ports == 2) {
		__set_bit(PHY_INTERFACE_MODE_SGMII, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_1000BASEX, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_2500BASEX, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_10GBASER, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_USXGMII, config->supported_interfaces);
		config->mac_capabilities |= MAC_10000FD;
	} else {
		__set_bit(PHY_INTERFACE_MODE_NA, config->supported_interfaces);
	}
}

static void mxl862xx_phylink_mac_config(struct phylink_config *config, unsigned int mode,
					const struct phylink_link_state *state)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	int hw_port = DSA_MXL_PORT(dp->index);
	int ret;

	if (dsa_is_cpu_port(dp->ds, dp->index))
		return;

	if (hw_port == 9 || hw_port == 13) {
		struct mxl862xx_sys_sfp_cfg ser_intf = {
			.option = 0,
			.mode = 1,
		};

		if (hw_port == 9)
			ser_intf.port_id = 0;
		else
			ser_intf.port_id = 1;

		/** select speed when mode is 1
		 *	0 - 10G Quad USXGMII
		*	1 - 1000BaseX ANeg
		*	2 - 10G	XFI
		*	3 - 10G Single USXGMII
		*	4 - 2.5G SGMII
		*	5 - 2500 Single USXGMI
		*	6 - 2500BaseX NonANeg
		*	7 - 1000BaseX NonANeg
		*	8 - 1G SGMI
		*/
		switch (state->interface) {
		case PHY_INTERFACE_MODE_SGMII:
			ser_intf.speed = 8;
			break;
		case PHY_INTERFACE_MODE_1000BASEX:
			ser_intf.speed = 7;
			break;
		case PHY_INTERFACE_MODE_2500BASEX:
			ser_intf.speed = 4;
			break;
		case PHY_INTERFACE_MODE_10GBASER:
			ser_intf.speed = 2;
			break;
		case PHY_INTERFACE_MODE_USXGMII:
			ser_intf.speed = 3;
			break;
		default:
			dev_err(dp->ds->dev, "Unsupported interface: %d\n", state->interface);
			return;
		}

		ret = MXL862XX_API_WRITE(dp->ds->priv, SYS_MISC_SFP_SET, ser_intf);
		if (ret)
			dev_err(dp->ds->dev, "failed to set intf on port %d\n", dp->index);
	} else {
		/* Internal phy */
		if (state->interface != PHY_INTERFACE_MODE_INTERNAL) {
			dev_err(dp->ds->dev, "Unsupported interface: %d\n", state->interface);
			return;
		}
	}
}

static struct phylink_pcs *
mxl862xx_phylink_mac_select_pcs(struct phylink_config *config,
					      phy_interface_t interface)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct mxl862xx_priv *priv = dp->ds->priv;
	struct phylink_pcs *pcs = NULL;

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
	case PHY_INTERFACE_MODE_10GBASER:
	case PHY_INTERFACE_MODE_USXGMII:
		switch (DSA_MXL_PORT(dp->index)) {
		case 13:
			pcs = &priv->pcs_port_1.pcs;
			break;
		}
		break;

	default:
		break;
	}

	return pcs;
}

static void mxl862xx_phylink_mac_link_down(struct phylink_config *config, unsigned int mode,
					   phy_interface_t interface)
{
	/* MxL862xx system automatically synchronize the state between MAC link and PHY link or Serdes link*/
	return;
}

static void mxl862xx_phylink_mac_link_up(struct phylink_config *config,
					 struct phy_device *phydev, unsigned int mode,
					 phy_interface_t interface, int speed, int duplex,
					 bool tx_pause, bool rx_pause)
{
	/* MxL862xx system automatically synchronize the state between MAC link and PHY link or Serdes link*/
	return;
}

static void mxl862xx_get_ethtool_stats(struct dsa_switch *ds, int port,
				       uint64_t *data)
{
	struct mxl862xx_rmon_port_cnt param = {
		.port_type = MXL862XX_CTP_PORT,
		.port_id = DSA_MXL_PORT(port),
	};
	int ret, i;

	ret = MXL862XX_API_READ(ds->priv, MXL862XX_RMON_PORT_GET, param);
	if (ret) {
		dev_err(ds->dev, "failed to read RMON stats on port %d\n", port);
		return;
	}

	for (i = 0; i < ARRAY_SIZE(mxl862xx_mib); i++) {
		const struct mxl862xx_mib_desc *mib = &mxl862xx_mib[i];
		void *field = (u8 *)&param + mib->offset;

		if (mib->size == 1)
			*data++ = le32_to_cpu(*(__le32 *)field);
		else
			*data++ = le64_to_cpu(*(__le64 *)field);
	}
}

static void mxl862xx_get_strings(struct dsa_switch *ds, int port,
				 u32 stringset, u8 *data)
{
	u8 i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < ARRAY_SIZE(mxl862xx_mib); i++)
		ethtool_puts(&data, mxl862xx_mib[i].name);
}

static int mxl862xx_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return 0;

	return ARRAY_SIZE(mxl862xx_mib);
}

static int mxl862xx_port_mirror_add(struct dsa_switch *ds, int port,
				    struct dsa_mall_mirror_tc_entry *mirror,
				    bool ingress, struct netlink_ext_ack *extack)
{
	struct mxl862xx_ctp_port_config ctp_port = {
		.logical_port_id = DSA_MXL_PORT(port),
		.mask = MXL862XX_CTP_PORT_CONFIG_MASK_ALL,
	};
	struct mxl862xx_monitor_port_cfg monitor_port = {
		.port_id = DSA_MXL_PORT(mirror->to_local_port),
	};
	struct mxl862xx_priv *priv = ds->priv;
	int ret;

	ret = MXL862XX_API_READ(priv, MXL862XX_CTP_PORTCONFIGGET, ctp_port);
	if (ret) {
		dev_err(ds->dev, "failed to enable mirroring on port %d\n", port);
		return ret;
	}

	ctp_port.mask = MXL862XX_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR;
	ctp_port.ingress_mirror_enable = ingress;
	ctp_port.egress_mirror_enable = !ingress;
	ret = MXL862XX_API_WRITE(priv, MXL862XX_CTP_PORTCONFIGSET, ctp_port);
	if (ret) {
		dev_err(ds->dev, "failed to enable monitoring on port %d\n", port);
		return ret;
	}

	ret = MXL862XX_API_WRITE(priv, MXL862XX_COMMON_MONITORPORTCFGSET, monitor_port);
	if (ret) {
		dev_err(ds->dev, "failed to setup monitor port\n");
		return ret;
	}

	if (mirror->ingress)
		priv->port_info[port].ingress_mirror_enabled = true;
	else
		priv->port_info[port].egress_mirror_enabled = true;

	return 0;
}

static void mxl862xx_port_mirror_del(struct dsa_switch *ds, int port,
				     struct dsa_mall_mirror_tc_entry *mirror)
{
	struct mxl862xx_ctp_port_config param = {
		.logical_port_id = DSA_MXL_PORT(port),
		.mask = MXL862XX_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR,
	};
	struct mxl862xx_priv *priv = ds->priv;
	u8 max_ports = priv->hw_info->max_ports;
	int active_mirrors = 0;
	int ret, i;

	if (mirror->ingress)
		priv->port_info[port].ingress_mirror_enabled = false;
	else
		priv->port_info[port].egress_mirror_enabled = false;

	param.ingress_mirror_enable = priv->port_info[port].ingress_mirror_enabled;
	param.egress_mirror_enable = priv->port_info[port].egress_mirror_enabled;
	ret = MXL862XX_API_WRITE(priv, MXL862XX_CTP_PORTCONFIGSET, param);
	if (ret) {
		dev_err(ds->dev, "failed to disable mirroring on port %d\n", port);
		return;
	}

	for (i = 0; i < max_ports; i++)
	{
		if (dsa_is_unused_port(ds, i))
			continue;

		if (priv->port_info[i].egress_mirror_enabled ||
		    priv->port_info[i].egress_mirror_enabled)
			active_mirrors = 1;
	}

	if (!active_mirrors) {
		struct mxl862xx_monitor_port_cfg monitor_port = { };

		ret = MXL862XX_API_WRITE(priv, MXL862XX_COMMON_MONITORPORTCFGSET, monitor_port);
		if (ret)
			dev_err(ds->dev, "failed to release monitor port\n");
	}
}

static int mxl862xx_port_fdb_add(struct dsa_switch *ds, int port,
				 const unsigned char *addr, u16 vid, struct dsa_db db)
{
	struct mxl862xx_mac_table_add param = {
		.port_id = DSA_MXL_PORT(port),
		.tci = 0,
		.static_entry = true,
	};
	struct mxl862xx_priv *priv = ds->priv;
	int ret, i;

	memcpy(param.mac, addr, ETH_ALEN);

	for (i = 0; i < priv->hw_info->max_ports; i++) {
		if (dsa_is_unused_port(ds, i))
			continue;

		if (!dsa_is_cpu_port(ds, port) && i != port)
			continue;

		if (priv->port_info[i].isolated)
			continue;

		param.fid = priv->port_info[i].bridge_id;
		ret = MXL862XX_API_READ(ds->priv, MXL862XX_MAC_TABLEENTRYADD, param);
		if (ret) {
			dev_err(ds->dev, "failed to add FDB entry on port %d / fid %d\n",
				port, param.fid);
			return ret;
		}
	}

	return 0;
}

static int mxl862xx_port_fdb_del(struct dsa_switch *ds, int port,
				 const unsigned char *addr, u16 vid, struct dsa_db db)
{
	struct mxl862xx_mac_table_remove param = {
		.tci = 0,
	};
	struct mxl862xx_priv *priv = ds->priv;
	int ret, i;

	memcpy(param.mac, addr, ETH_ALEN);
	for (i = 0; i < priv->hw_info->max_ports; i++) {
		if (dsa_is_unused_port(ds, i))
			continue;

		if (!dsa_is_cpu_port(ds, port) && i != port)
			continue;

		param.fid = priv->port_info[i].bridge_id;
		ret = MXL862XX_API_READ(priv, MXL862XX_MAC_TABLEENTRYREMOVE, param);
		if (ret) {
			dev_err(ds->dev, "failed to remove FDB entry on port %d / fid %d\n",
				port, param.fid);
			return ret;
		}
	}

	return 0;
}

static int mxl862xx_port_fdb_dump(struct dsa_switch *ds, int port,
				  dsa_fdb_dump_cb_t *cb, void *data)
{
	struct mxl862xx_mac_table_read param = {
		.initial = 1,
	};
	int ret;

	while (true) {
		ret = MXL862XX_API_READ(ds->priv, MXL862XX_MAC_TABLEENTRYREAD, param);
		if (ret) {
			dev_err(ds->dev, "failed to read FDB entries on port %d\n", port);
			return ret;
		}

		if (param.last == 1)
			break;

		if (param.port_id == DSA_MXL_PORT(port))
			cb(param.mac, param.tci & 0x0FFF, param.static_entry, data);

		memset(&param, 0, sizeof(param));
	}

	return 0;
}

static int mxl862xx_port_pre_bridge_flags(struct dsa_switch *ds, int port,
					  struct switchdev_brport_flags flags,
					  struct netlink_ext_ack *extack)
{
	if (flags.mask & ~(BR_FLOOD | BR_MCAST_FLOOD | BR_BCAST_FLOOD |
			   BR_LEARNING | BR_PORT_LOCKED)) {
		dev_err(ds->dev, "unsupported bridge flags on port %d\n", port);
		return -EINVAL;
	}

	return 0;
}

static int mxl862xx_port_bridge_flags(struct dsa_switch *ds, int port,
				      struct switchdev_brport_flags flags,
				      struct netlink_ext_ack *extack)
{
	struct mxl862xx_priv *priv = ds->priv;
	bool bridge_ctx = true;
	u16 bridge_id;
	int ret = 0;

	if (!dsa_is_user_port(ds, port))
		return 0;

	bridge_id = priv->port_info[port].bridge_id;
	if (!bridge_id || !priv->port_info[port].bridge)
		bridge_ctx = false;

	if (bridge_ctx && (flags.mask & (BR_FLOOD | BR_MCAST_FLOOD | BR_BCAST_FLOOD))) {
		struct mxl862xx_bridge_config bridge_config = { };

		bridge_config.mask = MXL862XX_BRIDGE_CONFIG_MASK_FORWARDING_MODE;
		bridge_config.bridge_id = bridge_id;

		if (flags.mask & BR_FLOOD)
			bridge_config.forward_unknown_unicast = (flags.val & BR_FLOOD) ?
				MXL862XX_BRIDGE_FORWARD_FLOOD : MXL862XX_BRIDGE_FORWARD_DISCARD;

		if (flags.mask & BR_MCAST_FLOOD) {
			bridge_config.forward_unknown_multicast_ip = (flags.val & BR_MCAST_FLOOD) ?
				MXL862XX_BRIDGE_FORWARD_FLOOD : MXL862XX_BRIDGE_FORWARD_DISCARD;
			bridge_config.forward_unknown_multicast_non_ip =
				bridge_config.forward_unknown_multicast_ip;
		}

		if (flags.mask & BR_BCAST_FLOOD)
			bridge_config.forward_broadcast = (flags.val & BR_BCAST_FLOOD) ?
				MXL862XX_BRIDGE_FORWARD_FLOOD : MXL862XX_BRIDGE_FORWARD_DISCARD;

		ret = MXL862XX_API_WRITE(ds->priv, MXL862XX_BRIDGE_CONFIGSET, bridge_config);
		if (ret) {
			dev_err(ds->dev, "%s: Port:%d bridge:%d configuration  failed\n",
				__func__, port, bridge_config.bridge_id);
			return ret;
		}
	}

	if (flags.mask & BR_LEARNING)
		mxl862xx_mac_learning(ds, port, flags.val & BR_LEARNING);

	return 0;
}

static enum dsa_tag_protocol mxl862xx_get_tag_protocol(struct dsa_switch *ds,
						       int port, enum dsa_tag_protocol m)
{
/* ToDo */
	return mxl862_parse_tag_proto(ds, port);
}

static int mxl862xx_change_tag_protocol(struct dsa_switch *ds,
				     enum dsa_tag_protocol proto)
{
/* ToDo */
	return 0;
};

static const struct phylink_mac_ops mxl862xx_phylink_mac_ops = {
	.mac_config = mxl862xx_phylink_mac_config,
	.mac_link_down = mxl862xx_phylink_mac_link_down,
	.mac_link_up = mxl862xx_phylink_mac_link_up,
	.mac_select_pcs = mxl862xx_phylink_mac_select_pcs,
};

static const struct dsa_switch_ops mxl862xx_switch_ops = {
	.get_ethtool_stats = mxl862xx_get_ethtool_stats,
	.get_strings = mxl862xx_get_strings,
	.get_sset_count = mxl862xx_get_sset_count,
	.change_tag_protocol = mxl862xx_change_tag_protocol,
	.get_tag_protocol = mxl862xx_get_tag_protocol,
	.phy_read = mxl862xx_phy_read,
	.phy_write = mxl862xx_phy_write,
	.phylink_get_caps = mxl862xx_phylink_get_caps,
	.set_ageing_time = mxl862xx_set_ageing_time,
	.port_bridge_join = mxl862xx_port_bridge_join,
	.port_bridge_leave = mxl862xx_port_bridge_leave,
	.port_disable = mxl862xx_port_disable,
	.port_enable = mxl862xx_port_enable,
	.port_fast_age = mxl862xx_port_fast_age,
	.port_stp_state_set = mxl862xx_port_stp_state_set,
	.port_mirror_add = mxl862xx_port_mirror_add,
	.port_mirror_del = mxl862xx_port_mirror_del,
	.port_vlan_filtering = mxl862xx_port_vlan_filtering,
	.port_vlan_add = mxl862xx_port_vlan_add,
	.port_vlan_del = mxl862xx_port_vlan_del,
	.port_fdb_add = mxl862xx_port_fdb_add,
	.port_fdb_del = mxl862xx_port_fdb_del,
	.port_fdb_dump = mxl862xx_port_fdb_dump,
	.port_pre_bridge_flags = mxl862xx_port_pre_bridge_flags,
	.port_bridge_flags = mxl862xx_port_bridge_flags,
	.setup = mxl862xx_setup,
};

static int mxl862xx_probe(struct mdio_device *mdiodev)
{
	struct mxl862xx_sys_fw_image_version fw_version;
	struct device *dev = &mdiodev->dev;
	struct mxl862xx_priv *priv;
	struct dsa_switch *ds;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;
	priv->bus = mdiodev->bus;
	priv->sw_addr = mdiodev->addr;
	priv->hw_info = of_device_get_match_data(dev);
	if (!priv->hw_info)
		return -EINVAL;

	if (of_property_read_bool(dev->of_node, "c22-extended")) {
		priv->c22_extended = true;
		dev_info(dev, "%s:%u: Enable c22 extended", __func__, __LINE__);
	}

	mutex_init(&priv->pce_table_lock);

	ds = devm_kzalloc(dev, sizeof(*ds), GFP_KERNEL);
	if (!ds)
		return -ENOMEM;

	priv->ds = ds;
	ds->dev = dev;
	ds->priv = priv;
	ds->ops = &mxl862xx_switch_ops;
	ds->phylink_mac_ops = &mxl862xx_phylink_mac_ops;
	ds->num_ports = priv->hw_info->max_ports;
	ds->assisted_learning_on_cpu_port = true;

	dev_set_drvdata(dev, ds);

	priv->reset = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(priv->reset)) {
		dev_err(dev, "Couldn't get our reset line\n");
		return PTR_ERR(priv->reset);
	}

	ret = dsa_register_switch(ds);
	if (ret) {
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "failed to register DSA switch\n");
		return ret;
	}

	if (!dsa_is_cpu_port(ds, priv->cpu_port)) {
		dev_err(dev,
			"wrong CPU port defined, HW only supports port: %i",
			priv->cpu_port);
		dsa_unregister_switch(ds);
		return -EINVAL;
	}

	ret = MXL862XX_API_READ(priv, SYS_MISC_FW_VERSION, fw_version);
	if (ret) {
		dev_err(dev, "failed to read firmware version\n");
		return -EINVAL;
	}

	dev_info(dev, "Firmware version %d.%d.%d.%d",
		 fw_version.iv_major, fw_version.iv_minor,
		 fw_version.iv_revision, fw_version.iv_build_num);

	if (of_property_read_bool(dev->of_node, "force-isolate")) {
		priv->force_isolate = true;
		dev_info(dev, "%s:%u: Enable force isolate", __func__, __LINE__);
	}

	return 0;
}

static void mxl862xx_remove(struct mdio_device *mdiodev)
{
	struct dsa_switch *ds = dev_get_drvdata(&mdiodev->dev);

	dsa_unregister_switch(ds);
}

static const struct mxl862xx_hw_info mxl86282_data = {
	.max_ports = MXL862XX_MAX_PORT_NUM,
	.phy_ports = MXL86282_PHY_PORT_NUM,
	.ext_ports = MXL86282_EXT_PORT_NUM,
};

static const struct mxl862xx_hw_info mxl86252_data = {
	.max_ports = MXL862XX_MAX_PORT_NUM,
	.phy_ports = MXL86252_PHY_PORT_NUM,
	.ext_ports = MXL86252_EXT_PORT_NUM,
};

static const struct of_device_id mxl862xx_of_match[] = {
	{ .compatible = "maxlinear,mxl86282", .data = &mxl86282_data },
	{ .compatible = "maxlinear,mxl86252", .data = &mxl86252_data },
	{ /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, mxl862xx_of_match);

static struct mdio_driver mxl862xx_driver = {
	.probe  = mxl862xx_probe,
	.remove = mxl862xx_remove,
	.mdiodrv.driver = {
		.name = "mxl862xx",
		.of_match_table = mxl862xx_of_match,
	},
};

mdio_module_driver(mxl862xx_driver);

MODULE_DESCRIPTION("Driver for MaxLinear MxL862xx switch family");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:mxl862xx");
