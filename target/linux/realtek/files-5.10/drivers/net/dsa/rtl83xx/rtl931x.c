// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include <linux/inetdevice.h>
#include "rtl83xx.h"

extern struct mutex smi_lock;
extern struct rtl83xx_soc_info soc_info;

/* Definition of the RTL931X-specific template field IDs as used in the PIE */
enum template_field_id {
	TEMPLATE_FIELD_SPM0 = 1,
	TEMPLATE_FIELD_SPM1 = 2,
	TEMPLATE_FIELD_SPM2 = 3,
	TEMPLATE_FIELD_SPM3 = 4,
	TEMPLATE_FIELD_DMAC0 = 9,
	TEMPLATE_FIELD_DMAC1 = 10,
	TEMPLATE_FIELD_DMAC2 = 11,
	TEMPLATE_FIELD_SMAC0 = 12,
	TEMPLATE_FIELD_SMAC1 = 13,
	TEMPLATE_FIELD_SMAC2 = 14,
	TEMPLATE_FIELD_ETHERTYPE = 15,
	TEMPLATE_FIELD_OTAG = 16,
	TEMPLATE_FIELD_ITAG = 17,
	TEMPLATE_FIELD_SIP0 = 18,
	TEMPLATE_FIELD_SIP1 = 19,
	TEMPLATE_FIELD_DIP0 = 20,
	TEMPLATE_FIELD_DIP1 = 21,
	TEMPLATE_FIELD_IP_TOS_PROTO = 22,
	TEMPLATE_FIELD_L4_SPORT = 23,
	TEMPLATE_FIELD_L4_DPORT = 24,
	TEMPLATE_FIELD_L34_HEADER = 25,
	TEMPLATE_FIELD_TCP_INFO = 26,
	TEMPLATE_FIELD_SIP2 = 34,
	TEMPLATE_FIELD_SIP3 = 35,
	TEMPLATE_FIELD_SIP4 = 36,
	TEMPLATE_FIELD_SIP5 = 37,
	TEMPLATE_FIELD_SIP6 = 38,
	TEMPLATE_FIELD_SIP7 = 39,
	TEMPLATE_FIELD_DIP2 = 42,
	TEMPLATE_FIELD_DIP3 = 43,
	TEMPLATE_FIELD_DIP4 = 44,
	TEMPLATE_FIELD_DIP5 = 45,
	TEMPLATE_FIELD_DIP6 = 46,
	TEMPLATE_FIELD_DIP7 = 47,
	TEMPLATE_FIELD_FLOW_LABEL = 49,
	TEMPLATE_FIELD_DSAP_SSAP = 50,
	TEMPLATE_FIELD_FWD_VID = 52,
	TEMPLATE_FIELD_RANGE_CHK = 53,
	TEMPLATE_FIELD_SLP = 55,
	TEMPLATE_FIELD_DLP = 56,
	TEMPLATE_FIELD_META_DATA = 57,
	TEMPLATE_FIELD_FIRST_MPLS1 = 60,
	TEMPLATE_FIELD_FIRST_MPLS2 = 61,
	TEMPLATE_FIELD_DPM3 = 8,
};

/* The meaning of TEMPLATE_FIELD_VLAN depends on phase and the configuration in
 * RTL931X_PIE_CTRL. We use always the same definition and map to the inner VLAN tag:
 */
#define TEMPLATE_FIELD_VLAN TEMPLATE_FIELD_ITAG

// Number of fixed templates predefined in the RTL9300 SoC
#define N_FIXED_TEMPLATES 5
// RTL931x specific predefined templates
static enum template_field_id fixed_templates[N_FIXED_TEMPLATES][N_FIXED_FIELDS_RTL931X] =
{
	{
		TEMPLATE_FIELD_DMAC0, TEMPLATE_FIELD_DMAC1, TEMPLATE_FIELD_DMAC2,
		TEMPLATE_FIELD_SMAC0, TEMPLATE_FIELD_SMAC1, TEMPLATE_FIELD_SMAC2,
		TEMPLATE_FIELD_VLAN, TEMPLATE_FIELD_IP_TOS_PROTO, TEMPLATE_FIELD_DSAP_SSAP,
		TEMPLATE_FIELD_ETHERTYPE, TEMPLATE_FIELD_SPM0, TEMPLATE_FIELD_SPM1,
		TEMPLATE_FIELD_SPM2, TEMPLATE_FIELD_SPM3
	}, {
		TEMPLATE_FIELD_SIP0, TEMPLATE_FIELD_SIP1, TEMPLATE_FIELD_DIP0,
		TEMPLATE_FIELD_DIP1, TEMPLATE_FIELD_IP_TOS_PROTO, TEMPLATE_FIELD_TCP_INFO,
		TEMPLATE_FIELD_L4_SPORT, TEMPLATE_FIELD_L4_DPORT, TEMPLATE_FIELD_VLAN,
		TEMPLATE_FIELD_RANGE_CHK, TEMPLATE_FIELD_SPM0, TEMPLATE_FIELD_SPM1,
		TEMPLATE_FIELD_SPM2, TEMPLATE_FIELD_SPM3
	}, {
		TEMPLATE_FIELD_DMAC0, TEMPLATE_FIELD_DMAC1, TEMPLATE_FIELD_DMAC2,
		TEMPLATE_FIELD_VLAN, TEMPLATE_FIELD_ETHERTYPE, TEMPLATE_FIELD_IP_TOS_PROTO,
		TEMPLATE_FIELD_SIP0, TEMPLATE_FIELD_SIP1, TEMPLATE_FIELD_DIP0,
		TEMPLATE_FIELD_DIP1, TEMPLATE_FIELD_L4_SPORT, TEMPLATE_FIELD_L4_DPORT,
		TEMPLATE_FIELD_META_DATA, TEMPLATE_FIELD_SLP
	}, {
		TEMPLATE_FIELD_DIP0, TEMPLATE_FIELD_DIP1, TEMPLATE_FIELD_DIP2,
		TEMPLATE_FIELD_DIP3, TEMPLATE_FIELD_DIP4, TEMPLATE_FIELD_DIP5,
		TEMPLATE_FIELD_DIP6, TEMPLATE_FIELD_DIP7, TEMPLATE_FIELD_IP_TOS_PROTO,
		TEMPLATE_FIELD_TCP_INFO, TEMPLATE_FIELD_L4_SPORT, TEMPLATE_FIELD_L4_DPORT,
		TEMPLATE_FIELD_RANGE_CHK, TEMPLATE_FIELD_SLP
	}, {
		TEMPLATE_FIELD_SIP0, TEMPLATE_FIELD_SIP1, TEMPLATE_FIELD_SIP2,
		TEMPLATE_FIELD_SIP3, TEMPLATE_FIELD_SIP4, TEMPLATE_FIELD_SIP5,
		TEMPLATE_FIELD_SIP6, TEMPLATE_FIELD_SIP7, TEMPLATE_FIELD_META_DATA,
		TEMPLATE_FIELD_VLAN, TEMPLATE_FIELD_SPM0, TEMPLATE_FIELD_SPM1,
		TEMPLATE_FIELD_SPM2, TEMPLATE_FIELD_SPM3
	},
};

inline void rtl931x_exec_tbl0_cmd(u32 cmd)
{
	sw_w32(cmd, RTL931X_TBL_ACCESS_CTRL_0);
	do { } while (sw_r32(RTL931X_TBL_ACCESS_CTRL_0) & (1 << 20));
}

inline void rtl931x_exec_tbl1_cmd(u32 cmd)
{
	sw_w32(cmd, RTL931X_TBL_ACCESS_CTRL_1);
	do { } while (sw_r32(RTL931X_TBL_ACCESS_CTRL_1) & (1 << 17));
}

inline int rtl931x_tbl_access_data_0(int i)
{
	return RTL931X_TBL_ACCESS_DATA_0(i);
}

void rtl931x_vlan_profile_dump(int index)
{
	u64 profile[4];

	if (index < 0 || index > 15)
		return;

	profile[0] = sw_r32(RTL931X_VLAN_PROFILE_SET(index));
	profile[1] = (sw_r32(RTL931X_VLAN_PROFILE_SET(index) + 4) & 0x1FFFFFFFULL) << 32
		| (sw_r32(RTL931X_VLAN_PROFILE_SET(index) + 8) & 0xFFFFFFFF);
	profile[2] = (sw_r32(RTL931X_VLAN_PROFILE_SET(index) + 16) & 0x1FFFFFFFULL) << 32
		| (sw_r32(RTL931X_VLAN_PROFILE_SET(index) + 12) & 0xFFFFFFFF);
	profile[3] = (sw_r32(RTL931X_VLAN_PROFILE_SET(index) + 20) & 0x1FFFFFFFULL) << 32
		| (sw_r32(RTL931X_VLAN_PROFILE_SET(index) + 24) & 0xFFFFFFFF);

	pr_info("VLAN %d: L2 learning: %d, L2 Unknown MultiCast Field %llx, \
		IPv4 Unknown MultiCast Field %llx, IPv6 Unknown MultiCast Field: %llx",
		index, (u32) (profile[0] & (3 << 14)), profile[1], profile[2], profile[3]);
}

static void rtl931x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, u32 port_state[])
{
	int i;
	u32 cmd = 1 << 20 /* Execute cmd */
		| 0 << 19 /* Read */
		| 5 << 15 /* Table type 0b101 */
		| (msti & 0x3fff);
	priv->r->exec_tbl0_cmd(cmd);

	for (i = 0; i < 4; i++)
		port_state[i] = sw_r32(priv->r->tbl_access_data_0(i));
}

static void rtl931x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, u32 port_state[])
{
	int i;
	u32 cmd = 1 << 20 /* Execute cmd */
		| 1 << 19 /* Write */
		| 5 << 15 /* Table type 0b101 */
		| (msti & 0x3fff);
	for (i = 0; i < 4; i++)
		sw_w32(port_state[i], priv->r->tbl_access_data_0(i));
	priv->r->exec_tbl0_cmd(cmd);
}

inline static int rtl931x_trk_mbr_ctr(int group)
{
	return RTL931X_TRK_MBR_CTRL + (group << 2);
}

static void rtl931x_vlan_tables_read(u32 vlan, struct rtl838x_vlan_info *info)
{
	u32 v, w, x, y;
	// Read VLAN table (3) via register 0
	struct table_reg *r = rtl_table_get(RTL9310_TBL_0, 3);

	rtl_table_read(r, vlan);
	v = sw_r32(rtl_table_data(r, 0));
	w = sw_r32(rtl_table_data(r, 1));
	x = sw_r32(rtl_table_data(r, 2));
	y = sw_r32(rtl_table_data(r, 3));
	rtl_table_release(r);

	pr_debug("VLAN_READ %d: %08x %08x %08x %08x\n", vlan, v, w, x, y);
	info->tagged_ports = ((u64) v) << 25 | (w >> 7);
	info->profile_id = (x >> 16) & 0xf;
	info->fid = w & 0x7f;				// AKA MSTI depending on context
	info->hash_uc_fid = !!(x & BIT(31));
	info->hash_mc_fid = !!(x & BIT(30));
	info->if_id = (x >> 20) & 0x3ff;
	info->profile_id = (x >> 16) & 0xf;
	info->multicast_grp_mask = x & 0xffff;
	if (x & BIT(31))
		info->l2_tunnel_list_id = y >> 18;
	else
		info->l2_tunnel_list_id = -1;
	pr_debug("%s read tagged %016llx, profile-id %d, uc %d, mc %d, intf-id %d\n", __func__,
		info->tagged_ports, info->profile_id, info->hash_uc_fid, info->hash_mc_fid,
		info->if_id);

	// Read UNTAG table via table register 3
	r = rtl_table_get(RTL9310_TBL_3, 0);
	rtl_table_read(r, vlan);
	v = ((u64)sw_r32(rtl_table_data(r, 0))) << 25;
	v |= sw_r32(rtl_table_data(r, 1)) >> 7;
	rtl_table_release(r);

	info->untagged_ports = v;
}

static void rtl931x_vlan_set_tagged(u32 vlan, struct rtl838x_vlan_info *info)
{
	u32 v, w, x, y;
	// Access VLAN table (3) via register 0
	struct table_reg *r = rtl_table_get(RTL9310_TBL_0, 3);

	v = info->tagged_ports >> 25;
	w = (info->tagged_ports & 0x1fffff) << 7;
	w |= info->fid & 0x7f;
	x = info->hash_uc_fid ? BIT(31) : 0;
	x |= info->hash_mc_fid ? BIT(30) : 0;
	x |= info->if_id & 0x3ff << 20;
	x |= (info->profile_id & 0xf) << 16;
	x |= info->multicast_grp_mask & 0xffff;
	if (info->l2_tunnel_list_id >= 0) {
		y = info->l2_tunnel_list_id << 18;
		y |= BIT(31);
	} else {
		y = 0;
	}

	sw_w32(v, rtl_table_data(r, 0));
	sw_w32(w, rtl_table_data(r, 1));
	sw_w32(x, rtl_table_data(r, 2));
	sw_w32(y, rtl_table_data(r, 3));

	rtl_table_write(r, vlan);
	rtl_table_release(r);
}

static void rtl931x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	struct table_reg *r = rtl_table_get(RTL9310_TBL_3, 0);

	rtl839x_set_port_reg_be(portmask << 7, rtl_table_data(r, 0));
	rtl_table_write(r, vlan);
	rtl_table_release(r);
}

static inline int rtl931x_mac_force_mode_ctrl(int p)
{
	return RTL931X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl931x_mac_link_spd_sts(int p)
{
	return RTL931X_MAC_LINK_SPD_STS + (((p >> 3) << 2));
}

static inline int rtl931x_mac_port_ctrl(int p)
{
	return RTL931X_MAC_L2_PORT_CTRL + (p << 7);
}

static inline int rtl931x_l2_port_new_salrn(int p)
{
	return RTL931X_L2_PORT_NEW_SALRN(p);
}

static inline int rtl931x_l2_port_new_sa_fwd(int p)
{
	return RTL931X_L2_PORT_NEW_SA_FWD(p);
}

irqreturn_t rtl931x_switch_irq(int irq, void *dev_id)
{
	struct dsa_switch *ds = dev_id;
	u32 status = sw_r32(RTL931X_ISR_GLB_SRC);
	u64 ports = rtl839x_get_port_reg_le(RTL931X_ISR_PORT_LINK_STS_CHG);
	u64 link;
	u32 sds;
	int i;

	/* Clear status */
	rtl839x_set_port_reg_le(ports, RTL931X_ISR_PORT_LINK_STS_CHG);
	pr_info("RTL931X Link change: status: %x, ports %016llx\n", status, ports);

	link = rtl839x_get_port_reg_le(RTL931X_MAC_LINK_STS);
	// Must re-read this to get correct status
	link = rtl839x_get_port_reg_le(RTL931X_MAC_LINK_STS);
	pr_info("RTL931X Link change: status: %x, link status %016llx\n", status, link);

	for (i = 0; i < RTL931X_CPU_PORT; i++) {
		if (ports & BIT_ULL(i)) {
			if (link & BIT_ULL(i)) {
				pr_info("%s port %d up\n", __func__, i);
				dsa_port_phylink_mac_change(ds, i, true);
			} else {
				pr_info("%s port %d down\n", __func__, i);
				dsa_port_phylink_mac_change(ds, i, false);
			}
		}
	}

	// Handle SDS Errors
	sds = sw_r32(RTL931X_ISR_SERDES_ERR);
	if (sds) {
		pr_info("%s error on SDS: %08x\n", __func__, sds);
		sw_w32(sds, RTL931X_ISR_SERDES_ERR);
	}

	// Handle SDS RX Idle
	sds = sw_r32(RTL931X_ISR_SERDES_RXIDLE);
	if (sds) {
		pr_info("%s RXIDLE on SDS: %08x\n", __func__, sds);
		sw_w32(sds, RTL931X_ISR_SERDES_RXIDLE);
	}

	// Handle SDS PHYSTS errors
	ports = rtl839x_get_port_reg_le(RTL931X_ISR_SDS_UPD_PHYSTS);
	if (ports) {
		pr_info("%s SDS_UPD_PHYSTS: %016llx\n", __func__, ports);
		rtl839x_set_port_reg_le(ports, RTL931X_ISR_SDS_UPD_PHYSTS);
	}

	return IRQ_HANDLED;
}

int rtl931x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v;
	int err = 0;

	val &= 0xffff;
	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&smi_lock);
	pr_debug("%s: writing to phy %d %d %d %d\n", __func__, port, page, reg, val);
	/* Clear both port registers */
	sw_w32(0, RTL931X_SMI_INDRT_ACCESS_CTRL_2);
	sw_w32(0, RTL931X_SMI_INDRT_ACCESS_CTRL_2 + 4);
	sw_w32_mask(0, BIT(port % 32), RTL931X_SMI_INDRT_ACCESS_CTRL_2 + (port / 32) * 4);

	sw_w32_mask(0xffff, val, RTL931X_SMI_INDRT_ACCESS_CTRL_3);

	v = reg << 6 | page << 11 ;
	sw_w32(v, RTL931X_SMI_INDRT_ACCESS_CTRL_0);

	sw_w32(0x1ff, RTL931X_SMI_INDRT_ACCESS_CTRL_1);

	v |= BIT(4) | 1; /* Write operation and execute */
	sw_w32(v, RTL931X_SMI_INDRT_ACCESS_CTRL_0);

	do {
	} while (sw_r32(RTL931X_SMI_INDRT_ACCESS_CTRL_0) & 0x1);

	if (sw_r32(RTL931X_SMI_INDRT_ACCESS_CTRL_0) & 0x2)
		err = -EIO;

	mutex_unlock(&smi_lock);
	return err;
}

