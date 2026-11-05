// SPDX-License-Identifier: GPL-2.0
/*
 * OMCI MIB -> kernel driver.
 *
 * Covered: T-CONT Alloc-ID (262), GEM port network CTP (268), GEM and
 * multicast GEM interworking TP (266/281: default upstream GEM, multicast
 * GEM), 802.1p mapper (130: default upstream GEM; the P-bit/VID -> GEM
 * rules are computed with the VLANs in eth.c), ANI-G SD/SF
 * (263), ONU-G administrative state (256), OLT-G time of day (131),
 * upstream QoS: T-CONT policy (262) with the weights of its priority
 * queues (277) -> QOS_SCHED.  No rate is taken from the traffic descriptor
 * (280) a GEM port (268) points to: the SDK applies a traffic descriptor
 * only through the inbound / outbound pointers of a bridge port (ME 47,
 * setInBoundTD() / setOutBoundTD()) and leaves the one of a GEM port alone
 * (the PON MAC calls of getTrafficDescriptorValue() / setTrafficDescriptor-
 * Value() are commented out), so the T-CONT is not shaped from it either;
 * the bridge port meters are eth.c's policers.
 *
 * The Ethernet side (bridges, VLANs, L3 hosts, multicast, policing, the
 * PPTP Ethernet UNI port settings, the upstream classification) is eth.c;
 * bind_check() rejects the values a Set or Create must not store.
 */
#include "gpond.h"
#include "omci.h"
#include "omci_pm.h"
#include "omci_mcast.h"
#include "eth.h"

#include <stdio.h>

#define MAPPER_PBITS		8
#define IW_OPT_DOWN_BROADCAST	6

static uint16_t us_gem = 0xffff;

/* ---- Set value checks ---- */

/* code point lists, end with -1 */
static bool in_list(uint8_t v, const int *list)
{
	for (; *list >= 0; list++)
		if (v == *list)
			return true;

	return false;
}

/*
 * PPTP Ethernet UNI (11), the SDK checks (omci_me_ethernet_services_func.c
 * and the code point columns of omci_me_ethernet_services_nodetable.c):
 *   1 expected type     only 0 (setPPTPEthernetUNIExpectedType())
 *   3 auto detection    AutoDetectionCfg[] (nodetable.c:40)
 *   4 loopback          EtherLoopbackCfg[] 0 / 3 (nodetable.c:41,
 *                       setPPTPEthernetUNIEthernetLoopbackCfg())
 *   5 admin state       0 unlock / 1 lock, what the alarm code treats as
 *                       locked (the SDK keeps any value)
 *   9 DTE or DCE ind    DTEorDCEind[] (nodetable.c:43)
 *  11 bridged or IP ind bridgedOrIPind[] (nodetable.c:44)
 *  14 PPPoE filter      0 or 1 (setPPTPEthernetUNIPPPoEFilter() took only
 *                       0; the hardware drops 0x8863/0x8864 when it is 1)
 */
static bool pptp_check(uint8_t idx, const uint8_t *v)
{
	static const int auto_detect[] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x10, 0x11, 0x12,
		0x13, 0x14, 0x20, 0x30, 0x40, -1
	};
	static const int loopback[] = { 0, 3, -1 };
	static const int ind[] = { 0, 1, 2, -1 };

	switch (idx) {
	case 1:
		return v[0] == 0;
	case 5:
	case 14:
		return v[0] <= 1;
	case 3:
		return in_list(v[0], auto_detect);
	case 4:
		return in_list(v[0], loopback);
	case 9:
	case 11:
		return in_list(v[0], ind);
	default:
		return true;
	}
}

/*
 * Extended VLAN tagging operation configuration data (171), SDK
 * omci_me_vlan_func.c:
 *   1 association type  0, 2, 3, 10 (setExtVlanTagOpAssociaTypeValue(),
 *                       5148-5169; 3 only with TCSUPPORT_PON_IP_HOST,
 *                       implemented here with the IP host)
 *   3/4 input/output TPID  the SDK takes any value (5178-5266); only
 *                       0x8100 and 0x88a8 exist as Linux VLAN protocols
 *   5 downstream mode   0 or 1 (setExtVlanTagOpDownModeValue(), 5267-5290)
 *   6 table row         the SDK decode (setExtVlanTagRuleByPkt(),
 *                       973-1428), the 32 rows of a port (omci_me_vlan.h:318)
 *                       and what Linux can express (eth_vlan.c)
 */
