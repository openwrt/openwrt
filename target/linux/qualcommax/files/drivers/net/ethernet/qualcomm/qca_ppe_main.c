// SPDX-License-Identifier: GPL-2.0-or-later OR MIT

#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_net.h>
#include <linux/pcs/pcs.h>
#include <linux/pcs/pcs-qca-uniphy.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/version.h>

#include "qca_ppe.h"

/* A port carrying nothing but minimum-sized frames at 10G wraps a packet
 * counter in under five minutes, and a total only stays monotonic for as long
 * as no counter wraps twice unobserved between folds.
 */
#define PPE_MIB_FOLD_INTERVAL	(30 * HZ)

static void ppe_port_gmac_set(struct qca_ppe_priv *priv, int port,
			     bool tx_en, bool rx_en)
{
	int gmac = port - 1;
	u32 val = 0;

	if (port < 1 || port >= priv->data->num_ports)
		return;

	priv->port_is_xgmac[port] = false;

	if (tx_en)
		val |= PPE_MAC_ENABLE_TXMAC_EN;
	if (rx_en)
		val |= PPE_MAC_ENABLE_RXMAC_EN;
	regmap_update_bits(priv->regmap, PPE_GMAC_ENABLE(gmac),
			   PPE_MAC_ENABLE_TXMAC_EN | PPE_MAC_ENABLE_RXMAC_EN,
			   val);
}

static void ppe_port_xgmac_set(struct qca_ppe_priv *priv, int port,
			       bool tx_en, bool rx_en)
{
	int xgmac = port - 5;

	if (port < 5 || port >= priv->data->num_ports)
		return;

	priv->port_is_xgmac[port] = true;

	regmap_update_bits(priv->regmap, PPE_XGMAC_TX_CONF(xgmac),
			   PPE_XGMAC_TX_ENABLE,
			   tx_en ? PPE_XGMAC_TX_ENABLE : 0);

	regmap_update_bits(priv->regmap, PPE_XGMAC_RX_CONF(xgmac),
			   PPE_XGMAC_RX_ENABLE,
			   rx_en ? PPE_XGMAC_RX_ENABLE : 0);
}

static void ppe_port_bridge_txmac_set(struct qca_ppe_priv *priv, int port,
				      bool enable)
{
	regmap_update_bits(priv->regmap, PPE_PORT_BRIDGE_CTRL(port),
			   PPE_PORT_BRIDGE_CTRL_TXMAC_EN,
			   enable ? PPE_PORT_BRIDGE_CTRL_TXMAC_EN : 0);
}

static void ppe_gmac_link_up(struct qca_ppe_priv *priv, int port,
			     int speed, int duplex,
			     bool tx_pause, bool rx_pause)
{
	int gmac = port - 1;
	u32 val;

	regmap_read(priv->regmap, PPE_GMAC_SPEED(gmac), &val);
	val &= ~PPE_GMAC_SPEED_MASK;
	switch (speed) {
	case SPEED_100:
		val |= FIELD_PREP(PPE_GMAC_SPEED_MASK, 1);
		break;
	case SPEED_2500:
	case SPEED_1000:
		val |= FIELD_PREP(PPE_GMAC_SPEED_MASK, 2);
		break;
	}
	regmap_write(priv->regmap, PPE_GMAC_SPEED(gmac), val);

	val = 0;
	if (duplex == DUPLEX_FULL)
		val |= PPE_MAC_ENABLE_DUPLEX;
	if (tx_pause)
		val |= PPE_MAC_ENABLE_TX_FLOW_EN;
	if (rx_pause)
		val |= PPE_MAC_ENABLE_RX_FLOW_EN;
	regmap_update_bits(priv->regmap, PPE_GMAC_ENABLE(gmac),
			   PPE_MAC_ENABLE_DUPLEX | PPE_MAC_ENABLE_TX_FLOW_EN |
			   PPE_MAC_ENABLE_RX_FLOW_EN, val);
}

static void ppe_xgmac_link_up(struct qca_ppe_priv *priv, int port,
			      phy_interface_t interface, int speed,
			      bool tx_pause, bool rx_pause)
{
	int xgmac = port - 5;
	u32 val;

	switch (speed) {
	case SPEED_10:
	case SPEED_100:
	case SPEED_1000:
		val = PPE_XGMAC_SPEED_SELECT_1000;
		break;
	case SPEED_2500:
		val = PPE_XGMAC_SPEED_SELECT_2500;
		break;
	case SPEED_5000:
		val = PPE_XGMAC_SPEED_SELECT_5000;
		break;
	case SPEED_10000:
		val = PPE_XGMAC_SPEED_SELECT_10000;
		break;
	default:
		return;
	}

	if (interface == PHY_INTERFACE_MODE_USXGMII ||
	    interface == PHY_INTERFACE_MODE_10GBASER) {
		switch (speed) {
		case SPEED_2500:
		case SPEED_5000:
		case SPEED_10000:
			val |= PPE_XGMAC_USXGMII_SELECT;
			break;
		default:
			break;
		}
	}

	regmap_update_bits(priv->regmap, PPE_XGMAC_TX_CONF(xgmac),
			   PPE_XGMAC_SPEED_SELECT |
			   PPE_XGMAC_USXGMII_SELECT, val);

	regmap_write_bits(priv->regmap, PPE_XGMAC_RX_CONF(xgmac),
			   PPE_XGMAC_AUTO_CRC_STRIP |
			   PPE_XGMAC_CRC_STRIP_TYPE,
			   PPE_XGMAC_AUTO_CRC_STRIP |
			   PPE_XGMAC_CRC_STRIP_TYPE);

	regmap_write_bits(priv->regmap, PPE_XGMAC_TX_FLOW_CTRL(xgmac),
			  PPE_XGMAC_TX_FLOW_ENABLE,
			  tx_pause ? PPE_XGMAC_TX_FLOW_ENABLE : 0);

	regmap_write_bits(priv->regmap, PPE_XGMAC_RX_FLOW_CTRL(xgmac),
			  PPE_XGMAC_RX_FLOW_ENABLE,
			  rx_pause ? PPE_XGMAC_RX_FLOW_ENABLE : 0);
}

static void ppe_port_cnt_enable(struct qca_ppe_priv *priv, int port)
{
	regmap_update_bits(priv->regmap,
			   PPE_MRU_MTU_CTRL(port,
					    priv->data->mru_mtu_ctrl_stride) + 4,
			   PPE_MRU_MTU_CTRL_RX_CNT_EN | PPE_MRU_MTU_CTRL_TX_CNT_EN,
			   PPE_MRU_MTU_CTRL_RX_CNT_EN | PPE_MRU_MTU_CTRL_TX_CNT_EN);

	regmap_update_bits(priv->regmap, PPE_MC_MTU_CTRL(port),
			   PPE_MC_MTU_CTRL_TX_CNT_EN, PPE_MC_MTU_CTRL_TX_CNT_EN);

	regmap_update_bits(priv->regmap, PPE_PORT_EG_VLAN(port),
			   PPE_PORT_EG_VLAN_TX_CNT_EN, PPE_PORT_EG_VLAN_TX_CNT_EN);
}

/* The sizes are word 0 of a two-word entry that latches on word 1, so editing
 * them alone leaves the write staged. Word 1 holds the counter enables and the
 * source profile and goes back unchanged.
 */
static void ppe_port_mtu_set(struct qca_ppe_priv *priv, int port,
			     u32 frame_size)
{
	u32 reg = PPE_MRU_MTU_CTRL(port, priv->data->mru_mtu_ctrl_stride);
	u32 w1;

	regmap_read(priv->regmap, reg + 4, &w1);
	regmap_write(priv->regmap, reg,
		     FIELD_PREP(PPE_MRU_MTU_CTRL_MRU, frame_size) |
		     FIELD_PREP(PPE_MRU_MTU_CTRL_MRU_CMD,
				PPE_MTU_CMD_RDT_TO_CPU) |
		     FIELD_PREP(PPE_MRU_MTU_CTRL_MTU, frame_size) |
		     FIELD_PREP(PPE_MRU_MTU_CTRL_MTU_CMD,
				PPE_MTU_CMD_RDT_TO_CPU));
	regmap_write(priv->regmap, reg + 4, w1);

	regmap_update_bits(priv->regmap, PPE_MC_MTU_CTRL(port),
			   PPE_MC_MTU_CTRL_MTU | PPE_MC_MTU_CTRL_MTU_CMD,
			   FIELD_PREP(PPE_MC_MTU_CTRL_MTU, frame_size) |
			   FIELD_PREP(PPE_MC_MTU_CTRL_MTU_CMD,
				      PPE_MTU_CMD_RDT_TO_CPU));
}

int ppe_vsi_alloc(struct qca_ppe_priv *priv)
{
	int vsi;

	lockdep_assert_held(&priv->vlan_lock);

	vsi = find_first_zero_bit(priv->vsi_bitmap, PPE_VSI_MAX);
	if (vsi >= PPE_VSI_MAX)
		return -ENOSPC;

	set_bit(vsi, priv->vsi_bitmap);
	priv->vsi_member[vsi] = 0;

	regmap_write(priv->regmap, PPE_VSI_TBL(vsi), 0);
	regmap_write(priv->regmap, PPE_VSI_TBL(vsi) + 4,
		     PPE_VSI_TBL_NEW_ADDR_LRN_EN | PPE_VSI_TBL_STA_MOVE_LRN_EN);

	return vsi;
}

void ppe_vsi_free(struct qca_ppe_priv *priv, u32 vsi)
{
	lockdep_assert_held(&priv->vlan_lock);

	regmap_write(priv->regmap, PPE_VSI_TBL(vsi), 0);
	regmap_write(priv->regmap, PPE_VSI_TBL(vsi) + 4, 0);
	priv->vsi_member[vsi] = 0;
	clear_bit(vsi, priv->vsi_bitmap);
}

/* The one member of a trunk that carries its flooded copies, so that a frame
 * flooded into a VSI crosses the aggregate once. A port outside every trunk
 * carries its own.
 */
static int ppe_trunk_flood_port(struct qca_ppe_priv *priv, int port)
{
	u8 members;
	int g;

	for (g = 0; g < PPE_TRUNK_GROUPS; g++) {
		if (!(priv->trunk_members[g] & BIT(port)))
			continue;

		members = priv->trunk_tx[g];
		if (!members)
			members = priv->trunk_members[g];

		return __ffs(members);
	}

	return port;
}

/* Each flood class is the member mask minus the ports whose bridge flags turned
 * that class off, so narrowing one costs nothing on the ports that still want
 * it. The member mask is remembered because a flag change has to reprogram a
 * VSI whose membership did not move.
 */
void ppe_vsi_member_set(struct qca_ppe_priv *priv, u32 vsi,
			       u32 portmask)
{
	u32 uuc = 0, umc = 0, bc = 0, val;
	int port;

	priv->vsi_member[vsi] = portmask;

	for (port = 0; port < priv->ds.num_ports; port++) {
		if (!(portmask & BIT(port)))
			continue;

		/* A trunk's other members leave the flood classes, never the
		 * member mask: the aggregate is one bridge port and must see
		 * one copy.
		 */
		if (ppe_trunk_flood_port(priv, port) != port)
			continue;

		if (priv->port_brflags[port] & BR_FLOOD)
			uuc |= BIT(port);
		if (priv->port_brflags[port] & BR_MCAST_FLOOD)
			umc |= BIT(port);
		if (priv->port_brflags[port] & BR_BCAST_FLOOD)
			bc |= BIT(port);
	}

	val = FIELD_PREP(PPE_VSI_TBL_MEMBER, portmask) |
	      FIELD_PREP(PPE_VSI_TBL_UUC, uuc) |
	      FIELD_PREP(PPE_VSI_TBL_UMC, umc) |
	      FIELD_PREP(PPE_VSI_TBL_BC, bc);
	regmap_write(priv->regmap, PPE_VSI_TBL(vsi), val);
	regmap_write(priv->regmap, PPE_VSI_TBL(vsi) + 4,
		PPE_VSI_TBL_NEW_ADDR_LRN_EN | PPE_VSI_TBL_STA_MOVE_LRN_EN);
}

/* VSI 0 is skipped: it carries the ports no bridge has claimed, whose flood set
 * is the host alone whatever their flags say.
 */
static void ppe_vsi_flood_refresh(struct qca_ppe_priv *priv)
{
	int vsi;

	for_each_set_bit(vsi, priv->vsi_bitmap, PPE_VSI_MAX)
		if (vsi)
			ppe_vsi_member_set(priv, vsi, priv->vsi_member[vsi]);
}

/* The bitmap names the ports a frame from this one may leave by, so an isolated
 * port is expressed by taking the other isolated ports out of its own.
 */
static void ppe_port_isolation_update(struct qca_ppe_priv *priv)
{
	u32 all = BIT(priv->ds.num_ports) - 1;
	u32 mask;
	int port;

	for (port = 0; port < priv->ds.num_ports; port++) {
		mask = all;
		if (priv->port_isolated & BIT(port))
			mask = (all & ~priv->port_isolated) |
			       BIT(QCA_PPE_CPU_PORT);

		regmap_update_bits(priv->regmap, PPE_PORT_BRIDGE_CTRL(port),
				   PPE_BRIDGE_PORT_ISOL,
				   FIELD_PREP(PPE_BRIDGE_PORT_ISOL, mask));
	}
}

