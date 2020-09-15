// SPDX-License-Identifier: GPL-2.0-only

#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/iopoll.h>
#include <linux/mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/phylink.h>
#include <linux/phy_fixed.h>
#include <net/dsa.h>

#include <asm/mach-rtl838x/mach-rtl838x.h>
#include "rtl838x.h"

#define RTL8380_VERSION_A 'A'
#define RTL8390_VERSION_A 'A'
#define RTL8380_VERSION_B 'B'

DEFINE_MUTEX(smi_lock);

#define MIB_DESC(_size, _offset, _name) {.size = _size, .offset = _offset, .name = _name}
struct rtl838x_mib_desc {
	unsigned int size;
	unsigned int offset;
	const char *name;
};

inline void rtl838x_mask_port_reg(u64 clear, u64 set, int reg)
{
	sw_w32_mask((u32)clear, (u32)set, reg);
}

inline void rtl839x_mask_port_reg(u64 clear, u64 set, int reg)
{
	sw_w32_mask((u32) (clear >> 32), (u32) (set >> 32), reg);
	sw_w32_mask((u32) (clear & 0xffffffff), (u32) (set & 0xffffffff), reg + 4);
}

inline void rtl838x_set_port_reg(u64 set, int reg)
{
	sw_w32(set, reg);
}

inline void rtl839x_set_port_reg(u64 set, int reg)
{
	sw_w32(set >> 32, reg);
	sw_w32(set & 0xffffffff, reg + 4);
}

inline u64 rtl838x_get_port_reg(int reg)
{
	return ((u64) sw_r32(reg));
}

inline u64 rtl839x_get_port_reg(int reg)
{
	u64 v = sw_r32(reg);

	v <<= 32;
	v |= sw_r32(reg + 4);
	return v;
}

inline int rtl838x_stat_port_std_mib(int p)
{
	return RTL838X_STAT_PORT_STD_MIB + (p << 8);
}

inline int rtl839x_stat_port_std_mib(int p)
{
	return RTL839X_STAT_PORT_STD_MIB + (p << 8);
}

inline void rtl838x_mask_port_iso_ctrl(u64 clear, u64 set, int port)
{
	sw_w32_mask(clear, set, RTL838X_PORT_ISO_CTRL(port));
}

inline void rtl839x_mask_port_iso_ctrl(u64 clear, u64 set, int port)
{
	sw_w32_mask(clear >> 32, set >> 32, RTL839X_PORT_ISO_CTRL(port));
	sw_w32_mask(clear & 0xffffffff, set & 0xffffffff,
			RTL839X_PORT_ISO_CTRL(port) + 4);
}

inline void rtl838x_set_port_iso_ctrl(u64 set, int port)
{
	sw_w32(set, RTL838X_PORT_ISO_CTRL(port));
}

inline void rtl839x_set_port_iso_ctrl(u64 set, int port)
{
	sw_w32(set >> 32, RTL839X_PORT_ISO_CTRL(port));
	sw_w32(set & 0xffffffff, RTL839X_PORT_ISO_CTRL(port) + 4);
}

inline void rtl838x_exec_tbl0_cmd(u32 cmd)
{
	sw_w32(cmd, RTL838X_TBL_ACCESS_CTRL_0);
	do { } while (sw_r32(RTL838X_TBL_ACCESS_CTRL_0) & (1 << 15));
}

inline void rtl839x_exec_tbl0_cmd(u32 cmd)
{
	sw_w32(cmd, RTL839X_TBL_ACCESS_CTRL_0);
	do { } while (sw_r32(RTL839X_TBL_ACCESS_CTRL_0) & (1 << 16));
}

inline void rtl838x_exec_tbl1_cmd(u32 cmd)
{
	sw_w32(cmd, RTL838X_TBL_ACCESS_CTRL_1);
	do { } while (sw_r32(RTL838X_TBL_ACCESS_CTRL_1) & (1 << 15));
}

inline void rtl839x_exec_tbl1_cmd(u32 cmd)
{
	sw_w32(cmd, RTL839X_TBL_ACCESS_CTRL_1);
	do { } while (sw_r32(RTL839X_TBL_ACCESS_CTRL_1) & (1 << 16));
}

inline int rtl838x_tbl_access_data_0(int i)
{
	return RTL838X_TBL_ACCESS_DATA_0(i);
}

inline int rtl839x_tbl_access_data_0(int i)
{
	return RTL839X_TBL_ACCESS_DATA_0(i);
}

static void rtl839x_vlan_tables_read(u32 vlan, struct rtl838x_vlan_info *info)
{
	u32 cmd;
	u64 v;

	cmd = 1 << 16 /* Execute cmd */
		| 0 << 15 /* Read */
		| 0 << 12 /* Table type 0b000 */
		| (vlan & 0xfff);
	rtl839x_exec_tbl0_cmd(cmd);

	v = sw_r32(RTL838X_TBL_ACCESS_DATA_0(0));
	v <<= 32;
	v |= sw_r32(RTL838X_TBL_ACCESS_DATA_0(1));
	info->tagged_ports = v >> 11;
	info->vlan_conf = (v & 0x7ff) << 2;
	info->vlan_conf |= sw_r32(RTL838X_TBL_ACCESS_DATA_0(1)) >> 30;

	cmd = 1 << 16 /* Execute cmd */
		| 0 << 15 /* Read */
		| 0 << 12 /* Table type 0b000 */
		| (vlan & 0xfff);
	rtl839x_exec_tbl1_cmd(cmd);
	v = sw_r32(RTL838X_TBL_ACCESS_DATA_1(0));
	v <<= 32;
	v |= sw_r32(RTL838X_TBL_ACCESS_DATA_1(1));
	info->untagged_ports = v >> 11;
}

static void rtl838x_vlan_tables_read(u32 vlan, struct rtl838x_vlan_info *info)
{
	u32 cmd;

	cmd = 1 << 15 /* Execute cmd */
		| 1 << 14 /* Read */
		| 0 << 12 /* Table type 0b00 */
		| (vlan & 0xfff);
	rtl838x_exec_tbl0_cmd(cmd);
	info->tagged_ports = sw_r32(RTL838X_TBL_ACCESS_DATA_0(0));
	info->vlan_conf = sw_r32(RTL838X_TBL_ACCESS_DATA_0(1));

	cmd = 1 << 15 /* Execute cmd */
		| 1 << 14 /* Read */
		| 0 << 12 /* Table type 0b00 */
		| (vlan & 0xfff);
	rtl838x_exec_tbl1_cmd(cmd);
	info->untagged_ports = sw_r32(RTL838X_TBL_ACCESS_DATA_1(0));
}

static void rtl839x_vlan_set_tagged(u32 vlan, u64 portmask, u32 conf)
{
	u32 cmd = 1 << 16 /* Execute cmd */
		| 1 << 15 /* Write */
		| 0 << 12 /* Table type 0b00 */
		| (vlan & 0xfff);
	u64 v = portmask << 11;

	v |= (conf >> 2) & 0x7ff;
	sw_w64(v, RTL838X_TBL_ACCESS_DATA_0(0));
	sw_w32(conf << 30, RTL838X_TBL_ACCESS_DATA_0(2));
	rtl839x_exec_tbl0_cmd(cmd);
}

static void rtl838x_vlan_set_tagged(u32 vlan, u64 portmask, u32 conf)
{
	u32 cmd = 1 << 15 /* Execute cmd */
		| 0 << 14 /* Write */
		| 0 << 12 /* Table type 0b00 */
		| (vlan & 0xfff);

	sw_w32(portmask, RTL838X_TBL_ACCESS_DATA_0(0));
	sw_w32(conf, RTL838X_TBL_ACCESS_DATA_0(1));
	rtl838x_exec_tbl0_cmd(cmd);
}

static void rtl839x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	u32 cmd = 1 << 16 /* Execute cmd */
		| 1 << 15 /* Write */
		| 0 << 12 /* Table type 0b00 */
		| (vlan & 0xfff);
	sw_w64(portmask << 11, RTL838X_TBL_ACCESS_DATA_1(0));
	rtl839x_exec_tbl1_cmd(cmd);
}

static void rtl838x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	u32 cmd = 1 << 15 /* Execute cmd */
		| 0 << 14 /* Write */
		| 0 << 12 /* Table type 0b00 */
		| (vlan & 0xfff);
	sw_w32(portmask & 0x1fffffff, RTL838X_TBL_ACCESS_DATA_1(0));
	rtl838x_exec_tbl1_cmd(cmd);
}

