// SPDX-License-Identifier: GPL-2.0
/*
 * OMCI MIB in RAM.  The OLT owns it: after MIB reset only the ONU-created
 * instances below exist, everything else is created by the OLT and lost
 * on the next MIB reset.  What the OLT built is kept in /usr/lib/gpon/gpon.json when
 * the admin submits it (mibstore.c) and is read back at every start.
 *
 * Initial instances and values follow the SDK OMCI stack
 * (omciInternalCreateFor*(), omci_me.h GPON_SLOT/MIN_TONT_MEID) with the
 * capabilities of the SDK romfile defaults (tcprofile romfile.cfg
 * GPON_ONU/GPON_ANI: TMOption 2, ONUSurvivalTime 10, ExtendedTClayer 3,
 * SecurityCap 1, TotalGEMPortNum 256, ConnectivityCap 0x7f, QosConfFlex 0,
 * SRIndication 1, PiggybackDBA 1, ONUResponseTime 35000); identity and
 * sizes come from /etc/config/gpon.
 */
#include <stdlib.h>
#include <string.h>

#include "gpond.h"
#include "omci.h"
#include "eth.h"
#include "omci_pm.h"

#define GPON_SLOT		0x80
#define MIN_TCONT_MEID		0x8000
#define QUEUES_PER_TCONT	8
#define QUEUES_PER_UNI		8
#define TCONT_POLICY_SP		1

#define ONU_TM_OPTION		2
#define ONU_SURVIVAL_TIME	10
#define ONU_EXT_TC_LAYER	3
#define ONU_SECURITY_CAP	1
#define ONU_TOTAL_GEM		256
#define ONU_CONNECTIVITY_CAP	0x7f
#define ANI_SR_INDICATION	1
#define ANI_PIGGYBACK_DBA	1
#define ANI_GEM_BLOCK_LEN	48
#define ANI_RESPONSE_TIME	35000
/* omciInternalCreateForPPTPEthernetUNI(): sensed type 0x2f, omci_me.h
 * UNIT_TYPE_BASET_101001000 */
#define PPTP_SENSED_TYPE	47

LIST_HEAD(omci_mib);

const struct omci_me_def *omci_me_find(uint16_t class_id)
{
	unsigned int i;

	for (i = 0; i < omci_me_defs_num; i++)
		if (omci_me_defs[i].class_id == class_id)
			return &omci_me_defs[i];

	return NULL;
}

const struct omci_attr_def *omci_attr(const struct omci_me_def *d,
				      uint8_t idx)
{
	unsigned int i;

	for (i = 0; i < d->nattr; i++)
		if (d->attr[i].idx == idx)
			return &d->attr[i];

	return NULL;
}

struct omci_inst *mib_find(uint16_t class_id, uint16_t id)
{
	struct omci_inst *in;

	list_for_each_entry(in, &omci_mib, list)
		if (in->def->class_id == class_id && in->id == id)
			return in;

	return NULL;
}

/* the first instance of a class (ONU-created singletons: ANI-G, ...) */
struct omci_inst *mib_first(uint16_t class_id)
{
	struct omci_inst *in;

	list_for_each_entry(in, &omci_mib, list)
		if (in->def->class_id == class_id)
			return in;

	return NULL;
}

struct omci_inst *mib_create(uint16_t class_id, uint16_t id, bool by_olt)
{
	const struct omci_me_def *d = omci_me_find(class_id);
	struct omci_inst *in;
	unsigned int i;

	if (!d || mib_find(class_id, id))
		return NULL;

	in = calloc(1, sizeof(*in));
	if (!in)
		return NULL;
	in->def = d;
	in->id = id;
	in->by_olt = by_olt;
	for (i = 0; i < d->nattr; i++) {
		const struct omci_attr_def *a = &d->attr[i];

		if (a->format == OMCI_FMT_TABLE || !a->size)
			continue;		/* empty table */
		in->val[a->idx] = calloc(1, a->size);
		if (!in->val[a->idx]) {		/* no value, no instance */
			for (i = 0; i <= OMCI_MAX_ATTR; i++)
				free(in->val[i]);
			free(in);
			return NULL;
		}
		in->len[a->idx] = a->size;
	}
	list_add_tail(&in->list, &omci_mib);

