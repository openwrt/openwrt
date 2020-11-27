/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _RTL838X_H
#define _RTL838X_H

#include <net/dsa.h>

/*
 * Register definition
 */
#define RTL838X_CPU_PORT			28
#define RTL839X_CPU_PORT			52

#define RTL838X_MAC_PORT_CTRL(port)		(0xd560 + (((port) << 7)))
#define RTL839X_MAC_PORT_CTRL(port)		(0x8004 + (((port) << 7)))
#define RTL838X_RST_GLB_CTRL_0			(0x003c)
#define RTL838X_MAC_FORCE_MODE_CTRL		(0xa104)
#define RTL839X_MAC_FORCE_MODE_CTRL		(0x02bc)

#define RTL838X_DMY_REG31			(0x3b28)
#define RTL838X_SDS_MODE_SEL			(0x0028)
#define RTL838X_SDS_CFG_REG			(0x0034)
#define RTL838X_INT_MODE_CTRL			(0x005c)
#define RTL838X_CHIP_INFO			(0x00d8)
#define RTL839X_CHIP_INFO			(0x0ff4)
#define RTL838X_SDS4_REG28			(0xef80)
#define RTL838X_SDS4_DUMMY0			(0xef8c)
#define RTL838X_SDS5_EXT_REG6			(0xf18c)
#define RTL838X_PORT_ISO_CTRL(port)		(0x4100 + ((port) << 2))
#define RTL839X_PORT_ISO_CTRL(port)		(0x1400 + ((port) << 3))
#define RTL8380_SDS4_FIB_REG0			(0xF800)
#define RTL838X_STAT_PORT_STD_MIB		(0x1200)
#define RTL839X_STAT_PORT_STD_MIB		(0xC000)
#define RTL838X_STAT_RST			(0x3100)
#define RTL839X_STAT_RST			(0xF504)
#define RTL838X_STAT_PORT_RST			(0x3104)
#define RTL839X_STAT_PORT_RST			(0xF508)
#define RTL838X_STAT_CTRL			(0x3108)
#define RTL839X_STAT_CTRL			(0x04cc)

/* Registers of the internal Serdes of the 8390 */
#define RTL8390_SDS0_1_XSG0			(0xA000)
#define RTL8390_SDS0_1_XSG1			(0xA100)
#define RTL839X_SDS12_13_XSG0			(0xB800)
#define RTL839X_SDS12_13_XSG1			(0xB900)
#define RTL839X_SDS12_13_PWR0			(0xb880)
#define RTL839X_SDS12_13_PWR1			(0xb980)

/* Registers of the internal Serdes of the 8380 */
#define MAPLE_SDS4_REG0r			RTL838X_SDS4_REG28
#define MAPLE_SDS5_REG0r			(RTL838X_SDS4_REG28 + 0x100)
#define MAPLE_SDS4_REG3r			RTL838X_SDS4_DUMMY0
#define MAPLE_SDS5_REG3r			(RTL838X_SDS4_REG28 + 0x100)
#define MAPLE_SDS4_FIB_REG0r			(RTL838X_SDS4_REG28 + 0x880)
#define MAPLE_SDS5_FIB_REG0r			(RTL838X_SDS4_REG28 + 0x980)

/* VLAN registers */
#define RTL838X_VLAN_PROFILE(idx)		(0x3A88 + ((idx) << 2))
#define RTL838X_VLAN_PORT_EGR_FLTR		(0x3A84)
#define RTL838X_VLAN_PORT_PB_VLAN(port)		(0x3C00 + ((port) << 2))
#define RTL838X_VLAN_PORT_IGR_FLTR(port)	(0x3A7C + (((port >> 4) << 2)))
#define RTL838X_VLAN_PORT_IGR_FLTR_0		(0x3A7C)
#define RTL838X_VLAN_PORT_IGR_FLTR_1		(0x3A7C + 4)
#define RTL838X_VLAN_PORT_TAG_STS_CTRL(port)	(0xA530 + (((port) << 2)))
#define RTL839X_VLAN_PROFILE(idx)		(0x25C0 + (((idx) << 3)))
#define RTL839X_VLAN_CTRL			(0x26D4)
#define RTL839X_VLAN_PORT_PB_VLAN(port)		(0x26D8 + (((port) << 2)))
#define RTL839X_VLAN_PORT_IGR_FLTR(port)	(0x27B4 + (((port >> 4) << 2)))
#define RTL839X_VLAN_PORT_EGR_FLTR(port)	(0x27C4 + (((port >> 5) << 2)))
#define RTL839X_VLAN_PORT_TAG_STS_CTRL(port)	(0x6828 + (((port) << 2)))

