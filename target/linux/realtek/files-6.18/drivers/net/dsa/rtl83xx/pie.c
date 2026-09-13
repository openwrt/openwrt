// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/etherdevice.h>
#include <linux/iopoll.h>

#include "pie.h"
#include "rtl-otto.h"

/* RTL838X */

/* see_dal_maple_acl_log2PhyTmplteField and src/app/diag_v2/src/diag_acl.c */
/* Definition of the RTL838X-specific template field IDs as used in the PIE */
enum rtl838x_template_field_id {
	RTL838X_TEMPLATE_FIELD_SPMMASK = 0,
	RTL838X_TEMPLATE_FIELD_SPM0 = 1,	/* Source portmask ports 0-15 */
	RTL838X_TEMPLATE_FIELD_SPM1 = 2,	/* Source portmask ports 16-28 */
	RTL838X_TEMPLATE_FIELD_RANGE_CHK = 3,
	RTL838X_TEMPLATE_FIELD_DMAC0 = 4,	/* Destination MAC [15:0] */
	RTL838X_TEMPLATE_FIELD_DMAC1 = 5,	/* Destination MAC [31:16] */
	RTL838X_TEMPLATE_FIELD_DMAC2 = 6,	/* Destination MAC [47:32] */
	RTL838X_TEMPLATE_FIELD_SMAC0 = 7,	/* Source MAC [15:0] */
	RTL838X_TEMPLATE_FIELD_SMAC1 = 8,	/* Source MAC [31:16] */
	RTL838X_TEMPLATE_FIELD_SMAC2 = 9,	/* Source MAC [47:32] */
	RTL838X_TEMPLATE_FIELD_ETHERTYPE = 10,	/* Ethernet typ */
	RTL838X_TEMPLATE_FIELD_OTAG = 11,	/* Outer VLAN tag */
	RTL838X_TEMPLATE_FIELD_ITAG = 12,	/* Inner VLAN tag */
	RTL838X_TEMPLATE_FIELD_SIP0 = 13,	/* IPv4 or IPv6 source IP[15:0] or ARP/RARP */
					/* source protocol address in header */
	RTL838X_TEMPLATE_FIELD_SIP1 = 14,	/* IPv4 or IPv6 source IP[31:16] or ARP/RARP */
	RTL838X_TEMPLATE_FIELD_DIP0 = 15,	/* IPv4 or IPv6 destination IP[15:0] */
	RTL838X_TEMPLATE_FIELD_DIP1 = 16,	/* IPv4 or IPv6 destination IP[31:16] */
	RTL838X_TEMPLATE_FIELD_IP_TOS_PROTO = 17, /* IPv4 TOS/IPv6 traffic class and */
					  /* IPv4 proto/IPv6 next header fields */
	RTL838X_TEMPLATE_FIELD_L34_HEADER = 18,	/* packet with extra tag and IPv6 with auth, dest, */
					/* frag, route, hop-by-hop option header, */
					/* IGMP type, TCP flag */
	RTL838X_TEMPLATE_FIELD_L4_SPORT = 19,	/* TCP/UDP source port */
	RTL838X_TEMPLATE_FIELD_L4_DPORT = 20,	/* TCP/UDP destination port */
	RTL838X_TEMPLATE_FIELD_ICMP_IGMP = 21,
	RTL838X_TEMPLATE_FIELD_IP_RANGE = 22,
	RTL838X_TEMPLATE_FIELD_FIELD_SELECTOR_VALID = 23, /* Field selector mask */
	RTL838X_TEMPLATE_FIELD_FIELD_SELECTOR_0 = 24,
	RTL838X_TEMPLATE_FIELD_FIELD_SELECTOR_1 = 25,
	RTL838X_TEMPLATE_FIELD_FIELD_SELECTOR_2 = 26,
	RTL838X_TEMPLATE_FIELD_FIELD_SELECTOR_3 = 27,
	RTL838X_TEMPLATE_FIELD_SIP2 = 28,	/* IPv6 source IP[47:32] */
	RTL838X_TEMPLATE_FIELD_SIP3 = 29,	/* IPv6 source IP[63:48] */
	RTL838X_TEMPLATE_FIELD_SIP4 = 30,	/* IPv6 source IP[79:64] */
	RTL838X_TEMPLATE_FIELD_SIP5 = 31,	/* IPv6 source IP[95:80] */
	RTL838X_TEMPLATE_FIELD_SIP6 = 32,	/* IPv6 source IP[111:96] */
	RTL838X_TEMPLATE_FIELD_SIP7 = 33,	/* IPv6 source IP[127:112] */
	RTL838X_TEMPLATE_FIELD_DIP2 = 34,	/* IPv6 destination IP[47:32] */
	RTL838X_TEMPLATE_FIELD_DIP3 = 35,	/* IPv6 destination IP[63:48] */
	RTL838X_TEMPLATE_FIELD_DIP4 = 36,	/* IPv6 destination IP[79:64] */
	RTL838X_TEMPLATE_FIELD_DIP5 = 37,	/* IPv6 destination IP[95:80] */
	RTL838X_TEMPLATE_FIELD_DIP6 = 38,	/* IPv6 destination IP[111:96] */
	RTL838X_TEMPLATE_FIELD_DIP7 = 39,	/* IPv6 destination IP[127:112] */
	RTL838X_TEMPLATE_FIELD_FWD_VID = 40,	/* Forwarding VLAN-ID */
	RTL838X_TEMPLATE_FIELD_FLOW_LABEL = 41,
};

/* The RTL838X SoCs use 5 fixed templates with definitions for which data fields are to
 * be copied from the Ethernet Frame header into the 12 User-definable fields of the Packet
 * Inspection Engine's buffer. The following defines the field contents for each of the fixed
 * templates. Additionally, 3 user-definable templates can be set up via the definitions
 * in RTL838X_ACL_TMPLTE_CTRL control registers.
 * TODO: See all src/app/diag_v2/src/diag_pie.c
 */
#define RTL838X_N_FIXED_TEMPLATES 5
static enum rtl838x_template_field_id rtl838x_fixed_templates[RTL838X_N_FIXED_TEMPLATES][N_FIXED_FIELDS] = {
	{
	  RTL838X_TEMPLATE_FIELD_SPM0, RTL838X_TEMPLATE_FIELD_SPM1, RTL838X_TEMPLATE_FIELD_OTAG,
	  RTL838X_TEMPLATE_FIELD_SMAC0, RTL838X_TEMPLATE_FIELD_SMAC1, RTL838X_TEMPLATE_FIELD_SMAC2,
	  RTL838X_TEMPLATE_FIELD_DMAC0, RTL838X_TEMPLATE_FIELD_DMAC1, RTL838X_TEMPLATE_FIELD_DMAC2,
	  RTL838X_TEMPLATE_FIELD_ETHERTYPE, RTL838X_TEMPLATE_FIELD_ITAG, RTL838X_TEMPLATE_FIELD_RANGE_CHK
	}, {
	  RTL838X_TEMPLATE_FIELD_SIP0, RTL838X_TEMPLATE_FIELD_SIP1, RTL838X_TEMPLATE_FIELD_DIP0,
	  RTL838X_TEMPLATE_FIELD_DIP1, RTL838X_TEMPLATE_FIELD_IP_TOS_PROTO, RTL838X_TEMPLATE_FIELD_L4_SPORT,
	  RTL838X_TEMPLATE_FIELD_L4_DPORT, RTL838X_TEMPLATE_FIELD_ICMP_IGMP, RTL838X_TEMPLATE_FIELD_ITAG,
	  RTL838X_TEMPLATE_FIELD_RANGE_CHK, RTL838X_TEMPLATE_FIELD_SPM0, RTL838X_TEMPLATE_FIELD_SPM1
	}, {
	  RTL838X_TEMPLATE_FIELD_DMAC0, RTL838X_TEMPLATE_FIELD_DMAC1, RTL838X_TEMPLATE_FIELD_DMAC2,
	  RTL838X_TEMPLATE_FIELD_ITAG, RTL838X_TEMPLATE_FIELD_ETHERTYPE, RTL838X_TEMPLATE_FIELD_IP_TOS_PROTO,
	  RTL838X_TEMPLATE_FIELD_L4_DPORT, RTL838X_TEMPLATE_FIELD_L4_SPORT, RTL838X_TEMPLATE_FIELD_SIP0,
	  RTL838X_TEMPLATE_FIELD_SIP1, RTL838X_TEMPLATE_FIELD_DIP0, RTL838X_TEMPLATE_FIELD_DIP1
	}, {
	  RTL838X_TEMPLATE_FIELD_DIP0, RTL838X_TEMPLATE_FIELD_DIP1, RTL838X_TEMPLATE_FIELD_DIP2,
	  RTL838X_TEMPLATE_FIELD_DIP3, RTL838X_TEMPLATE_FIELD_DIP4, RTL838X_TEMPLATE_FIELD_DIP5,
	  RTL838X_TEMPLATE_FIELD_DIP6, RTL838X_TEMPLATE_FIELD_DIP7, RTL838X_TEMPLATE_FIELD_L4_DPORT,
	  RTL838X_TEMPLATE_FIELD_L4_SPORT, RTL838X_TEMPLATE_FIELD_ICMP_IGMP, RTL838X_TEMPLATE_FIELD_IP_TOS_PROTO
	}, {
	  RTL838X_TEMPLATE_FIELD_SIP0, RTL838X_TEMPLATE_FIELD_SIP1, RTL838X_TEMPLATE_FIELD_SIP2,
	  RTL838X_TEMPLATE_FIELD_SIP3, RTL838X_TEMPLATE_FIELD_SIP4, RTL838X_TEMPLATE_FIELD_SIP5,
	  RTL838X_TEMPLATE_FIELD_SIP6, RTL838X_TEMPLATE_FIELD_SIP7, RTL838X_TEMPLATE_FIELD_ITAG,
	  RTL838X_TEMPLATE_FIELD_RANGE_CHK, RTL838X_TEMPLATE_FIELD_SPM0, RTL838X_TEMPLATE_FIELD_SPM1
	},
};

static void rtl838x_pie_lookup_enable(struct rtl838x_switch_priv *priv, int index)
{
	int block = index / PIE_BLOCK_SIZE;
	u32 block_state = sw_r32(RTL838X_ACL_BLK_LOOKUP_CTRL);

	/* Make sure rule-lookup is enabled in the block */
	if (!(block_state & BIT(block)))
		sw_w32(block_state | BIT(block), RTL838X_ACL_BLK_LOOKUP_CTRL);
}

static void rtl838x_pie_rule_del(struct rtl838x_switch_priv *priv, int index_from, int index_to)
{
	int block_from = index_from / PIE_BLOCK_SIZE;
	int block_to = index_to / PIE_BLOCK_SIZE;
	u32 v = (index_from << 1) | (index_to << 12) | BIT(0);
	u32 block_state;

	pr_debug("%s: from %d to %d\n", __func__, index_from, index_to);
	mutex_lock(&priv->reg_mutex);

	/* Remember currently active blocks */
	block_state = sw_r32(RTL838X_ACL_BLK_LOOKUP_CTRL);

	/* Make sure rule-lookup is disabled in the relevant blocks */
	for (int block = block_from; block <= block_to; block++) {
		if (block_state & BIT(block))
			sw_w32(block_state & (~BIT(block)), RTL838X_ACL_BLK_LOOKUP_CTRL);
	}

	/* Write from-to and execute bit into control register */
	sw_w32(v, RTL838X_ACL_CLR_CTRL);

	/* Wait until command has completed */
	do {
	} while (sw_r32(RTL838X_ACL_CLR_CTRL) & BIT(0));

	/* Re-enable rule lookup */
	for (int block = block_from; block <= block_to; block++) {
		if (!(block_state & BIT(block)))
			sw_w32(block_state | BIT(block), RTL838X_ACL_BLK_LOOKUP_CTRL);
	}

	mutex_unlock(&priv->reg_mutex);
}

/* Reads the intermediate representation of the templated match-fields of the
 * PIE rule in the pie_rule structure and fills in the raw data fields in the
 * raw register space r[].
 * The register space configuration size is identical for the RTL8380/90 and RTL9300,
 * however the RTL9310 has 2 more registers / fields and the physical field-ids
 * are specific to every platform.
 */
static void rtl838x_write_pie_templated(u32 r[], struct pie_rule *pr, enum rtl838x_template_field_id t[])
{
	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		enum rtl838x_template_field_id field_type = t[i];
		u16 data = 0, data_m = 0;

		switch (field_type) {
		case RTL838X_TEMPLATE_FIELD_SPM0:
			data = pr->spm;
			data_m = pr->spm_m;
			break;
		case RTL838X_TEMPLATE_FIELD_SPM1:
			data = pr->spm >> 16;
			data_m = pr->spm_m >> 16;
			break;
		case RTL838X_TEMPLATE_FIELD_OTAG:
			data = pr->otag;
			data_m = pr->otag_m;
			break;
		case RTL838X_TEMPLATE_FIELD_SMAC0:
			data = pr->smac[4];
			data = (data << 8) | pr->smac[5];
			data_m = pr->smac_m[4];
			data_m = (data_m << 8) | pr->smac_m[5];
			break;
		case RTL838X_TEMPLATE_FIELD_SMAC1:
			data = pr->smac[2];
			data = (data << 8) | pr->smac[3];
			data_m = pr->smac_m[2];
			data_m = (data_m << 8) | pr->smac_m[3];
			break;
		case RTL838X_TEMPLATE_FIELD_SMAC2:
			data = pr->smac[0];
			data = (data << 8) | pr->smac[1];
			data_m = pr->smac_m[0];
			data_m = (data_m << 8) | pr->smac_m[1];
			break;
		case RTL838X_TEMPLATE_FIELD_DMAC0:
			data = pr->dmac[4];
			data = (data << 8) | pr->dmac[5];
			data_m = pr->dmac_m[4];
			data_m = (data_m << 8) | pr->dmac_m[5];
			break;
		case RTL838X_TEMPLATE_FIELD_DMAC1:
			data = pr->dmac[2];
			data = (data << 8) | pr->dmac[3];
			data_m = pr->dmac_m[2];
			data_m = (data_m << 8) | pr->dmac_m[3];
			break;
		case RTL838X_TEMPLATE_FIELD_DMAC2:
			data = pr->dmac[0];
			data = (data << 8) | pr->dmac[1];
			data_m = pr->dmac_m[0];
			data_m = (data_m << 8) | pr->dmac_m[1];
			break;
		case RTL838X_TEMPLATE_FIELD_ETHERTYPE:
			data = pr->ethertype;
			data_m = pr->ethertype_m;
			break;
		case RTL838X_TEMPLATE_FIELD_ITAG:
			data = pr->itag;
			data_m = pr->itag_m;
			break;
		case RTL838X_TEMPLATE_FIELD_RANGE_CHK:
			data = pr->field_range_check;
			data_m = pr->field_range_check_m;
			break;
		case RTL838X_TEMPLATE_FIELD_SIP0:
			if (pr->is_ipv6) {
				data = pr->sip6.s6_addr16[7];
				data_m = pr->sip6_m.s6_addr16[7];
			} else {
				data = pr->sip;
				data_m = pr->sip_m;
			}
			break;
		case RTL838X_TEMPLATE_FIELD_SIP1:
			if (pr->is_ipv6) {
				data = pr->sip6.s6_addr16[6];
				data_m = pr->sip6_m.s6_addr16[6];
			} else {
				data = pr->sip >> 16;
				data_m = pr->sip_m >> 16;
			}
			break;
		case RTL838X_TEMPLATE_FIELD_SIP2:
		case RTL838X_TEMPLATE_FIELD_SIP3:
		case RTL838X_TEMPLATE_FIELD_SIP4:
		case RTL838X_TEMPLATE_FIELD_SIP5:
		case RTL838X_TEMPLATE_FIELD_SIP6:
		case RTL838X_TEMPLATE_FIELD_SIP7:
			data = pr->sip6.s6_addr16[5 - (field_type - RTL838X_TEMPLATE_FIELD_SIP2)];
			data_m = pr->sip6_m.s6_addr16[5 - (field_type - RTL838X_TEMPLATE_FIELD_SIP2)];
			break;
		case RTL838X_TEMPLATE_FIELD_DIP0:
			if (pr->is_ipv6) {
				data = pr->dip6.s6_addr16[7];
				data_m = pr->dip6_m.s6_addr16[7];
			} else {
				data = pr->dip;
				data_m = pr->dip_m;
			}
			break;
		case RTL838X_TEMPLATE_FIELD_DIP1:
			if (pr->is_ipv6) {
				data = pr->dip6.s6_addr16[6];
				data_m = pr->dip6_m.s6_addr16[6];
			} else {
				data = pr->dip >> 16;
				data_m = pr->dip_m >> 16;
			}
			break;
		case RTL838X_TEMPLATE_FIELD_DIP2:
		case RTL838X_TEMPLATE_FIELD_DIP3:
		case RTL838X_TEMPLATE_FIELD_DIP4:
		case RTL838X_TEMPLATE_FIELD_DIP5:
		case RTL838X_TEMPLATE_FIELD_DIP6:
		case RTL838X_TEMPLATE_FIELD_DIP7:
			data = pr->dip6.s6_addr16[5 - (field_type - RTL838X_TEMPLATE_FIELD_DIP2)];
			data_m = pr->dip6_m.s6_addr16[5 - (field_type - RTL838X_TEMPLATE_FIELD_DIP2)];
			break;
		case RTL838X_TEMPLATE_FIELD_IP_TOS_PROTO:
			data = pr->tos_proto;
			data_m = pr->tos_proto_m;
			break;
		case RTL838X_TEMPLATE_FIELD_L4_SPORT:
			data = pr->sport;
			data_m = pr->sport_m;
			break;
		case RTL838X_TEMPLATE_FIELD_L4_DPORT:
			data = pr->dport;
			data_m = pr->dport_m;
			break;
		case RTL838X_TEMPLATE_FIELD_ICMP_IGMP:
			data = pr->icmp_igmp;
			data_m = pr->icmp_igmp_m;
			break;
		default:
			pr_debug("%s: unknown field %d\n", __func__, field_type);
			continue;
		}
		if (!(i % 2)) {
			r[5 - i / 2] = data;
			r[12 - i / 2] = data_m;
		} else {
			r[5 - i / 2] |= ((u32)data) << 16;
			r[12 - i / 2] |= ((u32)data_m) << 16;
		}
	}
}

