// SPDX-License-Identifier: GPL-2.0
/*
 * OMCI message engine (ITU-T G.988 baseline and extended message set).
 *
 * The kernel delivers every downstream OMCI frame of the OMCC GEM port
 * (baseline 44 or 48 bytes, extended 10 + contents length, each with or
 * without the 4 byte CRC) and transmits the responses built here without
 * it; the MAC appends the CRC.
 *
 * Extended message set (device identifier 0x0b), after the BSP omci binary
 * (userfs/bin/omci, the TCSUPPORT_OMCI_EXTENDED_MSG build of the SDK
 * omci_external_msg.h): 10 byte header (TCI, type, device id, ME id,
 * contents length), at most 1966 contents bytes, then the 4 bytes that are
 * the CRC on GPON (omciExtendedPayload_t.mic).  The SDK computes no MIC
 * in the OMCI process at all: its GPON MAC strips the trailing bytes of
 * every frame (gwan_prepare_tx_message(), remove_omci_crc_if_exist()); the
 * only MIC belongs to the XGS-PON driver, which a GPON MAC does not
 * have.  So nothing here checks or adds a MIC, and the bytes after the
 * contents are ignored.
 * The ONU only takes part when it announces it: the OMCC version of the
 * ONU2-G (onu.omcc_version, SDK romfile OMCCVersion) is 0xb0 or more;
 * the default 0xa0 answers an extended request "not supported" as before.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gpond.h"
#include "omci.h"
#include "omci_pm.h"
#include "eth.h"

#define OMCI_BASE_LEN		44	/* without CRC */
#define OMCI_VERSION_EXT	0xb0
#define OMCI_EXT_GET_HDR	7	/* result, mask, optional mask, failed mask */
#define OMCI_EXT_NEXT_HDR	3	/* result, attribute mask */
#define OMCI_EXT_ALARM_ROWS	61
#define OMCI_ALARM_ROW		32
#define OMCI_CLASS_OMCI		287
#define OMCI_MT_AR		0x40
#define OMCI_MT_AK		0x20
#define OMCI_MT_MASK		0x1f

static struct omci_upload_state upload;
static struct omci_table_get_state table_get;

/*
 * Last response per TCI priority, for retransmitted requests.  A Test whose
 * response is held back (omci_test.c) reserves the slot with pending set:
 * a retransmission of that Test is then swallowed instead of started a
 * second time, and the real answer follows as soon as the command is known
 * to run.
 */
static struct omci_last_resp last[2];
static struct omci_stats_cnt stats;

/*
 * The OLT has used the extended format (omciCreateAction() and
 * omciMIBUploadAction() set oltSupportVersion 0xb0 in the SDK): from now on
 * Alarm, AVC and the result of a Test that came as an extended message go
 * out as extended messages.
 */
static bool olt_ext;
static bool test_ext;
static uint16_t test_ext_tci;

/* the ONU announces the extended message set (ONU2-G OMCC version) */
static bool ext_enabled(void)
{
	return cfg.omcc_version >= OMCI_VERSION_EXT;
}

static uint8_t onu_state;
static bool cfg_done_sent;
static struct uloop_timeout cfg_done_tmo;
static struct uloop_timeout reboot_tmo;

static void mib_sync_inc(void)
{
	struct omci_inst *in = mib_find(2, 0);
	uint8_t v;

	if (!in)
		return;
	v = mib_get_uint(in, 1);
	/* 0xff wraps to 1; 0 only after MIB reset */
	mib_set_uint(in, 1, v == 0xff ? 1 : v + 1);
}

/* -------------------------------------------------------------------- */

static void send_resp(const uint8_t *req, uint8_t *rsp)
{
	uint16_t tci = req[0] << 8 | req[1];
	int pri = tci >> 15;

	rsp[0] = req[0];
	rsp[1] = req[1];
	rsp[2] = (req[2] & OMCI_MT_MASK) | OMCI_MT_AK;
	rsp[3] = OMCI_DEV_BASELINE;
	memcpy(&rsp[4], &req[4], 4);
	rsp[40] = 0;			/* CPCS-UU */
	rsp[41] = 0;			/* CPI */
	rsp[42] = 0;
	rsp[43] = 0x28;			/* SDU length */

	last[pri].tci = tci;
	last[pri].mt = req[2] & OMCI_MT_MASK;
	memcpy(last[pri].msg, rsp, OMCI_BASE_LEN);
	last[pri].len = OMCI_BASE_LEN;
	last[pri].valid = true;

	stats.tx++;
	kx_omci_tx(rsp, OMCI_BASE_LEN);
}

/* extended response: @rc is the contents, @rlen their length */
static void send_resp_ext(const uint8_t *req, const uint8_t *rc, size_t rlen)
{
	uint16_t tci = req[0] << 8 | req[1];
	int pri = tci >> 15;
	uint8_t *rsp = last[pri].msg;

	if (rlen > OMCI_EXT_CONT_MAX)
		rlen = OMCI_EXT_CONT_MAX;
	rsp[0] = req[0];
	rsp[1] = req[1];
	rsp[2] = (req[2] & OMCI_MT_MASK) | OMCI_MT_AK;
	rsp[3] = OMCI_DEV_EXTENDED;
	memcpy(&rsp[4], &req[4], 4);
	rsp[8] = rlen >> 8;
	rsp[9] = rlen & 0xff;
	memcpy(&rsp[OMCI_EXT_HDR], rc, rlen);

	last[pri].tci = tci;
	last[pri].mt = req[2] & OMCI_MT_MASK;
	last[pri].len = OMCI_EXT_HDR + rlen;
	last[pri].valid = true;

	stats.tx++;
	kx_omci_tx(rsp, last[pri].len);
}

/* row size of a table attribute, for the extended Get next chunks and AVCs:
 * the definitions of the older SDK ME tables leave some at 0, the omci
 * binary has them (descriptor length at +0x82) */
static unsigned int table_row(uint16_t cls, const struct omci_attr_def *a)
{
	if (a->size)
		return a->size;
	switch (cls << 8 | a->idx) {
	case 50 << 8 | 1:				/* Bridge table */
		return 8;
	case OMCI_CLASS_OMCI << 8 | 1:			/* ME type table */
		return 2;
	case OMCI_CLASS_OMCI << 8 | 2:			/* message type table */
		return 1;
	default:
		return 1;
	}
}

/*
 * Alarm, AVC or Test result (an extended message type 16, 17, 27 as the SDK
 * builds them: omciSendAlarmExtendMsg() 29 bytes, bitmap and sequence
 * number; omciSendAvcExtendMsg() attribute mask and value; the test result
 * keeps its 32 bytes), TCI as in the baseline messages.  false: not for
 * the extended format.
 */
static bool onu_msg_ext(uint16_t tci, uint8_t mt, uint16_t class_id,
			uint16_t id, const uint8_t *contents, bool test_ext)
{
	const struct omci_me_def *d = omci_me_find(class_id);
	uint8_t msg[OMCI_EXT_HDR + OMCI_CONTENTS];
	size_t clen;
	int idx;

	if (mt == MT_TEST_RESULT) {
		/* the result follows the format of its own Test request, not
		 * of whatever Test the OLT sent last */
		if (!test_ext)
			return false;
		clen = OMCI_CONTENTS;
	} else if (!olt_ext || !ext_enabled()) {
		return false;
	} else if (mt == OMCI_MT_ALARM) {
		clen = OMCI_ALARM_BYTES + 1;
	} else if (mt == OMCI_MT_AVC) {
		clen = 2;
		for (idx = 1; d && idx <= OMCI_MAX_ATTR; idx++) {
			const struct omci_attr_def *a = omci_attr(d, idx);

			if (!(rd_be16(contents) & omci_mask(idx)) || !a)
				continue;
			clen += a->format == OMCI_FMT_TABLE ?
				table_row(class_id, a) : a->size;
			break;
		}
		if (clen > OMCI_CONTENTS)
			clen = OMCI_CONTENTS;
	} else {
		return false;
	}

	memset(msg, 0, sizeof(msg));
	wr_be16(&msg[0], tci);
	msg[2] = mt & OMCI_MT_MASK;
	msg[3] = OMCI_DEV_EXTENDED;
	wr_be16(&msg[4], class_id);
	wr_be16(&msg[6], id);
	wr_be16(&msg[8], clen);
	memcpy(&msg[OMCI_EXT_HDR], contents, clen);
	if (mt == OMCI_MT_ALARM)			/* sequence number */
		msg[OMCI_EXT_HDR + OMCI_ALARM_BYTES] = contents[31];

	stats.tx++;
	kx_omci_tx(msg, OMCI_EXT_HDR + clen);

	return true;
}