static inline int rtl838x_mac_force_mode_ctrl(int p)
{
	return RTL838X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl839x_mac_force_mode_ctrl(int p)
{
	return RTL839X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static const struct rtl838x_reg rtl838x_reg = {
	.mask_port_reg = rtl838x_mask_port_reg,
	.set_port_reg = rtl838x_set_port_reg,
	.get_port_reg = rtl838x_get_port_reg,
	.stat_port_rst = RTL838X_STAT_PORT_RST,
	.stat_rst = RTL838X_STAT_RST,
	.stat_port_std_mib = rtl838x_stat_port_std_mib,
	.mask_port_iso_ctrl = rtl838x_mask_port_iso_ctrl,
	.set_port_iso_ctrl = rtl838x_set_port_iso_ctrl,
	.l2_ctrl_0 = RTL838X_L2_CTRL_0,
	.l2_ctrl_1 = RTL838X_L2_CTRL_1,
	.l2_port_aging_out = RTL838X_L2_PORT_AGING_OUT,
	.smi_poll_ctrl = RTL838X_SMI_POLL_CTRL,
	.l2_tbl_flush_ctrl = RTL838X_L2_TBL_FLUSH_CTRL,
	.exec_tbl0_cmd = rtl838x_exec_tbl0_cmd,
	.exec_tbl1_cmd = rtl838x_exec_tbl1_cmd,
	.tbl_access_data_0 = rtl838x_tbl_access_data_0,
	.isr_glb_src = RTL838X_ISR_GLB_SRC,
	.isr_port_link_sts_chg = RTL838X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL838X_IMR_PORT_LINK_STS_CHG,
	.imr_glb = RTL838X_IMR_GLB,
	.vlan_tables_read = rtl838x_vlan_tables_read,
	.vlan_set_tagged = rtl838x_vlan_set_tagged,
	.vlan_set_untagged = rtl838x_vlan_set_untagged,
	.mac_force_mode_ctrl = rtl838x_mac_force_mode_ctrl,
	.rst_glb_ctrl = RTL838X_RST_GLB_CTRL_0,
};

static const struct rtl838x_reg rtl839x_reg = {
	.mask_port_reg = rtl839x_mask_port_reg,
	.set_port_reg = rtl839x_set_port_reg,
	.get_port_reg = rtl839x_get_port_reg,
	.stat_port_rst = RTL839X_STAT_PORT_RST,
	.stat_rst = RTL839X_STAT_RST,
	.stat_port_std_mib = rtl839x_stat_port_std_mib,
	.mask_port_iso_ctrl = rtl839x_mask_port_iso_ctrl,
	.set_port_iso_ctrl = rtl839x_set_port_iso_ctrl,
	.l2_ctrl_0 = RTL839X_L2_CTRL_0,
	.l2_ctrl_1 = RTL839X_L2_CTRL_1,
	.l2_port_aging_out = RTL839X_L2_PORT_AGING_OUT,
	.smi_poll_ctrl = RTL839X_SMI_PORT_POLLING_CTRL,
	.l2_tbl_flush_ctrl = RTL839X_L2_TBL_FLUSH_CTRL,
	.exec_tbl0_cmd = rtl839x_exec_tbl0_cmd,
	.exec_tbl1_cmd = rtl839x_exec_tbl1_cmd,
	.tbl_access_data_0 = rtl839x_tbl_access_data_0,
	.isr_glb_src = RTL839X_ISR_GLB_SRC,
	.isr_port_link_sts_chg = RTL839X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL839X_IMR_PORT_LINK_STS_CHG,
	.imr_glb = RTL839X_IMR_GLB,
	.vlan_tables_read = rtl839x_vlan_tables_read,
	.vlan_set_tagged = rtl839x_vlan_set_tagged,
	.vlan_set_untagged = rtl839x_vlan_set_untagged,
	.mac_force_mode_ctrl = rtl839x_mac_force_mode_ctrl,
	.rst_glb_ctrl = RTL839X_RST_GLB_CTRL,
};

static const struct rtl838x_mib_desc rtl838x_mib[] = {
	MIB_DESC(2, 0xf8, "ifInOctets"),
	MIB_DESC(2, 0xf0, "ifOutOctets"),
	MIB_DESC(1, 0xec, "dot1dTpPortInDiscards"),
	MIB_DESC(1, 0xe8, "ifInUcastPkts"),
	MIB_DESC(1, 0xe4, "ifInMulticastPkts"),
	MIB_DESC(1, 0xe0, "ifInBroadcastPkts"),
	MIB_DESC(1, 0xdc, "ifOutUcastPkts"),
	MIB_DESC(1, 0xd8, "ifOutMulticastPkts"),
	MIB_DESC(1, 0xd4, "ifOutBroadcastPkts"),
	MIB_DESC(1, 0xd0, "ifOutDiscards"),
	MIB_DESC(1, 0xcc, ".3SingleCollisionFrames"),
	MIB_DESC(1, 0xc8, ".3MultipleCollisionFrames"),
	MIB_DESC(1, 0xc4, ".3DeferredTransmissions"),
	MIB_DESC(1, 0xc0, ".3LateCollisions"),
	MIB_DESC(1, 0xbc, ".3ExcessiveCollisions"),
	MIB_DESC(1, 0xb8, ".3SymbolErrors"),
	MIB_DESC(1, 0xb4, ".3ControlInUnknownOpcodes"),
	MIB_DESC(1, 0xb0, ".3InPauseFrames"),
	MIB_DESC(1, 0xac, ".3OutPauseFrames"),
	MIB_DESC(1, 0xa8, "DropEvents"),
	MIB_DESC(1, 0xa4, "tx_BroadcastPkts"),
	MIB_DESC(1, 0xa0, "tx_MulticastPkts"),
	MIB_DESC(1, 0x9c, "CRCAlignErrors"),
	MIB_DESC(1, 0x98, "tx_UndersizePkts"),
	MIB_DESC(1, 0x94, "rx_UndersizePkts"),
	MIB_DESC(1, 0x90, "rx_UndersizedropPkts"),
	MIB_DESC(1, 0x8c, "tx_OversizePkts"),
	MIB_DESC(1, 0x88, "rx_OversizePkts"),
	MIB_DESC(1, 0x84, "Fragments"),
	MIB_DESC(1, 0x80, "Jabbers"),
	MIB_DESC(1, 0x7c, "Collisions"),
	MIB_DESC(1, 0x78, "tx_Pkts64Octets"),
	MIB_DESC(1, 0x74, "rx_Pkts64Octets"),
	MIB_DESC(1, 0x70, "tx_Pkts65to127Octets"),
	MIB_DESC(1, 0x6c, "rx_Pkts65to127Octets"),
	MIB_DESC(1, 0x68, "tx_Pkts128to255Octets"),
	MIB_DESC(1, 0x64, "rx_Pkts128to255Octets"),
	MIB_DESC(1, 0x60, "tx_Pkts256to511Octets"),
	MIB_DESC(1, 0x5c, "rx_Pkts256to511Octets"),
	MIB_DESC(1, 0x58, "tx_Pkts512to1023Octets"),
	MIB_DESC(1, 0x54, "rx_Pkts512to1023Octets"),
	MIB_DESC(1, 0x50, "tx_Pkts1024to1518Octets"),
	MIB_DESC(1, 0x4c, "rx_StatsPkts1024to1518Octets"),
	MIB_DESC(1, 0x48, "tx_Pkts1519toMaxOctets"),
	MIB_DESC(1, 0x44, "rx_Pkts1519toMaxOctets"),
	MIB_DESC(1, 0x40, "rxMacDiscards")
};

static irqreturn_t rtl838x_switch_irq(int irq, void *dev_id)
{
	struct dsa_switch *ds = dev_id;
	u32 status = sw_r32(RTL838X_ISR_GLB_SRC);
	u32 ports = sw_r32(RTL838X_ISR_PORT_LINK_STS_CHG);
	u32 link;
	int i;

	/* Clear status */
	sw_w32(ports, RTL838X_ISR_PORT_LINK_STS_CHG);
	pr_info("Link change: status: %x, ports %x\n", status, ports);

	for (i = 0; i < 28; i++) {
		if (ports & (1 << i)) {
			link = sw_r32(RTL838X_MAC_LINK_STS);
			if (link & (1 << i))
				dsa_port_phylink_mac_change(ds, i, true);
			else
				dsa_port_phylink_mac_change(ds, i, false);
		}
	}
	return IRQ_HANDLED;
}

static irqreturn_t rtl839x_switch_irq(int irq, void *dev_id)
{
	struct dsa_switch *ds = dev_id;
	u32 status = sw_r32(RTL839X_ISR_GLB_SRC);
	u64 ports = sw_r64(RTL839X_ISR_PORT_LINK_STS_CHG);
	u64 link;
	int i;

	/* Clear status */
	sw_w64(ports, RTL839X_ISR_PORT_LINK_STS_CHG);
	pr_info("Link change: status: %x, ports %llx\n", status, ports);

	for (i = 0; i < 52; i++) {
		if (ports & (1 << i)) {
			link = sw_r64(RTL839X_MAC_LINK_STS);
			if (link & (1 << i))
				dsa_port_phylink_mac_change(ds, i, true);
			else
				dsa_port_phylink_mac_change(ds, i, false);
		}
	}
	return IRQ_HANDLED;
}

int rtl8380_sds_power(int mac, int val)
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

static int rtl838x_smi_wait_op(int timeout)
{
	do {
		timeout--;
		udelay(10);
	} while ((sw_r32(RTL838X_SMI_ACCESS_PHY_CTRL_1) & 0x1) && (timeout >= 0));
	if (timeout <= 0)
		return -1;
	return 0;
}

/*
 * Write to a register in a page of the PHY
 */
int rtl838x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v;
	u32 park_page;

	val &= 0xffff;
	if (port > 31 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&smi_lock);
	if (rtl838x_smi_wait_op(10000))
		goto timeout;

	sw_w32(1 << port, RTL838X_SMI_ACCESS_PHY_CTRL_0);
	mdelay(10);

	sw_w32_mask(0xffff0000, val << 16, RTL838X_SMI_ACCESS_PHY_CTRL_2);

	park_page = sw_r32(RTL838X_SMI_ACCESS_PHY_CTRL_1) & ((0x1f << 15) | 0x2);
	v = reg << 20 | page << 3 | 0x4;
	sw_w32(v | park_page, RTL838X_SMI_ACCESS_PHY_CTRL_1);
	sw_w32_mask(0, 1, RTL838X_SMI_ACCESS_PHY_CTRL_1);

	if (rtl838x_smi_wait_op(10000))
		goto timeout;

	mutex_unlock(&smi_lock);
	return 0;

timeout:
	mutex_unlock(&smi_lock);
	return -ETIMEDOUT;
}

int rtl839x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v;
	int err = 0;

	val &= 0xffff;
	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&smi_lock);
	/* Clear both port registers */
	sw_w32(0, RTL839X_PHYREG_PORT_CTRL(0));
	sw_w32(0, RTL839X_PHYREG_PORT_CTRL(0) + 4);
	sw_w32_mask(0, 1 << port, RTL839X_PHYREG_PORT_CTRL(port));

	sw_w32_mask(0xffff0000, val << 16, RTL839X_PHYREG_DATA_CTRL);

	v = reg << 5 | page << 10 | ((page == 0x1fff) ? 0x1f : 0) << 23;
	sw_w32(v, RTL839X_PHYREG_ACCESS_CTRL);

	sw_w32(0x1ff, RTL839X_PHYREG_CTRL);

	v |= 1 << 3 | 1; /* Write operation and execute */
	sw_w32(v, RTL839X_PHYREG_ACCESS_CTRL);

	do {
	} while (sw_r32(RTL839X_PHYREG_ACCESS_CTRL) & 0x1);

	if (sw_r32(RTL839X_PHYREG_ACCESS_CTRL) & 0x2)
		err = -EIO;

	mutex_unlock(&smi_lock);
	return err;
}