static bool ext_vlan_check(struct omci_inst *in, uint8_t idx, const uint8_t *v)
{
	const char *why;
	uint16_t tpid;

	switch (idx) {
	case 1:
		return v[0] == 0 || v[0] == 2 || v[0] == 3 || v[0] == 10;
	case 3:
	case 4:
		tpid = v[0] << 8 | v[1];
		return !tpid || tpid == 0x8100 || tpid == 0x88a8;
	case 5:
		return v[0] <= 1;
	case 6:
		why = xv_row_check(in, v);
		if (why)
			INFO("OMCI: ME 171 0x%04x row refused: %s", in->id, why);
		return !why;
	default:
		return true;
	}
}

/*
 * VLAN tagging filter data (84): forward operation, SDK
 * setvlanFilterForwardOPValue() (4515-4555; omci_me_vlan_func.c #defines
 * TCSUPPORT_VLAN_FILTER_EX at 3521, so every code of filterFwdOpTbl,
 * 0..0x21, is accepted).  The codes eth_vlan.c implements (VID and P-bit
 * investigations, tc flower vlan_prio, see xv_filter_op_ok()) are taken;
 * refused: 0x01, action (g), the TCI investigations (they include the DEI).
 * Number of entries: at most 12 (setvlanFilterValidNumberEntryValue(),
 * 4557-4575, MAX_VLAN_FILTER_TCI_LIST_NUM, omci_me_vlan.h:223).
 */
static bool vlan_filter_check(uint8_t idx, const uint8_t *v, bool create)
{
	switch (idx) {
	case 2:
		return xv_filter_op_ok(v[0]);
	case 3:
		return v[0] <= 12;
	default:
		return true;
	}
}

/*
 * Dot1ag maintenance domain (299) and association (300): stored and
 * reported only, as in the SDK (omci_me_layer2_data_services_func.c: the
 * "MD info API" calls are comments, there is no MEP), with its value checks:
 *   MD  1 level 0..7 (setDot1agMaintenanceDomainMDLevel(), :184), 2 name
 *       format 1 2 3 4 32 (:209), 5 MHF creation 1 2 3 (:277), 6 sender ID
 *       permission 1 2 3 4 (:303)
 *   MA  2 name format 1..4 (setDot1agMaintenanceDomainMANameFormat()), 7 MHF
 *       creation 1..4, 8 sender ID permission 1..5
 */
static bool dot1ag_check(uint16_t cls, uint8_t idx, const uint8_t *v)
{
	static const int md_name[] = { 1, 2, 3, 4, 32, -1 };
	static const int to3[] = { 1, 2, 3, -1 };
	static const int to4[] = { 1, 2, 3, 4, -1 };
	static const int to5[] = { 1, 2, 3, 4, 5, -1 };

	if (cls == 299) {
		switch (idx) {
		case 1:
			return v[0] <= 7;
		case 2:
			return in_list(v[0], md_name);
		case 5:
			return in_list(v[0], to3);
		case 6:
			return in_list(v[0], to4);
		}
		return true;
	}
	switch (idx) {
	case 2:
	case 7:
		return in_list(v[0], to4);
	case 8:
		return in_list(v[0], to5);
	}

	return true;
}

/* G.988 9.3.1 gives the bridge timers in 256ths of a second; the SDK also
 * takes plain seconds (setMBSPMaxAgeValue() and the like shift the value
 * only when its high byte is set) */
static bool stp_time_ok(uint16_t v, uint16_t lo, uint16_t hi)
{
	if (v > 0xff)
		v >>= 8;

	return v >= lo && v <= hi;
}