/*
 * Spontaneous Alarm and AVC messages wait for the OLT to speak first: the
 * OMCC is not used by the ONU before the OLT has sent its first message in
 * this O5.  What was held back is not lost - the OLT resynchronises with
 * MIB reset / get all alarms / MIB upload.
 */
static bool olt_spoke;

bool omci_notify_ok(void)
{
	return onu_state == 5 && olt_spoke;
}

/*
 * ONU-initiated message: no AR, baseline format unless the OLT speaks the
 * extended one.  Alarm and AVC have TCI 0
 * (SDK omciSendAlarmMsg()/omciSendAvcBasicMsg()), a test result the TCI of
 * its test command (omci_test.c).
 */
static void onu_msg_tx(uint16_t tci, uint8_t mt, uint16_t class_id,
		       uint16_t id, const uint8_t *contents, bool test_ext)
{
	uint8_t msg[OMCI_BASE_LEN] = { 0 };

	if (onu_msg_ext(tci, mt, class_id, id, contents, test_ext))
		return;

	wr_be16(&msg[0], tci);
	msg[2] = mt & OMCI_MT_MASK;
	msg[3] = OMCI_DEV_BASELINE;
	wr_be16(&msg[4], class_id);
	wr_be16(&msg[6], id);
	memcpy(&msg[8], contents, OMCI_CONTENTS);
	msg[43] = 0x28;			/* SDU length, as send_resp() */

	stats.tx++;
	kx_omci_tx(msg, OMCI_BASE_LEN);
}

void omci_onu_msg_tx(uint16_t tci, uint8_t mt, uint16_t class_id,
		     uint16_t id, const uint8_t *contents)
{
	/* a Test result follows the Test the daemon answered last */
	onu_msg_tx(tci, mt, class_id, id, contents,
		   test_ext && test_ext_tci == tci);
}

void omci_notify_tx(uint8_t mt, uint16_t class_id, uint16_t id,
		    const uint8_t *contents)
{
	omci_onu_msg_tx(0, mt, class_id, id, contents);
}

static void cfg_changed(void)
{
	mib_sync_inc();
	eth_changed();
	if (onu_state == 5 && !cfg_done_sent && cfg.cfg_done_s > 0)
		uloop_timeout_set(&cfg_done_tmo, cfg.cfg_done_s * 1000);
	mibstore_changed();
}

/* ---- create / delete / set ---- */

static uint8_t do_create(uint16_t cls, uint16_t id, const uint8_t *c,
			 size_t clen, uint8_t *rc, bool ext)
{
	const struct omci_me_def *d = omci_me_find(cls);
	struct omci_inst *in;
	unsigned int i, off = 0;
	uint16_t fail = 0;

	if (!d || !(d->flags & OMCI_ME_SUPPORTED))
		return RES_UNKNOWN_ME;
	if (!(d->access & OMCI_ME_BY_OLT))
		return RES_NOT_SUPPORTED;
	in = mib_find(cls, id);
	if (in) {
		/* an object this ONU restored from /usr/lib/gpon/gpon.json and reported
		 * to the OLT: the OLT now asks for it, so it is replaced */
		if (!in->restored)
			return RES_INST_EXISTS;
		bind_delete(in);
		mib_delete(in);
	}

	in = mib_create(cls, id, true);
	if (!in)
		return RES_PROC_ERR;

	/* set-by-create attributes follow in attribute order */
	for (i = 0; i < d->nattr; i++) {
		const struct omci_attr_def *a = &d->attr[i];

		if (!(a->access & OMCI_ACC_SBC) || a->format == OMCI_FMT_TABLE)
			continue;
		if (off + a->size > clen) {
			/* extended: the attributes the message does not reach
			 * keep their defaults (getMaskOfCreateBySet()) */
			if (ext)
				break;
			fail |= omci_mask(a->idx);
			continue;
		}
		if (!bind_check(in, a->idx, c + off, true)) {
			DBG("OMCI: %s 0x%04x attribute %d: value rejected at create",
			    d->name, id, a->idx);
			fail |= omci_mask(a->idx);
			off += a->size;
			continue;
		}
		mib_set_raw(in, a->idx, c + off, a->size);
		off += a->size;
	}
	if (fail) {
		mib_delete(in);
		wr_be16(rc + 1, fail);
		return RES_PARAM_ERR;
	}

	if (bind_create(in)) {
		mib_delete(in);
		return RES_PROC_ERR;
	}
	omci_pm_inst_create(in);
	cfg_changed();

	return RES_OK;
}

static uint8_t do_delete(uint16_t cls, uint16_t id)
{
	const struct omci_me_def *d = omci_me_find(cls);
	struct omci_inst *in;

	if (!d || !(d->flags & OMCI_ME_SUPPORTED))
		return RES_UNKNOWN_ME;
	in = mib_find(cls, id);
	if (!in)
		return RES_UNKNOWN_INST;
	if (!in->by_olt)
		return RES_NOT_SUPPORTED;

	bind_delete(in);
	mib_delete(in);
	cfg_changed();

	return RES_OK;
}

/*
 * Rows of a table attribute: a row whose first half matches an existing
 * one replaces it, otherwise it is appended (G.988 tables differ in their
 * keys; the binding layer interprets them).
 */
static bool table_set(struct omci_inst *in, const struct omci_attr_def *a,
		      const uint8_t *row)
{
	unsigned int sz = a->size;			/* do_set() refuses size 0 */
	unsigned int key;
	bool del = true;
	uint32_t n;
	uint8_t *p;
	uint8_t delbyte = 0xff;

	if (sz < 2)
		return false;
	key = sz / 2;

	/*
	 * ME 281 IPv4 multicast address table (G.988 9.3.13): the key is the
	 * 4 byte destination MAC (attribute 9.1), the rest of the 12 byte row
	 * is the VLAN / source / group filter, and a row whose last 8 bytes
	 * are zero deletes it (the SDK, setMulticastGemIWTPIPv4MultiAddrTB()).
	 */
	if (in->def->class_id == 281 && a->idx == 9) {
		key = 4;
		delbyte = 0;
	}

	/* G.988 9.3.13: the second half all ones deletes the row with that
	 * filter (for ME 281 the 8 bytes after the MAC all zero) */
	for (n = key; n < sz; n++)
		if (row[n] != delbyte)
			del = false;
	if (del && key != sz) {
		for (n = 0; n + sz <= in->len[a->idx]; n += sz) {
			if (memcmp(in->val[a->idx] + n, row, key))
				continue;
			memmove(in->val[a->idx] + n, in->val[a->idx] + n + sz,
				in->len[a->idx] - n - sz);
			in->len[a->idx] -= sz;
			return true;
		}
		return true;
	}

	for (n = 0; n + sz <= in->len[a->idx]; n += sz) {
		if (!memcmp(in->val[a->idx] + n, row, key)) {
			memcpy(in->val[a->idx] + n, row, sz);
			return true;
		}
	}

	p = realloc(in->val[a->idx], in->len[a->idx] + sz);
	if (!p)
		return false;
	memcpy(p + in->len[a->idx], row, sz);
	in->val[a->idx] = p;
	in->len[a->idx] += sz;

	return true;
}

/* @ext: the contents may be longer.  A table attribute carries more than
 * one row in either format, so the window is taken in both. */