/* Table 0/1 access registers */
#define RTL838X_TBL_ACCESS_CTRL_0		(0x6914)
#define RTL838X_TBL_ACCESS_DATA_0(idx)		(0x6918 + ((idx) << 2))
#define RTL838X_TBL_ACCESS_CTRL_1		(0xA4C8)
#define RTL838X_TBL_ACCESS_DATA_1(idx)		(0xA4CC + ((idx) << 2))
#define RTL839X_TBL_ACCESS_CTRL_0		(0x1190)
#define RTL839X_TBL_ACCESS_DATA_0(idx)		(0x1194 + ((idx) << 2))
#define RTL839X_TBL_ACCESS_CTRL_1		(0x6b80)
#define RTL839X_TBL_ACCESS_DATA_1(idx)		(0x6b84 + ((idx) << 2))

/* MAC handling */
#define RTL838X_MAC_LINK_STS			(0xa188)
#define RTL839X_MAC_LINK_STS			(0x0390)
#define RTL838X_MAC_LINK_SPD_STS(port)		(0xa190 + (((port >> 4) << 2)))
#define RTL839X_MAC_LINK_SPD_STS(port)		(0x03a0 + (((port >> 4) << 2)))
#define RTL838X_MAC_LINK_DUP_STS		(0xa19c)
#define RTL839X_MAC_LINK_DUP_STS		(0x03b0)
#define RTL838X_MAC_TX_PAUSE_STS		(0xa1a0)
#define RTL839X_MAC_TX_PAUSE_STS		(0x03b8)
#define RTL838X_MAC_RX_PAUSE_STS		(0xa1a4)
#define RTL839X_MAC_RX_PAUSE_STS		(0x03c0)
#define RTL838X_EEE_TX_TIMER_GIGA_CTRL		(0xaa04)
#define RTL838X_EEE_TX_TIMER_GELITE_CTRL	(0xaa08)

#define RTL838X_DMA_IF_CTRL			(0x9f58)

/* MAC link state bits */
#define FORCE_EN				(1 << 0)
#define FORCE_LINK_EN				(1 << 1)
#define NWAY_EN					(1 << 2)
#define DUPLX_MODE				(1 << 3)
#define TX_PAUSE_EN				(1 << 6)
#define RX_PAUSE_EN				(1 << 7)

/* EEE */
#define RTL838X_MAC_EEE_ABLTY			(0xa1a8)
#define RTL838X_EEE_PORT_TX_EN			(0x014c)
#define RTL838X_EEE_PORT_RX_EN			(0x0150)
#define RTL838X_EEE_CLK_STOP_CTRL		(0x0148)

/* L2 functionality */
#define RTL838X_L2_CTRL_0			(0x3200)
#define RTL839X_L2_CTRL_0			(0x3800)
#define RTL838X_L2_CTRL_1			(0x3204)
#define RTL839X_L2_CTRL_1			(0x3804)
#define RTL838X_L2_PORT_AGING_OUT		(0x3358)
#define RTL839X_L2_PORT_AGING_OUT		(0x3b74)
#define RTL838X_TBL_ACCESS_L2_CTRL		(0x6900)
#define RTL839X_TBL_ACCESS_L2_CTRL		(0x1180)
#define RTL838X_TBL_ACCESS_L2_DATA(idx)		(0x6908 + ((idx) << 2))
#define RTL839X_TBL_ACCESS_L2_DATA(idx)		(0x1184 + ((idx) << 2))
#define RTL838X_L2_TBL_FLUSH_CTRL		(0x3370)
#define RTL839X_L2_TBL_FLUSH_CTRL		(0x3ba0)
#define RTL838X_L2_PORT_NEW_SALRN(p)		(0x328c + (((p >> 4) << 2)))
#define RTL839X_L2_PORT_NEW_SALRN(p)		(0x38F0 + (((p >> 4) << 2)))
#define RTL838X_L2_PORT_NEW_SA_FWD(p)		(0x3294 + (((p >> 4) << 2)))
#define RTL839X_L2_PORT_NEW_SA_FWD(p)		(0x3900 + (((p >> 4) << 2)))
#define RTL838X_L2_PORT_SALRN(p)		(0x328c + (((p >> 4) << 2)))
#define RTL839X_L2_PORT_SALRN(p)		(0x38F0 + (((p >> 4) << 2)))