/* may an OLT Set / Create store @v (attribute size bytes) in attribute @idx */
bool bind_check(struct omci_inst *in, uint8_t idx, const uint8_t *v,
		bool create)
{
	if (omci_pm_is_pm(in->def))
		return omci_pm_check(in, idx, v);
	if (mcast_is_me(in->def->class_id))
		return mcast_check(in, idx, v, create);

	switch (in->def->class_id) {
	case 6:
		if (idx == 6)
			return v[0] <= 1;
		if (idx == 8)
			return v[0] <= 2;
		if (idx == 14)
			return !(v[0] | v[1] | v[2] | v[3]);
		return true;
	case 256:
		if (idx == 6)
			return !v[0];
		return true;
	case 11:
		return pptp_check(idx, v);
	case 45:
		switch (idx) {
		case 1:		/* spanning tree ind */
		case 2:		/* learning ind */
		case 3:		/* port bridging ind */
		case 8:		/* unknown MAC addr discard */
			return v[0] <= 1;
		case 5:		/* max age, 256ths of a second */
			return stp_time_ok(v[0] << 8 | v[1], 6, 40);
		case 6:		/* hello time, 256ths of a second */
			return stp_time_ok(v[0] << 8 | v[1], 1, 10);
		case 7:		/* forward delay, 256ths of a second */
			return stp_time_ok(v[0] << 8 | v[1], 4, 30);
		case 10: {	/* dynamic filter age time, seconds */
			uint32_t dw = (uint32_t)v[0] << 24 | v[1] << 16 |
				      v[2] << 8 | v[3];

			/* 0: the OLT leaves it to the ONU (eth.c keeps the
			 * bridge default), and refusing it fails the whole
			 * Create of the profile */
			return !dw || (dw >= 10 && dw <= 1000000);
		}
		default:
			return true;
		}
	case 78:
		/*
		 * G.988 9.3.12: upstream mode 0..2, downstream 0 transparent /
		 * 1 strip a tag, association 0 PPTP Ethernet UNI (implicit),
		 * 1 IP host, 3 MAC bridge port, 10 PPTP Ethernet UNI
		 * (explicit pointer), 11 VEIP.
		 */
		if (idx == 1)
			return v[0] <= 2;
		if (idx == 3)
			return v[0] <= 1;
		if (idx == 4)
			return v[0] == VT_ASSOC_UNI_IMPLICIT ||
			       v[0] == VT_ASSOC_IP_HOST ||
			       v[0] == VT_ASSOC_BRIDGE_PORT ||
			       v[0] == VT_ASSOC_UNI || v[0] == VT_ASSOC_VEIP;
		return true;
	case 281: {
		/*
		 * "Not used 1" (4) and "Not used 2" (8) are reserved in G.988:
		 * the SDK stores only zero (setMulticastGemIWTPNotUsed1/2()).
		 */
		const struct omci_attr_def *a = omci_attr(in->def, idx);
		unsigned int n;

		if (idx != 4 && idx != 8)
			return true;
		for (n = 0; a && n < a->size; n++)
			if (v[n])
				return false;
		return true;
	}
	case 299:
	case 300:
		return dot1ag_check(in->def->class_id, idx, v);
	case 268:
		/*
		 * Attribute 1 "Port-ID": 0..0xFFF (G.984.3 12-bit GEM port-ID).
		 * Attribute 3 "Direction": 1 UNI-to-ANI, 2 ANI-to-UNI,
		 * 3 bidirectional (G.988 9.2.3).
		 */
		if (idx == 1)
			return (v[0] << 8 | v[1]) < 0x1000;
		if (idx == 3)
			return v[0] >= GEM_DIR_UNI_TO_ANI &&
			       v[0] <= GEM_DIR_BIDIR;
		return true;
	case 262:
		/*
		 * Attribute 1 "Alloc-ID": 0..0xFFF (G.984.3).  The value goes
		 * to kx_tcont_add() and kx_gem_add(), and tcont_alloc_changed()
		 * calls kx_tcont_del() on the old one, so an out-of-range value
		 * would act on another ONU's allocation.  The OMCC Alloc-ID
		 * (the ONU-ID) is applied by omci_state(), not by a Set.
		 */
		return idx != 1 || (v[0] << 8 | v[1]) < 0x1000;
	case 157:
		return idx != 1 || v[0] <= 15;
	case 158:
		if (idx == 2) {
			char cmd[26];
			unsigned int n;

			for (n = 0; n < 25 && v[n]; n++)
				cmd[n] = (v[n] >= 0x20 && v[n] < 0x7f) ?
					 (char)v[n] : '?';
			cmd[n] = 0;
			WARN("OMCI: ONU remote debug command rejected: '%s'",
			     cmd);
		}
		return false;
	case CLASS_VLAN_FILTER:
		return vlan_filter_check(idx, v, create);
	case CLASS_EXT_VLAN:
		return ext_vlan_check(in, idx, v);
	case CLASS_IP_HOST:
	case CLASS_VEIP:
	case CLASS_TR069:
		return eth_l3_check(in, idx, v);
	default:
		return true;
	}
}