/* The entry latches on the write to its last word: rewriting word 1 alone is
 * staged and never takes effect, so the whole entry is read and written back.
 */
static void ppe_port_vsi_set(struct qca_ppe_priv *priv, int port, u32 vsi)
{
	u32 val[3];
	int i;

	for (i = 0; i < ARRAY_SIZE(val); i++)
		regmap_read(priv->regmap, PPE_L3_VP_PORT_TBL(port) + i * 4,
			    &val[i]);

	val[1] &= ~(PPE_L3_VP_VSI_VALID | PPE_L3_VP_VSI);
	if (vsi != PPE_VSI_INVALID) {
		val[1] |= PPE_L3_VP_VSI_VALID;
		val[1] |= FIELD_PREP(PPE_L3_VP_VSI, vsi);
	}

	for (i = 0; i < ARRAY_SIZE(val); i++)
		regmap_write(priv->regmap, PPE_L3_VP_PORT_TBL(port) + i * 4,
			     val[i]);
}

static int ppe_fdb_op_wait(struct qca_ppe_priv *priv, u32 rslt_reg,
			   u32 cmd_id)
{
	u32 val;
	int i;

	for (i = 0; i < 100; i++) {
		regmap_read(priv->regmap, rslt_reg, &val);
		if (FIELD_GET(PPE_FDB_RSLT_CMD_ID, val) == cmd_id)
			return 0;
		udelay(1);
	}

	return -ETIMEDOUT;
}

static void ppe_fdb_encode(const unsigned char *addr, int port, u32 vsi,
			   bool is_static, u32 *data0, u32 *data1, u32 *data2)
{
	*data0 = (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5];

	*data1 = (addr[0] << 8) | addr[1];
	*data1 |= PPE_FDB_DATA1_VALID | PPE_FDB_DATA1_LKP_VALID;
	*data1 |= FIELD_PREP(PPE_FDB_DATA1_VSI, vsi);
	*data1 |= FIELD_PREP(PPE_FDB_DATA1_DST_LO, port);

	*data2 = FIELD_PREP(PPE_FDB_DATA2_DST_TYPE, PPE_FDB_DST_PORT) |
		 FIELD_PREP(PPE_FDB_DATA2_HIT_AGE,
			    is_static ? PPE_FDB_AGE_STATIC : 2);
}

static int ppe_fdb_op(struct qca_ppe_priv *priv, const unsigned char *addr,
		      int port, u32 vsi, u32 op_type)
{
	u32 data0, data1, data2;
	int ret;

	ppe_fdb_encode(addr, port, vsi, op_type == PPE_FDB_OP_ADD,
		       &data0, &data1, &data2);

	spin_lock_bh(&priv->fdb_lock);

	regmap_write(priv->regmap, PPE_FDB_OP_DATA0, data0);
	regmap_write(priv->regmap, PPE_FDB_OP_DATA1, data1);
	regmap_write(priv->regmap, PPE_FDB_OP_DATA2, data2);
	regmap_write(priv->regmap, PPE_FDB_OP,
		     FIELD_PREP(PPE_FDB_OP_TYPE, op_type) |
		     FIELD_PREP(PPE_FDB_OP_HASH_BLOCK, 3));

	ret = ppe_fdb_op_wait(priv, PPE_FDB_OP_RSLT, 0);

	spin_unlock_bh(&priv->fdb_lock);

	return ret;
}

static int ppe_fdb_read_entry(struct qca_ppe_priv *priv, u32 index,
			      unsigned char *addr, u32 *vsi, int *port,
			      bool *is_static)
{
	u32 data0, data1, data2, cmd_id, val;
	int ret;

	cmd_id = index % 15;

	spin_lock_bh(&priv->fdb_lock);

	regmap_write(priv->regmap, PPE_FDB_RD_OP_DATA0, 0);
	regmap_write(priv->regmap, PPE_FDB_RD_OP_DATA1, 0);
	regmap_write(priv->regmap, PPE_FDB_RD_OP_DATA2, 0);

	val = FIELD_PREP(PPE_FDB_OP_CMD_ID, cmd_id) |
	      FIELD_PREP(PPE_FDB_OP_TYPE, PPE_FDB_OP_GET) |
	      FIELD_PREP(PPE_FDB_OP_HASH_BLOCK, 3) |
	      PPE_FDB_OP_MODE |
	      FIELD_PREP(PPE_FDB_OP_ENTRY_IDX, index);
	regmap_write(priv->regmap, PPE_FDB_RD_OP, val);

	ret = ppe_fdb_op_wait(priv, PPE_FDB_RD_OP_RSLT, cmd_id);
	if (ret)
		goto unlock;

	regmap_read(priv->regmap, PPE_FDB_RD_RSLT_DATA0, &data0);
	regmap_read(priv->regmap, PPE_FDB_RD_RSLT_DATA1, &data1);
	regmap_read(priv->regmap, PPE_FDB_RD_RSLT_DATA2, &data2);

unlock:
	spin_unlock_bh(&priv->fdb_lock);

	if (ret)
		return ret;

	if (!(data1 & PPE_FDB_DATA1_VALID))
		return -ENOENT;

	if (FIELD_GET(PPE_FDB_DATA2_DST_TYPE, data2) != PPE_FDB_DST_PORT)
		return -ENOENT;

	addr[2] = (data0 >> 24) & 0xff;
	addr[3] = (data0 >> 16) & 0xff;
	addr[4] = (data0 >> 8) & 0xff;
	addr[5] = data0 & 0xff;
	addr[0] = (data1 >> 8) & 0xff;
	addr[1] = data1 & 0xff;

	*vsi = FIELD_GET(PPE_FDB_DATA1_VSI, data1);
	*port = FIELD_GET(PPE_FDB_DATA1_DST_LO, data1) |
		(FIELD_GET(PPE_FDB_DATA2_DST_HI, data2) << 9);
	*is_static = FIELD_GET(PPE_FDB_DATA2_HIT_AGE, data2) == PPE_FDB_AGE_STATIC;

	return 0;
}

static int ppe_fdb_flush(struct qca_ppe_priv *priv)
{
	int ret;

	spin_lock_bh(&priv->fdb_lock);

	regmap_write(priv->regmap, PPE_FDB_OP,
		FIELD_PREP(PPE_FDB_OP_TYPE, PPE_FDB_OP_FLUSH));

	ret = ppe_fdb_op_wait(priv, PPE_FDB_OP_RSLT, 0);

	spin_unlock_bh(&priv->fdb_lock);

	return ret;
}

static void ppe_fdb_encode_mcast(const unsigned char *addr, u32 portmap,
				 u32 vsi, u32 *data0, u32 *data1, u32 *data2)
{
	*data0 = (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5];

	*data1 = (addr[0] << 8) | addr[1];
	*data1 |= PPE_FDB_DATA1_VALID | PPE_FDB_DATA1_LKP_VALID;
	*data1 |= FIELD_PREP(PPE_FDB_DATA1_VSI, vsi);
	*data1 |= FIELD_PREP(PPE_FDB_DATA1_DST_LO, portmap);

	*data2 = FIELD_PREP(PPE_FDB_DATA2_DST_HI, portmap >> 9) |
		 FIELD_PREP(PPE_FDB_DATA2_DST_TYPE, PPE_FDB_DST_PORTMAP) |
		 FIELD_PREP(PPE_FDB_DATA2_HIT_AGE, PPE_FDB_AGE_STATIC);
}

static int ppe_fdb_lookup(struct qca_ppe_priv *priv,
			  const unsigned char *addr, u32 vsi, u32 *portmap)
{
	u32 data1, data2;
	int ret;

	spin_lock_bh(&priv->fdb_lock);

	regmap_write(priv->regmap, PPE_FDB_RD_OP_DATA0,
		     (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5]);
	regmap_write(priv->regmap, PPE_FDB_RD_OP_DATA1,
		     ((addr[0] << 8) | addr[1]) |
		     FIELD_PREP(PPE_FDB_DATA1_VSI, vsi));
	regmap_write(priv->regmap, PPE_FDB_RD_OP_DATA2, 0);

	regmap_write(priv->regmap, PPE_FDB_RD_OP,
		     FIELD_PREP(PPE_FDB_OP_TYPE, PPE_FDB_OP_GET) |
		     FIELD_PREP(PPE_FDB_OP_HASH_BLOCK, 3));

	ret = ppe_fdb_op_wait(priv, PPE_FDB_RD_OP_RSLT, 0);
	if (ret)
		goto out;

	regmap_read(priv->regmap, PPE_FDB_RD_RSLT_DATA1, &data1);
	regmap_read(priv->regmap, PPE_FDB_RD_RSLT_DATA2, &data2);

	/* The destination field of a unicast entry is a port number rather than
	 * a member map, so an entry of the wrong type read back as one would
	 * name a set of ports the group was never given.
	 */
	if (!(data1 & PPE_FDB_DATA1_VALID) ||
	    FIELD_GET(PPE_FDB_DATA2_DST_TYPE, data2) != PPE_FDB_DST_PORTMAP) {
		ret = -ENOENT;
		goto out;
	}

	*portmap = FIELD_GET(PPE_FDB_DATA1_DST_LO, data1) |
		   (FIELD_GET(PPE_FDB_DATA2_DST_HI, data2) << 9);

out:
	spin_unlock_bh(&priv->fdb_lock);
	return ret;
}

static int ppe_fdb_mcast_op(struct qca_ppe_priv *priv,
			    const unsigned char *addr, u32 portmap,
			    u32 vsi, u32 op_type)
{
	u32 data0, data1, data2;
	int ret;

	ppe_fdb_encode_mcast(addr, portmap, vsi, &data0, &data1, &data2);

	spin_lock_bh(&priv->fdb_lock);

	regmap_write(priv->regmap, PPE_FDB_OP_DATA0, data0);
	regmap_write(priv->regmap, PPE_FDB_OP_DATA1, data1);
	regmap_write(priv->regmap, PPE_FDB_OP_DATA2, data2);
	regmap_write(priv->regmap, PPE_FDB_OP,
		     FIELD_PREP(PPE_FDB_OP_TYPE, op_type) |
		     FIELD_PREP(PPE_FDB_OP_HASH_BLOCK, 3));

	ret = ppe_fdb_op_wait(priv, PPE_FDB_OP_RSLT, 0);

	spin_unlock_bh(&priv->fdb_lock);

	return ret;
}

static enum dsa_tag_protocol
qca_ppe_get_tag_protocol(struct dsa_switch *ds, int port,
			     enum dsa_tag_protocol mprot)
{
	return DSA_TAG_PROTO_OOB;
}

/* The tables an offloaded flow draws on, plus the classifier's. The hardware
 * counts none of them, so occupancy is the driver's own bookkeeping, which is
 * what makes "the table was full" a number rather than an inference from the
 * flow that stayed in software.
 */
enum ppe_devlink_resource_id {
	PPE_RESOURCE_FLOW = 1,
	PPE_RESOURCE_HOST,
	PPE_RESOURCE_NEXTHOP,
	PPE_RESOURCE_ACL,
};

static u64 ppe_devlink_flow_occ(void *p)
{
	struct qca_ppe_priv *priv = p;

	return atomic_read(&priv->flow_table.nelems);
}

static u64 ppe_devlink_host_occ(void *p)
{
	struct qca_ppe_priv *priv = p;
	u64 used = 0;
	u32 i;

	guard(mutex)(&priv->flow_lock);

	for (i = 0; i < priv->data->num_host_entries; i++)
		if (priv->host_ref[i])
			used++;

	return used;
}

static u64 ppe_devlink_nexthop_occ(void *p)
{
	struct qca_ppe_priv *priv = p;
	u64 used = 0;
	u32 i;

	guard(mutex)(&priv->flow_lock);

	for (i = 0; i < priv->data->num_nexthop_entries; i++)
		if (priv->nexthop[i].refcount)
			used++;

	return used;
}

static u64 ppe_devlink_acl_occ(void *p)
{
	struct qca_ppe_priv *priv = p;
	u64 free = 0;
	u32 i;

	guard(mutex)(&priv->acl_lock);

	for (i = 0; i < PPE_ACL_LISTS; i++)
		free += hweight8(priv->acl_free[i]);

	return PPE_ACL_LISTS * PPE_ACL_LIST_ENTRIES - free;
}

static int ppe_devlink_resource(struct dsa_switch *ds, const char *name,
				u64 size, u64 id,
				devlink_resource_occ_get_t *occ)
{
	struct devlink_resource_size_params params;
	int ret;

	/* Silicon geometry: the only size the resource can ever have. */
	devlink_resource_size_params_init(&params, size, size, 1,
					  DEVLINK_RESOURCE_UNIT_ENTRY);

	ret = dsa_devlink_resource_register(ds, name, size, id,
					    DEVLINK_RESOURCE_ID_PARENT_TOP,
					    &params);
	if (ret)
		return ret;

	dsa_devlink_resource_occ_get_register(ds, id, occ, ds_to_priv(ds));

	return 0;
}