/*
 * Reads a register in a page from the PHY
 */
int rtl838x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	u32 v;
	u32 park_page;

	if (port > 31 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&smi_lock);

	if (rtl838x_smi_wait_op(10000))
		goto timeout;

	sw_w32_mask(0xffff0000, port << 16, RTL838X_SMI_ACCESS_PHY_CTRL_2);

	park_page = sw_r32(RTL838X_SMI_ACCESS_PHY_CTRL_1) & ((0x1f << 15) | 0x2);
	v = reg << 20 | page << 3;
	sw_w32(v | park_page, RTL838X_SMI_ACCESS_PHY_CTRL_1);
	sw_w32_mask(0, 1, RTL838X_SMI_ACCESS_PHY_CTRL_1);

	if (rtl838x_smi_wait_op(10000))
		goto timeout;

	*val = sw_r32(RTL838X_SMI_ACCESS_PHY_CTRL_2) & 0xffff;

	mutex_unlock(&smi_lock);
	return 0;

timeout:
	mutex_unlock(&smi_lock);
	return -ETIMEDOUT;
}

int rtl839x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	u32 v;

	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&smi_lock);

	sw_w32_mask(0xffff0000, port << 16, RTL839X_PHYREG_DATA_CTRL);
	v = reg << 5 | page << 10 | ((page == 0x1fff) ? 0x1f : 0) << 23;
	sw_w32(v, RTL839X_PHYREG_ACCESS_CTRL);

	sw_w32(0x1ff, RTL839X_PHYREG_CTRL);

	v |= 1;
	sw_w32(v, RTL839X_PHYREG_ACCESS_CTRL);

	do {
	} while (sw_r32(RTL839X_PHYREG_ACCESS_CTRL) & 0x1);

	*val = sw_r32(RTL839X_PHYREG_DATA_CTRL) & 0xffff;

	mutex_unlock(&smi_lock);
	return 0;
}

static int read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	if (soc_info.family == RTL8390_FAMILY_ID)
		return rtl839x_read_phy(port, page, reg, val);
	else
		return rtl838x_read_phy(port, page, reg, val);
}

static int write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	if (soc_info.family == RTL8390_FAMILY_ID)
		return rtl839x_write_phy(port, page, reg, val);
	else
		return rtl838x_write_phy(port, page, reg, val);
}

/*
 * Write to an mmd register of the PHY
 */
int rtl838x_write_mmd_phy(u32 port, u32 addr, u32 reg, u32 val)
{
	u32 v;

	pr_debug("MMD write: port %d, dev %d, reg %d, val %x\n", port, addr, reg, val);
	val &= 0xffff;
	mutex_lock(&smi_lock);

	if (rtl838x_smi_wait_op(10000))
		goto timeout;

	sw_w32(1 << port, RTL838X_SMI_ACCESS_PHY_CTRL_0);
	mdelay(10);

	sw_w32_mask(0xffff0000, val << 16, RTL838X_SMI_ACCESS_PHY_CTRL_2);

	sw_w32_mask(0x1f << 16, addr << 16, RTL838X_SMI_ACCESS_PHY_CTRL_3);
	sw_w32_mask(0xffff, reg, RTL838X_SMI_ACCESS_PHY_CTRL_3);
	/* mmd-access | write | cmd-start */
	v = 1 << 1 | 1 << 2 | 1;
	sw_w32(v, RTL838X_SMI_ACCESS_PHY_CTRL_1);

	if (rtl838x_smi_wait_op(10000))
		goto timeout;

	mutex_unlock(&smi_lock);
	return 0;

timeout:
	mutex_unlock(&smi_lock);
	return -ETIMEDOUT;
}

/*
 * Read to an mmd register of the PHY
 */
int rtl838x_read_mmd_phy(u32 port, u32 addr, u32 reg, u32 *val)
{
	u32 v;

	mutex_lock(&smi_lock);

	if (rtl838x_smi_wait_op(10000))
		goto timeout;

	sw_w32(1 << port, RTL838X_SMI_ACCESS_PHY_CTRL_0);
	mdelay(10);

	sw_w32_mask(0xffff0000, port << 16, RTL838X_SMI_ACCESS_PHY_CTRL_2);

	v = addr << 16 | reg;
	sw_w32(v, RTL838X_SMI_ACCESS_PHY_CTRL_3);

	/* mmd-access | read | cmd-start */
	v = 1 << 1 | 0 << 2 | 1;
	sw_w32(v, RTL838X_SMI_ACCESS_PHY_CTRL_1);

	if (rtl838x_smi_wait_op(10000))
		goto timeout;

	*val = sw_r32(RTL838X_SMI_ACCESS_PHY_CTRL_2) & 0xffff;

	mutex_unlock(&smi_lock);
	return 0;

timeout:
	mutex_unlock(&smi_lock);
	return -ETIMEDOUT;
}

static void rtl8380_get_version(struct rtl838x_switch_priv *priv)
{
	u32 rw_save, info_save;
	u32 info;

	if (priv->id)
		pr_debug("SoC ID: %4x: %s\n", priv->id, soc_info.name);
	else
		pr_err("Unknown chip id (%04x)\n", priv->id);

	rw_save = sw_r32(RTL838X_INT_RW_CTRL);
	sw_w32(rw_save | 0x3, RTL838X_INT_RW_CTRL);

	info_save = sw_r32(RTL838X_CHIP_INFO);
	sw_w32(info_save | 0xA0000000, RTL838X_CHIP_INFO);

	info = sw_r32(RTL838X_CHIP_INFO);
	sw_w32(info_save, RTL838X_CHIP_INFO);
	sw_w32(rw_save, RTL838X_INT_RW_CTRL);

	if ((info & 0xFFFF) == 0x6275) {
		if (((info >> 16) & 0x1F) == 0x1)
			priv->version = RTL8380_VERSION_A;
		else if (((info >> 16) & 0x1F) == 0x2)
			priv->version = RTL8380_VERSION_B;
		else
			priv->version = RTL8380_VERSION_B;
	} else {
		priv->version = '-';
	}
}

static void rtl8390_get_version(struct rtl838x_switch_priv *priv)
{
	u32 info;

	sw_w32_mask(0xf << 28, 0xa << 28, RTL839X_CHIP_INFO);
	info = sw_r32(RTL839X_CHIP_INFO);
	pr_info("Chip-Info: %x\n", info);
	priv->version = RTL8390_VERSION_A;
}

int dsa_phy_read(struct dsa_switch *ds, int phy_addr, int phy_reg)
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

	read_phy(phy_addr, 0, phy_reg, &val);
	return val;
}

int dsa_phy_write(struct dsa_switch *ds, int phy_addr, int phy_reg, u16 val)
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
	return write_phy(phy_addr, 0, phy_reg, val);
}

static int rtl838x_mdio_read(struct mii_bus *bus, int addr, int regnum)
{
	int ret;
	struct rtl838x_switch_priv *priv = bus->priv;

	ret = dsa_phy_read(priv->ds, addr, regnum);
	return ret;
}

static int rtl838x_mdio_write(struct mii_bus *bus, int addr, int regnum,
				 u16 val)
{
	struct rtl838x_switch_priv *priv = bus->priv;

	return dsa_phy_write(priv->ds, addr, regnum, val);
}

static void rtl838x_enable_phy_polling(struct rtl838x_switch_priv *priv)
{
	int i;
	u64 v = 0;

	msleep(1000);
	/* Enable all ports with a PHY, including the SFP-ports */
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy)
			v |= 1 << i;
	}

	pr_info("%s: %16llx\n", __func__, v);
	priv->r->set_port_reg(v, priv->r->smi_poll_ctrl);

	/* PHY update complete */
	if (priv->family_id == RTL8390_FAMILY_ID)
		sw_w32_mask(1 << 7, 0, RTL839X_SMI_GLB_CTRL);
	else
		sw_w32_mask(0, 0x8000, RTL838X_SMI_GLB_CTRL);
}

void rtl839x_print_matrix(void)
{
	volatile u64 *ptr = RTL838X_SW_BASE + RTL839X_PORT_ISO_CTRL(0);
	int i;

	for (i = 0; i < 52; i += 4)
		pr_info("> %16llx %16llx %16llx %16llx\n",
			ptr[i + 0], ptr[i + 1], ptr[i + 2], ptr[i + 3]);
	pr_info("CPU_PORT> %16llx\n", ptr[52]);
}