int rtl931x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&smi_lock);

	sw_w32(port << 5, RTL931X_SMI_INDRT_ACCESS_BC_PHYID_CTRL);

	sw_w32(reg << 6 | page << 11 | 1, RTL931X_SMI_INDRT_ACCESS_CTRL_0);

	do {
	} while (sw_r32(RTL931X_SMI_INDRT_ACCESS_CTRL_0) & 0x1);

	*val = sw_r32(RTL931X_SMI_INDRT_ACCESS_CTRL_3);
	*val = (*val & 0xffff0000) >> 16;

	pr_debug("%s: port %d, page: %d, reg: %x, val: %x\n",
		__func__, port, page, reg, *val);

	mutex_unlock(&smi_lock);
	return 0;
}

/*
 * Read an mmd register of the PHY
 */
int rtl931x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val)
{
	int err = 0;
	u32 v;
	/* Select PHY register type
	 * If select 1G/10G MMD register type, registers EXT_PAGE, MAIN_PAGE and REG settings are don’t care.
	 * 0x0  Normal register (Clause 22)
	 * 0x1: 1G MMD register (MMD via Clause 22 registers 13 and 14)
	 * 0x2: 10G MMD register (MMD via Clause 45)
	 */
//	int type = (regnum & MII_ADDR_C45)?2:1;
	int type = 2;

	mutex_lock(&smi_lock);

	// Set PHY to access via port-number
	sw_w32(port << 5, RTL931X_SMI_INDRT_ACCESS_BC_PHYID_CTRL);

	// Set MMD device number and register to write to
	sw_w32(devnum << 16 | mdiobus_c45_regad(regnum), RTL931X_SMI_INDRT_ACCESS_MMD_CTRL);

	v = type << 2 | BIT(0); // MMD-access-type | EXEC
	sw_w32(v, RTL931X_SMI_INDRT_ACCESS_CTRL_0);

	do {
		v = sw_r32(RTL931X_SMI_INDRT_ACCESS_CTRL_0);
	} while (v & BIT(0));

	// Check for error condition
	if (v & BIT(1))
		err = -EIO;

	*val = sw_r32(RTL931X_SMI_INDRT_ACCESS_CTRL_3) >> 16;

	pr_debug("%s: port %d, dev: %x, regnum: %x, val: %x (err %d)\n", __func__,
		 port, devnum, mdiobus_c45_regad(regnum), *val, err);

	mutex_unlock(&smi_lock);

	return err;
}

/*
 * Write to an mmd register of the PHY
 */
int rtl931x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val)
{
	int err = 0;
	u32 v;
//	int type = (regnum & MII_ADDR_C45)?2:1;
	int type = 1;

	mutex_lock(&smi_lock);

	// Set PHY to access via port-number
	sw_w32(port << 5, RTL931X_SMI_INDRT_ACCESS_BC_PHYID_CTRL);

	// Set data to write
	sw_w32_mask(0xffff, val, RTL931X_SMI_INDRT_ACCESS_CTRL_3);

	// Set MMD device number and register to write to
	sw_w32(devnum << 16 | mdiobus_c45_regad(regnum), RTL931X_SMI_INDRT_ACCESS_MMD_CTRL);

	v = BIT(4) | type << 2 | BIT(0); // WRITE | MMD-access-type | EXEC
	sw_w32(v, RTL931X_SMI_INDRT_ACCESS_CTRL_0);

	do {
		v = sw_r32(RTL931X_SMI_INDRT_ACCESS_CTRL_0);
	} while (v & BIT(0));

	pr_debug("%s: port %d, dev: %x, regnum: %x, val: %x (err %d)\n", __func__,
		 port, devnum, mdiobus_c45_regad(regnum), val, err);
	mutex_unlock(&smi_lock);
	return err;
}

void rtl931x_print_matrix(void)
{
	volatile u64 *ptr = RTL838X_SW_BASE + RTL839X_PORT_ISO_CTRL(0);
	int i;

	for (i = 0; i < 52; i += 4)
		pr_info("> %16llx %16llx %16llx %16llx\n",
			ptr[i + 0], ptr[i + 1], ptr[i + 2], ptr[i + 3]);
	pr_info("CPU_PORT> %16llx\n", ptr[52]);
}

void rtl931x_set_receive_management_action(int port, rma_ctrl_t type, action_type_t action)
{
	u32 value = 0;

	/* hack for value mapping */
	if (type == GRATARP && action == COPY2CPU)
		action = TRAP2MASTERCPU;

	switch(action) {
	case FORWARD:
		value = 0;
		break;
	case DROP:
		value = 1;
		break;
	case TRAP2CPU:
		value = 2;
		break;
	case TRAP2MASTERCPU:
		value = 3;
		break;
	case FLOODALL:
		value = 4;
		break;
	default:
		break;
	}

	switch(type) {
	case BPDU:
		sw_w32_mask(7 << ((port % 10) * 3), value << ((port % 10) * 3), RTL931X_RMA_BPDU_CTRL + ((port / 10) << 2));
	break;
	case PTP:
		//udp
		sw_w32_mask(3 << 2, value << 2, RTL931X_RMA_PTP_CTRL + (port << 2));
		//eth2
		sw_w32_mask(3, value, RTL931X_RMA_PTP_CTRL + (port << 2));
	break;
	case PTP_UDP:
		sw_w32_mask(3 << 2, value << 2, RTL931X_RMA_PTP_CTRL + (port << 2));
	break;
	case PTP_ETH2:
		sw_w32_mask(3, value, RTL931X_RMA_PTP_CTRL + (port << 2));
	break;
	case LLTP:
		sw_w32_mask(7 << ((port % 10) * 3), value << ((port % 10) * 3), RTL931X_RMA_LLTP_CTRL + ((port / 10) << 2));
	break;
	case EAPOL:
		sw_w32_mask(7 << ((port % 10) * 3), value << ((port % 10) * 3), RTL931X_RMA_EAPOL_CTRL + ((port / 10) << 2));
	break;
	case GRATARP:
		sw_w32_mask(3 << ((port & 0xf) << 1), value << ((port & 0xf) << 1), RTL931X_TRAP_ARP_GRAT_PORT_ACT + ((port >> 4) << 2));
	break;
	}
}

u64 rtl931x_traffic_get(int source)
{
	u64 v;
	// Read PORT_ISO_CTRL table (1) via register RTL9310_TBL_2
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 1);

	rtl_table_read(r, source);
	v = rtl839x_get_port_reg_be(rtl_table_data(r, 0));
	rtl_table_release(r);
	return v >> 7;
}

/*
 * Enable traffic between a source port and a destination port matrix
 */
void rtl931x_traffic_set(int source, u64 dest_matrix)
{
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 1);

	rtl839x_set_port_reg_be((dest_matrix << 7), rtl_table_data(r, 0));
	pr_debug("%s source %d pmask %016llx\n",
		__func__, source, rtl839x_get_port_reg_be(rtl_table_data(r, 0)));
	rtl_table_write(r, source);
	rtl_table_release(r);
}

void rtl931x_traffic_enable(int source, int dest)
{
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 1);
	rtl_table_read(r, source);
	rtl839x_mask_port_reg_be(0, BIT_ULL(dest + 7), rtl_table_data(r, 0));
	pr_debug("%s source %d pmask %016llx\n",
		__func__, source, rtl839x_get_port_reg_be(rtl_table_data(r, 0)));
	rtl_table_write(r, source);
	rtl_table_release(r);
}

void rtl931x_traffic_disable(int source, int dest)
{
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 1);
	rtl_table_read(r, source);
	rtl839x_mask_port_reg_be(BIT_ULL(dest + 7), 0, rtl_table_data(r, 0));
	pr_debug("%s source %d pmask %016llx\n",
		__func__, source, rtl839x_get_port_reg_be(rtl_table_data(r, 0)));
	rtl_table_write(r, source);
	rtl_table_release(r);
}

/*
 * Enables or disables the EEE/EEEP capability of a port
 */
void rtl931x_port_eee_set(struct rtl838x_switch_priv *priv, int port, bool enable)
{
	u32 v;

	// We do not support the SFP+ ports where the internal PHY is used
	if (priv->ports[port].phy_is_integrated)
		return;

	pr_info("In %s: setting port %d to %d\n", __func__, port, enable);
	v = enable ? 0x3f : 0x0;

	// Set EEE/EEEP state for 100, 500, 1000MBit and 2.5, 5 and 10GBit
	sw_w32_mask(0, v << 18, rtl931x_mac_force_mode_ctrl(port));

	pr_info("%s RTL9310_MAC_FORCE_MODE_CTRL for port %d is 0x%08x\n",
		__func__, port, sw_r32(rtl931x_mac_force_mode_ctrl(port)));

	v = enable ? BIT(port % 32) : 0;
	sw_w32_mask(BIT(port % 32), v, RTL931X_EEE_PORT_TX_EN + 4 * (port >> 5));
	sw_w32_mask(BIT(port % 32), v, RTL931X_EEE_PORT_RX_EN + 4 * (port >> 5));

	priv->ports[port].eee_enabled = enable;
}

/*
 * Get EEE own capabilities and negotiation result
 */
int rtl931x_eee_port_ability(struct rtl838x_switch_priv *priv, struct ethtool_eee *e, int port)
{
	u64 link, a, speed;
	u32 v;

	// We do not support the SFP+ ports where the internal PHY is used
	if (priv->ports[port].phy_is_integrated)
		return -ENOTSUPP;

	pr_info("In %s, port %d\n", __func__, port);
	link = rtl839x_get_port_reg_le(RTL931X_MAC_LINK_STS);
	link = rtl839x_get_port_reg_le(RTL931X_MAC_LINK_STS); // Read twice because of latching
	if (!(link & BIT_ULL(port)))
		return 0;

	a = rtl839x_get_port_reg_le(RTL931X_MAC_EEE_ABLTY);
	a = rtl839x_get_port_reg_le(RTL931X_MAC_EEE_ABLTY);
	pr_info("Link partner: %016llx\n", a);
	if (a & BIT_ULL(port)) {
		speed = priv->r->get_port_reg_le(priv->r->mac_link_spd_sts(port));
		speed >>= (port % 8) << 2;
		switch (speed & 0xf) {
		case 1:
			v = sw_r32(RTL931X_EEE_TX_MINIFG_CTRL0) & 0xffff;
			break;
		case 2:
		case 7:
			v = sw_r32(RTL931X_EEE_TX_MINIFG_CTRL1) & 0xffff;
			break;
		case 4:
			v = sw_r32(RTL931X_EEE_TX_MINIFG_CTRL1) >> 16;
			break;
		case 5:
		case 8:
			v = sw_r32(RTL931X_EEE_TX_MINIFG_CTRL2) & 0xffff;
			break;
		case 6:
			v = sw_r32(RTL931X_EEE_TX_MINIFG_CTRL2) >> 16;
			break;
		default:
			v = 0;
		}
		e->tx_lpi_timer = v;
		if (v)
			e->tx_lpi_enabled = true;
	}

	// Read 2x to clear latched state
	a = rtl839x_get_port_reg_le(RTL931X_EEE_PORT_TX_STS);
	a = rtl839x_get_port_reg_le(RTL931X_EEE_PORT_TX_STS);
	pr_info("%s RTL931X_EEE_PORT_TX_STS: %016llx\n", __func__, a);

	a = rtl839x_get_port_reg_le(RTL931X_EEE_PORT_RX_STS);
	a = rtl839x_get_port_reg_le(RTL931X_EEE_PORT_RX_STS);
	pr_info("%s RTL931X_EEE_PORT_RX_STS: %016llx\n", __func__, a);

	return 0;
}

static void rtl931x_init_eee(struct rtl838x_switch_priv *priv, bool enable)
{
	int i;

	pr_info("Setting up EEE, state: %d\n", enable);

	// Setup EEE on all ports
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy)
			rtl931x_port_eee_set(priv, i, enable);
	}

	priv->eee_enabled = enable;
}

#define HASH_PICK(val, lsb, len, m)   (((val & (((1 << len) - 1) << lsb)) >> lsb) << m)

/*
 * Hash algorithm 0 used with IPv4 on L3
 */
static u32 rtl931x_l3_hash4_0(u32 sip, u32 dip, u32 vrf_id, u32 vid)
{
	u32 rows[11];
	u32 hash;

	memset(rows, 0, sizeof(rows));

	rows[0] = HASH_PICK(vrf_id, 0, 5, 5) | HASH_PICK(vrf_id, 5, 3, 0);

	rows[1] = HASH_PICK(sip, 27, 5, 0);
	rows[2] = HASH_PICK(sip, 17, 10, 0);
	rows[3] = HASH_PICK(sip, 7, 10, 0);
	rows[4] = HASH_PICK(sip, 0, 7, 3);
	
	rows[5] = HASH_PICK(dip, 30, 2, 0);
	rows[6] = HASH_PICK(dip, 20, 10, 0);
	rows[7] = HASH_PICK(dip, 10, 10, 0);
	rows[8] = HASH_PICK(dip, 0, 10, 0);

	rows[9] = HASH_PICK(vid, 3, 9, 0);
	rows[10] = HASH_PICK(vid, 0, 3, 7);

	hash = rows[0] ^ rows[1] ^ rows[2] ^ rows[3] ^ rows[4]
		^ rows[5] ^ rows[6] ^ rows[7] ^ rows[8] ^ rows[9] ^ rows[10];

	return hash;
}

/*
 * Hash algorithm 1 used with IPv4 on L3
 */
static u32 rtl931x_l3_hash4_1(u32 sip, u32 dip, u32 vrf_id, u32 vid)
{
	u32 rows[11];
	u32 hash;
	u32 s0;

	memset(rows, 0, sizeof(rows));

	rows[8] = HASH_PICK(vrf_id, 0, 5, 5) | HASH_PICK(vrf_id, 5, 3, 0);

	rows[0] = HASH_PICK(sip, 30, 2, 0);
	rows[1] = HASH_PICK(sip, 20, 10, 0);
	rows[2] = HASH_PICK(sip, 10, 10, 0);
	rows[3] = HASH_PICK(sip, 0, 10, 0);

	rows[4] = HASH_PICK(dip, 30, 2, 0);
	rows[5] = HASH_PICK(dip, 20, 10, 0);
	rows[6] = HASH_PICK(dip, 10, 10, 0);
	rows[7] = HASH_PICK(dip, 0, 10, 0);

	rows[9] = HASH_PICK(vid, 8, 4, 0);
	rows[10] = HASH_PICK(vid, 0, 8, 2);

	s0 = rows[0] + rows[1];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[2];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[3];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[4];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[5];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[6];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[7];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);

	hash = s0 ^ rows[8] ^ rows[9] ^ rows[10];

	return hash;
}