static int ppe_devlink_setup(struct dsa_switch *ds)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	int ret;

	ret = ppe_devlink_resource(ds, "flow", priv->data->num_flow_entries,
				   PPE_RESOURCE_FLOW, ppe_devlink_flow_occ);
	if (!ret)
		ret = ppe_devlink_resource(ds, "host",
					   priv->data->num_host_entries,
					   PPE_RESOURCE_HOST,
					   ppe_devlink_host_occ);
	if (!ret)
		ret = ppe_devlink_resource(ds, "nexthop",
					   priv->data->num_nexthop_entries,
					   PPE_RESOURCE_NEXTHOP,
					   ppe_devlink_nexthop_occ);
	if (!ret)
		ret = ppe_devlink_resource(ds, "acl",
					   PPE_ACL_LISTS * PPE_ACL_LIST_ENTRIES,
					   PPE_RESOURCE_ACL,
					   ppe_devlink_acl_occ);
	if (!ret)
		ret = qca_ppe_devlink_sb_setup(ds);
	if (ret)
		dsa_devlink_resources_unregister(ds);

	return ret;
}

/* The part names itself in the first register of the global block. The vendor's
 * named accessor for it is compiled out; what proves the field split is its init
 * path reading the same word raw (ssdk_init.c chip_ver_get): device 0x15 is this
 * switch generation, revision 0 IPQ807x and 1 IPQ6018.
 */
static int qca_ppe_devlink_info_get(struct dsa_switch *ds,
				    struct devlink_info_req *req,
				    struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 id, dev, rev;
	char buf[8];
	int ret;

	ret = regmap_read(priv->regmap, PPE_SWITCH_ID, &id);
	if (ret)
		return ret;

	dev = FIELD_GET(PPE_SWITCH_ID_DEV, id);
	rev = FIELD_GET(PPE_SWITCH_ID_REV, id);

	snprintf(buf, sizeof(buf), "0x%02x", dev);
	ret = devlink_info_version_fixed_put(req,
					     DEVLINK_INFO_VERSION_GENERIC_ASIC_ID,
					     buf);
	if (ret)
		return ret;

	snprintf(buf, sizeof(buf), "0x%02x", rev);

	return devlink_info_version_fixed_put(req,
					      DEVLINK_INFO_VERSION_GENERIC_ASIC_REV,
					      buf);
}

static int qca_ppe_setup(struct dsa_switch *ds)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	int num_ports = ds->num_ports;
	u32 frame_size;
	u32 port_mask;
	u32 val;
	int i;

	port_mask = BIT(num_ports) - 1;
	frame_size = PPE_DEFAULT_MTU + 2 * VLAN_HLEN;

	for (i = 0; i < num_ports; i++)
		priv->port_vsi[i] = PPE_VSI_INVALID;

	regmap_write(priv->regmap, PPE_FDB_OP, 0);

	for (i = 0; i < num_ports; i++) {
		regmap_write(priv->regmap, PPE_CST_STATE(i), PPE_STP_FORWARDING);

		ppe_port_mtu_set(priv, i, frame_size);

		if (i >= 1)
			regmap_write(priv->regmap, PPE_GMAC_MIB_CTRL(i - 1),
				     PPE_MIB_EN);

		/* The state DSA gives a port no bridge has claimed: it floods,
		 * and it does not learn, so two of them cannot reach each
		 * other behind the CPU's back.
		 */
		priv->port_brflags[i] = BR_FLOOD | BR_MCAST_FLOOD |
					BR_BCAST_FLOOD;

		val = PPE_BRIDGE_STA_MOVE_EN |
		      FIELD_PREP(PPE_BRIDGE_PORT_ISOL, port_mask);
		if (dsa_is_cpu_port(ds, i)) {
			val |= PPE_PORT_BRIDGE_CTRL_TXMAC_EN |
			       PPE_BRIDGE_NEW_LRN_EN;
			priv->port_brflags[i] |= BR_LEARNING;
		}
		regmap_update_bits(priv->regmap, PPE_PORT_BRIDGE_CTRL(i),
				   PPE_BRIDGE_NEW_LRN_EN |
				   PPE_BRIDGE_STA_MOVE_EN |
				   PPE_BRIDGE_PORT_ISOL |
				   PPE_PORT_BRIDGE_CTRL_TXMAC_EN,
				   val);

		ppe_port_cnt_enable(priv, i);
	}

	qca_ppe_vlan_setup(ds);

	set_bit(0, priv->vsi_bitmap);
	val = FIELD_PREP(PPE_VSI_TBL_MEMBER,
			 dsa_user_ports(ds) | BIT(QCA_PPE_CPU_PORT)) |
	      FIELD_PREP(PPE_VSI_TBL_UUC, BIT(QCA_PPE_CPU_PORT)) |
	      FIELD_PREP(PPE_VSI_TBL_UMC, BIT(QCA_PPE_CPU_PORT)) |
	      FIELD_PREP(PPE_VSI_TBL_BC, BIT(QCA_PPE_CPU_PORT));
	regmap_write(priv->regmap, PPE_VSI_TBL(0), val);
	regmap_write(priv->regmap, PPE_VSI_TBL(0) + 4,
		PPE_VSI_TBL_NEW_ADDR_LRN_EN | PPE_VSI_TBL_STA_MOVE_LRN_EN);

	for (i = 1; i < num_ports; i++)
		ppe_port_vsi_set(priv, i, 0);

	ppe_fdb_flush(priv);

	regmap_update_bits(priv->regmap, PPE_L2_GLOBAL_CONF,
			   PPE_L2_LRN_EN | PPE_L2_AGE_EN,
			   PPE_L2_LRN_EN | PPE_L2_AGE_EN);

	ds->ageing_time_min = PPE_AGE_UNIT_MS;
	ds->ageing_time_max = (unsigned int)min_t(u64,
		(u64)PPE_AGE_UNIT_MS * PPE_AGE_TIMER_MASK, U32_MAX);
	ds->assisted_learning_on_cpu_port = true;

	schedule_delayed_work(&priv->mib_work, PPE_MIB_FOLD_INTERVAL);

	return ppe_devlink_setup(ds);
}

static int qca_ppe_set_ageing_time(struct dsa_switch *ds, unsigned int msecs)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 timer = msecs / PPE_AGE_UNIT_MS;

	regmap_update_bits(priv->regmap, PPE_AGE_TIMER, PPE_AGE_TIMER_MASK,
			   FIELD_PREP(PPE_AGE_TIMER_MASK, timer));

	return 0;
}

static int qca_ppe_port_change_mtu(struct dsa_switch *ds, int port,
				   int new_mtu)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	ppe_port_mtu_set(priv, port, new_mtu + ETH_HLEN + 2 * VLAN_HLEN);

	return 0;
}

static int qca_ppe_port_max_mtu(struct dsa_switch *ds, int port)
{
	return PPE_MAX_FRAME_SIZE - ETH_HLEN - ETH_FCS_LEN -
	       2 * VLAN_HLEN;
}

static int qca_ppe_port_enable(struct dsa_switch *ds, int port,
				   struct phy_device *phy)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	ppe_port_queues_enable(priv, port, true);

	/* A user port's gate is opened by qca_ppe_mac_link_up() once its MAC
	 * is up. DSA calls this before phylink_start(), so opening it here
	 * would aim the fabric at a MAC that is still down and about to be
	 * re-clocked. The CPU port has no MAC of ours to wait for.
	 */
	if (dsa_is_cpu_port(ds, port))
		ppe_port_bridge_txmac_set(priv, port, true);

	return 0;
}

static void qca_ppe_port_disable(struct dsa_switch *ds, int port)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	ppe_port_bridge_txmac_set(priv, port, false);
	ppe_port_queues_enable(priv, port, false);
}

static struct qca_ppe_bridge_vsi *
bridge_vsi_find(struct qca_ppe_priv *priv, struct net_device *br_dev)
{
	int i;

	for (i = 0; i < QCA_PPE_MAX_BRIDGES; i++)
		if (priv->bridges[i].br_dev == br_dev)
			return &priv->bridges[i];

	return NULL;
}

static struct qca_ppe_bridge_vsi *
bridge_vsi_alloc(struct qca_ppe_priv *priv, struct net_device *br_dev)
{
	int vsi, i;

	vsi = ppe_vsi_alloc(priv);
	if (vsi < 0)
		return NULL;

	for (i = 0; i < QCA_PPE_MAX_BRIDGES; i++) {
		if (priv->bridges[i].br_dev)
			continue;

		priv->bridges[i].br_dev = br_dev;
		priv->bridges[i].vsi = vsi;
		priv->bridges[i].refcount = 0;
		return &priv->bridges[i];
	}

	ppe_vsi_free(priv, vsi);
	return NULL;
}

static void bridge_vsi_put(struct qca_ppe_priv *priv,
			   struct qca_ppe_bridge_vsi *bvsi)
{
	bvsi->refcount--;
	if (bvsi->refcount > 0)
		return;

	ppe_flow_purge_vsi(priv, bvsi->vsi);
	ppe_vsi_free(priv, bvsi->vsi);
	bvsi->br_dev = NULL;
	bvsi->vsi = 0;
}

static void bridge_vsi_members_update(struct qca_ppe_priv *priv,
				      struct qca_ppe_bridge_vsi *bvsi)
{
	u32 portmask = 0;
	int i;

	for (i = 0; i < priv->ds.num_ports; i++)
		if (priv->port_vsi[i] != PPE_VSI_INVALID &&
		    priv->port_vsi[i] == bvsi->vsi)
			portmask |= BIT(i);

	portmask |= BIT(QCA_PPE_CPU_PORT);

	ppe_vsi_member_set(priv, bvsi->vsi, portmask);
}

static int qca_ppe_port_bridge_join(struct dsa_switch *ds, int port,
					struct dsa_bridge bridge,
					bool *tx_fwd_offload,
					struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct qca_ppe_bridge_vsi *bvsi;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	bvsi = bridge_vsi_find(priv, bridge.dev);
	if (!bvsi) {
		bvsi = bridge_vsi_alloc(priv, bridge.dev);
		if (!bvsi)
			return -ENOSPC;
	}

	bvsi->refcount++;
	priv->port_vsi[port] = bvsi->vsi;
	priv->port_br_dev[port] = bridge.dev;

	ppe_port_vsi_set(priv, port, bvsi->vsi);
	bridge_vsi_members_update(priv, bvsi);

	return 0;
}

static void qca_ppe_port_bridge_leave(struct dsa_switch *ds, int port,
					  struct dsa_bridge bridge)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct qca_ppe_bridge_vsi *bvsi;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	bvsi = bridge_vsi_find(priv, bridge.dev);
	if (!bvsi)
		return;

	priv->port_vsi[port] = PPE_VSI_INVALID;
	priv->port_br_dev[port] = NULL;
	ppe_port_vsi_set(priv, port, PPE_VSI_INVALID);
	bridge_vsi_members_update(priv, bvsi);
	bridge_vsi_put(priv, bvsi);
}

/* The entry is keyed on the VSI the frame will carry, which is the bridge's
 * own VSI while it does not filter - a VLAN it does not enforce classifies
 * nothing - and the VLAN's VSI once it does.
 */
static u32 ppe_fdb_vsi(struct qca_ppe_priv *priv, u16 vid, struct dsa_db db)
{
	struct qca_ppe_vlan_entry *vlan;
	struct qca_ppe_bridge_vsi *bvsi;

	if (db.type != DSA_DB_BRIDGE)
		return PPE_VSI_INVALID;

	if (vid) {
		vlan = ppe_vlan_find(priv, db.bridge.dev, vid);
		if (vlan)
			return vlan->vsi;
	}

	bvsi = bridge_vsi_find(priv, db.bridge.dev);

	return bvsi ? bvsi->vsi : PPE_VSI_INVALID;
}

/* The reverse, for the dump: only a VLAN's VSI has a vid of its own, and a
 * bridge that does not filter has none to report.
 */
static u16 ppe_fdb_vid(struct qca_ppe_priv *priv, u32 vsi)
{
	int i;

	for (i = 0; i < PPE_VSI_MAX; i++)
		if (priv->vlans[i].br_dev && priv->vlans[i].vsi == vsi)
			return priv->vlans[i].vid;

	return 0;
}

static int qca_ppe_port_fdb_add(struct dsa_switch *ds, int port,
				    const unsigned char *addr, u16 vid,
				    struct dsa_db db)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 vsi;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	vsi = ppe_fdb_vsi(priv, vid, db);
	if (vsi == PPE_VSI_INVALID)
		return -EOPNOTSUPP;

	return ppe_fdb_op(priv, addr, port, vsi, PPE_FDB_OP_ADD);
}

static int qca_ppe_port_fdb_del(struct dsa_switch *ds, int port,
				    const unsigned char *addr, u16 vid,
				    struct dsa_db db)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 vsi;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	vsi = ppe_fdb_vsi(priv, vid, db);
	if (vsi == PPE_VSI_INVALID)
		return -EOPNOTSUPP;

	return ppe_fdb_op(priv, addr, port, vsi, PPE_FDB_OP_DEL);
}

static int qca_ppe_port_fdb_dump(struct dsa_switch *ds, int port,
				     dsa_fdb_dump_cb_t *cb, void *data)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	unsigned char addr[ETH_ALEN];
	bool is_static;
	int fdb_port;
	u32 i, vsi;
	int ret;

	guard(mutex)(&priv->vlan_lock);

	for (i = 0; i < PPE_FDB_TBL_NUM; i++) {
		if (ppe_fdb_read_entry(priv, i, addr, &vsi, &fdb_port,
				       &is_static))
			continue;

		if (fdb_port != port)
			continue;

		ret = cb(addr, ppe_fdb_vid(priv, vsi), is_static, data);
		if (ret)
			return ret;
	}

	return 0;
}