static uint16_t ctp_alloc(struct omci_inst *ctp)
{
	struct omci_inst *t;

	/* a downstream-only GEM port (direction 2, ANI-to-UNI) has no T-CONT */
	if (mib_get_uint(ctp, 3) == GEM_DIR_ANI_TO_UNI)
		return 0x00ff;
	t = mib_find(262, mib_get_uint(ctp, 2));

	return t ? mib_get_uint(t, 1) : 0x00ff;
}

static bool ctp_is_mcast(uint16_t ctp_id, const struct omci_inst *skip)
{
	struct omci_inst *in;

	list_for_each_entry(in, &omci_mib, list)
		if (in != skip && in->def->class_id == 281 &&
		    mib_get_uint(in, 1) == ctp_id)
			return true;

	return false;
}

/* (re)announce one GEM port with the Alloc-ID of its T-CONT */
static int ctp_apply(struct omci_inst *ctp, bool readd)
{
	uint16_t gem = mib_get_uint(ctp, 1);
	bool mcast = ctp_is_mcast(ctp->id, NULL);
	uint16_t alloc = mcast ? 0 : ctp_alloc(ctp);

	if (readd)
		kx_gem_del(gem);

	return kx_gem_add(gem, alloc, mcast);
}

static int iwtp_gem_ex(uint16_t iwtp_id, const struct omci_inst *skip)
{
	struct omci_inst *iw = mib_find(266, iwtp_id);
	struct omci_inst *ctp;

	if (!iw || iw == skip || mib_get_uint(iw, 2) == IW_OPT_DOWN_BROADCAST)
		return -1;
	ctp = mib_find(268, mib_get_uint(iw, 1));
	if (!ctp || ctp == skip ||
	    mib_get_uint(ctp, 3) == GEM_DIR_ANI_TO_UNI)
		return -1;

	return (int)mib_get_uint(ctp, 1);
}

/* upstream-capable GEM port id behind an interworking TP (266), -1: none */
int bind_iwtp_gem(uint16_t iwtp_id)
{
	return iwtp_gem_ex(iwtp_id, NULL);
}

/*
 * The default upstream GEM of the MAC: the first upstream-capable GEM port a
 * mapper (ME 130) or a GEM interworking TP (ME 266) names.  The P-bit/VID ->
 * GEM rules follow the VLANs the ANI side sends and are programmed by eth.c.
 */
static void us_gem_refresh(const struct omci_inst *skip)
{
	struct omci_inst *in;
	int p, first = -1;

	if (us_gem != 0xffff) {
		list_for_each_entry(in, &omci_mib, list) {
			if (in != skip && in->def->class_id == 268 &&
			    mib_get_uint(in, 1) == us_gem &&
			    mib_get_uint(in, 3) != GEM_DIR_ANI_TO_UNI)
				return;
		}
	}

	list_for_each_entry(in, &omci_mib, list) {
		if (in == skip || in->def->class_id != 130)
			continue;
		for (p = 0; p < MAPPER_PBITS && first < 0; p++) {
			uint16_t ptr = mib_get_uint(in, 2 + p);

			if (ptr != PTR_NULL)
				first = iwtp_gem_ex(ptr, skip);
		}
		if (first >= 0)
			break;
	}
	list_for_each_entry(in, &omci_mib, list) {
		if (first >= 0)
			break;
		if (in == skip || in->def->class_id != 266)
			continue;
		first = iwtp_gem_ex(in->id, skip);
	}

	if (first >= 0) {
		if (!kx_us_gem(first))
			us_gem = first;
	} else if (us_gem != 0xffff) {
		us_gem = 0xffff;
		kx_us_gem(0xffff);
	}
}