static u32 rtl931x_l3_hash6_0(struct in6_addr *sip6, struct in6_addr *dip6, u32 vrf_id, u32 vid)
{
	u32 rows[29];
	u32 hash;

	memset(rows, 0, sizeof(rows));
	rows[0] = HASH_PICK(vrf_id, 0, 5, 5) | HASH_PICK(vrf_id, 5, 3, 0);

	rows[1] = HASH_PICK(sip6->s6_addr[0], 0, 7, 3) | HASH_PICK(sip6->s6_addr[1], 5, 3, 0);
	rows[2] = HASH_PICK(sip6->s6_addr[1], 0, 5, 5) | HASH_PICK(sip6->s6_addr[2], 3, 5, 0);
	rows[3] = HASH_PICK(sip6->s6_addr[2], 0, 3, 7) | HASH_PICK(sip6->s6_addr[2], 1, 7, 0);
	rows[4] = HASH_PICK(sip6->s6_addr[3], 0, 1, 9) | HASH_PICK(sip6->s6_addr[4], 0, 8, 1)
		   | HASH_PICK(sip6->s6_addr[5], 7, 1, 0);
	rows[5] = HASH_PICK(sip6->s6_addr[5], 0, 7, 3) | HASH_PICK(sip6->s6_addr[6], 5, 3, 0);
	rows[6] = HASH_PICK(sip6->s6_addr[6], 0, 5, 5) | HASH_PICK(sip6->s6_addr[7], 3, 5, 0);
	rows[7] = HASH_PICK(sip6->s6_addr[7], 0, 3, 7) | HASH_PICK(sip6->s6_addr[8], 1, 7, 0);
	rows[8] = HASH_PICK(sip6->s6_addr[8], 0, 1, 9) | HASH_PICK(sip6->s6_addr[9], 0, 8, 1)
		   | HASH_PICK(sip6->s6_addr[10], 7, 1, 0);
	rows[9] = HASH_PICK(sip6->s6_addr[10], 0, 7, 3) | HASH_PICK(sip6->s6_addr[11], 5, 3, 0);
	rows[10] = HASH_PICK(sip6->s6_addr[11], 0, 5, 5) | HASH_PICK(sip6->s6_addr[12], 3, 5, 0);
	rows[11] = HASH_PICK(sip6->s6_addr[12], 0, 3, 7) | HASH_PICK(sip6->s6_addr[13], 1, 7, 0);
	rows[12] = HASH_PICK(sip6->s6_addr[13], 0, 1, 9) | HASH_PICK(sip6->s6_addr[14], 0, 8, 1)
		   | HASH_PICK(sip6->s6_addr[15], 7, 1, 0);
	rows[13] = HASH_PICK(sip6->s6_addr[15], 0, 7, 3) | HASH_PICK(sip6->s6_addr[0], 7, 1, 0);

	rows[14] = HASH_PICK(dip6->s6_addr[0], 0, 8, 0);
	rows[15] = HASH_PICK(dip6->s6_addr[1], 0, 8, 2) | HASH_PICK(dip6->s6_addr[2], 6, 2, 0);
	rows[16] = HASH_PICK(dip6->s6_addr[2], 0, 6, 4) | HASH_PICK(dip6->s6_addr[3], 4, 4, 0);
	rows[17] = HASH_PICK(dip6->s6_addr[3], 0, 4, 6) | HASH_PICK(dip6->s6_addr[4], 2, 6, 0);
	rows[18] = HASH_PICK(dip6->s6_addr[4], 0, 2, 8) | HASH_PICK(dip6->s6_addr[5], 0, 8, 0);
	rows[19] = HASH_PICK(dip6->s6_addr[6], 0, 8, 2) | HASH_PICK(dip6->s6_addr[7], 6, 2, 0);
	rows[20] = HASH_PICK(dip6->s6_addr[7], 0, 6, 4) | HASH_PICK(dip6->s6_addr[8], 4, 4, 0);
	rows[21] = HASH_PICK(dip6->s6_addr[8], 0, 4, 6) | HASH_PICK(dip6->s6_addr[9], 2, 6, 0);
	rows[22] = HASH_PICK(dip6->s6_addr[9], 0, 2, 8) | HASH_PICK(dip6->s6_addr[10], 0, 8, 0);
	rows[23] = HASH_PICK(dip6->s6_addr[11], 0, 8, 2) | HASH_PICK(dip6->s6_addr[12], 6, 2, 0);
	rows[24] = HASH_PICK(dip6->s6_addr[12], 0, 6, 4) | HASH_PICK(dip6->s6_addr[13], 4, 4, 0);
	rows[25] = HASH_PICK(dip6->s6_addr[13], 0, 4, 6) | HASH_PICK(dip6->s6_addr[14], 2, 6, 0);
	rows[26] = HASH_PICK(sip6->s6_addr[14], 0, 2, 8) | HASH_PICK(dip6->s6_addr[15], 0, 8, 0);

	rows[27] = HASH_PICK(vid, 3, 9, 0);
	rows[28] = HASH_PICK(vid, 0, 3, 7);

	hash = rows[0] ^ rows[1] ^ rows[2] ^ rows[3] ^ rows[4]
		^ rows[5] ^ rows[6] ^ rows[7] ^ rows[8] ^ rows[9] ^ rows[10]
		^ rows[11] ^ rows[12] ^ rows[13] ^ rows[14] ^ rows[15] ^ rows[16]
		^ rows[17] ^ rows[18] ^ rows[19] ^ rows[20] ^ rows[21] ^ rows[22]
		^ rows[23] ^ rows[24] ^ rows[25] ^ rows[26] ^ rows[27] ^ rows[28];

	return hash;
}


static u32 rtl931x_l3_hash6_1(struct in6_addr *sip6, struct in6_addr *dip6, u32 vrf_id, u32 vid)
{
	u32 rows[29];
	u32 hash;
	u32 s0;

	memset(rows, 0, sizeof(rows));
	rows[0] = HASH_PICK(sip6->s6_addr[12], 6, 2, 0);
	rows[1] = HASH_PICK(sip6->s6_addr[12], 0, 6, 4) | HASH_PICK(sip6->s6_addr[13], 4, 4, 0);
	rows[2] = HASH_PICK(sip6->s6_addr[13], 0, 4, 6) | HASH_PICK(sip6->s6_addr[14], 2, 6, 0);
	rows[3] = HASH_PICK(sip6->s6_addr[14], 0, 2, 8) | HASH_PICK(sip6->s6_addr[15], 0, 8, 0);

	rows[4] = HASH_PICK(dip6->s6_addr[12], 6, 2, 0);
	rows[5] = HASH_PICK(dip6->s6_addr[12], 0, 6, 4) | HASH_PICK(dip6->s6_addr[13], 4, 4, 0);
	rows[6] = HASH_PICK(dip6->s6_addr[13], 0, 4, 6) | HASH_PICK(dip6->s6_addr[14], 2, 6, 0);
	rows[7] = HASH_PICK(dip6->s6_addr[14], 0, 2, 8) | HASH_PICK(dip6->s6_addr[15], 0, 8, 0);

	rows[0] = HASH_PICK(vrf_id, 0, 4, 6) | HASH_PICK(vrf_id, 4, 4, 0);

	rows[9] = HASH_PICK(sip6->s6_addr[0], 2, 6, 0);
	rows[10] = HASH_PICK(sip6->s6_addr[0], 0, 2, 8) | HASH_PICK(sip6->s6_addr[1], 0, 8, 0);
	rows[11] = HASH_PICK(sip6->s6_addr[2], 0, 8, 2) | HASH_PICK(sip6->s6_addr[3], 6, 2, 0);
	rows[12] = HASH_PICK(sip6->s6_addr[3], 0, 6, 4) | HASH_PICK(sip6->s6_addr[4], 4, 4, 0);
	rows[13] = HASH_PICK(sip6->s6_addr[4], 0, 4, 6) | HASH_PICK(sip6->s6_addr[5], 2, 6, 0);
	rows[14] = HASH_PICK(sip6->s6_addr[5], 0, 2, 8) | HASH_PICK(sip6->s6_addr[6], 0, 8, 0);
	rows[15] = HASH_PICK(sip6->s6_addr[7], 0, 8, 2) | HASH_PICK(sip6->s6_addr[8], 6, 2, 0);
	rows[16] = HASH_PICK(sip6->s6_addr[8], 0, 6, 4) | HASH_PICK(sip6->s6_addr[9], 4, 4, 0);
	rows[17] = HASH_PICK(sip6->s6_addr[9], 0, 4, 6) | HASH_PICK(sip6->s6_addr[10], 2, 6, 0);
	rows[18] = HASH_PICK(sip6->s6_addr[10], 0, 2, 8) | HASH_PICK(sip6->s6_addr[11], 0, 8, 0);

	rows[19] = HASH_PICK(dip6->s6_addr[0], 0, 8, 2) | HASH_PICK(dip6->s6_addr[1], 6, 2, 0);
	rows[20] = HASH_PICK(dip6->s6_addr[1], 0, 6, 4) | HASH_PICK(dip6->s6_addr[2], 4, 4, 0);
	rows[21] = HASH_PICK(dip6->s6_addr[2], 0, 4, 6) | HASH_PICK(dip6->s6_addr[3], 2, 6, 0);
	rows[22] = HASH_PICK(dip6->s6_addr[3], 0, 2, 8) | HASH_PICK(dip6->s6_addr[4], 0, 8, 0);
	rows[23] = HASH_PICK(dip6->s6_addr[5], 0, 8, 2) | HASH_PICK(dip6->s6_addr[6], 6, 2, 0);
	rows[24] = HASH_PICK(dip6->s6_addr[6], 0, 6, 4) | HASH_PICK(dip6->s6_addr[7], 4, 4, 0);
	rows[25] = HASH_PICK(dip6->s6_addr[7], 0, 4, 6) | HASH_PICK(dip6->s6_addr[8], 2, 6, 0);
	rows[26] = HASH_PICK(dip6->s6_addr[8], 0, 2, 8) | HASH_PICK(dip6->s6_addr[9], 0, 8, 0);
	rows[27] = HASH_PICK(dip6->s6_addr[10], 0, 8, 2) | HASH_PICK(dip6->s6_addr[11], 6, 2, 0);

	rows[28] = HASH_PICK(dip6->s6_addr[11], 0, 6, 4) | HASH_PICK(vid, 8, 4, 0);

	rows[29] = HASH_PICK(vid, 0, 8, 2);

	s0 = rows[0] + rows[1];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[2];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[3];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[4];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[5];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[6];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);
	s0 += rows[7];
	s0 = (s0 & 0x3FF) + ((s0 & 0xFFFFFC00)? 1 : 0);

	hash = s0 ^ rows[8] ^ rows[9] ^ rows[10] ^ rows[11] ^ rows[12]
		^ rows[13] ^ rows[14] ^ rows[15] ^ rows[16] ^ rows[17]
		^ rows[18] ^ rows[19] ^ rows[20] ^ rows[21] ^ rows[22]
		^ rows[23] ^ rows[24] ^ rows[25] ^ rows[26] ^ rows[27] ^ rows[28];

	return hash;
}

/*
 * Read a prefix route entry from the L3_PREFIX_ROUTE_IPUC table
 * We currently only support IPv4 and IPv6 unicast route
 */
static void rtl931x_route_read(int idx, struct rtl83xx_route *rt)
{
	u32 v, ip4_m;
	bool host_route, default_route;
	struct in6_addr ip6_m;
	// Read L3_PREFIX_ROUTE_IPUC table (4) via register RTL9310_TBL_2
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 4);

	rtl_table_read(r, idx);
	// The table has a size of 6 registers
	rt->attr.valid = !!(sw_r32(rtl_table_data(r, 0)) & BIT(31));
	if (!rt->attr.valid)
		goto out;

	// Is this actually an OpenFlow entry from an FT_L3_TCAM table?
	if (sw_r32(rtl_table_data(r, 0)) & BIT(30)) {
		pr_info("%s: Error, got OpenFlow entry from FT_L3_TCAM\n", __func__);
		rt->attr.valid = false;
		goto out;
	}

	rt->attr.type = (sw_r32(rtl_table_data(r, 0)) >> 28) & 0x3;

	v = sw_r32(rtl_table_data(r, 3));
	host_route = !!(v & BIT(22));
	default_route = !!(v & BIT(21));
	rt->prefix_len = -1;
	pr_info("%s: host route %d, default_route %d\n", __func__, host_route, default_route);

	switch (rt->attr.type) {
	case ROUTE_TYPE_IPV4UC:
		rt->dst_ip = (sw_r32(rtl_table_data(r, 0)) << 12) 
				| (sw_r32(rtl_table_data(r, 1)) >> 20);

		ip4_m = sw_r32(rtl_table_data(r, 9));
		pr_info("%s: Read ip4 mask: %08x\n", __func__, ip4_m);
		rt->prefix_len = host_route ? 32 : -1;
		rt->prefix_len = (rt->prefix_len < 0 && default_route) ? 0 : -1;
		if (rt->prefix_len < 0)
			rt->prefix_len = inet_mask_len(ip4_m);
		break;
	case ROUTE_TYPE_IPV6UC:
		ipv6_addr_set(&rt->dst_ip6,
			      sw_r32(rtl_table_data(r, 1)), sw_r32(rtl_table_data(r, 2)),
			      sw_r32(rtl_table_data(r, 3)), sw_r32(rtl_table_data(r, 4)));
		ipv6_addr_set(&ip6_m,
			      sw_r32(rtl_table_data(r, 6)), sw_r32(rtl_table_data(r, 7)),
			      sw_r32(rtl_table_data(r, 8)), sw_r32(rtl_table_data(r, 9)));
		rt->prefix_len = host_route ? 128 : 0;
		rt->prefix_len = (rt->prefix_len < 0 && default_route) ? 0 : -1;
		if (rt->prefix_len < 0)
			rt->prefix_len = find_last_bit((unsigned long int *)&ip6_m.s6_addr32,
							 128);
		break;
	case ROUTE_TYPE_IPV4MC:
	case ROUTE_TYPE_IPV6MC:
		pr_warn("%s: route type not supported\n", __func__);
		goto out;
	}

	rt->attr.hit = !!(v & BIT(22));
	rt->attr.action = (v >> 18) & 3;
	rt->nh.id = (v >> 7) & 0x7ff;
	rt->attr.ttl_dec = !!(v & BIT(6));
	rt->attr.ttl_check = !!(v & BIT(5));
	rt->attr.dst_null = !!(v & BIT(4));
	rt->attr.qos_as = !!(v & BIT(3));
	rt->attr.qos_prio =  v & 0x7;
	pr_info("%s: index %d is valid: %d\n", __func__, idx, rt->attr.valid);
	pr_info("%s: next_hop: %d, hit: %d, action :%d, ttl_dec %d, ttl_check %d, dst_null %d\n",
		__func__, rt->nh.id, rt->attr.hit, rt->attr.action,
		rt->attr.ttl_dec, rt->attr.ttl_check, rt->attr.dst_null);
	pr_info("%s: GW: %pI4, prefix_len: %d\n", __func__, &rt->dst_ip, rt->prefix_len);
out:
	rtl_table_release(r);
}

static void rtl931x_net6_mask(int prefix_len, struct in6_addr *ip6_m)
{
	int o, b;
	// Define network mask
	o = prefix_len >> 3;
	b = prefix_len & 0x7;
	memset(ip6_m->s6_addr, 0xff, o);
	ip6_m->s6_addr[o] |= b ? 0xff00 >> b : 0x00;
}

/*
 * Read a host route entry from the table using its index
 * We currently only support IPv4 and IPv6 unicast route
 */
static void rtl931x_host_route_read(int idx, struct rtl83xx_route *rt)
{
	u32 v, ipv6[4];
	// Read L3_HOST_ROUTE_IPUC table (3) via register RTL9310_TBL_2
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 3);

	idx = ((idx / 6) * 8) + (idx % 6);

	pr_debug("In %s, physical index %d\n", __func__, idx);
	rtl_table_read(r, idx);
	// The table has a size of 4 registers
	rt->attr.valid = !!(sw_r32(rtl_table_data(r, 0)) & BIT(31));
	if (!rt->attr.valid)
		goto out;

	// Is this actually an OpenFlow entry from an FT_L3_TCAM table?
	if (sw_r32(rtl_table_data(r, 0)) & BIT(30)) {
		pr_info("%s: Error, got OpenFlow entry from FT_L3_TCAM\n", __func__);
		rt->attr.valid = false;
		goto out;
	}

	rt->attr.type = (sw_r32(rtl_table_data(r, 0)) >> 28) & 0x3;

	v = sw_r32(rtl_table_data(r, 1));
	rt->attr.dst_null = !!(v & BIT(10));
	rt->attr.action = (v >> 7) & 3;
	rt->attr.ecmp_en = !!(v & BIT(6));
	rt->nh.id = (v & 0x3f) << 6;

	v = sw_r32(rtl_table_data(r, 2));
	rt->nh.id |= v >> 25;
	rt->attr.ttl_dec = !!(v & BIT(24));
	rt->attr.ttl_check = !!(v & BIT(23));
	rt->attr.qos_as = !!(v & BIT(22));
	rt->attr.qos_prio =  (v >> 19) & 0x7;

	v = sw_r32(rtl_table_data(r, 3));
	rt->attr.hit = !!(v & BIT(15));

	switch (rt->attr.type) {
	case ROUTE_TYPE_IPV4UC:
		rt->dst_ip = sw_r32(rtl_table_data(r, 0)) << 12;
		rt->dst_ip |= sw_r32(rtl_table_data(r, 1)) >> 20;
		break;

	case ROUTE_TYPE_IPV6UC:
		ipv6[0] = sw_r32(rtl_table_data(r, 0)) << 12;
		ipv6[0] |= sw_r32(rtl_table_data(r, 1)) >> 20;

		rtl_table_read(r, idx + 1);
		ipv6[1] = sw_r32(rtl_table_data(r, 0)) << 4;
		ipv6[1] |= sw_r32(rtl_table_data(r, 1)) >> 28;
		ipv6[2] = (sw_r32(rtl_table_data(r, 1)) << 4) & 0xffff0000;

		rtl_table_read(r, idx + 2);
		ipv6[2] |= (sw_r32(rtl_table_data(r, 0)) >> 12) & 0xffff;
		ipv6[3] = sw_r32(rtl_table_data(r, 0)) << 20;
		ipv6[3] |= sw_r32(rtl_table_data(r, 1)) >> 12;

		ipv6_addr_set(&rt->dst_ip6, ipv6[0], ipv6[1], ipv6[2], ipv6[3]);

		break;

	case ROUTE_TYPE_IPV4MC:
	case ROUTE_TYPE_IPV6MC:
		pr_warn("%s: route type not supported\n", __func__);
		goto out;
	}

	
	pr_debug("%s: index %d is valid: %d\n", __func__, idx, rt->attr.valid);
	pr_debug("%s: next_hop: %d, hit: %d, action :%d, ttl_dec %d, ttl_check %d, dst_null %d\n",
		__func__, rt->nh.id, rt->attr.hit, rt->attr.action, rt->attr.ttl_dec, rt->attr.ttl_check,
		rt->attr.dst_null);
	pr_debug("%s: Destination: %pI4\n", __func__, &rt->dst_ip);