static uint8_t do_set(uint16_t cls, uint16_t id, const uint8_t *c,
		      size_t clen, uint8_t *rc, bool ext)
{
	const struct omci_me_def *d = omci_me_find(cls);
	const uint8_t *old[OMCI_MAX_ATTR + 1] = { 0 };
	uint16_t mask = rd_be16(c), fail = 0, done = 0;
	struct omci_inst *in;
	unsigned int off = 2;
	int idx, ret;

	if (!d || !(d->flags & OMCI_ME_SUPPORTED))
		return RES_UNKNOWN_ME;
	in = mib_find(cls, id);
	if (!in)
		return RES_UNKNOWN_INST;

	/* extended: a table attribute is the only one of its message
	 * (preprocessOmciMsgByMask()) */
	for (idx = 1; ext && idx <= OMCI_MAX_ATTR; idx++) {
		const struct omci_attr_def *a = omci_attr(d, idx);

		if ((mask & omci_mask(idx)) && a && a->format == OMCI_FMT_TABLE &&
		    (mask & ~omci_mask(idx))) {
			wr_be16(rc + 3, mask);
			return RES_ATTR_FAILED;
		}
	}
	for (idx = 1; idx <= OMCI_MAX_ATTR; idx++) {
		const struct omci_attr_def *a;
		unsigned int len, row;
		uint8_t *copy;

		if (!(mask & omci_mask(idx)))
			continue;
		a = omci_attr(d, idx);
		if (!a || !(a->access & OMCI_ACC_W) || !a->size) {
			fail |= omci_mask(idx);
			continue;
		}
		len = a->size;
		if (a->format == OMCI_FMT_TABLE && off < clen)
			len = (clen - off) / a->size * a->size;
		if (!len || off + len > clen) {
			fail |= omci_mask(idx);
			continue;
		}
		if (a->format == OMCI_FMT_TABLE) {
			/* the rows in turn; a rejected one fails the attribute */
			bool any = false;

			for (row = 0; row < len; row += a->size) {
				if (!bind_check(in, idx, c + off + row, false)) {
					DBG("OMCI: %s 0x%04x attribute %d: value rejected",
					    d->name, id, idx);
					fail |= omci_mask(idx);
					continue;
				}
				if (table_set(in, a, c + off + row))
					any = true;
				else
					fail |= omci_mask(idx);
			}
			off += len;
			if (any)
				done |= omci_mask(idx);
			continue;
		}
		if (!bind_check(in, idx, c + off, false)) {
			DBG("OMCI: %s 0x%04x attribute %d: value rejected",
			    d->name, id, idx);
			fail |= omci_mask(idx);
			off += a->size;
			continue;
		}
		copy = in->val[idx] && in->len[idx] ? malloc(in->len[idx]) : NULL;
		if (copy)
			memcpy(copy, in->val[idx], in->len[idx]);
		old[idx] = copy;
		mib_set_raw(in, idx, c + off, a->size);
		off += a->size;
		done |= omci_mask(idx);
	}

	if (!in->by_olt)
		in->olt_mask |= done;
	ret = done ? bind_set(in, done, old) : 0;
	/* unlocked: alarms raised while locked were not reported */
	if (d->admin && (done & omci_mask(d->admin)) && old[d->admin] &&
	    old[d->admin][0] == 1 && mib_get_uint(in, d->admin) == 0) {
		omci_alarm_unlocked(in);
		omci_pm_parent_unlocked(in);
	}
	if (done)
		omci_pm_set(in, done);
	for (idx = 0; idx <= OMCI_MAX_ATTR; idx++)
		free((void *)old[idx]);
	if (done)
		cfg_changed();
	/* alarm reporting control (re)started or stopped */
	if ((d->arc && (done & omci_mask(d->arc))) ||
	    (d->arc_intv && (done & omci_mask(d->arc_intv))))
		omci_alarm_arc_set(in);
	if (ret)
		return RES_PROC_ERR;
	if (fail) {
		wr_be16(rc + 3, fail);		/* attribute execution mask */
		return RES_ATTR_FAILED;
	}

	return RES_OK;
}

/* ---- get / get-next ---- */

/*
 * ME 287 OMCI: the tables are built when read (SDK getMETypeTableValue(),
 * getMessageTypeTableValue()): the class of every supported ME, and the
 * message types, as the omci binary lists them (omci binary 0x33fc0: 4, 6,
 * 8, 9, 0x0b-0x1d).
 */
static void me287_refresh(struct omci_inst *in)
{
	static const uint8_t mts[] = {
		4, 6, 8, 9, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
		0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c,
		0x1d,
	};
	uint8_t *t = malloc(2 * omci_me_defs_num);
	unsigned int i, n = 0;

	if (!t)
		return;
	for (i = 0; i < omci_me_defs_num; i++) {
		if (!(omci_me_defs[i].flags & OMCI_ME_SUPPORTED))
			continue;
		wr_be16(t + n, omci_me_defs[i].class_id);
		n += 2;
	}
	mib_set_raw(in, 1, t, n);
	free(t);
	mib_set_raw(in, 2, mts, sizeof(mts));
}

/*
 * ME 288 Managed Entity / ME 289 Attribute: created and refreshed on demand
 * when read (G.988 9.12.9-10; SDK getAttributeTableValue(), getAlarmsTable-
 * Value(), getAVCsTableValue(), getInstancesTableValue(), omciInternalCreate-
 * ForAttribute()).  Attribute instance IDs follow the SDK encoding:
 * (attr_idx << 10) | (class_id & 0x3ff).
 */
static struct omci_inst *me288_get(uint16_t id)
{
	const struct omci_me_def *d = omci_me_find(id);
	struct omci_inst *in, *it;
	uint8_t atab[2 * OMCI_MAX_ATTR], avcs[OMCI_MAX_ATTR];
	uint8_t *itab = NULL;
	const uint8_t *alms = NULL;
	unsigned int i, na = 0, navc = 0, nalm, ninst = 0;
	uint32_t acts = 1u << MT_GET;

	if (!d || !(d->flags & OMCI_ME_SUPPORTED))
		return NULL;
	in = mib_find(288, id);
	if (!in)
		in = mib_create(288, id, false);
	if (!in)
		return NULL;

	if (d->access & OMCI_ME_BY_OLT)
		acts |= (1u << MT_CREATE) | (1u << MT_DELETE);
	if (omci_pm_is_pm(d))
		acts |= 1u << MT_GET_CURRENT_DATA;
	if (id == 256 || id == 263)
		acts |= 1u << MT_TEST;
	if (id == 256)
		acts |= (1u << MT_REBOOT) | (1u << MT_SYNC_TIME);
	if (id == 2)
		acts |= (1u << MT_MIB_RESET) | (1u << MT_MIB_UPLOAD) |
			(1u << MT_MIB_UPLOAD_NEXT) | (1u << MT_GET_ALL_ALARMS) |
			(1u << MT_GET_ALL_ALARMS_NEXT);

	for (i = 0; i < d->nattr; i++) {
		const struct omci_attr_def *a = &d->attr[i];

		wr_be16(atab + na, ((uint16_t)a->idx << 10) | (id & 0x3ff));
		na += 2;
		if (a->flags & OMCI_ATTR_AVC)
			avcs[navc++] = a->idx;
		if (a->access & OMCI_ACC_W)
			acts |= 1u << MT_SET;
		if (a->format == OMCI_FMT_TABLE)
			acts |= 1u << MT_GET_NEXT;
	}
	nalm = omci_alarm_list(d, &alms);

	list_for_each_entry(it, &omci_mib, list)
		if (it->def == d)
			ninst++;
	if (ninst) {
		itab = malloc(2 * ninst);
		ninst = 0;
		list_for_each_entry(it, &omci_mib, list) {
			if (!itab || it->def != d)
				continue;
			wr_be16(itab + 2 * ninst, it->id);
			ninst++;
		}
	}

	mib_set_str(in, 1, d->name);
	mib_set_raw(in, 2, atab, na);
	mib_set_uint(in, 3, d->access);
	mib_set_raw(in, 4, alms, alms ? nalm : 0);
	mib_set_raw(in, 5, avcs, navc);
	mib_set_uint(in, 6, acts);
	mib_set_raw(in, 7, itab, 2 * ninst);
	mib_set_uint(in, 8, 1);
	free(itab);

	return in;
}

static struct omci_inst *me289_get(uint16_t id)
{
	uint16_t cls = id & 0x3ff;
	uint8_t idx = (id >> 10) & 0x1f;
	const struct omci_me_def *d = omci_me_find(cls);
	const struct omci_attr_def *a = d ? omci_attr(d, idx) : NULL;
	struct omci_inst *in;

	if (!d || !(d->flags & OMCI_ME_SUPPORTED) || !a)
		return NULL;
	in = mib_find(289, id);
	if (!in)
		in = mib_create(289, id, false);
	if (!in)
		return NULL;

	mib_set_str(in, 1, a->name);
	mib_set_uint(in, 2, a->size);
	mib_set_uint(in, 3, a->access);
	mib_set_uint(in, 4, a->format);
	mib_set_uint(in, 5, 0);
	mib_set_uint(in, 6, 0);
	mib_set_uint(in, 7, 0);
	mib_set_raw(in, 8, NULL, 0);
	mib_set_uint(in, 9, 1);

	return in;
}

/*
 * @ext: the answer is result, attribute mask, optional-attribute mask,
 * attribute execution mask, then the values (the layout of the omci binary,
 * 0x2e60c, "generalGetAction"); the baseline one has the two masks at the
 * end of its 32 bytes.  @rlen: the contents length of the extended answer.
 */