/*
 * T-CONT scheduling.  Queue q of a T-CONT has priority 7 - q
 * (omciInternalCreateForPriorityQueue(): related port = T-CONT << 16 |
 * (7 - q)); G.988 policy 1 strict priority, 2 WRR.
 */

/* ME 278 policy if the OLT wrote it, else the T-CONT one (SDK setQOSPolicy) */
static uint8_t tcont_policy(struct omci_inst *t)
{
	struct omci_inst *ts;

	list_for_each_entry(ts, &omci_mib, list)
		if (ts->def->class_id == 278 && mib_get_uint(ts, 1) == t->id &&
		    (ts->olt_mask & omci_mask(3)))
			return mib_get_uint(ts, 3);

	return mib_get_uint(t, 3);
}

static void qos_tcont(struct omci_inst *t)
{
	uint16_t alloc = mib_get_uint(t, 1), w[8] = { 0 };
	uint32_t rate_bps = 0, bucket = 0;
	struct omci_inst *pq, *ctp;
	struct nl_msg *m;
	int i;

	if (ALLOC_UNASSIGNED(alloc) || alloc == omci_onu_id)
		return;

	list_for_each_entry(pq, &omci_mib, list) {
		uint32_t rp;
		uint8_t prio;

		if (pq->def->class_id != 277)
			continue;
		rp = mib_get_uint(pq, 6);
		prio = rp & 7;
		if ((rp >> 16) != t->id)
			continue;
		w[7 - prio] = mib_get_uint(pq, 8);
	}

	m = nl_msg(GPON_CMD_QOS_SCHED, false);
	nl_put_u16(m, GPON_ATTR_ALLOC_ID, alloc);
	nl_put_u8(m, GPON_ATTR_SCHED_MODE,
		  tcont_policy(t) == 2 ? 0 /* WRR8 */ : 1 /* SP */);
	for (i = 0; i < 8; i++)		/* host order, as the kernel takes it */
		w[i] = w[i] ? w[i] : 1;
	nl_put(m, GPON_ATTR_WEIGHTS, w, sizeof(w));
	if (nl_request(m, NULL, NULL))
		WARN("OMCI: T-CONT 0x%04x scheduling rejected", t->id);

	list_for_each_entry(ctp, &omci_mib, list) {
		struct omci_inst *td;
		uint16_t td_ptr;
		uint32_t r, b;

		if (ctp->def->class_id != 268 ||
		    mib_get_uint(ctp, 2) != t->id ||
		    mib_get_uint(ctp, 3) == GEM_DIR_ANI_TO_UNI)
			continue;
		td_ptr = mib_get_uint(ctp, 5);
		if (!td_ptr || td_ptr == 0xffff)
			continue;
		td = mib_find(280, td_ptr);
		if (!td)
			continue;
		r = mib_get_uint(td, 2);		/* PIR, bytes/s */
		b = r ? mib_get_uint(td, 4) : 0;	/* PBS */
		if (!r) {
			r = mib_get_uint(td, 1);	/* CIR */
			b = mib_get_uint(td, 3);	/* CBS */
		} else if (!b) {
			b = mib_get_uint(td, 3);
		}
		if (r > rate_bps) {
			rate_bps = r;
			bucket = b;
		}
	}
	kx_qos_rate(alloc, (uint32_t)(((uint64_t)rate_bps * 8 + 999) / 1000),
		    bucket);
}

static void qos_all(void)
{
	struct omci_inst *t;

	list_for_each_entry(t, &omci_mib, list)
		if (t->def->class_id == 262)
			qos_tcont(t);
}

/* ---- ME 277 congestion control (an7552-gpon sysfs "gpon" group) ---- */

#define PQ_UPSTREAM_BASE	0x8000	/* omci_mib.c MIN_TCONT_MEID */
#define WRED_THR_MAX		56	/* AIROHA_XPON_WRED_THR_MAX */
#define WRED_PROB_NUM		4	/* AIROHA_XPON_WRED_PROB_NUM */

/* The knobs are flat files in /sys/class/net/pon0/gpon */
static void qos_set(const char *name, unsigned int v)
{
	char path[96];
	FILE *f;

	snprintf(path, sizeof(path), "/sys/class/net/%s/gpon/%s",
		 GPON_NETDEV, name);
	f = fopen(path, "w");
	if (!f)
		return;
	fprintf(f, "%u\n", v);
	fclose(f);
}