/* Creates the intermediate representation of the templated match-fields of the
 * PIE rule in the pie_rule structure by reading the raw data fields in the
 * raw register space r[].
 * The register space configuration size is identical for the RTL8380/90 and RTL9300,
 * however the RTL9310 has 2 more registers / fields and the physical field-ids
 */
static void rtl838x_read_pie_templated(u32 r[], struct pie_rule *pr, enum rtl838x_template_field_id t[])
{
	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		enum rtl838x_template_field_id field_type = t[i];
		u16 data, data_m;

		field_type = t[i];
		if (!(i % 2)) {
			data = r[5 - i / 2];
			data_m = r[12 - i / 2];
		} else {
			data = r[5 - i / 2] >> 16;
			data_m = r[12 - i / 2] >> 16;
		}

		switch (field_type) {
		case RTL838X_TEMPLATE_FIELD_SPM0:
			pr->spm = (pr->spn << 16) | data;
			pr->spm_m = (pr->spn << 16) | data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_SPM1:
			pr->spm = data;
			pr->spm_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_OTAG:
			pr->otag = data;
			pr->otag_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_SMAC0:
			pr->smac[4] = data >> 8;
			pr->smac[5] = data;
			pr->smac_m[4] = data >> 8;
			pr->smac_m[5] = data;
			break;
		case RTL838X_TEMPLATE_FIELD_SMAC1:
			pr->smac[2] = data >> 8;
			pr->smac[3] = data;
			pr->smac_m[2] = data >> 8;
			pr->smac_m[3] = data;
			break;
		case RTL838X_TEMPLATE_FIELD_SMAC2:
			pr->smac[0] = data >> 8;
			pr->smac[1] = data;
			pr->smac_m[0] = data >> 8;
			pr->smac_m[1] = data;
			break;
		case RTL838X_TEMPLATE_FIELD_DMAC0:
			pr->dmac[4] = data >> 8;
			pr->dmac[5] = data;
			pr->dmac_m[4] = data >> 8;
			pr->dmac_m[5] = data;
			break;
		case RTL838X_TEMPLATE_FIELD_DMAC1:
			pr->dmac[2] = data >> 8;
			pr->dmac[3] = data;
			pr->dmac_m[2] = data >> 8;
			pr->dmac_m[3] = data;
			break;
		case RTL838X_TEMPLATE_FIELD_DMAC2:
			pr->dmac[0] = data >> 8;
			pr->dmac[1] = data;
			pr->dmac_m[0] = data >> 8;
			pr->dmac_m[1] = data;
			break;
		case RTL838X_TEMPLATE_FIELD_ETHERTYPE:
			pr->ethertype = data;
			pr->ethertype_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_ITAG:
			pr->itag = data;
			pr->itag_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_RANGE_CHK:
			pr->field_range_check = data;
			pr->field_range_check_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_SIP0:
			pr->sip = data;
			pr->sip_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_SIP1:
			pr->sip = (pr->sip << 16) | data;
			pr->sip_m = (pr->sip << 16) | data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_SIP2:
			pr->is_ipv6 = true;
			/* Make use of limitiations on the position of the match values */
			ipv6_addr_set(&pr->sip6, pr->sip, r[5 - i / 2],
				      r[4 - i / 2], r[3 - i / 2]);
			ipv6_addr_set(&pr->sip6_m, pr->sip_m, r[5 - i / 2],
				      r[4 - i / 2], r[3 - i / 2]);
		case RTL838X_TEMPLATE_FIELD_SIP3:
		case RTL838X_TEMPLATE_FIELD_SIP4:
		case RTL838X_TEMPLATE_FIELD_SIP5:
		case RTL838X_TEMPLATE_FIELD_SIP6:
		case RTL838X_TEMPLATE_FIELD_SIP7:
			break;
		case RTL838X_TEMPLATE_FIELD_DIP0:
			pr->dip = data;
			pr->dip_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_DIP1:
			pr->dip = (pr->dip << 16) | data;
			pr->dip_m = (pr->dip << 16) | data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_DIP2:
			pr->is_ipv6 = true;
			ipv6_addr_set(&pr->dip6, pr->dip, r[5 - i / 2],
				      r[4 - i / 2], r[3 - i / 2]);
			ipv6_addr_set(&pr->dip6_m, pr->dip_m, r[5 - i / 2],
				      r[4 - i / 2], r[3 - i / 2]);
		case RTL838X_TEMPLATE_FIELD_DIP3:
		case RTL838X_TEMPLATE_FIELD_DIP4:
		case RTL838X_TEMPLATE_FIELD_DIP5:
		case RTL838X_TEMPLATE_FIELD_DIP6:
		case RTL838X_TEMPLATE_FIELD_DIP7:
			break;
		case RTL838X_TEMPLATE_FIELD_IP_TOS_PROTO:
			pr->tos_proto = data;
			pr->tos_proto_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_L4_SPORT:
			pr->sport = data;
			pr->sport_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_L4_DPORT:
			pr->dport = data;
			pr->dport_m = data_m;
			break;
		case RTL838X_TEMPLATE_FIELD_ICMP_IGMP:
			pr->icmp_igmp = data;
			pr->icmp_igmp_m = data_m;
			break;
		default:
			pr_debug("%s: unknown field %d\n", __func__, field_type);
		}
	}
}

static void rtl838x_read_pie_fixed_fields(u32 r[], struct pie_rule *pr)
{
	pr->spmmask_fix = (r[6] >> 22) & 0x3;
	pr->spn = (r[6] >> 16) & 0x3f;
	pr->mgnt_vlan = (r[6] >> 15) & 1;
	pr->dmac_hit_sw = (r[6] >> 14) & 1;
	pr->not_first_frag = (r[6] >> 13) & 1;
	pr->frame_type_l4 = (r[6] >> 10) & 7;
	pr->frame_type = (r[6] >> 8) & 3;
	pr->otag_fmt = (r[6] >> 7) & 1;
	pr->itag_fmt = (r[6] >> 6) & 1;
	pr->otag_exist = (r[6] >> 5) & 1;
	pr->itag_exist = (r[6] >> 4) & 1;
	pr->frame_type_l2 = (r[6] >> 2) & 3;
	pr->tid = r[6] & 3;

	pr->spmmask_fix_m = (r[13] >> 22) & 0x3;
	pr->spn_m = (r[13] >> 16) & 0x3f;
	pr->mgnt_vlan_m = (r[13] >> 15) & 1;
	pr->dmac_hit_sw_m = (r[13] >> 14) & 1;
	pr->not_first_frag_m = (r[13] >> 13) & 1;
	pr->frame_type_l4_m = (r[13] >> 10) & 7;
	pr->frame_type_m = (r[13] >> 8) & 3;
	pr->otag_fmt_m = (r[13] >> 7) & 1;
	pr->itag_fmt_m = (r[13] >> 6) & 1;
	pr->otag_exist_m = (r[13] >> 5) & 1;
	pr->itag_exist_m = (r[13] >> 4) & 1;
	pr->frame_type_l2_m = (r[13] >> 2) & 3;
	pr->tid_m = r[13] & 3;

	pr->valid = r[14] & BIT(31);
	pr->cond_not = r[14] & BIT(30);
	pr->cond_and1 = r[14] & BIT(29);
	pr->cond_and2 = r[14] & BIT(28);
	pr->ivalid = r[14] & BIT(27);

	pr->drop = (r[17] >> 14) & 3;
	pr->fwd_sel = r[17] & BIT(13);
	pr->ovid_sel = r[17] & BIT(12);
	pr->ivid_sel = r[17] & BIT(11);
	pr->flt_sel = r[17] & BIT(10);
	pr->log_sel = r[17] & BIT(9);
	pr->rmk_sel = r[17] & BIT(8);
	pr->meter_sel = r[17] & BIT(7);
	pr->tagst_sel = r[17] & BIT(6);
	pr->mir_sel = r[17] & BIT(5);
	pr->nopri_sel = r[17] & BIT(4);
	pr->cpupri_sel = r[17] & BIT(3);
	pr->otpid_sel = r[17] & BIT(2);
	pr->itpid_sel = r[17] & BIT(1);
	pr->shaper_sel = r[17] & BIT(0);
}

static void rtl838x_write_pie_fixed_fields(u32 r[],  struct pie_rule *pr)
{
	r[6] = ((u32)(pr->spmmask_fix & 0x3)) << 22;
	r[6] |= ((u32)(pr->spn & 0x3f)) << 16;
	r[6] |= pr->mgnt_vlan ? BIT(15) : 0;
	r[6] |= pr->dmac_hit_sw ? BIT(14) : 0;
	r[6] |= pr->not_first_frag ? BIT(13) : 0;
	r[6] |= ((u32)(pr->frame_type_l4 & 0x7)) << 10;
	r[6] |= ((u32)(pr->frame_type & 0x3)) << 8;
	r[6] |= pr->otag_fmt ? BIT(7) : 0;
	r[6] |= pr->itag_fmt ? BIT(6) : 0;
	r[6] |= pr->otag_exist ? BIT(5) : 0;
	r[6] |= pr->itag_exist ? BIT(4) : 0;
	r[6] |= ((u32)(pr->frame_type_l2 & 0x3)) << 2;
	r[6] |= ((u32)(pr->tid & 0x3));

	r[13] = ((u32)(pr->spmmask_fix_m & 0x3)) << 22;
	r[13] |= ((u32)(pr->spn_m & 0x3f)) << 16;
	r[13] |= pr->mgnt_vlan_m ? BIT(15) : 0;
	r[13] |= pr->dmac_hit_sw_m ? BIT(14) : 0;
	r[13] |= pr->not_first_frag_m ? BIT(13) : 0;
	r[13] |= ((u32)(pr->frame_type_l4_m & 0x7)) << 10;
	r[13] |= ((u32)(pr->frame_type_m & 0x3)) << 8;
	r[13] |= pr->otag_fmt_m ? BIT(7) : 0;
	r[13] |= pr->itag_fmt_m ? BIT(6) : 0;
	r[13] |= pr->otag_exist_m ? BIT(5) : 0;
	r[13] |= pr->itag_exist_m ? BIT(4) : 0;
	r[13] |= ((u32)(pr->frame_type_l2_m & 0x3)) << 2;
	r[13] |= ((u32)(pr->tid_m & 0x3));

	r[14] = pr->valid ? BIT(31) : 0;
	r[14] |= pr->cond_not ? BIT(30) : 0;
	r[14] |= pr->cond_and1 ? BIT(29) : 0;
	r[14] |= pr->cond_and2 ? BIT(28) : 0;
	r[14] |= pr->ivalid ? BIT(27) : 0;

	if (pr->drop)
		r[17] = 0x1 << 14;	/* Standard drop action */
	else
		r[17] = 0;
	r[17] |= pr->fwd_sel ? BIT(13) : 0;
	r[17] |= pr->ovid_sel ? BIT(12) : 0;
	r[17] |= pr->ivid_sel ? BIT(11) : 0;
	r[17] |= pr->flt_sel ? BIT(10) : 0;
	r[17] |= pr->log_sel ? BIT(9) : 0;
	r[17] |= pr->rmk_sel ? BIT(8) : 0;
	r[17] |= pr->meter_sel ? BIT(7) : 0;
	r[17] |= pr->tagst_sel ? BIT(6) : 0;
	r[17] |= pr->mir_sel ? BIT(5) : 0;
	r[17] |= pr->nopri_sel ? BIT(4) : 0;
	r[17] |= pr->cpupri_sel ? BIT(3) : 0;
	r[17] |= pr->otpid_sel ? BIT(2) : 0;
	r[17] |= pr->itpid_sel ? BIT(1) : 0;
	r[17] |= pr->shaper_sel ? BIT(0) : 0;
}

static int rtl838x_write_pie_action(u32 r[],  struct pie_rule *pr)
{
	u16 *aif = (u16 *)&r[17];
	u16 data;
	int fields_used = 0;

	aif--;

	pr_debug("%s, at %08x\n", __func__, (u32)aif);
	/* Multiple actions can be linked to a match of a PIE rule,
	 * they have different precedence depending on their type and this precedence
	 * defines which Action Information Field (0-4) in the IACL table stores
	 * the additional data of the action (like e.g. the port number a packet is
	 * forwarded to)
	 */
	/* TODO: count bits in selectors to limit to a maximum number of actions */
	if (pr->fwd_sel) { /* Forwarding action */
		data = pr->fwd_act << 13;
		data |= pr->fwd_data;
		data |= pr->bypass_all ? BIT(12) : 0;
		data |= pr->bypass_ibc_sc ? BIT(11) : 0;
		data |= pr->bypass_igr_stp ? BIT(10) : 0;
		*aif-- = data;
		fields_used++;
	}

	if (pr->ovid_sel) { /* Outer VID action */
		data = (pr->ovid_act & 0x3) << 12;
		data |= pr->ovid_data;
		*aif-- = data;
		fields_used++;
	}

	if (pr->ivid_sel) { /* Inner VID action */
		data = (pr->ivid_act & 0x3) << 12;
		data |= pr->ivid_data;
		*aif-- = data;
		fields_used++;
	}

	if (pr->flt_sel) { /* Filter action */
		*aif-- = pr->flt_data;
		fields_used++;
	}

	if (pr->log_sel) { /* Log action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->log_data;
		fields_used++;
	}

	if (pr->rmk_sel) { /* Remark action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->rmk_data;
		fields_used++;
	}

	if (pr->meter_sel) { /* Meter action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->meter_data;
		fields_used++;
	}

	if (pr->tagst_sel) { /* Egress Tag Status action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->tagst_data;
		fields_used++;
	}

	if (pr->mir_sel) { /* Mirror action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->mir_data;
		fields_used++;
	}

	if (pr->nopri_sel) { /* Normal Priority action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->nopri_data;
		fields_used++;
	}

	if (pr->cpupri_sel) { /* CPU Priority action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->nopri_data;
		fields_used++;
	}

	if (pr->otpid_sel) { /* OTPID action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->otpid_data;
		fields_used++;
	}

	if (pr->itpid_sel) { /* ITPID action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->itpid_data;
		fields_used++;
	}

	if (pr->shaper_sel) { /* Traffic shaper action */
		if (fields_used >= 4)
			return -1;
		*aif-- = pr->shaper_data;
		fields_used++;
	}

	return 0;
}

static void rtl838x_read_pie_action(u32 r[],  struct pie_rule *pr)
{
	u16 *aif = (u16 *)&r[17];

	aif--;

	pr_debug("%s, at %08x\n", __func__, (u32)aif);
	if (pr->drop)
		pr_debug("%s: Action Drop: %d", __func__, pr->drop);

	if (pr->fwd_sel) { /* Forwarding action */
		pr->fwd_act = *aif >> 13;
		pr->fwd_data = *aif--;
		pr->bypass_all = pr->fwd_data & BIT(12);
		pr->bypass_ibc_sc = pr->fwd_data & BIT(11);
		pr->bypass_igr_stp = pr->fwd_data & BIT(10);
		if (pr->bypass_all || pr->bypass_ibc_sc || pr->bypass_igr_stp)
			pr->bypass_sel = true;
	}
	if (pr->ovid_sel) /* Outer VID action */
		pr->ovid_data = *aif--;
	if (pr->ivid_sel) /* Inner VID action */
		pr->ivid_data = *aif--;
	if (pr->flt_sel) /* Filter action */
		pr->flt_data = *aif--;
	if (pr->log_sel) /* Log action */
		pr->log_data = *aif--;
	if (pr->rmk_sel) /* Remark action */
		pr->rmk_data = *aif--;
	if (pr->meter_sel) /* Meter action */
		pr->meter_data = *aif--;
	if (pr->tagst_sel) /* Egress Tag Status action */
		pr->tagst_data = *aif--;
	if (pr->mir_sel) /* Mirror action */
		pr->mir_data = *aif--;
	if (pr->nopri_sel) /* Normal Priority action */
		pr->nopri_data = *aif--;
	if (pr->cpupri_sel) /* CPU Priority action */
		pr->nopri_data = *aif--;
	if (pr->otpid_sel) /* OTPID action */
		pr->otpid_data = *aif--;
	if (pr->itpid_sel) /* ITPID action */
		pr->itpid_data = *aif--;
	if (pr->shaper_sel) /* Traffic shaper action */
		pr->shaper_data = *aif--;
}

static void rtl838x_pie_rule_dump_raw(u32 r[])
{
	pr_debug("Raw IACL table entry:\n");
	pr_debug("Match  : %08x %08x %08x %08x %08x %08x\n", r[0], r[1], r[2], r[3], r[4], r[5]);
	pr_debug("Fixed  : %08x\n", r[6]);
	pr_debug("Match M: %08x %08x %08x %08x %08x %08x\n", r[7], r[8], r[9], r[10], r[11], r[12]);
	pr_debug("Fixed M: %08x\n", r[13]);
	pr_debug("AIF    : %08x %08x %08x\n", r[14], r[15], r[16]);
	pr_debug("Sel    : %08x\n", r[17]);
}

// Currently not used
// static void rtl838x_pie_rule_dump(struct  pie_rule *pr)
// {
// 	pr_debug("Drop: %d, fwd: %d, ovid: %d, ivid: %d, flt: %d, log: %d, rmk: %d, meter: %d tagst: %d, mir: %d, nopri: %d, cpupri: %d, otpid: %d, itpid: %d, shape: %d\n",
// 		pr->drop, pr->fwd_sel, pr->ovid_sel, pr->ivid_sel, pr->flt_sel, pr->log_sel, pr->rmk_sel, pr->log_sel, pr->tagst_sel, pr->mir_sel, pr->nopri_sel,
// 		pr->cpupri_sel, pr->otpid_sel, pr->itpid_sel, pr->shaper_sel);
// 	if (pr->fwd_sel)
// 		pr_debug("FWD: %08x\n", pr->fwd_data);
// 	pr_debug("TID: %x, %x\n", pr->tid, pr->tid_m);
// }

