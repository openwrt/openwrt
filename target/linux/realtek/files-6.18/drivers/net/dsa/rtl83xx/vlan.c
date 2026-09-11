// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/if_bridge.h>
#include <net/dsa.h>

#include "rtl-otto.h"
#include "vlan.h"

#define RTL838X_VLAN_PORT_TAG_STS_UNTAG				0x0
#define RTL838X_VLAN_PORT_TAG_STS_TAGGED			0x1
#define RTL838X_VLAN_PORT_TAG_STS_PRIORITY_TAGGED		0x2

#define RTL838X_VLAN_PORT_TAG_STS_CTRL_BASE			0xA530
/* port 0-28 */
#define RTL838X_VLAN_PORT_TAG_STS_CTRL(port) \
	(RTL838X_VLAN_PORT_TAG_STS_CTRL_BASE + (port << 2))

#define RTL838X_VLAN_PORT_TAG_STS_CTRL_EGR_P_OTAG_KEEP_MASK	GENMASK(11, 10)
#define RTL838X_VLAN_PORT_TAG_STS_CTRL_EGR_P_ITAG_KEEP_MASK	GENMASK(9, 8)
#define RTL838X_VLAN_PORT_TAG_STS_CTRL_IGR_P_OTAG_KEEP_MASK	GENMASK(7, 6)
#define RTL838X_VLAN_PORT_TAG_STS_CTRL_IGR_P_ITAG_KEEP_MASK	GENMASK(5, 4)
#define RTL838X_VLAN_PORT_TAG_STS_CTRL_OTAG_STS_MASK		GENMASK(3, 2)
#define RTL838X_VLAN_PORT_TAG_STS_CTRL_ITAG_STS_MASK		GENMASK(1, 0)

#define RTL839X_VLAN_PORT_TAG_STS_UNTAG				0x0
#define RTL839X_VLAN_PORT_TAG_STS_TAGGED			0x1
#define RTL839X_VLAN_PORT_TAG_STS_PRIORITY_TAGGED		0x2

#define RTL839X_VLAN_PORT_TAG_STS_CTRL_BASE			0x6828
/* port 0-52 */
#define RTL839X_VLAN_PORT_TAG_STS_CTRL(port) \
	(RTL839X_VLAN_PORT_TAG_STS_CTRL_BASE + (port << 2))
#define RTL839X_VLAN_PORT_TAG_STS_CTRL_OTAG_STS_MASK		GENMASK(7, 6)
#define RTL839X_VLAN_PORT_TAG_STS_CTRL_ITAG_STS_MASK		GENMASK(5, 4)
#define RTL839X_VLAN_PORT_TAG_STS_CTRL_EGR_P_OTAG_KEEP_MASK	GENMASK(3, 3)
#define RTL839X_VLAN_PORT_TAG_STS_CTRL_EGR_P_ITAG_KEEP_MASK	GENMASK(2, 2)
#define RTL839X_VLAN_PORT_TAG_STS_CTRL_IGR_P_OTAG_KEEP_MASK	GENMASK(1, 1)
#define RTL839X_VLAN_PORT_TAG_STS_CTRL_IGR_P_ITAG_KEEP_MASK	GENMASK(0, 0)

#define RTL930X_VLAN_PORT_TAG_STS_INTERNAL			0x0
#define RTL930X_VLAN_PORT_TAG_STS_UNTAG				0x1
#define RTL930X_VLAN_PORT_TAG_STS_TAGGED			0x2
#define RTL930X_VLAN_PORT_TAG_STS_PRIORITY_TAGGED		0x3

#define RTL930X_VLAN_PORT_TAG_STS_CTRL_BASE			0xCE24
/* port 0-28 */
#define RTL930X_VLAN_PORT_TAG_STS_CTRL(port) \
	(RTL930X_VLAN_PORT_TAG_STS_CTRL_BASE + (port << 2))
#define RTL930X_VLAN_PORT_TAG_STS_CTRL_EGR_OTAG_STS_MASK	GENMASK(7, 6)
#define RTL930X_VLAN_PORT_TAG_STS_CTRL_EGR_ITAG_STS_MASK	GENMASK(5, 4)
#define RTL930X_VLAN_PORT_TAG_STS_CTRL_EGR_P_OTAG_KEEP_MASK	GENMASK(3, 3)
#define RTL930X_VLAN_PORT_TAG_STS_CTRL_EGR_P_ITAG_KEEP_MASK	GENMASK(2, 2)
#define RTL930X_VLAN_PORT_TAG_STS_CTRL_IGR_P_OTAG_KEEP_MASK	GENMASK(1, 1)
#define RTL930X_VLAN_PORT_TAG_STS_CTRL_IGR_P_ITAG_KEEP_MASK	GENMASK(0, 0)

#define RTL931X_VLAN_PORT_TAG_STS_INTERNAL			0x0
#define RTL931X_VLAN_PORT_TAG_STS_UNTAG				0x1
#define RTL931X_VLAN_PORT_TAG_STS_TAGGED			0x2
#define RTL931X_VLAN_PORT_TAG_STS_PRIORITY_TAGGED		0x3

#define RTL931X_VLAN_PORT_TAG_CTRL_BASE				0x4860
/* port 0-56 */
#define RTL931X_VLAN_PORT_TAG_CTRL(port) \
	(RTL931X_VLAN_PORT_TAG_CTRL_BASE + (port << 2))