static void read_dev_mac(const char *dev, uint8_t mac[6])
{
	unsigned int b[6];
	char path[96];
	FILE *f;
	int i;

	snprintf(path, sizeof(path), "/sys/class/net/%s/address", dev);
	f = fopen(path, "r");
	if (!f)
		return;
	if (fscanf(f, "%x:%x:%x:%x:%x:%x", &b[0], &b[1], &b[2], &b[3], &b[4],
		   &b[5]) == 6) {
		for (i = 0; i < 6; i++)
			mac[i] = b[i];
	}
	fclose(f);
}

static void me46_refresh(struct omci_inst *in)
{
	struct omci_inst *br = mib_find(45, in->id), *bp;
	uint8_t mac[6] = { 0 }, root[8] = { 0 };
	uint16_t prio = br ? mib_get_uint(br, 4) : 0x8000;
	unsigned int cnt = 0;
	const char *brdev;

	brdev = eth_bridge_name(in->id);
	read_dev_mac(brdev ? brdev : GPON_NETDEV, mac);
	if (!(mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]))
		read_dev_mac(GPON_NETDEV, mac);
	list_for_each_entry(bp, &omci_mib, list)
		if (bp->def->class_id == 47 && mib_get_uint(bp, 1) == in->id)
			cnt++;

	wr_be16(root, prio);
	memcpy(root + 2, mac, 6);
	mib_set_raw(in, 1, mac, 6);
	mib_set_uint(in, 2, prio);
	mib_set_raw(in, 3, root, 8);
	mib_set_uint(in, 5, cnt > 255 ? 255 : cnt);
	if (br) {
		mib_set_uint(in, 7, mib_get_uint(br, 6));
		mib_set_uint(in, 8, mib_get_uint(br, 7));
	}
}

static int read_brport_no(const char *dev)
{
	unsigned int p = 0;
	char path[96];
	FILE *f;

	if (!dev || !dev[0])
		return -1;
	snprintf(path, sizeof(path), "/sys/class/net/%s/brport/port_no", dev);
	f = fopen(path, "r");
	if (!f)
		return -1;
	if (fscanf(f, "%i", &p) != 1)
		p = 0;
	fclose(f);

	return p ? (int)p : -1;
}

static void me50_refresh(struct omci_inst *in)
{
	struct omci_inst *bp = mib_find(47, in->id);
	struct br_fdb_rec fdb[128];
	uint8_t rows[128 * 8];
	unsigned int len = 0;
	int want_port = -1;
	size_t n, i;
	const char *brdev;
	char path[96];
	FILE *f;

	if (!bp) {
		mib_set_raw(in, 1, NULL, 0);
		return;
	}
	if (mib_get_uint(bp, 3) == 1)
		want_port = read_brport_no(eth_uni_dev(mib_get_uint(bp, 4)));

	brdev = eth_bridge_name(mib_get_uint(bp, 1));
	if (!brdev) {
		mib_set_raw(in, 1, NULL, 0);
		return;
	}
	snprintf(path, sizeof(path), "/sys/class/net/%s/brforward", brdev);
	f = fopen(path, "rb");
	if (!f) {
		mib_set_raw(in, 1, NULL, 0);
		return;
	}
	n = fread(fdb, sizeof(fdb[0]), 128, f);
	fclose(f);

	for (i = 0; i < n; i++) {
		int pno = ((int)fdb[i].port_hi << 8) | fdb[i].port_no;
		uint16_t age, info;

		if (fdb[i].is_local)
			continue;
		if (want_port >= 0 && pno != want_port)
			continue;
		age = fdb[i].ageing_timer / 100;
		if (!age)
			age = 1;
		else if (age > 4095)
			age = 4095;
		info = (age << 4) | (1 << 2);
		wr_be16(rows + len, info);
		memcpy(rows + len + 2, fdb[i].mac, 6);
		len += 8;
	}
	mib_set_raw(in, 1, rows, len);
}

/*
 * ME 268 GEM port network CTP: attribute 8 "Encryption state" is not
 * configured by OMCI (the OLT signals it with the PLOAM Encrypted_Port-ID);
 * report the state the kernel holds for the port.
 */
static void me268_refresh(struct omci_inst *in)
{
	uint8_t enc;

	if (!kx_gem_encrypt(mib_get_uint(in, 1), &enc))
		mib_set_uint(in, 8, enc);
}

/*
 * ME 47 MAC bridge port config data: attribute 10 "Port MAC address" is the
 * MAC of the device the port sits on, as the SDK reports the switch port MAC
 * for a LAN port and the CPE MAC for the WAN one.
 */
static void me47_refresh(struct omci_inst *in)
{
	uint8_t mac[6] = { 0 };
	uint8_t type = mib_get_uint(in, 3);
	uint16_t ptr = mib_get_uint(in, 4);
	const char *dev = NULL;

	switch (type) {
	case TP_PPTP_ETH_UNI:
		dev = eth_uni_dev(ptr);
		break;
	case TP_IP_HOST:
		dev = eth_l3_dev(CLASS_IP_HOST, ptr);
		break;
	case TP_VEIP:
		dev = eth_l3_dev(CLASS_VEIP, ptr);
		break;
	default:
		break;
	}
	read_dev_mac(dev ? dev : GPON_NETDEV, mac);
	mib_set_raw(in, 10, mac, 6);
}

/*
 * ME 48 MAC bridge port designation data (G.988 9.3.5, SDK getMBPDDPortState-
 * Value()): attribute 1 "bridge root cost port" (24 bytes: designated bridge
 * 8, designated root 8, designated port 4, path cost 4), attribute 2 "Port
 * state" (0 disabled, 1 listening, 2 learning, 3 forwarding, 4 blocking,
 * 5 linkdown, 6 stp_off).
 */
static void me48_refresh(struct omci_inst *in)
{
	struct omci_inst *bp = mib_find(47, in->id);
	struct omci_inst *br = bp ? mib_find(45, mib_get_uint(bp, 1)) : NULL;
	uint8_t brcp[24] = { 0 }, mac[6] = { 0 }, state = 6;
	uint16_t prio = br ? mib_get_uint(br, 4) : 0x8000;
	const char *brdev = br ? eth_bridge_name(br->id) : NULL;

	read_dev_mac(brdev ? brdev : GPON_NETDEV, mac);
	if (!(mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]))
		read_dev_mac(GPON_NETDEV, mac);
	wr_be16(brcp, prio);
	memcpy(brcp + 2, mac, 6);
	wr_be16(brcp + 8, prio);
	memcpy(brcp + 10, mac, 6);
	if (bp) {
		uint32_t pnum = mib_get_uint(bp, 2), cost = mib_get_uint(bp, 6);

		wr_be16(brcp + 16, pnum >> 16);
		wr_be16(brcp + 18, pnum);
		wr_be16(brcp + 20, cost >> 16);
		wr_be16(brcp + 22, cost);
	}
	mib_set_raw(in, 1, brcp, sizeof(brcp));

	if (bp && mib_get_uint(bp, 3) == TP_PPTP_ETH_UNI) {
		uint16_t uid = mib_get_uint(bp, 4);
		struct omci_inst *uni = mib_find(11, uid);
		const char *udev = eth_uni_dev(uid);
		bool stp = (br && mib_get_uint(br, 1)) || mib_get_uint(bp, 7);

		if (uni && mib_get_uint(uni, 5) == 1) {
			state = 0;		/* disabled (admin locked) */
		} else if (uni && mib_get_uint(uni, 6) == 1) {
			state = 5;		/* linkdown */
		} else if (stp && udev) {
			unsigned int st = 3;
			char path[96];
			FILE *f;

			snprintf(path, sizeof(path),
				 "/sys/class/net/%s/brport/state", udev);
			f = fopen(path, "r");
			if (f) {
				if (fscanf(f, "%u", &st) == 1 && st <= 4)
					state = st;
				fclose(f);
			}
		}
	} else if (br && mib_get_uint(br, 1)) {
		state = 3;			/* forwarding */
	}
	mib_set_uint(in, 2, state);
}

