// SPDX-License-Identifier: GPL-2.0
/*
 * Sources of OMCI alarms and ONU-side attribute changes.
 *
 * Only clean interfaces are used: the kernel alarm bitmap of the GET_STATE
 * events, the hwmon device of the EN7571 (sysfs, read only) and netifd
 * (ubus network.device status).  Polled once per second.  Without the
 * hwmon device or netifd the alarms of that source stay as they are; an
 * alarm attribute the driver does not have is a condition it does not
 * detect (no alarm).
 *
 *   source                              ME / alarm or attribute
 *   kernel GPON_ALM_SF                  ANI-G alarm 2 SF
 *   kernel GPON_ALM_SD                  ANI-G alarm 3 SD
 *   hwmon power2 (rx)                   ANI-G attr 10 optical signal level,
 *                                       alarms 0/1 low/high received power
 *   hwmon power1 (tx)                   ANI-G attr 14 transmit optical level,
 *                                       alarms 4/5 low/high transmit power
 *   hwmon curr1 (bias) max/crit alarm   ANI-G alarm 6 laser bias current
 *   hwmon temp1 min/max, lcrit/crit     ONU-G alarms 8/9 temperature
 *                                       yellow/red
 *   hwmon in0 min/max, lcrit/crit       ONU-G alarms 10/11 voltage
 *                                       yellow/red
 *   netifd carrier of onu.uni_dev[n]    PPTP Ethernet UNI 0x101+n alarm 0
 *                                       LAN-LOS, attr 6 operational state
 *                                       (AVC), attr 7 configuration ind
 *
 * Alarm numbers: SDK meMapTableAniG / meMapTableOnuG /
 * alarmIdTablePPTPEthernetUNI (OMCI_ALARM_ID_* = class << 16 | number).
 * The kernel LOS/LOF/... alarms have no ANI-G or ONU-G alarm (the OLT
 * detects those itself).
 *
 * Optical thresholds (G.988 ANI-G, the same decoding as the SDK
 * gponAlarmHandle()): lower/upper optical threshold (attr 11/12) in
 * -0.5 dB steps from 0 dBm, 0xff = ONU internal policy; lower/upper
 * transmit power threshold (attr 15/16) 2's complement 0.5 dB steps,
 * 0x81 = internal policy.  An explicit threshold is compared with the
 * measured level here, the internal policy is the alarm flag the EN7571
 * driver raises against its own thresholds (min/lcrit: low, max/crit:
 * high).  The SDK reports nothing at all with the internal policy.
 * Levels (attr 10/14): 2's complement, 0.002 dB steps from 1 mW, as the
 * SDK getOpticalSignalLevelValue() writes them; no light (0 uW) is the
 * lowest value, 0x8000.
 */
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <netlink/attr.h>

#include "gpond.h"
#include "omci.h"

#define MON_POLL_MS		1000

/* ANI-G alarms (meMapTableAniG) */
#define ANI_ALM_LOW_RX		0
#define ANI_ALM_HIGH_RX		1
#define ANI_ALM_SF		2
#define ANI_ALM_SD		3
#define ANI_ALM_LOW_TX		4
#define ANI_ALM_HIGH_TX		5
#define ANI_ALM_BIAS		6
/* ONU-G alarms (meMapTableOnuG) */
#define ONU_ALM_TEMP_YELLOW	8
#define ONU_ALM_TEMP_RED	9
#define ONU_ALM_VOLT_YELLOW	10
#define ONU_ALM_VOLT_RED	11
/* PPTP Ethernet UNI alarms (alarmIdTablePPTPEthernetUNI) */
#define PPTP_ALM_LAN_LOS	0

/* ANI-G attributes */
#define ANI_RX_LEVEL		10
#define ANI_RX_LOWER		11
#define ANI_RX_UPPER		12
#define ANI_TX_LEVEL		14
#define ANI_TX_LOWER		15
#define ANI_TX_UPPER		16
#define ANI_RX_POLICY		0xff
#define ANI_TX_POLICY		0x81
#define LEVEL_PER_HALF_DB	250	/* 0.5 dB in 0.002 dB steps */

/* PPTP Ethernet UNI attributes; operational state 0 enabled, 1 disabled
 * (SDK lan_port_alarm_func(): ETH_LINK_UP 0, ETH_LINK_DOWN 1) */
#define PPTP_OP_STATE		6
#define PPTP_CFG_IND		7

static struct monitor_state mon;

static struct uloop_timeout poll_tmo;