out:
	rtl_table_release(r);
}

/*
 * Write a host route entry from the table using its index
 * We currently only support IPv4 and IPv6 unicast route
 */
static void rtl931x_host_route_write(int idx, struct rtl83xx_route *rt)
{
	u32 v;
	/* Access L3_HOST_ROUTE_IPUC table (3) via register RTL9310_TBL_2
	 * the table has a size of 4 registers for IPv4 UC, MC and IPv6 use
	 * 2 additional entries that are only 2 registers long */
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 3);

	idx = ((idx / 6) * 8) + (idx % 6);

	pr_debug("%s: index %d is valid: %d\n", __func__, idx, rt->attr.valid);
	pr_debug("%s: next_hop: %d, hit: %d, action :%d, ttl_dec %d, ttl_check %d, dst_null %d\n",
		__func__, rt->nh.id, rt->attr.hit, rt->attr.action, rt->attr.ttl_dec, rt->attr.ttl_check,
		rt->attr.dst_null);
	pr_debug("%s: GW: %pI4, prefix_len: %d\n", __func__, &rt->dst_ip, rt->prefix_len);

	v = BIT(31); // Entry is valid
	v |= (rt->attr.type & 0x3) << 28;
	sw_w32(v, rtl_table_data(r, 0));

	v = rt->attr.dst_null ? BIT(10) : 0;
	v |= rt->attr.action & 0x3 << 7;
	v |= rt->attr.ecmp_en ? BIT(6) : 0;
	v |= rt->nh.id = (v & 0x3f) >> 6;
	sw_w32(v, rtl_table_data(r, 1));

	v = rt->nh.id << 25;
	v |= rt->attr.ttl_dec ? BIT(24) : 0;
	v |= rt->attr.ttl_check ? BIT(23) : 0;
	v |= rt->attr.qos_as ? BIT(22) : 0;
	v |= (rt->attr.qos_prio & 0x7) << 19;
	sw_w32(v, rtl_table_data(r, 2));

	switch (rt->attr.type) {
	case ROUTE_TYPE_IPV4UC:
		v = rt->dst_ip >> 12;
		sw_w32_mask(0xfffff, v, rtl_table_data(r, 0));
		v = rt->dst_ip << 20;
		sw_w32_mask(0xfff00000, v, rtl_table_data(r, 1));
		rtl_table_write(r, idx);
		break;

	case ROUTE_TYPE_IPV6UC:
		v = rt->dst_ip6.s6_addr32[0] >> 12;
		sw_w32_mask(0xfffff, v, sw_r32(rtl_table_data(r, 0)));
		v = rt->dst_ip6.s6_addr32[0] << 20;
		sw_w32_mask(0xfff00000, v, rtl_table_data(r, 1));
		rtl_table_write(r, idx);

		// idx + 1
		v = BIT(31); // Entry is valid
		v |= (rt->attr.type & 0x3) << 28;
		v |= rt->dst_ip6.s6_addr32[1] >> 4;  // 28 bit
		sw_w32(v, rtl_table_data(r, 0));

		v = rt->dst_ip6.s6_addr32[1] << 28;  // 4 bit
		v |= (rt->dst_ip6.s6_addr32[2] >> 4) & 0xfffff000; // 16 bit
		sw_w32(v, rtl_table_data(r, 1));
		rtl_table_write(r, idx + 1);

		// idx + 2
		v = BIT(31); // Entry is valid
		v |= (rt->attr.type & 0x3) << 28;
		v |= (rt->dst_ip6.s6_addr32[2] & 0xffff) << 12;
		v |= rt->dst_ip6.s6_addr32[3] >> 20;
		sw_w32(v, rtl_table_data(r, 0));

		v = rt->dst_ip6.s6_addr32[3] << 12;
		sw_w32(v, rtl_table_data(r, 1));
		rtl_table_write(r, idx + 2);

		break;

	case ROUTE_TYPE_IPV4MC:
	case ROUTE_TYPE_IPV6MC:
		pr_warn("%s: route type not supported\n", __func__);
		goto out;
	}

out:
	rtl_table_release(r);
}

/*
 * Look up the index of a prefix route in the routing table CAM for unicast IPv4/6 routes
 * using hardware offload.
 */
static int rtl931x_route_lookup_hw(struct rtl83xx_route *rt)
{
	u32 ip4_m, v;
	struct in6_addr ip6_m;
	int i;

	// Set entry type being searched, all other fields are 0
	sw_w32(rt->attr.type << 22, RTL931X_L3_HW_LU_KEY_CTRL);

	if (rt->attr.type == ROUTE_TYPE_IPV6UC || rt->attr.type == ROUTE_TYPE_IPV6MC) {
		sw_w32(0, RTL931X_L3_HW_LU_KEY_CTRL);
		rtl931x_net6_mask(rt->prefix_len, &ip6_m);
		for (i = 0; i < 4; i++)
			sw_w32(rt->dst_ip6.s6_addr32[i] & ip6_m.s6_addr32[i],
			       RTL931X_L3_HW_LU_KEY_DIP_CTRL + (i << 2));
	} else { // IPv4
		ip4_m = inet_make_mask(rt->prefix_len);
		sw_w32(0, RTL931X_L3_HW_LU_KEY_DIP_CTRL);
		sw_w32(0, RTL931X_L3_HW_LU_KEY_DIP_CTRL + 4);
		sw_w32(0, RTL931X_L3_HW_LU_KEY_DIP_CTRL + 8);
		v = rt->dst_ip & ip4_m;
		pr_info("%s: searching for %pI4\n", __func__, &v);
		sw_w32(v, RTL931X_L3_HW_LU_KEY_DIP_CTRL + 12);
	}

	// Execute CAM lookup in SoC
	sw_w32(BIT(15), RTL931X_L3_HW_LU_CTRL);

	// Wait until execute bit clears and result is ready
	do {
		v = sw_r32(RTL931X_L3_HW_LU_CTRL);
	} while (v & BIT(15));

	pr_info("%s: found: %d, index: %d\n", __func__, !!(v & BIT(14)), v & 0x1ff);

	// Test if search successful (BIT 14 set)
	if (v & BIT(14))
		return v & 0x1ff;

	return -1;
}

static int rtl931x_find_l3_slot(struct rtl83xx_route *rt, bool must_exist)
{
	int t, s, slot_width, algorithm, addr, idx;
	u32 hash;
	struct rtl83xx_route route_entry;

	// IPv6 entries take up 3 slots
	slot_width = (rt->attr.type == 0) || (rt->attr.type == 2) ? 1 : 3;

	for (t = 0; t < 2; t++) {
		algorithm = sw_r32(RTL931X_L3_HOST_TBL_CTRL) & BIT(2 + t);
		
		if (algorithm)
			hash = rtl931x_l3_hash4_1(0, rt->dst_ip, 0, 0);
		else
			hash = rtl931x_l3_hash4_0(0, rt->dst_ip, 0, 0);

		pr_info("%s: table %d, algorithm %d, hash %04x\n", __func__, t, algorithm, hash);

		for (s = 0; s < 6; s += slot_width) {
			addr = (t << 12) | ((hash & 0x1ff) << 3) | s;
			pr_info("%s physical address %d\n", __func__, addr);
			idx = ((addr / 8) * 6) + (addr % 8);
			pr_info("%s logical address %d\n", __func__, idx);

			rtl931x_host_route_read(idx, &route_entry);
			pr_info("%s route valid %d, route dest: %pI4, hit %d\n", __func__,
				rt->attr.valid, &rt->dst_ip, rt->attr.hit);
			if (!must_exist && rt->attr.valid)
				return idx;
			if (must_exist && route_entry.dst_ip == rt->dst_ip)
				return idx;
		}
	}

	return -1;
}

/*
 * Write a prefix route into the routing table CAM at position idx
 * Currently only IPv4 and IPv6 unicast routes are supported
 */
static void rtl931x_route_write(int idx, struct rtl83xx_route *rt)
{
	u32 head, head_bmsk, attr, v, ip4_m;
	struct in6_addr ip6_m;
	// Access L3_PREFIX_ROUTE_IPUC table (4) via register RTL9310_TBL_2
	// The table has a size of 6 registers for IPv4 UC (6 + 4 + 4 for IPv6)
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 4);

	pr_debug("%s: index %d is valid: %d\n", __func__, idx, rt->attr.valid);
	pr_debug("%s: nexthop: %d, hit: %d, action :%d, ttl_dec %d, ttl_check %d, dst_null %d\n",
		__func__, rt->nh.id, rt->attr.hit, rt->attr.action,
		rt->attr.ttl_dec, rt->attr.ttl_check, rt->attr.dst_null);
	pr_debug("%s: GW: %pI4, prefix_len: %d\n", __func__, &rt->dst_ip, rt->prefix_len);

	head = rt->attr.valid ? BIT(31) : 0;
	// BIT 30 (FMT) stays 0 for non-OpenFlow entries
	head |= (rt->attr.type & 0x3) << 28; // bit mask for entry type
	sw_w32(head | ((rt->attr.vrf_id & 0xff) << 20), rtl_table_data(r, 0));

	head_bmsk = BIT(10) | 0x3 << 8; // set BMSK_FMT to 0x1 and BMSK_ENTRY_TYPE to 0x3

	// Clear word 5 including HIT bit (bit 27)
	sw_w32(0, rtl_table_data(r, 5));
	
	attr = rt->prefix_len == 0 ? BIT(21) : 0; // set default route bit
	attr |= rt->attr.dst_null ? BIT(20) : 0;
	attr |= (rt->attr.action & 0x3) << 17;
	attr |= rt->attr.ecmp_en ? BIT(16) : 0;
	attr |= (rt->nh.id & 0x1fff) << 3;	// NH_ECMP_IDX
	attr |= rt->attr.ttl_dec ? BIT(2) : 0;
	attr |= rt->attr.ttl_check ? BIT(1) : 0;
	attr |= rt->attr.qos_as ? BIT(0) : 0;
	attr |= rt->attr.qos_prio & 0x7;

	switch (rt->attr.type) {
	case ROUTE_TYPE_IPV4UC:
		sw_w32_mask(0x000fffff, rt->dst_ip >> 12, rtl_table_data(r, 0));
		sw_w32(rt->dst_ip << 20 | head_bmsk, rtl_table_data(r, 1));

		attr |= rt->prefix_len == 32 ? BIT(22) : 0; // set host-route bit
		sw_w32(ip4_m, rtl_table_data(r, 2));
		sw_w32(attr, rtl_table_data(r, 3));
		pr_info("%s: %08x %08x %08x %08x %08x %08x\n", __func__,
			sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)),
			sw_r32(rtl_table_data(r, 2)), sw_r32(rtl_table_data(r, 3)),
			sw_r32(rtl_table_data(r, 4)), sw_r32(rtl_table_data(r, 5)));
		rtl_table_write(r, idx);

		break;

	case ROUTE_TYPE_IPV6UC:
		// Write idx
		rtl931x_net6_mask(rt->prefix_len, &ip6_m);

		sw_w32_mask(0x000fffff, rt->dst_ip6.s6_addr32[0] >> 12, rtl_table_data(r, 0));
		sw_w32(rt->dst_ip6.s6_addr32[0] << 20 | head_bmsk, rtl_table_data(r, 1));

		sw_w32(ip6_m.s6_addr32[0], rtl_table_data(r, 2));

		attr |= rt->prefix_len == 128 ? BIT(21) : 0; // set host-route bit
		sw_w32(attr, rtl_table_data(r, 3));
		rtl_table_write(r, idx);

		pr_info("%s: %08x %08x %08x %08x %08x %08x\n", __func__,
			sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)),
			sw_r32(rtl_table_data(r, 2)), sw_r32(rtl_table_data(r, 3)),
			sw_r32(rtl_table_data(r, 4)), sw_r32(rtl_table_data(r, 5)));

		// Write idx + 1 (only 4 registers wide)
		v = rt->dst_ip6.s6_addr32[1] >> 4;
		sw_w32(head | v, rtl_table_data(r, 0));

		v = rt->dst_ip6.s6_addr32[1] << 28;
		v |= rt->dst_ip6.s6_addr32[2] >> 4;
		v &= 0xfffff000;
		v |= BIT(10) | (0x3 << 8); // BMSK_FMT | BMSK_ENTRY_TYPE
		v |= ip6_m.s6_addr32[1] >> 24;
		sw_w32(v, rtl_table_data(r, 1));

		v = ip6_m.s6_addr32[1] << 8;
		v |= ip6_m.s6_addr32[2] >> 24;
		sw_w32(v, rtl_table_data(r, 2));

		v = ip6_m.s6_addr32[2] << 24;
		sw_w32(v, rtl_table_data(r, 3));
		pr_info("%s: %08x %08x %08x %08x\n", __func__,
			sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)),
			sw_r32(rtl_table_data(r, 2)), sw_r32(rtl_table_data(r, 3)));
		rtl_table_write(r, idx + 1);

		// Write idx + 2 (only 4 registers wide)
		v = (rt->dst_ip6.s6_addr32[2] & 0xffff) << 12;
		v |= rt->dst_ip6.s6_addr32[3] >> 20;
		sw_w32(head | v, rtl_table_data(r, 0));

		v = rt->dst_ip6.s6_addr32[3] << 20;
		v |= BIT(10) | (0x3 << 8); // BMSK_FMT | BMSK_ENTRY_TYPE
		v |= ip6_m.s6_addr32[2] >> 24;
		sw_w32(v, rtl_table_data(r, 1));

		v = ip6_m.s6_addr32[2] << 16;
		v |= ip6_m.s6_addr32[3] >> 16;
		sw_w32(v, rtl_table_data(r, 2));

		v = ip6_m.s6_addr32[3] << 24;
		sw_w32(v, rtl_table_data(r, 3));

		pr_info("%s: %08x %08x %08x %08x\n", __func__,
			sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)),
			sw_r32(rtl_table_data(r, 2)), sw_r32(rtl_table_data(r, 3)));
		rtl_table_write(r, idx + 2);

		break;

	case ROUTE_TYPE_IPV4MC:
	case ROUTE_TYPE_IPV6MC:
		pr_warn("%s: route type not supported\n", __func__);
		rtl_table_release(r);
		return;
	}
	sw_w32(attr, rtl_table_data(r, 3));

	pr_info("%s: %08x %08x %08x %08x %08x %08x\n", __func__,
		sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)), sw_r32(rtl_table_data(r, 2)),
		sw_r32(rtl_table_data(r, 3)), sw_r32(rtl_table_data(r, 4)), sw_r32(rtl_table_data(r, 5)));

	rtl_table_release(r);
}

/*
 * Get the destination MAC and L3 egress interface ID of a nexthop entry from
 * the SoC's L3_NEXTHOP table
 */
static void rtl931x_get_l3_nexthop(int idx, u16 *dmac_id, u16 *interface)
{
	u32 v;
	// Read L3_NEXTHOP table (6) via register RTL9310_TBL_2
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 6);

	rtl_table_read(r, idx);
	// The table has a size of 1 register
	v = sw_r32(rtl_table_data(r, 0));
	rtl_table_release(r);

	*dmac_id = v >> 16;
	*interface = (v >> 6) & 0x3ff;
}

static int rtl931x_l3_mtu_add(struct rtl838x_switch_priv *priv, int mtu)
{
	int i, free_mtu;
	int mtu_id;

	// Try to find an existing mtu-value or a free slot
	free_mtu = MAX_INTF_MTUS;
	for (i = 0; i < MAX_INTF_MTUS && priv->intf_mtus[i] != mtu; i++) {
		if ((!priv->intf_mtu_count[i]) && (free_mtu == MAX_INTF_MTUS))
			free_mtu = i;
	}
	i = (i < MAX_INTF_MTUS) ? i : free_mtu;
	if (i < MAX_INTF_MTUS) {
		mtu_id = i;
	} else {
		pr_err("%s: No free MTU slot available!\n", __func__);
		return -EINVAL;
	}

	priv->intf_mtus[i] = mtu;
	pr_info("Writing MTU %d to slot %d\n", priv->intf_mtus[i], i);
	// Set MTU-value of the slot TODO: distinguish between IPv4/IPv6 routes / slots
	// TODO: MAX_INTF_MTUS on the RTL9310 is actually 16, not 8 like on the RTL9300
	sw_w32(priv->intf_mtus[i], RTL931X_L3_INTF_IP_MTU(i));
	sw_w32(priv->intf_mtus[i], RTL931X_L3_INTF_IP6_MTU(i));

	priv->intf_mtu_count[i]++;

	return mtu_id;
}

