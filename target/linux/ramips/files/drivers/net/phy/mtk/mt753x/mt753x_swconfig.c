// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 */

#include <linux/if.h>
#include <linux/list.h>
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/bitops.h>
#include <net/genetlink.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/lockdep.h>
#include <linux/workqueue.h>
#include <linux/of_device.h>

#include "mt753x.h"
#include "mt753x_swconfig.h"
#include "mt753x_regs.h"

#define MT753X_PORT_MIB_TXB_ID	18	/* TxByte */
#define MT753X_PORT_MIB_RXB_ID	37	/* RxByte */

#define MIB_DESC(_s, _o, _n)   \
	{                       \
		.size = (_s),   \
		.offset = (_o), \
		.name = (_n),   \
	}

struct mt753x_mib_desc {
	unsigned int size;
	unsigned int offset;
	const char *name;
};

static const struct mt753x_mib_desc mt7620_mibs[] = {
	MIB_DESC(1, MT7620_MIB_STATS_PPE_AC_BCNT0, "PPE_AC_BCNT0"),
	MIB_DESC(1, MT7620_MIB_STATS_PPE_AC_PCNT0, "PPE_AC_PCNT0"),
	MIB_DESC(1, MT7620_MIB_STATS_PPE_AC_BCNT63, "PPE_AC_BCNT63"),
	MIB_DESC(1, MT7620_MIB_STATS_PPE_AC_PCNT63, "PPE_AC_PCNT63"),
	MIB_DESC(1, MT7620_MIB_STATS_PPE_MTR_CNT0, "PPE_MTR_CNT0"),
	MIB_DESC(1, MT7620_MIB_STATS_PPE_MTR_CNT63, "PPE_MTR_CNT63"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_TX_GBCNT, "GDM1_TX_GBCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_TX_GPCNT, "GDM1_TX_GPCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_TX_SKIPCNT, "GDM1_TX_SKIPCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_TX_COLCNT, "GDM1_TX_COLCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_RX_GBCNT1, "GDM1_RX_GBCNT1"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_RX_GPCNT1, "GDM1_RX_GPCNT1"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_RX_OERCNT, "GDM1_RX_OERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_RX_FERCNT, "GDM1_RX_FERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_RX_SERCNT, "GDM1_RX_SERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_RX_LERCNT, "GDM1_RX_LERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_RX_CERCNT, "GDM1_RX_CERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM1_RX_FCCNT, "GDM1_RX_FCCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_TX_GBCNT, "GDM2_TX_GBCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_TX_GPCNT, "GDM2_TX_GPCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_TX_SKIPCNT, "GDM2_TX_SKIPCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_TX_COLCNT, "GDM2_TX_COLCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_RX_GBCNT, "GDM2_RX_GBCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_RX_GPCNT, "GDM2_RX_GPCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_RX_OERCNT, "GDM2_RX_OERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_RX_FERCNT, "GDM2_RX_FERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_RX_SERCNT, "GDM2_RX_SERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_RX_LERCNT, "GDM2_RX_LERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_RX_CERCNT, "GDM2_RX_CERCNT"),
	MIB_DESC(1, MT7620_MIB_STATS_GDM2_RX_FCCNT, "GDM2_RX_FCCNT")
};