static uint8_t do_get(uint16_t cls, uint16_t id, const uint8_t *c,
		      uint8_t *rc, bool current, bool ext, size_t *rlen)
{
	const struct omci_me_def *d = omci_me_find(cls);
	uint16_t mask = rd_be16(c), got = 0, fail = 0;
	unsigned int off = ext ? OMCI_EXT_GET_HDR : 3;
	size_t limit = ext ? OMCI_EXT_CONT_MAX : 28;
	struct omci_inst *in;
	int idx;

	if (!d || !(d->flags & OMCI_ME_SUPPORTED))
		return RES_UNKNOWN_ME;
	if (cls == 288)
		in = me288_get(id);
	else if (cls == 289)
		in = me289_get(id);
	else
		in = mib_find(cls, id);
	if (!in)
		return RES_UNKNOWN_INST;
	if (ext && !mask)
		return RES_PROC_ERR;		/* the SDK refuses an empty mask */
	if (cls == OMCI_CLASS_OMCI)
		me287_refresh(in);
	else if (cls == 46)
		me46_refresh(in);
	else if (cls == 48)
		me48_refresh(in);
	else if (cls == 50)
		me50_refresh(in);
	else if (cls == 268 && (mask & omci_mask(8)))
		me268_refresh(in);
	else if (cls == 47 && (mask & omci_mask(10)))
		me47_refresh(in);

	/* PM ME: the history bank, Get current data: the running one */
	if (omci_pm_is_pm(d))
		omci_pm_refresh(in, current);

	if (cls == 257 && (mask & omci_mask(10))) {	/* SysUpTime, 10 ms */
		struct timespec t;

		clock_gettime(CLOCK_MONOTONIC, &t);
		mib_set_uint(in, 10, t.tv_sec * 100 + t.tv_nsec / 10000000);
	}

	for (idx = 1; idx <= OMCI_MAX_ATTR; idx++) {
		const struct omci_attr_def *a;

		if (!(mask & omci_mask(idx)))
			continue;
		a = omci_attr(d, idx);
		if (!a || !(a->access & OMCI_ACC_R)) {
			fail |= omci_mask(idx);
			continue;
		}
		if (a->format == OMCI_FMT_TABLE) {
			/* the size; rows follow with get-next */
			uint32_t len = in->len[idx];

			if (off + 4 > limit) {
				fail |= omci_mask(idx);
				continue;
			}
			free(table_get.data);
			table_get.data = len ? malloc(len) : NULL;
			if (len && table_get.data)
				memcpy(table_get.data, in->val[idx], len);
			table_get.len = table_get.data ? len : 0;
			table_get.class_id = cls;
			table_get.id = id;
			table_get.mask = omci_mask(idx);
			rc[off] = len >> 24;
			rc[off + 1] = len >> 16;
			rc[off + 2] = len >> 8;
			rc[off + 3] = len;
			off += 4;
		} else {
			if (off + in->len[idx] > limit) {
				fail |= omci_mask(idx);
				continue;
			}
			memcpy(rc + off, in->val[idx], in->len[idx]);
			off += in->len[idx];
		}
		got |= omci_mask(idx);
	}

	wr_be16(rc + 1, got);
	*rlen = ext ? off : OMCI_CONTENTS;
	if (fail) {
		/* attribute execution mask */
		wr_be16(rc + (ext ? 5 : 30), fail);
		return RES_ATTR_FAILED;
	}

	return RES_OK;
}

/*
 * A Get-next chunk: 29 bytes in the baseline format; extended: whole rows
 * that fit the contents behind result and mask (omciGetTableContent()
 * rounds 0x7ae down to a multiple of the row size; here the 3 header bytes
 * are taken off first, the SDK would overrun the contents by them).
 */
static uint32_t next_chunk(uint16_t cls, uint16_t mask, bool ext)
{
	const struct omci_me_def *d = omci_me_find(cls);
	unsigned int row = 1, avail = OMCI_EXT_CONT_MAX - OMCI_EXT_NEXT_HDR;
	int idx;

	if (!ext)
		return 29;
	for (idx = 1; d && idx <= OMCI_MAX_ATTR; idx++) {
		const struct omci_attr_def *a = omci_attr(d, idx);

		if ((mask & omci_mask(idx)) && a) {
			row = table_row(cls, a);
			break;
		}
	}

	return row > avail ? avail : avail / row * row;
}

static uint8_t do_get_next(uint16_t cls, uint16_t id, const uint8_t *c,
			   uint8_t *rc, bool ext, size_t *rlen)
{
	uint16_t mask = rd_be16(c), seq = rd_be16(c + 2);
	uint32_t chunk = next_chunk(cls, mask, ext), off = seq * chunk, n;

	if (cls != table_get.class_id || id != table_get.id ||
	    mask != table_get.mask || off >= table_get.len)
		return RES_PARAM_ERR;

	n = table_get.len - off < chunk ? table_get.len - off : chunk;
	wr_be16(rc + 1, mask);
	memcpy(rc + 3, table_get.data + off, n);
	*rlen = 3 + n;

	return RES_OK;
}

/* ---- MIB upload ---- */

static void upload_free(void)
{
	free(upload.chunks);
	upload.chunks = NULL;
	upload.n = 0;
}

/* a new upload-next message for @in; NULL when out of memory */
static struct upload_chunk *upload_chunk(struct omci_inst *in)
{
	struct upload_chunk *p, *ch;

	p = realloc(upload.chunks, (upload.n + 1) * sizeof(*upload.chunks));
	if (!p)
		return NULL;
	upload.chunks = p;
	ch = &p[upload.n++];
	memset(ch, 0, sizeof(*ch));
	ch->class_id = in->def->class_id;
	ch->id = in->id;

	return ch;
}

static void upload_add(struct omci_inst *in)
{
	const struct omci_me_def *d = in->def;
	struct upload_chunk *ch = NULL;
	unsigned int i;

	for (i = 0; i < d->nattr; i++) {
		const struct omci_attr_def *a = &d->attr[i];

		/* a PM ME uploads only its threshold data id / control
		 * block (SDK omciMIBUploadCalculatePmMe(): mask 0x4000) */
		if (omci_pm_is_pm(d) && a->idx != 2)
			continue;
		if (!(a->access & OMCI_ACC_R) || a->format == OMCI_FMT_TABLE ||
		    !a->size || a->size > sizeof(ch->data) ||
		    !in->val[a->idx])
			continue;
		if (!ch || ch->len + a->size > sizeof(ch->data)) {
			ch = upload_chunk(in);
			if (!ch)
				return;
		}
		memcpy(ch->data + ch->len, in->val[a->idx], a->size);
		ch->len += a->size;
		ch->mask |= omci_mask(a->idx);
	}

	/* an instance without uploadable attributes is still announced */
	if (!ch)
		upload_chunk(in);
}

static uint8_t do_mib_upload(uint8_t *rc)
{
	struct omci_inst *in;

	upload_free();
	list_for_each_entry(in, &omci_mib, list) {
		if ((in->def->flags & OMCI_ME_NO_MIB_UPLOAD) ||
		    !(in->def->flags & OMCI_ME_SUPPORTED))
			continue;
		upload_add(in);
	}
	wr_be16(rc, upload.n);

	return RES_OK;
}

static void do_mib_upload_next(const uint8_t *c, uint8_t *rc)
{
	uint16_t seq = rd_be16(c);
	struct upload_chunk *ch;

	if (seq >= upload.n)
		return;			/* all zero */
	ch = &upload.chunks[seq];
	wr_be16(rc, ch->class_id);
	wr_be16(rc + 2, ch->id);
	wr_be16(rc + 4, ch->mask);
	memcpy(rc + 6, ch->data, ch->len);
}

/* ---- extended MIB upload / get all alarms ---- */

/*
 * The extended MIB upload snapshot: blocks of at most 1966 bytes, one block
 * per MIB upload next, never mixing classes (omciMIBUploadCalculateCommomMe_
 * Extend()).  A block is a list of entries
 *   attribute bytes (2), class (2), instance (2), attribute mask (2),
 *   the values of the readable attributes of the instance in index order
 * (every attribute of the ME in one entry, not 26 byte parts); a PM ME only
 * has its threshold data id (omciMIBUploadCalculatePmMe_Extend()).  A new
 * block starts when the next entry would not fit.
 */
static struct omci_upload_ext_state upload_ext;

static void upload_ext_free(void)
{
	free(upload_ext.data);
	free(upload_ext.start);
	free(upload_ext.size);
	memset(&upload_ext, 0, sizeof(upload_ext));
}