int rtl838x_pie_rule_read(struct rtl838x_switch_priv *priv, int idx, struct  pie_rule *pr)
{
	int tbl = otto_table_acquire(RTL8380_TBL_IACL);
	u32 r[18];
	int block = idx / PIE_BLOCK_SIZE;
	u32 t_select = sw_r32(RTL838X_ACL_BLK_TMPLTE_CTRL(block));

	memset(pr, 0, sizeof(*pr));
	__otto_table_read(tbl, idx, &r);

	otto_table_release(tbl);

	rtl838x_read_pie_fixed_fields(r, pr);
	if (!pr->valid)
		return 0;

	pr_debug("%s: template_selectors %08x, tid: %d\n", __func__, t_select, pr->tid);
	rtl838x_pie_rule_dump_raw(r);

	rtl838x_read_pie_templated(r, pr, rtl838x_fixed_templates[(t_select >> (pr->tid * 3)) & 0x7]);

	rtl838x_read_pie_action(r, pr);

	return 0;
}

int rtl838x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr)
{
	int tbl = otto_table_acquire(RTL8380_TBL_IACL);
	u32 r[18];
	int err;
	int block = idx / PIE_BLOCK_SIZE;
	u32 t_select = sw_r32(RTL838X_ACL_BLK_TMPLTE_CTRL(block));

	pr_debug("%s: %d, t_select: %08x\n", __func__, idx, t_select);

	for (int i = 0; i < 18; i++)
		r[i] = 0;

	if (!pr->valid) {
		err = -EINVAL;
		pr_err("Rule invalid\n");
		goto errout;
	}

	rtl838x_write_pie_fixed_fields(r, pr);

	pr_debug("%s: template %d\n", __func__, (t_select >> (pr->tid * 3)) & 0x7);
	rtl838x_write_pie_templated(r, pr, rtl838x_fixed_templates[(t_select >> (pr->tid * 3)) & 0x7]);

	err = rtl838x_write_pie_action(r, pr);
	if (err) {
		pr_err("Rule actions too complex\n");
		goto errout;
	}

/*	rtl838x_pie_rule_dump_raw(r); */

	__otto_table_write(tbl, idx, &r);
	otto_table_release(tbl);

	return err;

errout:
	/* Leave no half-built rule behind: commit an empty entry. The write
	 * used to go out with the data registers untouched, which committed
	 * whatever the previous table access had left in them.
	 */
	memset(r, 0, sizeof(r));
	__otto_table_write(tbl, idx, &r);
	otto_table_release(tbl);

	return err;
}

static bool rtl838x_pie_templ_has(int t, enum rtl838x_template_field_id field_type)
{
	enum rtl838x_template_field_id ft;

	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		ft = rtl838x_fixed_templates[t][i];
		if (field_type == ft)
			return true;
	}

	return false;
}

static int rtl838x_pie_verify_template(struct rtl838x_switch_priv *priv,
				       struct pie_rule *pr, int t, int block)
{
	int i;

	if (!pr->is_ipv6 && pr->sip_m && !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_SIP0))
		return -1;

	if (!pr->is_ipv6 && pr->dip_m && !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_DIP0))
		return -1;

	if (pr->is_ipv6) {
		if ((pr->sip6_m.s6_addr32[0] ||
		     pr->sip6_m.s6_addr32[1] ||
		     pr->sip6_m.s6_addr32[2] ||
		     pr->sip6_m.s6_addr32[3]) &&
		    !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_SIP2))
			return -1;
		if ((pr->dip6_m.s6_addr32[0] ||
		     pr->dip6_m.s6_addr32[1] ||
		     pr->dip6_m.s6_addr32[2] ||
		     pr->dip6_m.s6_addr32[3]) &&
		    !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_DIP2))
			return -1;
	}

	if (ether_addr_to_u64(pr->smac_m) && !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_SMAC0))
		return -1;

	if (ether_addr_to_u64(pr->dmac_m) && !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_DMAC0))
		return -1;

	if (pr->itag_m && !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_ITAG))
		return -1;

	if (pr->sport_m && !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_L4_SPORT))
		return -1;

	if (pr->dport_m && !rtl838x_pie_templ_has(t, RTL838X_TEMPLATE_FIELD_L4_DPORT))
		return -1;

	/* TODO: Check more */

	i = find_first_zero_bit(&priv->pie_use_bm[block * 4], PIE_BLOCK_SIZE);

	if (i >= PIE_BLOCK_SIZE)
		return -1;

	return i + PIE_BLOCK_SIZE * block;
}

int rtl838x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx, block, j;

	pr_debug("In %s\n", __func__);

	mutex_lock(&priv->pie_mutex);

	for (block = 0; block < priv->r->n_pie_blocks; block++) {
		for (j = 0; j < 3; j++) {
			int t = (sw_r32(RTL838X_ACL_BLK_TMPLTE_CTRL(block)) >> (j * 3)) & 0x7;

			pr_debug("Testing block %d, template %d, template id %d\n", block, j, t);
			idx = rtl838x_pie_verify_template(priv, pr, t, block);
			if (idx >= 0)
				break;
		}
		if (j < 3)
			break;
	}

	if (block >= priv->r->n_pie_blocks) {
		mutex_unlock(&priv->pie_mutex);
		return -EOPNOTSUPP;
	}

	pr_debug("Using block: %d, index %d, template-id %d\n", block, idx, j);
	set_bit(idx, priv->pie_use_bm);

	pr->valid = true;
	pr->tid = j;  /* Mapped to template number */
	pr->tid_m = 0x3;
	pr->id = idx;

	rtl838x_pie_lookup_enable(priv, idx);
	rtl838x_pie_rule_write(priv, idx, pr);

	mutex_unlock(&priv->pie_mutex);

	return 0;
}

void rtl838x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx = pr->id;

	rtl838x_pie_rule_del(priv, idx, idx);
	clear_bit(idx, priv->pie_use_bm);
}

/* Initializes the Packet Inspection Engine:
 * powers it up, enables default matching templates for all blocks
 * and clears all rules possibly installed by u-boot
 */