static const struct mt753x_mib_desc mt753x_mibs[] = {
	MIB_DESC(1, STATS_TDPC, "TxDrop"),
	MIB_DESC(1, STATS_TCRC, "TxCRC"),
	MIB_DESC(1, STATS_TUPC, "TxUni"),
	MIB_DESC(1, STATS_TMPC, "TxMulti"),
	MIB_DESC(1, STATS_TBPC, "TxBroad"),
	MIB_DESC(1, STATS_TCEC, "TxCollision"),
	MIB_DESC(1, STATS_TSCEC, "TxSingleCol"),
	MIB_DESC(1, STATS_TMCEC, "TxMultiCol"),
	MIB_DESC(1, STATS_TDEC, "TxDefer"),
	MIB_DESC(1, STATS_TLCEC, "TxLateCol"),
	MIB_DESC(1, STATS_TXCEC, "TxExcCol"),
	MIB_DESC(1, STATS_TPPC, "TxPause"),
	MIB_DESC(1, STATS_TL64PC, "Tx64Byte"),
	MIB_DESC(1, STATS_TL65PC, "Tx65Byte"),
	MIB_DESC(1, STATS_TL128PC, "Tx128Byte"),
	MIB_DESC(1, STATS_TL256PC, "Tx256Byte"),
	MIB_DESC(1, STATS_TL512PC, "Tx512Byte"),
	MIB_DESC(1, STATS_TL1024PC, "Tx1024Byte"),
	MIB_DESC(2, STATS_TOC, "TxByte"),
	MIB_DESC(1, STATS_RDPC, "RxDrop"),
	MIB_DESC(1, STATS_RFPC, "RxFiltered"),
	MIB_DESC(1, STATS_RUPC, "RxUni"),
	MIB_DESC(1, STATS_RMPC, "RxMulti"),
	MIB_DESC(1, STATS_RBPC, "RxBroad"),
	MIB_DESC(1, STATS_RAEPC, "RxAlignErr"),
	MIB_DESC(1, STATS_RCEPC, "RxCRC"),
	MIB_DESC(1, STATS_RUSPC, "RxUnderSize"),
	MIB_DESC(1, STATS_RFEPC, "RxFragment"),
	MIB_DESC(1, STATS_ROSPC, "RxOverSize"),
	MIB_DESC(1, STATS_RJEPC, "RxJabber"),
	MIB_DESC(1, STATS_RPPC, "RxPause"),
	MIB_DESC(1, STATS_RL64PC, "Rx64Byte"),
	MIB_DESC(1, STATS_RL65PC, "Rx65Byte"),
	MIB_DESC(1, STATS_RL128PC, "Rx128Byte"),
	MIB_DESC(1, STATS_RL256PC, "Rx256Byte"),
	MIB_DESC(1, STATS_RL512PC, "Rx512Byte"),
	MIB_DESC(1, STATS_RL1024PC, "Rx1024Byte"),
	MIB_DESC(2, STATS_ROC, "RxByte"),
	MIB_DESC(1, STATS_RDPC_CTRL, "RxCtrlDrop"),
	MIB_DESC(1, STATS_RDPC_ING, "RxIngDrop"),
	MIB_DESC(1, STATS_RDPC_ARL, "RxARLDrop")
};

enum {
	/* Global attributes. */
	MT753X_ATTR_ENABLE_VLAN,
};

static int mt753x_get_vlan_enable(struct switch_dev *dev,
				  const struct switch_attr *attr,
				  struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	val->value.i = gsw->global_vlan_enable;

	return 0;
}

static int mt753x_set_vlan_enable(struct switch_dev *dev,
				  const struct switch_attr *attr,
				  struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	gsw->global_vlan_enable = val->value.i != 0;

	return 0;
}

static int mt753x_get_port_pvid(struct switch_dev *dev, int port, int *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	if (port >= MT753X_NUM_PORTS)
		return -EINVAL;

	mutex_lock(&gsw->reg_mutex);

	*val = mt753x_reg_read(gsw, PPBV1(port));
	*val &= GRP_PORT_VID_M;

	mutex_unlock(&gsw->reg_mutex);

	return 0;
}

static int mt753x_set_port_pvid(struct switch_dev *dev, int port, int pvid)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	if (port >= MT753X_NUM_PORTS)
		return -EINVAL;

	if (pvid < MT753X_MIN_VID || pvid > MT753X_MAX_VID)
		return -EINVAL;

	gsw->port_entries[port].pvid = pvid;

	return 0;
}

static int mt753x_get_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);
	u32 member;
	u32 etags;
	int i;

	val->len = 0;

	if (val->port_vlan < 0 || val->port_vlan >= MT753X_NUM_VLANS)
		return -EINVAL;

	mutex_lock(&gsw->reg_mutex);

	mt753x_vlan_ctrl(gsw, VTCR_READ_VLAN_ENTRY, val->port_vlan);

	member = mt753x_reg_read(gsw, VAWD1);
	member &= PORT_MEM_M;
	member >>= PORT_MEM_S;

	etags = mt753x_reg_read(gsw, VAWD2);

	mutex_unlock(&gsw->reg_mutex);

	for (i = 0; i < MT753X_NUM_PORTS; i++) {
		struct switch_port *p;
		int etag;

		if (!(member & BIT(i)))
			continue;

		p = &val->value.ports[val->len++];
		p->id = i;

		etag = (etags >> PORT_ETAG_S(i)) & PORT_ETAG_M;

		if (etag == ETAG_CTRL_TAG)
			p->flags |= BIT(SWITCH_PORT_FLAG_TAGGED);
		else if (etag != ETAG_CTRL_UNTAG)
			dev_info(gsw->dev,
				 "vlan egress tag control neither untag nor tag.\n");
	}

	return 0;
}