/* appends an entry (@e, @elen) to the current block of the snapshot */
static int upload_ext_add(const uint8_t *e, size_t elen, bool *fresh)
{
	size_t *ns, *nz;
	uint8_t *nd;

	if (*fresh || !upload_ext.n ||
	    upload_ext.size[upload_ext.n - 1] + elen > OMCI_EXT_CONT_MAX) {
		ns = realloc(upload_ext.start, (upload_ext.n + 1) * sizeof(*ns));
		if (!ns)
			return -1;
		upload_ext.start = ns;
		nz = realloc(upload_ext.size, (upload_ext.n + 1) * sizeof(*nz));
		if (!nz)
			return -1;
		upload_ext.size = nz;
		upload_ext.start[upload_ext.n] = upload_ext.len;
		upload_ext.size[upload_ext.n] = 0;
		upload_ext.n++;
		*fresh = false;
	}
	nd = realloc(upload_ext.data, upload_ext.len + elen);
	if (!nd)
		return -1;
	memcpy(nd + upload_ext.len, e, elen);
	upload_ext.data = nd;
	upload_ext.len += elen;
	upload_ext.size[upload_ext.n - 1] += elen;

	return 0;
}

static void upload_ext_inst(struct omci_inst *in, bool *fresh)
{
	const struct omci_me_def *d = in->def;
	uint8_t e[OMCI_EXT_CONT_MAX];
	unsigned int i, alen = 0;
	uint16_t m = 0;

	for (i = 0; i < d->nattr; i++) {
		const struct omci_attr_def *a = &d->attr[i];

		if (omci_pm_is_pm(d) && a->idx != 2)
			continue;
		if (!(a->access & OMCI_ACC_R) || a->format == OMCI_FMT_TABLE ||
		    !a->size || !in->val[a->idx] ||
		    8 + alen + a->size > sizeof(e))
			continue;
		memcpy(e + 8 + alen, in->val[a->idx], a->size);
		alen += a->size;
		m |= omci_mask(a->idx);
	}
	wr_be16(e, alen);
	wr_be16(e + 2, d->class_id);
	wr_be16(e + 4, in->id);
	wr_be16(e + 6, m);
	if (upload_ext_add(e, 8 + alen, fresh))
		ERR("OMCI: out of memory in the MIB upload");
}

static uint16_t do_mib_upload_ext(void)
{
	unsigned int i;

	upload_ext_free();
	for (i = 0; i < omci_me_defs_num; i++) {
		const struct omci_me_def *d = &omci_me_defs[i];
		bool fresh = true;
		struct omci_inst *in;

		if ((d->flags & OMCI_ME_NO_MIB_UPLOAD) ||
		    !(d->flags & OMCI_ME_SUPPORTED))
			continue;
		list_for_each_entry(in, &omci_mib, list)
			if (in->def == d)
				upload_ext_inst(in, &fresh);
	}

	return upload_ext.n;
}

/* the contents of block @seq; 0: none */
static size_t do_mib_upload_next_ext(uint16_t seq, uint8_t *rc)
{
	if (seq >= upload_ext.n)
		return 0;
	memcpy(rc, upload_ext.data + upload_ext.start[seq],
	       upload_ext.size[seq]);

	return upload_ext.size[seq];
}

/* get all alarms: entries (class, instance, bitmap) of 32 bytes, 61 a message */
static unsigned int ga_total;

static uint16_t do_get_all_alarms_ext(uint8_t mode)
{
	ga_total = omci_get_all_alarms(mode);

	return (ga_total + OMCI_EXT_ALARM_ROWS - 1) / OMCI_EXT_ALARM_ROWS;
}

static size_t do_get_all_alarms_next_ext(uint16_t seq, uint8_t *rc)
{
	unsigned int first = seq * OMCI_EXT_ALARM_ROWS, n = 0;

	while (n < OMCI_EXT_ALARM_ROWS && first + n < ga_total) {
		omci_get_all_alarms_next(first + n, rc + n * OMCI_ALARM_ROW);
		n++;
	}

	return n * OMCI_ALARM_ROW;
}

/*
 * The MEs of the extended set next to the ONU-created MIB: OMCI (287, one
 * instance, omciInitInstForOmci()).  After mib_reset().
 */
static void omci_ext_init(void)
{
	mib_create(OMCI_CLASS_OMCI, 0, false);
	mib_create(158, 0, false);		/* ONU remote debug */
}

static void do_mib_reset(void)
{
	upload_ext_free();
	ga_total = 0;
	upload_free();
	free(table_get.data);
	memset(&table_get, 0, sizeof(table_get));
	bind_mib_reset();
	eth_reset();
	mib_reset();
	omci_ext_init();
	omci_alarm_mib_reset();
	omci_pm_mib_reset();
	/* conditions that are still there are active in the new MIB too */
	monitor_resync();
	eth_changed();
	cfg_done_sent = false;
	uloop_timeout_cancel(&cfg_done_tmo);
	mibstore_changed();
	INFO("OMCI: MIB reset");
}

/* factory: the MIB the ONU had before any OLT touched it */
void omci_factory(void)
{
	do_mib_reset();
	mibstore_erase();
}

/* ---- test ---- */

static uint8_t do_test(const uint8_t *req, const uint8_t *c)
{
	struct omci_inst *in;
	uint16_t cls = rd_be16(req + 4), id = rd_be16(req + 6);
	const struct omci_me_def *d = omci_me_find(cls);

	if (!d || !(d->flags & OMCI_ME_SUPPORTED))
		return RES_UNKNOWN_ME;
	in = mib_find(cls, id);
	if (!in)
		return RES_UNKNOWN_INST;

	return omci_test(rd_be16(req), in, req, c);
}

/* ---- software image ---- */

/*
 * Nothing is flashed: activating or committing an image only changes what
 * is reported (ME 7 is_active/is_committed) and onu.sw_active /
 * onu.sw_committed, written as main.config_mode says.
 */
static uint8_t do_sw_image(uint8_t mt, uint16_t id)
{
	struct omci_inst *in = mib_find(7, id), *other = mib_find(7, !id);
	uint8_t attr = mt == MT_ACTIVATE_SW ? 3 : 2;
	int *slot = mt == MT_ACTIVATE_SW ? &cfg.sw_active : &cfg.sw_committed;

	if (!in || id > 1)
		return RES_UNKNOWN_INST;
	if (!mib_get_uint(in, 4))
		return RES_PARAM_ERR;		/* image not valid */

	mib_set_uint(in, attr, 1);
	if (other)
		mib_set_uint(other, attr, 0);
	if (*slot != id) {
		*slot = id;
		INFO("OMCI: image %u %s", id,
		     mt == MT_ACTIVATE_SW ? "activated" : "committed");
		if (config_write(&cfg))
			ERR("OMCI: cannot record the software image state");
	}

	return RES_OK;
}

static void reboot_cb(struct uloop_timeout *t)
{
	INFO("OMCI: reboot requested by the OLT (reboot_mode=1)");
	/* what the OLT changed since "submit" is not stored */
	sync();
	if (ubus_system_reboot())
		ERR("OMCI: reboot through procd failed");
}

static void do_reboot(void)
{
	if (cfg.reboot_mode == 1) {
		uloop_timeout_set(&reboot_tmo, 1000);
		return;
	}
	INFO("OMCI: reboot requested by the OLT (ignored, reboot_mode=0)");
}

static void cfg_done_cb(struct uloop_timeout *t)
{
	if (onu_state != 5 || cfg_done_sent)
		return;
	cfg_done_sent = true;
	INFO("OMCI: configuration settled");
	kx_cfg_done();
	if (mibstore_dirty())
		mibstore_submit();
}

/* -------------------------------------------------------------------- */

/* the extended answer of an error the SDK's omciRxHandle() reports: result only */
static size_t ext_rlen(uint8_t res, size_t len)
{
	switch (res) {
	case RES_NOT_SUPPORTED:
	case RES_UNKNOWN_ME:
	case RES_UNKNOWN_INST:
		return 1;
	default:
		return len;
	}
}

/*
 * One extended message of the OLT (@len: header, contents and whatever
 * follows them).  The response contents length per message type is the one
 * of the SDK actions: Create 3 (result, attribute execution mask), Delete
 * 1, Set 5 (result, unsupported and failed attribute masks), Set table 1,
 * Get / Get current data 7 + values, Get next 3 + chunk, MIB upload 2 (the
 * number of blocks), MIB upload next one block, Get all alarms 2, Get all
 * alarms next up to 61 entries, MIB reset 1, Test 1, Synchronize time 2,
 * Reboot, Activate and Commit software 1.
 */