#define RTL931X_VLAN_PORT_TAG_EGR_OTAG_STS_MASK			GENMASK(13, 12)
#define RTL931X_VLAN_PORT_TAG_EGR_ITAG_STS_MASK			GENMASK(11, 10)
#define RTL931X_VLAN_PORT_TAG_EGR_OTAG_KEEP_MASK		GENMASK(9, 9)
#define RTL931X_VLAN_PORT_TAG_EGR_ITAG_KEEP_MASK		GENMASK(8, 8)
#define RTL931X_VLAN_PORT_TAG_IGR_OTAG_KEEP_MASK		GENMASK(7, 7)
#define RTL931X_VLAN_PORT_TAG_IGR_ITAG_KEEP_MASK		GENMASK(6, 6)
#define RTL931X_VLAN_PORT_TAG_OTPID_IDX_MASK			GENMASK(5, 4)
#define RTL931X_VLAN_PORT_TAG_OTPID_KEEP_MASK			GENMASK(3, 3)
#define RTL931X_VLAN_PORT_TAG_ITPID_IDX_MASK			GENMASK(2, 1)
#define RTL931X_VLAN_PORT_TAG_ITPID_KEEP_MASK			GENMASK(0, 0)

static void rtldsa_vlan_set_pvid(struct rtl838x_switch_priv *priv,
				  int port, int pvid)
{
	/* Set both inner and outer PVID of the port */
	priv->r->vlan_port_pvid_set(port, PBVLAN_TYPE_INNER, pvid);
	priv->r->vlan_port_pvid_set(port, PBVLAN_TYPE_OUTER, pvid);
	priv->r->vlan_port_pvidmode_set(port, PBVLAN_TYPE_INNER,
					PBVLAN_MODE_UNTAG_AND_PRITAG);
	priv->r->vlan_port_pvidmode_set(port, PBVLAN_TYPE_OUTER,
					PBVLAN_MODE_UNTAG_AND_PRITAG);

	priv->ports[port].pvid = pvid;
}

/* Initialize all VLANS */
void rtldsa_vlan_setup(struct rtl838x_switch_priv *priv)
{
	struct rtldsa_vlan_info info = {
		.l2_tunnel_list_id = -1,
	};

	pr_info("In %s\n", __func__);

	priv->r->vlan_profile_setup(0);
	priv->r->vlan_profile_dump(priv, 0);

	/* Initialize normal VLANs 1-4095 */
	for (int i = 1; i < MAX_VLANS; i++)
		priv->r->vlan_set_tagged(i, &info);

	/*
	 * Initialize the special VLAN 0 and reset PVIDs. The CPU port PVID
	 * is applied to packets from the CPU for untagged destinations,
	 * regardless if the actual ingress VID. Any port with untagged
	 * egress VLAN(s) must therefore be a member of VLAN 0 to support
	 * CPU port as ingress when VLAN filtering is enabled.
	 */
	for (int i = 0; i <= priv->r->cpu_port; i++) {
		rtldsa_vlan_set_pvid(priv, i, 0);
		info.member_ports |= BIT_ULL(i);
	}
	priv->r->vlan_set_tagged(0, &info);

	/* Set forwarding action based on inner VLAN tag */
	for (int i = 0; i < priv->r->cpu_port; i++)
		priv->r->vlan_fwd_on_inner(i, true);
}

/* RTL838x VLAN operations */
void rtl838x_vlan_tables_read(u32 vlan, struct rtldsa_vlan_info *info)
{
	u32 buf[2];
	u32 untag;
	u32 v;

	otto_table_read(RTL8380_TBL_VLAN, vlan, &buf);
	info->member_ports = buf[0];
	v = buf[1];
	pr_debug("VLAN_READ %d: %016llx %08x\n", vlan, info->member_ports, v);

	info->profile_id = v & 0x7;
	info->hash_mc_fid = !!(v & 0x8);
	info->hash_uc_fid = !!(v & 0x10);
	info->fid = (v >> 5) & 0x3f;

	otto_table_read(RTL8380_TBL_UNTAG, vlan, &untag);
	info->untagged_ports = untag;
}

void rtl838x_vlan_set_tagged(u32 vlan, struct rtldsa_vlan_info *info)
{
	u32 buf[2];
	u32 v;

	buf[0] = info->member_ports;

	v = info->profile_id;
	v |= info->hash_mc_fid ? 0x8 : 0;
	v |= info->hash_uc_fid ? 0x10 : 0;
	v |= ((u32)info->fid) << 5;
	buf[1] = v;

	otto_table_write(RTL8380_TBL_VLAN, vlan, &buf);
}

void rtl838x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	u32 buf[1];

	buf[0] = portmask & RTL838X_MC_PMASK_ALL_PORTS;

	otto_table_write(RTL8380_TBL_UNTAG, vlan, &buf);
}

void rtl838x_vlan_fwd_on_inner(int port, bool is_set)
{
	if (is_set)
		sw_w32_mask(BIT(port), 0, RTL838X_VLAN_PORT_FWD);
	else
		sw_w32_mask(0, BIT(port), RTL838X_VLAN_PORT_FWD);
}

