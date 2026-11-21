// SPDX-License-Identifier: GPL-2.0
/*
 * OMCI Test (MT 18) and Test result (MT 27) for ONU-G (256) and ANI-G (263)
 * optical self-test measurements via the EN7571 hwmon device.
 */
#include <stdlib.h>
#include <string.h>

#include "gpond.h"
#include "omci.h"

#define SELF_TEST_MIN		7

static void result_cb(struct uloop_timeout *t)
{
	struct test_result *r = container_of(t, struct test_result, tmo);

	omci_onu_msg_tx(r->tci, MT_TEST_RESULT, r->class_id, r->id, r->c);
	free(r);
}

static void result_queue(uint16_t tci, uint16_t class_id, uint16_t id,
			 const uint8_t *c)
{
	struct test_result *r = calloc(1, sizeof(*r));

	if (!r)
		return;
	r->tci = tci;
	r->class_id = class_id;
	r->id = id;
	memcpy(r->c, c, OMCI_CONTENTS);
	r->tmo.cb = result_cb;
	uloop_timeout_set(&r->tmo, 0);
}

static uint16_t uw_dbuw(long uw)
{
	static const uint16_t mant_tab[33] = {
		    0,  1069,  2106,  3113,  4092,  5044,  5971,  6874,
		 7755,  8614,  9453, 10273, 11074, 11858, 12625, 13376,
		14112, 14833, 15540, 16233, 16914, 17582, 18238, 18883,
		19517, 20140, 20752, 21355, 21948, 22532, 23107, 23673,
		24082,
	};
	uint32_t x, norm, rem, m0, m1, mant16;
	unsigned int k, idx;
	int32_t l;

	if (uw <= 0)
		return 0x8000;
	x = (uint32_t)uw;
	k = 31 - __builtin_clz(x);
	norm = x << (31 - k);
	idx = (norm >> 26) & 31;
	rem = (norm >> 10) & 0xffff;
	m0 = mant_tab[idx];
	m1 = mant_tab[idx + 1];
	mant16 = m0 + (((m1 - m0) * rem + 0x8000) >> 16);
	l = (int32_t)(((uint64_t)k * 98641689ULL + 32768ULL) >> 16) +
	    (int32_t)((mant16 + 8) >> 4);

	return (uint16_t)(int16_t)(l > INT16_MAX ? INT16_MAX : l);
}

static void triplet(uint8_t *p, uint8_t type, const char *attr,
		    uint16_t (*conv)(long))
{
	long v;

	if (monitor_hwmon_read(attr, &v))
		return;
	p[0] = type;
	wr_be16(p + 1, conv(v));
}

static uint16_t mv_20mv(long v)
{
	return (uint16_t)(v / 20);
}

static uint16_t ma_2ua(long v)
{
	return (uint16_t)(v * 500);
}

static uint16_t mc_256th(long v)
{
	return (uint16_t)(int16_t)(v * 256 / 1000);
}

uint8_t omci_test(uint16_t tci, struct omci_inst *in, const uint8_t *req,
		  const uint8_t *c)
{
	uint8_t r[OMCI_CONTENTS] = { 0 };

	if (in->def->class_id != CLASS_ONU_G &&
	    in->def->class_id != CLASS_ANI_G)
		return RES_NOT_SUPPORTED;
	if (c[0] < SELF_TEST_MIN)
		return RES_NOT_SUPPORTED;

	triplet(r, 1, "in0_input", mv_20mv);
	triplet(r + 3, 3, "power2_input", uw_dbuw);
	triplet(r + 6, 5, "power1_input", uw_dbuw);
	triplet(r + 9, 9, "curr1_input", ma_2ua);
	triplet(r + 12, 12, "temp1_input", mc_256th);
	INFO("OMCI: %s 0x%04x self test %u", in->def->name, in->id, c[0]);
	result_queue(tci, in->def->class_id, in->id, r);

	return RES_OK;
}