void rtl838x_pie_init(struct rtl838x_switch_priv *priv)
{
	u32 template_selectors;

	mutex_init(&priv->pie_mutex);

	/* Enable ACL lookup on all ports, including CPU_PORT */
	for (int i = 0; i <= priv->r->cpu_port; i++)
		sw_w32(1, RTL838X_ACL_PORT_LOOKUP_CTRL(i));

	/* Power on all PIE blocks */
	for (int i = 0; i < priv->r->n_pie_blocks; i++)
		sw_w32_mask(0, BIT(i), RTL838X_ACL_BLK_PWR_CTRL);

	/* Include IPG in metering */
	sw_w32(1, RTL838X_METER_GLB_CTRL);

	/* Delete all present rules */
	rtl838x_pie_rule_del(priv, 0, priv->r->n_pie_blocks * PIE_BLOCK_SIZE - 1);

	/* Routing bypasses source port filter */
	sw_w32_mask(0, 1, RTL838X_DMY_REG27);

	/* Enable predefined templates 0, 1 and 2 for even blocks */
	template_selectors = 0 | (1 << 3) | (2 << 6);
	for (int i = 0; i < 6; i += 2)
		sw_w32(template_selectors, RTL838X_ACL_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 0, 3 and 4 (IPv6 support) for odd blocks */
	template_selectors = 0 | (3 << 3) | (4 << 6);
	for (int i = 1; i < priv->r->n_pie_blocks; i += 2)
		sw_w32(template_selectors, RTL838X_ACL_BLK_TMPLTE_CTRL(i));

	/* Group each pair of physical blocks together to a logical block */
	sw_w32(0b10101010101, RTL838X_ACL_BLK_GROUP_CTRL);
}

/* RTL839X */

/* Definition of the RTL839X-specific template field IDs as used in the PIE */
enum rtl839x_template_field_id {
	RTL839X_TEMPLATE_FIELD_SPMMASK = 0,
	RTL839X_TEMPLATE_FIELD_SPM0 = 1,		/* Source portmask ports 0-15 */
	RTL839X_TEMPLATE_FIELD_SPM1 = 2,		/* Source portmask ports 16-31 */
	RTL839X_TEMPLATE_FIELD_SPM2 = 3,		/* Source portmask ports 32-47 */
	RTL839X_TEMPLATE_FIELD_SPM3 = 4,		/* Source portmask ports 48-56 */
	RTL839X_TEMPLATE_FIELD_DMAC0 = 5,		/* Destination MAC [15:0] */
	RTL839X_TEMPLATE_FIELD_DMAC1 = 6,		/* Destination MAC [31:16] */
	RTL839X_TEMPLATE_FIELD_DMAC2 = 7,		/* Destination MAC [47:32] */
	RTL839X_TEMPLATE_FIELD_SMAC0 = 8,		/* Source MAC [15:0] */
	RTL839X_TEMPLATE_FIELD_SMAC1 = 9,		/* Source MAC [31:16] */
	RTL839X_TEMPLATE_FIELD_SMAC2 = 10,		/* Source MAC [47:32] */
	RTL839X_TEMPLATE_FIELD_ETHERTYPE = 11,		/* Ethernet frame type field */
	/* Field-ID 12 is not used */
	RTL839X_TEMPLATE_FIELD_OTAG = 13,
	RTL839X_TEMPLATE_FIELD_ITAG = 14,
	RTL839X_TEMPLATE_FIELD_SIP0 = 15,
	RTL839X_TEMPLATE_FIELD_SIP1 = 16,
	RTL839X_TEMPLATE_FIELD_DIP0 = 17,
	RTL839X_TEMPLATE_FIELD_DIP1 = 18,
	RTL839X_TEMPLATE_FIELD_IP_TOS_PROTO = 19,
	RTL839X_TEMPLATE_FIELD_IP_FLAG = 20,
	RTL839X_TEMPLATE_FIELD_L4_SPORT = 21,
	RTL839X_TEMPLATE_FIELD_L4_DPORT = 22,
	RTL839X_TEMPLATE_FIELD_L34_HEADER = 23,
	RTL839X_TEMPLATE_FIELD_ICMP_IGMP = 24,
	RTL839X_TEMPLATE_FIELD_VID_RANG0 = 25,
	RTL839X_TEMPLATE_FIELD_VID_RANG1 = 26,
	RTL839X_TEMPLATE_FIELD_L4_PORT_RANG = 27,
	RTL839X_TEMPLATE_FIELD_FIELD_SELECTOR_VALID = 28,
	RTL839X_TEMPLATE_FIELD_FIELD_SELECTOR_0 = 29,
	RTL839X_TEMPLATE_FIELD_FIELD_SELECTOR_1 = 30,
	RTL839X_TEMPLATE_FIELD_FIELD_SELECTOR_2 = 31,
	RTL839X_TEMPLATE_FIELD_FIELD_SELECTOR_3 = 32,
	RTL839X_TEMPLATE_FIELD_FIELD_SELECTOR_4 = 33,
	RTL839X_TEMPLATE_FIELD_FIELD_SELECTOR_5 = 34,
	RTL839X_TEMPLATE_FIELD_SIP2 = 35,
	RTL839X_TEMPLATE_FIELD_SIP3 = 36,
	RTL839X_TEMPLATE_FIELD_SIP4 = 37,
	RTL839X_TEMPLATE_FIELD_SIP5 = 38,
	RTL839X_TEMPLATE_FIELD_SIP6 = 39,
	RTL839X_TEMPLATE_FIELD_SIP7 = 40,
	RTL839X_TEMPLATE_FIELD_OLABEL = 41,
	RTL839X_TEMPLATE_FIELD_ILABEL = 42,
	RTL839X_TEMPLATE_FIELD_OILABEL = 43,
	RTL839X_TEMPLATE_FIELD_DPMMASK = 44,
	RTL839X_TEMPLATE_FIELD_DPM0 = 45,
	RTL839X_TEMPLATE_FIELD_DPM1 = 46,
	RTL839X_TEMPLATE_FIELD_DPM2 = 47,
	RTL839X_TEMPLATE_FIELD_DPM3 = 48,
	RTL839X_TEMPLATE_FIELD_L2DPM0 = 49,
	RTL839X_TEMPLATE_FIELD_L2DPM1 = 50,
	RTL839X_TEMPLATE_FIELD_L2DPM2 = 51,
	RTL839X_TEMPLATE_FIELD_L2DPM3 = 52,
	RTL839X_TEMPLATE_FIELD_IVLAN = 53,
	RTL839X_TEMPLATE_FIELD_OVLAN = 54,
	RTL839X_TEMPLATE_FIELD_FWD_VID = 55,
	RTL839X_TEMPLATE_FIELD_DIP2 = 56,
	RTL839X_TEMPLATE_FIELD_DIP3 = 57,
	RTL839X_TEMPLATE_FIELD_DIP4 = 58,
	RTL839X_TEMPLATE_FIELD_DIP5 = 59,
	RTL839X_TEMPLATE_FIELD_DIP6 = 60,
	RTL839X_TEMPLATE_FIELD_DIP7 = 61,
};

/* Number of fixed templates predefined in the SoC */
#define RTL839X_N_FIXED_TEMPLATES 5
static enum rtl839x_template_field_id rtl839x_fixed_templates[RTL839X_N_FIXED_TEMPLATES][N_FIXED_FIELDS] = {
	{
	  RTL839X_TEMPLATE_FIELD_SPM0, RTL839X_TEMPLATE_FIELD_SPM1, RTL839X_TEMPLATE_FIELD_ITAG,
	  RTL839X_TEMPLATE_FIELD_SMAC0, RTL839X_TEMPLATE_FIELD_SMAC1, RTL839X_TEMPLATE_FIELD_SMAC2,
	  RTL839X_TEMPLATE_FIELD_DMAC0, RTL839X_TEMPLATE_FIELD_DMAC1, RTL839X_TEMPLATE_FIELD_DMAC2,
	  RTL839X_TEMPLATE_FIELD_ETHERTYPE, RTL839X_TEMPLATE_FIELD_SPM2, RTL839X_TEMPLATE_FIELD_SPM3
	}, {
	  RTL839X_TEMPLATE_FIELD_SIP0, RTL839X_TEMPLATE_FIELD_SIP1, RTL839X_TEMPLATE_FIELD_DIP0,
	  RTL839X_TEMPLATE_FIELD_DIP1, RTL839X_TEMPLATE_FIELD_IP_TOS_PROTO, RTL839X_TEMPLATE_FIELD_L4_SPORT,
	  RTL839X_TEMPLATE_FIELD_L4_DPORT, RTL839X_TEMPLATE_FIELD_ICMP_IGMP, RTL839X_TEMPLATE_FIELD_SPM0,
	  RTL839X_TEMPLATE_FIELD_SPM1, RTL839X_TEMPLATE_FIELD_SPM2, RTL839X_TEMPLATE_FIELD_SPM3
	}, {
	  RTL839X_TEMPLATE_FIELD_DMAC0, RTL839X_TEMPLATE_FIELD_DMAC1, RTL839X_TEMPLATE_FIELD_DMAC2,
	  RTL839X_TEMPLATE_FIELD_ITAG, RTL839X_TEMPLATE_FIELD_ETHERTYPE, RTL839X_TEMPLATE_FIELD_IP_TOS_PROTO,
	  RTL839X_TEMPLATE_FIELD_L4_DPORT, RTL839X_TEMPLATE_FIELD_L4_SPORT, RTL839X_TEMPLATE_FIELD_SIP0,
	  RTL839X_TEMPLATE_FIELD_SIP1, RTL839X_TEMPLATE_FIELD_DIP0, RTL839X_TEMPLATE_FIELD_DIP1
	}, {
	  RTL839X_TEMPLATE_FIELD_DIP0, RTL839X_TEMPLATE_FIELD_DIP1, RTL839X_TEMPLATE_FIELD_DIP2,
	  RTL839X_TEMPLATE_FIELD_DIP3, RTL839X_TEMPLATE_FIELD_DIP4, RTL839X_TEMPLATE_FIELD_DIP5,
	  RTL839X_TEMPLATE_FIELD_DIP6, RTL839X_TEMPLATE_FIELD_DIP7, RTL839X_TEMPLATE_FIELD_L4_DPORT,
	  RTL839X_TEMPLATE_FIELD_L4_SPORT, RTL839X_TEMPLATE_FIELD_ICMP_IGMP, RTL839X_TEMPLATE_FIELD_IP_TOS_PROTO
	}, {
	  RTL839X_TEMPLATE_FIELD_SIP0, RTL839X_TEMPLATE_FIELD_SIP1, RTL839X_TEMPLATE_FIELD_SIP2,
	  RTL839X_TEMPLATE_FIELD_SIP3, RTL839X_TEMPLATE_FIELD_SIP4, RTL839X_TEMPLATE_FIELD_SIP5,
	  RTL839X_TEMPLATE_FIELD_SIP6, RTL839X_TEMPLATE_FIELD_SIP7, RTL839X_TEMPLATE_FIELD_SPM0,
	  RTL839X_TEMPLATE_FIELD_SPM1, RTL839X_TEMPLATE_FIELD_SPM2, RTL839X_TEMPLATE_FIELD_SPM3
	},
};

static void rtl839x_pie_lookup_enable(struct rtl838x_switch_priv *priv, int index)
{
	int block = index / PIE_BLOCK_SIZE;

	sw_w32_mask(0, BIT(block), RTL839X_ACL_BLK_LOOKUP_CTRL);
}

/* Delete a range of Packet Inspection Engine rules */
static int rtl839x_pie_rule_del(struct rtl838x_switch_priv *priv, int index_from, int index_to)
{
	u32 v = (index_from << 1) | (index_to << 13) | BIT(0);

	pr_debug("%s: from %d to %d\n", __func__, index_from, index_to);
	mutex_lock(&priv->reg_mutex);

	/* Write from-to and execute bit into control register */
	sw_w32(v, RTL839X_ACL_CLR_CTRL);

	/* Wait until command has completed */
	do {
	} while (sw_r32(RTL839X_ACL_CLR_CTRL) & BIT(0));

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

/* Reads the intermediate representation of the templated match-fields of the
 * PIE rule in the pie_rule structure and fills in the raw data fields in the
 * raw register space r[].
 * The register space configuration size is identical for the RTL8380/90 and RTL9300,
 * however the RTL9310 has 2 more registers / fields and the physical field-ids are different
 * on all SoCs
 * On the RTL8390 the template mask registers are not word-aligned!
 */
static void rtl839x_write_pie_templated(u32 r[], struct pie_rule *pr, enum rtl839x_template_field_id t[])
{
	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		enum rtl839x_template_field_id field_type = t[i];
		u16 data = 0, data_m = 0;

		switch (field_type) {
		case RTL839X_TEMPLATE_FIELD_SPM0:
			data = pr->spm;
			data_m = pr->spm_m;
			break;
		case RTL839X_TEMPLATE_FIELD_SPM1:
			data = pr->spm >> 16;
			data_m = pr->spm_m >> 16;
			break;
		case RTL839X_TEMPLATE_FIELD_SPM2:
			data = pr->spm >> 32;
			data_m = pr->spm_m >> 32;
			break;
		case RTL839X_TEMPLATE_FIELD_SPM3:
			data = pr->spm >> 48;
			data_m = pr->spm_m >> 48;
			break;
		case RTL839X_TEMPLATE_FIELD_OTAG:
			data = pr->otag;
			data_m = pr->otag_m;
			break;
		case RTL839X_TEMPLATE_FIELD_SMAC0:
			data = pr->smac[4];
			data = (data << 8) | pr->smac[5];
			data_m = pr->smac_m[4];
			data_m = (data_m << 8) | pr->smac_m[5];
			break;
		case RTL839X_TEMPLATE_FIELD_SMAC1:
			data = pr->smac[2];
			data = (data << 8) | pr->smac[3];
			data_m = pr->smac_m[2];
			data_m = (data_m << 8) | pr->smac_m[3];
			break;
		case RTL839X_TEMPLATE_FIELD_SMAC2:
			data = pr->smac[0];
			data = (data << 8) | pr->smac[1];
			data_m = pr->smac_m[0];
			data_m = (data_m << 8) | pr->smac_m[1];
			break;
		case RTL839X_TEMPLATE_FIELD_DMAC0:
			data = pr->dmac[4];
			data = (data << 8) | pr->dmac[5];
			data_m = pr->dmac_m[4];
			data_m = (data_m << 8) | pr->dmac_m[5];
			break;
		case RTL839X_TEMPLATE_FIELD_DMAC1:
			data = pr->dmac[2];
			data = (data << 8) | pr->dmac[3];
			data_m = pr->dmac_m[2];
			data_m = (data_m << 8) | pr->dmac_m[3];
			break;
		case RTL839X_TEMPLATE_FIELD_DMAC2:
			data = pr->dmac[0];
			data = (data << 8) | pr->dmac[1];
			data_m = pr->dmac_m[0];
			data_m = (data_m << 8) | pr->dmac_m[1];
			break;
		case RTL839X_TEMPLATE_FIELD_ETHERTYPE:
			data = pr->ethertype;
			data_m = pr->ethertype_m;
			break;
		case RTL839X_TEMPLATE_FIELD_ITAG:
			data = pr->itag;
			data_m = pr->itag_m;
			break;
		case RTL839X_TEMPLATE_FIELD_SIP0:
			if (pr->is_ipv6) {
				data = pr->sip6.s6_addr16[7];
				data_m = pr->sip6_m.s6_addr16[7];
			} else {
				data = pr->sip;
				data_m = pr->sip_m;
			}
			break;
		case RTL839X_TEMPLATE_FIELD_SIP1:
			if (pr->is_ipv6) {
				data = pr->sip6.s6_addr16[6];
				data_m = pr->sip6_m.s6_addr16[6];
			} else {
				data = pr->sip >> 16;
				data_m = pr->sip_m >> 16;
			}
			break;
		case RTL839X_TEMPLATE_FIELD_SIP2:
		case RTL839X_TEMPLATE_FIELD_SIP3:
		case RTL839X_TEMPLATE_FIELD_SIP4:
		case RTL839X_TEMPLATE_FIELD_SIP5:
		case RTL839X_TEMPLATE_FIELD_SIP6:
		case RTL839X_TEMPLATE_FIELD_SIP7:
			data = pr->sip6.s6_addr16[5 - (field_type - RTL839X_TEMPLATE_FIELD_SIP2)];
			data_m = pr->sip6_m.s6_addr16[5 - (field_type - RTL839X_TEMPLATE_FIELD_SIP2)];
			break;
		case RTL839X_TEMPLATE_FIELD_DIP0:
			if (pr->is_ipv6) {
				data = pr->dip6.s6_addr16[7];
				data_m = pr->dip6_m.s6_addr16[7];
			} else {
				data = pr->dip;
				data_m = pr->dip_m;
			}
			break;
		case RTL839X_TEMPLATE_FIELD_DIP1:
			if (pr->is_ipv6) {
				data = pr->dip6.s6_addr16[6];
				data_m = pr->dip6_m.s6_addr16[6];
			} else {
				data = pr->dip >> 16;
				data_m = pr->dip_m >> 16;
			}
			break;
		case RTL839X_TEMPLATE_FIELD_DIP2:
		case RTL839X_TEMPLATE_FIELD_DIP3:
		case RTL839X_TEMPLATE_FIELD_DIP4:
		case RTL839X_TEMPLATE_FIELD_DIP5:
		case RTL839X_TEMPLATE_FIELD_DIP6:
		case RTL839X_TEMPLATE_FIELD_DIP7:
			data = pr->dip6.s6_addr16[5 - (field_type - RTL839X_TEMPLATE_FIELD_DIP2)];
			data_m = pr->dip6_m.s6_addr16[5 - (field_type - RTL839X_TEMPLATE_FIELD_DIP2)];
			break;
		case RTL839X_TEMPLATE_FIELD_IP_TOS_PROTO:
			data = pr->tos_proto;
			data_m = pr->tos_proto_m;
			break;
		case RTL839X_TEMPLATE_FIELD_L4_SPORT:
			data = pr->sport;
			data_m = pr->sport_m;
			break;
		case RTL839X_TEMPLATE_FIELD_L4_DPORT:
			data = pr->dport;
			data_m = pr->dport_m;
			break;
		case RTL839X_TEMPLATE_FIELD_ICMP_IGMP:
			data = pr->icmp_igmp;
			data_m = pr->icmp_igmp_m;
			break;
		default:
			pr_debug("%s: unknown field %d\n", __func__, field_type);
		}

		/* On the RTL8390, the mask fields are not word aligned! */
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

/* Creates the intermediate representation of the templated match-fields of the
 * PIE rule in the pie_rule structure by reading the raw data fields in the
 * raw register space r[].
 * The register space configuration size is identical for the RTL8380/90 and RTL9300,
 * however the RTL9310 has 2 more registers / fields and the physical field-ids
 * On the RTL8390 the template mask registers are not word-aligned!
 */
static void rtl839x_read_pie_templated(u32 r[], struct pie_rule *pr, enum rtl839x_template_field_id t[])
{
	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		enum rtl839x_template_field_id field_type = t[i];
		u16 data, data_m;

		if (!(i % 2)) {
			data = r[5 - i / 2];
			data_m = r[12 - i / 2];
		} else {
			data = r[5 - i / 2] >> 16;
			data_m = r[12 - i / 2] >> 16;
		}

		switch (field_type) {
		case RTL839X_TEMPLATE_FIELD_SPM0:
			pr->spm = (pr->spn << 16) | data;
			pr->spm_m = (pr->spn << 16) | data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_SPM1:
			pr->spm = data;
			pr->spm_m = data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_OTAG:
			pr->otag = data;
			pr->otag_m = data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_SMAC0:
			pr->smac[4] = data >> 8;
			pr->smac[5] = data;
			pr->smac_m[4] = data >> 8;
			pr->smac_m[5] = data;
			break;
		case RTL839X_TEMPLATE_FIELD_SMAC1:
			pr->smac[2] = data >> 8;
			pr->smac[3] = data;
			pr->smac_m[2] = data >> 8;
			pr->smac_m[3] = data;
			break;
		case RTL839X_TEMPLATE_FIELD_SMAC2:
			pr->smac[0] = data >> 8;
			pr->smac[1] = data;
			pr->smac_m[0] = data >> 8;
			pr->smac_m[1] = data;
			break;
		case RTL839X_TEMPLATE_FIELD_DMAC0:
			pr->dmac[4] = data >> 8;
			pr->dmac[5] = data;
			pr->dmac_m[4] = data >> 8;
			pr->dmac_m[5] = data;
			break;
		case RTL839X_TEMPLATE_FIELD_DMAC1:
			pr->dmac[2] = data >> 8;
			pr->dmac[3] = data;
			pr->dmac_m[2] = data >> 8;
			pr->dmac_m[3] = data;
			break;
		case RTL839X_TEMPLATE_FIELD_DMAC2:
			pr->dmac[0] = data >> 8;
			pr->dmac[1] = data;
			pr->dmac_m[0] = data >> 8;
			pr->dmac_m[1] = data;
			break;
		case RTL839X_TEMPLATE_FIELD_ETHERTYPE:
			pr->ethertype = data;
			pr->ethertype_m = data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_ITAG:
			pr->itag = data;
			pr->itag_m = data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_SIP0:
			pr->sip = data;
			pr->sip_m = data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_SIP1:
			pr->sip = (pr->sip << 16) | data;
			pr->sip_m = (pr->sip << 16) | data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_SIP2:
			pr->is_ipv6 = true;
			/* Make use of limitiations on the position of the match values */
			ipv6_addr_set(&pr->sip6, pr->sip, r[5 - i / 2],
				      r[4 - i / 2], r[3 - i / 2]);
			ipv6_addr_set(&pr->sip6_m, pr->sip_m, r[5 - i / 2],
				      r[4 - i / 2], r[3 - i / 2]);
		case RTL839X_TEMPLATE_FIELD_SIP3:
		case RTL839X_TEMPLATE_FIELD_SIP4:
		case RTL839X_TEMPLATE_FIELD_SIP5:
		case RTL839X_TEMPLATE_FIELD_SIP6:
		case RTL839X_TEMPLATE_FIELD_SIP7:
			break;

		case RTL839X_TEMPLATE_FIELD_DIP0:
			pr->dip = data;
			pr->dip_m = data_m;
			break;

		case RTL839X_TEMPLATE_FIELD_DIP1:
			pr->dip = (pr->dip << 16) | data;
			pr->dip_m = (pr->dip << 16) | data_m;
			break;

		case RTL839X_TEMPLATE_FIELD_DIP2:
			pr->is_ipv6 = true;
			ipv6_addr_set(&pr->dip6, pr->dip, r[5 - i / 2],
				      r[4 - i / 2], r[3 - i / 2]);
			ipv6_addr_set(&pr->dip6_m, pr->dip_m, r[5 - i / 2],
				      r[4 - i / 2], r[3 - i / 2]);
		case RTL839X_TEMPLATE_FIELD_DIP3:
		case RTL839X_TEMPLATE_FIELD_DIP4:
		case RTL839X_TEMPLATE_FIELD_DIP5:
		case RTL839X_TEMPLATE_FIELD_DIP6:
		case RTL839X_TEMPLATE_FIELD_DIP7:
			break;
		case RTL839X_TEMPLATE_FIELD_IP_TOS_PROTO:
			pr->tos_proto = data;
			pr->tos_proto_m = data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_L4_SPORT:
			pr->sport = data;
			pr->sport_m = data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_L4_DPORT:
			pr->dport = data;
			pr->dport_m = data_m;
			break;
		case RTL839X_TEMPLATE_FIELD_ICMP_IGMP:
			pr->icmp_igmp = data;
			pr->icmp_igmp_m = data_m;
			break;
		default:
			pr_debug("%s: unknown field %d\n", __func__, field_type);
		}
	}
}

static void rtl839x_read_pie_fixed_fields(u32 r[], struct pie_rule *pr)
{
	pr->spmmask_fix = (r[6] >> 30) & 0x3;
	pr->spn = (r[6] >> 24) & 0x3f;
	pr->mgnt_vlan = (r[6] >> 23) & 1;
	pr->dmac_hit_sw = (r[6] >> 22) & 1;
	pr->not_first_frag = (r[6] >> 21) & 1;
	pr->frame_type_l4 = (r[6] >> 18) & 7;
	pr->frame_type = (r[6] >> 16) & 3;
	pr->otag_fmt = (r[6] >> 15) & 1;
	pr->itag_fmt = (r[6] >> 14) & 1;
	pr->otag_exist = (r[6] >> 13) & 1;
	pr->itag_exist = (r[6] >> 12) & 1;
	pr->frame_type_l2 = (r[6] >> 10) & 3;
	pr->tid = (r[6] >> 8) & 3;

	pr->spmmask_fix_m = (r[12] >> 6) & 0x3;
	pr->spn_m = r[12]  & 0x3f;
	pr->mgnt_vlan_m = (r[13] >> 31) & 1;
	pr->dmac_hit_sw_m = (r[13] >> 30) & 1;
	pr->not_first_frag_m = (r[13] >> 29) & 1;
	pr->frame_type_l4_m = (r[13] >> 26) & 7;
	pr->frame_type_m = (r[13] >> 24) & 3;
	pr->otag_fmt_m = (r[13] >> 23) & 1;
	pr->itag_fmt_m = (r[13] >> 22) & 1;
	pr->otag_exist_m = (r[13] >> 21) & 1;
	pr->itag_exist_m = (r[13] >> 20) & 1;
	pr->frame_type_l2_m = (r[13] >> 18) & 3;
	pr->tid_m = (r[13] >> 16) & 3;

	pr->valid = r[13] & BIT(15);
	pr->cond_not = r[13] & BIT(14);
	pr->cond_and1 = r[13] & BIT(13);
	pr->cond_and2 = r[13] & BIT(12);
}

static void rtl839x_write_pie_fixed_fields(u32 r[],  struct pie_rule *pr)
{
	r[6] = ((u32)(pr->spmmask_fix & 0x3)) << 30;
	r[6] |= ((u32)(pr->spn & 0x3f)) << 24;
	r[6] |= pr->mgnt_vlan ? BIT(23) : 0;
	r[6] |= pr->dmac_hit_sw ? BIT(22) : 0;
	r[6] |= pr->not_first_frag ? BIT(21) : 0;
	r[6] |= ((u32)(pr->frame_type_l4 & 0x7)) << 18;
	r[6] |= ((u32)(pr->frame_type & 0x3)) << 16;
	r[6] |= pr->otag_fmt ? BIT(15) : 0;
	r[6] |= pr->itag_fmt ? BIT(14) : 0;
	r[6] |= pr->otag_exist ? BIT(13) : 0;
	r[6] |= pr->itag_exist ? BIT(12) : 0;
	r[6] |= ((u32)(pr->frame_type_l2 & 0x3)) << 10;
	r[6] |= ((u32)(pr->tid & 0x3)) << 8;

	r[12] |= ((u32)(pr->spmmask_fix_m & 0x3)) << 6;
	r[12] |= (u32)(pr->spn_m & 0x3f);
	r[13] |= pr->mgnt_vlan_m ? BIT(31) : 0;
	r[13] |= pr->dmac_hit_sw_m ? BIT(30) : 0;
	r[13] |= pr->not_first_frag_m ? BIT(29) : 0;
	r[13] |= ((u32)(pr->frame_type_l4_m & 0x7)) << 26;
	r[13] |= ((u32)(pr->frame_type_m & 0x3)) << 24;
	r[13] |= pr->otag_fmt_m ? BIT(23) : 0;
	r[13] |= pr->itag_fmt_m ? BIT(22) : 0;
	r[13] |= pr->otag_exist_m ? BIT(21) : 0;
	r[13] |= pr->itag_exist_m ? BIT(20) : 0;
	r[13] |= ((u32)(pr->frame_type_l2_m & 0x3)) << 18;
	r[13] |= ((u32)(pr->tid_m & 0x3)) << 16;

	r[13] |= pr->valid ? BIT(15) : 0;
	r[13] |= pr->cond_not ? BIT(14) : 0;
	r[13] |= pr->cond_and1 ? BIT(13) : 0;
	r[13] |= pr->cond_and2 ? BIT(12) : 0;
}

static void rtl839x_write_pie_action(u32 r[],  struct pie_rule *pr)
{
	if (pr->drop) {
		r[13] |= 0x9;	/* Set ACT_MASK_FWD & FWD_ACT = DROP */
		r[13] |= BIT(3);
	} else {
		r[13] |= pr->fwd_sel ? BIT(3) : 0;
		r[13] |= pr->fwd_act;
	}
	r[13] |= pr->bypass_sel ? BIT(11) : 0;
	r[13] |= pr->mpls_sel ? BIT(10) : 0;
	r[13] |= pr->nopri_sel ? BIT(9) : 0;
	r[13] |= pr->ovid_sel ? BIT(8) : 0;
	r[13] |= pr->ivid_sel ? BIT(7) : 0;
	r[13] |= pr->meter_sel ? BIT(6) : 0;
	r[13] |= pr->mir_sel ? BIT(5) : 0;
	r[13] |= pr->log_sel ? BIT(4) : 0;

	r[14] |= ((u32)(pr->fwd_data & 0x3fff)) << 18;
	r[14] |= pr->log_octets ? BIT(17) : 0;
	r[14] |= ((u32)(pr->log_data & 0x7ff)) << 4;
	r[14] |= (pr->mir_data & 0x3) << 3;
	r[14] |= ((u32)(pr->meter_data >> 7)) & 0x7;
	r[15] |= (u32)(pr->meter_data) << 26;
	r[15] |= ((u32)(pr->ivid_act) << 23) & 0x3;
	r[15] |= ((u32)(pr->ivid_data) << 9) & 0xfff;
	r[15] |= ((u32)(pr->ovid_act) << 6) & 0x3;
	r[15] |= ((u32)(pr->ovid_data) >> 4) & 0xff;
	r[16] |= ((u32)(pr->ovid_data) & 0xf) << 28;
	r[16] |= ((u32)(pr->nopri_data) & 0x7) << 20;
	r[16] |= ((u32)(pr->mpls_act) & 0x7) << 20;
	r[16] |= ((u32)(pr->mpls_lib_idx) & 0x7) << 20;
	r[16] |= pr->bypass_all ? BIT(9) : 0;
	r[16] |= pr->bypass_igr_stp ? BIT(8) : 0;
	r[16] |= pr->bypass_ibc_sc ? BIT(7) : 0;
}

static void rtl839x_read_pie_action(u32 r[],  struct pie_rule *pr)
{
	if (r[13] & BIT(3)) { /* ACT_MASK_FWD set, is it a drop? */
		if ((r[14] & 0x7) == 1) {
			pr->drop = true;
		} else {
			pr->fwd_sel = true;
			pr->fwd_act = r[14] & 0x7;
		}
	}

	pr->bypass_sel = r[13] & BIT(11);
	pr->mpls_sel = r[13] & BIT(10);
	pr->nopri_sel = r[13] & BIT(9);
	pr->ovid_sel = r[13] & BIT(8);
	pr->ivid_sel = r[13] & BIT(7);
	pr->meter_sel = r[13] & BIT(6);
	pr->mir_sel = r[13] & BIT(5);
	pr->log_sel = r[13] & BIT(4);

	/* TODO: Read in data fields */

	pr->bypass_all = r[16] & BIT(9);
	pr->bypass_igr_stp = r[16] & BIT(8);
	pr->bypass_ibc_sc = r[16] & BIT(7);
}

static void rtl839x_pie_rule_dump_raw(u32 r[])
{
	pr_debug("Raw IACL table entry:\n");
	pr_debug("Match  : %08x %08x %08x %08x %08x %08x\n", r[0], r[1], r[2], r[3], r[4], r[5]);
	pr_debug("Fixed  : %06x\n", r[6] >> 8);
	pr_debug("Match M: %08x %08x %08x %08x %08x %08x\n",
		 (r[6] << 24) | (r[7] >> 8), (r[7] << 24) | (r[8] >> 8), (r[8] << 24) | (r[9] >> 8),
		 (r[9] << 24) | (r[10] >> 8), (r[10] << 24) | (r[11] >> 8),
		 (r[11] << 24) | (r[12] >> 8));
	pr_debug("R[13]:   %08x\n", r[13]);
	pr_debug("Fixed M: %06x\n", ((r[12] << 16) | (r[13] >> 16)) & 0xffffff);
	pr_debug("Valid / not / and1 / and2 : %1x\n", (r[13] >> 12) & 0xf);
	pr_debug("r 13-16: %08x %08x %08x %08x\n", r[13], r[14], r[15], r[16]);
}

void rtl839x_pie_rule_dump(struct  pie_rule *pr)
{
	pr_debug("Drop: %d, fwd: %d, ovid: %d, ivid: %d, flt: %d, log: %d, rmk: %d, meter: %d tagst: %d, mir: %d, nopri: %d, cpupri: %d, otpid: %d, itpid: %d, shape: %d\n",
		 pr->drop, pr->fwd_sel, pr->ovid_sel, pr->ivid_sel, pr->flt_sel, pr->log_sel, pr->rmk_sel, pr->log_sel, pr->tagst_sel, pr->mir_sel, pr->nopri_sel,
		 pr->cpupri_sel, pr->otpid_sel, pr->itpid_sel, pr->shaper_sel);
	if (pr->fwd_sel)
		pr_debug("FWD: %08x\n", pr->fwd_data);
	pr_debug("TID: %x, %x\n", pr->tid, pr->tid_m);
}

int rtl839x_pie_rule_read(struct rtl838x_switch_priv *priv, int idx, struct  pie_rule *pr)
{
	int tbl = otto_table_acquire(RTL8390_TBL_IACL);
	u32 r[17];
	int block = idx / PIE_BLOCK_SIZE;
	u32 t_select = sw_r32(RTL839X_ACL_BLK_TMPLTE_CTRL(block));

	memset(pr, 0, sizeof(*pr));
	__otto_table_read(tbl, idx, &r);

	otto_table_release(tbl);

	rtl839x_read_pie_fixed_fields(r, pr);
	if (!pr->valid)
		return 0;

	pr_debug("%s: template_selectors %08x, tid: %d\n", __func__, t_select, pr->tid);
	rtl839x_pie_rule_dump_raw(r);

	rtl839x_read_pie_templated(r, pr, rtl839x_fixed_templates[(t_select >> (pr->tid * 3)) & 0x7]);

	rtl839x_read_pie_action(r, pr);

	return 0;
}

int rtl839x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr)
{
	int tbl = otto_table_acquire(RTL8390_TBL_IACL);
	u32 r[17];
	int block = idx / PIE_BLOCK_SIZE;
	u32 t_select = sw_r32(RTL839X_ACL_BLK_TMPLTE_CTRL(block));

	pr_debug("%s: %d, t_select: %08x\n", __func__, idx, t_select);

	for (int i = 0; i < 17; i++)
		r[i] = 0;

	if (!pr->valid) {
		__otto_table_write(tbl, idx, &r);
		otto_table_release(tbl);
		return 0;
	}
	rtl839x_write_pie_fixed_fields(r, pr);

	pr_debug("%s: template %d\n", __func__, (t_select >> (pr->tid * 3)) & 0x7);
	rtl839x_write_pie_templated(r, pr, rtl839x_fixed_templates[(t_select >> (pr->tid * 3)) & 0x7]);

	rtl839x_write_pie_action(r, pr);

/*	rtl839x_pie_rule_dump_raw(r); */

	__otto_table_write(tbl, idx, &r);
	otto_table_release(tbl);

	return 0;
}

static bool rtl839x_pie_templ_has(int t, enum rtl839x_template_field_id field_type)
{
	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		enum rtl839x_template_field_id ft = rtl839x_fixed_templates[t][i];

		if (field_type == ft)
			return true;
	}

	return false;
}