/*
 * Set the destination MAC and L3 egress interface ID for a nexthop entry in the SoC's
 * L3_NEXTHOP table. The nexthop entry is identified by idx.
 * dmac_id is the reference to the L2 entry in the L2 forwarding table
 * TODO: Are there special values like for RTL9300?
 */
static void rtl931x_set_l3_nexthop(int idx, u16 dmac_id, u16 interface)
{
	u32 v;

	// Access L3_NEXTHOP table (3) via register RTL9300_TBL_1
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 6);

	pr_info("%s: Writing to L3_NEXTHOP table, index %d, dmac_id %d, interface %d\n",
		__func__, idx, dmac_id, interface);
	v = dmac_id << 16;
	v |= (interface & 0x3ff) << 6;
	sw_w32(v, rtl_table_data(r, 0));

	pr_info("%s: %08x\n", __func__, sw_r32(rtl_table_data(r,0)));
	rtl_table_write(r, idx);
	rtl_table_release(r);
}

static void rtl930x_pie_lookup_enable(struct rtl838x_switch_priv *priv, int index)
{
	int block = index / PIE_BLOCK_SIZE;

	sw_w32_mask(0, BIT(block), RTL930X_PIE_BLK_LOOKUP_CTRL);
}

static u64 rtl931x_l2_hash_seed(u64 mac, u32 vid)
{
	u64 v = vid;

	v <<= 48;
	v |= mac;

	return v;
}

/*
 * Calculate both the block 0 and the block 1 hash by applyingthe same hash
 * algorithm as the one used currently by the ASIC to the seed, and return
 * both hashes in the lower and higher word of the return value since only 12 bit of
 * the hash are significant.
 */
static u32 rtl931x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 h, h0, h1, h2, h3, h4, k0, k1;

	h0 = seed & 0xfff;
	h1 = (seed >> 12) & 0xfff;
	h2 = (seed >> 24) & 0xfff;
	h3 = (seed >> 36) & 0xfff;
	h4 = (seed >> 48) & 0xfff;
	h4 = ((h4 & 0x7) << 9) | ((h4 >> 3) & 0x1ff);
	k0 = h0 ^ h1 ^ h2 ^ h3 ^ h4;

	h0 = seed & 0xfff;
	h0 = ((h0 & 0x1ff) << 3) | ((h0 >> 9) & 0x7);
	h1 = (seed >> 12) & 0xfff;
	h1 = ((h1 & 0x3f) << 6) | ((h1 >> 6) & 0x3f);
	h2 = (seed >> 24) & 0xfff;
	h3 = (seed >> 36) & 0xfff;
	h3 = ((h3 & 0x3f) << 6) | ((h3 >> 6) & 0x3f);
	h4 = (seed >> 48) & 0xfff;
	k1 = h0 ^ h1 ^ h2 ^ h3 ^ h4;

	// Algorithm choice for block 0
	if (sw_r32(RTL931X_L2_CTRL) & BIT(0))
		h = k1;
	else
		h = k0;

	/* Algorithm choice for block 1
	 * Since k0 and k1 are < 4096, adding 4096 will offset the hash into the second
	 * half of hash-space
	 * 4096 is in fact the hash-table size 32768 divided by 4 hashes per bucket
	 * divided by 2 to divide the hash space in 2
	 */
	if (sw_r32(RTL931X_L2_CTRL) & BIT(1))
		h |= (k1 + 4096) << 16;
	else
		h |= (k0 + 4096) << 16;

	return h;
}

/*
 * Fills an L2 entry structure from the SoC registers
 */
static void rtl931x_fill_l2_entry(u32 r[], struct rtl838x_l2_entry *e)
{
	pr_debug("In %s valid?\n", __func__);
	e->valid = !!(r[0] & BIT(31));
	if (!e->valid)
		return;

	pr_debug("%s: entry valid, raw: %08x %08x %08x %08x\n", __func__, r[0], r[1], r[2], r[3]);
	e->is_ip_mc = false;
	e->is_ipv6_mc = false;

	e->mac[0] = r[0] >> 8;
	e->mac[1] = r[0];
	e->mac[2] = r[1] >> 24;
	e->mac[3] = r[1] >> 16;
	e->mac[4] = r[1] >> 8;
	e->mac[5] = r[1];

	e->is_open_flow = !!(r[0] & BIT(30));
	e->is_pe_forward = !!(r[0] & BIT(29));
	e->next_hop = !!(r[2] & BIT(30));
	e->rvid = (r[0] >> 16) & 0xfff;

	/* Is it a unicast entry? check multicast bit */
	if (!(e->mac[0] & 1)) {
		e->type = L2_UNICAST;
		e->is_l2_tunnel = !!(r[2] & BIT(31));
		e->is_static = !!(r[2] & BIT(13));
		e->port = (r[2] >> 19) & 0x3ff;
		// Check for trunk port
		if (r[2] & BIT(29)) {
			e->is_trunk = true;
			e->stack_dev = (e->port >> 9) & 1;
			e->trunk = e->port & 0x3f;
		} else {
			e->is_trunk = false;
			e->stack_dev = (e->port >> 6) & 0xf;
			e->port = e->port & 0x3f;
		}

		e->block_da = !!(r[2] & BIT(14));
		e->block_sa = !!(r[2] & BIT(15));
		e->suspended = !!(r[2] & BIT(12));
		e->age = (r[2] >> 16) & 3;

		// the UC_VID field in hardware is used for the VID or for the route id
		if (e->next_hop) {
			e->nh_route_id = r[2] & 0x7ff;
			e->vid = 0;
		} else {
			e->vid = r[2] & 0xfff;
			e->nh_route_id = 0;
		}
		if (e->is_l2_tunnel)
			e->l2_tunnel_id = ((r[2] & 0xff) << 4) | (r[3] >> 28);
		// TODO: Implement VLAN conversion
	} else {
		e->type = L2_MULTICAST;
		e->is_local_forward = !!(r[2] & BIT(31));
		e->is_remote_forward = !!(r[2] & BIT(17));
		e->mc_portmask_index = (r[2] >> 18) & 0xfff;
		e->l2_tunnel_list_id = (r[2] >> 4) & 0x1fff;
	}
}

/*
 * Fills the 3 SoC table registers r[] with the information of in the rtl838x_l2_entry
 */
static void rtl931x_fill_l2_row(u32 r[], struct rtl838x_l2_entry *e)
{
	u32 port;

	if (!e->valid) {
		r[0] = r[1] = r[2] = 0;
		return;
	}

	r[2] = BIT(31);	// Set valid bit

	r[0] = ((u32)e->mac[0]) << 24 | ((u32)e->mac[1]) << 16
		| ((u32)e->mac[2]) << 8 | ((u32)e->mac[3]);
	r[1] = ((u32)e->mac[4]) << 24 | ((u32)e->mac[5]) << 16;

	r[2] |= e->next_hop ? BIT(12) : 0;

	if (e->type == L2_UNICAST) {
		r[2] |= e->is_static ? BIT(14) : 0;
		r[1] |= e->rvid & 0xfff;
		r[2] |= (e->port & 0x3ff) << 20;
		if (e->is_trunk) {
			r[2] |= BIT(30);
			port = e->stack_dev << 9 | (e->port & 0x3f);
		} else {
			port = (e->stack_dev & 0xf) << 6;
			port |= e->port & 0x3f;
		}
		r[2] |= port << 20;
		r[2] |= e->block_da ? BIT(15) : 0;
		r[2] |= e->block_sa ? BIT(17) : 0;
		r[2] |= e->suspended ? BIT(13) : 0;
		r[2] |= (e->age & 0x3) << 17;
		// the UC_VID field in hardware is used for the VID or for the route id
		if (e->next_hop)
			r[2] |= e->nh_route_id & 0x7ff;
		else
			r[2] |= e->vid & 0xfff;
	} else { // L2_MULTICAST
		r[2] |= (e->mc_portmask_index & 0x3ff) << 16;
		r[2] |= e->mc_mac_index & 0x7ff;
	}
}

/*
 * Read an L2 UC or MC entry out of a hash bucket of the L2 forwarding table
 * hash is the id of the bucket and pos is the position of the entry in that bucket
 * The data read from the SoC is filled into rtl838x_l2_entry
 */
static u64 rtl931x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[4];
	struct table_reg *q = rtl_table_get(RTL9310_TBL_0, 0);
	u32 idx;
	int i;
	u64 mac;
	u64 seed;

	pr_debug("%s: hash %08x, pos: %d\n", __func__, hash, pos);

	/* On the RTL93xx, 2 different hash algorithms are used making it a total of
	 * 8 buckets that need to be searched, 4 for each hash-half
	 * Use second hash space when bucket is between 4 and 8 */
	if (pos >= 4) {
		pos -= 4;
		hash >>= 16;
	} else {
		hash &= 0xffff;
	}

	idx = (0 << 14) | (hash << 2) | pos; // Search SRAM, with hash and at pos in bucket
	pr_debug("%s: NOW hash %08x, pos: %d\n", __func__, hash, pos);

	rtl_table_read(q, idx);
	for (i = 0; i < 4; i++)
		r[i] = sw_r32(rtl_table_data(q, i));

	rtl_table_release(q);

	rtl931x_fill_l2_entry(r, e);

	pr_debug("%s: valid: %d, nh: %d\n", __func__, e->valid, e->next_hop);
	if (!e->valid)
		return 0;

	mac = ((u64)e->mac[0]) << 40 | ((u64)e->mac[1]) << 32 | ((u64)e->mac[2]) << 24
		| ((u64)e->mac[3]) << 16 | ((u64)e->mac[4]) << 8 | ((u64)e->mac[5]);

	seed = rtl931x_l2_hash_seed(mac, e->rvid);
	pr_debug("%s: mac %016llx, seed %016llx\n", __func__, mac, seed);
	// return vid with concatenated mac as unique id
	return seed;
}

static u64 rtl931x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
		return 0;
}

static void rtl931x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
}

static void rtl931x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[4];
	struct table_reg *q = rtl_table_get(RTL9310_TBL_0, 0);
	u32 idx = (0 << 14) | (hash << 2) | pos; // Access SRAM, with hash and at pos in bucket
	int i;

	pr_info("%s: hash %d, pos %d\n", __func__, hash, pos);
	pr_info("%s: index %d -> mac %02x:%02x:%02x:%02x:%02x:%02x\n", __func__, idx,
		e->mac[0], e->mac[1], e->mac[2], e->mac[3],e->mac[4],e->mac[5]);

	rtl931x_fill_l2_row(r, e);
	pr_info("%s: %d: %08x %08x %08x\n", __func__, idx, r[0], r[1], r[2]);

	for (i= 0; i < 4; i++)
		sw_w32(r[i], rtl_table_data(q, i));

	rtl_table_write(q, idx);
	rtl_table_release(q);

}

static void rtl931x_vlan_fwd_on_inner(int port, bool is_set)
{
	// Always set all tag modes to fwd based on either inner or outer tag
	if (is_set)
		sw_w32_mask(0, 0xf, RTL931X_VLAN_PORT_FWD + (port << 2));
	else
		sw_w32_mask(0xf, 0, RTL931X_VLAN_PORT_FWD + (port << 2));
}

static void rtl931x_vlan_profile_setup(int profile)
{
	u32 p[7];
	int i;

	pr_info("In %s\n", __func__);

	if (profile > 15)
		return;

	p[0] = sw_r32(RTL931X_VLAN_PROFILE_SET(profile));

	// Enable routing of Ipv4/6 Unicast and IPv4/6 Multicast traffic
	//p[0] |= BIT(17) | BIT(16) | BIT(13) | BIT(12);
	p[0] |= 0x3 << 11; // COPY2CPU

	p[1] = 0x1FFFFFF; // L2 unknwon MC flooding portmask all ports, including the CPU-port
	p[2] = 0xFFFFFFFF;
	p[3] = 0x1FFFFFF; // IPv4 unknwon MC flooding portmask
	p[4] = 0xFFFFFFFF;
	p[5] = 0x1FFFFFF; // IPv6 unknwon MC flooding portmask
	p[6] = 0xFFFFFFFF;

	for (i = 0; i < 7; i++)
		sw_w32(p[i], RTL931X_VLAN_PROFILE_SET(profile) + i * 4);
	pr_info("Leaving %s\n", __func__);
}

static void rtl931x_l2_learning_setup(void)
{
	u32 v;
	// Portmask for flooding broadcast traffic
	rtl839x_set_port_reg_be(0x1FFFFFFFFFFFFFF, RTL931X_L2_BC_FLD_PMSK);

	// Portmask for flooding unicast traffic with unknown destination
	rtl839x_set_port_reg_be(0x1FFFFFFFFFFFFFF, RTL931X_L2_UNKN_UC_FLD_PMSK);

	// Limit learning to maximum: 64k entries, after that just flood (bits 0-2)
	sw_w32((0xffff << 3) | FORWARD, RTL931X_L2_LRN_CONSTRT_CTRL);

	/* Add CPU port to VLAN 1 */
	sw_w32(0x118001, 0x8500);
	v = sw_r32(0x8508);
	v |= 0x80000000;
	sw_w32(v, 0x8508);
	sw_w32(0x198001, 0x8500);
}

static u64 rtl931x_read_mcast_pmask(int idx)
{
	u64 portmask;
	// Read MC_PMSK (2) via register RTL9310_TBL_0
	struct table_reg *q = rtl_table_get(RTL9310_TBL_0, 2);

	rtl_table_read(q, idx);
	portmask = sw_r32(rtl_table_data(q, 0));
	portmask <<= 32;
	portmask |= sw_r32(rtl_table_data(q, 1));
	portmask >>= 7;
	rtl_table_release(q);

	pr_debug("%s: Index idx %d has portmask %016llx\n", __func__, idx, portmask);
	return portmask;
}

static void rtl931x_write_mcast_pmask(int idx, u64 portmask)
{
	u64 pm = portmask;

	// Access MC_PMSK (2) via register RTL9310_TBL_0
	struct table_reg *q = rtl_table_get(RTL9310_TBL_0, 2);

	pr_debug("%s: Index idx %d has portmask %016llx\n", __func__, idx, pm);
	pm <<= 7;
	sw_w32((u32)(pm >> 32), rtl_table_data(q, 0));
	sw_w32((u32)pm, rtl_table_data(q, 1));
	rtl_table_write(q, idx);
	rtl_table_release(q);
}


static int rtl931x_set_ageing_time(unsigned long msec)
{
	int t = sw_r32(RTL931X_L2_AGE_CTRL);

	t &= 0x1FFFFF;
	t = (t * 8) / 10;
	pr_debug("L2 AGING time: %d sec\n", t);

	t = (msec / 100 + 7) / 8;
	t = t > 0x1FFFFF ? 0x1FFFFF : t;
	sw_w32_mask(0x1FFFFF, t, RTL931X_L2_AGE_CTRL);
	pr_debug("Dynamic aging for ports: %x\n", sw_r32(RTL931X_L2_PORT_AGE_CTRL));
	return 0;
}
void rtl931x_sw_init(struct rtl838x_switch_priv *priv)
{
//	rtl931x_sds_init(priv);
}

static void rtl931x_pie_lookup_enable(struct rtl838x_switch_priv *priv, int index)
{
	int block = index / PIE_BLOCK_SIZE;

	sw_w32_mask(0, BIT(block), RTL931X_PIE_BLK_LOOKUP_CTRL);
}

/*
 * Fills the data in the intermediate representation in the pie_rule structure
 * into a data field for a given template field field_type
 * TODO: This function looks very similar to the function of the rtl9300, but
 * since it uses the physical template_field_id, which are different for each
 * SoC and there are other field types, it is actually not. If we would also use
 * an intermediate representation for a field type, we would could have one
 * pie_data_fill function for all SoCs, provided we have also for each SoC a
 * function to map between physical and intermediate field type
 */