	return in;
}

void mib_delete(struct omci_inst *in)
{
	unsigned int i;

	list_del(&in->list);
	omci_alarm_inst_free(in);
	omci_pm_inst_free(in);
	for (i = 0; i <= OMCI_MAX_ATTR; i++)
		free(in->val[i]);
	free(in);
}

int mib_set_raw(struct omci_inst *in, uint8_t idx, const void *v, uint16_t len)
{
	uint8_t *p;

	if (idx < 1 || idx > OMCI_MAX_ATTR)
		return -1;
	p = realloc(in->val[idx], len ? len : 1);
	if (!p)
		return -1;
	memcpy(p, v, len);
	in->val[idx] = p;
	in->len[idx] = len;

	return 0;
}

uint8_t *mib_val(struct omci_inst *in, uint8_t idx)
{
	return idx >= 1 && idx <= OMCI_MAX_ATTR ? in->val[idx] : NULL;
}

/* big endian, attribute size bytes */
uint32_t mib_get_uint(struct omci_inst *in, uint8_t idx)
{
	uint8_t *p = mib_val(in, idx);
	uint32_t v = 0;
	unsigned int i;

	if (!p)
		return 0;
	for (i = 0; i < in->len[idx] && i < 4; i++)
		v = v << 8 | p[i];

	return v;
}

void mib_set_uint(struct omci_inst *in, uint8_t idx, uint32_t v)
{
	uint8_t *p = mib_val(in, idx);
	int i;

	if (!p)
		return;
	for (i = in->len[idx] - 1; i >= 0; i--, v >>= 8)
		p[i] = v & 0xff;
}

void mib_set_str(struct omci_inst *in, uint8_t idx, const char *s)
{
	uint8_t *p = mib_val(in, idx);
	size_t l = strlen(s);

	if (!p)
		return;
	memset(p, 0, in->len[idx]);
	memcpy(p, s, l < in->len[idx] ? l : in->len[idx]);
}

/* ---- the ONU-created MIB ---- */

static void init_onu(void)
{
	struct omci_inst *in;
	int i;

	mib_create(2, 0, false);		/* ONU data */

	in = mib_create(256, 0, false);		/* ONU-G */
	if (in) {
		mib_set_str(in, 1, cfg.vendor_id);
		mib_set_str(in, 2, cfg.version);
		if (cfg.sn_set)
			mib_set_raw(in, 3, cfg.sn, GPON_SN_LEN);
		mib_set_uint(in, 4, ONU_TM_OPTION);
		mib_set_uint(in, 9, ONU_SURVIVAL_TIME);
		mib_set_str(in, 10, cfg.loid);
		mib_set_str(in, 11, cfg.lopw);
		mib_set_uint(in, 13, ONU_EXT_TC_LAYER);
	}

	in = mib_create(257, 0, false);		/* ONU2-G */
	if (in) {
		mib_set_str(in, 1, cfg.equipment_id);
		mib_set_uint(in, 2, cfg.omcc_version);
		mib_set_uint(in, 3, cfg.product_code);
		mib_set_uint(in, 4, ONU_SECURITY_CAP);
		mib_set_uint(in, 5, 1);
		mib_set_uint(in, 6, (cfg.tconts * QUEUES_PER_TCONT) +
				    (cfg.uni_ports * QUEUES_PER_UNI));
		mib_set_uint(in, 7, cfg.tconts);
		mib_set_uint(in, 8, 1);
		mib_set_uint(in, 9, ONU_TOTAL_GEM);
		mib_set_uint(in, 11, ONU_CONNECTIVITY_CAP);
		mib_set_uint(in, 14, 1);
	}

	mib_create(131, 0, false);		/* OLT-G, written by the OLT */
	mib_create(160, 0, false);		/* Equipment extension package */

	for (i = 0; i < 2; i++) {		/* Software image */
		in = mib_create(7, i, false);
		if (!in)
			continue;
		mib_set_str(in, 1, cfg.sw_ver_slot[i]);
		mib_set_uint(in, 2, cfg.sw_committed == i);
		mib_set_uint(in, 3, cfg.sw_active == i);
		mib_set_uint(in, 4, cfg.sw_valid[i]);
	}
}