static void omci_rx_ext(const uint8_t *msg, size_t len)
{
	uint8_t cbuf[OMCI_EXT_CONT_MAX + 8] = { 0 };
	uint8_t rc[OMCI_EXT_CONT_MAX];
	uint16_t tci = rd_be16(msg), cls = rd_be16(msg + 4), id = rd_be16(msg + 6);
	size_t clen = rd_be16(msg + 8), rlen = 1;
	uint8_t mt = msg[2] & OMCI_MT_MASK, res = RES_OK;
	const uint8_t *c = cbuf;
	int pri = tci >> 15;

	if (len < OMCI_EXT_HDR || clen > OMCI_EXT_CONT_MAX ||
	    OMCI_EXT_HDR + clen > len) {
		stats.errors++;
		return;
	}
	memset(rc, 0, sizeof(rc));
	/* a short message reads as zeros past its contents */
	memcpy(cbuf, msg + OMCI_EXT_HDR, clen);
	stats.rx_ext++;

	if (last[pri].valid && last[pri].tci == tci && last[pri].mt == mt &&
	    tci) {
		stats.retrans++;
		kx_omci_tx(last[pri].msg, last[pri].len);
		return;
	}

	DBG("OMCI rx ext tci %04x mt %u class %u inst 0x%04x len %zu", tci, mt,
	    cls, id, clen);

	switch (mt) {
	case MT_CREATE:
		olt_ext = true;
		res = do_create(cls, id, c, clen, rc, true);
		if (!res)
			mibstore_record_frame(msg, OMCI_EXT_HDR + clen);
		rlen = ext_rlen(res, 3);
		break;
	case MT_DELETE:
		res = do_delete(cls, id);
		if (!res)
			mibstore_record_frame(msg, OMCI_EXT_HDR + clen);
		break;
	case MT_SET:
	case MT_SET_TABLE:
		res = do_set(cls, id, c, clen, rc, true);
		if (!res)
			mibstore_record_frame(msg, OMCI_EXT_HDR + clen);
		rlen = mt == MT_SET ? ext_rlen(res, 5) : 1;
		break;
	case MT_GET:
	case MT_GET_CURRENT_DATA: {
		const struct omci_me_def *d = omci_me_find(cls);

		if (mt == MT_GET_CURRENT_DATA && !(d && omci_pm_is_pm(d))) {
			res = d ? RES_NOT_SUPPORTED : RES_UNKNOWN_ME;
			break;
		}
		res = do_get(cls, id, c, rc, mt == MT_GET_CURRENT_DATA, true,
			     &rlen);
		if (res == RES_PROC_ERR)
			rlen = 1;
		else
			rlen = ext_rlen(res, rlen);
		break;
	}
	case MT_GET_NEXT:
		res = do_get_next(cls, id, c, rc, true, &rlen);
		if (res)
			rlen = 1;
		break;
	case MT_GET_ALL_ALARMS:
		wr_be16(rc, do_get_all_alarms_ext(c[0]));
		rlen = 2;
		break;
	case MT_GET_ALL_ALARMS_NEXT:
		rlen = do_get_all_alarms_next_ext(rd_be16(c), rc);
		break;
	case MT_MIB_UPLOAD:
		olt_ext = true;
		wr_be16(rc, do_mib_upload_ext());
		rlen = 2;
		break;
	case MT_MIB_UPLOAD_NEXT:
		rlen = do_mib_upload_next_ext(rd_be16(c), rc);
		break;
	case MT_MIB_RESET:
		do_mib_reset();
		mibstore_record_frame(msg, OMCI_EXT_HDR + clen);
		break;
	case MT_SYNC_TIME:
		omci_pm_sync_time();
		rlen = 2;
		break;
	case MT_REBOOT:
		do_reboot();
		break;
	case MT_ACTIVATE_SW:
	case MT_COMMIT_SW:
		res = do_sw_image(mt, id);
		break;
	case MT_TEST:
		test_ext = true;
		test_ext_tci = tci;
		res = do_test(msg, c);
		break;
	default:
		/* software download stays baseline only: its sections are 31
		 * bytes (swdl.c) */
		res = RES_NOT_SUPPORTED;
		break;
	}

	/* the alarm and MIB upload answers start with their count / entries */
	if (mt < MT_GET_ALL_ALARMS || mt > MT_MIB_UPLOAD_NEXT)
		rc[0] = res;

	if (msg[2] & OMCI_MT_AR)
		send_resp_ext(msg, rc, rlen);
}

static void extended_not_supported(const uint8_t *req)
{
	uint8_t rsp[14] = { 0 };

	memcpy(rsp, req, 8);
	rsp[2] = (req[2] & OMCI_MT_MASK) | OMCI_MT_AK;
	rsp[9] = 1;			/* message contents length */
	rsp[10] = RES_NOT_SUPPORTED;
	stats.tx++;
	kx_omci_tx(rsp, 11);
}

void omci_rx(const uint8_t *msg, size_t len)
{
	uint8_t rsp[OMCI_BASE_LEN] = { 0 };
	uint8_t *rc = &rsp[8];
	uint16_t tci, cls, id;
	const uint8_t *c;
	size_t rlen;
	uint8_t mt;
	int pri;

	stats.rx++;
	olt_spoke = true;
	gpond_hexdump("OMCI rx", msg, len);
	if (len < 8)
		goto bad;

	/*
	 * An AK is the OLT acknowledging one of our messages, not a request.
	 * No request here waits for one, so running it through the request
	 * path would execute a response as a command.  Count it and drop it.
	 */
	if (msg[2] & OMCI_MT_AK) {
		stats.rx_ak++;
		return;
	}

	if (msg[3] == OMCI_DEV_EXTENDED) {
		if (ext_enabled())
			omci_rx_ext(msg, len);
		else if (msg[2] & OMCI_MT_AR)
			extended_not_supported(msg);
		return;
	}
	if (msg[3] != OMCI_DEV_BASELINE || len < OMCI_BASE_LEN)
		goto bad;

	tci = rd_be16(msg);
	mt = msg[2] & OMCI_MT_MASK;
	cls = rd_be16(msg + 4);
	id = rd_be16(msg + 6);
	c = msg + 8;
	pri = tci >> 15;

	/* a retransmission gets the same answer again */
	if (last[pri].valid && last[pri].tci == tci && last[pri].mt == mt &&
	    tci) {
		stats.retrans++;
		kx_omci_tx(last[pri].msg, last[pri].len);
		return;
	}

	DBG("OMCI rx tci %04x mt %u class %u inst 0x%04x", tci, mt, cls, id);

	switch (mt) {
	case MT_CREATE:
		rc[0] = do_create(cls, id, c, OMCI_CONTENTS, rc, false);
		if (!rc[0])
			mibstore_record_frame(msg, OMCI_BASE_LEN);
		break;
	case MT_DELETE:
		rc[0] = do_delete(cls, id);
		if (!rc[0])
			mibstore_record_frame(msg, OMCI_BASE_LEN);
		break;
	case MT_SET:
		rc[0] = do_set(cls, id, c, OMCI_CONTENTS, rc, false);
		if (!rc[0])
			mibstore_record_frame(msg, OMCI_BASE_LEN);
		break;
	case MT_GET:
		rc[0] = do_get(cls, id, c, rc, false, false, &rlen);
		break;
	case MT_GET_NEXT:
		rc[0] = do_get_next(cls, id, c, rc, false, &rlen);
		break;
	case MT_GET_ALL_ALARMS:
		/* contents: alarm retrieval mode; answer: number of nexts */
		wr_be16(rc, omci_get_all_alarms(c[0]));
		break;
	case MT_GET_ALL_ALARMS_NEXT:
		omci_get_all_alarms_next(rd_be16(c), rc);
		break;
	case MT_MIB_UPLOAD:
		do_mib_upload(rc);
		break;
	case MT_MIB_UPLOAD_NEXT:
		do_mib_upload_next(c, rc);
		break;
	case MT_MIB_RESET:
		do_mib_reset();
		mibstore_record_frame(msg, OMCI_BASE_LEN);
		rc[0] = RES_OK;
		break;
	case MT_SYNC_TIME:
		rc[0] = RES_OK;
		omci_pm_sync_time();
		break;
	case MT_REBOOT:
		rc[0] = RES_OK;
		do_reboot();
		break;
	case MT_ACTIVATE_SW:
	case MT_COMMIT_SW:
		rc[0] = do_sw_image(mt, id);
		break;
	case MT_START_SW_DOWNLOAD:
		rc[0] = swdl_start(id, c, rc);
		break;
	case MT_DOWNLOAD_SECTION:
		/* only the last section of a window asks for an answer */
		rc[0] = swdl_section(id, c, msg[2] & OMCI_MT_AR, rc);
		break;
	case MT_END_SW_DOWNLOAD:
		rc[0] = swdl_end(id, c, rc);
		break;
	case MT_TEST:
		test_ext = false;
		rc[0] = do_test(msg, c);
		break;
	case MT_GET_CURRENT_DATA:
		/* only the PM MEs have a running bank */
		if (omci_me_find(cls) && omci_pm_is_pm(omci_me_find(cls)))
			rc[0] = do_get(cls, id, c, rc, true, false, &rlen);
		else
			rc[0] = omci_me_find(cls) ? RES_NOT_SUPPORTED :
						    RES_UNKNOWN_ME;
		break;
	default:
		rc[0] = RES_NOT_SUPPORTED;
		break;
	}

	/* download sections without AR get no answer */
	if (msg[2] & OMCI_MT_AR)
		send_resp(msg, rsp);
	return;

bad:
	INFO("OMCI rx: malformed or unsupported frame, len %zu", len);
	stats.errors++;
}

