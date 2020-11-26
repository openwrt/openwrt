// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl83xx.h"

extern struct mutex smi_lock;


static inline void rtl839x_mask_port_reg_be(u64 clear, u64 set, int reg)
{
	sw_w32_mask((u32)(clear >> 32), (u32)(set >> 32), reg);
	sw_w32_mask((u32)(clear & 0xffffffff), (u32)(set & 0xffffffff), reg + 4);
}

static inline u64 rtl839x_get_port_reg_be(int reg)
{
	u64 v = sw_r32(reg);

	v <<= 32;
	v |= sw_r32(reg + 4);
	return v;
}

static inline void rtl839x_set_port_reg_be(u64 set, int reg)
{
	sw_w32(set >> 32, reg);
	sw_w32(set & 0xffffffff, reg + 4);
}

static inline void rtl839x_mask_port_reg_le(u64 clear, u64 set, int reg)
{
	sw_w32_mask((u32)clear, (u32)set, reg);
	sw_w32_mask((u32)(clear >> 32), (u32)(set >> 32), reg + 4);
}

static inline void rtl839x_set_port_reg_le(u64 set, int reg)
{
	sw_w32(set, reg);
	sw_w32(set >> 32, reg + 4);
}

static inline u64 rtl839x_get_port_reg_le(int reg)
{
	u64 v = sw_r32(reg + 4);

	v <<= 32;
	v |= sw_r32(reg);
	return v;
}

static inline int rtl839x_stat_port_std_mib(int p)
{
	return RTL839X_STAT_PORT_STD_MIB + (p << 8);
}

static inline int rtl839x_port_iso_ctrl(int p)
{
	return RTL839X_PORT_ISO_CTRL(p);
}

static inline void rtl839x_exec_tbl0_cmd(u32 cmd)
{
	sw_w32(cmd, RTL839X_TBL_ACCESS_CTRL_0);
	do { } while (sw_r32(RTL839X_TBL_ACCESS_CTRL_0) & BIT(16));
}

static inline void rtl839x_exec_tbl1_cmd(u32 cmd)
{
	sw_w32(cmd, RTL839X_TBL_ACCESS_CTRL_1);
	do { } while (sw_r32(RTL839X_TBL_ACCESS_CTRL_1) & BIT(16));
}

static inline int rtl839x_tbl_access_data_0(int i)
{
	return RTL839X_TBL_ACCESS_DATA_0(i);
}

static void rtl839x_vlan_tables_read(u32 vlan, struct rtl838x_vlan_info *info)
{
	u32 cmd;
	u64 v;
	u32 u, w;

	cmd = BIT(16) /* Execute cmd */
		| 0 << 15 /* Read */
		| 0 << 12 /* Table type 0b000 */
		| (vlan & 0xfff);
	rtl839x_exec_tbl0_cmd(cmd);

	v = sw_r32(RTL838X_TBL_ACCESS_DATA_0(0));
	v <<= 32;
	u = sw_r32(RTL838X_TBL_ACCESS_DATA_0(1));
	v |= u;
	info->tagged_ports = v >> 11;

	w = sw_r32(RTL838X_TBL_ACCESS_DATA_0(2));

	info->profile_id = w >> 30 | ((u & 1) << 2);
	info->hash_mc_fid = !!(u & 2);
	info->hash_uc_fid = !!(u & 4);
	info->fid = (u >> 3) & 0xff;

	cmd = BIT(16) /* Execute cmd */
		| 0 << 15 /* Read */
		| 0 << 12 /* Table type 0b000 */
		| (vlan & 0xfff);
	rtl839x_exec_tbl1_cmd(cmd);
	v = sw_r32(RTL838X_TBL_ACCESS_DATA_1(0));
	v <<= 32;
	v |= sw_r32(RTL838X_TBL_ACCESS_DATA_1(1));
	info->untagged_ports = v >> 11;
}

static void rtl839x_vlan_set_tagged(u32 vlan, struct rtl838x_vlan_info *info)
{
	u32 cmd = BIT(16) /* Execute cmd */
		| BIT(15) /* Write */
		| 0 << 12 /* Table type 0b00 */
		| (vlan & 0xfff);
	u32 w;
	u64 v = info->tagged_ports << 11;

	v |= info->profile_id >> 2;
	v |= info->hash_mc_fid ? 2 : 0;
	v |= info->hash_uc_fid ? 4 : 0;
	v |= ((u32)info->fid) << 3;
	rtl839x_set_port_reg_be(v, RTL838X_TBL_ACCESS_DATA_0(0));

	w = info->profile_id;
	sw_w32(w << 30, RTL838X_TBL_ACCESS_DATA_0(2));
	rtl839x_exec_tbl0_cmd(cmd);
}