static const char * const low_flags[] = { "min_alarm", "lcrit_alarm", NULL };
static const char * const high_flags[] = { "max_alarm", "crit_alarm", NULL };
static const char * const warn_flags[] = { "min_alarm", "max_alarm", NULL };
static const char * const crit_flags[] = { "lcrit_alarm", "crit_alarm", NULL };

/* ---- inputs ---- */

static int read_long(const char *dir, const char *attr, long *v)
{
	char path[HWMON_ATTR_PATH_MAX];
	FILE *f;
	int ret;

	snprintf(path, sizeof(path), "%s/%s", dir, attr);
	f = fopen(path, "r");
	if (!f)
		return -1;
	ret = fscanf(f, "%ld", v) == 1 ? 0 : -1;
	fclose(f);

	return ret;
}

static void hwmon_find(void)
{
	char path[HWMON_ATTR_PATH_MAX], name[32];
	struct dirent *e;
	DIR *d;
	FILE *f;

	mon.hwmon[0] = 0;
	d = opendir(GPOND_HWMON_DIR);
	if (!d)
		return;
	while ((e = readdir(d))) {
		if (strncmp(e->d_name, "hwmon", 5))
			continue;
		snprintf(path, sizeof(path), "%s/%s/name", GPOND_HWMON_DIR,
			 e->d_name);
		f = fopen(path, "r");
		if (!f)
			continue;
		if (fgets(name, sizeof(name), f) &&
		    !strcmp(strtok(name, "\n") ?: "", EN7571_HWMON_NAME))
			snprintf(mon.hwmon, sizeof(mon.hwmon), "%s/%s",
				 GPOND_HWMON_DIR, e->d_name);
		fclose(f);
		if (mon.hwmon[0]) {
			INFO("monitor: optical module at %s", mon.hwmon);
			break;
		}
	}
	closedir(d);
}

/* any of <chan>_<flag> set: C_ON; otherwise C_OFF, also when the driver
 * has none of them (it does not detect that condition) */
static int hw_flag(const char *chan, const char * const *flags)
{
	char attr[48];
	long v;

	for (; *flags; flags++) {
		snprintf(attr, sizeof(attr), "%s_%s", chan, *flags);
		if (!read_long(mon.hwmon, attr, &v) && v)
			return C_ON;
	}

	return C_OFF;
}

/*
 * Integer 5000 * log10(x) for x >= 1, using log10(2^k * (1 + f)):
 *   5000 * log10(2) = 98641689 / 65536
 *   mant_tab[i] = round(5000 * log10(1 + i/32) * 16) for i = 0..32
 */
static int32_t ilog10_5000(uint32_t x)
{
	static const uint16_t mant_tab[33] = {
		    0,  1069,  2106,  3113,  4092,  5044,  5971,  6874,
		 7755,  8614,  9453, 10273, 11074, 11858, 12625, 13376,
		14112, 14833, 15540, 16233, 16914, 17582, 18238, 18883,
		19517, 20140, 20752, 21355, 21948, 22532, 23107, 23673,
		24082,
	};
	unsigned int k = 31 - __builtin_clz(x);
	uint32_t norm = x << (31 - k);		/* bit 31 = 1 */
	unsigned int idx = (norm >> 26) & 31;	/* top 5 fractional bits */
	uint32_t rem = (norm >> 10) & 0xffff;	/* next 16 fractional bits */
	uint32_t m0 = mant_tab[idx], m1 = mant_tab[idx + 1];
	uint32_t mant16 = m0 + (((m1 - m0) * rem + 0x8000) >> 16);

	return (int32_t)(((uint64_t)k * 98641689ULL + 32768ULL) >> 16) +
	       (int32_t)((mant16 + 8) >> 4);
}

/* uW -> 0.002 dB steps from 1 mW, 2's complement 16 bit */
static int16_t uw_level(long uw)
{
	int32_t l;

	if (uw <= 0)
		return INT16_MIN;
	l = ilog10_5000((uint32_t)uw) - 15000;
	if (l < INT16_MIN)
		return INT16_MIN;
	if (l > INT16_MAX)
		return INT16_MAX;

	return (int16_t)l;
}