static int rtl839x_pie_verify_template(struct rtl838x_switch_priv *priv,
				       struct pie_rule *pr, int t, int block)
{
	int i;

	if (!pr->is_ipv6 && pr->sip_m && !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_SIP0))
		return -1;

	if (!pr->is_ipv6 && pr->dip_m && !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_DIP0))
		return -1;

	if (pr->is_ipv6) {
		if ((pr->sip6_m.s6_addr32[0] ||
		     pr->sip6_m.s6_addr32[1] ||
		     pr->sip6_m.s6_addr32[2] ||
		     pr->sip6_m.s6_addr32[3]) &&
		    !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_SIP2))
			return -1;
		if ((pr->dip6_m.s6_addr32[0] ||
		     pr->dip6_m.s6_addr32[1] ||
		     pr->dip6_m.s6_addr32[2] ||
		     pr->dip6_m.s6_addr32[3]) &&
		    !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_DIP2))
			return -1;
	}

	if (ether_addr_to_u64(pr->smac_m) && !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_SMAC0))
		return -1;

	if (ether_addr_to_u64(pr->dmac_m) && !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_DMAC0))
		return -1;

	if (pr->itag_m && !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_ITAG))
		return -1;

	if (pr->sport_m && !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_L4_SPORT))
		return -1;

	if (pr->dport_m && !rtl839x_pie_templ_has(t, RTL839X_TEMPLATE_FIELD_L4_DPORT))
		return -1;

	/* TODO: Check more */

	i = find_first_zero_bit(&priv->pie_use_bm[block * 4], PIE_BLOCK_SIZE);

	if (i >= PIE_BLOCK_SIZE)
		return -1;

	return i + PIE_BLOCK_SIZE * block;
}

int rtl839x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx, block, j, t;
	int min_block = 0;
	int max_block = priv->r->n_pie_blocks / 2;

	if (pr->is_egress) {
		min_block = max_block;
		max_block = priv->r->n_pie_blocks;
	}

	mutex_lock(&priv->pie_mutex);

	for (block = min_block; block < max_block; block++) {
		for (j = 0; j < 2; j++) {
			t = (sw_r32(RTL839X_ACL_BLK_TMPLTE_CTRL(block)) >> (j * 3)) & 0x7;
			idx = rtl839x_pie_verify_template(priv, pr, t, block);
			if (idx >= 0)
				break;
		}
		if (j < 2)
			break;
	}

	if (block >= max_block) {
		mutex_unlock(&priv->pie_mutex);
		return -EOPNOTSUPP;
	}

	set_bit(idx, priv->pie_use_bm);

	pr->valid = true;
	pr->tid = j;  /* Mapped to template number */
	pr->tid_m = 0x3;
	pr->id = idx;

	rtl839x_pie_lookup_enable(priv, idx);
	rtl839x_pie_rule_write(priv, idx, pr);

	mutex_unlock(&priv->pie_mutex);

	return 0;
}

void rtl839x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx = pr->id;

	rtl839x_pie_rule_del(priv, idx, idx);
	clear_bit(idx, priv->pie_use_bm);
}