static void rtl839x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	u32 cmd = BIT(16) /* Execute cmd */
		| BIT(15) /* Write */
		| 0 << 12 /* Table type 0b00 */
		| (vlan & 0xfff);
	rtl839x_set_port_reg_be(portmask << 11, RTL838X_TBL_ACCESS_DATA_1(0));
	rtl839x_exec_tbl1_cmd(cmd);
}

static inline int rtl839x_mac_force_mode_ctrl(int p)
{
	return RTL839X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl839x_mac_port_ctrl(int p)
{
	return RTL839X_MAC_PORT_CTRL(p);
}

static inline int rtl839x_l2_port_new_salrn(int p)
{
	return RTL839X_L2_PORT_NEW_SALRN(p);
}

static inline int rtl839x_l2_port_new_sa_fwd(int p)
{
	return RTL839X_L2_PORT_NEW_SA_FWD(p);
}

static inline int rtl839x_mir_ctrl(int group)
{
	return RTL839X_MIR_CTRL(group);
}

static inline int rtl839x_mir_dpm(int group)
{
	return RTL839X_MIR_DPM_CTRL(group);
}

static inline int rtl839x_mir_spm(int group)
{
	return RTL839X_MIR_SPM_CTRL(group);
}

static inline int rtl839x_mac_link_spd_sts(int p)
{
	return RTL839X_MAC_LINK_SPD_STS(p);
}

static u64 rtl839x_read_l2_entry_using_hash(u32 hash, u32 position, struct rtl838x_l2_entry *e)
{
	u64 entry;
	u32 r[3];

	/* Search in SRAM, with hash and at position in hash bucket (0-3) */
	u32 idx = (0 << 14) | (hash << 2) | position;

	u32 cmd = BIT(17) /* Execute cmd */
		| 0 << 16 /* Read */
		| 0 << 14 /* Table type 0b00 */
		| (idx & 0x3fff);

	sw_w32(cmd, RTL839X_TBL_ACCESS_L2_CTRL);
	do { }  while (sw_r32(RTL839X_TBL_ACCESS_L2_CTRL) & BIT(17));
	r[0] = sw_r32(RTL839X_TBL_ACCESS_L2_DATA(0));
	r[1] = sw_r32(RTL839X_TBL_ACCESS_L2_DATA(1));
	r[2] = sw_r32(RTL839X_TBL_ACCESS_L2_DATA(2));

	/* Table contains different entry types, we need to identify the right one:
	 * Check for MC entries, first
	 */
	e->is_ip_mc = !!(r[2] & BIT(31));
	e->is_ipv6_mc = !!(r[2] & BIT(30));
	e->type = L2_INVALID;
	if (!e->is_ip_mc) {
		e->mac[0] = (r[0] >> 12);
		e->mac[1] = (r[0] >> 4);
		e->mac[2] = ((r[1] >> 28) | (r[0] << 4));
		e->mac[3] = (r[1] >> 20);
		e->mac[4] = (r[1] >> 12);
		e->mac[5] = (r[1] >> 4);

		/* Is it a unicast entry? check multicast bit */
		if (!(e->mac[0] & 1)) {
			e->is_static = !!((r[2] >> 18) & 1);
			e->vid = (r[2] >> 4) & 0xfff;
			e->rvid = (r[0] >> 20) & 0xfff;
			e->port = (r[2] >> 24) & 0x3f;
			e->block_da = !!(r[2] & BIT(19));
			e->block_sa = !!(r[2] & BIT(20));
			e->suspended = !!(r[2] & BIT(17));
			e->next_hop = !!(r[2] & BIT(16));
			if (e->next_hop)
				pr_debug("Found next hop entry, need to read data\n");
			e->age = (r[2] >> 21) & 3;
			e->valid = true;
			if (!(r[2] & 0xc0fd0000)) /* Check for valid entry */
				e->valid = false;
			else
				e->type = L2_UNICAST;
		} else {
			e->valid = true;
			e->type = L2_MULTICAST;
			e->mc_portmask_index = (r[2]>>6) & 0xfff;
		}
	}
	if (e->is_ip_mc) {
		e->valid = true;
		e->type = IP4_MULTICAST;
	}
	if (e->is_ipv6_mc) {
		e->valid = true;
		e->type = IP6_MULTICAST;
	}

	entry = (((u64) r[0]) << 12) | ((r[1] & 0xfffffff0) << 12) | ((r[2] >> 4) & 0xfff);
	return entry;
}

static u64 rtl839x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
	u64 entry;
	u32 r[3];

	u32 cmd = BIT(17) /* Execute cmd */
		| 0 << 16 /* Read */
		| BIT(14) /* Table type 0b01 */
		| (idx & 0x3f);
	sw_w32(cmd, RTL839X_TBL_ACCESS_L2_CTRL);
	do { }  while (sw_r32(RTL839X_TBL_ACCESS_L2_CTRL) & BIT(17));
	r[0] = sw_r32(RTL839X_TBL_ACCESS_L2_DATA(0));
	r[1] = sw_r32(RTL839X_TBL_ACCESS_L2_DATA(1));
	r[2] = sw_r32(RTL839X_TBL_ACCESS_L2_DATA(2));

	e->mac[0] = (r[0] >> 12);
	e->mac[1] = (r[0] >> 4);
	e->mac[2] = ((r[1] >> 28) | (r[0] << 4));
	e->mac[3] = (r[1] >> 20);
	e->mac[4] = (r[1] >> 12);
	e->mac[5] = (r[1] >> 4);
	e->is_static = !!((r[2] >> 18) & 1);
	e->vid = (r[2] >> 4) & 0xfff;
	e->rvid = (r[0] >> 20) & 0xfff;
	e->port = (r[2] >> 24) & 0x3f;

	e->valid = true;
	if (!(r[2] & 0x10fd0000)) /* Check for invalid entry */
		e->valid = false;

	if (e->valid)
		pr_debug("Found in CAM: R1 %x R2 %x R3 %x\n", r[0], r[1], r[2]);

	entry = (((u64) r[0]) << 12) | ((r[1] & 0xfffffff0) << 12) | ((r[2] >> 4) & 0xfff);
	return entry;
}