int
rtldsa_838x_vlan_profile_get(int idx, struct rtldsa_vlan_profile *profile)
{
	u32 p;

	if (idx < 0 || idx > RTL838X_VLAN_PROFILE_MAX)
		return -EINVAL;

	p = sw_r32(RTL838X_VLAN_PROFILE(idx));

	*profile = (struct rtldsa_vlan_profile) {
		.l2_learn = RTL838X_VLAN_L2_LEARN_EN_R(p),
		.unkn_mc_fld.pmsks_idx = {
			.l2 = RTL838X_VLAN_L2_UNKN_MC_FLD_PMSK(p),
			.ip = RTL838X_VLAN_IP4_UNKN_MC_FLD_PMSK(p),
			.ip6 = RTL838X_VLAN_IP6_UNKN_MC_FLD_PMSK(p),
		},
		.pmsk_is_idx = 1,
	};

	return 0;
}

void rtl838x_vlan_profile_setup(int profile)
{
	u32 p = RTL838X_VLAN_L2_LEARN_EN(1) |
		RTL838X_VLAN_L2_UNKN_MC_FLD(MC_PMASK_ALL_PORTS_IDX) |
		RTL838X_VLAN_IP4_UNKN_MC_FLD(MC_PMASK_ALL_PORTS_IDX) |
		RTL838X_VLAN_IP6_UNKN_MC_FLD(MC_PMASK_ALL_PORTS_IDX);

	sw_w32(p, RTL838X_VLAN_PROFILE(profile));
}

void rtl838x_vlan_port_keep_tag_set(int port, bool keep_outer, bool keep_inner)
{
	sw_w32(FIELD_PREP(RTL838X_VLAN_PORT_TAG_STS_CTRL_OTAG_STS_MASK,
			  keep_outer ? RTL838X_VLAN_PORT_TAG_STS_TAGGED : RTL838X_VLAN_PORT_TAG_STS_UNTAG) |
	       FIELD_PREP(RTL838X_VLAN_PORT_TAG_STS_CTRL_ITAG_STS_MASK,
			  keep_inner ? RTL838X_VLAN_PORT_TAG_STS_TAGGED : RTL838X_VLAN_PORT_TAG_STS_UNTAG),
	       RTL838X_VLAN_PORT_TAG_STS_CTRL(port));
}

void rtl838x_vlan_port_pvidmode_set(int port, enum pbvlan_type type, enum pbvlan_mode mode)
{
	if (type == PBVLAN_TYPE_INNER)
		sw_w32_mask(0x3, mode, RTL838X_VLAN_PORT_PB_VLAN + (port << 2));
	else
		sw_w32_mask(0x3 << 14, mode << 14, RTL838X_VLAN_PORT_PB_VLAN + (port << 2));
}

void rtl838x_vlan_port_pvid_set(int port, enum pbvlan_type type, int pvid)
{
	if (type == PBVLAN_TYPE_INNER)
		sw_w32_mask(0xfff << 2, pvid << 2, RTL838X_VLAN_PORT_PB_VLAN + (port << 2));
	else
		sw_w32_mask(0xfff << 16, pvid << 16, RTL838X_VLAN_PORT_PB_VLAN + (port << 2));
}

void
rtldsa_838x_vlan_profile_dump(struct rtl838x_switch_priv *priv, int idx)
{
	struct rtldsa_vlan_profile p;

	if (rtldsa_838x_vlan_profile_get(idx, &p) < 0)
		return;

	dev_dbg(priv->dev,
		"VLAN profile %d: L2 learning: %d, UNKN L2MC FLD PMSK %d, UNKN IPMC FLD PMSK %d, UNKN IPv6MC FLD PMSK: %d\n", idx,
		p.l2_learn, p.unkn_mc_fld.pmsks_idx.l2,
		p.unkn_mc_fld.pmsks_idx.ip, p.unkn_mc_fld.pmsks_idx.ip6);
}

/* RTL839x VLAN operations */
void rtl839x_vlan_tables_read(u32 vlan, struct rtldsa_vlan_info *info)
{
	u32 buf[3], untag[2];
	u32 u, v, w;

	otto_table_read(RTL8390_TBL_VLAN, vlan, &buf);
	u = buf[0];
	v = buf[1];
	w = buf[2];

	info->member_ports = u;
	info->member_ports = (info->member_ports << 21) | ((v >> 11) & 0x1fffff);
	info->profile_id = w >> 30 | ((v & 1) << 2);
	info->hash_mc_fid = !!(w & BIT(2));
	info->hash_uc_fid = !!(w & BIT(3));
	info->fid = (v >> 3) & 0xff;

	otto_table_read(RTL8390_TBL_UNTAG, vlan, &untag);
	u = untag[0];
	v = untag[1];

	info->untagged_ports = u;
	info->untagged_ports = (info->untagged_ports << 21) | ((v >> 11) & 0x1fffff);
}

void rtl839x_vlan_set_tagged(u32 vlan, struct rtldsa_vlan_info *info)
{
	u32 buf[3];
	u32 u, v, w;

	u = info->member_ports >> 21;
	v = info->member_ports << 11;
	v |= ((u32)info->fid) << 3;
	v |= info->hash_uc_fid ? BIT(2) : 0;
	v |= info->hash_mc_fid ? BIT(1) : 0;
	v |= (info->profile_id & 0x4) ? 1 : 0;
	w = ((u32)(info->profile_id & 3)) << 30;

	buf[0] = u;
	buf[1] = v;
	buf[2] = w;

	otto_table_write(RTL8390_TBL_VLAN, vlan, &buf);
}

void rtl839x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	u32 buf[2];
	u32 u, v;

	u = portmask >> 21;
	v = portmask << 11;

	buf[0] = u;
	buf[1] = v;

	otto_table_write(RTL8390_TBL_UNTAG, vlan, &buf);
}