static int mt753x_set_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);
	u8 member = 0;
	u8 etags = 0;
	int i;

	if (val->port_vlan < 0 || val->port_vlan >= MT753X_NUM_VLANS ||
	    val->len > MT753X_NUM_PORTS)
		return -EINVAL;

	for (i = 0; i < val->len; i++) {
		struct switch_port *p = &val->value.ports[i];

		if (p->id >= MT753X_NUM_PORTS)
			return -EINVAL;

		member |= BIT(p->id);

		if (p->flags & BIT(SWITCH_PORT_FLAG_TAGGED))
			etags |= BIT(p->id);
	}

	gsw->vlan_entries[val->port_vlan].member = member;
	gsw->vlan_entries[val->port_vlan].etags = etags;

	return 0;
}

static int mt753x_set_vid(struct switch_dev *dev,
			  const struct switch_attr *attr,
			  struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);
	int vlan;
	u16 vid;

	vlan = val->port_vlan;
	vid = (u16)val->value.i;

	if (vlan < 0 || vlan >= MT753X_NUM_VLANS)
		return -EINVAL;

	if (vid < MT753X_MIN_VID || vid > MT753X_MAX_VID)
		return -EINVAL;

	gsw->vlan_entries[vlan].vid = vid;
	return 0;
}

static int mt753x_get_vid(struct switch_dev *dev,
			  const struct switch_attr *attr,
			  struct switch_val *val)
{
	val->value.i = val->port_vlan;
	return 0;
}

static int mt753x_get_port_link(struct switch_dev *dev, int port,
				struct switch_port_link *link)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);
	u32 speed, pmsr;

	if (port < 0 || port >= MT753X_NUM_PORTS)
		return -EINVAL;

	mutex_lock(&gsw->reg_mutex);

	pmsr = mt753x_reg_read(gsw, PMSR(port));

	mutex_unlock(&gsw->reg_mutex);

	link->link = pmsr & MAC_LNK_STS;
	link->duplex = pmsr & MAC_DPX_STS;
	speed = (pmsr & MAC_SPD_STS_M) >> MAC_SPD_STS_S;

	switch (speed) {
	case MAC_SPD_10:
		link->speed = SWITCH_PORT_SPEED_10;
		break;
	case MAC_SPD_100:
		link->speed = SWITCH_PORT_SPEED_100;
		break;
	case MAC_SPD_1000:
		link->speed = SWITCH_PORT_SPEED_1000;
		break;
	case MAC_SPD_2500:
		/* TODO: swconfig has no support for 2500 now */
		link->speed = SWITCH_PORT_SPEED_UNKNOWN;
		break;
	}

	return 0;
}

static int mt753x_set_port_link(struct switch_dev *dev, int port,
				struct switch_port_link *link)
{
#ifndef MODULE
	if (port >= MT753X_NUM_PHYS)
		return -EINVAL;

	return switch_generic_set_link(dev, port, link);
#else
	return -ENOTSUPP;
#endif
}

static u64 get_mib_counter(struct gsw_mt753x *gsw, int i, int port)
{
	unsigned int offset;
	u64 lo, hi, hi2;

	offset = mt753x_mibs[i].offset;

	if (mt753x_mibs[i].size == 1)
		return mt753x_reg_read(gsw, MIB_COUNTER_REG(port, offset));

	do {
		hi = mt753x_reg_read(gsw, MIB_COUNTER_REG(port, offset + 4));
		lo = mt753x_reg_read(gsw, MIB_COUNTER_REG(port, offset));
		hi2 = mt753x_reg_read(gsw, MIB_COUNTER_REG(port, offset + 4));
	} while (hi2 != hi);

	return (hi << 32) | lo;
}