/* Port Mirroring */
#define RTL838X_MIR_CTRL(grp)			(0x5D00 + (((grp) << 2)))
#define RTL838X_MIR_DPM_CTRL(grp)		(0x5D20 + (((grp) << 2)))
#define RTL838X_MIR_SPM_CTRL(grp)		(0x5D10 + (((grp) << 2)))
#define RTL839X_MIR_CTRL(grp)			(0x2500 + (((grp) << 2)))
#define RTL839X_MIR_DPM_CTRL(grp)		(0x2530 + (((grp) << 2)))
#define RTL839X_MIR_SPM_CTRL(grp)		(0x2510 + (((grp) << 2)))

/* Storm control */
#define RTL838X_STORM_CTRL			(0x4700)
#define RTL839X_STORM_CTRL			(0x1800)
#define RTL838X_STORM_CTRL_LB_CTRL(p)		(0x4884 + (((p) << 2)))
#define RTL838X_STORM_CTRL_BURST_PPS_0		(0x4874)
#define RTL838X_STORM_CTRL_BURST_PPS_1		(0x4878)
#define RTL838X_STORM_CTRL_BURST_0		(0x487c)
#define RTL838X_STORM_CTRL_BURST_1		(0x4880)
#define RTL838X_SCHED_CTRL			(0xB980)
#define RTL838X_SCHED_LB_TICK_TKN_CTRL_0	(0xAD58)
#define RTL838X_SCHED_LB_TICK_TKN_CTRL_1	(0xAD5C)
#define RTL839X_SCHED_LB_TICK_TKN_CTRL_0	(0x1804)
#define RTL839X_SCHED_LB_TICK_TKN_CTRL_1	(0x1808)
#define RTL838X_SCHED_LB_THR			(0xB984)
#define RTL838X_STORM_CTRL_PORT_BC_EXCEED	(0x470C)
#define RTL838X_STORM_CTRL_PORT_MC_EXCEED	(0x4710)
#define RTL838X_STORM_CTRL_PORT_UC_EXCEED	(0x4714)
#define RTL839X_STORM_CTRL_PORT_BC_EXCEED(p)	(0x180c + (((p >> 5) << 2)))
#define RTL839X_STORM_CTRL_PORT_MC_EXCEED(p)	(0x1814 + (((p >> 5) << 2)))
#define RTL839X_STORM_CTRL_PORT_UC_EXCEED(p)	(0x181c + (((p >> 5) << 2)))
#define RTL838X_STORM_CTRL_PORT_UC(p)		(0x4718 + (((p) << 2)))
#define RTL838X_STORM_CTRL_PORT_MC(p)		(0x478c + (((p) << 2)))
#define RTL838X_STORM_CTRL_PORT_BC(p)		(0x4800 + (((p) << 2)))

/* Attack prevention */
#define RTL838X_ATK_PRVNT_PORT_EN		(0x5B00)
#define RTL838X_ATK_PRVNT_CTRL			(0x5B04)
#define RTL838X_ATK_PRVNT_ACT			(0x5B08)
#define RTL838X_ATK_PRVNT_STS			(0x5B1C)

enum phy_type {
	PHY_NONE = 0,
	PHY_RTL838X_SDS = 1,
	PHY_RTL8218B_INT = 2,
	PHY_RTL8218B_EXT = 3,
	PHY_RTL8214FC = 4,
	PHY_RTL839X_SDS = 5,
};