int rtl931x_pie_data_fill(enum template_field_id field_type, struct pie_rule *pr, u16 *data, u16 *data_m)
{
	*data = *data_m = 0;

	switch (field_type) {
	case TEMPLATE_FIELD_SPM0:
		*data = pr->spm;
		*data_m = pr->spm_m;
		break;
	case TEMPLATE_FIELD_SPM1:
		*data = pr->spm >> 16;
		*data_m = pr->spm_m >> 16;
		break;
	case TEMPLATE_FIELD_OTAG:
		*data = pr->otag;
		*data_m = pr->otag_m;
		break;
	case TEMPLATE_FIELD_SMAC0:
		*data = pr->smac[4];
		*data = (*data << 8) | pr->smac[5];
		*data_m = pr->smac_m[4];
		*data_m = (*data_m << 8) | pr->smac_m[5];
		break;
	case TEMPLATE_FIELD_SMAC1:
		*data = pr->smac[2];
		*data = (*data << 8) | pr->smac[3];
		*data_m = pr->smac_m[2];
		*data_m = (*data_m << 8) | pr->smac_m[3];
		break;
	case TEMPLATE_FIELD_SMAC2:
		*data = pr->smac[0];
		*data = (*data << 8) | pr->smac[1];
		*data_m = pr->smac_m[0];
		*data_m = (*data_m << 8) | pr->smac_m[1];
		break;
	case TEMPLATE_FIELD_DMAC0:
		*data = pr->dmac[4];
		*data = (*data << 8) | pr->dmac[5];
		*data_m = pr->dmac_m[4];
		*data_m = (*data_m << 8) | pr->dmac_m[5];
		break;
	case TEMPLATE_FIELD_DMAC1:
		*data = pr->dmac[2];
		*data = (*data << 8) | pr->dmac[3];
		*data_m = pr->dmac_m[2];
		*data_m = (*data_m << 8) | pr->dmac_m[3];
		break;
	case TEMPLATE_FIELD_DMAC2:
		*data = pr->dmac[0];
		*data = (*data << 8) | pr->dmac[1];
		*data_m = pr->dmac_m[0];
		*data_m = (*data_m << 8) | pr->dmac_m[1];
		break;
	case TEMPLATE_FIELD_ETHERTYPE:
		*data = pr->ethertype;
		*data_m = pr->ethertype_m;
		break;
	case TEMPLATE_FIELD_ITAG:
		*data = pr->itag;
		*data_m = pr->itag_m;
		break;
	case TEMPLATE_FIELD_SIP0:
		if (pr->is_ipv6) {
			*data = pr->sip6.s6_addr16[7];
			*data_m = pr->sip6_m.s6_addr16[7];
		} else {
			*data = pr->sip;
			*data_m = pr->sip_m;
		}
		break;
	case TEMPLATE_FIELD_SIP1:
		if (pr->is_ipv6) {
			*data = pr->sip6.s6_addr16[6];
			*data_m = pr->sip6_m.s6_addr16[6];
		} else {
			*data = pr->sip >> 16;
			*data_m = pr->sip_m >> 16;
		}
		break;
	case TEMPLATE_FIELD_SIP2:
	case TEMPLATE_FIELD_SIP3:
	case TEMPLATE_FIELD_SIP4:
	case TEMPLATE_FIELD_SIP5:
	case TEMPLATE_FIELD_SIP6:
	case TEMPLATE_FIELD_SIP7:
		*data = pr->sip6.s6_addr16[5 - (field_type - TEMPLATE_FIELD_SIP2)];
		*data_m = pr->sip6_m.s6_addr16[5 - (field_type - TEMPLATE_FIELD_SIP2)];
		break;

	case TEMPLATE_FIELD_DIP0:
		if (pr->is_ipv6) {
			*data = pr->dip6.s6_addr16[7];
			*data_m = pr->dip6_m.s6_addr16[7];
		} else {
			*data = pr->dip;
			*data_m = pr->dip_m;
		}
		break;
		case TEMPLATE_FIELD_DIP1:
		if (pr->is_ipv6) {
			*data = pr->dip6.s6_addr16[6];
			*data_m = pr->dip6_m.s6_addr16[6];
		} else {
			*data = pr->dip >> 16;
			*data_m = pr->dip_m >> 16;
		}
		break;

	case TEMPLATE_FIELD_DIP2:
	case TEMPLATE_FIELD_DIP3:
	case TEMPLATE_FIELD_DIP4:
	case TEMPLATE_FIELD_DIP5:
	case TEMPLATE_FIELD_DIP6:
	case TEMPLATE_FIELD_DIP7:
		*data = pr->dip6.s6_addr16[5 - (field_type - TEMPLATE_FIELD_DIP2)];
		*data_m = pr->dip6_m.s6_addr16[5 - (field_type - TEMPLATE_FIELD_DIP2)];
		break;

	case TEMPLATE_FIELD_IP_TOS_PROTO:
		*data = pr->tos_proto;
		*data_m = pr->tos_proto_m;
		break;
	case TEMPLATE_FIELD_L4_SPORT:
		*data = pr->sport;
		*data_m = pr->sport_m;
		break;
	case TEMPLATE_FIELD_L4_DPORT:
		*data = pr->dport;
		*data_m = pr->dport_m;
		break;
	case TEMPLATE_FIELD_DSAP_SSAP:
		*data = pr->dsap_ssap;
		*data_m = pr->dsap_ssap_m;
		break;
	case TEMPLATE_FIELD_TCP_INFO:
		*data = pr->tcp_info;
		*data_m = pr->tcp_info_m;
		break;
	case TEMPLATE_FIELD_RANGE_CHK:
		pr_info("TEMPLATE_FIELD_RANGE_CHK: not configured\n");
		break;
	default:
		pr_info("%s: unknown field %d\n", __func__, field_type);
		return -1;
	}

	return 0;
}

/*
 * Reads the intermediate representation of the templated match-fields of the
 * PIE rule in the pie_rule structure and fills in the raw data fields in the
 * raw register space r[].
 * The register space configuration size is identical for the RTL8380/90 and RTL9300,
 * however the RTL931X has 2 more registers / fields and the physical field-ids are different
 * on all SoCs
 * On the RTL9300 the mask fields are not word-aligend!
 */
static void rtl931x_write_pie_templated(u32 r[], struct pie_rule *pr, enum template_field_id t[])
{
	int i;
	u16 data, data_m;

	for (i = 0; i < N_FIXED_FIELDS; i++) {
		rtl931x_pie_data_fill(t[i], pr, &data, &data_m);

		// On the RTL9300, the mask fields are not word aligned!
		if (!(i % 2)) {
			r[5 - i / 2] = data;
			r[12 - i / 2] |= ((u32)data_m << 8);
		} else {
			r[5 - i / 2] |= ((u32)data) << 16;
			r[12 - i / 2] |= ((u32)data_m) << 24;
			r[11 - i / 2] |= ((u32)data_m) >> 8;
		}
	}
}

static void rtl931x_read_pie_fixed_fields(u32 r[], struct pie_rule *pr)
{
	pr->mgnt_vlan = r[7] & BIT(31);
	if (pr->phase == PHASE_IACL)
		pr->dmac_hit_sw = r[7] & BIT(30);
	else  // TODO: EACL/VACL phase handling
		pr->content_too_deep = r[7] & BIT(30);
	pr->not_first_frag = r[7]  & BIT(29);
	pr->frame_type_l4 = (r[7] >> 26) & 7;
	pr->frame_type = (r[7] >> 24) & 3;
	pr->otag_fmt = (r[7] >> 23) & 1;
	pr->itag_fmt = (r[7] >> 22) & 1;
	pr->otag_exist = (r[7] >> 21) & 1;
	pr->itag_exist = (r[7] >> 20) & 1;
	pr->frame_type_l2 = (r[7] >> 18) & 3;
	pr->igr_normal_port = (r[7] >> 17) & 1;
	pr->tid = (r[7] >> 16) & 1;

	pr->mgnt_vlan_m = r[14] & BIT(15);
	if (pr->phase == PHASE_IACL)
		pr->dmac_hit_sw_m = r[14] & BIT(14);
	else
		pr->content_too_deep_m = r[14] & BIT(14);
	pr->not_first_frag_m = r[14] & BIT(13);
	pr->frame_type_l4_m = (r[14] >> 10) & 7;
	pr->frame_type_m = (r[14] >> 8) & 3;
	pr->otag_fmt_m = r[14] & BIT(7);
	pr->itag_fmt_m = r[14] & BIT(6);
	pr->otag_exist_m = r[14] & BIT(5);
	pr->itag_exist_m = r[14] & BIT (4);
	pr->frame_type_l2_m = (r[14] >> 2) & 3;
	pr->igr_normal_port_m = r[14] & BIT(1);
	pr->tid_m = r[14] & 1;

	pr->valid = r[15] & BIT(31);
	pr->cond_not = r[15] & BIT(30);
	pr->cond_and1 = r[15] & BIT(29);
	pr->cond_and2 = r[15] & BIT(28);
}

static void rtl931x_write_pie_fixed_fields(u32 r[],  struct pie_rule *pr)
{
	r[7] |= pr->mgnt_vlan ? BIT(31) : 0;
	if (pr->phase == PHASE_IACL)
		r[7] |= pr->dmac_hit_sw ? BIT(30) : 0;
	else
		r[7] |= pr->content_too_deep ? BIT(30) : 0;
	r[7] |= pr->not_first_frag ? BIT(29) : 0;
	r[7] |= ((u32) (pr->frame_type_l4 & 0x7)) << 26;
	r[7] |= ((u32) (pr->frame_type & 0x3)) << 24;
	r[7] |= pr->otag_fmt ? BIT(23) : 0;
	r[7] |= pr->itag_fmt ? BIT(22) : 0;
	r[7] |= pr->otag_exist ? BIT(21) : 0;
	r[7] |= pr->itag_exist ? BIT(20) : 0;
	r[7] |= ((u32) (pr->frame_type_l2 & 0x3)) << 18;
	r[7] |= pr->igr_normal_port ? BIT(17) : 0;
	r[7] |= ((u32) (pr->tid & 0x1)) << 16;

	r[14] |= pr->mgnt_vlan_m ? BIT(15) : 0;
	if (pr->phase == PHASE_IACL)
		r[14] |= pr->dmac_hit_sw_m ? BIT(14) : 0;
	else
		r[14] |= pr->content_too_deep_m ? BIT(14) : 0;
	r[14] |= pr->not_first_frag_m ? BIT(13) : 0;
	r[14] |= ((u32) (pr->frame_type_l4_m & 0x7)) << 10;
	r[14] |= ((u32) (pr->frame_type_m & 0x3)) << 8;
	r[14] |= pr->otag_fmt_m ? BIT(7) : 0;
	r[14] |= pr->itag_fmt_m ? BIT(6) : 0;
	r[14] |= pr->otag_exist_m ? BIT(5) : 0;
	r[14] |= pr->itag_exist_m ? BIT(4) : 0;
	r[14] |= ((u32) (pr->frame_type_l2_m & 0x3)) << 2;
	r[14] |= pr->igr_normal_port_m ? BIT(1) : 0;
	r[14] |= (u32) (pr->tid_m & 0x1);

	r[15] |= pr->valid ? BIT(31) : 0;
	r[15] |= pr->cond_not ? BIT(30) : 0;
	r[15] |= pr->cond_and1 ? BIT(29) : 0;
	r[15] |= pr->cond_and2 ? BIT(28) : 0;
}

static void rtl931x_write_pie_action(u32 r[],  struct pie_rule *pr)
{
	// Either drop or forward
	if (pr->drop) {
		r[15] |= BIT(11) | BIT(12) | BIT(13); // Do Green, Yellow and Red drops
		// Actually DROP, not PERMIT in Green / Yellow / Red
		r[16] |= BIT(27) | BIT(28) | BIT(29);
	} else {
		r[15] |= pr->fwd_sel ? BIT(14) : 0;
		r[16] |= pr->fwd_act << 24;
		r[16] |= BIT(21); // We overwrite any drop
	}
	if (pr->phase == PHASE_VACL)
		r[16] |= pr->fwd_sa_lrn ? BIT(22) : 0;
	r[15] |= pr->bypass_sel ? BIT(10) : 0;
	r[15] |= pr->nopri_sel ? BIT(21) : 0;
	r[15] |= pr->tagst_sel ? BIT(20) : 0;
	r[15] |= pr->ovid_sel ? BIT(18) : 0;
	r[15] |= pr->ivid_sel ? BIT(16) : 0;
	r[15] |= pr->meter_sel ? BIT(27) : 0;
	r[15] |= pr->mir_sel ? BIT(15) : 0;
	r[15] |= pr->log_sel ? BIT(26) : 0;

	r[16] |= ((u32)(pr->fwd_data & 0xfff)) << 9;
//	r[15] |= pr->log_octets ? BIT(31) : 0;
	r[15] |= (u32)(pr->meter_data) >> 2;
	r[16] |= (((u32)(pr->meter_data) >> 7) & 0x3) << 29;

	r[16] |= ((u32)(pr->ivid_act & 0x3)) << 21;
	r[15] |= ((u32)(pr->ivid_data & 0xfff)) << 9;
	r[16] |= ((u32)(pr->ovid_act & 0x3)) << 30;
	r[16] |= ((u32)(pr->ovid_data & 0xfff)) << 16;
	r[16] |= ((u32)(pr->mir_data & 0x3)) << 6;
	r[17] |= ((u32)(pr->tagst_data & 0xf)) << 28;
	r[17] |= ((u32)(pr->nopri_data & 0x7)) << 25;
	r[17] |= pr->bypass_ibc_sc ? BIT(16) : 0;
}

void rtl931x_pie_rule_dump_raw(u32 r[])
{
	pr_info("Raw IACL table entry:\n");
	pr_info("r 0 - 7: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
	pr_info("r 8 - 15: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15]);
	pr_info("r 16 - 18: %08x %08x %08x\n", r[16], r[17], r[18]);
	pr_info("Match  : %08x %08x %08x %08x %08x %08x\n", r[0], r[1], r[2], r[3], r[4], r[5]);
	pr_info("Fixed  : %06x\n", r[6] >> 8);
	pr_info("Match M: %08x %08x %08x %08x %08x %08x\n",
		(r[6] << 24) | (r[7] >> 8), (r[7] << 24) | (r[8] >> 8), (r[8] << 24) | (r[9] >> 8),
		(r[9] << 24) | (r[10] >> 8), (r[10] << 24) | (r[11] >> 8),
		(r[11] << 24) | (r[12] >> 8));
	pr_info("R[13]:   %08x\n", r[13]);
	pr_info("Fixed M: %06x\n", ((r[12] << 16) | (r[13] >> 16)) & 0xffffff);
	pr_info("Valid / not / and1 / and2 : %1x\n", (r[13] >> 12) & 0xf);
	pr_info("r 13-16: %08x %08x %08x %08x\n", r[13], r[14], r[15], r[16]);
}

static int rtl931x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr)
{
	// Access IACL table (0) via register 1, the table size is 4096
	struct table_reg *q = rtl_table_get(RTL9310_TBL_1, 0);
	u32 r[22];
	int i;
	int block = idx / PIE_BLOCK_SIZE;
	u32 t_select = sw_r32(RTL931X_PIE_BLK_TMPLTE_CTRL(block));

	pr_info("%s: %d, t_select: %08x\n", __func__, idx, t_select);

	for (i = 0; i < 22; i++)
		r[i] = 0;

	if (!pr->valid) {
		rtl_table_write(q, idx);
		rtl_table_release(q);
		return 0;
	}
	rtl931x_write_pie_fixed_fields(r, pr);

	pr_info("%s: template %d\n", __func__, (t_select >> (pr->tid * 4)) & 0xf);
	rtl931x_write_pie_templated(r, pr, fixed_templates[(t_select >> (pr->tid * 4)) & 0xf]);

	rtl931x_write_pie_action(r, pr);

	rtl931x_pie_rule_dump_raw(r);

	for (i = 0; i < 22; i++)
		sw_w32(r[i], rtl_table_data(q, i));

	rtl_table_write(q, idx);
	rtl_table_release(q);

	return 0;
}

static bool rtl931x_pie_templ_has(int t, enum template_field_id field_type)
{
	int i;
	enum template_field_id ft;

	for (i = 0; i < N_FIXED_FIELDS_RTL931X; i++) {
		ft = fixed_templates[t][i];
		if (field_type == ft)
			return true;
	}

	return false;
}

/*
 * Verify that the rule pr is compatible with a given template t in block block
 * Note that this function is SoC specific since the values of e.g. TEMPLATE_FIELD_SIP0
 * depend on the SoC
 */
static int rtl931x_pie_verify_template(struct rtl838x_switch_priv *priv,
				       struct pie_rule *pr, int t, int block)
{
	int i;

	if (!pr->is_ipv6 && pr->sip_m && !rtl931x_pie_templ_has(t, TEMPLATE_FIELD_SIP0))
		return -1;

	if (!pr->is_ipv6 && pr->dip_m && !rtl931x_pie_templ_has(t, TEMPLATE_FIELD_DIP0))
		return -1;

	if (pr->is_ipv6) {
		if ((pr->sip6_m.s6_addr32[0] || pr->sip6_m.s6_addr32[1]
			|| pr->sip6_m.s6_addr32[2] || pr->sip6_m.s6_addr32[3])
			&& !rtl931x_pie_templ_has(t, TEMPLATE_FIELD_SIP2))
			return -1;
		if ((pr->dip6_m.s6_addr32[0] || pr->dip6_m.s6_addr32[1]
			|| pr->dip6_m.s6_addr32[2] || pr->dip6_m.s6_addr32[3])
			&& !rtl931x_pie_templ_has(t, TEMPLATE_FIELD_DIP2))
			return -1;
	}

	if (ether_addr_to_u64(pr->smac) && !rtl931x_pie_templ_has(t, TEMPLATE_FIELD_SMAC0))
		return -1;

	if (ether_addr_to_u64(pr->dmac) && !rtl931x_pie_templ_has(t, TEMPLATE_FIELD_DMAC0))
		return -1;

	// TODO: Check more

	i = find_first_zero_bit(&priv->pie_use_bm[block * 4], PIE_BLOCK_SIZE);

	if (i >= PIE_BLOCK_SIZE)
		return -1;

	return i + PIE_BLOCK_SIZE * block;
}

static int rtl931x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx, block, j, t;
	int min_block = 0;
	int max_block = priv->n_pie_blocks / 2;

	if (pr->is_egress) {
		min_block = max_block;
		max_block = priv->n_pie_blocks;
	}
	pr_info("In %s\n", __func__);

	mutex_lock(&priv->pie_mutex);

	for (block = min_block; block < max_block; block++) {
		for (j = 0; j < 2; j++) {
			t = (sw_r32(RTL931X_PIE_BLK_TMPLTE_CTRL(block)) >> (j * 4)) & 0xf;
			pr_info("Testing block %d, template %d, template id %d\n", block, j, t);
			pr_info("%s: %08x\n",
				__func__, sw_r32(RTL931X_PIE_BLK_TMPLTE_CTRL(block)));
			idx = rtl931x_pie_verify_template(priv, pr, t, block);
			if (idx >= 0)
				break;
		}
		if (j < 2)
			break;
	}

	if (block >= priv->n_pie_blocks) {
		mutex_unlock(&priv->pie_mutex);
		return -EOPNOTSUPP;
	}

	pr_info("Using block: %d, index %d, template-id %d\n", block, idx, j);
	set_bit(idx, priv->pie_use_bm);

	pr->valid = true;
	pr->tid = j;  // Mapped to template number
	pr->tid_m = 0x1;
	pr->id = idx;

	rtl931x_pie_lookup_enable(priv, idx);
	rtl931x_pie_rule_write(priv, idx, pr);

	mutex_unlock(&priv->pie_mutex);
	return 0;
}