static u64 get_mib_counter_7620(struct gsw_mt753x *gsw, int i)
{
	return mt753x_reg_read(gsw, MT7620_MIB_COUNTER_BASE + mt7620_mibs[i].offset);
}

static int mt753x_get_port_mib(struct switch_dev *dev,
			       const struct switch_attr *attr,
			       struct switch_val *val)
{
	static char buf[4096];
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);
	int i, len = 0;

	if (val->port_vlan >= MT753X_NUM_PORTS)
		return -EINVAL;

	len += snprintf(buf + len, sizeof(buf) - len,
			"Port %d MIB counters\n", val->port_vlan);

	for (i = 0; i < ARRAY_SIZE(mt753x_mibs); ++i) {
		u64 counter;

		len += snprintf(buf + len, sizeof(buf) - len,
				"%-11s: ", mt753x_mibs[i].name);
		mutex_lock(&gsw->reg_mutex);
		counter = get_mib_counter(gsw, i, val->port_vlan);
		mutex_unlock(&gsw->reg_mutex);
		len += snprintf(buf + len, sizeof(buf) - len, "%llu\n",
				counter);
	}

	val->value.s = buf;
	val->len = len;
	return 0;
}

static int mt753x_get_port_stats(struct switch_dev *dev, int port,
				 struct switch_port_stats *stats)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	if (port < 0 || port >= MT753X_NUM_PORTS)
		return -EINVAL;

	mutex_lock(&gsw->reg_mutex);

	stats->tx_bytes = get_mib_counter(gsw, MT753X_PORT_MIB_TXB_ID, port);
	stats->rx_bytes = get_mib_counter(gsw, MT753X_PORT_MIB_RXB_ID, port);

	mutex_unlock(&gsw->reg_mutex);

	return 0;
}

static void mt753x_port_isolation(struct gsw_mt753x *gsw)
{
	int i;

	for (i = 0; i < MT753X_NUM_PORTS; i++)
		mt753x_reg_write(gsw, PCR(i),
				 BIT(gsw->cpu_port) << PORT_MATRIX_S);

	mt753x_reg_write(gsw, PCR(gsw->cpu_port), PORT_MATRIX_M);

	for (i = 0; i < MT753X_NUM_PORTS; i++)
		mt753x_reg_write(gsw, PVC(i),
				 (0x8100 << STAG_VPID_S) |
				 (VA_TRANSPARENT_PORT << VLAN_ATTR_S));
}

static int mt753x_apply_config(struct switch_dev *dev)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	if (!gsw->global_vlan_enable) {
		mt753x_port_isolation(gsw);
		return 0;
	}

	mt753x_apply_vlan_config(gsw);

	return 0;
}

static int mt753x_reset_switch(struct switch_dev *dev)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);
	int i;

	memset(gsw->port_entries, 0, sizeof(gsw->port_entries));
	memset(gsw->vlan_entries, 0, sizeof(gsw->vlan_entries));

	/* set default vid of each vlan to the same number of vlan, so the vid
	 * won't need be set explicitly.
	 */
	for (i = 0; i < MT753X_NUM_VLANS; i++)
		gsw->vlan_entries[i].vid = i;

	return 0;
}

static int mt753x_phy_read16(struct switch_dev *dev, int addr, u8 reg,
			     u16 *value)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	*value = gsw->mii_read(gsw, addr, reg);

	return 0;
}

static int mt753x_phy_write16(struct switch_dev *dev, int addr, u8 reg,
			      u16 value)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	gsw->mii_write(gsw, addr, reg, value);

	return 0;
}

static int mt753x_sw_get_mib(struct switch_dev *dev,
				  const struct switch_attr *attr,
				  struct switch_val *val)
{
	static char buf[4096];
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);
	int i, len = 0;

	len += snprintf(buf + len, sizeof(buf) - len, "Switch MIB counters\n");

	for (i = 0; i < ARRAY_SIZE(mt7620_mibs); ++i) {
		u64 counter;
		len += snprintf(buf + len, sizeof(buf) - len,
				"%-11s: ", mt7620_mibs[i].name);
		mutex_lock(&gsw->reg_mutex);
		counter = get_mib_counter_7620(gsw, i);
		mutex_unlock(&gsw->reg_mutex);
		len += snprintf(buf + len, sizeof(buf) - len, "%llu\n",
				counter);
	}

	val->value.s = buf;
	val->len = len;

	return 0;
}

