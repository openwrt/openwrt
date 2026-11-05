// SPDX-License-Identifier: GPL-2.0
/*
 * OMCI alarms, attribute value changes and alarm reporting control.
 *
 * Every instance keeps the bitmap of its active alarms.  A change is sent
 * as an Alarm message (MT 16, no AR, TCI 0): contents bytes 0..27 the
 * whole bitmap of the instance, byte 31 the alarm sequence number.  That
 * layout and the rules below follow the SDK OMCI stack (omci.o of the
 * AN7552 BSP, only shipped as an object: omciSendAlarmMsg(),
 * omciAlarmTcaMsgReport_unlock(), omciSetAlarmReportControl(),
 * omciArcTmOut(), omciAvcMsgHandle(), omciSendAvcBasicMsg(),
 * omci_general_func.o omciGetAllAlarmsAction()/omciGetAllAlarmsNextAction())
 * and G.988:
 *   - the sequence number is incremented for every Alarm message sent,
 *     wraps from 255 to 1 (0 only right after a reset) and is reset by
 *     get all alarms and by MIB reset;
 *   - an alarm of an instance whose "admin state" is locked (1) or whose
 *     ARC is active is recorded but not reported;
 *   - ARC (alarm reporting control): setting ARC to 1 inhibits the
 *     reports for "ARC interval" minutes (255: until ARC is set to 0,
 *     0: ends at once); when the interval ends the ONU sets ARC back to 0
 *     and sends an AVC for it;
 *   - get all alarms (MT 11) takes a snapshot of every instance with an
 *     active alarm (retrieval mode 1: only those without ARC) and answers
 *     with the number of get all alarms next (MT 12) commands; each next
 *     returns class, instance and bitmap of one of them, out of range: all
 *     zero.  (Unlike the SDK, which also clears the bitmaps it reported,
 *     the alarms stay active until their source clears them.)
 *   - AVC (MT 17, no AR, TCI 0): attribute mask and value of one
 *     AVC-capable attribute (SDK avcId column) that the ONU changed by
 *     itself; nothing is sent for changes made by an OLT Set, or while
 *     the instance is locked.
 *   - threshold crossing alerts of the PM MEs (omci_pm.c) are alarms of
 *     the PM instance like any other: the alarm numbers come from the PM
 *     table, they are reported, held back while the parent UNI is locked
 *     and part of get all alarms; the PM engine clears them when the
 *     15 minute interval ends.
 * Nothing is sent before the ONU is in O5 and the OLT has sent its first
 * OMCI message there (omci_notify_ok()); the OLT resynchronises with
 * get all alarms / MIB upload anyway.
 */
#include <stdlib.h>
#include <string.h>

#include "gpond.h"
#include "omci.h"
#include "omci_pm.h"

static struct alarm_snap snap;

static uint8_t alarm_seq;		/* of the last Alarm message sent */
static uint64_t alarm_tx, avc_tx;

unsigned int omci_alarm_list(const struct omci_me_def *d, const uint8_t **l)
{
	if (d->nalarm) {
		*l = d->alarm;
		return d->nalarm;
	}

	return omci_pm_alarms(d, l);
}

static bool me_has_alarm(const struct omci_me_def *d, uint8_t num)
{
	const uint8_t *l = NULL;
	unsigned int i, n = omci_alarm_list(d, &l);

	for (i = 0; i < n; i++)
		if (l[i] == num)
			return true;

	return false;
}

static bool bitmap_empty(const uint8_t *bm)
{
	unsigned int i;

	for (i = 0; i < OMCI_ALARM_BYTES; i++)
		if (bm[i])
			return false;

	return true;
}

/* "admin state" 1: lock (G.988) - alarms and AVCs are not reported */
static bool inst_locked(struct omci_inst *in)
{
	return in->def->admin && mib_get_uint(in, in->def->admin) == 1;
}

static bool omci_alarm_get(struct omci_inst *in, uint8_t num)
{
	if (num >= OMCI_MAX_ALARM)
		return false;

	return in->alarm[num / 8] & (0x80 >> (num % 8));
}

static void alarm_report(struct omci_inst *in)
{
	uint8_t c[OMCI_CONTENTS] = { 0 };

	if (!omci_notify_ok() || in->arc_on || inst_locked(in))
		return;

	if (alarm_seq == 0xff)
		alarm_seq = 1;
	else
		alarm_seq++;
	memcpy(c, in->alarm, OMCI_ALARM_BYTES);
	c[31] = alarm_seq;
	alarm_tx++;
	omci_notify_tx(OMCI_MT_ALARM, in->def->class_id, in->id, c);
}

