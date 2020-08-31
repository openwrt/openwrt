//  SPDX-License-Identifier: GPL-2.0-only

#ifndef _RTL838X_H
#define _RTL838X_H

#include <net/dsa.h>

#define RTL838X_SW_BASE ((volatile void *)0xBB000000)

#define sw_r32(reg)	__raw_readl(reg)
#define sw_w32(val, reg)	__raw_writel(val, reg)
#define sw_w32_mask(clear, set, reg)	\
	sw_w32((sw_r32(reg) & ~(clear)) | (set), reg)

#define sw_r64(reg)		((((u64)__raw_readl(reg)) << 32) | \
				__raw_readl(reg + 4))
#define sw_w64(val, reg)	__raw_writel((u32)((val) >> 32), reg); \
				__raw_writel((u32)((val) & 0xffffffff), reg + 4)
#define sw_w64_mask(clear, set, reg)	\
	sw_w32((sw_r32(reg) & ~(((clear) >> 32))) | ((set) >> 32), reg); \
	sw_w32((sw_r32((reg) + 4) & ~((clear) & 0xffffffff)) | ((set) & 0xffffffff), (reg) + 4);

/*
 * Register definition
 */
 
#define RTL838X_CPU_PORT			28
#define RTL839X_CPU_PORT			52

#define RTL838X_MAC_PORT_CTRL(port)		(RTL838X_SW_BASE + 0xd560 + (((port) << 7)))
#define RTL839X_MAC_PORT_CTRL(port)		(RTL838X_SW_BASE + 0x8004 + (((port) << 7)))
#define RTL838X_RST_GLB_CTRL_0			(RTL838X_SW_BASE + 0x3c)
#define RTL838X_MAC_FORCE_MODE_CTRL		(RTL838X_SW_BASE + 0xa104)
#define RTL839X_MAC_FORCE_MODE_CTRL		(RTL838X_SW_BASE + 0x02bc)

#define RTL838X_DMY_REG31			(RTL838X_SW_BASE + 0x3b28)
#define RTL838X_SDS_MODE_SEL			(RTL838X_SW_BASE + 0x28)
#define RTL838X_SDS_CFG_REG			(RTL838X_SW_BASE + 0x34)
#define RTL838X_INT_MODE_CTRL			(RTL838X_SW_BASE + 0x5c)
#define RTL838X_CHIP_INFO			(RTL838X_SW_BASE + 0xd8)
#define RTL839X_CHIP_INFO			(RTL838X_SW_BASE + 0x0ff4)
#define RTL838X_SDS4_REG28			(RTL838X_SW_BASE + 0xef80)
#define RTL838X_SDS4_DUMMY0			(RTL838X_SW_BASE + 0xef8c)
#define RTL838X_SDS5_EXT_REG6			(RTL838X_SW_BASE + 0xf18c)
#define RTL838X_PORT_ISO_CTRL(port)		(RTL838X_SW_BASE + 0x4100 + ((port) << 2))
#define RTL839X_PORT_ISO_CTRL(port)		(RTL838X_SW_BASE + 0x1400 + ((port) << 3))

#define RTL838X_STAT_PORT_STD_MIB		(RTL838X_SW_BASE + 0x1200)
#define RTL839X_STAT_PORT_STD_MIB		(RTL838X_SW_BASE + 0xC000)
#define RTL838X_STAT_RST			(RTL838X_SW_BASE + 0x3100)
#define RTL839X_STAT_RST			(RTL838X_SW_BASE + 0xF504)
#define RTL838X_STAT_PORT_RST			(RTL838X_SW_BASE + 0x3104)
#define RTL839X_STAT_PORT_RST			(RTL838X_SW_BASE + 0xF508)
#define RTL838X_STAT_CTRL			(RTL838X_SW_BASE + 0x3108)
#define RTL839X_STAT_CTRL			(RTL838X_SW_BASE + 0x04cc)