static unsigned int thr_clamp(unsigned int v)
{
	return v > WRED_THR_MAX ? WRED_THR_MAX : v;
}

/*
 * The kernel has five WRED thresholds per packet class and one probability
 * for each of the four regions between them; ME 277 gives only the min and
 * max of the green (normal) and yellow (DEI) range, so spread them evenly
 * over the five points.  The values are the queue occupancy the SDK's
 * congest thresholds use (small units, not bytes).
 */
static void wred_thr(const char *prefix, unsigned int lo, unsigned int hi)
{
	unsigned int i;

	lo = thr_clamp(lo);
	hi = thr_clamp(hi);
	for (i = 0; i < 5; i++) {
		char n[16];

		snprintf(n, sizeof(n), "%s%u", prefix, i);
		qos_set(n, lo + (hi - lo) * i / 4);
	}
}

/* G.988 9.4.2: max_p is (value + 1) / 256, the sysfs attribute takes a percentage */
static unsigned int wred_pct(unsigned int v)
{
	unsigned int p = (v + 1) * 100 / 256;

	return p > 100 ? 100 : p;
}

static void qos_wred(struct omci_inst *pq)
{
	uint8_t *t = mib_val(pq, 13);
	const uint8_t *p = mib_val(pq, 14);
	const uint8_t *m = mib_val(pq, 16);
	unsigned int green_p = 0, yellow_p = 0, mark = 0, i;

	/* the downstream queues are switch queues, not the QDMA's */
	if (pq->id < PQ_UPSTREAM_BASE)
		return;
	/*
	 * The QDMA has one WRED / DEI configuration for all its queues, so a
	 * queue the OLT never configured must not reset what another one set.
	 */
	if (!t && !p && !m)
		return;

	if (p && pq->len[14] >= 2) {
		green_p = wred_pct(p[0]);
		yellow_p = wred_pct(p[1]);
	} else if (t && pq->len[13] >= 8) {
		/* G.988: max_p defaults to 255, i.e. drop everything at max */
		green_p = yellow_p = 100;
	}
	if (m && pq->len[16])
		mark = m[0];

	/* G.988 9.4.2: green min, green max, yellow min, yellow max */
	if (t && pq->len[13] >= 8) {
		wred_thr("wred_thr_n", t[0] << 8 | t[1], t[2] << 8 | t[3]);
		wred_thr("wred_thr_d", t[4] << 8 | t[5], t[6] << 8 | t[7]);
	}
	if (t || p) {
		for (i = 0; i < WRED_PROB_NUM; i++) {
			char n[16];

			snprintf(n, sizeof(n), "wred_prob_n%u", i);
			qos_set(n, green_p);
			snprintf(n, sizeof(n), "wred_prob_d%u", i);
			qos_set(n, yellow_p);
		}
		qos_set("wred", green_p || yellow_p);
	}
	/* the kernel knows DEI marking only (attribute value 2) */
	qos_set("dei_mark", mark == 2);
	qos_set("dei_drop", yellow_p || mark == 2 ||
		(t && pq->len[13] >= 8));
}

static void tcont_alloc_changed(struct omci_inst *t, uint16_t old_alloc)
{
	uint16_t alloc = mib_get_uint(t, 1);
	struct omci_inst *ctp;

	/* the ONU-ID alloc belongs to PLOAM in the kernel */
	if (!ALLOC_UNASSIGNED(old_alloc) && old_alloc != omci_onu_id)
		kx_tcont_del(old_alloc);
	if (!ALLOC_UNASSIGNED(alloc) && alloc != omci_onu_id)
		kx_tcont_add(alloc);

	/* GEM ports pointing at this T-CONT follow the new Alloc-ID */
	list_for_each_entry(ctp, &omci_mib, list)
		if (ctp->def->class_id == 268 &&
		    mib_get_uint(ctp, 2) == t->id)
			ctp_apply(ctp, true);
}

/* what qos_tcont() reads: the T-CONT policy, queues and traffic descriptors */
static bool qos_class(uint16_t c)
{
	return c == 262 || c == 268 || c == 277 || c == 278 || c == 280;
}