void rtl839x_pie_init(struct rtl838x_switch_priv *priv)
{
	u32 template_selectors;

	mutex_init(&priv->pie_mutex);

	/* Power on all PIE blocks */
	for (int i = 0; i < priv->r->n_pie_blocks; i++)
		sw_w32_mask(0, BIT(i), RTL839X_PS_ACL_PWR_CTRL);

	/* Set ingress and egress ACL blocks to 50/50: first Egress block is 9 */
	sw_w32_mask(0x1f, 9, RTL839X_ACL_CTRL);  /* Writes 9 to cutline field */

	/* Include IPG in metering */
	sw_w32(1, RTL839X_METER_GLB_CTRL);

	/* Delete all present rules */
	rtl839x_pie_rule_del(priv, 0, priv->r->n_pie_blocks * PIE_BLOCK_SIZE - 1);

	/* Enable predefined templates 0, 1 for blocks 0-2 */
	template_selectors = 0 | (1 << 3);
	for (int i = 0; i < 3; i++)
		sw_w32(template_selectors, RTL839X_ACL_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 2, 3 for blocks 3-5 */
	template_selectors = 2 | (3 << 3);
	for (int i = 3; i < 6; i++)
		sw_w32(template_selectors, RTL839X_ACL_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 1, 4 for blocks 6-8 */
	template_selectors = 2 | (3 << 3);
	for (int i = 6; i < 9; i++)
		sw_w32(template_selectors, RTL839X_ACL_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 0, 1 for blocks 9-11 */
	template_selectors = 0 | (1 << 3);
	for (int i = 9; i < 12; i++)
		sw_w32(template_selectors, RTL839X_ACL_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 2, 3 for blocks 12-14 */
	template_selectors = 2 | (3 << 3);
	for (int i = 12; i < 15; i++)
		sw_w32(template_selectors, RTL839X_ACL_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 1, 4 for blocks 15-17 */
	template_selectors = 2 | (3 << 3);
	for (int i = 15; i < 18; i++)
		sw_w32(template_selectors, RTL839X_ACL_BLK_TMPLTE_CTRL(i));
}

/* RTL930X */

/* Definition of the RTL930X-specific template field IDs as used in the PIE */
enum rtl930x_template_field_id {
	RTL930X_TEMPLATE_FIELD_SPM0 = 0,		/* Source portmask ports 0-15 */
	RTL930X_TEMPLATE_FIELD_SPM1 = 1,		/* Source portmask ports 16-31 */
	RTL930X_TEMPLATE_FIELD_DMAC0 = 2,		/* Destination MAC [15:0] */
	RTL930X_TEMPLATE_FIELD_DMAC1 = 3,		/* Destination MAC [31:16] */
	RTL930X_TEMPLATE_FIELD_DMAC2 = 4,		/* Destination MAC [47:32] */
	RTL930X_TEMPLATE_FIELD_SMAC0 = 5,		/* Source MAC [15:0] */
	RTL930X_TEMPLATE_FIELD_SMAC1 = 6,		/* Source MAC [31:16] */
	RTL930X_TEMPLATE_FIELD_SMAC2 = 7,		/* Source MAC [47:32] */
	RTL930X_TEMPLATE_FIELD_ETHERTYPE = 8,		/* Ethernet frame type field */
	RTL930X_TEMPLATE_FIELD_OTAG = 9,
	RTL930X_TEMPLATE_FIELD_ITAG = 10,
	RTL930X_TEMPLATE_FIELD_SIP0 = 11,
	RTL930X_TEMPLATE_FIELD_SIP1 = 12,
	RTL930X_TEMPLATE_FIELD_DIP0 = 13,
	RTL930X_TEMPLATE_FIELD_DIP1 = 14,
	RTL930X_TEMPLATE_FIELD_IP_TOS_PROTO = 15,
	RTL930X_TEMPLATE_FIELD_L4_SPORT = 16,
	RTL930X_TEMPLATE_FIELD_L4_DPORT = 17,
	RTL930X_TEMPLATE_FIELD_L34_HEADER = 18,
	RTL930X_TEMPLATE_FIELD_TCP_INFO = 19,
	RTL930X_TEMPLATE_FIELD_FIELD_SELECTOR_VALID = 20,
	RTL930X_TEMPLATE_FIELD_FIELD_SELECTOR_0 = 21,
	RTL930X_TEMPLATE_FIELD_FIELD_SELECTOR_1 = 22,
	RTL930X_TEMPLATE_FIELD_FIELD_SELECTOR_2 = 23,
	RTL930X_TEMPLATE_FIELD_FIELD_SELECTOR_3 = 24,
	RTL930X_TEMPLATE_FIELD_FIELD_SELECTOR_4 = 25,
	RTL930X_TEMPLATE_FIELD_FIELD_SELECTOR_5 = 26,
	RTL930X_TEMPLATE_FIELD_SIP2 = 27,
	RTL930X_TEMPLATE_FIELD_SIP3 = 28,
	RTL930X_TEMPLATE_FIELD_SIP4 = 29,
	RTL930X_TEMPLATE_FIELD_SIP5 = 30,
	RTL930X_TEMPLATE_FIELD_SIP6 = 31,
	RTL930X_TEMPLATE_FIELD_SIP7 = 32,
	RTL930X_TEMPLATE_FIELD_DIP2 = 33,
	RTL930X_TEMPLATE_FIELD_DIP3 = 34,
	RTL930X_TEMPLATE_FIELD_DIP4 = 35,
	RTL930X_TEMPLATE_FIELD_DIP5 = 36,
	RTL930X_TEMPLATE_FIELD_DIP6 = 37,
	RTL930X_TEMPLATE_FIELD_DIP7 = 38,
	RTL930X_TEMPLATE_FIELD_PKT_INFO = 39,
	RTL930X_TEMPLATE_FIELD_FLOW_LABEL = 40,
	RTL930X_TEMPLATE_FIELD_DSAP_SSAP = 41,
	RTL930X_TEMPLATE_FIELD_SNAP_OUI = 42,
	RTL930X_TEMPLATE_FIELD_FWD_VID = 43,
	RTL930X_TEMPLATE_FIELD_RANGE_CHK = 44,
	RTL930X_TEMPLATE_FIELD_VLAN_GMSK = 45,		/* VLAN Group Mask/IP range check */
	RTL930X_TEMPLATE_FIELD_DLP = 46,
	RTL930X_TEMPLATE_FIELD_META_DATA = 47,
	RTL930X_TEMPLATE_FIELD_SRC_FWD_VID = 48,
	RTL930X_TEMPLATE_FIELD_SLP = 49,
};

/* The meaning of RTL930X_TEMPLATE_FIELD_VLAN depends on phase and the configuration in
 * RTL930X_PIE_CTRL. We use always the same definition and map to the inner VLAN tag:
 */
#define RTL930X_TEMPLATE_FIELD_VLAN RTL930X_TEMPLATE_FIELD_ITAG

/* Number of fixed templates predefined in the RTL9300 SoC */
#define RTL930X_N_FIXED_TEMPLATES 5
/* RTL9300 specific predefined templates */
static enum rtl930x_template_field_id rtl930x_fixed_templates[RTL930X_N_FIXED_TEMPLATES][N_FIXED_FIELDS] = {
	{
	  RTL930X_TEMPLATE_FIELD_DMAC0, RTL930X_TEMPLATE_FIELD_DMAC1, RTL930X_TEMPLATE_FIELD_DMAC2,
	  RTL930X_TEMPLATE_FIELD_SMAC0, RTL930X_TEMPLATE_FIELD_SMAC1, RTL930X_TEMPLATE_FIELD_SMAC2,
	  RTL930X_TEMPLATE_FIELD_VLAN, RTL930X_TEMPLATE_FIELD_IP_TOS_PROTO, RTL930X_TEMPLATE_FIELD_DSAP_SSAP,
	  RTL930X_TEMPLATE_FIELD_ETHERTYPE, RTL930X_TEMPLATE_FIELD_SPM0, RTL930X_TEMPLATE_FIELD_SPM1
	}, {
	  RTL930X_TEMPLATE_FIELD_SIP0, RTL930X_TEMPLATE_FIELD_SIP1, RTL930X_TEMPLATE_FIELD_DIP0,
	  RTL930X_TEMPLATE_FIELD_DIP1, RTL930X_TEMPLATE_FIELD_IP_TOS_PROTO, RTL930X_TEMPLATE_FIELD_TCP_INFO,
	  RTL930X_TEMPLATE_FIELD_L4_SPORT, RTL930X_TEMPLATE_FIELD_L4_DPORT, RTL930X_TEMPLATE_FIELD_VLAN,
	  RTL930X_TEMPLATE_FIELD_RANGE_CHK, RTL930X_TEMPLATE_FIELD_SPM0, RTL930X_TEMPLATE_FIELD_SPM1
	}, {
	  RTL930X_TEMPLATE_FIELD_DMAC0, RTL930X_TEMPLATE_FIELD_DMAC1, RTL930X_TEMPLATE_FIELD_DMAC2,
	  RTL930X_TEMPLATE_FIELD_VLAN, RTL930X_TEMPLATE_FIELD_ETHERTYPE, RTL930X_TEMPLATE_FIELD_IP_TOS_PROTO,
	  RTL930X_TEMPLATE_FIELD_SIP0, RTL930X_TEMPLATE_FIELD_SIP1, RTL930X_TEMPLATE_FIELD_DIP0,
	  RTL930X_TEMPLATE_FIELD_DIP1, RTL930X_TEMPLATE_FIELD_L4_SPORT, RTL930X_TEMPLATE_FIELD_L4_DPORT
	}, {
	  RTL930X_TEMPLATE_FIELD_DIP0, RTL930X_TEMPLATE_FIELD_DIP1, RTL930X_TEMPLATE_FIELD_DIP2,
	  RTL930X_TEMPLATE_FIELD_DIP3, RTL930X_TEMPLATE_FIELD_DIP4, RTL930X_TEMPLATE_FIELD_DIP5,
	  RTL930X_TEMPLATE_FIELD_DIP6, RTL930X_TEMPLATE_FIELD_DIP7, RTL930X_TEMPLATE_FIELD_IP_TOS_PROTO,
	  RTL930X_TEMPLATE_FIELD_TCP_INFO, RTL930X_TEMPLATE_FIELD_L4_SPORT, RTL930X_TEMPLATE_FIELD_L4_DPORT
	}, {
	  RTL930X_TEMPLATE_FIELD_SIP0, RTL930X_TEMPLATE_FIELD_SIP1, RTL930X_TEMPLATE_FIELD_SIP2,
	  RTL930X_TEMPLATE_FIELD_SIP3, RTL930X_TEMPLATE_FIELD_SIP4, RTL930X_TEMPLATE_FIELD_SIP5,
	  RTL930X_TEMPLATE_FIELD_SIP6, RTL930X_TEMPLATE_FIELD_SIP7, RTL930X_TEMPLATE_FIELD_VLAN,
	  RTL930X_TEMPLATE_FIELD_RANGE_CHK, RTL930X_TEMPLATE_FIELD_SPM1, RTL930X_TEMPLATE_FIELD_SPM1
	},
};

static void rtl930x_pie_lookup_enable(struct rtl838x_switch_priv *priv, int index)
{
	int block = index / PIE_BLOCK_SIZE;

	sw_w32_mask(0, BIT(block), RTL930X_PIE_BLK_LOOKUP_CTRL);
}

/* Reads the intermediate representation of the templated match-fields of the
 * PIE rule in the pie_rule structure and fills in the raw data fields in the
 * raw register space r[].
 * The register space configuration size is identical for the RTL8380/90 and RTL9300,
 * however the RTL9310 has 2 more registers / fields and the physical field-ids are different
 * on all SoCs
 * On the RTL9300 the mask fields are not word-aligend!
 */
static void rtl930x_write_pie_templated(u32 r[], struct pie_rule *pr, enum rtl930x_template_field_id t[])
{
	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		enum rtl930x_template_field_id field_type = t[i];
		u16 data = 0, data_m = 0;

		switch (field_type) {
		case RTL930X_TEMPLATE_FIELD_SPM0:
			data = pr->spm;
			data_m = pr->spm_m;
			break;
		case RTL930X_TEMPLATE_FIELD_SPM1:
			data = pr->spm >> 16;
			data_m = pr->spm_m >> 16;
			break;
		case RTL930X_TEMPLATE_FIELD_OTAG:
			data = pr->otag;
			data_m = pr->otag_m;
			break;
		case RTL930X_TEMPLATE_FIELD_SMAC0:
			data = pr->smac[4];
			data = (data << 8) | pr->smac[5];
			data_m = pr->smac_m[4];
			data_m = (data_m << 8) | pr->smac_m[5];
			break;
		case RTL930X_TEMPLATE_FIELD_SMAC1:
			data = pr->smac[2];
			data = (data << 8) | pr->smac[3];
			data_m = pr->smac_m[2];
			data_m = (data_m << 8) | pr->smac_m[3];
			break;
		case RTL930X_TEMPLATE_FIELD_SMAC2:
			data = pr->smac[0];
			data = (data << 8) | pr->smac[1];
			data_m = pr->smac_m[0];
			data_m = (data_m << 8) | pr->smac_m[1];
			break;
		case RTL930X_TEMPLATE_FIELD_DMAC0:
			data = pr->dmac[4];
			data = (data << 8) | pr->dmac[5];
			data_m = pr->dmac_m[4];
			data_m = (data_m << 8) | pr->dmac_m[5];
			break;
		case RTL930X_TEMPLATE_FIELD_DMAC1:
			data = pr->dmac[2];
			data = (data << 8) | pr->dmac[3];
			data_m = pr->dmac_m[2];
			data_m = (data_m << 8) | pr->dmac_m[3];
			break;
		case RTL930X_TEMPLATE_FIELD_DMAC2:
			data = pr->dmac[0];
			data = (data << 8) | pr->dmac[1];
			data_m = pr->dmac_m[0];
			data_m = (data_m << 8) | pr->dmac_m[1];
			break;
		case RTL930X_TEMPLATE_FIELD_ETHERTYPE:
			data = pr->ethertype;
			data_m = pr->ethertype_m;
			break;
		case RTL930X_TEMPLATE_FIELD_ITAG:
			data = pr->itag;
			data_m = pr->itag_m;
			break;
		case RTL930X_TEMPLATE_FIELD_SIP0:
			if (pr->is_ipv6) {
				data = pr->sip6.s6_addr16[7];
				data_m = pr->sip6_m.s6_addr16[7];
			} else {
				data = pr->sip;
				data_m = pr->sip_m;
			}
			break;
		case RTL930X_TEMPLATE_FIELD_SIP1:
			if (pr->is_ipv6) {
				data = pr->sip6.s6_addr16[6];
				data_m = pr->sip6_m.s6_addr16[6];
			} else {
				data = pr->sip >> 16;
				data_m = pr->sip_m >> 16;
			}
			break;
		case RTL930X_TEMPLATE_FIELD_SIP2:
		case RTL930X_TEMPLATE_FIELD_SIP3:
		case RTL930X_TEMPLATE_FIELD_SIP4:
		case RTL930X_TEMPLATE_FIELD_SIP5:
		case RTL930X_TEMPLATE_FIELD_SIP6:
		case RTL930X_TEMPLATE_FIELD_SIP7:
			data = pr->sip6.s6_addr16[5 - (field_type - RTL930X_TEMPLATE_FIELD_SIP2)];
			data_m = pr->sip6_m.s6_addr16[5 - (field_type - RTL930X_TEMPLATE_FIELD_SIP2)];
			break;
		case RTL930X_TEMPLATE_FIELD_DIP0:
			if (pr->is_ipv6) {
				data = pr->dip6.s6_addr16[7];
				data_m = pr->dip6_m.s6_addr16[7];
			} else {
				data = pr->dip;
				data_m = pr->dip_m;
			}
			break;
		case RTL930X_TEMPLATE_FIELD_DIP1:
			if (pr->is_ipv6) {
				data = pr->dip6.s6_addr16[6];
				data_m = pr->dip6_m.s6_addr16[6];
			} else {
				data = pr->dip >> 16;
				data_m = pr->dip_m >> 16;
			}
			break;
		case RTL930X_TEMPLATE_FIELD_DIP2:
		case RTL930X_TEMPLATE_FIELD_DIP3:
		case RTL930X_TEMPLATE_FIELD_DIP4:
		case RTL930X_TEMPLATE_FIELD_DIP5:
		case RTL930X_TEMPLATE_FIELD_DIP6:
		case RTL930X_TEMPLATE_FIELD_DIP7:
			data = pr->dip6.s6_addr16[5 - (field_type - RTL930X_TEMPLATE_FIELD_DIP2)];
			data_m = pr->dip6_m.s6_addr16[5 - (field_type - RTL930X_TEMPLATE_FIELD_DIP2)];
			break;
		case RTL930X_TEMPLATE_FIELD_IP_TOS_PROTO:
			data = pr->tos_proto;
			data_m = pr->tos_proto_m;
			break;
		case RTL930X_TEMPLATE_FIELD_L4_SPORT:
			data = pr->sport;
			data_m = pr->sport_m;
			break;
		case RTL930X_TEMPLATE_FIELD_L4_DPORT:
			data = pr->dport;
			data_m = pr->dport_m;
			break;
		case RTL930X_TEMPLATE_FIELD_DSAP_SSAP:
			data = pr->dsap_ssap;
			data_m = pr->dsap_ssap_m;
			break;
		case RTL930X_TEMPLATE_FIELD_TCP_INFO:
			data = pr->tcp_info;
			data_m = pr->tcp_info_m;
			break;
		case RTL930X_TEMPLATE_FIELD_RANGE_CHK:
			pr_debug("RTL930X_TEMPLATE_FIELD_RANGE_CHK: not configured\n");
			break;
		default:
			pr_debug("%s: unknown field %d\n", __func__, field_type);
		}

		/* On the RTL9300, the mask fields are not word aligned! */
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

// Currently not used
// static void rtl930x_read_pie_fixed_fields(u32 r[], struct pie_rule *pr)
// {
// 	pr->stacking_port = r[6] & BIT(31);
// 	pr->spn = (r[6] >> 24) & 0x7f;
// 	pr->mgnt_vlan = r[6] & BIT(23);
// 	if (pr->phase == PHASE_IACL)
// 		pr->dmac_hit_sw = r[6] & BIT(22);
// 	else
// 		pr->content_too_deep = r[6] & BIT(22);
// 	pr->not_first_frag = r[6]  & BIT(21);
// 	pr->frame_type_l4 = (r[6] >> 18) & 7;
// 	pr->frame_type = (r[6] >> 16) & 3;
// 	pr->otag_fmt = (r[6] >> 15) & 1;
// 	pr->itag_fmt = (r[6] >> 14) & 1;
// 	pr->otag_exist = (r[6] >> 13) & 1;
// 	pr->itag_exist = (r[6] >> 12) & 1;
// 	pr->frame_type_l2 = (r[6] >> 10) & 3;
// 	pr->igr_normal_port = (r[6] >> 9) & 1;
// 	pr->tid = (r[6] >> 8) & 1;

// 	pr->stacking_port_m = r[12] & BIT(7);
// 	pr->spn_m = r[12]  & 0x7f;
// 	pr->mgnt_vlan_m = r[13] & BIT(31);
// 	if (pr->phase == PHASE_IACL)
// 		pr->dmac_hit_sw_m = r[13] & BIT(30);
// 	else
// 		pr->content_too_deep_m = r[13] & BIT(30);
// 	pr->not_first_frag_m = r[13] & BIT(29);
// 	pr->frame_type_l4_m = (r[13] >> 26) & 7;
// 	pr->frame_type_m = (r[13] >> 24) & 3;
// 	pr->otag_fmt_m = r[13] & BIT(23);
// 	pr->itag_fmt_m = r[13] & BIT(22);
// 	pr->otag_exist_m = r[13] & BIT(21);
// 	pr->itag_exist_m = r[13] & BIT (20);
// 	pr->frame_type_l2_m = (r[13] >> 18) & 3;
// 	pr->igr_normal_port_m = r[13] & BIT(17);
// 	pr->tid_m = (r[13] >> 16) & 1;

// 	pr->valid = r[13] & BIT(15);
// 	pr->cond_not = r[13] & BIT(14);
// 	pr->cond_and1 = r[13] & BIT(13);
// 	pr->cond_and2 = r[13] & BIT(12);
// }

static void rtl930x_write_pie_fixed_fields(u32 r[],  struct pie_rule *pr)
{
	r[6] = pr->stacking_port ? BIT(31) : 0;
	r[6] |= ((u32)(pr->spn & 0x7f)) << 24;
	r[6] |= pr->mgnt_vlan ? BIT(23) : 0;
	if (pr->phase == PHASE_IACL)
		r[6] |= pr->dmac_hit_sw ? BIT(22) : 0;
	else
		r[6] |= pr->content_too_deep ? BIT(22) : 0;
	r[6] |= pr->not_first_frag ? BIT(21) : 0;
	r[6] |= ((u32)(pr->frame_type_l4 & 0x7)) << 18;
	r[6] |= ((u32)(pr->frame_type & 0x3)) << 16;
	r[6] |= pr->otag_fmt ? BIT(15) : 0;
	r[6] |= pr->itag_fmt ? BIT(14) : 0;
	r[6] |= pr->otag_exist ? BIT(13) : 0;
	r[6] |= pr->itag_exist ? BIT(12) : 0;
	r[6] |= ((u32)(pr->frame_type_l2 & 0x3)) << 10;
	r[6] |= pr->igr_normal_port ? BIT(9) : 0;
	r[6] |= ((u32)(pr->tid & 0x1)) << 8;

	r[12] |= pr->stacking_port_m ? BIT(7) : 0;
	r[12] |= (u32)(pr->spn_m & 0x7f);
	r[13] |= pr->mgnt_vlan_m ? BIT(31) : 0;
	if (pr->phase == PHASE_IACL)
		r[13] |= pr->dmac_hit_sw_m ? BIT(30) : 0;
	else
		r[13] |= pr->content_too_deep_m ? BIT(30) : 0;
	r[13] |= pr->not_first_frag_m ? BIT(29) : 0;
	r[13] |= ((u32)(pr->frame_type_l4_m & 0x7)) << 26;
	r[13] |= ((u32)(pr->frame_type_m & 0x3)) << 24;
	r[13] |= pr->otag_fmt_m ? BIT(23) : 0;
	r[13] |= pr->itag_fmt_m ? BIT(22) : 0;
	r[13] |= pr->otag_exist_m ? BIT(21) : 0;
	r[13] |= pr->itag_exist_m ? BIT(20) : 0;
	r[13] |= ((u32)(pr->frame_type_l2_m & 0x3)) << 18;
	r[13] |= pr->igr_normal_port_m ? BIT(17) : 0;
	r[13] |= ((u32)(pr->tid_m & 0x1)) << 16;

	r[13] |= pr->valid ? BIT(15) : 0;
	r[13] |= pr->cond_not ? BIT(14) : 0;
	r[13] |= pr->cond_and1 ? BIT(13) : 0;
	r[13] |= pr->cond_and2 ? BIT(12) : 0;
}

static void rtl930x_write_pie_action(u32 r[],  struct pie_rule *pr)
{
	/* Either drop or forward */
	if (pr->drop) {
		r[14] |= BIT(24) | BIT(25) | BIT(26); /* Do Green, Yellow and Red drops */
		/* Actually DROP, not PERMIT in Green / Yellow / Red */
		r[14] |= BIT(23) | BIT(22) | BIT(20);
	} else {
		r[14] |= pr->fwd_sel ? BIT(27) : 0;
		r[14] |= pr->fwd_act << 18;
		r[14] |= BIT(14); /* We overwrite any drop */
	}
	if (pr->phase == PHASE_VACL)
		r[14] |= pr->fwd_sa_lrn ? BIT(15) : 0;
	r[13] |= pr->bypass_sel ? BIT(5) : 0;
	r[13] |= pr->nopri_sel ? BIT(4) : 0;
	r[13] |= pr->tagst_sel ? BIT(3) : 0;
	r[13] |= pr->ovid_sel ? BIT(1) : 0;
	r[14] |= pr->ivid_sel ? BIT(31) : 0;
	r[14] |= pr->meter_sel ? BIT(30) : 0;
	r[14] |= pr->mir_sel ? BIT(29) : 0;
	r[14] |= pr->log_sel ? BIT(28) : 0;

	r[14] |= ((u32)(pr->fwd_data & 0x3fff)) << 3;
	r[15] |= pr->log_octets ? BIT(31) : 0;
	r[15] |= (u32)(pr->meter_data) << 23;

	r[15] |= ((u32)(pr->ivid_act) & 0x3) << 21;
	r[15] |= ((u32)(pr->ivid_data) & 0xfff) << 9;
	r[16] |= ((u32)(pr->ovid_act) & 0x3) << 30;
	r[16] |= ((u32)(pr->ovid_data) & 0xfff) << 16;
	r[16] |= (pr->mir_data & 0x3) << 6;
	r[17] |= ((u32)(pr->tagst_data) & 0xf) << 28;
	r[17] |= ((u32)(pr->nopri_data) & 0x7) << 25;
	r[17] |= pr->bypass_ibc_sc ? BIT(16) : 0;
}

void rtl930x_pie_rule_dump_raw(u32 r[])
{
	pr_debug("Raw IACL table entry:\n");
	pr_debug("r 0 - 7: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		 r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
	pr_debug("r 8 - 15: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		 r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15]);
	pr_debug("r 16 - 18: %08x %08x %08x\n", r[16], r[17], r[18]);
	pr_debug("Match  : %08x %08x %08x %08x %08x %08x\n", r[0], r[1], r[2], r[3], r[4], r[5]);
	pr_debug("Fixed  : %06x\n", r[6] >> 8);
	pr_debug("Match M: %08x %08x %08x %08x %08x %08x\n",
		 (r[6] << 24) | (r[7] >> 8), (r[7] << 24) | (r[8] >> 8), (r[8] << 24) | (r[9] >> 8),
		 (r[9] << 24) | (r[10] >> 8), (r[10] << 24) | (r[11] >> 8),
		 (r[11] << 24) | (r[12] >> 8));
	pr_debug("R[13]:   %08x\n", r[13]);
	pr_debug("Fixed M: %06x\n", ((r[12] << 16) | (r[13] >> 16)) & 0xffffff);
	pr_debug("Valid / not / and1 / and2 : %1x\n", (r[13] >> 12) & 0xf);
	pr_debug("r 13-16: %08x %08x %08x %08x\n", r[13], r[14], r[15], r[16]);
}

int rtl930x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr)
{
	int tbl = otto_table_acquire(RTL9300_TBL_IACL);
	u32 r[19];
	int block = idx / PIE_BLOCK_SIZE;
	u32 t_select = sw_r32(RTL930X_PIE_BLK_TMPLTE_CTRL(block));

	pr_debug("%s: %d, t_select: %08x\n", __func__, idx, t_select);

	for (int i = 0; i < 19; i++)
		r[i] = 0;

	if (!pr->valid) {
		__otto_table_write(tbl, idx, &r);
		otto_table_release(tbl);
		return 0;
	}
	rtl930x_write_pie_fixed_fields(r, pr);

	pr_debug("%s: template %d\n", __func__, (t_select >> (pr->tid * 4)) & 0xf);
	rtl930x_write_pie_templated(r, pr, rtl930x_fixed_templates[(t_select >> (pr->tid * 4)) & 0xf]);

	rtl930x_write_pie_action(r, pr);

/*	rtl930x_pie_rule_dump_raw(r); */

	__otto_table_write(tbl, idx, &r);
	otto_table_release(tbl);

	return 0;
}

static bool rtl930x_pie_templ_has(int t, enum rtl930x_template_field_id field_type)
{
	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		enum rtl930x_template_field_id ft = rtl930x_fixed_templates[t][i];

		if (field_type == ft)
			return true;
	}

	return false;
}

/* Verify that the rule pr is compatible with a given template t in block
 * Note that this function is SoC specific since the values of e.g. RTL930X_TEMPLATE_FIELD_SIP0
 * depend on the SoC
 */
static int rtl930x_pie_verify_template(struct rtl838x_switch_priv *priv,
				       struct pie_rule *pr, int t, int block)
{
	int i;

	if (!pr->is_ipv6 && pr->sip_m && !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_SIP0))
		return -1;

	if (!pr->is_ipv6 && pr->dip_m && !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_DIP0))
		return -1;

	if (pr->is_ipv6) {
		if ((pr->sip6_m.s6_addr32[0] ||
		     pr->sip6_m.s6_addr32[1] ||
		     pr->sip6_m.s6_addr32[2] ||
		     pr->sip6_m.s6_addr32[3]) &&
		    !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_SIP2))
			return -1;
		if ((pr->dip6_m.s6_addr32[0] ||
		     pr->dip6_m.s6_addr32[1] ||
		     pr->dip6_m.s6_addr32[2] ||
		     pr->dip6_m.s6_addr32[3]) &&
		    !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_DIP2))
			return -1;
	}

	if (ether_addr_to_u64(pr->smac_m) && !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_SMAC0))
		return -1;

	if (ether_addr_to_u64(pr->dmac_m) && !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_DMAC0))
		return -1;

	if (pr->ethertype_m && !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_ETHERTYPE))
		return -1;

	if (pr->itag_m && !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_VLAN))
		return -1;

	if (pr->sport_m && !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_L4_SPORT))
		return -1;

	if (pr->dport_m && !rtl930x_pie_templ_has(t, RTL930X_TEMPLATE_FIELD_L4_DPORT))
		return -1;

	/* TODO: Check more */

	i = find_first_zero_bit(&priv->pie_use_bm[block * 4], PIE_BLOCK_SIZE);

	if (i >= PIE_BLOCK_SIZE)
		return -1;

	return i + PIE_BLOCK_SIZE * block;
}