static int
mt753x_get_mirror_monitor_port(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	val->value.i = gsw->mirror_dest_port;

	return 0;
}

static int
mt753x_set_mirror_monitor_port(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	gsw->mirror_dest_port = val->value.i;

	return 0;
}

static int
mt753x_get_port_mirror_rx(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	val->value.i = gsw->port_entries[val->port_vlan].mirror_rx;

	return 0;
}

static int
mt753x_set_port_mirror_rx(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	gsw->port_entries[val->port_vlan].mirror_rx = val->value.i;

	return 0;
}

static int
mt753x_get_port_mirror_tx(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	val->value.i =  gsw->port_entries[val->port_vlan].mirror_tx;

	return 0;
}

static int
mt753x_set_port_mirror_tx(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);

	gsw->port_entries[val->port_vlan].mirror_tx = val->value.i;

	return 0;
}

static char *mt753x_print_arl_table_row(u32 atrd,
					u32 mac1,
					u32 mac2,
					char *buf,
					size_t *size)
{
	int ret;
	size_t port;
	size_t i;
	u8 port_map;
	u8 mac[ETH_ALEN];

	mac1 = ntohl(mac1);
	mac2 = ntohl(mac2);
	port_map = (u8)((atrd & REG_ATRD_PORT_MASK) >> 4);
	memcpy(mac, &mac1, sizeof(mac1));
	memcpy(mac + sizeof(mac1), &mac2, sizeof(mac) - sizeof(mac1));
	for (port = 0, i = 1; port < MT753X_NUM_PORTS; ++port, i <<= 1) {
		if (port_map & i) {
			ret = snprintf(buf, *size, "Port %d: MAC %pM\n", port, mac);
			if (ret >= *size || ret <= 0) {
				*buf = 0;
				buf = NULL;
				goto out;
			}
			buf += ret;
			*size = *size - ret;
		}
	}
out:
	return buf;
}

static int mt753x_get_arl_table(struct switch_dev *dev,
				const struct switch_attr *attr,
				struct switch_val *val)
{
	struct gsw_mt753x *gsw = container_of(dev, struct gsw_mt753x, swdev);
	char *buf = gsw->arl_buf;
	size_t size = sizeof(gsw->arl_buf);
	size_t count = 0;
	size_t retry_times = 100;
	int ret;
	u32 atc;

	mutex_lock(&gsw->reg_mutex);

	ret = snprintf(buf, size, "address resolution table\n");
	if (ret >= size || ret <= 0) {
		gsw->arl_buf[0] = 0;
		goto out;
	}
	buf += ret;
	size = size - ret;

	mt753x_reg_write(gsw, REG_ESW_WT_MAC_ATC, REG_MAC_ATC_START);

	do {
		atc = mt753x_reg_read(gsw, REG_ESW_WT_MAC_ATC);
		if (atc & REG_MAC_ATC_SRCH_HIT && !(atc & REG_MAC_ATC_BUSY)) {
			u32 atrd;

			++count;
			atrd = mt753x_reg_read(gsw, REG_ESW_TABLE_ATRD);
			if (atrd & REG_ATRD_VALID) {
				u32 mac1;
				u32 mac2;

				mac1 = mt753x_reg_read(gsw, REG_ESW_TABLE_TSRA1);
				mac2 = mt753x_reg_read(gsw, REG_ESW_TABLE_TSRA2);

				if (!(atc & REG_MAC_ATC_SRCH_END))
					mt753x_reg_write(gsw, REG_ESW_WT_MAC_ATC, REG_MAC_ATC_NEXT);

				buf = mt753x_print_arl_table_row(atrd, mac1, mac2, buf, &size);
				if (!buf) {
					pr_warn("%s: too many addresses\n", __func__);
					goto out;
				}
			} else if (!(atc & REG_MAC_ATC_SRCH_END)) {
				mt753x_reg_write(gsw, REG_ESW_WT_MAC_ATC, REG_MAC_ATC_NEXT);
			}
		} else {
			--retry_times;
			usleep_range(1000, 5000);
		}
	} while (!(atc & REG_MAC_ATC_SRCH_END) &&
		 count < MT753X_NUM_ARL_RECORDS &&
		 retry_times > 0);
out:
	mutex_unlock(&gsw->reg_mutex);