/* what a Create does in the kernel, without the QoS refresh of the whole MIB */
static int bind_create_kernel(struct omci_inst *in)
{
	if (mcast_is_me(in->def->class_id))
		mcast_create(in);

	switch (in->def->class_id) {
	case 45:
		if (!mib_find(46, in->id))
			mib_create(46, in->id, false);
		break;
	case 47:
		if (!mib_find(48, in->id))
			mib_create(48, in->id, false);
		if (!mib_find(50, in->id))
			mib_create(50, in->id, false);
		break;
	case 268:
		if (ctp_apply(in, false))
			return -1;
		us_gem_refresh(NULL);
		return 0;
	case 266:
	case 130:
		us_gem_refresh(NULL);
		break;
	case 281: {
		struct omci_inst *ctp = mib_find(268, mib_get_uint(in, 1));

		if (ctp) {
			kx_gem_del(mib_get_uint(ctp, 1));
			kx_gem_add(mib_get_uint(ctp, 1), 0, true);
		}
		break;
	}
	case CLASS_EXT_VLAN:
		/* G.988 / SDK getExtVlanTagOpMaxNumValue(), 5172-5176 */
		mib_set_uint(in, 2, XV_MAX_ROWS);
		break;
	case 277:
		qos_wred(in);
		break;
	default:
		break;
	}

	return 0;
}

int bind_create(struct omci_inst *in)
{
	if (qos_class(in->def->class_id))
		qos_all();

	return bind_create_kernel(in);
}

static int bind_set_one(struct omci_inst *in, uint16_t mask,
			const uint8_t *old[17])
{
	switch (in->def->class_id) {
	case 262:
		if ((mask & omci_mask(1)) && old[1])
			tcont_alloc_changed(in, old[1][0] << 8 | old[1][1]);
		break;
	case 268:
		if (mask & (omci_mask(1) | omci_mask(2) | omci_mask(3))) {
			kx_gem_del(old[1] ? (uint16_t)(old[1][0] << 8 | old[1][1]) :
				   mib_get_uint(in, 1));
			ctp_apply(in, false);
			us_gem_refresh(NULL);
		}
		break;
	case 266:
	case 130:
		us_gem_refresh(NULL);
		break;
	case 277:
		if (mask & (omci_mask(13) | omci_mask(14) | omci_mask(16)))
			qos_wred(in);
		break;
	case 263: {
		struct nl_msg *m;

		/* only SF/SD go to the kernel (ARC, optical thresholds: the
		 * alarm monitor reads them from the MIB) */
		if (!(mask & (omci_mask(6) | omci_mask(7))))
			break;
		m = nl_msg(GPON_CMD_ONU_CTRL, false);
		if (mask & omci_mask(6))
			nl_put_u8(m, GPON_ATTR_SF, mib_get_uint(in, 6));
		if (mask & omci_mask(7))
			nl_put_u8(m, GPON_ATTR_SD, mib_get_uint(in, 7));
		if (nl_request(m, NULL, NULL))
			return -1;
		break;
	}
	case 256:
		if (mask & omci_mask(7))
			return kx_onu_flag(GPON_ATTR_ADMIN_DOWN,
					   mib_get_uint(in, 7) == 1);
		break;
	case CLASS_VEIP:
		if (mask & omci_mask(1))
			eth_l3_admin(in);
		break;
	case 131:
		/* superframe counter (4), seconds (6), nanoseconds (4) */
		if (mask & omci_mask(4)) {
			const uint8_t *t = mib_val(in, 4);

			if (!t)
				break;
			kx_tod((uint32_t)t[0] << 24 | t[1] << 16 | t[2] << 8 | t[3],
			       (uint32_t)t[6] << 24 | t[7] << 16 | t[8] << 8 | t[9],
			       (uint32_t)t[10] << 24 | t[11] << 16 | t[12] << 8 |
			       t[13]);
		}
		break;
	default:
		break;
	}

	return 0;
}

int bind_set(struct omci_inst *in, uint16_t mask, const uint8_t *old[17])
{
	int ret = bind_set_one(in, mask, old);

	if (qos_class(in->def->class_id))
		qos_all();
	if (mcast_is_me(in->def->class_id))
		mcast_set(in, mask);

	return ret;
}