/*
 * Delete a range of Packet Inspection Engine rules
 */
static int rtl931x_pie_rule_del(struct rtl838x_switch_priv *priv, int index_from, int index_to)
{
	u32 v = (index_from << 1)| (index_to << 13 ) | BIT(0);

	pr_info("%s: from %d to %d\n", __func__, index_from, index_to);
	mutex_lock(&priv->reg_mutex);

	// Write from-to and execute bit into control register
	sw_w32(v, RTL931X_PIE_CLR_CTRL);

	// Wait until command has completed
	do {
	} while (sw_r32(RTL931X_PIE_CLR_CTRL) & BIT(0));

	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static void rtl931x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx = pr->id;

	rtl931x_pie_rule_del(priv, idx, idx);
	clear_bit(idx, priv->pie_use_bm);
}

static void rtl931x_pie_init(struct rtl838x_switch_priv *priv)
{
	int i;
	u32 template_selectors;

	mutex_init(&priv->pie_mutex);

	pr_info("%s\n", __func__);
	// Enable ACL lookup on all ports, including CPU_PORT
	for (i = 0; i <= priv->cpu_port; i++)
		sw_w32(1, RTL931X_ACL_PORT_LOOKUP_CTRL(i));

	// Include IPG in metering
	sw_w32_mask(0, 1, RTL931X_METER_GLB_CTRL);

	// Delete all present rules, block size is 128 on all SoC families
	rtl931x_pie_rule_del(priv, 0, priv->n_pie_blocks * 128 - 1);

	// Assign first half blocks 0-7 to VACL phase, second half to IACL
	// 3 bits are used for each block, values for PIE blocks are
	// 6: Disabled, 0: VACL, 1: IACL, 2: EACL
	// And for OpenFlow Flow blocks: 3: Ingress Flow table 0,
	// 4: Ingress Flow Table 3, 5: Egress flow table 0
	for (i = 0; i < priv->n_pie_blocks; i++) {
		int pos = (i % 10) * 3;
		u32 r = RTL931X_PIE_BLK_PHASE_CTRL + 4 * (i / 10);

		if (i < priv->n_pie_blocks / 2)
			sw_w32_mask(0x7 << pos, 0, r);
		else
			sw_w32_mask(0x7 << pos, 1 << pos, r);
	}

	// Enable predefined templates 0, 1 for first quarter of all blocks
	template_selectors = 0 | (1 << 4);
	for (i = 0; i < priv->n_pie_blocks / 4; i++)
		sw_w32(template_selectors, RTL931X_PIE_BLK_TMPLTE_CTRL(i));

	// Enable predefined templates 2, 3 for second quarter of all blocks
	template_selectors = 2 | (3 << 4);
	for (i = priv->n_pie_blocks / 4; i < priv->n_pie_blocks / 2; i++)
		sw_w32(template_selectors, RTL931X_PIE_BLK_TMPLTE_CTRL(i));

	// Enable predefined templates 0, 1 for third quater of all blocks
	template_selectors = 0 | (1 << 4);
	for (i = priv->n_pie_blocks / 2; i < priv->n_pie_blocks * 3 / 4; i++)
		sw_w32(template_selectors, RTL931X_PIE_BLK_TMPLTE_CTRL(i));

	// Enable predefined templates 2, 3 for fourth quater of all blocks
	template_selectors = 2 | (3 << 4);
	for (i = priv->n_pie_blocks * 3 / 4; i < priv->n_pie_blocks; i++)
		sw_w32(template_selectors, RTL931X_PIE_BLK_TMPLTE_CTRL(i));

}

/*
 * Sets up an egress interface for L3 actions
 * Actions for ip4/6_icmp_redirect, ip4/6_pbr_icmp_redirect are:
 * 0: FORWARD, 1: DROP, 2: TRAP2CPU, 3: COPY2CPU, 4: TRAP2MASTERCPU 5: COPY2MASTERCPU
 * 6: HARDDROP
 * idx is the index in the HW interface table: idx < 0x80
 */
static void rtl931x_set_l3_egress_intf(int idx, struct rtl838x_l3_intf *intf)
{
	u32 v;
	// Access L3_EGR_INTF table (8) via register RTL9310_TBL_2
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 8);

	// The table has 4 registers
	v = (intf->vid & 0xfff) << 20;
	v |= (intf->smac) >> 28;
	sw_w32(v, rtl_table_data(r, 0));

	v = intf->smac << 4;
	v |= intf->ip4_mtu_id & 0xf;
	sw_w32(v, rtl_table_data(r, 1));

	v = (intf->ip6_mtu_id & 0xf) << 28;
	v |= (intf->ttl_scope & 0xff) << 20;
	v |= (intf->hl_scope & 0xff) << 12;
	v |= (intf->ip4_icmp_redirect & 0x7) << 9;
	v |= (intf->ip6_icmp_redirect & 0x7)<< 6;
	v |= (intf->ip4_pbr_icmp_redirect & 0x7) << 3;
	v |= (intf->ip6_pbr_icmp_redirect & 0x7);
	sw_w32(v, rtl_table_data(r, 2));

	v = intf->is_tunnel ? BIT(31) : 0;
	v |= intf->tunnel_idx << 22;
	sw_w32(v, rtl_table_data(r, 3));

	pr_info("%s writing to index %d: %08x %08x %08x %08x\n",__func__, idx,
		sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)),
		sw_r32(rtl_table_data(r, 2)), sw_r32(rtl_table_data(r, 3)));
	rtl_table_write(r, idx & 0x7f);
	rtl_table_release(r);
}

static void rtl931x_get_l3_egress_intf(int idx, struct rtl838x_l3_intf *intf)
{
	u32 v;
	// Read L3_EGR_INTF table (8) via register RTL9310_TBL_2
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 8);

	// The table has 4 registers
	v = sw_r32(rtl_table_data(r, 0));
	intf->vid = v >> 20;
	intf->smac = (((u64) v) & 0xfffff) << 28;

	v = sw_r32(rtl_table_data(r, 1));
	intf->smac |= v >> 4;
	intf->ip4_mtu_id = v & 0xf;

	v = sw_r32(rtl_table_data(r, 2));
	intf->ip6_mtu_id = v >> 28;
	intf->ttl_scope = (v >> 20) & 0xff;
	intf->hl_scope = (v >> 12) & 0xff;
	intf->ip4_icmp_redirect = (v >> 9) & 0x7;
	intf->ip6_icmp_redirect = (v >> 6) & 0x7;
	intf->ip4_pbr_icmp_redirect = (v >> 3) & 0x7;
	intf->ip6_pbr_icmp_redirect = v & 0x7;

	v = sw_r32(rtl_table_data(r, 3));
	intf->is_tunnel = v & BIT(31);
	intf->tunnel_idx = (v >> 22) & 0x1ff;

	pr_info("%s read from index %d: %08x %08x %08x %08x\n",__func__, idx,
		sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)),
		sw_r32(rtl_table_data(r, 2)), sw_r32(rtl_table_data(r, 3)));

	pr_info("%s smac %016llx\n", __func__, intf->smac);
	rtl_table_write(r, idx & 0x7f);
	rtl_table_release(r);
}

/*
 * Reads a MAC entry for L3 termination as entry point for routing
 * from the hardware table
 * idx is the index into the L3_ROUTER_MAC table
 */
static void rtl931x_get_l3_router_mac(u32 idx, struct rtl93xx_rt_mac *m)
{
	u32 v;
	// Read L3_ROUTER_MAC table (2) via register RTL9310_TBL_2
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 2);

	rtl_table_read(r, idx);
	// The table has a size of 5 registers, 1024 entries
	v = sw_r32(rtl_table_data(r, 0));
	m->valid = !!(v & BIT(31));
	if (!m->valid)
		goto out;

	m->p_type = !!(v & BIT(30));
	m->p_id = (v >> 23) & 0x7f;  // trunk id of port
	m->intf_id = (v >> 13) & 0x3ff;

	m->mac = ((u64)v & 0x7) << 35;
	v = sw_r32(rtl_table_data(r, 1));
	m->mac |= ((u64)v) << 3;
	v = sw_r32(rtl_table_data(r, 2));
	m->mac |= v >> 29;
	// Bits L3_INTF, LU_PHASE are 0

	m->p_id_mask = (v >> 15) & 0x7f;
	m->intf_id_mask = v >> 5 & 0x3ff;

	m->mac_mask = ((u64)v & 0x7) << 43;
	v = sw_r32(rtl_table_data(r, 3));
	m->mac_mask |= ((u64)v) << 11;
	v = sw_r32(rtl_table_data(r, 4));
	m->mac_mask |= v >> 21;
	// Bits BMSK_LU_PHASE and BMSK_L3_INTF are 0

	m->action = v >> 12 & 0x7;

out:
	rtl_table_release(r);
}

/*
 * Writes a MAC entry for L3 termination as entry point for routing
 * into the hardware table
 * idx is the index into the L3_ROUTER_MAC table
 */
static void rtl931x_set_l3_router_mac(u32 idx, struct rtl93xx_rt_mac *m)
{
	u32 v;
	// Read L3_ROUTER_MAC table (2) via register RTL9310_TBL_2
	struct table_reg *r = rtl_table_get(RTL9310_TBL_2, 2);

	// The table has a size of 5 registers, 1024 entries
	v = m->valid ? BIT(31) : 0;
	v |= m->p_type ? BIT(30) : 0;
	v |= (m->p_id  & 0x7f) << 23;
	v |= m->intf_id & 0x3ff << 13;
	v |= m->mac >> 35;
	sw_w32(v, rtl_table_data(r, 0));

	v = m->mac << 3;
	sw_w32(v, rtl_table_data(r, 1));

	v = m->mac << 29;
	// Bits L3_INTF, LU_PHASE are 0
	v |= m->p_id_mask & 0x7f << 15;
	v |= m->intf_id_mask & 0x3ff << 5;
	v |= ((u64)v & 0x7) >> 43;
	sw_w32(v, rtl_table_data(r, 2));

	v = m->mac_mask >> 11;
	v = sw_r32(rtl_table_data(r, 3));

	v = m->mac_mask << 21;
	// Bits BMSK_LU_PHASE and BMSK_L3_INTF are 0
	v |= (m->action & 0x7) << 12;
	v = sw_r32(rtl_table_data(r, 4));

	pr_info("%s writing index %d: %08x %08x %08x %08x %08x %08x %08x\n", __func__, idx,
		sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)), sw_r32(rtl_table_data(r, 2)),
		sw_r32(rtl_table_data(r, 3)), sw_r32(rtl_table_data(r, 4)), sw_r32(rtl_table_data(r, 5)),
		sw_r32(rtl_table_data(r, 6))
	);
	rtl_table_write(r, idx);
	rtl_table_release(r);
}

static int rtl931x_alloc_egress_intf(struct rtl838x_switch_priv *priv, u64 mac, int vlan)
{
	int i, free_mac = -1;
	struct rtl838x_l3_intf intf;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < MAX_INTERFACES; i++) {
		rtl931x_get_l3_egress_intf(i, &intf);
		if (intf.smac == mac)
			break;
		if (!intf.smac)
			break;
	}

	if (i >= MAX_INTERFACES) {
		pr_err("No free egress interface, cannot offload\n");
		return -1;
	}

	if (!intf.smac) {
		// Set up default egress interface 1
		intf.vid = vlan;
		intf.smac_idx = 0xff;
		intf.smac = mac;
		intf.ip4_mtu_id = 1;
		intf.ip6_mtu_id = 1;
		intf.ttl_scope = 1; // TTL
		intf.hl_scope = 1;  // Hop Limit
		intf.ip4_icmp_redirect = intf.ip6_icmp_redirect = 2;  // FORWARD
		intf.ip4_pbr_icmp_redirect = intf.ip6_pbr_icmp_redirect = 2; // FORWARD;
		rtl931x_set_l3_egress_intf(free_mac, &intf);
	}

	mutex_unlock(&priv->reg_mutex);

	return i;
}

int rtl931x_l3_setup(struct rtl838x_switch_priv *priv)
{
	int i;
	struct rtl838x_l3_intf intf;

	// Setup MTU with id 0 for default interface
	for (i = 0; i < MAX_INTF_MTUS; i++)
		priv->intf_mtu_count[i] = priv->intf_mtus[i] = 0;

/*	// Clear all existing interface definitions
	for (i = 0; i < ; i++)
		rtl931x_set_l3_egress_intf(i, &intf)
*/
	priv->intf_mtu_count[0] = 0; // Needs to stay forever
	priv->intf_mtus[0] = DEFAULT_MTU;
	sw_w32(DEFAULT_MTU, RTL931X_L3_INTF_IP_MTU(0));
	sw_w32(DEFAULT_MTU, RTL931X_L3_INTF_IP6_MTU(0));
	priv->intf_mtus[1] = DEFAULT_MTU;
	sw_w32(DEFAULT_MTU, RTL931X_L3_INTF_IP_MTU(1));
	sw_w32(DEFAULT_MTU, RTL931X_L3_INTF_IP6_MTU(1));

	// Configure the default L3 hash algorithm
	sw_w32_mask(BIT(2), 0, RTL931X_L3_HOST_TBL_CTRL);  // Algorithm selection 0 = 0
	sw_w32_mask(0, BIT(3), RTL931X_L3_HOST_TBL_CTRL);  // Algorithm selection 1 = 1

	pr_info("L3_IPUC_ROUTE_CTRL %08x, IPMC_ROUTE %08x, IP6UC_ROUTE %08x, IP6MC_ROUTE %08x\n",
		sw_r32(RTL931X_L3_IPUC_ROUTE_CTRL), sw_r32(RTL931X_L3_IPMC_ROUTE_CTRL),
		sw_r32(RTL931X_L3_IP6UC_ROUTE_CTRL), sw_r32(RTL931X_L3_IP6MC_ROUTE_CTRL));
	sw_w32_mask(0, 1, RTL931X_L3_IPUC_ROUTE_CTRL); // Enable IPv4 Unicast routing globally
	sw_w32_mask(0, 1, RTL931X_L3_IP6UC_ROUTE_CTRL); // Enable IPv6 Unicast
	sw_w32_mask(0, 1, RTL931X_L3_IPMC_ROUTE_CTRL);	// Enable IPv6 Multicast
	sw_w32_mask(0, 1, RTL931X_L3_IP6MC_ROUTE_CTRL); // Enable IPv6 Multicast

	pr_info("L3_IPUC_ROUTE_CTRL %08x, IPMC_ROUTE %08x, IP6UC_ROUTE %08x, IP6MC_ROUTE %08x\n",
		sw_r32(RTL930X_L3_IPUC_ROUTE_CTRL), sw_r32(RTL930X_L3_IPMC_ROUTE_CTRL),
		sw_r32(RTL930X_L3_IP6UC_ROUTE_CTRL), sw_r32(RTL930X_L3_IP6MC_ROUTE_CTRL));

	// Trap non-ip traffic to the CPU-port (e.g. ARP so we stay reachable)
	sw_w32_mask(0x3 << 8, 0x1 << 8, RTL930X_L3_IP_ROUTE_CTRL);
	pr_info("L3_IP_ROUTE_CTRL %08x\n", sw_r32(RTL930X_L3_IP_ROUTE_CTRL));

	// PORT_ISO_RESTRICT_ROUTE_CTRL ?

	// Do not use prefix route 0 because of HW limitations
	set_bit(0, priv->route_use_bm);

	return 0;
}