static int qca_ppe_port_mdb_add(struct dsa_switch *ds, int port,
				    const struct switchdev_obj_port_mdb *mdb,
				    struct dsa_db db)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 portmap, vsi;
	int ret;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	vsi = ppe_fdb_vsi(priv, mdb->vid, db);
	if (vsi == PPE_VSI_INVALID)
		return -EOPNOTSUPP;

	ret = ppe_fdb_lookup(priv, mdb->addr, vsi, &portmap);
	if (ret)
		portmap = BIT(QCA_PPE_CPU_PORT);

	portmap |= BIT(port);

	return ppe_fdb_mcast_op(priv, mdb->addr, portmap,
				vsi, PPE_FDB_OP_ADD);
}

static int qca_ppe_port_mdb_del(struct dsa_switch *ds, int port,
				    const struct switchdev_obj_port_mdb *mdb,
				    struct dsa_db db)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 portmap, vsi;
	int ret;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	vsi = ppe_fdb_vsi(priv, mdb->vid, db);
	if (vsi == PPE_VSI_INVALID)
		return -EOPNOTSUPP;

	ret = ppe_fdb_lookup(priv, mdb->addr, vsi, &portmap);
	if (ret)
		return ret;

	portmap &= ~BIT(port);

	if (!portmap || portmap == BIT(QCA_PPE_CPU_PORT))
		return ppe_fdb_mcast_op(priv, mdb->addr, 0,
					vsi, PPE_FDB_OP_DEL);

	return ppe_fdb_mcast_op(priv, mdb->addr, portmap,
				vsi, PPE_FDB_OP_ADD);
}

static int qca_ppe_fill_available_pcs(struct phylink_config *config,
				      struct phylink_pcs **available_pcs,
				      unsigned int num_available_pcs)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);

	return fwnode_phylink_pcs_parse(of_fwnode_handle(dp->dn), available_pcs,
					&num_available_pcs);
}

static void qca_ppe_phylink_get_caps(struct dsa_switch *ds, int port,
				     struct phylink_config *config)
{
	struct dsa_port *dp = dsa_to_port(ds, port);
	int ret;

	if (port != 0) {
		ret = fwnode_phylink_pcs_parse(of_fwnode_handle(dp->dn), NULL,
					       &config->num_available_pcs);
		if (ret)
			return;

		config->fill_available_pcs = qca_ppe_fill_available_pcs;
	}

	switch (port) {
	case 0:
		config->mac_capabilities =
			MAC_1000FD | MAC_SYM_PAUSE | MAC_ASYM_PAUSE;

		__set_bit(PHY_INTERFACE_MODE_INTERNAL,
			  config->supported_interfaces);
		break;
	case 1 ... 4:
		config->mac_capabilities =
			MAC_1000FD | MAC_100FD | MAC_10FD |
			MAC_SYM_PAUSE | MAC_ASYM_PAUSE;

		__set_bit(PHY_INTERFACE_MODE_QSGMII,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_PSGMII,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_SGMII,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_RGMII,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_RGMII_ID,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_RGMII_RXID,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_RGMII_TXID,
			  config->supported_interfaces);
		break;
	case 5 ... 6:
		config->mac_capabilities =
			MAC_10000FD | MAC_5000FD | MAC_2500FD |
			MAC_1000FD | MAC_100FD | MAC_10FD |
			MAC_SYM_PAUSE | MAC_ASYM_PAUSE;

		__set_bit(PHY_INTERFACE_MODE_PSGMII,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_SGMII,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_1000BASEX,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_2500BASEX,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_USXGMII,
			  config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_10GBASER,
			  config->supported_interfaces);
		break;
	}

	if (port != 0)
		phy_interface_copy(config->pcs_interfaces,
				   config->supported_interfaces);
}

static void ppe_pcs_set_mux_hppe(struct qca_ppe_priv *priv, int port,
				 unsigned int mode, phy_interface_t interface)
{
	u32 mask, val;

	switch (port) {
	case 4:
		/* The select names where port 4's GMII comes from, not the
		 * protocol carried on it, so one value covers every interface
		 * uniphy0 drives.
		 */
		mask = HPPE_PORT4_PCS_SEL;
		val = FIELD_PREP(HPPE_PORT4_PCS_SEL, HPPE_PORT4_PCS0);
		break;
	case 5:
		mask = HPPE_PORT5_PCS_SEL | HPPE_PORT5_GMAC_SEL;
		switch (interface) {
		case PHY_INTERFACE_MODE_QSGMII:
		case PHY_INTERFACE_MODE_PSGMII:
			val = FIELD_PREP(HPPE_PORT5_PCS_SEL,
					 HPPE_PORT5_PCS0) |
			      FIELD_PREP(HPPE_PORT5_GMAC_SEL,
					 HPPE_PORT5_GMAC_SEL_GMAC);
			break;
		case PHY_INTERFACE_MODE_SGMII:
		case PHY_INTERFACE_MODE_1000BASEX:
			val = FIELD_PREP(HPPE_PORT5_PCS_SEL,
					 HPPE_PORT5_PCS1) |
			      FIELD_PREP(HPPE_PORT5_GMAC_SEL,
					 HPPE_PORT5_GMAC_SEL_GMAC);
			break;
		case PHY_INTERFACE_MODE_2500BASEX:
			val = FIELD_PREP(HPPE_PORT5_PCS_SEL,
					 HPPE_PORT5_PCS1);
			/* In-Band is only supported by XGMAC */
			if (!phylink_autoneg_inband(mode))
				val |= FIELD_PREP(HPPE_PORT5_GMAC_SEL,
						  HPPE_PORT5_GMAC_SEL_GMAC);
			break;
		case PHY_INTERFACE_MODE_10GBASER:
		case PHY_INTERFACE_MODE_USXGMII:
			val = FIELD_PREP(HPPE_PORT5_PCS_SEL,
					 HPPE_PORT5_PCS1);
			break;
		default:
			return;
		}
		break;
	case 6:
		mask = HPPE_PORT6_PCS_SEL | HPPE_PORT6_GMAC_SEL;
		val = FIELD_PREP(HPPE_PORT6_PCS_SEL, HPPE_PORT6_PCS2);

		switch (interface) {
		case PHY_INTERFACE_MODE_SGMII:
		case PHY_INTERFACE_MODE_1000BASEX:
			val |= FIELD_PREP(HPPE_PORT6_GMAC_SEL,
					  HPPE_PORT6_GMAC_SEL_GMAC);
			break;
		case PHY_INTERFACE_MODE_2500BASEX:
			/* In-Band is only supported by XGMAC */
			if (!phylink_autoneg_inband(mode))
				val |= FIELD_PREP(HPPE_PORT6_GMAC_SEL,
						  HPPE_PORT6_GMAC_SEL_GMAC);

			break;
		case PHY_INTERFACE_MODE_10GBASER:
		case PHY_INTERFACE_MODE_USXGMII:
			break;
		default:
			return;
		}
		break;
	default:
		return;
	}

	regmap_update_bits(priv->regmap, PPE_PORT_MUX_CTRL, mask, val);
}

static void ppe_pcs_set_mux_cppe(struct qca_ppe_priv *priv, int port,
				 unsigned int mode, phy_interface_t interface)
{
	u32 mask, val = 0;

	switch (port) {
	case 5:
		mask = CPPE_PORT5_PCS_SEL | CPPE_PORT5_GMAC_SEL;
		switch (interface) {
		case PHY_INTERFACE_MODE_SGMII:
		case PHY_INTERFACE_MODE_1000BASEX:
			val = FIELD_PREP(CPPE_PORT5_PCS_SEL,
					 CPPE_PORT5_PCS1_CH0);
			break;
		case PHY_INTERFACE_MODE_2500BASEX:
			val = FIELD_PREP(CPPE_PORT5_PCS_SEL,
					 CPPE_PORT5_PCS1_CH0);
			/* In-Band is only supported by XGMAC */
			if (phylink_autoneg_inband(mode))
				val |= CPPE_PORT5_GMAC_SEL;
			break;
		case PHY_INTERFACE_MODE_10GBASER:
		case PHY_INTERFACE_MODE_USXGMII:
			val = FIELD_PREP(CPPE_PORT5_PCS_SEL,
					 CPPE_PORT5_PCS1_CH0) |
					 CPPE_PORT5_GMAC_SEL;
			break;
		default:
			return;
		}
		break;
	default:
		return;
	}

	regmap_update_bits(priv->regmap, PPE_PORT_MUX_CTRL, mask, val);
}

static int qca_ppe_mac_prepare(struct phylink_config *config, unsigned int mode,
			       phy_interface_t interface)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct qca_ppe_priv *priv = ds_to_priv(dp->ds);
	const struct ppe_data *d = priv->data;
	int port = dp->index;

	if (d->type == PPE_TYPE_IPQ8074)
		ppe_pcs_set_mux_hppe(priv, port, mode, interface);
	else
		ppe_pcs_set_mux_cppe(priv, port, mode, interface);

	return 0;
}

static void qca_ppe_xgmac_config(struct qca_ppe_priv *priv, int port)
{
	int xgmac = port - 5;

	regmap_set_bits(priv->regmap, PPE_XGMAC_TX_CONF(xgmac),
			PPE_XGMAC_JABBER_DISABLE);

	regmap_update_bits(priv->regmap, PPE_XGMAC_RX_CONF(xgmac),
			   PPE_XGMAC_GMII_MPLS_LAYER_CK |
			   PPE_XGMAC_WATCHDOG_DISABLE,
			   PPE_XGMAC_GMII_MPLS_LAYER_CK);

	regmap_update_bits(priv->regmap, PPE_XGMAC_PACKET_FILTER(xgmac),
			   PPE_XGMAC_PROMISCUOUS |
			   PPE_XGMAC_PASS_CONTROL_FRAME |
			   PPE_XGMAC_RATE_ADAPTATION,
			   PPE_XGMAC_PROMISCUOUS |
			   FIELD_PREP(PPE_XGMAC_PASS_CONTROL_FRAME, 0x2) |
			   PPE_XGMAC_RATE_ADAPTATION);

	regmap_update_bits(priv->regmap, PPE_XGMAC_WATCHDOG_TIMEOUT(xgmac),
			   PPE_XGMAC_WATCHDOG_ENABLE |
			   PPE_XGMAC_WATCHDOG_THRESHOLD,
			   PPE_XGMAC_WATCHDOG_ENABLE |
			   FIELD_PREP(PPE_XGMAC_WATCHDOG_THRESHOLD, 0xb));

	regmap_update_bits(priv->regmap, PPE_XGMAC_TX_FLOW_CTRL(xgmac),
			   PPE_XGMAC_PAUSE_TIME,
			   FIELD_PREP(PPE_XGMAC_PAUSE_TIME, 0xffff));
}

/* Defined with the MIB table it walks; the port reset and the bank change
 * below have to bank the counters before the MIB they read stops being the
 * one they were counted in.
 */
static void ppe_mib_fold(struct qca_ppe_priv *priv, int port);

static void qca_ppe_mac_config(struct phylink_config *config,
				    unsigned int mode,
				    const struct phylink_link_state *state)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct qca_ppe_priv *priv = ds_to_priv(dp->ds);
	int port = dp->index;

	if ((state->interface == PHY_INTERFACE_MODE_2500BASEX &&
	     phylink_autoneg_inband(mode)) ||
	    state->interface == PHY_INTERFACE_MODE_USXGMII ||
	    state->interface == PHY_INTERFACE_MODE_10GBASER) {
		qca_ppe_xgmac_config(priv, port);
	}

	if (priv->port_rst[port]) {
		/* The reset clears the MIB, so bank what it has counted and
		 * hold the rebase across the reset: a fold racing the window
		 * below would otherwise consume it while the counters are
		 * still running, and leave the drop to zero to be read as a
		 * wrap. The baseline is taken here rather than left to the
		 * periodic fold, so that nothing the port counts from
		 * link-up is discarded.
		 */
		spin_lock_bh(&priv->mib_lock);
		ppe_mib_fold(priv, port);
		priv->mib_rebase[port] = true;
		spin_unlock_bh(&priv->mib_lock);

		reset_control_assert(priv->port_rst[port]);
		msleep(150);
		reset_control_deassert(priv->port_rst[port]);

		spin_lock_bh(&priv->mib_lock);
		ppe_mib_fold(priv, port);
		priv->mib_rebase[port] = false;
		spin_unlock_bh(&priv->mib_lock);
	}
}

/* Release what is still in an XGMAC port's egress path by looping the
 * transmitter back into it, as qca-ssdk does on link-down ("release ppe port
 * egress packets when link down"). RX goes down in the same write: only the
 * transmitter has to drain, and the loop would otherwise learn the hosts
 * behind the other ports onto this one.
 */
static void ppe_port_xgmac_loopback_pulse(struct qca_ppe_priv *priv, int port)
{
	int xgmac = port - 5;

	if (port < 5 || port >= priv->data->num_ports)
		return;

	regmap_update_bits(priv->regmap, PPE_XGMAC_RX_CONF(xgmac),
			   PPE_XGMAC_LOOPBACK | PPE_XGMAC_RX_ENABLE,
			   PPE_XGMAC_LOOPBACK);
	usleep_range(1000, 2000);
	regmap_clear_bits(priv->regmap, PPE_XGMAC_RX_CONF(xgmac),
			  PPE_XGMAC_LOOPBACK);
}