int rtl930x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx, block, j, t;
	int min_block = 0;
	int max_block = priv->r->n_pie_blocks / 2;

	if (pr->is_egress) {
		min_block = max_block;
		max_block = priv->r->n_pie_blocks;
	}
	pr_debug("In %s\n", __func__);

	mutex_lock(&priv->pie_mutex);

	for (block = min_block; block < max_block; block++) {
		for (j = 0; j < 2; j++) {
			t = (sw_r32(RTL930X_PIE_BLK_TMPLTE_CTRL(block)) >> (j * 4)) & 0xf;
			pr_debug("Testing block %d, template %d, template id %d\n", block, j, t);
			pr_debug("%s: %08x\n",
				 __func__, sw_r32(RTL930X_PIE_BLK_TMPLTE_CTRL(block)));
			idx = rtl930x_pie_verify_template(priv, pr, t, block);
			if (idx >= 0)
				break;
		}
		if (j < 2)
			break;
	}

	if (block >= max_block) {
		mutex_unlock(&priv->pie_mutex);
		return -EOPNOTSUPP;
	}

	pr_debug("Using block: %d, index %d, template-id %d\n", block, idx, j);
	set_bit(idx, priv->pie_use_bm);

	pr->valid = true;
	pr->tid = j;  /* Mapped to template number */
	pr->tid_m = 0x1;
	pr->id = idx;

	rtl930x_pie_lookup_enable(priv, idx);
	rtl930x_pie_rule_write(priv, idx, pr);

	mutex_unlock(&priv->pie_mutex);
	return 0;
}

/* Delete a range of Packet Inspection Engine rules */
static int rtl930x_pie_rule_del(struct rtl838x_switch_priv *priv, int index_from, int index_to)
{
	u32 v = (index_from << 1) | (index_to << 12) | BIT(0);

	pr_debug("%s: from %d to %d\n", __func__, index_from, index_to);
	mutex_lock(&priv->reg_mutex);

	/* Write from-to and execute bit into control register */
	sw_w32(v, RTL930X_PIE_CLR_CTRL);

	/* Wait until command has completed */
	do {
	} while (sw_r32(RTL930X_PIE_CLR_CTRL) & BIT(0));

	mutex_unlock(&priv->reg_mutex);
	return 0;
}

void rtl930x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx = pr->id;

	rtl930x_pie_rule_del(priv, idx, idx);
	clear_bit(idx, priv->pie_use_bm);
}

void rtl930x_pie_init(struct rtl838x_switch_priv *priv)
{
	u32 template_selectors;

	mutex_init(&priv->pie_mutex);

	pr_debug("%s\n", __func__);
	/* Enable ACL lookup on all ports, including CPU_PORT */
	for (int i = 0; i <= priv->r->cpu_port; i++)
		sw_w32(1, RTL930X_ACL_PORT_LOOKUP_CTRL(i));

	/* Include IPG in metering */
	sw_w32_mask(0, 1, RTL930X_METER_GLB_CTRL);

	/* Delete all present rules, block size is 128 on all SoC families */
	rtl930x_pie_rule_del(priv, 0, priv->r->n_pie_blocks * 128 - 1);

	/* Assign blocks 0-7 to VACL phase (bit = 0), blocks 8-15 to IACL (bit = 1) */
	sw_w32(0xff00, RTL930X_PIE_BLK_PHASE_CTRL);

	/* Enable predefined templates 0, 1 for first quarter of all blocks */
	template_selectors = 0 | (1 << 4);
	for (int i = 0; i < priv->r->n_pie_blocks / 4; i++)
		sw_w32(template_selectors, RTL930X_PIE_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 2, 3 for second quarter of all blocks */
	template_selectors = 2 | (3 << 4);
	for (int i = priv->r->n_pie_blocks / 4; i < priv->r->n_pie_blocks / 2; i++)
		sw_w32(template_selectors, RTL930X_PIE_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 0, 1 for third half of all blocks */
	template_selectors = 0 | (1 << 4);
	for (int i = priv->r->n_pie_blocks / 2; i < priv->r->n_pie_blocks * 3 / 4; i++)
		sw_w32(template_selectors, RTL930X_PIE_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 2, 3 for fourth quater of all blocks */
	template_selectors = 2 | (3 << 4);
	for (int i = priv->r->n_pie_blocks * 3 / 4; i < priv->r->n_pie_blocks; i++)
		sw_w32(template_selectors, RTL930X_PIE_BLK_TMPLTE_CTRL(i));
}

/* RTL931X */

/* Definition of the RTL931X-specific template field IDs as used in the PIE */
enum rtl931x_template_field_id {
	RTL931X_TEMPLATE_FIELD_SPM0 = 1,
	RTL931X_TEMPLATE_FIELD_SPM1 = 2,
	RTL931X_TEMPLATE_FIELD_SPM2 = 3,
	RTL931X_TEMPLATE_FIELD_SPM3 = 4,
	RTL931X_TEMPLATE_FIELD_DMAC0 = 9,
	RTL931X_TEMPLATE_FIELD_DMAC1 = 10,
	RTL931X_TEMPLATE_FIELD_DMAC2 = 11,
	RTL931X_TEMPLATE_FIELD_SMAC0 = 12,
	RTL931X_TEMPLATE_FIELD_SMAC1 = 13,
	RTL931X_TEMPLATE_FIELD_SMAC2 = 14,
	RTL931X_TEMPLATE_FIELD_ETHERTYPE = 15,
	RTL931X_TEMPLATE_FIELD_OTAG = 16,
	RTL931X_TEMPLATE_FIELD_ITAG = 17,
	RTL931X_TEMPLATE_FIELD_SIP0 = 18,
	RTL931X_TEMPLATE_FIELD_SIP1 = 19,
	RTL931X_TEMPLATE_FIELD_DIP0 = 20,
	RTL931X_TEMPLATE_FIELD_DIP1 = 21,
	RTL931X_TEMPLATE_FIELD_IP_TOS_PROTO = 22,
	RTL931X_TEMPLATE_FIELD_L4_SPORT = 23,
	RTL931X_TEMPLATE_FIELD_L4_DPORT = 24,
	RTL931X_TEMPLATE_FIELD_L34_HEADER = 25,
	RTL931X_TEMPLATE_FIELD_TCP_INFO = 26,
	RTL931X_TEMPLATE_FIELD_SIP2 = 34,
	RTL931X_TEMPLATE_FIELD_SIP3 = 35,
	RTL931X_TEMPLATE_FIELD_SIP4 = 36,
	RTL931X_TEMPLATE_FIELD_SIP5 = 37,
	RTL931X_TEMPLATE_FIELD_SIP6 = 38,
	RTL931X_TEMPLATE_FIELD_SIP7 = 39,
	RTL931X_TEMPLATE_FIELD_DIP2 = 42,
	RTL931X_TEMPLATE_FIELD_DIP3 = 43,
	RTL931X_TEMPLATE_FIELD_DIP4 = 44,
	RTL931X_TEMPLATE_FIELD_DIP5 = 45,
	RTL931X_TEMPLATE_FIELD_DIP6 = 46,
	RTL931X_TEMPLATE_FIELD_DIP7 = 47,
	RTL931X_TEMPLATE_FIELD_FLOW_LABEL = 49,
	RTL931X_TEMPLATE_FIELD_DSAP_SSAP = 50,
	RTL931X_TEMPLATE_FIELD_FWD_VID = 52,
	RTL931X_TEMPLATE_FIELD_RANGE_CHK = 53,
	RTL931X_TEMPLATE_FIELD_SLP = 55,
	RTL931X_TEMPLATE_FIELD_DLP = 56,
	RTL931X_TEMPLATE_FIELD_META_DATA = 57,
	RTL931X_TEMPLATE_FIELD_FIRST_MPLS1 = 60,
	RTL931X_TEMPLATE_FIELD_FIRST_MPLS2 = 61,
	RTL931X_TEMPLATE_FIELD_DPM3 = 8,
};

/* The meaning of RTL931X_TEMPLATE_FIELD_VLAN depends on phase and the configuration in
 * RTL931X_PIE_CTRL. We use always the same definition and map to the inner VLAN tag:
 */
#define RTL931X_TEMPLATE_FIELD_VLAN RTL931X_TEMPLATE_FIELD_ITAG

/* Number of fixed templates predefined in the RTL9300 SoC */
#define RTL931X_N_FIXED_TEMPLATES 5
/* RTL931x specific predefined templates */
static enum rtl931x_template_field_id rtl931x_fixed_templates[RTL931X_N_FIXED_TEMPLATES][N_FIXED_FIELDS_RTL931X] = {
	{
		RTL931X_TEMPLATE_FIELD_DMAC0, RTL931X_TEMPLATE_FIELD_DMAC1, RTL931X_TEMPLATE_FIELD_DMAC2,
		RTL931X_TEMPLATE_FIELD_SMAC0, RTL931X_TEMPLATE_FIELD_SMAC1, RTL931X_TEMPLATE_FIELD_SMAC2,
		RTL931X_TEMPLATE_FIELD_VLAN, RTL931X_TEMPLATE_FIELD_IP_TOS_PROTO, RTL931X_TEMPLATE_FIELD_DSAP_SSAP,
		RTL931X_TEMPLATE_FIELD_ETHERTYPE, RTL931X_TEMPLATE_FIELD_SPM0, RTL931X_TEMPLATE_FIELD_SPM1,
		RTL931X_TEMPLATE_FIELD_SPM2, RTL931X_TEMPLATE_FIELD_SPM3
	}, {
		RTL931X_TEMPLATE_FIELD_SIP0, RTL931X_TEMPLATE_FIELD_SIP1, RTL931X_TEMPLATE_FIELD_DIP0,
		RTL931X_TEMPLATE_FIELD_DIP1, RTL931X_TEMPLATE_FIELD_IP_TOS_PROTO, RTL931X_TEMPLATE_FIELD_TCP_INFO,
		RTL931X_TEMPLATE_FIELD_L4_SPORT, RTL931X_TEMPLATE_FIELD_L4_DPORT, RTL931X_TEMPLATE_FIELD_VLAN,
		RTL931X_TEMPLATE_FIELD_RANGE_CHK, RTL931X_TEMPLATE_FIELD_SPM0, RTL931X_TEMPLATE_FIELD_SPM1,
		RTL931X_TEMPLATE_FIELD_SPM2, RTL931X_TEMPLATE_FIELD_SPM3
	}, {
		RTL931X_TEMPLATE_FIELD_DMAC0, RTL931X_TEMPLATE_FIELD_DMAC1, RTL931X_TEMPLATE_FIELD_DMAC2,
		RTL931X_TEMPLATE_FIELD_VLAN, RTL931X_TEMPLATE_FIELD_ETHERTYPE, RTL931X_TEMPLATE_FIELD_IP_TOS_PROTO,
		RTL931X_TEMPLATE_FIELD_SIP0, RTL931X_TEMPLATE_FIELD_SIP1, RTL931X_TEMPLATE_FIELD_DIP0,
		RTL931X_TEMPLATE_FIELD_DIP1, RTL931X_TEMPLATE_FIELD_L4_SPORT, RTL931X_TEMPLATE_FIELD_L4_DPORT,
		RTL931X_TEMPLATE_FIELD_META_DATA, RTL931X_TEMPLATE_FIELD_SLP
	}, {
		RTL931X_TEMPLATE_FIELD_DIP0, RTL931X_TEMPLATE_FIELD_DIP1, RTL931X_TEMPLATE_FIELD_DIP2,
		RTL931X_TEMPLATE_FIELD_DIP3, RTL931X_TEMPLATE_FIELD_DIP4, RTL931X_TEMPLATE_FIELD_DIP5,
		RTL931X_TEMPLATE_FIELD_DIP6, RTL931X_TEMPLATE_FIELD_DIP7, RTL931X_TEMPLATE_FIELD_IP_TOS_PROTO,
		RTL931X_TEMPLATE_FIELD_TCP_INFO, RTL931X_TEMPLATE_FIELD_L4_SPORT, RTL931X_TEMPLATE_FIELD_L4_DPORT,
		RTL931X_TEMPLATE_FIELD_RANGE_CHK, RTL931X_TEMPLATE_FIELD_SLP
	}, {
		RTL931X_TEMPLATE_FIELD_SIP0, RTL931X_TEMPLATE_FIELD_SIP1, RTL931X_TEMPLATE_FIELD_SIP2,
		RTL931X_TEMPLATE_FIELD_SIP3, RTL931X_TEMPLATE_FIELD_SIP4, RTL931X_TEMPLATE_FIELD_SIP5,
		RTL931X_TEMPLATE_FIELD_SIP6, RTL931X_TEMPLATE_FIELD_SIP7, RTL931X_TEMPLATE_FIELD_META_DATA,
		RTL931X_TEMPLATE_FIELD_VLAN, RTL931X_TEMPLATE_FIELD_SPM0, RTL931X_TEMPLATE_FIELD_SPM1,
		RTL931X_TEMPLATE_FIELD_SPM2, RTL931X_TEMPLATE_FIELD_SPM3
	},
};

static void rtl931x_pie_lookup_enable(struct rtl838x_switch_priv *priv, int index)
{
	int block = index / PIE_BLOCK_SIZE;

	sw_w32_mask(0, BIT(block), RTL931X_PIE_BLK_LOOKUP_CTRL);
}

/* Fills the data in the intermediate representation in the pie_rule structure
 * into a data field for a given template field field_type
 * TODO: This function looks very similar to the function of the rtl9300, but
 * since it uses the physical template_field_id, which are different for each
 * SoC and there are other field types, it is actually not. If we would also use
 * an intermediate representation for a field type, we would could have one
 * pie_data_fill function for all SoCs, provided we have also for each SoC a
 * function to map between physical and intermediate field type
 */
static int rtl931x_pie_data_fill(enum rtl931x_template_field_id field_type, struct pie_rule *pr, u16 *data, u16 *data_m)
{
	*data = *data_m = 0;

	switch (field_type) {
	case RTL931X_TEMPLATE_FIELD_SPM0:
		*data = pr->spm;
		*data_m = pr->spm_m;
		break;
	case RTL931X_TEMPLATE_FIELD_SPM1:
		*data = pr->spm >> 16;
		*data_m = pr->spm_m >> 16;
		break;
	case RTL931X_TEMPLATE_FIELD_OTAG:
		*data = pr->otag;
		*data_m = pr->otag_m;
		break;
	case RTL931X_TEMPLATE_FIELD_SMAC0:
		*data = pr->smac[4];
		*data = (*data << 8) | pr->smac[5];
		*data_m = pr->smac_m[4];
		*data_m = (*data_m << 8) | pr->smac_m[5];
		break;
	case RTL931X_TEMPLATE_FIELD_SMAC1:
		*data = pr->smac[2];
		*data = (*data << 8) | pr->smac[3];
		*data_m = pr->smac_m[2];
		*data_m = (*data_m << 8) | pr->smac_m[3];
		break;
	case RTL931X_TEMPLATE_FIELD_SMAC2:
		*data = pr->smac[0];
		*data = (*data << 8) | pr->smac[1];
		*data_m = pr->smac_m[0];
		*data_m = (*data_m << 8) | pr->smac_m[1];
		break;
	case RTL931X_TEMPLATE_FIELD_DMAC0:
		*data = pr->dmac[4];
		*data = (*data << 8) | pr->dmac[5];
		*data_m = pr->dmac_m[4];
		*data_m = (*data_m << 8) | pr->dmac_m[5];
		break;
	case RTL931X_TEMPLATE_FIELD_DMAC1:
		*data = pr->dmac[2];
		*data = (*data << 8) | pr->dmac[3];
		*data_m = pr->dmac_m[2];
		*data_m = (*data_m << 8) | pr->dmac_m[3];
		break;
	case RTL931X_TEMPLATE_FIELD_DMAC2:
		*data = pr->dmac[0];
		*data = (*data << 8) | pr->dmac[1];
		*data_m = pr->dmac_m[0];
		*data_m = (*data_m << 8) | pr->dmac_m[1];
		break;
	case RTL931X_TEMPLATE_FIELD_ETHERTYPE:
		*data = pr->ethertype;
		*data_m = pr->ethertype_m;
		break;
	case RTL931X_TEMPLATE_FIELD_ITAG:
		*data = pr->itag;
		*data_m = pr->itag_m;
		break;
	case RTL931X_TEMPLATE_FIELD_SIP0:
		if (pr->is_ipv6) {
			*data = pr->sip6.s6_addr16[7];
			*data_m = pr->sip6_m.s6_addr16[7];
		} else {
			*data = pr->sip;
			*data_m = pr->sip_m;
		}
		break;
	case RTL931X_TEMPLATE_FIELD_SIP1:
		if (pr->is_ipv6) {
			*data = pr->sip6.s6_addr16[6];
			*data_m = pr->sip6_m.s6_addr16[6];
		} else {
			*data = pr->sip >> 16;
			*data_m = pr->sip_m >> 16;
		}
		break;
	case RTL931X_TEMPLATE_FIELD_SIP2:
	case RTL931X_TEMPLATE_FIELD_SIP3:
	case RTL931X_TEMPLATE_FIELD_SIP4:
	case RTL931X_TEMPLATE_FIELD_SIP5:
	case RTL931X_TEMPLATE_FIELD_SIP6:
	case RTL931X_TEMPLATE_FIELD_SIP7:
		*data = pr->sip6.s6_addr16[5 - (field_type - RTL931X_TEMPLATE_FIELD_SIP2)];
		*data_m = pr->sip6_m.s6_addr16[5 - (field_type - RTL931X_TEMPLATE_FIELD_SIP2)];
		break;
	case RTL931X_TEMPLATE_FIELD_DIP0:
		if (pr->is_ipv6) {
			*data = pr->dip6.s6_addr16[7];
			*data_m = pr->dip6_m.s6_addr16[7];
		} else {
			*data = pr->dip;
			*data_m = pr->dip_m;
		}
		break;
	case RTL931X_TEMPLATE_FIELD_DIP1:
		if (pr->is_ipv6) {
			*data = pr->dip6.s6_addr16[6];
			*data_m = pr->dip6_m.s6_addr16[6];
		} else {
			*data = pr->dip >> 16;
			*data_m = pr->dip_m >> 16;
		}
		break;
	case RTL931X_TEMPLATE_FIELD_DIP2:
	case RTL931X_TEMPLATE_FIELD_DIP3:
	case RTL931X_TEMPLATE_FIELD_DIP4:
	case RTL931X_TEMPLATE_FIELD_DIP5:
	case RTL931X_TEMPLATE_FIELD_DIP6:
	case RTL931X_TEMPLATE_FIELD_DIP7:
		*data = pr->dip6.s6_addr16[5 - (field_type - RTL931X_TEMPLATE_FIELD_DIP2)];
		*data_m = pr->dip6_m.s6_addr16[5 - (field_type - RTL931X_TEMPLATE_FIELD_DIP2)];
		break;
	case RTL931X_TEMPLATE_FIELD_IP_TOS_PROTO:
		*data = pr->tos_proto;
		*data_m = pr->tos_proto_m;
		break;
	case RTL931X_TEMPLATE_FIELD_L4_SPORT:
		*data = pr->sport;
		*data_m = pr->sport_m;
		break;
	case RTL931X_TEMPLATE_FIELD_L4_DPORT:
		*data = pr->dport;
		*data_m = pr->dport_m;
		break;
	case RTL931X_TEMPLATE_FIELD_DSAP_SSAP:
		*data = pr->dsap_ssap;
		*data_m = pr->dsap_ssap_m;
		break;
	case RTL931X_TEMPLATE_FIELD_TCP_INFO:
		*data = pr->tcp_info;
		*data_m = pr->tcp_info_m;
		break;
	case RTL931X_TEMPLATE_FIELD_RANGE_CHK:
		pr_debug("RTL931X_TEMPLATE_FIELD_RANGE_CHK: not configured\n");
		break;
	default:
		pr_debug("%s: unknown field %d\n", __func__, field_type);
		return -1;
	}

	return 0;
}

/* Reads the intermediate representation of the templated match-fields of the
 * PIE rule in the pie_rule structure and fills in the raw data fields in the
 * raw register space r[].
 * The register space configuration size is identical for the RTL8380/90 and RTL9300,
 * however the RTL931X has 2 more registers / fields and the physical field-ids are different
 * on all SoCs
 * On the RTL9300 the mask fields are not word-aligend!
 */
static void rtl931x_write_pie_templated(u32 r[], struct pie_rule *pr, enum rtl931x_template_field_id t[])
{
	for (int i = 0; i < N_FIXED_FIELDS; i++) {
		u16 data, data_m;

		rtl931x_pie_data_fill(t[i], pr, &data, &data_m);

		/* On the RTL9300, the mask fields are not word aligned! */
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

// Currently unused
// static void rtl931x_read_pie_fixed_fields(u32 r[], struct pie_rule *pr)
// {
// 	pr->mgnt_vlan = r[7] & BIT(31);
// 	if (pr->phase == PHASE_IACL)
// 		pr->dmac_hit_sw = r[7] & BIT(30);
// 	else  /* TODO: EACL/VACL phase handling */
// 		pr->content_too_deep = r[7] & BIT(30);
// 	pr->not_first_frag = r[7]  & BIT(29);
// 	pr->frame_type_l4 = (r[7] >> 26) & 7;
// 	pr->frame_type = (r[7] >> 24) & 3;
// 	pr->otag_fmt = (r[7] >> 23) & 1;
// 	pr->itag_fmt = (r[7] >> 22) & 1;
// 	pr->otag_exist = (r[7] >> 21) & 1;
// 	pr->itag_exist = (r[7] >> 20) & 1;
// 	pr->frame_type_l2 = (r[7] >> 18) & 3;
// 	pr->igr_normal_port = (r[7] >> 17) & 1;
// 	pr->tid = (r[7] >> 16) & 1;

// 	pr->mgnt_vlan_m = r[14] & BIT(15);
// 	if (pr->phase == PHASE_IACL)
// 		pr->dmac_hit_sw_m = r[14] & BIT(14);
// 	else
// 		pr->content_too_deep_m = r[14] & BIT(14);
// 	pr->not_first_frag_m = r[14] & BIT(13);
// 	pr->frame_type_l4_m = (r[14] >> 10) & 7;
// 	pr->frame_type_m = (r[14] >> 8) & 3;
// 	pr->otag_fmt_m = r[14] & BIT(7);
// 	pr->itag_fmt_m = r[14] & BIT(6);
// 	pr->otag_exist_m = r[14] & BIT(5);
// 	pr->itag_exist_m = r[14] & BIT (4);
// 	pr->frame_type_l2_m = (r[14] >> 2) & 3;
// 	pr->igr_normal_port_m = r[14] & BIT(1);
// 	pr->tid_m = r[14] & 1;

// 	pr->valid = r[15] & BIT(31);
// 	pr->cond_not = r[15] & BIT(30);
// 	pr->cond_and1 = r[15] & BIT(29);
// 	pr->cond_and2 = r[15] & BIT(28);
// }

static void rtl931x_write_pie_fixed_fields(u32 r[],  struct pie_rule *pr)
{
	r[7] |= pr->mgnt_vlan ? BIT(31) : 0;
	if (pr->phase == PHASE_IACL)
		r[7] |= pr->dmac_hit_sw ? BIT(30) : 0;
	else
		r[7] |= pr->content_too_deep ? BIT(30) : 0;
	r[7] |= pr->not_first_frag ? BIT(29) : 0;
	r[7] |= ((u32)(pr->frame_type_l4 & 0x7)) << 26;
	r[7] |= ((u32)(pr->frame_type & 0x3)) << 24;
	r[7] |= pr->otag_fmt ? BIT(23) : 0;
	r[7] |= pr->itag_fmt ? BIT(22) : 0;
	r[7] |= pr->otag_exist ? BIT(21) : 0;
	r[7] |= pr->itag_exist ? BIT(20) : 0;
	r[7] |= ((u32)(pr->frame_type_l2 & 0x3)) << 18;
	r[7] |= pr->igr_normal_port ? BIT(17) : 0;
	r[7] |= ((u32)(pr->tid & 0x1)) << 16;

	r[14] |= pr->mgnt_vlan_m ? BIT(15) : 0;
	if (pr->phase == PHASE_IACL)
		r[14] |= pr->dmac_hit_sw_m ? BIT(14) : 0;
	else
		r[14] |= pr->content_too_deep_m ? BIT(14) : 0;
	r[14] |= pr->not_first_frag_m ? BIT(13) : 0;
	r[14] |= ((u32)(pr->frame_type_l4_m & 0x7)) << 10;
	r[14] |= ((u32)(pr->frame_type_m & 0x3)) << 8;
	r[14] |= pr->otag_fmt_m ? BIT(7) : 0;
	r[14] |= pr->itag_fmt_m ? BIT(6) : 0;
	r[14] |= pr->otag_exist_m ? BIT(5) : 0;
	r[14] |= pr->itag_exist_m ? BIT(4) : 0;
	r[14] |= ((u32)(pr->frame_type_l2_m & 0x3)) << 2;
	r[14] |= pr->igr_normal_port_m ? BIT(1) : 0;
	r[14] |= (u32)(pr->tid_m & 0x1);

	r[15] |= pr->valid ? BIT(31) : 0;
	r[15] |= pr->cond_not ? BIT(30) : 0;
	r[15] |= pr->cond_and1 ? BIT(29) : 0;
	r[15] |= pr->cond_and2 ? BIT(28) : 0;
}

static void rtl931x_write_pie_action(u32 r[],  struct pie_rule *pr)
{
	/* Either drop or forward */
	if (pr->drop) {
		r[15] |= BIT(11) | BIT(12) | BIT(13); /* Do Green, Yellow and Red drops */
		/* Actually DROP, not PERMIT in Green / Yellow / Red */
		r[16] |= BIT(27) | BIT(28) | BIT(29);
	} else {
		r[15] |= pr->fwd_sel ? BIT(14) : 0;
		r[16] |= pr->fwd_act << 24;
		r[16] |= BIT(21); /* We overwrite any drop */
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
/*	r[15] |= pr->log_octets ? BIT(31) : 0; */
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

static void rtl931x_pie_rule_dump_raw(u32 r[])
{
	pr_debug("Raw IACL table entry:\n");
	pr_debug("r 0 - 7: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		 r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
	pr_debug("r 8 - 15: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		 r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15]);
	pr_debug("r 16 - 18: %08x %08x %08x\n", r[16], r[17], r[18]);
	pr_debug("Match  : %08x %08x %08x %08x %08x %08x\n", r[0], r[1], r[2], r[3], r[4], r[5]);
	pr_debug("Fixed  : %06x\n", r[6] >> 8);
	pr_debug("Match M: %08x %08x %08x %08x %08x %08x\n",
		 (r[6] << 24) | (r[7] >> 8), (r[7] << 24) | (r[8] >> 8), (r[8] << 24) | (r[9] >> 8),
		 (r[9] << 24) | (r[10] >> 8), (r[10] << 24) | (r[11] >> 8),
		 (r[11] << 24) | (r[12] >> 8));
	pr_debug("R[13]:   %08x\n", r[13]);
	pr_debug("Fixed M: %06x\n", ((r[12] << 16) | (r[13] >> 16)) & 0xffffff);
	pr_debug("Valid / not / and1 / and2 : %1x\n", (r[13] >> 12) & 0xf);
	pr_debug("r 13-16: %08x %08x %08x %08x\n", r[13], r[14], r[15], r[16]);
}

int rtl931x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr)
{
	int tbl = otto_table_acquire(RTL9310_TBL_IACL);
	u32 r[22];
	int block = idx / PIE_BLOCK_SIZE;
	u32 t_select = sw_r32(RTL931X_PIE_BLK_TMPLTE_CTRL(block));

	pr_debug("%s: %d, t_select: %08x\n", __func__, idx, t_select);

	for (int i = 0; i < 22; i++)
		r[i] = 0;

	if (!pr->valid) {
		__otto_table_write(tbl, idx, &r);
		otto_table_release(tbl);
		return 0;
	}
	rtl931x_write_pie_fixed_fields(r, pr);

	pr_debug("%s: template %d\n", __func__, (t_select >> (pr->tid * 4)) & 0xf);
	rtl931x_write_pie_templated(r, pr, rtl931x_fixed_templates[(t_select >> (pr->tid * 4)) & 0xf]);

	rtl931x_write_pie_action(r, pr);

	rtl931x_pie_rule_dump_raw(r);

	__otto_table_write(tbl, idx, &r);
	otto_table_release(tbl);

	return 0;
}

static bool rtl931x_pie_templ_has(int t, enum rtl931x_template_field_id field_type)
{
	for (int i = 0; i < N_FIXED_FIELDS_RTL931X; i++) {
		enum rtl931x_template_field_id ft = rtl931x_fixed_templates[t][i];

		if (field_type == ft)
			return true;
	}

	return false;
}

/* Verify that the rule pr is compatible with a given template t in block
 * Note that this function is SoC specific since the values of e.g. RTL931X_TEMPLATE_FIELD_SIP0
 * depend on the SoC
 */
static int rtl931x_pie_verify_template(struct rtl838x_switch_priv *priv,
				       struct pie_rule *pr, int t, int block)
{
	int i;

	if (!pr->is_ipv6 && pr->sip_m && !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_SIP0))
		return -1;

	if (!pr->is_ipv6 && pr->dip_m && !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_DIP0))
		return -1;

	if (pr->is_ipv6) {
		if ((pr->sip6_m.s6_addr32[0] ||
		     pr->sip6_m.s6_addr32[1] ||
		     pr->sip6_m.s6_addr32[2] ||
		     pr->sip6_m.s6_addr32[3]) &&
		    !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_SIP2))
			return -1;
		if ((pr->dip6_m.s6_addr32[0] ||
		     pr->dip6_m.s6_addr32[1] ||
		     pr->dip6_m.s6_addr32[2] ||
		     pr->dip6_m.s6_addr32[3]) &&
		    !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_DIP2))
			return -1;
	}

	if (ether_addr_to_u64(pr->smac_m) && !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_SMAC0))
		return -1;

	if (ether_addr_to_u64(pr->dmac_m) && !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_DMAC0))
		return -1;

	if (pr->itag_m && !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_VLAN))
		return -1;

	if (pr->sport_m && !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_L4_SPORT))
		return -1;

	if (pr->dport_m && !rtl931x_pie_templ_has(t, RTL931X_TEMPLATE_FIELD_L4_DPORT))
		return -1;

	/* TODO: Check more */

	i = find_first_zero_bit(&priv->pie_use_bm[block * 4], PIE_BLOCK_SIZE);

	if (i >= PIE_BLOCK_SIZE)
		return -1;

	return i + PIE_BLOCK_SIZE * block;
}