int omci_onu_id = -1;

/* GPON O-state and ONU-ID from the kernel */
/*
 * Type B protection (G.984.3): the light is lost in O5 and the kernel goes to
 * O6 with the ONU-ID, the T-CONTs, the GEM ports, the key and the equalization
 * delay kept (an7552_mac_gpon.c gpon_loss_of_light()).  The protecting OLT
 * port brings the ONU back with a Popup: directed, straight to O5, or
 * broadcast, through O4 and ranging.  Nothing was deactivated, so nothing is
 * programmed again: what the daemon does after a real deactivation (the
 * replay, the delayed reload, a new OLT that may not know the extended set)
 * would only disturb the traffic that is about to come back.  A TO2 that runs
 * out takes the kernel to O1 and drops everything: that is a deactivation.
 */
static bool typeb_hold;
static unsigned int typeb_switches;

/* no answer of the old OLT is repeated to a new one that reuses a TCI */
static void cache_reset(void)
{
	memset(last, 0, sizeof(last));
}

void omci_state(uint8_t st, int onu_id)
{
	struct omci_inst *t0;
	uint8_t prev = onu_state;

	if (onu_id >= 0 && onu_id != 0xff)
		omci_onu_id = onu_id;
	if (st == onu_state)
		return;
	onu_state = st;

	if (st == 6 && prev == 5) {
		typeb_hold = true;
		cache_reset();
		INFO("OMCI: light lost in O5, Type B: waiting for the protecting OLT");
		return;
	}
	if (typeb_hold && (st == 4 || st == 6))
		return;				/* Popup, ranging */
	if (st != 5) {
		olt_spoke = false;
		typeb_hold = false;
		cfg_done_sent = false;
		uloop_timeout_cancel(&cfg_done_tmo);
		cache_reset();
		/* the next OLT may not know the extended set */
		olt_ext = false;
		test_ext = false;
		return;
	}
	if (typeb_hold) {
		typeb_hold = false;
		typeb_switches++;
		INFO("OMCI: back in O5 after Type B protection (%u), the "
		     "configuration was kept", typeb_switches);
		return;
	}

	/* omciInternalCreateForTcont(): the first T-CONT shows the ONU-ID
	 * alloc (OMCC, owned by PLOAM in the kernel) once ranged */
	t0 = mib_find(262, 0x8000);
	if (t0 && omci_onu_id >= 0 && !(t0->olt_mask & omci_mask(1)))
		mib_set_uint(t0, 1, omci_onu_id);

	/*
	 * The kernel drops every T-CONT and GEM port with each deactivation
	 * while the MIB keeps what the OLT (or /usr/lib/gpon/gpon.json) put there, so the
	 * kernel is programmed from it whenever O5 is reached.  A locked ONU
	 * has no other source.  The kernel resizes the TX buffers once the
	 * configuration settled; nobody calls cfg_changed() here, so that is
	 * armed by hand.
	 */
	if (bind_replay()) {
		eth_changed();
		if (!cfg_done_sent && cfg.cfg_done_s > 0)
			uloop_timeout_set(&cfg_done_tmo, cfg.cfg_done_s * 1000);
	}
}

void omci_init(void)
{
	cfg_done_tmo.cb = cfg_done_cb;
	reboot_tmo.cb = reboot_cb;
	mib_reset();
	omci_ext_init();
	omci_pm_init();
	mibstore_load();
}

void omci_stats(struct blob_buf *b)
{
	struct omci_inst *in;
	unsigned int n = 0;

	list_for_each_entry(in, &omci_mib, list)
		n++;

	blobmsg_add_u64(b, "rx", stats.rx);
	blobmsg_add_u64(b, "tx", stats.tx);
	blobmsg_add_u64(b, "errors", stats.errors);
	blobmsg_add_u64(b, "retransmissions", stats.retrans);
	blobmsg_add_u64(b, "rx_extended", stats.rx_ext);
	blobmsg_add_u64(b, "rx_ak", stats.rx_ak);
	blobmsg_add_u8(b, "olt_extended", olt_ext);
	blobmsg_add_u32(b, "instances", n);
	blobmsg_add_u32(b, "mib_data_sync",
			mib_find(2, 0) ? mib_get_uint(mib_find(2, 0), 1) : 0);
	blobmsg_add_u8(b, "cfg_done", cfg_done_sent);
	blobmsg_add_u8(b, "mib_dirty", mibstore_dirty());
	blobmsg_add_u8(b, "protection_hold", typeb_hold);
	blobmsg_add_u32(b, "protection_switches", typeb_switches);
	omci_alarm_dump(b);
}

/*
 * UCI values the MIB mirrors changed (reload, SIGHUP): an ONU
 * side change, so AVC-capable ones are announced (ONU-G logical ONU ID and
 * password, ONU2-G OMCC version: SDK avcId column).
 */
void omci_config_changed(void)
{
	static const uint16_t ch_ids[] = { 0x0101, 0x010a, 0x0180 };
	struct omci_inst *in;
	int i;

	for (i = 0; i < 3; i++) {
		in = mib_find(5, ch_ids[i]);
		if (in) {
			mib_set_str(in, 4, cfg.equipment_id);
			mib_set_str(in, 5, cfg.equipment_id);
		}
	}
	in = mib_find(6, 0x0101);
	if (in) {
		if (cfg.sn_set)
			mib_set_raw(in, 3, cfg.sn, GPON_SN_LEN);
		mib_set_str(in, 4, cfg.version);
		mib_set_str(in, 5, cfg.vendor_id);
		mib_set_str(in, 9, cfg.equipment_id);
	}
	for (i = 0; i < 2; i++) {
		in = mib_find(7, i);
		if (!in)
			continue;
		mib_set_str(in, 1, cfg.sw_ver_slot[i]);
		mib_set_uint(in, 2, cfg.sw_committed == i);
		mib_set_uint(in, 3, cfg.sw_active == i);
		mib_set_uint(in, 4, cfg.sw_valid[i]);
	}
	in = mib_find(256, 0);
	if (in) {
		omci_onu_set_str(in, 10, cfg.loid);
		omci_onu_set_str(in, 11, cfg.lopw);
	}
	in = mib_find(257, 0);
	if (in)
		omci_onu_set_uint(in, 2, cfg.omcc_version);
	if (onu_state == 5)
		eth_changed();
}

void omci_dump(struct blob_buf *b)
{
	struct omci_inst *in;
	char hex[2 * 64 + 1];
	void *a, *t, *v;
	unsigned int i, j;

	a = blobmsg_open_array(b, "mib");
	list_for_each_entry(in, &omci_mib, list) {
		if (in->pm)
			omci_pm_refresh(in, false);
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_u32(b, "class", in->def->class_id);
		blobmsg_add_string(b, "name", in->def->name);
		blobmsg_add_u32(b, "instance", in->id);
		blobmsg_add_u8(b, "olt", in->by_olt);
		v = blobmsg_open_table(b, "attributes");
		for (i = 0; i < in->def->nattr; i++) {
			const struct omci_attr_def *ad = &in->def->attr[i];
			unsigned int n = in->len[ad->idx];

			if (!in->val[ad->idx])
				continue;
			if (n > 64)
				n = 64;
			for (j = 0; j < n; j++)
				sprintf(hex + 2 * j, "%02x", in->val[ad->idx][j]);
			hex[2 * n] = 0;
			blobmsg_add_string(b, ad->name, hex);
		}
		blobmsg_close_table(b, v);
		omci_alarm_inst_dump(b, in);
		omci_pm_inst_dump(b, in);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);
}