static void create_cardholder(uint16_t id, uint8_t type, uint8_t ports)
{
	struct omci_inst *in = mib_create(5, id, false);

	if (!in)
		return;
	mib_set_uint(in, 1, type);
	mib_set_uint(in, 2, type);
	mib_set_uint(in, 3, ports);
	mib_set_str(in, 4, cfg.equipment_id);
	mib_set_str(in, 5, cfg.equipment_id);
}

static void init_ani(void)
{
	struct omci_inst *in;
	int t, q;

	create_cardholder(0x0100 + GPON_SLOT, 248, 1);

	in = mib_create(263, (GPON_SLOT << 8) + 1, false);	/* ANI-G */
	if (in) {
		mib_set_uint(in, 1, ANI_SR_INDICATION);
		mib_set_uint(in, 2, cfg.tconts);
		mib_set_uint(in, 3, ANI_GEM_BLOCK_LEN);
		mib_set_uint(in, 4, ANI_PIGGYBACK_DBA);
		mib_set_uint(in, 6, cfg.sf ? cfg.sf : 5);
		mib_set_uint(in, 7, cfg.sd ? cfg.sd : 9);
		mib_set_uint(in, 11, 0xff);
		mib_set_uint(in, 12, 0xff);
		mib_set_uint(in, 13, ANI_RESPONSE_TIME);
		mib_set_uint(in, 15, 0x81);
		mib_set_uint(in, 16, 0x81);
	}

	for (t = 0; t < cfg.tconts; t++) {
		uint16_t tid = (GPON_SLOT << 8) + t;

		in = mib_create(262, tid, false);		/* T-CONT */
		if (in) {
			mib_set_uint(in, 1, !t && omci_onu_id >= 0 ?
					    omci_onu_id : 0x00ff);
			mib_set_uint(in, 2, 1);
			mib_set_uint(in, 3, TCONT_POLICY_SP);
		}
		in = mib_create(278, tid, false);		/* traffic scheduler */
		if (in) {
			mib_set_uint(in, 1, tid);
			mib_set_uint(in, 3, 1);
		}
		for (q = 0; q < QUEUES_PER_TCONT; q++) {	/* upstream PQ */
			in = mib_create(277, MIN_TCONT_MEID +
					t * QUEUES_PER_TCONT + q, false);
			if (!in)
				continue;
			mib_set_uint(in, 1, 1);
			mib_set_uint(in, 2, 0xffff);
			mib_set_uint(in, 3, 4);
			mib_set_uint(in, 6, (uint32_t)(0x8000 + t) << 16 |
					    (7 - (q & 7)));
			mib_set_uint(in, 8, 1);
			mib_set_uint(in, 11, 0xffff);
		}
	}
}