void rtl839x_vlan_fwd_on_inner(int port, bool is_set)
{
	if (is_set)
		rtl839x_mask_port_reg_be(BIT_ULL(port), 0ULL, RTL839X_VLAN_PORT_FWD);
	else
		rtl839x_mask_port_reg_be(0ULL, BIT_ULL(port), RTL839X_VLAN_PORT_FWD);
}

int
rtldsa_839x_vlan_profile_get(int idx, struct rtldsa_vlan_profile *profile)
{
	u32 p[2];

	if (idx < 0 || idx > RTL839X_VLAN_PROFILE_MAX)
		return -EINVAL;

	p[0] = sw_r32(RTL839X_VLAN_PROFILE(idx));
	p[1] = sw_r32(RTL839X_VLAN_PROFILE(idx) + 4);

	*profile = (struct rtldsa_vlan_profile) {
		.l2_learn = RTL839X_VLAN_L2_LEARN_EN_R(p),
		.unkn_mc_fld.pmsks_idx = {
			.l2 = RTL839X_VLAN_L2_UNKN_MC_FLD_PMSK(p),
			.ip = RTL839X_VLAN_IP4_UNKN_MC_FLD_PMSK(p),
			.ip6 = RTL839X_VLAN_IP6_UNKN_MC_FLD_PMSK(p),
		},
		.pmsk_is_idx = 1,
	};

	return 0;
}

void rtl839x_vlan_profile_setup(int profile)
{
	u32 p[2] = { 0, 0 };

	p[1] = RTL839X_VLAN_L2_LEARN_EN(1);
	p[1] |= RTL839X_VLAN_L2_UNKN_MC_FLD(MC_PMASK_ALL_PORTS_IDX) |
		RTL839X_VLAN_IP4_UNKN_MC_FLD(MC_PMASK_ALL_PORTS_IDX);
	p[0] |= RTL839X_VLAN_IP6_UNKN_MC_FLD(MC_PMASK_ALL_PORTS_IDX);

	sw_w32(p[0], RTL839X_VLAN_PROFILE(profile));
	sw_w32(p[1], RTL839X_VLAN_PROFILE(profile) + 4);
}

void
rtldsa_839x_vlan_profile_dump(struct rtl838x_switch_priv *priv, int idx)
{
	struct rtldsa_vlan_profile p;

	if (rtldsa_839x_vlan_profile_get(idx, &p) < 0)
		return;

	dev_dbg(priv->dev,
		"VLAN profile %d: L2 learning: %d, UNKN L2MC FLD PMSK %d, UNKN IPMC FLD PMSK %d, UNKN IPv6MC FLD PMSK: %d\n"
		"VLAN profile %d: raw %08x, %08x\n", idx,
		p.l2_learn, p.unkn_mc_fld.pmsks_idx.l2,
		p.unkn_mc_fld.pmsks_idx.ip, p.unkn_mc_fld.pmsks_idx.ip6, idx,
		sw_r32(RTL839X_VLAN_PROFILE(idx)),
		sw_r32(RTL839X_VLAN_PROFILE(idx) + 4));
}

void rtl839x_vlan_port_keep_tag_set(int port, bool keep_outer, bool keep_inner)
{
	sw_w32(FIELD_PREP(RTL839X_VLAN_PORT_TAG_STS_CTRL_OTAG_STS_MASK,
			  keep_outer ? RTL839X_VLAN_PORT_TAG_STS_TAGGED : RTL839X_VLAN_PORT_TAG_STS_UNTAG) |
	       FIELD_PREP(RTL839X_VLAN_PORT_TAG_STS_CTRL_ITAG_STS_MASK,
			  keep_inner ? RTL839X_VLAN_PORT_TAG_STS_TAGGED : RTL839X_VLAN_PORT_TAG_STS_UNTAG),
	       RTL839X_VLAN_PORT_TAG_STS_CTRL(port));
}

void rtl839x_vlan_port_pvidmode_set(int port, enum pbvlan_type type, enum pbvlan_mode mode)
{
	if (type == PBVLAN_TYPE_INNER)
		sw_w32_mask(0x3, mode, RTL839X_VLAN_PORT_PB_VLAN + (port << 2));
	else
		sw_w32_mask(0x3 << 14, mode << 14, RTL839X_VLAN_PORT_PB_VLAN + (port << 2));
}

void rtl839x_vlan_port_pvid_set(int port, enum pbvlan_type type, int pvid)
{
	if (type == PBVLAN_TYPE_INNER)
		sw_w32_mask(0xfff << 2, pvid << 2, RTL839X_VLAN_PORT_PB_VLAN + (port << 2));
	else
		sw_w32_mask(0xfff << 16, pvid << 16, RTL839X_VLAN_PORT_PB_VLAN + (port << 2));
}

/* RTL930x VLAN operations */
void rtl930x_vlan_tables_read(u32 vlan, struct rtldsa_vlan_info *info)
{
	u32 buf[2];
	u32 v, w;

	otto_table_read(RTL9300_TBL_VLAN, vlan, &buf);
	v = buf[0];
	w = buf[1];
	pr_debug("VLAN_READ %d: %08x %08x\n", vlan, v, w);

	info->member_ports = v >> 3;
	info->profile_id = (w >> 24) & 7;
	info->hash_mc_fid = !!(w & BIT(27));
	info->hash_uc_fid = !!(w & BIT(28));
	info->fid = ((v & 0x7) << 3) | ((w >> 29) & 0x7);

	otto_table_read(RTL9300_TBL_UNTAG, vlan, &v);

	info->untagged_ports = v >> 3;
}