static void hwmon_read(void)
{
	bool had = mon.hwmon[0];
	long v;

	if (!had || read_long(mon.hwmon, "power1_input", &v)) {
		/* not there (yet), or the driver went away */
		hwmon_find();
		if (had && !mon.hwmon[0])
			WARN("monitor: optical module hwmon gone");
	}

	mon.tx_valid = mon.hwmon[0] && !read_long(mon.hwmon, "power1_input",
						  &mon.tx_uw);
	mon.rx_valid = mon.hwmon[0] && !read_long(mon.hwmon, "power2_input",
						  &mon.rx_uw);
	if (mon.tx_valid)
		mon.tx_level = uw_level(mon.tx_uw);
	if (mon.rx_valid)
		mon.rx_level = uw_level(mon.rx_uw);

	if (!mon.hwmon[0]) {
		mon.rx_low = mon.rx_high = mon.tx_low = mon.tx_high = C_UNKNOWN;
		mon.bias = C_UNKNOWN;
		mon.temp_warn = mon.temp_crit = C_UNKNOWN;
		mon.volt_warn = mon.volt_crit = C_UNKNOWN;
		return;
	}
	mon.tx_low = hw_flag("power1", low_flags);
	mon.tx_high = hw_flag("power1", high_flags);
	mon.rx_low = hw_flag("power2", low_flags);
	mon.rx_high = hw_flag("power2", high_flags);
	mon.bias = hw_flag("curr1", high_flags);
	mon.temp_warn = hw_flag("temp1", warn_flags);
	mon.temp_crit = hw_flag("temp1", crit_flags);
	mon.volt_warn = hw_flag("in0", warn_flags);
	mon.volt_crit = hw_flag("in0", crit_flags);
}

static void uni_read(void)
{
	struct gpon_link l;
	int n;

	for (n = 0; n < GPON_MAX_UNI; n++) {
		if (n >= cfg.n_uni_dev || !cfg.uni_dev[n][0]) {
			mon.uni[n].valid = false;
			continue;
		}
		/* netifd not reachable: keep what was known */
		if (ubus_device_status(cfg.uni_dev[n], &l))
			continue;
		mon.uni[n].l = l;
		mon.uni[n].valid = true;
	}
}

/* ---- MIB ---- */

static void set_alarm(struct omci_inst *in, uint8_t num, int cond,
		      bool notify)
{
	if (in && cond != C_UNKNOWN)
		omci_alarm_set(in->def->class_id, in->id, num, cond == C_ON,
			       notify);
}

/* an ONU-side value: AVC (if capable) unless resynchronising */
static void attr_set(struct omci_inst *in, uint8_t idx, uint32_t v,
		     bool notify)
{
	if (notify)
		omci_onu_set_uint(in, idx, v);
	else
		mib_set_uint(in, idx, v);
}

static void apply_gpon(bool notify)
{
	struct omci_inst *ani = mib_first(CLASS_ANI_G);

	if (!ani || !mon.gpon_valid)
		return;
	set_alarm(ani, ANI_ALM_SF, !!(mon.gpon & (1u << GPON_ALM_SF)),
		  notify);
	set_alarm(ani, ANI_ALM_SD, !!(mon.gpon & (1u << GPON_ALM_SD)),
		  notify);
}

/* a < b for a measured level, unknown without a measurement */
static int lt(bool valid, int a, int b)
{
	return valid ? (a < b ? C_ON : C_OFF) : C_UNKNOWN;
}

static void apply_optical(bool notify)
{
	struct omci_inst *ani = mib_first(CLASS_ANI_G);
	int low, high;
	uint32_t thr;

	if (!ani)
		return;
	if (mon.rx_valid)
		attr_set(ani, ANI_RX_LEVEL, (uint16_t)mon.rx_level, notify);
	if (mon.tx_valid)
		attr_set(ani, ANI_TX_LEVEL, (uint16_t)mon.tx_level, notify);

	/* received power: -thr * 0.5 dBm */
	thr = mib_get_uint(ani, ANI_RX_LOWER);
	low = thr == ANI_RX_POLICY ? mon.rx_low :
	      lt(mon.rx_valid, mon.rx_level, -(int)thr * LEVEL_PER_HALF_DB);
	thr = mib_get_uint(ani, ANI_RX_UPPER);
	high = thr == ANI_RX_POLICY ? mon.rx_high :
	       lt(mon.rx_valid, -(int)thr * LEVEL_PER_HALF_DB, mon.rx_level);
	if (high == C_ON)		/* gponAlarmHandle(): one at a time */
		low = C_OFF;
	set_alarm(ani, ANI_ALM_LOW_RX, low, notify);
	set_alarm(ani, ANI_ALM_HIGH_RX, high, notify);

	/* transmit power: (int8)thr * 0.5 dBm */
	thr = mib_get_uint(ani, ANI_TX_LOWER);
	low = thr == ANI_TX_POLICY ? mon.tx_low :
	      lt(mon.tx_valid, mon.tx_level, (int8_t)thr * LEVEL_PER_HALF_DB);
	thr = mib_get_uint(ani, ANI_TX_UPPER);
	high = thr == ANI_TX_POLICY ? mon.tx_high :
	       lt(mon.tx_valid, (int8_t)thr * LEVEL_PER_HALF_DB, mon.tx_level);
	if (high == C_ON)
		low = C_OFF;
	set_alarm(ani, ANI_ALM_LOW_TX, low, notify);
	set_alarm(ani, ANI_ALM_HIGH_TX, high, notify);

	/* G.988: bias above the vendor's threshold (laser end of life) */
	set_alarm(ani, ANI_ALM_BIAS, mon.bias, notify);
}