static void qca_ppe_mac_link_down(struct phylink_config *config,
				  unsigned int mode,
				  phy_interface_t interface)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct qca_ppe_priv *priv = ds_to_priv(dp->ds);
	int port = dp->index;

	/* The CPU port is INTERNAL: it falls through the switch below
	 * without its MAC being touched, and qca_ppe_mac_link_up() would not
	 * re-open its gate. Leave it alone.
	 */
	if (dsa_is_cpu_port(dp->ds, port))
		return;

	/* Gate the fabric before the MAC is torn down; qca_ppe_mac_link_up()
	 * turns it back on once the MAC is up. Left on across a flap, the
	 * fabric dequeues into a MAC that is still being re-clocked and
	 * latches the port's egress scheduler in a state only a reboot clears.
	 */
	ppe_port_bridge_txmac_set(priv, port, false);

	/* Let the egress path drain before the MAC goes: packets stranded
	 * there when the link drops wedge the queue manager for good. Same
	 * 10ms as qca-ssdk.
	 */
	msleep(10);

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
		ppe_port_gmac_set(priv, port, false, false);
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		if (!phylink_autoneg_inband(mode)) {
			ppe_port_gmac_set(priv, port, false, false);
		} else {
			ppe_port_xgmac_loopback_pulse(priv, port);
			ppe_port_xgmac_set(priv, port, false, false);
		}
		break;
	case PHY_INTERFACE_MODE_10GBASER:
	case PHY_INTERFACE_MODE_USXGMII:
		ppe_port_xgmac_loopback_pulse(priv, port);
		ppe_port_xgmac_set(priv, port, false, false);
		break;
	default:
		return;
	}

	return;
}

static bool qca_ppe_port_uses_xgmac(unsigned int mode, phy_interface_t interface)
{
	switch (interface) {
	case PHY_INTERFACE_MODE_2500BASEX:
		return phylink_autoneg_inband(mode);
	case PHY_INTERFACE_MODE_USXGMII:
	case PHY_INTERFACE_MODE_10GBASER:
		return true;
	default:
		return false;
	}
}

static void qca_ppe_mac_link_up(struct phylink_config *config,
				     struct phy_device *phydev,
				     unsigned int mode,
				     phy_interface_t interface,
				     int speed, int duplex,
				     bool tx_pause, bool rx_pause)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct qca_ppe_priv *priv = ds_to_priv(dp->ds);
	int port = dp->index;
	/* Neither speed table below is exhaustive, and a speed outside the one
	 * its interface lands in leaves the gigabit rate rather than whatever
	 * the stack held.
	 */
	unsigned long rate = 125000000;

	/* Invalid mode for port < 5 */
	if ((interface == PHY_INTERFACE_MODE_2500BASEX ||
	     interface == PHY_INTERFACE_MODE_USXGMII ||
	     interface == PHY_INTERFACE_MODE_10GBASER) &&
	     port < 5)
		return;

	/* Bank what the MAC the port is leaving has counted, then baseline
	 * the one it arrives on: a rebase left to the periodic fold would
	 * discard everything the new MAC counted in the meantime. Where the
	 * bank does not change the second fold adds nothing.
	 */
	spin_lock_bh(&priv->mib_lock);
	ppe_mib_fold(priv, port);
	priv->port_xgmac[port] = qca_ppe_port_uses_xgmac(mode, interface);
	ppe_mib_fold(priv, port);
	spin_unlock_bh(&priv->mib_lock);

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
		ppe_gmac_link_up(priv, port, speed, duplex,
				 tx_pause, rx_pause);
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		if (!phylink_autoneg_inband(mode))
			ppe_gmac_link_up(priv, port, speed, duplex,
					 tx_pause, rx_pause);
		else
			ppe_xgmac_link_up(priv, port, interface, speed,
					  tx_pause, rx_pause);
		break;
	case PHY_INTERFACE_MODE_10GBASER:
	case PHY_INTERFACE_MODE_USXGMII:
		ppe_xgmac_link_up(priv, port, interface, speed,
				  tx_pause, rx_pause);
		break;
	default:
		return;
	}

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
		switch (speed) {
		case SPEED_10:
			rate = 2500000;
			break;
		case SPEED_100:
			rate = 25000000;
			break;
		case SPEED_1000:
			rate = 125000000;
			break;
		case SPEED_2500:
			rate = 312500000;
			break;
		}
		break;
	case PHY_INTERFACE_MODE_USXGMII:
	case PHY_INTERFACE_MODE_10GBASER:
		switch (speed) {
		case SPEED_10:
			rate = 1250000;
			break;
		case SPEED_100:
			rate = 12500000;
			break;
		case SPEED_1000:
			rate = 125000000;
			break;
		case SPEED_2500:
			rate = 78125000;
			break;
		case SPEED_5000:
			rate = 156250000;
			break;
		case SPEED_10000:
			rate = 312500000;
			break;
		}
		break;
	default:
		break;
	}

	if (priv->port_rx_clk[port])
		clk_set_rate(priv->port_rx_clk[port], rate);
	if (priv->port_tx_clk[port])
		clk_set_rate(priv->port_tx_clk[port], rate);

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
		ppe_port_gmac_set(priv, port, true, true);
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		if (!phylink_autoneg_inband(mode))
			ppe_port_gmac_set(priv, port, true, true);
		else
			ppe_port_xgmac_set(priv, port, true, true);
		break;
	case PHY_INTERFACE_MODE_USXGMII:
	case PHY_INTERFACE_MODE_10GBASER:
		ppe_port_xgmac_set(priv, port, true, true);
		break;
	default:
		return;
	}

	/* MAC is up, so the fabric may feed the port again. The early returns
	 * above bring no MAC up, so they leave the gate closed on purpose.
	 */
	ppe_port_bridge_txmac_set(priv, port, true);
}

/* qca_ppe implements no LPI. The stubs exist only to make
 * phylink_mac_implements_lpi() true with lpi_capabilities left at 0 -
 * phylink's "EEE always disabled" case, where phylink_bringup_phy() calls
 * phy_disable_eee(). Without that the PHYs negotiate 802.3az and egress into
 * a MAC waking from LPI wedges the port. Never called; the ops are 6.14+.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 14, 0)
static int qca_ppe_mac_enable_tx_lpi(struct phylink_config *config, u32 timer,
				     bool tx_clk_stop)
{
	return 0;
}

static void qca_ppe_mac_disable_tx_lpi(struct phylink_config *config)
{
}
#endif

static const struct phylink_mac_ops qca_ppe_phylink_mac_ops = {
	.mac_prepare	= qca_ppe_mac_prepare,
	.mac_config	= qca_ppe_mac_config,
	.mac_link_down	= qca_ppe_mac_link_down,
	.mac_link_up	= qca_ppe_mac_link_up,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 14, 0)
	.mac_enable_tx_lpi	= qca_ppe_mac_enable_tx_lpi,
	.mac_disable_tx_lpi	= qca_ppe_mac_disable_tx_lpi,
#endif
};

struct qca_ppe_mib_desc {
	unsigned int offset;
	unsigned int size;
	unsigned int xgmac;
	unsigned int xgmac_size;
	const char name[ETH_GSTRING_LEN];
};

#define MIB_ROW(_off, _sz, _xoff, _xsz, _name)				\
	{ .offset = (_off), .size = (_sz), .xgmac = (_xoff),		\
	  .xgmac_size = (_xsz), .name = _name }

/* Each counter as both MACs express it: the GMAC offset and word count, then
 * the XGMAC MMC offset and word count, or 0 where the XGMAC has no
 * equivalent. The XGMAC lumps 1519-and-up into its 1024-to-max bin and counts
 * no collisions, no alignment error and no bad receive bytes.
 */
static const struct qca_ppe_mib_desc qca_ppe_mib[] = {
	MIB_ROW(PPE_MIB_RXBROAD, 1, 0x918, 2, "rx_broadcast"),
	MIB_ROW(PPE_MIB_RXPAUSE, 1, 0x988, 2, "rx_pause"),
	MIB_ROW(PPE_MIB_RXMULTI, 1, 0x920, 2, "rx_multicast"),
	MIB_ROW(PPE_MIB_RXFCSERR, 1, 0x928, 2, "rx_fcs_error"),
	MIB_ROW(PPE_MIB_RXALIGNERR, 1, 0, 0, "rx_align_error"),
	MIB_ROW(PPE_MIB_RXRUNT, 1, 0x938, 1, "rx_runt"),
	MIB_ROW(PPE_MIB_RXFRAG, 1, 0x930, 1, "rx_fragment"),
	MIB_ROW(PPE_MIB_RXJUMBOFCSERR, 1, 0x934, 1, "rx_jumbo_fcs_error"),
	MIB_ROW(PPE_MIB_RXJUMBOALIGNERR, 1, 0, 0, "rx_jumbo_align_error"),
	MIB_ROW(PPE_MIB_RXPKT64, 1, 0x940, 2, "rx_64byte"),
	MIB_ROW(PPE_MIB_RXPKT65TO127, 1, 0x948, 2, "rx_65_127byte"),
	MIB_ROW(PPE_MIB_RXPKT128TO255, 1, 0x950, 2, "rx_128_255byte"),
	MIB_ROW(PPE_MIB_RXPKT256TO511, 1, 0x958, 2, "rx_256_511byte"),
	MIB_ROW(PPE_MIB_RXPKT512TO1023, 1, 0x960, 2, "rx_512_1023byte"),
	MIB_ROW(PPE_MIB_RXPKT1024TO1518, 1, 0x968, 2, "rx_1024_1518byte"),
	MIB_ROW(PPE_MIB_RXPKT1519TOX, 1, 0, 0, "rx_1519_maxbyte"),
	MIB_ROW(PPE_MIB_RXTOOLONG, 1, 0x93c, 1, "rx_too_long"),
	MIB_ROW(PPE_MIB_RXGOODBYTE_L, 2, 0x910, 2, "rx_good_bytes"),
	MIB_ROW(PPE_MIB_RXBADBYTE_L, 2, 0, 0, "rx_bad_bytes"),
	MIB_ROW(PPE_MIB_RXUNI, 1, 0x970, 2, "rx_unicast"),
	MIB_ROW(PPE_MIB_TXBROAD, 1, 0x874, 2, "tx_broadcast"),
	MIB_ROW(PPE_MIB_TXPAUSE, 1, 0x894, 2, "tx_pause"),
	MIB_ROW(PPE_MIB_TXMULTI, 1, 0x86c, 2, "tx_multicast"),
	MIB_ROW(PPE_MIB_TXUNDERRUN, 1, 0x87c, 2, "tx_underrun"),
	MIB_ROW(PPE_MIB_TXPKT64, 1, 0x834, 2, "tx_64byte"),
	MIB_ROW(PPE_MIB_TXPKT65TO127, 1, 0x83c, 2, "tx_65_127byte"),
	MIB_ROW(PPE_MIB_TXPKT128TO255, 1, 0x844, 2, "tx_128_255byte"),
	MIB_ROW(PPE_MIB_TXPKT256TO511, 1, 0x84c, 2, "tx_256_511byte"),
	MIB_ROW(PPE_MIB_TXPKT512TO1023, 1, 0x854, 2, "tx_512_1023byte"),
	MIB_ROW(PPE_MIB_TXPKT1024TO1518, 1, 0x85c, 2, "tx_1024_1518byte"),
	MIB_ROW(PPE_MIB_TXPKT1519TOX, 1, 0, 0, "tx_1519_maxbyte"),
	MIB_ROW(PPE_MIB_TXBYTE_L, 2, 0x814, 2, "tx_bytes"),
	MIB_ROW(PPE_MIB_TXCOLLISIONS, 1, 0, 0, "tx_collisions"),
	MIB_ROW(PPE_MIB_TXABORTCOL, 1, 0, 0, "tx_abort_collision"),
	MIB_ROW(PPE_MIB_TXMULTICOL, 1, 0, 0, "tx_multi_collision"),
	MIB_ROW(PPE_MIB_TXSINGLECOL, 1, 0, 0, "tx_single_collision"),
	MIB_ROW(PPE_MIB_TXEXCESSIVEDEFER, 1, 0, 0, "tx_excessive_defer"),
	MIB_ROW(PPE_MIB_TXDEFER, 1, 0, 0, "tx_defer"),
	MIB_ROW(PPE_MIB_TXLATECOL, 1, 0, 0, "tx_late_collision"),
	MIB_ROW(PPE_MIB_TXUNI, 1, 0x864, 2, "tx_unicast"),
};

/* What a counter has reached, and the raw register value that total was last
 * brought up to date from.
 */
struct qca_ppe_mib_stats {
	u64 total;
	u64 last;
};

static struct qca_ppe_mib_stats *ppe_port_mib(struct qca_ppe_priv *priv,
					      int port)
{
	return priv->port_mib + port * ARRAY_SIZE(qca_ppe_mib);
}

/* The GMAC keeps most counters in a single 32-bit register and neither MAC
 * ever stops counting, so the registers wrap where the totals reported to
 * userspace must not: each total takes the difference since the last fold, at
 * the width of the register it came from.
 *
 * A difference is only meaningful across a counter that kept running. Two
 * things break that, and both take a baseline instead: a port muxed to its
 * XGMAC starts reading a second MAC's independent counters, since the idle
 * block reads back zero; and resetting a port zeroes the MIB it owns, which
 * an unguarded difference would read as a wrap and add 2^32 for.
 */