int rtl931x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx, block, j;
	int min_block = 0;
	int max_block = priv->r->n_pie_blocks / 2;

	if (pr->is_egress) {
		min_block = max_block;
		max_block = priv->r->n_pie_blocks;
	}
	pr_debug("In %s\n", __func__);

	mutex_lock(&priv->pie_mutex);

	for (block = min_block; block < max_block; block++) {
		for (j = 0; j < 2; j++) {
			int t = (sw_r32(RTL931X_PIE_BLK_TMPLTE_CTRL(block)) >> (j * 4)) & 0xf;

			pr_debug("Testing block %d, template %d, template id %d\n", block, j, t);
			pr_debug("%s: %08x\n",
				 __func__, sw_r32(RTL931X_PIE_BLK_TMPLTE_CTRL(block)));
			idx = rtl931x_pie_verify_template(priv, pr, t, block);
			if (idx >= 0)
				break;
		}
		if (j < 2)
			break;
	}

	if (block >= max_block) {
		mutex_unlock(&priv->pie_mutex);
		return -EOPNOTSUPP;
	}

	pr_debug("Using block: %d, index %d, template-id %d\n", block, idx, j);
	set_bit(idx, priv->pie_use_bm);

	pr->valid = true;
	pr->tid = j;  /* Mapped to template number */
	pr->tid_m = 0x1;
	pr->id = idx;

	rtl931x_pie_lookup_enable(priv, idx);
	rtl931x_pie_rule_write(priv, idx, pr);

	mutex_unlock(&priv->pie_mutex);

	return 0;
}

/* Delete a range of Packet Inspection Engine rules */
static int rtl931x_pie_rule_del(struct rtl838x_switch_priv *priv, int index_from, int index_to)
{
	u32 v = (index_from << 1) | (index_to << 13) | BIT(0);

	pr_debug("%s: from %d to %d\n", __func__, index_from, index_to);
	mutex_lock(&priv->reg_mutex);

	/* Write from-to and execute bit into control register */
	sw_w32(v, RTL931X_PIE_CLR_CTRL);

	/* Wait until command has completed */
	do {
	} while (sw_r32(RTL931X_PIE_CLR_CTRL) & BIT(0));

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

void rtl931x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx = pr->id;

	rtl931x_pie_rule_del(priv, idx, idx);
	clear_bit(idx, priv->pie_use_bm);
}

void rtl931x_pie_init(struct rtl838x_switch_priv *priv)
{
	u32 template_selectors;

	mutex_init(&priv->pie_mutex);

	pr_debug("%s\n", __func__);
	/* Enable ACL lookup on all ports, including CPU_PORT */
	for (int i = 0; i <= priv->r->cpu_port; i++)
		sw_w32(1, RTL931X_ACL_PORT_LOOKUP_CTRL(i));

	/* Include IPG in metering */
	sw_w32_mask(0, 1, RTL931X_METER_GLB_CTRL);

	/* Delete all present rules, block size is 128 on all SoC families */
	rtl931x_pie_rule_del(priv, 0, priv->r->n_pie_blocks * 128 - 1);

	/* Assign first half blocks 0-7 to VACL phase, second half to IACL */
	/* 3 bits are used for each block, values for PIE blocks are */
	/* 6: Disabled, 0: VACL, 1: IACL, 2: EACL */
	/* And for OpenFlow Flow blocks: 3: Ingress Flow table 0, */
	/* 4: Ingress Flow Table 3, 5: Egress flow table 0 */
	for (int i = 0; i < priv->r->n_pie_blocks; i++) {
		int pos = (i % 10) * 3;
		u32 r = RTL931X_PIE_BLK_PHASE_CTRL + 4 * (i / 10);

		if (i < priv->r->n_pie_blocks / 2)
			sw_w32_mask(0x7 << pos, 0, r);
		else
			sw_w32_mask(0x7 << pos, 1 << pos, r);
	}

	/* Enable predefined templates 0, 1 for first quarter of all blocks */
	template_selectors = 0 | (1 << 4);
	for (int i = 0; i < priv->r->n_pie_blocks / 4; i++)
		sw_w32(template_selectors, RTL931X_PIE_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 2, 3 for second quarter of all blocks */
	template_selectors = 2 | (3 << 4);
	for (int i = priv->r->n_pie_blocks / 4; i < priv->r->n_pie_blocks / 2; i++)
		sw_w32(template_selectors, RTL931X_PIE_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 0, 1 for third quater of all blocks */
	template_selectors = 0 | (1 << 4);
	for (int i = priv->r->n_pie_blocks / 2; i < priv->r->n_pie_blocks * 3 / 4; i++)
		sw_w32(template_selectors, RTL931X_PIE_BLK_TMPLTE_CTRL(i));

	/* Enable predefined templates 2, 3 for fourth quater of all blocks */
	template_selectors = 2 | (3 << 4);
	for (int i = priv->r->n_pie_blocks * 3 / 4; i < priv->r->n_pie_blocks; i++)
		sw_w32(template_selectors, RTL931X_PIE_BLK_TMPLTE_CTRL(i));
}