void rtl931x_vlan_port_pvidmode_set(int port, enum pbvlan_type type, enum pbvlan_mode mode)
{
	if (type == PBVLAN_TYPE_INNER)
		sw_w32_mask(0x3 << 12, mode << 12, RTL931X_VLAN_PORT_IGR_CTRL + (port << 2));
	else
		sw_w32_mask(0x3 << 26, mode << 26, RTL931X_VLAN_PORT_IGR_CTRL + (port << 2));
}

void rtl931x_vlan_port_pvid_set(int port, enum pbvlan_type type, int pvid)
{
	if (type == PBVLAN_TYPE_INNER)
		sw_w32_mask(0xfff, pvid, RTL931X_VLAN_PORT_IGR_CTRL + (port << 2));
	else
		sw_w32_mask(0xfff << 14, pvid << 14, RTL931X_VLAN_PORT_IGR_CTRL + (port << 2));
}

static void rtl931x_set_igr_filter(int port, enum igr_filter state)
{
	sw_w32_mask(0x3 << ((port & 0xf)<<1), state << ((port & 0xf)<<1),
		    RTL931X_VLAN_PORT_IGR_FLTR + (((port >> 4) << 2)));
}

static void rtl931x_set_egr_filter(int port,  enum egr_filter state)
{
	sw_w32_mask(0x1 << (port % 0x20), state << (port % 0x20),
		    RTL931X_VLAN_PORT_EGR_FLTR + (((port >> 5) << 2)));
}

void rtl931x_set_distribution_algorithm(int group, int algoidx, u32 algomsk)
{
	u32 l3shift = 0;
	u32 newmask = 0;

	/* TODO: for now we set algoidx to 0 */
	algoidx=0;

	if (algomsk & TRUNK_DISTRIBUTION_ALGO_SIP_BIT) {
		l3shift = 4;
		newmask |= TRUNK_DISTRIBUTION_ALGO_L3_SIP_BIT;
	}
	if (algomsk & TRUNK_DISTRIBUTION_ALGO_DIP_BIT) {
		l3shift = 4;
		newmask |= TRUNK_DISTRIBUTION_ALGO_L3_DIP_BIT;
	}
	if (algomsk & TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT) {
		l3shift = 4;
		newmask |= TRUNK_DISTRIBUTION_ALGO_L3_SRC_L4PORT_BIT;
	}
	if (algomsk & TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT) {
		l3shift = 4;
		newmask |= TRUNK_DISTRIBUTION_ALGO_L3_SRC_L4PORT_BIT;
	}

	if (l3shift == 4) {
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_SMAC_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L3_SMAC_BIT;
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_DMAC_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L3_DMAC_BIT;
	} else {
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_SMAC_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L2_SMAC_BIT;
		if (algomsk & TRUNK_DISTRIBUTION_ALGO_DMAC_BIT)
			newmask |= TRUNK_DISTRIBUTION_ALGO_L2_DMAC_BIT;
	}

	sw_w32(newmask << l3shift, RTL931X_TRK_HASH_CTRL + (algoidx << 2));
}

static void rtl931x_led_init(struct rtl838x_switch_priv *priv)
{
	int i, j, pos;
	u32 v, set;
	u64 pm_copper = 0, pm_fiber = 0;
	u32 setlen;
	const __be32 *led_set;
	char set_name[9];
	u8 led_counts[4];
	struct device_node *node;

	pr_info("%s called\n", __func__);
	node = of_find_compatible_node(NULL, NULL, "realtek,rtl9300-leds");
	if (!node) {
		pr_info("%s No compatible LED node found\n", __func__);
		return;
	}

	for (i= 0; i < priv->cpu_port; i++) {
		pos = (i << 1) % 32;
		sw_w32_mask(0x3 << pos, 0, RTL931X_LED_PORT_FIB_SET_SEL_CTRL(i));
		sw_w32_mask(0x3 << pos, 0, RTL931X_LED_PORT_COPR_SET_SEL_CTRL(i));

		if (!priv->ports[i].phy)
			continue;

		if (priv->ports[i].phy_is_integrated)
		pm_fiber |= BIT_ULL(i);
			else
		pm_copper |= BIT_ULL(i);

		set = priv->ports[i].led_set;
		sw_w32_mask(0, set << pos, RTL931X_LED_PORT_COPR_SET_SEL_CTRL(i));
		sw_w32_mask(0, set << pos, RTL931X_LED_PORT_FIB_SET_SEL_CTRL(i));
	}

	for (i = 0; i < 4; i++) {
		sprintf(set_name, "led_set%d", i);
		pr_info(">%s<\n", set_name);
		led_set = of_get_property(node, set_name, &setlen);
		if (!led_set || setlen != 16)
			break;
		v = be32_to_cpup(led_set) << 16 | be32_to_cpup(led_set + 1);
		sw_w32(v, RTL931X_LED_SET0_0_CTRL - 4 - i * 8);
		v = be32_to_cpup(led_set + 2) << 16 | be32_to_cpup(led_set + 3);
		sw_w32(v, RTL931X_LED_SET0_0_CTRL - i * 8);

		led_counts[i] = 0;
		for (j = 0; j < 4; j++) {
			if (be32_to_cpup(led_set + j) && be32_to_cpup(led_set + j) != 0xffff)
				led_counts[i]++;
		}
		pr_info("%s: Led COUNT %d is %d\n", __func__, i, led_counts[i]);
	}

	for (i = 0; i < priv->cpu_port; i++) {
		pos = (i << 1) % 32;
		if (priv->ports[i].phy)
			v = priv->ports[i].led_set < 4 ? led_counts[priv->ports[i].led_set] - 1:0;
		else
			v = 0x2;
		sw_w32_mask(0x3 << pos, v << pos, RTL931X_LED_PORT_NUM_CTRL(i));
	}

	// Set LED mode to serial (0x1), color-scan is (0x2), bi-color-scan (0x3)
	sw_w32_mask(0x3, 0x1, RTL931X_LED_GLB_CTRL);
	// Set clock, serial requires 0x2, scan-mode requires 0x1 in bits 15 & 16
	sw_w32_mask(0x3 << 15, 0x2 << 15, RTL931X_LED_GLB_CTRL);

	rtl839x_set_port_reg_le(pm_copper, RTL931X_LED_PORT_COPR_MASK_CTRL);
	rtl839x_set_port_reg_le(pm_fiber, RTL931X_LED_PORT_FIB_MASK_CTRL);
	rtl839x_set_port_reg_le(pm_copper | pm_fiber, RTL931X_LED_PORT_COMBO_MASK_CTRL);

	for (i = 0; i < 32; i++)
		pr_info("%s %08x: %08x\n",__func__, 0xbb000600 + i * 4, sw_r32(0x0600 + i * 4));

}

const struct rtl83xx_mib_desc rtl9310_mib[] = {
	MIB_DESC(2,  0, "ifInOctets"),
	MIB_DESC(2,  2, "ifOutOctets"),
	MIB_DESC(2,  4, "ifInUcastPkts"),
	MIB_DESC(2,  6, "ifInMulticastPkts"),
	MIB_DESC(2,  8, "ifInBroadcastPkts"),
	MIB_DESC(2, 10, "ifOutUcastPkts"),
	MIB_DESC(2, 12, "ifOutMulticastPkts"),
	MIB_DESC(2, 14, "ifOutBroadcastPkts"),
	MIB_DESC(1, 16, "ifOutDiscards"),
	MIB_DESC(1, 17, "dot1dTpPortInDiscards"),
	MIB_DESC(1, 18, ".3SingleCollisionFrames"),
	MIB_DESC(1, 19, ".3MultipleCollisionFrames"),
	MIB_DESC(1, 20, ".3DeferredTransmissions"),
	MIB_DESC(1, 21, ".3LateCollisions"),
	MIB_DESC(1, 22, ".3ExcessiveCollisions"),
	MIB_DESC(1, 23, ".3SymbolErrors"),
	MIB_DESC(1, 24, ".3ControlInUnknownOpcodes"),
	MIB_DESC(1, 25, ".3InPauseFrames"),
	MIB_DESC(1, 26, ".3OutPauseFrames"),
	MIB_DESC(1, 27, "DropEvents"),
	MIB_DESC(1, 28, "tx_BroadcastPkts"),
	MIB_DESC(1, 29, "tx_MulticastPkts"),
	MIB_DESC(1, 30, "tx_CRCAlignErrors"),
	MIB_DESC(1, 31, "rx_CRCAlignErrors"),
	MIB_DESC(1, 32, "tx_UndersizePkts"),
	MIB_DESC(1, 33, "rx_UndersizePkts"),
	MIB_DESC(1, 34, "tx_OversizePkts"),
	MIB_DESC(1, 35, "rx_OversizePkts"),
	MIB_DESC(1, 36, "tx_Fragments"),
	MIB_DESC(1, 37, "rx_Fragments"),
	MIB_DESC(1, 38, "tx_Jabbers"),
	MIB_DESC(1, 39, "rx_Jabbers"),
	MIB_DESC(1, 40, "tx_Collisions"),
	MIB_DESC(1, 41, "tx_Pkts64Octets"),
	MIB_DESC(1, 42, "rx_Pkts64Octets"),
	MIB_DESC(1, 43, "tx_Pkts65to127Octets"),
	MIB_DESC(1, 44, "rx_Pkts65to127Octets"),
	MIB_DESC(1, 45, "tx_Pkts128to255Octets"),
	MIB_DESC(1, 46, "rx_Pkts128to255Octets"),
	MIB_DESC(1, 47, "tx_Pkts256to511Octets"),
	MIB_DESC(1, 48, "rx_Pkts256to511Octets"),
	MIB_DESC(1, 49, "tx_Pkts512to1023Octets"),
	MIB_DESC(1, 40, "rx_Pkts512to1023Octets"),
	MIB_DESC(1, 51, "tx_Pkts1024to1518Octets"),
	MIB_DESC(1, 52, "rx_Pkts1024to1518Octets")
};

void rtl931x_get_strings(int port, u32 stringset, u8 *data)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(rtl9310_mib); i++)
		strncpy(data + i * ETH_GSTRING_LEN, rtl9310_mib[i].name,
			ETH_GSTRING_LEN);
}

void rtl931x_get_ethtool_stats(struct rtl838x_switch_priv *priv, int port, uint64_t *data)
{
	// Read STAT_PORT_MIB_CNTR (0) via register RTL9310_TBL_5
	struct table_reg *q = rtl_table_get(RTL9310_TBL_5, 0);
	const struct rtl83xx_mib_desc *mib;
	int i;

	rtl_table_read(q, port);

	for (i = 0; i < ARRAY_SIZE(rtl9310_mib); i++) {
		mib = &rtl9310_mib[i];

		data[i] = sw_r32(rtl_table_data(q, mib->offset));
		if (mib->size == 2) {
			data[i] <<= 32;
			data[i] |= sw_r32(rtl_table_data(q, mib->offset + 1));
		}
	}
	rtl_table_release(q);
}

int rtl931x_get_sset_count(int port, int sset)
{
	return ARRAY_SIZE(rtl9310_mib);
}

const struct rtl838x_reg rtl931x_reg = {
	.mask_port_reg_be = rtl839x_mask_port_reg_be,
	.set_port_reg_be = rtl839x_set_port_reg_be,
	.get_port_reg_be = rtl839x_get_port_reg_be,
	.mask_port_reg_le = rtl839x_mask_port_reg_le,
	.set_port_reg_le = rtl839x_set_port_reg_le,
	.get_port_reg_le = rtl839x_get_port_reg_le,
	.stat_port_rst = RTL931X_STAT_PORT_RST,
	.stat_rst = RTL931X_STAT_RST,
	.stat_port_std_mib = 0,  // Not defined
	.traffic_enable = rtl931x_traffic_enable,
	.traffic_disable = rtl931x_traffic_disable,
	.traffic_get = rtl931x_traffic_get,
	.traffic_set = rtl931x_traffic_set,
	.l2_ctrl_0 = RTL931X_L2_CTRL,
	.l2_ctrl_1 = RTL931X_L2_AGE_CTRL,
	.l2_port_aging_out = RTL931X_L2_PORT_AGE_CTRL,
	.set_ageing_time = rtl931x_set_ageing_time,
	.smi_poll_ctrl = RTL931X_SMI_PORT_POLLING_CTRL,
	.l2_tbl_flush_ctrl = RTL931X_L2_TBL_FLUSH_CTRL,
	.exec_tbl0_cmd = rtl931x_exec_tbl0_cmd,
	.exec_tbl1_cmd = rtl931x_exec_tbl1_cmd,
	.tbl_access_data_0 = rtl931x_tbl_access_data_0,
	.isr_glb_src = RTL931X_ISR_GLB_SRC,
	.isr_port_link_sts_chg = RTL931X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL931X_IMR_PORT_LINK_STS_CHG,
	// imr_glb does not exist on RTL931X
	.vlan_tables_read = rtl931x_vlan_tables_read,
	.vlan_set_tagged = rtl931x_vlan_set_tagged,
	.vlan_set_untagged = rtl931x_vlan_set_untagged,
	.vlan_profile_dump = rtl931x_vlan_profile_dump,
	.vlan_profile_setup = rtl931x_vlan_profile_setup,
	.vlan_fwd_on_inner = rtl931x_vlan_fwd_on_inner,
	.stp_get = rtl931x_stp_get,
	.stp_set = rtl931x_stp_set,
	.mac_force_mode_ctrl = rtl931x_mac_force_mode_ctrl,
	.mac_port_ctrl = rtl931x_mac_port_ctrl,
	.l2_port_new_salrn = rtl931x_l2_port_new_salrn,
	.l2_port_new_sa_fwd = rtl931x_l2_port_new_sa_fwd,
	.mir_ctrl = RTL931X_MIR_CTRL,
	.mir_dpm = RTL931X_MIR_DPM_CTRL,
	.mir_spm = RTL931X_MIR_SPM_CTRL,
	.mac_link_sts = RTL931X_MAC_LINK_STS,
	.mac_link_dup_sts = RTL931X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts = rtl931x_mac_link_spd_sts,
	.mac_rx_pause_sts = RTL931X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts = RTL931X_MAC_TX_PAUSE_STS,
	.read_l2_entry_using_hash = rtl931x_read_l2_entry_using_hash,
	.write_l2_entry_using_hash = rtl931x_write_l2_entry_using_hash,
	.read_cam = rtl931x_read_cam,
	.write_cam = rtl931x_write_cam,
	.vlan_port_tag_sts_ctrl = RTL931X_VLAN_PORT_TAG_CTRL,
	.vlan_port_pvidmode_set = rtl931x_vlan_port_pvidmode_set,
	.vlan_port_pvid_set = rtl931x_vlan_port_pvid_set,
	.trk_mbr_ctr = rtl931x_trk_mbr_ctr,
	.set_vlan_igr_filter = rtl931x_set_igr_filter,
	.set_vlan_egr_filter = rtl931x_set_egr_filter,
	.set_distribution_algorithm = rtl931x_set_distribution_algorithm,
	.init_eee = rtl931x_init_eee,
	.port_eee_set = rtl931x_port_eee_set,
	.eee_port_ability = rtl931x_eee_port_ability,
	.l2_hash_seed = rtl931x_l2_hash_seed,
	.l2_hash_key = rtl931x_l2_hash_key,
	.read_mcast_pmask = rtl931x_read_mcast_pmask,
	.write_mcast_pmask = rtl931x_write_mcast_pmask,
	.pie_init = rtl931x_pie_init,
	.pie_rule_write = rtl931x_pie_rule_write,
	.pie_rule_add = rtl931x_pie_rule_add,
	.pie_rule_rm = rtl931x_pie_rule_rm,
	.l2_learning_setup = rtl931x_l2_learning_setup,
	.route_read = rtl931x_route_read,
	.route_write = rtl931x_route_write,
	.host_route_write = rtl931x_host_route_write,
	.l3_setup = rtl931x_l3_setup,
	.set_l3_nexthop = rtl931x_set_l3_nexthop,
	.get_l3_nexthop = rtl931x_get_l3_nexthop,
	.find_l3_slot = rtl931x_find_l3_slot,
	.route_lookup_hw = rtl931x_route_lookup_hw,
	.get_l3_router_mac = rtl931x_get_l3_router_mac,
	.set_l3_router_mac = rtl931x_set_l3_router_mac,
	.l3_mtu_add = rtl931x_l3_mtu_add,
	.alloc_egress_intf = rtl931x_alloc_egress_intf,
	.led_init = rtl931x_led_init,
};