static void apply_onu(bool notify)
{
	struct omci_inst *onu = mib_find(CLASS_ONU_G, 0);

	set_alarm(onu, ONU_ALM_TEMP_YELLOW, mon.temp_warn, notify);
	set_alarm(onu, ONU_ALM_TEMP_RED, mon.temp_crit, notify);
	set_alarm(onu, ONU_ALM_VOLT_YELLOW, mon.volt_warn, notify);
	set_alarm(onu, ONU_ALM_VOLT_RED, mon.volt_crit, notify);
}

/*
 * PPTP Ethernet UNI configuration ind (G.988 9.5.1):
 * 0x01/0x02/0x03/0x04/0x05/0x06 10/100/1000/10G/2.5G/5GBASE-T full duplex,
 * 0x11/0x12/0x13 10/100/1000BASE-T half duplex, 0 no link or anything else.
 */
static uint8_t cfg_ind(const struct gpon_link *l)
{
	uint8_t c;

	if (!l->carrier)
		return 0;
	switch (l->speed) {
	case 10:
		c = 0x01;
		break;
	case 100:
		c = 0x02;
		break;
	case 1000:
		c = 0x03;
		break;
	case 2500:
		return 0x05;
	case 5000:
		return 0x06;
	case 10000:
		return 0x04;
	default:
		return 0;
	}

	return l->full_duplex ? c : c | 0x10;
}

static void apply_uni(bool notify)
{
	struct omci_inst *in;
	int n;

	for (n = 0; n < GPON_MAX_UNI; n++) {
		const struct gpon_link *l = &mon.uni[n].l;

		if (!mon.uni[n].valid)
			continue;
		in = mib_find(CLASS_PPTP_ETH, PPTP_ID(n));
		if (!in)
			continue;
		/* lan_port_alarm_func(): alarm, then the op state AVC */
		set_alarm(in, PPTP_ALM_LAN_LOS, l->carrier ? C_OFF : C_ON,
			  notify);
		attr_set(in, PPTP_OP_STATE, !l->carrier, notify);
		attr_set(in, PPTP_CFG_IND, cfg_ind(l), notify);
	}
}

static void poll_cb(struct uloop_timeout *t)
{
	uloop_timeout_set(t, MON_POLL_MS);

	hwmon_read();
	uni_read();
	apply_optical(true);
	apply_onu(true);
	apply_uni(true);
}

/* ---- interface ---- */

/* the kernel alarm bitmap of a GET_STATE event or reply */
void monitor_gpon_alarms(uint32_t alarms)
{
	mon.gpon = alarms;
	mon.gpon_valid = true;
	apply_gpon(true);
}

/* after MIB reset: the conditions known are active in the new MIB too,
 * without reports (the OLT asks with get all alarms) */
void monitor_resync(void)
{
	apply_gpon(false);
	apply_optical(false);
	apply_onu(false);
	apply_uni(false);
}

/* one EN7571 hwmon attribute (OMCI tests); -1 without the device */
int monitor_hwmon_read(const char *attr, long *v)
{
	if (!mon.hwmon[0])
		hwmon_find();

	return mon.hwmon[0] ? read_long(mon.hwmon, attr, v) : -1;
}

static void state_cb(struct nlattr **tb, void *arg)
{
	if (!tb[GPON_ATTR_STATE])
		return;
	omci_state(nla_get_u8(tb[GPON_ATTR_STATE]),
		   tb[GPON_ATTR_ONU_ID] ?
		   nla_get_u8(tb[GPON_ATTR_ONU_ID]) : -1);
	if (tb[GPON_ATTR_ALARMS])
		monitor_gpon_alarms(nla_get_u32(tb[GPON_ATTR_ALARMS]));
}