/* Registers of the internal Serdes of the 8380 */
#define MAPLE_SDS4_REG0r			RTL838X_SDS4_REG28
#define MAPLE_SDS5_REG0r			(RTL838X_SDS4_REG28 + 0x100)
#define MAPLE_SDS4_REG3r			RTL838X_SDS4_DUMMY0
#define MAPLE_SDS5_REG3r			(RTL838X_SDS4_REG28 + 0x100)
#define MAPLE_SDS4_FIB_REG0r			(RTL838X_SDS4_REG28 + 0x880)
#define MAPLE_SDS5_FIB_REG0r			(RTL838X_SDS4_REG28 + 0x980)

/* VLAN registers */
#define RTL838X_VLAN_PROFILE(idx)		(RTL838X_SW_BASE + 0x3A88 + ((idx) << 2))
#define RTL838X_VLAN_PORT_EGR_FLTR		(RTL838X_SW_BASE + 0x3A84)
#define RTL838X_VLAN_PORT_PB_VLAN(port)		(RTL838X_SW_BASE + 0x3C00 + ((port) << 2))
#define RTL838X_VLAN_PORT_IGR_FLTR_0		(RTL838X_SW_BASE + 0x3A7C)
#define RTL838X_VLAN_PORT_IGR_FLTR_1		(RTL838X_SW_BASE + 0x3A7C + 4)

/* Table 0/1 access registers */
#define RTL838X_TBL_ACCESS_CTRL_0		(RTL838X_SW_BASE + 0x6914)
#define RTL838X_TBL_ACCESS_DATA_0(idx)		(RTL838X_SW_BASE + 0x6918 + ((idx) << 2))
#define RTL838X_TBL_ACCESS_CTRL_1		(RTL838X_SW_BASE + 0xA4C8)
#define RTL838X_TBL_ACCESS_DATA_1(idx)		(RTL838X_SW_BASE + 0xA4CC + ((idx) << 2))
#define RTL839X_TBL_ACCESS_CTRL_0		(RTL838X_SW_BASE + 0x1190)
#define RTL839X_TBL_ACCESS_DATA_0(idx)		(RTL838X_SW_BASE + 0x1194 + ((idx) << 2))
#define RTL839X_TBL_ACCESS_CTRL_1		(RTL838X_SW_BASE + 0x6b80)
#define RTL839X_TBL_ACCESS_DATA_1(idx)		(RTL838X_SW_BASE + 0x6b84 + ((idx) << 2))

/* MAC handling */
#define RTL838X_MAC_LINK_STS			(RTL838X_SW_BASE + 0xa188)
#define RTL839X_MAC_LINK_STS			(RTL838X_SW_BASE + 0x0390)
#define RTL838X_MAC_LINK_SPD_STS(port)		(RTL838X_SW_BASE + 0xa190 + (((port >> 4) << 2)))
#define RTL838X_MAC_LINK_DUP_STS		(RTL838X_SW_BASE + 0xa19c)
#define RTL838X_MAC_TX_PAUSE_STS		(RTL838X_SW_BASE + 0xa1a0)
#define RTL838X_MAC_RX_PAUSE_STS		(RTL838X_SW_BASE + 0xa1a4)
#define RTL838X_EEE_TX_TIMER_GIGA_CTRL		(RTL838X_SW_BASE + 0xaa04)
#define RTL838X_EEE_TX_TIMER_GELITE_CTRL	(RTL838X_SW_BASE + 0xaa08)

/* MAC link state bits */
#define FORCE_EN				(1 << 0)
#define FORCE_LINK_EN				(1 << 1)
#define NWAY_EN					(1 << 2)
#define DUPLX_MODE				(1 << 3)
#define TX_PAUSE_EN				(1 << 6)
#define RX_PAUSE_EN				(1 << 7)

/* EEE */
#define RTL838X_MAC_EEE_ABLTY			(RTL838X_SW_BASE + 0xa1a8)
#define RTL838X_EEE_PORT_TX_EN			(RTL838X_SW_BASE + 0x014c)
#define RTL838X_EEE_PORT_RX_EN			(RTL838X_SW_BASE + 0x0150)
#define RTL838X_EEE_CLK_STOP_CTRL		(RTL838X_SW_BASE + 0x0148)