void rtl838x_print_matrix(void)
{
	unsigned volatile int *ptr = RTL838X_SW_BASE + RTL838X_PORT_ISO_CTRL(0);
	int i;

	if (soc_info.family == RTL8390_FAMILY_ID)
		return rtl839x_print_matrix();

	for (i = 0; i < 28; i += 8)
		pr_info("> %8x %8x %8x %8x %8x %8x %8x %8x\n",
			ptr[i + 0], ptr[i + 1], ptr[i + 2], ptr[i + 3], ptr[i + 4], ptr[i + 5],
			ptr[i + 6], ptr[i + 7]);
	pr_info("CPU_PORT> %8x\n", ptr[28]);
}

static void rtl838x_init_stats(struct rtl838x_switch_priv *priv)
{
	mutex_lock(&priv->reg_mutex);

	/* Enable statistics module: all counters plus debug.
	 * On RTL839x all counters are enabled by default
	 */
	if (priv->family_id == RTL8380_FAMILY_ID)
		sw_w32_mask(0, 3, RTL838X_STAT_CTRL);

	/* Reset statistics counters */
	sw_w32_mask(0, 1, priv->r->stat_rst);

	mutex_unlock(&priv->reg_mutex);
}

static int rtl838x_setup(struct dsa_switch *ds)
{
	int i;
	u64 port_bitmap = 0;
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("%s called\n", __func__);

	/* Disable MAC polling the PHY so that we can start configuration */
	priv->r->set_port_reg(0, priv->r->smi_poll_ctrl);

	for (i = 0; i < ds->num_ports; i++)
		priv->ports[i].enable = false;
	priv->ports[priv->cpu_port].enable = true;

	/* Isolate ports from each other: traffic only CPU <-> port */
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy) {
			priv->r->set_port_iso_ctrl(1 << priv->cpu_port, i);
			priv->r->mask_port_iso_ctrl(0, 1 << i, i);
			port_bitmap |= 1 << i;
		}
	}
	priv->r->set_port_iso_ctrl(port_bitmap, priv->cpu_port);

	rtl838x_print_matrix();

	rtl838x_init_stats(priv);

	/* Enable MAC Polling PHY again */
	rtl838x_enable_phy_polling(priv);
	pr_info("Please wait until PHY is settled\n");
	msleep(1000);
	return 0;
}

static void rtl838x_get_strings(struct dsa_switch *ds,
				int port, u32 stringset, u8 *data)
{
	int i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < ARRAY_SIZE(rtl838x_mib); i++)
		strncpy(data + i * ETH_GSTRING_LEN, rtl838x_mib[i].name,
			ETH_GSTRING_LEN);
}

static void rtl838x_get_ethtool_stats(struct dsa_switch *ds, int port,
				      uint64_t *data)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	const struct rtl838x_mib_desc *mib;
	int i;
	u64 high;

	for (i = 0; i < ARRAY_SIZE(rtl838x_mib); i++) {
		mib = &rtl838x_mib[i];

		data[i] = sw_r32(priv->r->stat_port_std_mib(port) + 252 - mib->offset);
		if (mib->size == 2) {
			high = sw_r32(priv->r->stat_port_std_mib(port) + 252 - mib->offset - 4);
			data[i] |= high << 32;
		}
	}
}

static int rtl838x_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return 0;

	return ARRAY_SIZE(rtl838x_mib);
}

static enum dsa_tag_protocol
rtl838x_get_tag_protocol(struct dsa_switch *ds, int port)
{
	/* The switch does not tag the frames, instead internally the header
	 * structure for each packet is tagged accordingly.
	 */
	return DSA_TAG_PROTO_TRAILER;
}

static int rtl838x_get_l2aging(struct rtl838x_switch_priv *priv)
{
	int t = sw_r32(priv->r->l2_ctrl_1) & 0x7fffff;

	pr_debug("RTL838X_L2_CTRL_1 %x\n", sw_r32(priv->r->l2_ctrl_1));

	t = t * 128 / 625; /* Aging time in seconds. 0: L2 aging disabled */
	pr_info("L2 AGING time: %d sec\n", t);
	pr_info("Dynamic aging for ports: %x\n",
		sw_r32(priv->r->l2_port_aging_out));
	return t;
}

/*
 * Set Switch L2 Aging time, t is time in milliseconds
 * t = 0: aging is disabled
 */
static int rtl838x_set_l2aging(struct dsa_switch *ds, u32 t)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	/* Convert time in mseconds to internal value */
	if (t > 0x10000000) /* Set to maximum */
		t = 0x7fffff;
	else
		t = ((t * 625) / 1000 + 127) / 128;

	sw_w32(t, priv->r->l2_ctrl_1);

	return 0;
}

static void rtl838x_fast_age(struct dsa_switch *ds, int port)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	int s = priv->family_id == RTL8390_FAMILY_ID ? 2 : 0;

	pr_info("FAST AGE port %d\n", port);
	mutex_lock(&priv->reg_mutex);
	/* RTL838X_L2_TBL_FLUSH_CTRL register bits, 839x has 1 bit larger
	 * port fields:
	 * 0-4: Replacing port
	 * 5-9: Flushed/replaced port
	 * 10-21: FVID
	 * 22: Entry types: 1: dynamic, 0: also static
	 * 23: Match flush port
	 * 24: Match FVID
	 * 25: Flush (0) or replace (1) L2 entries
	 * 26: Status of action (1: Start, 0: Done)
	 */
	sw_w32(1 << (26 + s)  | 1 << (23 + s) | port << 5, priv->r->l2_tbl_flush_ctrl);

	do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & (1 << (26 + s)));

	mutex_unlock(&priv->reg_mutex);
}

/*
 * Applies the same hash algorithm as the one used currently by the ASIC
 */
static u32 rtl838x_hash(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 h1, h2, h3, h;

	if (sw_r32(priv->r->l2_ctrl_0) & 1) {
		h1 = (seed >> 11) & 0x7ff;
		h1 = ((h1 & 0x1f) << 6) | ((h1 >> 5) & 0x3f);

		h2 = (seed >> 33) & 0x7ff;
		h2 = ((h2 & 0x3f) << 5) | ((h2 >> 6) & 0x1f);

		h3 = (seed >> 44) & 0x7ff;
		h3 = ((h3 & 0x7f) << 4) | ((h3 >> 7) & 0xf);

		h = h1 ^ h2 ^ h3 ^ ((seed >> 55) & 0x1ff);
		h ^= ((seed >> 22) & 0x7ff) ^ (seed & 0x7ff);
	} else {
		h = ((seed >> 55) & 0x1ff) ^ ((seed >> 44) & 0x7ff)
			^ ((seed >> 33) & 0x7ff) ^ ((seed >> 22) & 0x7ff)
			^ ((seed >> 11) & 0x7ff) ^ (seed & 0x7ff);
	}

	return h;
}

static u64 rtl838x_hash_key(struct rtl838x_switch_priv *priv, u64 mac, u32 vid)
{
	return rtl838x_hash(priv, mac << 12 | vid);
}

static u64 read_l2_entry_using_hash(u32 hash, u32 position, u32 *r)
{
	u64 entry;
	/* Search in SRAM, with hash and at position in hash bucket (0-3) */
	u32 idx = (0 << 14) | (hash << 2) | position;

	u32 cmd = 1 << 16 /* Execute cmd */
		| 1 << 15 /* Read */
		| 0 << 13 /* Table type 0b00 */
		| (idx & 0x1fff);

	sw_w32(cmd, RTL838X_TBL_ACCESS_L2_CTRL);
	do { }  while (sw_r32(RTL838X_TBL_ACCESS_L2_CTRL) & (1 << 16));
	r[0] = sw_r32(RTL838X_TBL_ACCESS_L2_DATA(0));
	r[1] = sw_r32(RTL838X_TBL_ACCESS_L2_DATA(1));
	r[2] = sw_r32(RTL838X_TBL_ACCESS_L2_DATA(2));

	entry = (((u64) r[1]) << 32) | (r[2] & 0xfffff000) | (r[0] & 0xfff);
	return entry;
}

static u64 rtl838x_read_cam(int idx, u32 *r)
{
	u64 entry;
	u32 cmd = 1 << 16 /* Execute cmd */
		| 1 << 15 /* Read */
		| 1 << 13 /* Table type 0b01 */
		| (idx & 0x3f);
	sw_w32(cmd, RTL838X_TBL_ACCESS_L2_CTRL);
	do { }  while (sw_r32(RTL838X_TBL_ACCESS_L2_CTRL) & (1 << 16));
	r[0] = sw_r32(RTL838X_TBL_ACCESS_L2_DATA(0));
	r[1] = sw_r32(RTL838X_TBL_ACCESS_L2_DATA(1));
	r[2] = sw_r32(RTL838X_TBL_ACCESS_L2_DATA(2));

	entry = (((u64) r[1]) << 32) | (r[2] & 0xfffff000) | (r[0] & 0xfff);
	return entry;
}

static void rtl838x_write_cam(int idx, u32 *r)
{
	u32 cmd = 1 << 16 /* Execute cmd */
		| 1 << 15 /* Read */
		| 1 << 13 /* Table type 0b01 */
		| (idx & 0x3f);

	sw_w32(r[0], RTL838X_TBL_ACCESS_L2_DATA(0));
	sw_w32(r[1], RTL838X_TBL_ACCESS_L2_DATA(1));
	sw_w32(r[2], RTL838X_TBL_ACCESS_L2_DATA(2));

	sw_w32(cmd, RTL838X_TBL_ACCESS_L2_CTRL);
	do { }  while (sw_r32(RTL838X_TBL_ACCESS_L2_CTRL) & (1 << 16));
}