int bind_delete(struct omci_inst *in)
{
	if (mcast_is_me(in->def->class_id))
		mcast_delete(in);

	switch (in->def->class_id) {
	case 45: {
		struct omci_inst *cd = mib_find(46, in->id);

		if (cd)
			mib_delete(cd);
		break;
	}
	case 47: {
		struct omci_inst *dd = mib_find(48, in->id);
		struct omci_inst *td = mib_find(50, in->id);

		if (dd)
			mib_delete(dd);
		if (td)
			mib_delete(td);
		break;
	}
	case 268: {
		uint16_t gem = mib_get_uint(in, 1);
		int ret = kx_gem_del(gem);

		us_gem_refresh(in);
		return ret;
	}
	case 266:
	case 130:
		us_gem_refresh(in);
		break;
	case 281: {
		struct omci_inst *ctp = mib_find(268, mib_get_uint(in, 1));

		if (ctp && !ctp_is_mcast(ctp->id, in)) {
			kx_gem_del(mib_get_uint(ctp, 1));
			kx_gem_add(mib_get_uint(ctp, 1), ctp_alloc(ctp), false);
		}
		break;
	}
	default:
		break;
	}

	return 0;
}

/* MIB reset: every OLT-provisioned resource goes */
void bind_mib_reset(void)
{
	struct omci_inst *in;

	list_for_each_entry(in, &omci_mib, list) {
		uint16_t alloc;

		if (in->def->class_id != 262)
			continue;
		alloc = mib_get_uint(in, 1);
		if (!ALLOC_UNASSIGNED(alloc) && alloc != omci_onu_id)
			kx_tcont_del(alloc);
	}
	kx_gem_flush();
	mcast_mib_reset();
	us_gem = 0xffff;
	kx_onu_mib_reset();
}

/*
 * The kernel has nothing of what the OLT provisioned (it drops every T-CONT
 * and GEM port when the ONU is deactivated), but the MIB still has it: an
 * ONU keeps the MIB it restored or built.  Program the kernel from it,
 * the way the Creates and Sets would have, in the order of their
 * dependencies.  Nothing is announced or counted: the OLT did not ask for
 * any of it.
 */
bool bind_replay(void)
{
	/* a GEM port needs its T-CONT, an interworking TP its GEM port */
	static const uint16_t first[] = { 268, 266, 281, 130 };
	const uint8_t *old[17] = { 0 };
	struct omci_inst *in;
	unsigned int i;
	bool early, any = false;

	list_for_each_entry(in, &omci_mib, list)
		any |= in->by_olt || in->olt_mask;
	if (!any)
		return false;		/* the default MIB: nothing to put in */

	/* the kernel and the userspace state behind it start clean, as after
	 * a MIB reset; the MIB itself stays */
	bind_mib_reset();
	eth_reset();

	list_for_each_entry(in, &omci_mib, list) {
		uint16_t alloc;

		if (in->def->class_id != 262)
			continue;
		alloc = mib_get_uint(in, 1);
		if (!ALLOC_UNASSIGNED(alloc) && alloc != omci_onu_id)
			kx_tcont_add(alloc);
	}

	for (i = 0; i < sizeof(first) / sizeof(first[0]); i++)
		list_for_each_entry(in, &omci_mib, list)
			if (in->by_olt && in->def->class_id == first[i])
				bind_create_kernel(in);

	list_for_each_entry(in, &omci_mib, list) {
		early = false;
		for (i = 0; i < sizeof(first) / sizeof(first[0]); i++)
			early |= in->def->class_id == first[i];

		if (in->by_olt) {
			if (!early)
				bind_create_kernel(in);
			continue;
		}
		/* the ONU-created ones the OLT wrote to; the T-CONT Alloc-IDs
		 * are done above, the time of day is not replayed: it would be
		 * the time of the day it was stored */
		switch (in->def->class_id) {
		case 256:
		case 263:
		case 277:
		case CLASS_VEIP:
			if (in->olt_mask)
				bind_set_one(in, in->olt_mask, old);
			break;
		default:
			break;
		}
	}
	qos_all();
	INFO("OMCI: kernel programmed from the MIB");

	return true;
}