void rtl930x_vlan_set_tagged(u32 vlan, struct rtldsa_vlan_info *info)
{
	u32 v, w;
	u32 buf[2];

	v = info->member_ports << 3;
	v |= ((u32)info->fid) >> 3;

	w = ((u32)info->fid) << 29;
	w |= info->hash_mc_fid ? BIT(27) : 0;
	w |= info->hash_uc_fid ? BIT(28) : 0;
	w |= info->profile_id << 24;

	buf[0] = v;
	buf[1] = w;
	otto_table_write(RTL9300_TBL_VLAN, vlan, &buf);
}

int
rtldsa_930x_vlan_profile_get(int idx, struct rtldsa_vlan_profile *profile)
{
	u32 p[5];

	if (idx < 0 || idx > RTL930X_VLAN_PROFILE_MAX)
		return -EINVAL;

	for (int i = 0; i < 5; i++)
		p[i] = sw_r32(RTL930X_VLAN_PROFILE_SET(idx) + i * 4);

	*profile = (struct rtldsa_vlan_profile) {
		.l2_learn = RTL930X_VLAN_L2_LEARN_EN_R(p),
		.unkn_mc_fld.pmsks = {
			.l2 = RTL930X_VLAN_L2_UNKN_MC_FLD_PMSK(p),
			.ip = RTL930X_VLAN_IP4_UNKN_MC_FLD_PMSK(p),
			.ip6 = RTL930X_VLAN_IP6_UNKN_MC_FLD_PMSK(p),
		},
		.pmsk_is_idx = 0,
		.routing_ipuc = p[0] & BIT(17),
		.routing_ip6uc = p[0] & BIT(16),
		.routing_ipmc = p[0] & BIT(13),
		.routing_ip6mc = p[0] & BIT(12),
		.bridge_ipmc = p[0] & BIT(15),
		.bridge_ip6mc = p[0] & BIT(14),
	};

	return 0;
}

void
rtldsa_930x_vlan_profile_dump(struct rtl838x_switch_priv *priv, int idx)
{
	struct rtldsa_vlan_profile p;

	if (rtldsa_930x_vlan_profile_get(idx, &p) < 0)
		return;

	dev_dbg(priv->dev,
		"VLAN %d: L2 learn: %d; Unknown MC PMasks: L2 %llx, IPv4 %llx, IPv6: %llx\n"
		"  Routing enabled: IPv4 UC %c, IPv6 UC %c, IPv4 MC %c, IPv6 MC %c\n"
		"  Bridge enabled: IPv4 MC %c, IPv6 MC %c\n"
		"VLAN profile %d: raw %08x %08x %08x %08x %08x\n",
		idx, p.l2_learn, p.unkn_mc_fld.pmsks.l2,
		p.unkn_mc_fld.pmsks.ip, p.unkn_mc_fld.pmsks.ip6,
		p.routing_ipuc ? 'y' : 'n', p.routing_ip6uc ? 'y' : 'n',
		p.routing_ipmc ? 'y' : 'n', p.routing_ip6mc ? 'y' : 'n',
		p.bridge_ipmc ? 'y' : 'n', p.bridge_ip6mc ? 'y' : 'n', idx,
		sw_r32(RTL930X_VLAN_PROFILE_SET(idx)),
		sw_r32(RTL930X_VLAN_PROFILE_SET(idx) + 4),
		sw_r32(RTL930X_VLAN_PROFILE_SET(idx) + 8) & 0x1FFFFFFF,
		sw_r32(RTL930X_VLAN_PROFILE_SET(idx) + 12) & 0x1FFFFFFF,
		sw_r32(RTL930X_VLAN_PROFILE_SET(idx) + 16) & 0x1FFFFFFF);
}

void rtl930x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	u32 v = portmask << 3;

	otto_table_write(RTL9300_TBL_UNTAG, vlan, &v);
}

void rtl930x_vlan_fwd_on_inner(int port, bool is_set)
{
	/* Always set all tag modes to fwd based on either inner or outer tag */
	if (is_set)
		sw_w32_mask(0xf, 0, RTL930X_VLAN_PORT_FWD + (port << 2));
	else
		sw_w32_mask(0, 0xf, RTL930X_VLAN_PORT_FWD + (port << 2));
}

void rtl930x_vlan_profile_setup(int profile)
{
	u32 p[5];

	pr_debug("In %s\n", __func__);
	p[0] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile));
	p[1] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile) + 4);

	/* Enable routing of Ipv4/6 Unicast and IPv4/6 Multicast traffic */
	p[0] |= BIT(17) | BIT(16) | BIT(13) | BIT(12);

	p[2] = RTL930X_VLAN_L2_UNKN_MC_FLD(RTL930X_MC_PMASK_ALL_PORTS);
	p[3] = RTL930X_VLAN_IP4_UNKN_MC_FLD(RTL930X_MC_PMASK_ALL_PORTS);
	p[4] = RTL930X_VLAN_IP6_UNKN_MC_FLD(RTL930X_MC_PMASK_ALL_PORTS);

	sw_w32(p[0], RTL930X_VLAN_PROFILE_SET(profile));
	sw_w32(p[1], RTL930X_VLAN_PROFILE_SET(profile) + 4);
	sw_w32(p[2], RTL930X_VLAN_PROFILE_SET(profile) + 8);
	sw_w32(p[3], RTL930X_VLAN_PROFILE_SET(profile) + 12);
	sw_w32(p[4], RTL930X_VLAN_PROFILE_SET(profile) + 16);
}