	val->value.s = gsw->arl_buf;
	val->len = strlen(gsw->arl_buf);

	return 0;
}


static const struct switch_attr mt753x_global[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "VLAN mode (1:enabled)",
		.max = 1,
		.id = MT753X_ATTR_ENABLE_VLAN,
		.get = mt753x_get_vlan_enable,
		.set = mt753x_set_vlan_enable,
	}, {
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get MIB counters for switch",
		.get = mt753x_sw_get_mib,
		.set = NULL,
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "mirror_monitor_port",
		.description = "Mirror monitor port",
		.set = mt753x_set_mirror_monitor_port,
		.get = mt753x_get_mirror_monitor_port,
		.max = MT753X_NUM_PORTS - 1
	}, {
		.type = SWITCH_TYPE_STRING,
		.name = "arl_table",
		.description = "Get ARL table",
		.set = NULL,
		.get = mt753x_get_arl_table,
	},
};

static const struct switch_attr mt753x_port[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get MIB counters for port",
		.get = mt753x_get_port_mib,
		.set = NULL,
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "enable_mirror_rx",
		.description = "Enable mirroring of RX packets",
		.set = mt753x_set_port_mirror_rx,
		.get = mt753x_get_port_mirror_rx,
		.max = 1,
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "enable_mirror_tx",
		.description = "Enable mirroring of TX packets",
		.set = mt753x_set_port_mirror_tx,
		.get = mt753x_get_port_mirror_tx,
		.max = 1,
	},
};

static const struct switch_attr mt753x_vlan[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "vid",
		.description = "VLAN ID (0-4094)",
		.set = mt753x_set_vid,
		.get = mt753x_get_vid,
		.max = 4094,
	},
};

static const struct switch_dev_ops mt753x_swdev_ops = {
	.attr_global = {
		.attr = mt753x_global,
		.n_attr = ARRAY_SIZE(mt753x_global),
	},
	.attr_port = {
		.attr = mt753x_port,
		.n_attr = ARRAY_SIZE(mt753x_port),
	},
	.attr_vlan = {
		.attr = mt753x_vlan,
		.n_attr = ARRAY_SIZE(mt753x_vlan),
	},
	.get_vlan_ports = mt753x_get_vlan_ports,
	.set_vlan_ports = mt753x_set_vlan_ports,
	.get_port_pvid = mt753x_get_port_pvid,
	.set_port_pvid = mt753x_set_port_pvid,
	.get_port_link = mt753x_get_port_link,
	.set_port_link = mt753x_set_port_link,
	.get_port_stats = mt753x_get_port_stats,
	.apply_config = mt753x_apply_config,
	.reset_switch = mt753x_reset_switch,
	.phy_read16 = mt753x_phy_read16,
	.phy_write16 = mt753x_phy_write16,
};

int mt753x_swconfig_init(struct gsw_mt753x *gsw)
{
	struct device_node *np = gsw->dev->of_node;
	struct switch_dev *swdev;
	int ret;

	if (of_property_read_u32(np, "mediatek,cpuport", &gsw->cpu_port))
		gsw->cpu_port = MT753X_DFL_CPU_PORT;

	swdev = &gsw->swdev;

	swdev->name = gsw->name;
	swdev->alias = gsw->name;
	swdev->cpu_port = gsw->cpu_port;
	swdev->ports = MT753X_NUM_PORTS;
	swdev->vlans = MT753X_NUM_VLANS;
	swdev->ops = &mt753x_swdev_ops;

	ret = register_switch(swdev, NULL);
	if (ret) {
		dev_notice(gsw->dev, "Failed to register switch %s\n",
			   swdev->name);
		return ret;
	}

	mt753x_apply_config(swdev);

	return 0;
}

void mt753x_swconfig_destroy(struct gsw_mt753x *gsw)
{
	unregister_switch(&gsw->swdev);
}