struct rtl838x_port {
	bool enable;
	u64 pm;
	u16 pvid;
	bool eee_enabled;
	enum phy_type phy;
	const struct dsa_port *dp;
};

struct rtl838x_vlan_info {
	u64 untagged_ports;
	u64 tagged_ports;
	u8 profile_id;
	bool hash_mc_fid;
	bool hash_uc_fid;
	u8 fid;
};

enum l2_entry_type {
	L2_INVALID = 0,
	L2_UNICAST = 1,
	L2_MULTICAST = 2,
	IP4_MULTICAST = 3,
	IP6_MULTICAST = 4,
};

struct rtl838x_l2_entry {
	u8 mac[6];
	u16 vid;
	u16 rvid;
	u8 port;
	bool valid;
	enum l2_entry_type type;
	bool is_static;
	bool is_ip_mc;
	bool is_ipv6_mc;
	bool block_da;
	bool block_sa;
	bool suspended;
	bool next_hop;
	int age;
	u16 mc_portmask_index;
};

struct rtl838x_switch_priv;

struct rtl838x_reg {
	void (*mask_port_reg_be)(u64 clear, u64 set, int reg);
	void (*set_port_reg_be)(u64 set, int reg);
	u64 (*get_port_reg_be)(int reg);
	void (*mask_port_reg_le)(u64 clear, u64 set, int reg);
	void (*set_port_reg_le)(u64 set, int reg);
	u64 (*get_port_reg_le)(int reg);
	int stat_port_rst;
	int stat_rst;
	int (*stat_port_std_mib)(int p);
	int (*port_iso_ctrl)(int p);
	int l2_ctrl_0;
	int l2_ctrl_1;
	int l2_port_aging_out;
	int smi_poll_ctrl;
	int l2_tbl_flush_ctrl;
	void (*exec_tbl0_cmd)(u32 cmd);
	void (*exec_tbl1_cmd)(u32 cmd);
	int (*tbl_access_data_0)(int i);
	int isr_glb_src;
	int isr_port_link_sts_chg;
	int imr_port_link_sts_chg;
	int imr_glb;
	void (*vlan_tables_read)(u32 vlan, struct rtl838x_vlan_info *info);
	void (*vlan_set_tagged)(u32 vlan, struct rtl838x_vlan_info *info);
	void (*vlan_set_untagged)(u32 vlan, u64 portmask);
	int  (*mac_force_mode_ctrl)(int port);
	int  (*mac_port_ctrl)(int port);
	int  (*l2_port_new_salrn)(int port);
	int  (*l2_port_new_sa_fwd)(int port);
	int  (*mir_ctrl)(int group);
	int  (*mir_dpm)(int group);
	int  (*mir_spm)(int group);
	int mac_link_sts;
	int mac_link_dup_sts;
	int  (*mac_link_spd_sts)(int port);
	int mac_rx_pause_sts;
	int mac_tx_pause_sts;
	u64 (*read_l2_entry_using_hash)(u32 hash, u32 position, struct rtl838x_l2_entry *e);
	u64 (*read_cam)(int idx, struct rtl838x_l2_entry *e);
	int (*vlan_profile)(int profile);
	int (*vlan_port_egr_filter)(int port);
	int (*vlan_port_igr_filter)(int port);
	int (*vlan_port_pb)(int port);
	int (*vlan_port_tag_sts_ctrl)(int port);
};

struct rtl838x_switch_priv {
	/* Switch operation */
	struct dsa_switch *ds;
	struct device *dev;
	u16 id;
	u16 family_id;
	char version;
	struct rtl838x_port ports[54]; /* TODO: correct size! */
	struct mutex reg_mutex;
	int link_state_irq;
	int mirror_group_ports[4];
	struct mii_bus *mii_bus;
	const struct rtl838x_reg *r;
	u8 cpu_port;
	u8 port_mask;
	u32 fib_entries;
	struct dentry *dbgfs_dir;
};

void rtl838x_dbgfs_init(struct rtl838x_switch_priv *priv);

#endif /* _RTL838X_H */