static void rtl838x_write_hash(int idx, u32 *r)
{
	u32 cmd = 1 << 16 /* Execute cmd */
		| 0 << 15 /* Write */
		| 0 << 13 /* Table type 0b00 */
		| (idx & 0x1fff);

	sw_w32(0, RTL838X_TBL_ACCESS_L2_DATA(0));
	sw_w32(0, RTL838X_TBL_ACCESS_L2_DATA(1));
	sw_w32(0, RTL838X_TBL_ACCESS_L2_DATA(2));
	sw_w32(cmd, RTL838X_TBL_ACCESS_L2_CTRL);
	do { }  while (sw_r32(RTL838X_TBL_ACCESS_L2_CTRL) & (1 << 16));
}

static void dump_fdb(struct rtl838x_switch_priv *priv)
{
	u32 r[3];
	int i;
	u8 mac[6];
	u16 vid, rvid;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < 8192; i++) {
		read_l2_entry_using_hash(i >> 2, i & 0x3, r);
		mac[0] = (r[1] >> 20);
		mac[1] = (r[1] >> 12);
		mac[2] = (r[1] >> 4);
		mac[3] = (r[1] & 0xf) << 4 | (r[2] >> 28);
		mac[4] = (r[2] >> 20);
		mac[5] = (r[2] >> 12);
		vid = r[0] & 0xfff;
		rvid = r[2] & 0xfff;

		if (!(r[0] >> 17)) /* Check for invalid entry */
			continue;

		pr_info("-> port %02d: %pM, vid: %d, rvid: %d\n",
			(r[0] >> 12) & priv->port_mask, &mac[0], vid, rvid);
	}

	mutex_unlock(&priv->reg_mutex);
}

static int rtl838x_port_fdb_dump(struct dsa_switch *ds, int port,
				 dsa_fdb_dump_cb_t *cb, void *data)
{
	u32 r[3];
	u8 mac[6];
	u16 vid, rvid;
	struct rtl838x_switch_priv *priv = ds->priv;
	int i;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < 8192; i++) {
		read_l2_entry_using_hash(i >> 2, i & 0x3, r);
		mac[0] = (r[1] >> 20);
		mac[1] = (r[1] >> 12);
		mac[2] = (r[1] >> 4);
		mac[3] = (r[1] & 0xf) << 4 | (r[2] >> 28);
		mac[4] = (r[2] >> 20);
		mac[5] = (r[2] >> 12);
		vid = r[0] & 0xfff;
		rvid = r[2] & 0xfff;

		if (!(r[0] >> 17)) /* Check for invalid entry */
			continue;

		if (port == ((r[0] >> 12) & 0x1f)) {
			pr_info("-> mac %pM, vid: %d, rvid: %d\n", &mac[0], vid, rvid);
			cb(mac, vid, (r[0] >> 19) & 1, data);
		}
	}

	for (i = 0; i < 64; i++) {
		rtl838x_read_cam(i, r);
		mac[0] = (r[1] >> 20);
		mac[1] = (r[1] >> 12);
		mac[2] = (r[1] >> 4);
		mac[3] = (r[1] & 0xf) << 4 | (r[2] >> 28);
		mac[4] = (r[2] >> 20);
		mac[5] = (r[2] >> 12);
		vid = r[0] & 0xfff;

		if (!(r[0] >> 17))
			continue;

		pr_info("Found in CAM: R1 %x R2 %x R3 %x\n", r[0], r[1], r[2]);
		if (port == ((r[0] >> 12) & priv->port_mask))
			cb(mac, vid, (r[0] >> 19) & 1, data);
	}

	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static int rtl838x_port_fdb_del(struct dsa_switch *ds, int port,
			   const unsigned char *addr, u16 vid)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(addr);
	u32 key = rtl838x_hash_key(priv, mac, vid);
	int i;
	u32 r[3];
	u64 entry;
	int idx = -1;
	int err = 0;

	pr_info("In %s, mac %llx, vid: %d, key: %x\n", __func__, mac, vid, key);
	mutex_lock(&priv->reg_mutex);
	for (i = 0; i < 4; i++) {
		entry = read_l2_entry_using_hash(key, i, r);
		if (!(r[0] >> 17)) /* Check for invalid entry */
			continue;
		if ((entry & 0x0fffffffffffffff) == ((mac << 12) | vid)) {
			idx = (key << 2) | i;
			break;
		}
	}

	if (idx >= 0) {
		r[0] = r[1] = r[2] = 0;
		rtl838x_write_hash(idx, r);
		goto out;
	}

	/* Check CAM for spillover from hash buckets */
	for (i = 0; i < 64; i++) {
		entry = rtl838x_read_cam(i, r);
		if ((entry & 0x0fffffffffffffff) == ((mac << 12) | vid)) {
			idx = i;
			break;
		}
	}
	if (idx >= 0) {
		r[0] = r[1] = r[2] = 0;
		rtl838x_write_cam(idx, r);
		goto out;
	}
	err = -ENOENT;
out:
	mutex_unlock(&priv->reg_mutex);
	return err;
}

static int rtl838x_port_fdb_add(struct dsa_switch *ds, int port,
				const unsigned char *addr, u16 vid)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(addr);
	u32 key = rtl838x_hash_key(priv, mac, vid);
	int i;
	u32 r[3];
	int idx = -1;
	u64 entry;
	int err = 0;

	mutex_lock(&priv->reg_mutex);
	for (i = 0; i < 4; i++) {
		entry = read_l2_entry_using_hash(key, i, r);
		if (!(r[0] >> 17)) { /* Check for invalid entry */
			idx = (key << 2) | i;
			break;
		}
		if ((entry & 0x0fffffffffffffff) == ((mac << 12) | vid)) {
			idx = (key << 2) | i;
			break;
		}
	}
	if (idx >= 0) {
		// Found for del: R1 60000 R2 901b0e9 R3 12b0e000, 901b0e912b0e000
		r[0] = 3 << 17 | port << 12; // Aging and  port
		r[0] |= vid;
		r[1] = mac >> 16;
		r[2] = (mac & 0xffff) << 12; /* rvid = 0 */
		rtl838x_write_hash(idx, r);
		goto out;
	}

	/* Hash bucket full, try CAM */
	for (i = 0; i < 64; i++) {
		entry = rtl838x_read_cam(i, r);
		if (!(r[0] >> 17)) { /* Check for invalid entry */
			if (idx < 0) /* First empty entry? */
				idx = i;
			break;
		} else if ((entry & 0x0fffffffffffffff) == ((mac << 12) | vid)) {
			pr_debug("Found entry in CAM\n");
			idx = i;
			break;
		}
	}
	if (idx >= 0) {
		r[0] = 3 << 17 | port << 12; // Aging
		r[0] |= vid;
		r[1] = mac >> 16;
		r[2] = (mac & 0xffff) << 12; /* rvid = 0 */
		rtl838x_write_cam(idx, r);
		goto out;
	}
	err = -ENOTSUPP;
out:
	mutex_unlock(&priv->reg_mutex);
	return err;
}

static void rtl838x_port_stp_state_set(struct dsa_switch *ds, int port,
				       u8 state)
{
	u32 cmd, msti = 0;
	u32 port_state[4];
	int index, bit, i;

	struct rtl838x_switch_priv *priv = ds->priv;
	int n = priv->family_id == RTL8380_FAMILY_ID ? 2 : 4;

	pr_info("%s: port %d state %2x\n", __func__, port, state);
	if (port >= priv->cpu_port)
		return;

	mutex_lock(&priv->reg_mutex);

	index = n - (port >> 4) - 1;
	bit = (port << 1) % 32;

	if (priv->family_id == RTL8380_FAMILY_ID) {
		cmd = 1 << 15 /* Execute cmd */
			| 1 << 14 /* Read */
			| 2 << 12 /* Table type 0b10 */
			| (msti & 0xfff);
	} else {
		cmd = 1 << 16 /* Execute cmd */
			| 0 << 15 /* Read */
			| 5 << 12 /* Table type 0b101 */
			| (msti & 0xfff);
	}
	priv->r->exec_tbl0_cmd(cmd);

	for (i = 0; i < n; i++)
		port_state[i] = sw_r32(priv->r->tbl_access_data_0(i));

	pr_debug("Current state, port %d: %d\n", port, (port_state[index] >> bit) & 3);
	port_state[index] &= ~(3 << bit);

	switch (state) {
	case BR_STATE_DISABLED: /* 0 */
		port_state[index] |= (0 << bit);
		break;
	case BR_STATE_BLOCKING:  /* 4 */
	case BR_STATE_LISTENING: /* 1 */
		port_state[index] |= (1 << bit);
		break;
	case BR_STATE_LEARNING: /* 2 */
		port_state[index] |= (2 << bit);
		break;
	case BR_STATE_FORWARDING: /* 3*/
		port_state[index] |= (3 << bit);
	default:
		break;
	}

	if (priv->family_id == RTL8380_FAMILY_ID) {
		cmd = 1 << 15 /* Execute cmd */
			| 0 << 14 /* Write */
			| 2 << 12 /* Table type 0b10 */
			| (msti & 0xfff);
	} else {
		cmd = 1 << 16 /* Execute cmd */
			| 1 << 15 /* Write */
			| 5 << 12 /* Table type 0b101 */
			| (msti & 0xfff);
	}
	for (i = 0; i < n; i++)
		sw_w32(port_state[i], priv->r->tbl_access_data_0(i));
	priv->r->exec_tbl0_cmd(cmd);

	mutex_unlock(&priv->reg_mutex);
}