void rtl930x_vlan_port_keep_tag_set(int port, bool keep_outer, bool keep_inner)
{
	sw_w32(FIELD_PREP(RTL930X_VLAN_PORT_TAG_STS_CTRL_EGR_OTAG_STS_MASK,
			  keep_outer ? RTL930X_VLAN_PORT_TAG_STS_TAGGED : RTL930X_VLAN_PORT_TAG_STS_UNTAG) |
	       FIELD_PREP(RTL930X_VLAN_PORT_TAG_STS_CTRL_EGR_ITAG_STS_MASK,
			  keep_inner ? RTL930X_VLAN_PORT_TAG_STS_TAGGED : RTL930X_VLAN_PORT_TAG_STS_UNTAG),
	       RTL930X_VLAN_PORT_TAG_STS_CTRL(port));
}

void rtl930x_vlan_port_pvidmode_set(int port, enum pbvlan_type type, enum pbvlan_mode mode)
{
	if (type == PBVLAN_TYPE_INNER)
		sw_w32_mask(0x3, mode, RTL930X_VLAN_PORT_PB_VLAN + (port << 2));
	else
		sw_w32_mask(0x3 << 14, mode << 14, RTL930X_VLAN_PORT_PB_VLAN + (port << 2));
}

void rtl930x_vlan_port_pvid_set(int port, enum pbvlan_type type, int pvid)
{
	if (type == PBVLAN_TYPE_INNER)
		sw_w32_mask(0xfff << 2, pvid << 2, RTL930X_VLAN_PORT_PB_VLAN + (port << 2));
	else
		sw_w32_mask(0xfff << 16, pvid << 16, RTL930X_VLAN_PORT_PB_VLAN + (port << 2));
}

/* RTL931x VLAN operations */
int
rtldsa_931x_vlan_profile_get(int idx, struct rtldsa_vlan_profile *profile)
{
	u32 p[7];

	if (idx < 0 || idx > RTL931X_VLAN_PROFILE_MAX)
		return -EINVAL;

	for (int i = 0; i < 7; i++)
		p[i] = sw_r32(RTL931X_VLAN_PROFILE_SET(idx) + i * 4);

	*profile = (struct rtldsa_vlan_profile) {
		.l2_learn = RTL931X_VLAN_L2_LEARN_EN_R(p),
		.unkn_mc_fld.pmsks = {
			.l2 = RTL931X_VLAN_L2_UNKN_MC_FLD_PMSK(p),
			.ip = RTL931X_VLAN_IP4_UNKN_MC_FLD_PMSK(p),
			.ip6 = RTL931X_VLAN_IP6_UNKN_MC_FLD_PMSK(p),
		},
	};

	return 0;
}

void
rtldsa_931x_vlan_profile_dump(struct rtl838x_switch_priv *priv, int idx)
{
	struct rtldsa_vlan_profile p;

	if (rtldsa_931x_vlan_profile_get(idx, &p) < 0)
		return;

	dev_dbg(priv->dev,
		"VLAN %d: L2 learning: %d, L2 Unknown MultiCast Field %llx, IPv4 Unknown MultiCast Field %llx, IPv6 Unknown MultiCast Field: %llx\n",
		idx, p.l2_learn, p.unkn_mc_fld.pmsks.l2,
		p.unkn_mc_fld.pmsks.ip, p.unkn_mc_fld.pmsks.ip6);
}

void rtl931x_vlan_tables_read(u32 vlan, struct rtldsa_vlan_info *info)
{
	u32 v, w, x, y;
	u32 buf[4], buf2[2];

	otto_table_read(RTL9310_TBL_VLAN, vlan, &buf);
	v = buf[0];
	w = buf[1];
	x = buf[2];
	y = buf[3];

	pr_debug("VLAN_READ %d: %08x %08x %08x %08x\n", vlan, v, w, x, y);
	info->member_ports = ((u64)v) << 25 | (w >> 7);
	info->profile_id = (x >> 16) & 0xf;
	info->fid = w & 0x7f;				/* AKA MSTI depending on context */
	info->hash_uc_fid = !!(x & BIT(31));
	info->hash_mc_fid = !!(x & BIT(30));
	info->if_id = (x >> 20) & 0x3ff;
	info->multicast_grp_mask = x & 0xffff;
	if (y & BIT(31))
		info->l2_tunnel_list_id = y >> 18;
	else
		info->l2_tunnel_list_id = -1;
	pr_debug("%s read member %016llx, profile-id %d, uc %d, mc %d, intf-id %d\n", __func__,
		 info->member_ports, info->profile_id, info->hash_uc_fid, info->hash_mc_fid,
		 info->if_id);

	otto_table_read(RTL9310_TBL_VLAN_UNTAG, vlan, &buf2);
	info->untagged_ports = ((u64)buf2[0]) << 25;
	info->untagged_ports |= buf2[1] >> 7;
}

void rtl931x_vlan_set_tagged(u32 vlan, struct rtldsa_vlan_info *info)
{
	u32 v, w, x, y;
	u32 buf[4];

	v = info->member_ports >> 25;
	w = (info->member_ports & GENMASK(24, 0)) << 7;
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

	buf[0] = v;
	buf[1] = w;
	buf[2] = x;
	buf[3] = y;

	otto_table_write(RTL9310_TBL_VLAN, vlan, &buf);
}