void monitor_init(void)
{
	int n;

	mon.rx_low = mon.rx_high = mon.tx_low = mon.tx_high = C_UNKNOWN;
	mon.bias = C_UNKNOWN;
	mon.temp_warn = mon.temp_crit = C_UNKNOWN;
	mon.volt_warn = mon.volt_crit = C_UNKNOWN;
	for (n = 0; n < GPON_MAX_UNI; n++)
		mon.uni[n].valid = false;

	nl_request(nl_msg(GPON_CMD_GET_STATE, false), state_cb, NULL);
	poll_tmo.cb = poll_cb;
	uloop_timeout_set(&poll_tmo, MON_POLL_MS);
}

static const char * const gpon_alarm_name[] = {
	[GPON_ALM_LOS] = "LOS", [GPON_ALM_LOF] = "LOF",
	[GPON_ALM_LCDG] = "LCDG", [GPON_ALM_SUF] = "SUF",
	[GPON_ALM_DACT] = "DACT", [GPON_ALM_DIS] = "DIS",
	[GPON_ALM_MEM] = "MEM", [GPON_ALM_PEE] = "PEE",
	[GPON_ALM_MIS] = "MIS", [GPON_ALM_SD] = "SD",
	[GPON_ALM_SF] = "SF", [GPON_ALM_RDI] = "RDI",
	[GPON_ALM_NOPWD] = "NOPWD",
};

static void dump_cond(struct blob_buf *b, const char *name, int c)
{
	if (c != C_UNKNOWN)
		blobmsg_add_u8(b, name, c == C_ON);
}

void monitor_dump(struct blob_buf *b)
{
	void *t, *a;
	unsigned int i;
	int n;

	if (mon.gpon_valid) {
		a = blobmsg_open_array(b, "kernel");
		for (i = 0; i < ARRAY_SIZE(gpon_alarm_name); i++)
			if (gpon_alarm_name[i] && (mon.gpon & (1u << i)))
				blobmsg_add_string(b, NULL, gpon_alarm_name[i]);
		blobmsg_close_array(b, a);
	}

	t = blobmsg_open_table(b, "optical");
	blobmsg_add_string(b, "hwmon", mon.hwmon);
	if (mon.rx_valid) {
		int32_t md = (int32_t)mon.rx_level * 2;
		char dbm[24];

		snprintf(dbm, sizeof(dbm), "%s%d.%03d", md < 0 ? "-" : "",
			 (md < 0 ? -md : md) / 1000,
			 (md < 0 ? -md : md) % 1000);
		blobmsg_add_u32(b, "rx_power_uw", mon.rx_uw);
		blobmsg_add_string(b, "rx_dbm", dbm);
	}
	if (mon.tx_valid) {
		int32_t md = (int32_t)mon.tx_level * 2;
		char dbm[24];

		snprintf(dbm, sizeof(dbm), "%s%d.%03d", md < 0 ? "-" : "",
			 (md < 0 ? -md : md) / 1000,
			 (md < 0 ? -md : md) % 1000);
		blobmsg_add_u32(b, "tx_power_uw", mon.tx_uw);
		blobmsg_add_string(b, "tx_dbm", dbm);
	}
	dump_cond(b, "rx_low", mon.rx_low);
	dump_cond(b, "rx_high", mon.rx_high);
	dump_cond(b, "tx_low", mon.tx_low);
	dump_cond(b, "tx_high", mon.tx_high);
	dump_cond(b, "bias_high", mon.bias);
	dump_cond(b, "temp_warn", mon.temp_warn);
	dump_cond(b, "temp_crit", mon.temp_crit);
	dump_cond(b, "volt_warn", mon.volt_warn);
	dump_cond(b, "volt_crit", mon.volt_crit);
	blobmsg_close_table(b, t);

	a = blobmsg_open_array(b, "uni");
	for (n = 0; n < GPON_MAX_UNI; n++) {
		if (!mon.uni[n].valid)
			continue;
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_string(b, "device", cfg.uni_dev[n]);
		blobmsg_add_u32(b, "instance", PPTP_ID(n));
		blobmsg_add_u8(b, "present", mon.uni[n].l.present);
		blobmsg_add_u8(b, "carrier", mon.uni[n].l.carrier);
		if (mon.uni[n].l.speed)
			blobmsg_add_u32(b, "speed", mon.uni[n].l.speed);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);
}