/*
 * Raise or clear alarm @num of an instance.  Returns 1 when the state
 * changed, 0 when it was already so, < 0 when the instance does not exist
 * or the ME has no such alarm.  @notify false only records the state (MIB
 * reset resynchronisation).
 */
int omci_alarm_set(uint16_t class_id, uint16_t id, uint8_t num, bool on,
		   bool notify)
{
	struct omci_inst *in = mib_find(class_id, id);
	uint8_t bit = 0x80 >> (num % 8);

	if (!in)
		return -1;
	if (!me_has_alarm(in->def, num))
		return -2;
	if (omci_alarm_get(in, num) == on)
		return 0;

	if (on)
		in->alarm[num / 8] |= bit;
	else
		in->alarm[num / 8] &= ~bit;
	INFO("OMCI: %s 0x%04x alarm %u %s", in->def->name, id, num,
	     on ? "raised" : "cleared");
	if (notify)
		alarm_report(in);

	return 1;
}

/*
 * The OLT unlocked @in ("admin state" 1 -> 0): the alarms that are active
 * now were recorded but not reported while it was locked, so the bitmap
 * goes out once (gpond policy; how the SDK behaves here is in its omci.o
 * object and was not checked).
 */
void omci_alarm_unlocked(struct omci_inst *in)
{
	if (!bitmap_empty(in->alarm))
		alarm_report(in);
}

/* ---- AVC ---- */

/* one AVC message per attribute of @mask, with its current value */
static void omci_avc(struct omci_inst *in, uint16_t mask)
{
	int idx;

	if (!omci_notify_ok() || inst_locked(in))
		return;

	for (idx = 1; idx <= OMCI_MAX_ATTR; idx++) {
		const struct omci_attr_def *a = omci_attr(in->def, idx);
		uint8_t c[OMCI_CONTENTS] = { 0 };

		if (!(mask & omci_mask(idx)) || !a ||
		    !(a->flags & OMCI_ATTR_AVC))
			continue;
		wr_be16(c, omci_mask(idx));
		/* a table only announces that it changed */
		if (a->format != OMCI_FMT_TABLE && in->val[idx] &&
		    in->len[idx] <= OMCI_CONTENTS - 2)
			memcpy(c + 2, in->val[idx], in->len[idx]);
		avc_tx++;
		omci_notify_tx(OMCI_MT_AVC, in->def->class_id, in->id, c);
	}
}

/*
 * A value the ONU changed by itself (not an OLT Set): stored, and an AVC
 * goes out when the attribute is AVC-capable.  1: changed, 0: unchanged.
 */
int omci_onu_set(struct omci_inst *in, uint8_t idx, const void *v,
		 uint16_t len)
{
	if (!in || !omci_attr(in->def, idx))
		return -1;
	if (in->val[idx] && in->len[idx] == len &&
	    !memcmp(in->val[idx], v, len))
		return 0;
	if (mib_set_raw(in, idx, v, len))
		return -1;
	omci_avc(in, omci_mask(idx));

	return 1;
}

/* big endian in the size of the attribute */
int omci_onu_set_uint(struct omci_inst *in, uint8_t idx, uint32_t v)
{
	uint8_t buf[4];
	int i, n;

	if (!in || !mib_val(in, idx))
		return -1;
	n = in->len[idx] < 4 ? in->len[idx] : 4;
	for (i = n - 1; i >= 0; i--, v >>= 8)
		buf[i] = v & 0xff;

	return omci_onu_set(in, idx, buf, n);
}

/* zero padded to the size of the attribute */
int omci_onu_set_str(struct omci_inst *in, uint8_t idx, const char *s)
{
	uint8_t buf[64] = { 0 };
	size_t l = strlen(s);

	if (!in || !mib_val(in, idx) || in->len[idx] > sizeof(buf))
		return -1;
	memcpy(buf, s, l < in->len[idx] ? l : in->len[idx]);

	return omci_onu_set(in, idx, buf, in->len[idx]);
}

/* ---- ARC ---- */