static void ppe_mib_fold(struct qca_ppe_priv *priv, int port)
{
	struct qca_ppe_mib_stats *stats;
	bool xgmac, rebase;
	int i;

	/* The CPU port owns no MAC MIB, and phylink brings its fixed link up
	 * like any other, so the fold guards itself rather than each caller.
	 */
	if (port < 1)
		return;

	stats = ppe_port_mib(priv, port);
	xgmac = priv->port_xgmac[port];
	rebase = priv->mib_rebase[port] || xgmac != priv->mib_xgmac[port];

	for (i = 0; i < ARRAY_SIZE(qca_ppe_mib); i++) {
		const struct qca_ppe_mib_desc *mib = &qca_ppe_mib[i];
		unsigned int reg, size;
		u32 lo, hi = 0;
		u64 cur;

		if (xgmac) {
			reg = PPE_XGMAC_MIB(port - 5, mib->xgmac);
			size = mib->xgmac_size;
		} else {
			reg = PPE_GMAC_MIB(port - 1, mib->offset);
			size = mib->size;
		}

		if (!size)
			continue;

		regmap_read(priv->regmap, reg, &lo);
		if (size > 1)
			regmap_read(priv->regmap, reg + 4, &hi);
		cur = (u64)hi << 32 | lo;

		if (!rebase)
			stats[i].total += size > 1 ? cur - stats[i].last :
					  (u32)(cur - stats[i].last);
		stats[i].last = cur;
	}

	priv->mib_xgmac[port] = xgmac;
}

static u64 ppe_mib_total(const struct qca_ppe_mib_stats *stats,
			 unsigned int offset)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(qca_ppe_mib); i++)
		if (qca_ppe_mib[i].offset == offset)
			return stats[i].total;

	return 0;
}

/* One counter's banked total, for a reader outside this file. The fold is what
 * makes the value survive a register wrap and a port changing MAC, so a caller
 * that read the register directly would report a rate spike for either.
 */
u64 ppe_mib_read(struct qca_ppe_priv *priv, int port, unsigned int off)
{
	u64 total;

	spin_lock_bh(&priv->mib_lock);
	ppe_mib_fold(priv, port);
	total = ppe_mib_total(ppe_port_mib(priv, port), off);
	spin_unlock_bh(&priv->mib_lock);

	return total;
}

static void ppe_mib_work(struct work_struct *work)
{
	struct qca_ppe_priv *priv = container_of(to_delayed_work(work),
						 struct qca_ppe_priv,
						 mib_work);
	struct dsa_port *dp;

	dsa_switch_for_each_user_port(dp, &priv->ds) {
		spin_lock_bh(&priv->mib_lock);
		ppe_mib_fold(priv, dp->index);
		spin_unlock_bh(&priv->mib_lock);
	}

	schedule_delayed_work(&priv->mib_work, PPE_MIB_FOLD_INTERVAL);
}

static void qca_ppe_get_strings(struct dsa_switch *ds, int port,
				    u32 stringset, uint8_t *data)
{
	int i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < ARRAY_SIZE(qca_ppe_mib); i++)
		ethtool_puts(&data, qca_ppe_mib[i].name);
}

static int qca_ppe_get_sset_count(struct dsa_switch *ds, int port,
				      int sset)
{
	if (sset != ETH_SS_STATS)
		return 0;

	return ARRAY_SIZE(qca_ppe_mib);
}

static void qca_ppe_get_ethtool_stats(struct dsa_switch *ds, int port,
					  uint64_t *data)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct qca_ppe_mib_stats *stats;
	int i;

	if (port < 1 || port >= ds->num_ports) {
		memset(data, 0, sizeof(u64) * ARRAY_SIZE(qca_ppe_mib));
		return;
	}

	stats = ppe_port_mib(priv, port);

	spin_lock_bh(&priv->mib_lock);
	ppe_mib_fold(priv, port);

	for (i = 0; i < ARRAY_SIZE(qca_ppe_mib); i++)
		data[i] = stats[i].total;

	spin_unlock_bh(&priv->mib_lock);
}

/* Shorthand for the reader below, which holds the port's counters under that
 * name.
 */
#define MIB(_c)		ppe_mib_total(stats, PPE_MIB_ ## _c)

static void qca_ppe_get_stats64(struct dsa_switch *ds, int port,
				struct rtnl_link_stats64 *s)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct qca_ppe_mib_stats *stats;

	if (port < 1 || port >= ds->num_ports)
		return;

	stats = ppe_port_mib(priv, port);

	spin_lock_bh(&priv->mib_lock);
	ppe_mib_fold(priv, port);

	s->rx_packets = MIB(RXUNI) + MIB(RXMULTI) + MIB(RXBROAD);
	s->tx_packets = MIB(TXUNI) + MIB(TXMULTI) + MIB(TXBROAD);
	s->rx_bytes = MIB(RXGOODBYTE_L);
	s->tx_bytes = MIB(TXBYTE_L);
	s->multicast = MIB(RXMULTI);

	s->rx_crc_errors = MIB(RXFCSERR) + MIB(RXJUMBOFCSERR);
	s->rx_frame_errors = MIB(RXALIGNERR) + MIB(RXJUMBOALIGNERR);
	s->rx_length_errors = MIB(RXRUNT) + MIB(RXFRAG) + MIB(RXTOOLONG);
	s->rx_errors = s->rx_crc_errors + s->rx_frame_errors +
		       s->rx_length_errors;

	s->tx_fifo_errors = MIB(TXUNDERRUN);
	s->tx_aborted_errors = MIB(TXABORTCOL);
	s->tx_window_errors = MIB(TXLATECOL);
	s->tx_errors = s->tx_fifo_errors + s->tx_aborted_errors +
		       s->tx_window_errors;

	s->collisions = MIB(TXCOLLISIONS);

	spin_unlock_bh(&priv->mib_lock);
}

#undef MIB

static void qca_ppe_teardown(struct dsa_switch *ds)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	cancel_delayed_work_sync(&priv->mib_work);
	devlink_sb_unregister(ds->devlink, PPE_DEVLINK_SB);
	dsa_devlink_resources_unregister(ds);
}

/* Everything the PPE holds about one port that a value can be read out of:
 * where the L2 stage has it, what its VLAN stages do to a frame, the routing
 * entry that carries its VSI, the policer that meters what arrives and the
 * shaper that meters what leaves. The MAC window is deliberately absent - the
 * counters in it are what ethtool -S already reports, and the rest of it
 * depends on which of the two MACs the port is driving.
 *
 * A dump is address and value in pairs. It costs a word per register and buys
 * a dump that can be read without this table in hand, and an insertion here
 * that breaks nothing downstream.
 */
enum ppe_port_reg {
	PPE_REG_CST_STATE,
	PPE_REG_BRIDGE_CTRL,
	PPE_REG_MIRROR,
	PPE_REG_QOS_CTRL,
	PPE_REG_MC_MTU_CTRL,
	PPE_REG_MRU_MTU_CTRL_W0,
	PPE_REG_MRU_MTU_CTRL_W1,
	PPE_REG_DEF_VID,
	PPE_REG_VLAN_CFG,
	PPE_REG_EG_VLAN,
	PPE_REG_VP_PORT_W0,
	PPE_REG_VP_PORT_W1,
	PPE_REG_VP_PORT_W2,
	PPE_REG_METER_W0,
	PPE_REG_METER_W1,
	PPE_REG_METER_W2,
	PPE_REG_METER_W3,
	PPE_REG_SHP_CFG_W0,
	PPE_REG_SHP_CFG_W1,
	PPE_REG_COUNT,
};

/* Bumped if a register leaves this list or the pairing changes. */
#define PPE_REGS_VERSION	1

static int qca_ppe_get_regs_len(struct dsa_switch *ds, int port)
{
	return PPE_REG_COUNT * 2 * sizeof(u32);
}

static void qca_ppe_get_regs(struct dsa_switch *ds, int port,
			     struct ethtool_regs *regs, void *_p)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 off[PPE_REG_COUNT];
	u32 mru, vp;
	u32 *p = _p;
	int i;

	mru = PPE_MRU_MTU_CTRL(port, priv->data->mru_mtu_ctrl_stride);
	vp = PPE_L3_VP_PORT_TBL(port);

	off[PPE_REG_CST_STATE]		= PPE_CST_STATE(port);
	off[PPE_REG_BRIDGE_CTRL]	= PPE_PORT_BRIDGE_CTRL(port);
	off[PPE_REG_MIRROR]		= PPE_PORT_MIRROR(port);
	off[PPE_REG_QOS_CTRL]		= PPE_PORT_QOS_CTRL(port);
	off[PPE_REG_MC_MTU_CTRL]	= PPE_MC_MTU_CTRL(port);
	off[PPE_REG_MRU_MTU_CTRL_W0]	= mru;
	off[PPE_REG_MRU_MTU_CTRL_W1]	= mru + 0x4;
	off[PPE_REG_DEF_VID]		= PPE_PORT_DEF_VID(port);
	off[PPE_REG_VLAN_CFG]		= PPE_PORT_VLAN_CFG(port);
	off[PPE_REG_EG_VLAN]		= PPE_PORT_EG_VLAN(port);
	off[PPE_REG_VP_PORT_W0]		= vp;
	off[PPE_REG_VP_PORT_W1]		= vp + 0x4;
	off[PPE_REG_VP_PORT_W2]		= vp + 0x8;
	off[PPE_REG_METER_W0]		= PPE_PORT_METER_W0(port);
	off[PPE_REG_METER_W1]		= PPE_PORT_METER_W1(port);
	off[PPE_REG_METER_W2]		= PPE_PORT_METER_W2(port);
	off[PPE_REG_METER_W3]		= PPE_PORT_METER_W3(port);
	off[PPE_REG_SHP_CFG_W0]		= PPE_TM_PSCH_SHP_CFG_W0(port);
	off[PPE_REG_SHP_CFG_W1]		= PPE_TM_PSCH_SHP_CFG_W1(port);

	regs->version = PPE_REGS_VERSION;

	for (i = 0; i < PPE_REG_COUNT; i++) {
		p[i * 2] = off[i];
		regmap_read(priv->regmap, off[i], &p[i * 2 + 1]);
	}
}

static void qca_ppe_port_stp_state_set(struct dsa_switch *ds, int port,
					   u8 state)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 stp_state;

	switch (state) {
	case BR_STATE_DISABLED:
		stp_state = PPE_STP_DISABLED;
		break;
	case BR_STATE_BLOCKING:
	case BR_STATE_LISTENING:
		stp_state = PPE_STP_BLOCKING;
		break;
	case BR_STATE_LEARNING:
		stp_state = PPE_STP_LEARNING;
		break;
	case BR_STATE_FORWARDING:
	default:
		stp_state = PPE_STP_FORWARDING;
		break;
	}

	regmap_update_bits(priv->regmap, PPE_CST_STATE(port),
			   PPE_STP_STATE_MASK, stp_state);
}

#define QCA_PPE_BRIDGE_FLAGS	(BR_LEARNING | BR_FLOOD | BR_MCAST_FLOOD | \
				 BR_BCAST_FLOOD | BR_ISOLATED)

static int qca_ppe_port_pre_bridge_flags(struct dsa_switch *ds, int port,
					 struct switchdev_brport_flags flags,
					 struct netlink_ext_ack *extack)
{
	if (flags.mask & ~QCA_PPE_BRIDGE_FLAGS)
		return -EINVAL;

	return 0;
}

static int qca_ppe_port_bridge_flags(struct dsa_switch *ds, int port,
				     struct switchdev_brport_flags flags,
				     struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	guard(mutex)(&priv->vlan_lock);

	priv->port_brflags[port] &= ~flags.mask;
	priv->port_brflags[port] |= flags.val & flags.mask;

	if (flags.mask & BR_LEARNING)
		regmap_update_bits(priv->regmap, PPE_PORT_BRIDGE_CTRL(port),
				   PPE_BRIDGE_NEW_LRN_EN,
				   flags.val & BR_LEARNING ?
					PPE_BRIDGE_NEW_LRN_EN : 0);

	if (flags.mask & (BR_FLOOD | BR_MCAST_FLOOD | BR_BCAST_FLOOD))
		ppe_vsi_flood_refresh(priv);

	if (flags.mask & BR_ISOLATED) {
		if (flags.val & BR_ISOLATED)
			priv->port_isolated |= BIT(port);
		else
			priv->port_isolated &= ~BIT(port);

		ppe_port_isolation_update(priv);
	}

	return 0;
}

/* The parser hashes on whole fields, so a policy has an expression only where
 * it names a set of them: the encapsulated ones have no inner header to reach
 * here and NETDEV_LAG_HASH_VLAN_SRCMAC no field at all.
 */
static u32 ppe_trunk_hash_field(enum netdev_lag_hash hash)
{
	switch (hash) {
	case NETDEV_LAG_HASH_L2:
		return PPE_TRUNK_HASH_MAC_DA | PPE_TRUNK_HASH_MAC_SA;
	case NETDEV_LAG_HASH_L23:
		return PPE_TRUNK_HASH_MAC_DA | PPE_TRUNK_HASH_MAC_SA |
		       PPE_TRUNK_HASH_SIP | PPE_TRUNK_HASH_DIP;
	case NETDEV_LAG_HASH_L34:
		return PPE_TRUNK_HASH_SIP | PPE_TRUNK_HASH_DIP |
		       PPE_TRUNK_HASH_L4_SPORT | PPE_TRUNK_HASH_L4_DPORT;
	default:
		return 0;
	}
}