static void init_uni(void)
{
	uint8_t cpack_ports = cfg.uni_ports_set && cfg.uni_ports > 0 ?
			      cfg.uni_ports : 4;
	struct omci_inst *in;
	int p, q;

	create_cardholder(0x0101, PPTP_SENSED_TYPE, cpack_ports);
	create_cardholder(0x010a, 48, 1);

	in = mib_create(297, 0, false);			/* Port mapping */
	if (in) {
		uint8_t plist[16] = { 0 };
		uint8_t nports = 2 + (cpack_ports > 6 ? 6 : cpack_ports);

		mib_set_uint(in, 1, nports);
		wr_be16(&plist[0], 48);
		wr_be16(&plist[2], 248);
		for (p = 0; p < cpack_ports && p < 6; p++)
			wr_be16(&plist[4 + 2 * p], PPTP_SENSED_TYPE);
		mib_set_raw(in, 2, plist, sizeof(plist));
	}

	if (cfg.uni_ports > 0) {
		in = mib_create(6, 0x0101, false);	/* Circuit pack */
		if (in) {
			mib_set_uint(in, 1, PPTP_SENSED_TYPE);
			mib_set_uint(in, 2, cpack_ports);
			if (cfg.sn_set)
				mib_set_raw(in, 3, cfg.sn, GPON_SN_LEN);
			mib_set_str(in, 4, cfg.version);
			mib_set_str(in, 5, cfg.vendor_id);
			mib_set_str(in, 9, cfg.equipment_id);
			mib_set_uint(in, 12, cpack_ports * QUEUES_PER_UNI);
		}
	}

	for (p = 0; p < cfg.uni_ports; p++) {
		uint16_t id = 0x100 + p + 1;		/* slot 1, port p+1 */

		/* UNI-G: one per PPTP Ethernet UNI, as the SDK creates it
		 * (omciInitInstForPPTPEthernetUNI() -> omciInitInstForUniG()) */
		in = mib_create(264, id, false);
		if (in)
			mib_set_uint(in, 3, 2);	/* OMCI and non-OMCI */

		in = mib_create(11, id, false);		/* PPTP Ethernet UNI */
		if (in) {
			mib_set_uint(in, 2, PPTP_SENSED_TYPE);
			mib_set_uint(in, 8, 1518);	/* max frame size */
			mib_set_uint(in, 9, 0);
		}
		for (q = 0; q < QUEUES_PER_UNI; q++) {	/* downstream PQ */
			in = mib_create(277, p * QUEUES_PER_UNI + q, false);
			if (!in)
				continue;
			mib_set_uint(in, 1, 1);
			mib_set_uint(in, 2, 0xffff);
			mib_set_uint(in, 3, 4);
			mib_set_uint(in, 6, (uint32_t)id << 16 | q);
			mib_set_uint(in, 8, 1);
			mib_set_uint(in, 11, 0xffff);
		}
	}
}

/*
 * HGU side: the VEIP (omciInitInstForVirtualEthernetInterfacePoint(), SDK
 * omci_me_ethernet_services_func.c:2175-2186: UNI-G and VEIP 0x0a01, admin
 * and operational state 0, interdomain name zeros, TCP/UDP pointer 0,
 * IANA port 0xffff, :2104-2170) and the IP hosts
 * (omciInitInstForIPHostConfigData(), omci_me_layer3_data_services_func.c:
 * 247-268: ids 0 and 1, every attribute zero, :105-228).
 */
static void init_l3(void)
{
	struct omci_inst *in;
	int i;

	/* the VEIP has its own UNI-G, as the SDK creates it
	 * (omciInitInstForVirtualEthernetInterfacePoint()) */
	in = mib_create(264, VEIP_ME_ID, false);
	if (in)
		mib_set_uint(in, 3, 2);		/* OMCI and non-OMCI */
	in = mib_create(CLASS_VEIP, VEIP_ME_ID, false);
	if (in)
		mib_set_uint(in, 5, 0xffff);
	in = mib_create(CLASS_TR069, VEIP_ME_ID, false);
	if (in) {
		mib_set_uint(in, 1, 1);		/* locked until OLT unlocks */
		mib_set_uint(in, 2, 0xffff);
		mib_set_uint(in, 3, 0xffff);
	}
	for (i = 0; i < cfg.ip_hosts; i++)
		mib_create(CLASS_IP_HOST, i, false);
}

void mib_reset(void)
{
	struct omci_inst *in, *tmp;

	list_for_each_entry_safe(in, tmp, &omci_mib, list)
		mib_delete(in);

	if (cfg.tconts > GPON_MAX_TCONT)
		cfg.tconts = GPON_MAX_TCONT;
	if (cfg.uni_ports > GPON_MAX_UNI)
		cfg.uni_ports = GPON_MAX_UNI;
	if (cfg.ip_hosts < 0 || cfg.ip_hosts > 2)
		cfg.ip_hosts = cfg.ip_hosts < 0 ? 0 : 2;

	init_onu();
	init_ani();
	init_uni();
	init_l3();
}