static int rtl838x_port_mirror_add(struct dsa_switch *ds, int port,
				   struct dsa_mall_mirror_tc_entry *mirror,
				   bool ingress)
{
	/* We support 4 mirror groups, one destination port per group */
	int group;
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("In %s\n", __func__);

	for (group = 0; group < 4; group++) {
		if (priv->mirror_group_ports[group] == mirror->to_local_port)
			break;
	}
	if (group >= 4) {
		for (group = 0; group < 4; group++) {
			if (priv->mirror_group_ports[group] < 0)
				break;
		}
	}

	if (group >= 4)
		return -ENOSPC;

	pr_debug("Using group %d\n", group);
	mutex_lock(&priv->reg_mutex);
	/* Enable mirroring to port across VLANs (bit 11) */
	sw_w32(1 << 11 | (mirror->to_local_port << 4) | 1, RTL838X_MIR_CTRL(group));

	if (ingress && (sw_r32(RTL838X_MIR_SPM_CTRL(group)) & (1 << port))) {
		mutex_unlock(&priv->reg_mutex);
		return -EEXIST;
	}
	if ((!ingress) && (sw_r32(RTL838X_MIR_DPM_CTRL(group)) & (1 << port))) {
		mutex_unlock(&priv->reg_mutex);
		return -EEXIST;
	}
	if (ingress)
		sw_w32_mask(0, 1 << port, RTL838X_MIR_SPM_CTRL(group));
	else
		sw_w32_mask(0, 1 << port, RTL838X_MIR_DPM_CTRL(group));

	priv->mirror_group_ports[group] = mirror->to_local_port;
	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static void rtl838x_port_mirror_del(struct dsa_switch *ds, int port,
				    struct dsa_mall_mirror_tc_entry *mirror)
{
	int group = 0;
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("In %s\n", __func__);
	for (group = 0; group < 4; group++) {
		if (priv->mirror_group_ports[group] == mirror->to_local_port)
			break;
	}
	if (group >= 4)
		return;

	mutex_lock(&priv->reg_mutex);
	if (mirror->ingress) {
		/* Ingress, clear source port matrix */
		sw_w32_mask(1 << port, 0, RTL838X_MIR_SPM_CTRL(group));
	} else {
		/* Egress, clear destination port matrix */
		sw_w32_mask(1 << port, 0, RTL838X_MIR_DPM_CTRL(group));
	}

	if (!(sw_r32(RTL838X_MIR_DPM_CTRL(group)) || sw_r32(RTL838X_MIR_DPM_CTRL(group)))) {
		priv->mirror_group_ports[group] = -1;
		sw_w32(0, RTL838X_MIR_CTRL(group));
	}

	mutex_unlock(&priv->reg_mutex);
}


void rtl838x_vlan_profile_dump(int index)
{
	u32 profile;

	if (index < 0 || index > 7)
		return;

	profile = sw_r32(RTL838X_VLAN_PROFILE(index));

	pr_info("VLAN %d: L2 learning: %d, L2 Unknown MultiCast Field %x",
		index, profile & 1, (profile >> 1) & 0x1ff);
	pr_info("  IPv4 Unkn MultiCast Field %x, IPv6 Unkn MultiCast Field: %x",
		(profile >> 10) & 0x1ff, (profile >> 19) & 0x1ff);
}

static int rtl838x_vlan_filtering(struct dsa_switch *ds, int port,
				  bool vlan_filtering)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("%s: port %d\n", __func__, port);
	mutex_lock(&priv->reg_mutex);

	if (vlan_filtering) {
		/* Enable ingress and egress filtering */
		if (port != priv->cpu_port) {
			if (port < 16) {
				sw_w32_mask(0b10 << (port << 1),
					    0b01 << (port << 1),
					    RTL838X_VLAN_PORT_IGR_FLTR_0);
			} else {
				sw_w32_mask(0b10 << ((port - 16) << 1),
					    0b01 << ((port - 16) << 1),
					    RTL838X_VLAN_PORT_IGR_FLTR_1);
			}
		}
		sw_w32_mask(0, 1 << port, RTL838X_VLAN_PORT_EGR_FLTR);
	} else {
		/* Disable ingress and egress filtering */
		if (port != priv->cpu_port) {
			if (port < 16) {
				sw_w32_mask(0b11 << (port << 1),
					    0,
					    RTL838X_VLAN_PORT_IGR_FLTR_0);
			} else {
				sw_w32_mask(0b11 << ((port - 16) << 1),
					    0,
					    RTL838X_VLAN_PORT_IGR_FLTR_1);
			}
		}
		sw_w32_mask(1 << port, 0, RTL838X_VLAN_PORT_EGR_FLTR);
	}

	/* We need to do something to the CPU-Port, too */
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtl838x_vlan_prepare(struct dsa_switch *ds, int port,
				const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl838x_vlan_info info;
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("%s: port %d\n", __func__, port);

	mutex_lock(&priv->reg_mutex);

	priv->r->vlan_tables_read(1, &info);

	pr_info("Tagged ports %llx, untag %llx, prof %x, MC# %d, UC# %d, MSTI %x\n",
		info.tagged_ports, info.untagged_ports, info.vlan_conf & 7,
	       (info.vlan_conf & 8) >> 3, (info.vlan_conf & 16) >> 4,
	       (info.vlan_conf & 0x3e0) >> 5);

	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static void rtl838x_vlan_add(struct dsa_switch *ds, int port,
			    const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl838x_vlan_info info;
	struct rtl838x_switch_priv *priv = ds->priv;
	int v;
	u64 portmask;

	pr_info("%s port %d, vid_end %d, vid_end %d, flags %x\n", __func__,
	       port, vlan->vid_begin, vlan->vid_end, vlan->flags);

	if (vlan->vid_begin > 4095 || vlan->vid_end > 4095) {
		dev_err(priv->dev, "VLAN out of range: %d - %d",
			vlan->vid_begin, vlan->vid_end);
		return;
	}

	mutex_lock(&priv->reg_mutex);

	if (vlan->flags & BRIDGE_VLAN_INFO_PVID) {
		for (v = vlan->vid_begin; v <= vlan->vid_end; v++) {
			/* Set both inner and outer PVID of the port */
			sw_w32((v << 16) | v, RTL838X_VLAN_PORT_PB_VLAN(port));
		}
	}

	if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED) {
		for (v = vlan->vid_begin; v <= vlan->vid_end; v++) {
			/* Get untagged port memberships of this vlan */
			priv->r->vlan_tables_read(v, &info);
			portmask = info.untagged_ports | (1 << port);
			pr_debug("Untagged ports, VLAN %d: %llx\n", v, portmask);
			priv->r->vlan_set_untagged(v, portmask);
		}
	} else {
		for (v = vlan->vid_begin; v <= vlan->vid_end; v++) {
			/* Get tagged port memberships of this vlan */
			priv->r->vlan_tables_read(v, &info);
			portmask = info.tagged_ports | (1 << port);
			pr_debug("Tagged ports, VLAN %d: %llx\n", v, portmask);
			priv->r->vlan_set_tagged(v, portmask, info.vlan_conf);
		}
	}
	mutex_unlock(&priv->reg_mutex);
}

static int rtl838x_vlan_del(struct dsa_switch *ds, int port,
			    const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl838x_vlan_info info;
	struct rtl838x_switch_priv *priv = ds->priv;
	int v;
	u64 portmask;

	pr_info("%s: port %d, vid_end %d, vid_end %d, flags %x\n", __func__,
		port, vlan->vid_begin, vlan->vid_end, vlan->flags);

	if (vlan->vid_begin > 4095 || vlan->vid_end > 4095) {
		dev_err(priv->dev, "VLAN out of range: %d - %d",
			vlan->vid_begin, vlan->vid_end);
		return -ENOTSUPP;
	}

	mutex_lock(&priv->reg_mutex);

	for (v = vlan->vid_begin; v <= vlan->vid_end; v++) {
		/* Reset both inner and out PVID of the port */
		sw_w32(0, RTL838X_VLAN_PORT_PB_VLAN(port));

		if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED) {
			/* Get untagged port memberships of this vlan */
			priv->r->vlan_tables_read(v, &info);
			portmask = info.untagged_ports & (~(1 << port));
			pr_info("Untagged ports, VLAN %d: %llx\n", v, portmask);
			priv->r->vlan_set_untagged(v, portmask);
		}

		/* Get tagged port memberships of this vlan */
		priv->r->vlan_tables_read(v, &info);
		portmask = info.tagged_ports & (~(1 << port));
		pr_info("Tagged ports, VLAN %d: %llx\n", v, portmask);
		priv->r->vlan_set_tagged(v, portmask, info.vlan_conf);
	}
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void rtl838x_port_bridge_leave(struct dsa_switch *ds, int port,
				      struct net_device *bridge)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 port_bitmap = 1 << priv->cpu_port;
	int i;

	pr_info("%s %x: %d", __func__, (u32)priv, port);
	mutex_lock(&priv->reg_mutex);
	for (i = 0; i < ds->num_ports; i++) {
		/* Remove this port from the port matrix of the other ports
		 * in the same bridge. If the port is disabled, port matrix
		 * is kept and not being setup until the port becomes enabled.
		 * And the other port's port matrix cannot be broken when the
		 * other port is still a VLAN-aware port.
		 */
		if (dsa_is_user_port(ds, i) && i != port) {
			if (dsa_to_port(ds, i)->bridge_dev != bridge)
				continue;
			if (priv->ports[i].enable)
				priv->r->mask_port_iso_ctrl(1 << port, 0, i);
			priv->ports[i].pm |= 1 << port;

			port_bitmap &= ~(1 << i);
		}
	}

	/* Add all other ports to this port matrix. */
	if (priv->ports[port].enable)
		priv->r->mask_port_iso_ctrl(0, port_bitmap, port);
	priv->ports[port].pm &= ~port_bitmap;
	mutex_unlock(&priv->reg_mutex);

	rtl838x_print_matrix();
}