/* The member table is eight hash buckets rather than a member list: the
 * hardware picks a bucket and reads a port id out of it, so the transmitting
 * members are tiled across all eight. Falling back to every member when none
 * transmits covers the join, which runs before the bond reports a lower state,
 * and the aggregate whose links are all down - a dead port drops the frame,
 * where an empty table would have sent it to bucket zero's port.
 */
static void ppe_trunk_program(struct qca_ppe_priv *priv, unsigned int id)
{
	u8 slot[PPE_TRUNK_MEMBER_SLOTS];
	unsigned int g = id - 1;
	int i, port, n = 0;
	u32 val = 0;
	u8 members;

	members = priv->trunk_tx[g];
	if (!members)
		members = priv->trunk_members[g];

	for (port = 0; port < priv->ds.num_ports; port++)
		if (members & BIT(port))
			slot[n++] = port;

	for (i = 0; n && i < PPE_TRUNK_MEMBER_SLOTS; i++)
		val |= (u32)slot[i % n] << (i * PPE_TRUNK_MEMBER_SLOT_SHIFT);

	regmap_write(priv->regmap, PPE_TRUNK_MEMBER(g), val);
	regmap_write(priv->regmap, PPE_TRUNK_FILTER(g),
		     FIELD_PREP(PPE_TRUNK_FILTER_MEMBERS,
				priv->trunk_members[g]));
}

static int qca_ppe_port_lag_join(struct dsa_switch *ds, int port,
				 struct dsa_lag lag,
				 struct netdev_lag_upper_info *info,
				 struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 hash;

	/* DSA allocates one id per aggregate out of num_lag_ids, so its
	 * allocator is the trunk group allocator and a zero id is a third bond
	 * asking for a group the hardware does not have.
	 */
	if (!lag.id) {
		NL_SET_ERR_MSG_MOD(extack, "the hardware has two trunk groups");
		return -EOPNOTSUPP;
	}

	/* A frame arriving on a member is switched in hardware and never
	 * reaches the bond, which is what every mode but hashing relies on to
	 * drop what arrives on a link it is not using.
	 */
	if (info->tx_type != NETDEV_LAG_TX_TYPE_HASH) {
		NL_SET_ERR_MSG_MOD(extack, "only a hashing bond is offloaded");
		return -EOPNOTSUPP;
	}

	hash = ppe_trunk_hash_field(info->hash_type);
	if (!hash) {
		NL_SET_ERR_MSG_MOD(extack,
				   "the trunk hash has no such field set");
		return -EOPNOTSUPP;
	}

	guard(mutex)(&priv->vlan_lock);

	/* One hash-field register serves both groups, so the second aggregate
	 * may only ask for what the first is already hashing on.
	 */
	if (priv->trunk_hash && priv->trunk_hash != hash) {
		NL_SET_ERR_MSG_MOD(extack,
				   "the other trunk hashes on other fields");
		return -EOPNOTSUPP;
	}

	priv->trunk_hash = hash;
	regmap_write(priv->regmap, PPE_TRUNK_HASH_FIELD, hash);

	priv->trunk_members[lag.id - 1] |= BIT(port);
	ppe_trunk_program(priv, lag.id);

	/* Last, so that the port is never a member of a group whose tables it
	 * is not in yet; leaving unmarks it first for the same reason.
	 */
	regmap_update_bits(priv->regmap, PPE_PORT_TRUNK_ID(port),
			   PPE_PORT_TRUNK_EN | PPE_PORT_TRUNK_GROUP,
			   PPE_PORT_TRUNK_EN |
			   FIELD_PREP(PPE_PORT_TRUNK_GROUP, lag.id - 1));

	ppe_vsi_flood_refresh(priv);

	return 0;
}

static int qca_ppe_port_lag_leave(struct dsa_switch *ds, int port,
				  struct dsa_lag lag)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	int g;

	guard(mutex)(&priv->vlan_lock);

	regmap_update_bits(priv->regmap, PPE_PORT_TRUNK_ID(port),
			   PPE_PORT_TRUNK_EN, 0);

	priv->trunk_members[lag.id - 1] &= ~BIT(port);
	priv->trunk_tx[lag.id - 1] &= ~BIT(port);
	ppe_trunk_program(priv, lag.id);
	ppe_vsi_flood_refresh(priv);

	/* The hash-field register is released with the last group so that the
	 * next aggregate is free to ask for another policy.
	 */
	for (g = 0; g < PPE_TRUNK_GROUPS; g++)
		if (priv->trunk_members[g])
			return 0;

	priv->trunk_hash = 0;
	regmap_write(priv->regmap, PPE_TRUNK_HASH_FIELD, 0);

	return 0;
}

/* Rebuilding the buckets from the transmitting members is the failover, and it
 * is the only thing that sees an aggregator deselecting a member whose link is
 * still up. The flood member is chosen from the same set, so it moves too.
 */
static int qca_ppe_port_lag_change(struct dsa_switch *ds, int port)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct dsa_port *dp = dsa_to_port(ds, port);
	unsigned int id = dp->lag->id;

	guard(mutex)(&priv->vlan_lock);

	if (dp->lag_tx_enabled)
		priv->trunk_tx[id - 1] |= BIT(port);
	else
		priv->trunk_tx[id - 1] &= ~BIT(port);

	ppe_trunk_program(priv, id);
	ppe_vsi_flood_refresh(priv);

	return 0;
}

/* A trunk is named in the destination field of an ordinary entry, so the only
 * difference from a per-port address is the value written there.
 */
static int qca_ppe_lag_fdb_add(struct dsa_switch *ds, struct dsa_lag lag,
			       const unsigned char *addr, u16 vid,
			       struct dsa_db db)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 vsi;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	vsi = ppe_fdb_vsi(priv, vid, db);
	if (vsi == PPE_VSI_INVALID)
		return -EOPNOTSUPP;

	return ppe_fdb_op(priv, addr, PPE_FDB_DST_TRUNK(lag.id - 1), vsi,
			  PPE_FDB_OP_ADD);
}

static int qca_ppe_lag_fdb_del(struct dsa_switch *ds, struct dsa_lag lag,
			       const unsigned char *addr, u16 vid,
			       struct dsa_db db)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 vsi;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	vsi = ppe_fdb_vsi(priv, vid, db);
	if (vsi == PPE_VSI_INVALID)
		return -EOPNOTSUPP;

	return ppe_fdb_op(priv, addr, PPE_FDB_DST_TRUNK(lag.id - 1), vsi,
			  PPE_FDB_OP_DEL);
}

/* No hardware op deletes by port - the vendor walks the table too - so every
 * learned entry naming the port is read back and deleted one at a time. The
 * static ones are the bridge's own and outlive the transition that asked.
 */
static void qca_ppe_port_fast_age(struct dsa_switch *ds, int port)
{
	struct dsa_port *dp = dsa_to_port(ds, port);
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	unsigned char addr[ETH_ALEN];
	int dst, fdb_port;
	bool is_static;
	u32 i, vsi;

	guard(mutex)(&priv->vlan_lock);

	/* Addresses behind a trunk are learned against the trunk, not the
	 * member they arrived on, so ageing one member ages the aggregate -
	 * which is what the bridge asked for, the aggregate being its port.
	 * DSA leaves the bridge before it drops dp->lag, so an unenslaving
	 * port still flushes them.
	 */
	dst = dp->lag ? PPE_FDB_DST_TRUNK(dp->lag->id - 1) : port;

	for (i = 0; i < PPE_FDB_TBL_NUM; i++) {
		if (ppe_fdb_read_entry(priv, i, addr, &vsi, &fdb_port,
				       &is_static))
			continue;

		if (fdb_port != dst || is_static)
			continue;

		ppe_fdb_op(priv, addr, fdb_port, vsi, PPE_FDB_OP_DEL);
	}
}

/* One analyzer serves the whole switch, for both directions, so every mirror on
 * the box has to name the same destination; a second one naming another port is
 * refused rather than silently redirecting the first. The analyzer is a switch
 * port, so mirroring costs a LAN port for as long as it is on. A classifier
 * rule that mirrors takes the same analyzer, which is why this is shared.
 */
int ppe_mirror_analyzer_get(struct qca_ppe_priv *priv, int to_port)
{
	if (priv->mirror_ref && priv->mirror_port != to_port)
		return -EBUSY;

	regmap_write(priv->regmap, PPE_MIRROR_ANALYZER,
		     FIELD_PREP(PPE_MIRROR_IN_ANALYZER, to_port) |
		     FIELD_PREP(PPE_MIRROR_EG_ANALYZER, to_port));

	priv->mirror_port = to_port;
	priv->mirror_ref++;

	return 0;
}

void ppe_mirror_analyzer_put(struct qca_ppe_priv *priv)
{
	if (--priv->mirror_ref)
		return;

	regmap_write(priv->regmap, PPE_MIRROR_ANALYZER, 0);
	priv->mirror_port = -1;
}

int qca_ppe_port_mirror_add(struct dsa_switch *ds, int port,
			    struct dsa_mall_mirror_tc_entry *mirror,
			    bool ingress, struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	int ret;

	ret = ppe_mirror_analyzer_get(priv, mirror->to_local_port);
	if (ret) {
		NL_SET_ERR_MSG_MOD(extack,
				   "another port is already mirrored elsewhere");
		return ret;
	}

	regmap_update_bits(priv->regmap, PPE_PORT_MIRROR(port),
			   ingress ? PPE_PORT_MIRROR_IN_EN :
				     PPE_PORT_MIRROR_EG_EN,
			   ingress ? PPE_PORT_MIRROR_IN_EN :
				     PPE_PORT_MIRROR_EG_EN);

	priv->mirror_dir_ref[port][ingress]++;

	return 0;
}

void qca_ppe_port_mirror_del(struct dsa_switch *ds, int port,
			     struct dsa_mall_mirror_tc_entry *mirror)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	/* Two filters can name the same port and direction, and they share one
	 * enable bit: it goes off with the last of them, not the first.
	 */
	if (!--priv->mirror_dir_ref[port][mirror->ingress])
		regmap_update_bits(priv->regmap, PPE_PORT_MIRROR(port),
				   mirror->ingress ? PPE_PORT_MIRROR_IN_EN :
						     PPE_PORT_MIRROR_EG_EN, 0);

	ppe_mirror_analyzer_put(priv);
}

static const struct dsa_switch_ops qca_ppe_ops = {
	.port_setup_tc		= qca_ppe_setup_tc,
	.cls_flower_add		= qca_ppe_cls_flower_add,
	.cls_flower_del		= qca_ppe_cls_flower_del,
	.get_rxnfc		= qca_ppe_get_rxnfc,
	.set_rxnfc		= qca_ppe_set_rxnfc,
	.port_mirror_add	= qca_ppe_port_mirror_add,
	.port_mirror_del	= qca_ppe_port_mirror_del,
	.port_policer_add	= qca_ppe_port_policer_add,
	.port_policer_del	= qca_ppe_port_policer_del,
	.port_get_dscp_prio	= qca_ppe_port_get_dscp_prio,
	.port_add_dscp_prio	= qca_ppe_port_add_dscp_prio,
	.port_del_dscp_prio	= qca_ppe_port_del_dscp_prio,
	.port_get_apptrust	= qca_ppe_port_get_apptrust,
	.port_set_apptrust	= qca_ppe_port_set_apptrust,
	.get_tag_protocol	= qca_ppe_get_tag_protocol,
	.setup			= qca_ppe_setup,
	.teardown		= qca_ppe_teardown,
	.set_ageing_time	= qca_ppe_set_ageing_time,
	.port_change_mtu	= qca_ppe_port_change_mtu,
	.port_max_mtu		= qca_ppe_port_max_mtu,
	.port_enable		= qca_ppe_port_enable,
	.port_disable		= qca_ppe_port_disable,
	.port_stp_state_set	= qca_ppe_port_stp_state_set,
	.port_bridge_join	= qca_ppe_port_bridge_join,
	.port_bridge_leave	= qca_ppe_port_bridge_leave,
	.port_pre_bridge_flags	= qca_ppe_port_pre_bridge_flags,
	.port_bridge_flags	= qca_ppe_port_bridge_flags,
	.port_fast_age		= qca_ppe_port_fast_age,
	.port_fdb_add		= qca_ppe_port_fdb_add,
	.port_fdb_del		= qca_ppe_port_fdb_del,
	.port_fdb_dump		= qca_ppe_port_fdb_dump,
	.port_lag_join		= qca_ppe_port_lag_join,
	.port_lag_leave		= qca_ppe_port_lag_leave,
	.port_lag_change	= qca_ppe_port_lag_change,
	.lag_fdb_add		= qca_ppe_lag_fdb_add,
	.lag_fdb_del		= qca_ppe_lag_fdb_del,
	.port_mdb_add		= qca_ppe_port_mdb_add,
	.port_mdb_del		= qca_ppe_port_mdb_del,
	.phylink_get_caps	= qca_ppe_phylink_get_caps,
	.port_vlan_filtering	= qca_ppe_port_vlan_filtering,
	.port_vlan_add		= qca_ppe_port_vlan_add,
	.port_vlan_del		= qca_ppe_port_vlan_del,
	.get_strings		= qca_ppe_get_strings,
	.get_sset_count		= qca_ppe_get_sset_count,
	.get_ethtool_stats	= qca_ppe_get_ethtool_stats,
	.get_stats64		= qca_ppe_get_stats64,
	.get_regs_len		= qca_ppe_get_regs_len,
	.get_regs		= qca_ppe_get_regs,
	.devlink_info_get	= qca_ppe_devlink_info_get,
	.devlink_sb_pool_get	= qca_ppe_devlink_sb_pool_get,
	.devlink_sb_pool_set	= qca_ppe_devlink_sb_pool_set,
};