static void arc_end(struct uloop_timeout *t)
{
	struct omci_inst *in = container_of(t, struct omci_inst, arc_tmo);

	in->arc_on = false;
	INFO("OMCI: %s 0x%04x ARC interval over", in->def->name, in->id);
	omci_onu_set_uint(in, in->def->arc, 0);		/* + AVC */
}

/* the OLT set ARC or ARC interval of @in */
void omci_alarm_arc_set(struct omci_inst *in)
{
	const struct omci_me_def *d = in->def;
	uint32_t intv;

	if (!d->arc)
		return;

	uloop_timeout_cancel(&in->arc_tmo);
	in->arc_tmo.cb = arc_end;
	in->arc_on = mib_get_uint(in, d->arc) == 1;
	if (!in->arc_on)
		return;

	intv = d->arc_intv ? mib_get_uint(in, d->arc_intv) : 0;
	if (intv == 255)
		return;				/* until ARC is set to 0 */
	if (!intv)
		in->arc_on = false;		/* ends at once */
	uloop_timeout_set(&in->arc_tmo, intv * 60 * 1000);
}

void omci_alarm_inst_free(struct omci_inst *in)
{
	uloop_timeout_cancel(&in->arc_tmo);
}

/* ---- get all alarms ---- */

static void snap_free(void)
{
	free(snap.rows);
	snap.rows = NULL;
	snap.n = 0;
}

uint16_t omci_get_all_alarms(uint8_t mode)
{
	struct omci_inst *in;
	struct alarm_row *p;

	snap_free();
	alarm_seq = 0;

	list_for_each_entry(in, &omci_mib, list) {
		if (bitmap_empty(in->alarm))
			continue;
		/* mode 1: only instances without alarm reporting control */
		if ((mode & 1) && in->def->arc &&
		    mib_get_uint(in, in->def->arc) == 1)
			continue;
		if (snap.n == 0xffff)
			break;
		p = realloc(snap.rows, (snap.n + 1) * sizeof(*snap.rows));
		if (!p)
			break;
		snap.rows = p;
		p += snap.n++;
		p->class_id = in->def->class_id;
		p->id = in->id;
		memcpy(p->bitmap, in->alarm, OMCI_ALARM_BYTES);
	}

	return snap.n;
}

void omci_get_all_alarms_next(uint16_t seq, uint8_t *c)
{
	const struct alarm_row *r;

	if (seq >= snap.n)
		return;				/* all zero */
	r = &snap.rows[seq];
	wr_be16(c, r->class_id);
	wr_be16(c + 2, r->id);
	memcpy(c + 4, r->bitmap, OMCI_ALARM_BYTES);
}

/* after mib_reset(): the instances (and their ARC timers) are new */
void omci_alarm_mib_reset(void)
{
	snap_free();
	alarm_seq = 0;
}

/* ---- ubus ---- */

static void inst_alarms(struct blob_buf *b, struct omci_inst *in)
{
	void *a = blobmsg_open_array(b, "alarms");
	const uint8_t *l = NULL;
	unsigned int i, n = omci_alarm_list(in->def, &l);

	for (i = 0; i < n; i++)
		if (omci_alarm_get(in, l[i]))
			blobmsg_add_u32(b, NULL, l[i]);
	blobmsg_close_array(b, a);
}

/* alarms and ARC of one instance (ubus "mib") */
void omci_alarm_inst_dump(struct blob_buf *b, struct omci_inst *in)
{
	if (!bitmap_empty(in->alarm))
		inst_alarms(b, in);
	if (in->arc_on)
		blobmsg_add_u8(b, "arc", 1);
}

/* every instance with an active alarm (ubus "status" / "alarms") */
void omci_alarm_dump(struct blob_buf *b)
{
	struct omci_inst *in;
	void *a, *t;

	blobmsg_add_u32(b, "alarm_seq", alarm_seq);
	blobmsg_add_u64(b, "alarm_tx", alarm_tx);
	blobmsg_add_u64(b, "avc_tx", avc_tx);
	a = blobmsg_open_array(b, "active_alarms");
	list_for_each_entry(in, &omci_mib, list) {
		if (bitmap_empty(in->alarm))
			continue;
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_u32(b, "class", in->def->class_id);
		blobmsg_add_string(b, "name", in->def->name);
		blobmsg_add_u32(b, "instance", in->id);
		inst_alarms(b, in);
		blobmsg_add_u8(b, "reported", !in->arc_on && !inst_locked(in));
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);
}