static int rtl838x_port_bridge_join(struct dsa_switch *ds, int port,
				      struct net_device *bridge)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 port_bitmap = 1 << priv->cpu_port;
	int i;

	pr_info("%s %x: %d %llx", __func__, (u32)priv, port, port_bitmap);
	mutex_lock(&priv->reg_mutex);
	for (i = 0; i < ds->num_ports; i++) {
		/* Add this port to the port matrix of the other ports in the
		 * same bridge. If the port is disabled, port matrix is kept
		 * and not being setup until the port becomes enabled.
		 */
		if (dsa_is_user_port(ds, i) && i != port) {
			if (dsa_to_port(ds, i)->bridge_dev != bridge)
				continue;
			if (priv->ports[i].enable)
				priv->r->mask_port_iso_ctrl(0, 1 << port, i);
			priv->ports[i].pm |= 1 << port;

			port_bitmap |= 1 << i;
		}
	}

	/* Add all other ports to this port matrix. */
	if (priv->ports[port].enable) {
		priv->r->mask_port_iso_ctrl(0, 1 << port, priv->cpu_port);
		priv->r->mask_port_iso_ctrl(0, port_bitmap, port);
	}
	priv->ports[port].pm |= port_bitmap;
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtl838x_port_enable(struct dsa_switch *ds, int port,
				struct phy_device *phydev)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("%s: %x %d", __func__, (u32) priv, port);
	priv->ports[port].enable = true;

	if (dsa_is_cpu_port(ds, port))
		return 0;

	/* add port to switch mask of CPU_PORT */
	priv->r->mask_port_iso_ctrl(0, 1 << port, priv->cpu_port);

	/* add all other ports in the same bridge to switch mask of port */
	priv->r->mask_port_iso_ctrl(0, priv->ports[port].pm, port);

	/* enable PHY polling */
	sw_w32_mask(0, 1 << port, RTL838X_SMI_POLL_CTRL);

	return 0;
}

static void rtl838x_port_disable(struct dsa_switch *ds, int port)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("%s %x: %d", __func__, (u32)priv, port);

	/* you can only disable user ports */
	if (!dsa_is_user_port(ds, port))
		return;

	/* remove port from switch mask of CPU_PORT */
	priv->r->mask_port_iso_ctrl(1 << port, 0, priv->cpu_port);

	/* remove all other ports in the same bridge from switch mask of port */
	priv->r->mask_port_iso_ctrl(priv->ports[port].pm, 0, port);

	priv->ports[port].enable = false;

	/* disable PHY polling */
	sw_w32_mask(1 << port, 0, RTL838X_SMI_POLL_CTRL);
}

static int rtl838x_get_mac_eee(struct dsa_switch *ds, int port,
			       struct ethtool_eee *e)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("%s: port %d", __func__, port);
	e->supported = SUPPORTED_100baseT_Full | SUPPORTED_1000baseT_Full;
	if (sw_r32(priv->r->mac_force_mode_ctrl(port)) & (1 << 9))
		e->advertised |= ADVERTISED_100baseT_Full;

	if (sw_r32(priv->r->mac_force_mode_ctrl(port)) & (1 << 10))
		e->advertised |= ADVERTISED_1000baseT_Full;

	e->eee_enabled = priv->ports[port].eee_enabled;
	pr_info("enabled: %d, active %x\n", e->eee_enabled, e->advertised);

	if (sw_r32(RTL838X_MAC_EEE_ABLTY) & (1 << port)) {
		e->lp_advertised = ADVERTISED_100baseT_Full;
		e->lp_advertised |= ADVERTISED_1000baseT_Full;
	}

	e->eee_active = !!(e->advertised & e->lp_advertised);
	pr_info("active: %d, lp %x\n", e->eee_active, e->lp_advertised);

	return 0;
}

static int rtl838x_set_mac_eee(struct dsa_switch *ds, int port,
			       struct ethtool_eee *e)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_info("%s: port %d", __func__, port);
	if (e->eee_enabled) {
		pr_info("Globally enabling EEE\n");
		sw_w32_mask(0x4, 0, RTL838X_SMI_GLB_CTRL);
	}
	if (e->eee_enabled) {
		pr_info("Enabling EEE for MAC %d\n", port);
		sw_w32_mask(0, 3 << 9, priv->r->mac_force_mode_ctrl(port));
		sw_w32_mask(0, 1 << port, RTL838X_EEE_PORT_TX_EN);
		sw_w32_mask(0, 1 << port, RTL838X_EEE_PORT_RX_EN);
		priv->ports[port].eee_enabled = true;
		e->eee_enabled = true;
	} else {
		pr_info("Disabling EEE for MAC %d\n", port);
		sw_w32_mask(3 << 9, 0, priv->r->mac_force_mode_ctrl(port));
		sw_w32_mask(1 << port, 0, RTL838X_EEE_PORT_TX_EN);
		sw_w32_mask(1 << port, 0, RTL838X_EEE_PORT_RX_EN);
		priv->ports[port].eee_enabled = false;
		e->eee_enabled = false;
	}
	return 0;
}

static void rtl838x_phylink_mac_config(struct dsa_switch *ds, int port,
				      unsigned int mode,
				      const struct phylink_link_state *state)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u32 reg;

	pr_info("%s port %d, mode %x\n", __func__, port, mode);

	if (port == priv->cpu_port) {
		/* Set Speed, duplex, flow control
		 * FORCE_EN | LINK_EN | NWAY_EN | DUP_SEL
		 * | SPD_SEL = 0b10 | FORCE_FC_EN | PHY_MASTER_SLV_MANUAL_EN
		 * | MEDIA_SEL
		 */
		if (priv->family_id == RTL8380_FAMILY_ID) {
			sw_w32(0x6192F, priv->r->mac_force_mode_ctrl(priv->cpu_port));
			/* allow CRC errors on CPU-port */
			sw_w32_mask(0, 0x8, RTL838X_MAC_PORT_CTRL(priv->cpu_port));
		} else {
			sw_w32_mask(0, 3, priv->r->mac_force_mode_ctrl(priv->cpu_port));
		}
		return;
	}

	reg = sw_r32(priv->r->mac_force_mode_ctrl(port));
	if (mode == MLO_AN_PHY) {
		pr_info("PHY autonegotiates\n");
		reg |= 1 << 2;
		sw_w32(reg, priv->r->mac_force_mode_ctrl(port));
		return;
	}

	if (mode != MLO_AN_FIXED)
		pr_info("Not fixed\n");

	/* Clear id_mode_dis bit, and the existing port mode, let
	 * RGMII_MODE_EN bet set by mac_link_{up,down}
	 */
	reg &= ~(RX_PAUSE_EN | TX_PAUSE_EN);

	if (state->pause & MLO_PAUSE_TXRX_MASK) {
		if (state->pause & MLO_PAUSE_TX)
			reg |= TX_PAUSE_EN;
		reg |= RX_PAUSE_EN;
	}

	reg &= ~(3 << 4);
	switch (state->speed) {
	case SPEED_1000:
		reg |= 2 << 4;
		break;
	case SPEED_100:
		reg |= 1 << 4;
		break;
	}

	reg &= ~(DUPLEX_FULL | FORCE_LINK_EN);
	if (state->link)
		reg |= FORCE_LINK_EN;
	if (state->duplex == DUPLEX_FULL)
		reg |= DUPLX_MODE;

	// Disable AN
	reg &= ~(1 << 2);
	sw_w32(reg, priv->r->mac_force_mode_ctrl(port));
}

static void rtl838x_phylink_mac_link_down(struct dsa_switch *ds, int port,
				     unsigned int mode,
				     phy_interface_t interface)
{
	/* Stop TX/RX to port */
	sw_w32_mask(0x03, 0, RTL838X_MAC_PORT_CTRL(port));
}

static void rtl838x_phylink_mac_link_up(struct dsa_switch *ds, int port,
				   unsigned int mode,
				   phy_interface_t interface,
				   struct phy_device *phydev)
{
	/* Restart TX/RX to port */
	sw_w32_mask(0, 0x03, RTL838X_MAC_PORT_CTRL(port));
}

static void rtl838x_phylink_validate(struct dsa_switch *ds, int port,
				     unsigned long *supported,
				     struct phylink_link_state *state)
{
	__ETHTOOL_DECLARE_LINK_MODE_MASK(mask) = { 0, };

	pr_info("In %s port %d", __func__, port);

	if (!phy_interface_mode_is_rgmii(state->interface) &&
	    state->interface != PHY_INTERFACE_MODE_1000BASEX &&
	    state->interface != PHY_INTERFACE_MODE_MII &&
	    state->interface != PHY_INTERFACE_MODE_REVMII &&
	    state->interface != PHY_INTERFACE_MODE_GMII &&
	    state->interface != PHY_INTERFACE_MODE_QSGMII &&
	    state->interface != PHY_INTERFACE_MODE_INTERNAL &&
	    state->interface != PHY_INTERFACE_MODE_SGMII) {
		bitmap_zero(supported, __ETHTOOL_LINK_MODE_MASK_NBITS);
		dev_err(ds->dev,
			"Unsupported interface: %d for port %d\n",
			state->interface, port);
		return;
	}