static void ppe_mac_hw_init(struct qca_ppe_priv *priv)
{
	const struct ppe_data *d = priv->data;
	int lpbk_gmac = d->loopback_port - 1;
	int gmac;

	for (gmac = 0; gmac < d->num_gmacs; gmac++) {
		regmap_update_bits(priv->regmap, PPE_GMAC_CTRL2(gmac),
				   PPE_GMAC_CTRL2_MAXFR | PPE_GMAC_CTRL2_CRS_SEL |
				   PPE_GMAC_CTRL2_TX_THD,
				   FIELD_PREP(PPE_GMAC_CTRL2_MAXFR, PPE_MAX_FRAME_SIZE) |
				   FIELD_PREP(PPE_GMAC_CTRL2_TX_THD, 1));

		regmap_update_bits(priv->regmap, PPE_GMAC_DBG_CTRL(gmac),
				   PPE_GMAC_DBG_CTRL_HIHG_IPG,
				   FIELD_PREP(PPE_GMAC_DBG_CTRL_HIHG_IPG, 0xc));

		regmap_write(priv->regmap, PPE_GMAC_JUMBO_SIZE(gmac),
			     PPE_MAX_FRAME_SIZE);
	}

	regmap_update_bits(priv->regmap, PPE_LPBK_PPS_CTRL(lpbk_gmac),
			   PPE_LPBK_PPS_THRESHOLD,
			   FIELD_PREP(PPE_LPBK_PPS_THRESHOLD, 21));
	regmap_write(priv->regmap, PPE_LPBK_ENABLE(lpbk_gmac),
		PPE_LPBK_EN | PPE_LPBK_CRC_STRIP_EN);
	msleep(100);
	ppe_port_bridge_txmac_set(priv, d->loopback_port, true);
}

static void ppe_ctrlpkt_init(struct qca_ppe_priv *priv)
{
	u64 rfdb = (u64)BIT(PPE_RFDB_STP) << PPE_APP_CTRL_RFDB_BITMAP_SHIFT;
	u32 ports;

	/* The loopback port originates nothing a control-packet rule should
	 * see, and it is the port the SoC data names.
	 */
	ports = GENMASK(priv->data->num_ports - 1, 0) &
		~BIT(priv->data->loopback_port);

	/* RFDB_TBL[31]: STP multicast MAC 01:80:c2:00:00:00 */
	regmap_write(priv->regmap, PPE_RFDB_TBL(PPE_RFDB_STP), 0xc2000000);
	regmap_write(priv->regmap, PPE_RFDB_TBL(PPE_RFDB_STP) + 4, 0x00010180);

	regmap_write(priv->regmap, PPE_APP_CTRL(0),
		     PPE_APP_CTRL_VALID | PPE_APP_CTRL_RFDB_INCLUDE |
		     lower_32_bits(rfdb));
	regmap_write(priv->regmap, PPE_APP_CTRL(0) + 4, upper_32_bits(rfdb));
	regmap_write(priv->regmap, PPE_APP_CTRL(0) + 8,
		     PPE_APP_CTRL_W2_PORTBITMAP_INCLUDE |
		     FIELD_PREP(PPE_APP_CTRL_W2_PORTBITMAP, ports) |
		     PPE_APP_CTRL_W2_IN_STG_BYP |
		     FIELD_PREP(PPE_APP_CTRL_W2_CMD,
				PPE_APP_CTRL_CMD_RDT_TO_CPU));
}

static int ppe_ipq6018_mux_setup(struct qca_ppe_priv *priv)
{
	struct device_node *ports_np, *port_np;
	struct of_phandle_args pcs_args;
	int port3_ch = -1;
	u32 port;
	int ret;

	ports_np = of_get_child_by_name(priv->ds.dev->of_node, "ports");
	if (!ports_np)
		return -ENODEV;

	for_each_available_child_of_node(ports_np, port_np) {
		ret = of_property_read_u32(port_np, "reg", &port);
		if (ret)
			continue;

		if (port != 3)
			continue;

		ret = of_parse_phandle_with_args(port_np, "pcs-handle",
						 "#pcs-cells", 0, &pcs_args);
		if (ret)
			continue;

		port3_ch = pcs_args.args[0];
	}

	of_node_put(ports_np);

	/* FIXME: better investigate this */
	if (port3_ch == 4)
		regmap_update_bits(priv->regmap, PPE_PORT_MUX_CTRL,
				   CPPE_PORT3_PCS_SEL | CPPE_PCS0_CH4_SEL,
				   FIELD_PREP(CPPE_PORT3_PCS_SEL,
					      CPPE_PORT3_PCS0_CH4) |
				   CPPE_PCS0_CH4_SEL);

	return 0;
}

static const struct regmap_config ppe_regmap_cfg = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
};

static int qca_ppe_probe(struct platform_device *pdev)
{
	struct regmap_config regmap_cfg;
	const struct ppe_data *data;
	struct resource *res;
	struct device_node *ports;
	struct qca_ppe_priv *priv;
	struct reset_control *rst;
	struct dsa_switch *ds;
	void __iomem *base;
	int ret, i;

	data = of_device_get_match_data(&pdev->dev);
	if (!data)
		return -ENODEV;

	ports = of_get_child_by_name(pdev->dev.of_node, "ports");
	if (!ports)
		return -ENODEV;
	of_node_put(ports);

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->data = data;

	priv->num_clks = devm_clk_bulk_get_all(&pdev->dev, &priv->clks);
	if (priv->num_clks < 0)
		return priv->num_clks;

	ret = clk_bulk_prepare_enable(priv->num_clks, priv->clks);
	if (ret)
		return ret;

	base = devm_platform_get_and_ioremap_resource(pdev, 0, &res);
	if (IS_ERR(base))
		return dev_err_probe(&pdev->dev, PTR_ERR(base), "failed to ioremap resource");

	/* Bound the regmap by what is actually mapped: a register the window
	 * does not cover is an -EIO rather than a fault on unmapped memory.
	 */
	regmap_cfg = ppe_regmap_cfg;
	regmap_cfg.max_register = resource_size(res) - sizeof(u32);

	priv->regmap = devm_regmap_init_mmio(&pdev->dev, base, &regmap_cfg);
	if (IS_ERR(priv->regmap))
		return dev_err_probe(&pdev->dev, PTR_ERR(priv->regmap), "failed to init regmap");

	rst = devm_reset_control_get(&pdev->dev, "ppe_rst");
	if (IS_ERR(rst)) {
		ret = PTR_ERR(rst);
		goto err_clk;
	}
	reset_control_assert(rst);
	msleep(100);
	reset_control_deassert(rst);
	msleep(100);

	spin_lock_init(&priv->fdb_lock);
	spin_lock_init(&priv->mib_lock);
	INIT_DELAYED_WORK(&priv->mib_work, ppe_mib_work);

	priv->port_mib = devm_kcalloc(&pdev->dev,
				      data->num_ports * ARRAY_SIZE(qca_ppe_mib),
				      sizeof(*priv->port_mib), GFP_KERNEL);
	if (!priv->port_mib) {
		ret = -ENOMEM;
		goto err_clk;
	}

	ds = &priv->ds;
	ds->dev = &pdev->dev;
	ds->num_ports = data->num_ports;
	ds->ops = &qca_ppe_ops;
	ds->priv = priv;
	/* The two DSCP tables are chosen between per port, not filled per port,
	 * so every port shares one and DSA replicates an entry to all of them.
	 */
	ds->dscp_prio_mapping_is_global = true;
	/* mqprio carves these into traffic classes, one per hardware queue. */
	ds->num_tx_queues = PPE_QOS_MAX_PRI + 1;
	/* The id DSA hands an aggregate is the trunk group it is given, so this
	 * is what stops a third bond from sharing one.
	 */
	ds->num_lag_ids = PPE_TRUNK_GROUPS;
	priv->mirror_port = -1;
	ds->phylink_mac_ops = &qca_ppe_phylink_mac_ops;

	for (i = 1; i < data->num_ports; i++) {
		char name[32];

		snprintf(name, sizeof(name), "port%d_rx", i);
		priv->port_rx_clk[i] = devm_clk_get_optional(&pdev->dev, name);
		if (IS_ERR(priv->port_rx_clk[i])) {
			ret = PTR_ERR(priv->port_rx_clk[i]);
			goto err_clk;
		}

		snprintf(name, sizeof(name), "port%d_tx", i);
		priv->port_tx_clk[i] = devm_clk_get_optional(&pdev->dev, name);
		if (IS_ERR(priv->port_tx_clk[i])) {
			ret = PTR_ERR(priv->port_tx_clk[i]);
			goto err_clk;
		}

		snprintf(name, sizeof(name), "nss_port%d_rst", i);
		priv->port_rst[i] = devm_reset_control_get_optional_exclusive(
						&pdev->dev, name);
		if (IS_ERR(priv->port_rst[i])) {
			ret = PTR_ERR(priv->port_rst[i]);
			goto err_clk;
		}
	}

	ppe_scheduler_init(priv);

	ppe_mac_hw_init(priv);
	ppe_ctrlpkt_init(priv);
	ppe_flow_init(priv);
	ppe_acl_init(priv);

	ret = ppe_flow_offload_init(priv);
	if (ret)
		goto err_acl;

	if (data->type == PPE_TYPE_IPQ6018) {
		ret = ppe_ipq6018_mux_setup(priv);
		if (ret)
			goto err_flow;
	}

	ret = dsa_register_switch(ds);
	if (ret)
		goto err_flow;

	ppe_scheduler_ready(priv);
	ppe_flow_debugfs_init(priv);

	platform_set_drvdata(pdev, priv);

	return 0;

err_flow:
	ppe_flow_offload_exit(priv);
err_acl:
	ppe_acl_exit(priv);
	ppe_scheduler_exit(priv);
err_clk:
	clk_bulk_disable_unprepare(priv->num_clks, priv->clks);
	return ret;
}

static void qca_ppe_remove(struct platform_device *pdev)
{
	struct qca_ppe_priv *priv = platform_get_drvdata(pdev);

	ppe_flow_debugfs_exit(priv);
	ppe_scheduler_unready();
	dsa_unregister_switch(&priv->ds);
	/* After the switch is gone: unregistration flushes the flowtables, and
	 * their FLOW_CLS_DESTROY commands have to find the table still alive.
	 */
	ppe_flow_offload_exit(priv);
	ppe_acl_exit(priv);
	ppe_scheduler_exit(priv);
	clk_bulk_disable_unprepare(priv->num_clks, priv->clks);
}

static const struct ppe_data ipq6018_ppe_data = {
	.type			= PPE_TYPE_IPQ6018,
	.num_ports		= 7,
	.num_gmacs		= 5,
	.mru_mtu_ctrl_stride	= 0x10,
	.loopback_port		= 6,
	.bm_phy_end		= 12,
	.bm_internal_start	= 13,
	.bm_group_buf		= 1024,
	.bm_ceiling		= 216,
	.qm_total_buf		= 1506,
	.qm_ceiling		= 216,
	.qm_green_max		= 144,
	.num_flow_entries	= 2048,
	.num_host_entries	= 768,
	.num_nexthop_entries	= 768,
	.psch_tdm		= &cppe_psch_tdm_data,
	.bm_tdm			= &cppe_bm_tdm_data,
};

static const struct ppe_data ipq8074_ppe_data = {
	.type			= PPE_TYPE_IPQ8074,
	.num_ports		= 8,
	.num_gmacs		= 6,
	.mru_mtu_ctrl_stride	= 0x8,
	.loopback_port		= 7,
	.bm_phy_end		= 13,
	.bm_internal_start	= 14,
	.bm_group_buf		= 1400,
	.bm_ceiling		= 250,
	.qm_total_buf		= 2000,
	.qm_ceiling		= 400,
	.qm_green_max		= 250,
	.num_flow_entries	= 4096,
	.num_host_entries	= 6144,
	.num_nexthop_entries	= 2560,
	.psch_tdm		= &hppe_psch_tdm_data,
	.bm_tdm			= &hppe_bm_tdm_data,
};

static const struct of_device_id qca_ppe_of_match[] = {
	{ .compatible = "qualcomm,ipq6018-ppe", .data = &ipq6018_ppe_data },
	{ .compatible = "qualcomm,ipq8074-ppe", .data = &ipq8074_ppe_data },
	{},
};
MODULE_DEVICE_TABLE(of, qca_ppe_of_match);

static struct platform_driver qca_ppe_driver = {
	.driver = {
		.name = "qca-ppe",
		.of_match_table = qca_ppe_of_match,
	},
	.probe = qca_ppe_probe,
	.remove = qca_ppe_remove,
};
module_platform_driver(qca_ppe_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Driver for Qualcomm PPE switches");