static inline int rtl839x_vlan_profile(int profile)
{
	return RTL839X_VLAN_PROFILE(profile);
}

static inline int rtl839x_vlan_port_egr_filter(int port)
{
	return RTL839X_VLAN_PORT_EGR_FLTR(port);
}

static inline int rtl839x_vlan_port_igr_filter(int port)
{
	return RTL839X_VLAN_PORT_IGR_FLTR(port);
}

static inline int rtl839x_vlan_port_pb(int port)
{
	return RTL839X_VLAN_PORT_PB_VLAN(port);
}

static inline int rtl839x_vlan_port_tag_sts_ctrl(int port)
{
	return RTL839X_VLAN_PORT_TAG_STS_CTRL(port);
}

const struct rtl838x_reg rtl839x_reg = {
	.mask_port_reg_be = rtl839x_mask_port_reg_be,
	.set_port_reg_be = rtl839x_set_port_reg_be,
	.get_port_reg_be = rtl839x_get_port_reg_be,
	.mask_port_reg_le = rtl839x_mask_port_reg_le,
	.set_port_reg_le = rtl839x_set_port_reg_le,
	.get_port_reg_le = rtl839x_get_port_reg_le,
	.stat_port_rst = RTL839X_STAT_PORT_RST,
	.stat_rst = RTL839X_STAT_RST,
	.stat_port_std_mib = rtl839x_stat_port_std_mib,
	.port_iso_ctrl = rtl839x_port_iso_ctrl,
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
	.mac_port_ctrl = rtl839x_mac_port_ctrl,
	.l2_port_new_salrn = rtl839x_l2_port_new_salrn,
	.l2_port_new_sa_fwd = rtl839x_l2_port_new_sa_fwd,
	.mir_ctrl = rtl839x_mir_ctrl,
	.mir_dpm = rtl839x_mir_dpm,
	.mir_spm = rtl839x_mir_spm,
	.mac_link_sts = RTL839X_MAC_LINK_STS,
	.mac_link_dup_sts = RTL839X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts = rtl839x_mac_link_spd_sts,
	.mac_rx_pause_sts = RTL839X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts = RTL839X_MAC_TX_PAUSE_STS,
	.read_l2_entry_using_hash = rtl839x_read_l2_entry_using_hash,
	.read_cam = rtl839x_read_cam,
	.vlan_profile = rtl839x_vlan_profile,
	.vlan_port_egr_filter = rtl839x_vlan_port_egr_filter,
	.vlan_port_igr_filter = rtl839x_vlan_port_igr_filter,
	.vlan_port_pb = rtl839x_vlan_port_pb,
	.vlan_port_tag_sts_ctrl = rtl839x_vlan_port_tag_sts_ctrl,
};