	/* switch chip-id? if (priv->id == 0x8382) */

	/* Allow all the expected bits */
	phylink_set(mask, Autoneg);
	phylink_set_port_modes(mask);
	phylink_set(mask, Pause);
	phylink_set(mask, Asym_Pause);

	/* With the exclusion of MII and Reverse MII, we support Gigabit,
	 * including Half duplex
	 */
	if (state->interface != PHY_INTERFACE_MODE_MII &&
	    state->interface != PHY_INTERFACE_MODE_REVMII) {
		phylink_set(mask, 1000baseT_Full);
		phylink_set(mask, 1000baseT_Half);
	}

	/* On both the 8380 and 8382, ports 24-27 are SFP ports */
	if (port >= 24 && port <= 27)
		phylink_set(mask, 1000baseX_Full);

	phylink_set(mask, 10baseT_Half);
	phylink_set(mask, 10baseT_Full);
	phylink_set(mask, 100baseT_Half);
	phylink_set(mask, 100baseT_Full);

	bitmap_and(supported, supported, mask,
		   __ETHTOOL_LINK_MODE_MASK_NBITS);
	bitmap_and(state->advertising, state->advertising, mask,
		   __ETHTOOL_LINK_MODE_MASK_NBITS);
}

static int rtl838x_phylink_mac_link_state(struct dsa_switch *ds, int port,
					  struct phylink_link_state *state)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u32 speed;

	if (port < 0 || port > priv->cpu_port)
		return -EINVAL;

	state->link = 0;
	if (sw_r32(RTL838X_MAC_LINK_STS) & (1 << port))
		state->link = 1;
	state->duplex = 0;
	if (sw_r32(RTL838X_MAC_LINK_DUP_STS) & (1 << port))
		state->duplex = 1;

	speed = sw_r32(RTL838X_MAC_LINK_SPD_STS(port));
	speed >>= (port % 16) << 1;
	switch (speed & 0x3) {
	case 0:
		state->speed = SPEED_10;
		break;
	case 1:
		state->speed = SPEED_100;
		break;
	case 2:
		state->speed = SPEED_1000;
		break;
	case 3:
		if (port == 24 || port == 26) /* Internal serdes */
			state->speed = SPEED_2500;
		else
			state->speed = SPEED_100; /* Is in fact 500Mbit */
	}

	state->pause &= (MLO_PAUSE_RX | MLO_PAUSE_TX);
	if (sw_r32(RTL838X_MAC_RX_PAUSE_STS) & (1 << port))
		state->pause |= MLO_PAUSE_RX;
	if (sw_r32(RTL838X_MAC_TX_PAUSE_STS) & (1 << port))
		state->pause |= MLO_PAUSE_TX;
	return 1;
}

static int rtl838x_mdio_probe(struct rtl838x_switch_priv *priv)
{
	struct device *dev = priv->dev;
	struct device_node *dn, *mii_np = dev->of_node;
	struct mii_bus *bus;
	int ret;
	u32 pn;

	pr_info("In %s\n", __func__);
	mii_np = of_find_compatible_node(NULL, NULL, "realtek,rtl838x-mdio");
	if (mii_np) {
		pr_info("Found compatible MDIO node!\n");
	} else {
		dev_err(priv->dev, "no %s child node found", "mdio-bus");
		return -ENODEV;
	}

	priv->mii_bus = of_mdio_find_bus(mii_np);
	if (!priv->mii_bus) {
		pr_info("Deferring probe of mdio bus\n");
		return -EPROBE_DEFER;
	}
	if (!of_device_is_available(mii_np))
		ret = -ENODEV;

	bus = devm_mdiobus_alloc(priv->ds->dev);
	if (!bus)
		return -ENOMEM;

	bus->name = "rtl838x slave mii";
	bus->read = &rtl838x_mdio_read;
	bus->write = &rtl838x_mdio_write;
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

		// Check for the integrated SerDes of the RTL8380M first
		if (of_property_read_bool(dn, "phy-is-integrated")
			&& priv->id == 0x8380 && pn >= 24) {
			pr_info("----> FÓUND A SERDES\n");
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
	sw_w32(0x00000000, RTL838X_SMI_POLL_CTRL);

	/* Enable PHY control via SoC */
	sw_w32_mask(0, 1 << 15, RTL838X_SMI_GLB_CTRL);
	/* Power on fibre ports and reset them if necessary */
	if (priv->ports[24].phy == PHY_RTL838X_SDS) {
		pr_info("Powering on fibre ports & reset\n");
		rtl8380_sds_power(24, 1);
		rtl8380_sds_power(26, 1);
	}

	pr_info("%s done\n", __func__);
	return 0;
}

static const struct dsa_switch_ops rtl838x_switch_ops = {
	.get_tag_protocol	= rtl838x_get_tag_protocol,
	.setup			= rtl838x_setup,
	.port_vlan_filtering	= rtl838x_vlan_filtering,
	.port_vlan_prepare	= rtl838x_vlan_prepare,
	.port_vlan_add		= rtl838x_vlan_add,
	.port_vlan_del		= rtl838x_vlan_del,
	.port_bridge_join	= rtl838x_port_bridge_join,
	.port_bridge_leave	= rtl838x_port_bridge_leave,
	.port_stp_state_set	= rtl838x_port_stp_state_set,
	.set_ageing_time	= rtl838x_set_l2aging,
	.port_fast_age		= rtl838x_fast_age,
	.port_fdb_add		= rtl838x_port_fdb_add,
	.port_fdb_del		= rtl838x_port_fdb_del,
	.port_fdb_dump		= rtl838x_port_fdb_dump,
	.port_enable		= rtl838x_port_enable,
	.port_disable		= rtl838x_port_disable,
	.port_mirror_add	= rtl838x_port_mirror_add,
	.port_mirror_del	= rtl838x_port_mirror_del,
	.phy_read		= dsa_phy_read,
	.phy_write		= dsa_phy_write,
	.get_strings		= rtl838x_get_strings,
	.get_ethtool_stats	= rtl838x_get_ethtool_stats,
	.get_sset_count		= rtl838x_get_sset_count,
	.phylink_validate	= rtl838x_phylink_validate,
	.phylink_mac_link_state	= rtl838x_phylink_mac_link_state,
	.phylink_mac_config	= rtl838x_phylink_mac_config,
	.phylink_mac_link_down	= rtl838x_phylink_mac_link_down,
	.phylink_mac_link_up	= rtl838x_phylink_mac_link_up,
	.set_mac_eee		= rtl838x_set_mac_eee,
	.get_mac_eee		= rtl838x_get_mac_eee,

};

static int __init rtl838x_sw_probe(struct platform_device *pdev)
{
	int err = 0, i;
	struct rtl838x_switch_priv *priv;
	struct device *dev = &pdev->dev;

	pr_info("Probing RTL838X switch device\n");
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
	priv->ds->ops = &rtl838x_switch_ops;
	priv->dev = dev;

	priv->family_id = soc_info.family;
	priv->id = soc_info.id;
	if (soc_info.family == RTL8380_FAMILY_ID) {
		priv->cpu_port = RTL838X_CPU_PORT;
		priv->port_mask = 0x1f;
		priv->r = &rtl838x_reg;
		priv->ds->num_ports = 30;
		rtl8380_get_version(priv);
	} else {
		priv->cpu_port = RTL839X_CPU_PORT;
		priv->port_mask = 0x3f;
		priv->r = &rtl839x_reg;
		priv->ds->num_ports = 53;
		rtl8390_get_version(priv);
	}
	pr_info("Chip version %c\n", priv->version);

	err = rtl838x_mdio_probe(priv);
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
	priv->r->set_port_reg(0xffffffffffffffff, priv->r->isr_port_link_sts_chg);
	priv->r->set_port_reg(0xffffffffffffffff, priv->r->imr_port_link_sts_chg);

	priv->link_state_irq = 20;
	if (priv->family_id == RTL8380_FAMILY_ID) {
		err = request_irq(priv->link_state_irq, rtl838x_switch_irq,
				IRQF_SHARED, "rtl8838x-link-state", priv->ds);
	} else {
		err = request_irq(priv->link_state_irq, rtl839x_switch_irq,
				IRQF_SHARED, "rtl8838x-link-state", priv->ds);
	}
	if (err) {
		dev_err(dev, "Error setting up switch interrupt.\n");
		/* Need to free allocated switch here */
	}

	/* Enable interrupts for switch */
	sw_w32(0x1, priv->r->imr_glb);

	rtl838x_get_l2aging(priv);

	/* Clear all destination ports for mirror groups */
	for (i = 0; i < 4; i++)
		priv->mirror_group_ports[i] = -1;

	return err;
}

static int rtl838x_sw_remove(struct platform_device *pdev)
{
	pr_info("Removing platform driver for rtl838x-sw\n");
	return 0;
}

static const struct of_device_id rtl838x_switch_of_ids[] = {
	{ .compatible = "realtek,rtl838x-switch"},
	{ /* sentinel */ }
};


MODULE_DEVICE_TABLE(of, rtl838x_switch_of_ids);

static struct platform_driver rtl838x_switch_driver = {
	.probe = rtl838x_sw_probe,
	.remove = rtl838x_sw_remove,
	.driver = {
		.name = "rtl838x-switch",
		.pm = NULL,
		.of_match_table = rtl838x_switch_of_ids,
	},
};

module_platform_driver(rtl838x_switch_driver);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL838X SoC Switch Driver");
MODULE_LICENSE("GPL");