void rtl931x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	u32 buf[2] = { portmask >> (32 - 7), portmask << 7 };

	otto_table_write(RTL9310_TBL_VLAN_UNTAG, vlan, &buf);
}

void rtl931x_vlan_fwd_on_inner(int port, bool is_set)
{
	/* Always set all tag modes to fwd based on either inner or outer tag */
	if (is_set)
		sw_w32_mask(0xf, 0, RTL931X_VLAN_PORT_FWD + (port << 2));
	else
		sw_w32_mask(0, 0xf, RTL931X_VLAN_PORT_FWD + (port << 2));
}

void rtl931x_vlan_profile_setup(int profile)
{
	u32 p[7];

	pr_debug("In %s\n", __func__);

	if (profile > 15)
		return;

	p[0] = sw_r32(RTL931X_VLAN_PROFILE_SET(profile));

	/* Enable routing of Ipv4/6 Unicast and IPv4/6 Multicast traffic */
	/* p[0] |= BIT(17) | BIT(16) | BIT(13) | BIT(12); */
	p[0] |= 0x3 << 11; /* COPY2CPU */

	p[1] = RTL931X_VLAN_L2_UNKN_MC_FLD_H(RTL931X_MC_PMASK_ALL_PORTS);
	p[2] = RTL931X_VLAN_L2_UNKN_MC_FLD_L(RTL931X_MC_PMASK_ALL_PORTS);
	p[3] = RTL931X_VLAN_IP4_UNKN_MC_FLD_H(RTL931X_MC_PMASK_ALL_PORTS);
	p[4] = RTL931X_VLAN_IP4_UNKN_MC_FLD_L(RTL931X_MC_PMASK_ALL_PORTS);
	p[5] = RTL931X_VLAN_IP6_UNKN_MC_FLD_H(RTL931X_MC_PMASK_ALL_PORTS);
	p[6] = RTL931X_VLAN_IP6_UNKN_MC_FLD_L(RTL931X_MC_PMASK_ALL_PORTS);

	for (int i = 0; i < 7; i++)
		sw_w32(p[i], RTL931X_VLAN_PROFILE_SET(profile) + i * 4);
	pr_debug("Leaving %s\n", __func__);
}