/* L2 functionality */
#define RTL838X_L2_CTRL_0			(RTL838X_SW_BASE + 0x3200)
#define RTL839X_L2_CTRL_0			(RTL838X_SW_BASE + 0x3800)
#define RTL838X_L2_CTRL_1			(RTL838X_SW_BASE + 0x3204)
#define RTL839X_L2_CTRL_1			(RTL838X_SW_BASE + 0x3804)
#define RTL838X_L2_PORT_AGING_OUT		(RTL838X_SW_BASE + 0x3358)
#define RTL839X_L2_PORT_AGING_OUT		(RTL838X_SW_BASE + 0x3b74)
#define RTL838X_TBL_ACCESS_L2_CTRL		(RTL838X_SW_BASE + 0x6900)
#define RTL839X_TBL_ACCESS_L2_CTRL		(RTL838X_SW_BASE + 0x1180)
#define RTL838X_TBL_ACCESS_L2_DATA(idx)		(RTL838X_SW_BASE + 0x6908 + ((idx) << 2))
#define RTL838X_TBL_ACCESS_L2_DATA(idx)		(RTL838X_SW_BASE + 0x6908 + ((idx) << 2))
#define RTL838X_L2_TBL_FLUSH_CTRL		(RTL838X_SW_BASE + 0x3370)
#define RTL839X_L2_TBL_FLUSH_CTRL		(RTL838X_SW_BASE + 0x3ba0)

/* Port Mirroring */
#define RTL838X_MIR_CTRL(grp)			(RTL838X_SW_BASE + 0x5D00 + (((grp) << 2)))
#define RTL838X_MIR_DPM_CTRL(grp)		(RTL838X_SW_BASE + 0x5D20 + (((grp) << 2)))
#define RTL838X_MIR_SPM_CTRL(grp)		(RTL838X_SW_BASE + 0x5D10 + (((grp) << 2)))

enum phy_type {
	PHY_NONE = 0,
	PHY_RTL838X_SDS = 1,
	PHY_RTL8218B_INT = 2,
	PHY_RTL8218B_EXT = 3,
	PHY_RTL8214FC = 4
};

struct rtl838x_port {
	bool enable;
	u64 pm;
	u16 pvid;
	bool eee_enabled;
	enum phy_type phy;
};

struct rtl838x_vlan_info {
	u64 untagged_ports;
	u64 tagged_ports;
	u32 vlan_conf;
};

struct rtl838x_switch_priv;

struct rtl838x_reg {
	void (*mask_port_reg)(u64 clear, u64 set, volatile void __iomem *reg);
	void (*set_port_reg)(u64 set, volatile void __iomem *reg);
	u64 (*get_port_reg)(volatile void __iomem *reg);
	volatile void __iomem *stat_port_rst;
	volatile void __iomem *stat_rst;
	volatile void __iomem *(*stat_port_std_mib)(int p);
	void (*mask_port_iso_ctrl)(u64 clear, u64 set, int port);
	void (*set_port_iso_ctrl)(u64 set, int port);
	volatile void __iomem *l2_ctrl_0;
	volatile void __iomem *l2_ctrl_1;
	volatile void __iomem *l2_port_aging_out;
	volatile void __iomem *smi_poll_ctrl;
	volatile void __iomem *l2_tbl_flush_ctrl;
	void (*exec_tbl0_cmd)(u32 cmd);
	void (*exec_tbl1_cmd)(u32 cmd);
	volatile void __iomem *(*tbl_access_data_0)(int i);
	volatile void __iomem *isr_glb_src;
	volatile void __iomem *isr_port_link_sts_chg;
	volatile void __iomem *imr_port_link_sts_chg;
	volatile void __iomem *imr_glb;
	void (*vlan_tables_read)(u32 vlan, struct rtl838x_vlan_info *);
	void (*vlan_set_tagged)(u32 vlan, u64 portmask, u32 conf);
	void (*vlan_set_untagged)(u32 vlan, u64 portmask);
	volatile void __iomem * (*mac_force_mode_ctrl)(int);
	volatile void __iomem *rst_glb_ctrl;
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
};

#endif /* _RTL838X_H */