irqreturn_t rtl839x_switch_irq(int irq, void *dev_id)
{
	struct dsa_switch *ds = dev_id;
	u32 status = sw_r32(RTL839X_ISR_GLB_SRC);
	u64 ports = rtl839x_get_port_reg_le(RTL839X_ISR_PORT_LINK_STS_CHG);
	u64 link;
	int i;

	/* Clear status */
	rtl839x_set_port_reg_le(ports, RTL839X_ISR_PORT_LINK_STS_CHG);
	pr_debug("RTL8390 Link change: status: %x, ports %llx\n", status, ports);

	for (i = 0; i < 52; i++) {
		if (ports & (1ULL << i)) {
			link = rtl839x_get_port_reg_le(RTL839X_MAC_LINK_STS);
			if (link & (1ULL << i))
				dsa_port_phylink_mac_change(ds, i, true);
			else
				dsa_port_phylink_mac_change(ds, i, false);
		}
	}
	return IRQ_HANDLED;
}

// TODO: unused
int rtl8390_sds_power(int mac, int val)
{
	u32 offset = (mac == 48) ? 0x0 : 0x100;
	u32 mode = val ? 0 : 1;

	pr_debug("In %s: mac %d, set %d\n", __func__, mac, val);

	if ((mac != 48) && (mac != 49)) {
		pr_err("%s: not an SFP port: %d\n", __func__, mac);
		return -1;
	}

	// Set bit 1003. 1000 starts at 7c
	sw_w32_mask(BIT(11), mode << 11, RTL839X_SDS12_13_PWR0 + offset);

	return 0;
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
	sw_w32_mask(0, BIT(port), RTL839X_PHYREG_PORT_CTRL(port));

	sw_w32_mask(0xffff0000, val << 16, RTL839X_PHYREG_DATA_CTRL);

	v = reg << 5 | page << 10 | ((page == 0x1fff) ? 0x1f : 0) << 23;
	sw_w32(v, RTL839X_PHYREG_ACCESS_CTRL);

	sw_w32(0x1ff, RTL839X_PHYREG_CTRL);

	v |= BIT(3) | 1; /* Write operation and execute */
	sw_w32(v, RTL839X_PHYREG_ACCESS_CTRL);

	do {
	} while (sw_r32(RTL839X_PHYREG_ACCESS_CTRL) & 0x1);

	if (sw_r32(RTL839X_PHYREG_ACCESS_CTRL) & 0x2)
		err = -EIO;

	mutex_unlock(&smi_lock);
	return err;
}

void rtl8390_get_version(struct rtl838x_switch_priv *priv)
{
	u32 info;

	sw_w32_mask(0xf << 28, 0xa << 28, RTL839X_CHIP_INFO);
	info = sw_r32(RTL839X_CHIP_INFO);
	pr_debug("Chip-Info: %x\n", info);
	priv->version = RTL8390_VERSION_A;
}

u32 rtl839x_hash(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 h1, h2, h;

	if (sw_r32(priv->r->l2_ctrl_0) & 1) {
		h1 = (u32) (((seed >> 60) & 0x3f) ^ ((seed >> 54) & 0x3f)
				^ ((seed >> 36) & 0x3f) ^ ((seed >> 30) & 0x3f)
				^ ((seed >> 12) & 0x3f) ^ ((seed >> 6) & 0x3f));
		h2 = (u32) (((seed >> 48) & 0x3f) ^ ((seed >> 42) & 0x3f)
				^ ((seed >> 24) & 0x3f) ^ ((seed >> 18) & 0x3f)
				^ (seed & 0x3f));
		h = (h1 << 6) | h2;
	} else {
		h = (seed >> 60)
			^ ((((seed >> 48) & 0x3f) << 6) | ((seed >> 54) & 0x3f))
			^ ((seed >> 36) & 0xfff) ^ ((seed >> 24) & 0xfff)
			^ ((seed >> 12) & 0xfff) ^ (seed & 0xfff);
	}

	return h;
}

void rtl839x_vlan_profile_dump(int index)
{
	u32 profile, profile1;

	if (index < 0 || index > 7)
		return;

	profile1 = sw_r32(RTL839X_VLAN_PROFILE(index) + 4);
	profile = sw_r32(RTL839X_VLAN_PROFILE(index));

	pr_debug("VLAN %d: L2 learning: %d, L2 Unknown MultiCast Field %x, \
		IPv4 Unknown MultiCast Field %x, IPv6 Unknown MultiCast Field: %x",
		index, profile & 1, (profile >> 1) & 0xfff, (profile >> 13) & 0xfff,
		(profile1) & 0xfff);
}