void rtl931x_vlan_port_keep_tag_set(int port, bool keep_outer, bool keep_inner)
{
	sw_w32(FIELD_PREP(RTL931X_VLAN_PORT_TAG_EGR_OTAG_STS_MASK,
			  keep_outer ? RTL931X_VLAN_PORT_TAG_STS_TAGGED : RTL931X_VLAN_PORT_TAG_STS_UNTAG) |
	       FIELD_PREP(RTL931X_VLAN_PORT_TAG_EGR_ITAG_STS_MASK,
			  keep_inner ? RTL931X_VLAN_PORT_TAG_STS_TAGGED : RTL931X_VLAN_PORT_TAG_STS_UNTAG),
	       RTL931X_VLAN_PORT_TAG_CTRL(port));
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

static int rtldsa_vlan_prepare(struct dsa_switch *ds, int port,
			       const struct switchdev_obj_port_vlan *vlan)
{
	struct rtldsa_vlan_info info;
	struct rtl838x_switch_priv *priv = ds->priv;

	priv->r->vlan_tables_read(0, &info);

	pr_debug("VLAN 0: Member ports %llx, untag %llx, profile %d, MC# %d, UC# %d, FID %x\n",
		 info.member_ports, info.untagged_ports, info.profile_id,
		 info.hash_mc_fid, info.hash_uc_fid, info.fid);

	priv->r->vlan_tables_read(1, &info);
	pr_debug("VLAN 1: Member ports %llx, untag %llx, profile %d, MC# %d, UC# %d, FID %x\n",
		 info.member_ports, info.untagged_ports, info.profile_id,
		 info.hash_mc_fid, info.hash_uc_fid, info.fid);
	priv->r->vlan_set_untagged(1, info.untagged_ports);
	pr_debug("SET: Untagged ports, VLAN %d: %llx\n", 1, info.untagged_ports);

	priv->r->vlan_set_tagged(1, &info);
	pr_debug("SET: Member ports, VLAN %d: %llx\n", 1, info.member_ports);

	return 0;
}

int rtldsa_vlan_filtering(struct dsa_switch *ds, int port,
				 bool vlan_filtering,
				 struct netlink_ext_ack *extack)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("%s: port %d\n", __func__, port);
	mutex_lock(&priv->reg_mutex);

	if (vlan_filtering) {
		/* Enable ingress and egress filtering
		 * The VLAN_PORT_IGR_FILTER register uses 2 bits for each port to define
		 * the filter action:
		 * 0: Always Forward
		 * 1: Drop packet
		 * 2: Trap packet to CPU port
		 * The Egress filter used 1 bit per state (0: DISABLED, 1: ENABLED)
		 */
		if (port != priv->r->cpu_port) {
			priv->r->set_vlan_igr_filter(port, IGR_DROP);
			priv->r->set_vlan_egr_filter(port, EGR_ENABLE);
		} else {
			priv->r->set_vlan_igr_filter(port, IGR_TRAP);
			priv->r->set_vlan_egr_filter(port, EGR_DISABLE);
		}

	} else {
		/* Disable ingress and egress filtering */
		if (port != priv->r->cpu_port)
			priv->r->set_vlan_igr_filter(port, IGR_FORWARD);

		priv->r->set_vlan_egr_filter(port, EGR_DISABLE);
	}

	/* Do we need to do something to the CPU-Port, too? */
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

int rtldsa_vlan_add(struct dsa_switch *ds, int port,
			   const struct switchdev_obj_port_vlan *vlan,
			   struct netlink_ext_ack *extack)
{
	struct rtldsa_vlan_info info;
	struct rtl838x_switch_priv *priv = ds->priv;
	int err;

	pr_debug("%s port %d, vid %d, flags %x\n",
		 __func__, port, vlan->vid, vlan->flags);

	/* Let no one mess with our special VLAN 0 */
	if (!vlan->vid)
		return 0;

	if (vlan->vid >= MAX_VLANS) {
		dev_err(priv->dev, "VLAN out of range: %d", vlan->vid);
		return -ENOTSUPP;
	}

	err = rtldsa_vlan_prepare(ds, port, vlan);
	if (err)
		return err;

	mutex_lock(&priv->reg_mutex);

	/*
	 * Realtek switches copy frames as-is to/from the CPU. For a proper
	 * VLAN handling the 12 bit RVID field (= VLAN id) for incoming traffic
	 * and the 1 bit RVID_SEL field (0 = use inner tag, 1 = use outer tag)
	 * for outgoing traffic of the CPU tag structure need to be handled. As
	 * of now no such logic is in place. So for the CPU port keep the fixed
	 * PVID=0 from initial setup in place and ignore all subsequent settings.
	 */
	if (port != priv->r->cpu_port) {
		if (vlan->flags & BRIDGE_VLAN_INFO_PVID)
			rtldsa_vlan_set_pvid(priv, port, vlan->vid);
		else if (priv->ports[port].pvid == vlan->vid)
			rtldsa_vlan_set_pvid(priv, port, 0);
	}

	/* Get port memberships of this vlan */
	priv->r->vlan_tables_read(vlan->vid, &info);

	/* new VLAN? */
	if (!info.member_ports) {
		info.fid = 0;
		info.hash_mc_fid = false;
		info.hash_uc_fid = false;
		info.profile_id = 0;
	}

	/* sanitize untagged_ports - must be a subset */
	if (info.untagged_ports & ~info.member_ports)
		info.untagged_ports = 0;

	info.member_ports |= BIT_ULL(port);
	if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED)
		info.untagged_ports |= BIT_ULL(port);
	else
		info.untagged_ports &= ~BIT_ULL(port);

	priv->r->vlan_set_untagged(vlan->vid, info.untagged_ports);
	pr_debug("Untagged ports, VLAN %d: %llx\n", vlan->vid, info.untagged_ports);

	priv->r->vlan_set_tagged(vlan->vid, &info);
	pr_debug("Member ports, VLAN %d: %llx\n", vlan->vid, info.member_ports);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

int rtldsa_vlan_del(struct dsa_switch *ds, int port,
			   const struct switchdev_obj_port_vlan *vlan)
{
	struct rtldsa_vlan_info info;
	struct rtl838x_switch_priv *priv = ds->priv;
	u16 pvid;

	pr_debug("%s: port %d, vid %d, flags %x\n",
		 __func__, port, vlan->vid, vlan->flags);

	/* Let no one mess with our special VLAN 0 */
	if (!vlan->vid)
		return 0;

	if (vlan->vid >= MAX_VLANS) {
		dev_err(priv->dev, "VLAN out of range: %d", vlan->vid);
		return -ENOTSUPP;
	}

	mutex_lock(&priv->reg_mutex);
	pvid = priv->ports[port].pvid;

	/* Reset to default if removing the current PVID */
	if (vlan->vid == pvid)
		rtldsa_vlan_set_pvid(priv, port, 0);

	/* Get port memberships of this vlan */
	priv->r->vlan_tables_read(vlan->vid, &info);

	/* remove port from both tables */
	info.untagged_ports &= (~BIT_ULL(port));
	info.member_ports &= (~BIT_ULL(port));

	/* VLANs without members are set back (implicitly) to CIST by DSA */
	if (!info.member_ports) {
		u16 mst = info.fid;

		info.fid = 0;

		rtldsa_mst_put_slot(priv, mst);
	}

	priv->r->vlan_set_untagged(vlan->vid, info.untagged_ports);
	pr_debug("Untagged ports, VLAN %d: %llx\n", vlan->vid, info.untagged_ports);

	priv->r->vlan_set_tagged(vlan->vid, &info);
	pr_debug("Member ports, VLAN %d: %llx\n", vlan->vid, info.member_ports);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

int rtldsa_port_vlan_fast_age(struct dsa_switch *ds, int port, u16 vid)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	int ret;

	if (!priv->r->fast_age)
		return -EOPNOTSUPP;

	mutex_lock(&priv->reg_mutex);
	ret = priv->r->fast_age(priv, port, vid);
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

int rtldsa_vlan_msti_set(struct dsa_switch *ds, struct dsa_bridge bridge,
				const struct switchdev_vlan_msti *msti)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	struct rtldsa_vlan_info info;
	u16 mst_slot_old;
	int mst_slot;

	priv->r->vlan_tables_read(msti->vid, &info);
	mst_slot_old = info.fid;

	/* find HW slot for MSTI */
	mutex_lock(&priv->reg_mutex);
	mst_slot = rtldsa_mst_replace(priv, msti->msti, mst_slot_old);
	mutex_unlock(&priv->reg_mutex);

	if (mst_slot < 0)
		return mst_slot;

	info.fid = mst_slot;
	priv->r->vlan_set_tagged(msti->vid, &info);

	return 0;
}
