// SPDX-License-Identifier: GPL-2.0
/*
 * /etc/config/gpon - persistent configuration.
 *
 * One table describes every option: where it lives in UCI, how it is
 * parsed and where it goes in struct gpon_cfg.  The same table drives
 * loading and writing it back (software image state).
 */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <uci.h>

#include "gpond.h"

#define O(_s, _n, _t, _f) \
	{ _s, _n, _t, offsetof(struct gpon_cfg, _f), \
	  sizeof(((struct gpon_cfg *)0)->_f), (size_t)-1 }
#define OF(_s, _n, _t, _f, _flag) \
	{ _s, _n, _t, offsetof(struct gpon_cfg, _f), \
	  sizeof(((struct gpon_cfg *)0)->_f), \
	  offsetof(struct gpon_cfg, _flag) }

static const struct opt opts[] = {
	O("global", "enabled", T_BOOL, enabled),
	O("global", "own_config_only", T_BOOL, own_config_only),
	O("global", "mode", T_MODE, mode),
	O("global", "iface", T_STR, iface),
	O("global", "log_level", T_INT, log_level),
	OF("global", "sn", T_SN, sn, sn_set),
	OF("global", "password", T_PASSWD, passwd, passwd_set),

	O("gpon", "sd", T_INT, sd),
	O("gpon", "sf", T_INT, sf),
	O("gpon", "to1", T_INT, to1),
	O("gpon", "to2", T_INT, to2),
	O("gpon", "cfg_done", T_INT, cfg_done_s),

	O("onu", "vendor_id", T_STR, vendor_id),
	O("onu", "version", T_STR, version),
	O("onu", "equipment_id", T_STR, equipment_id),
	O("onu", "loid", T_STR, loid),
	O("onu", "lopw", T_STR, lopw),
	O("onu", "omcc_version", T_U8, omcc_version),
	O("onu", "product_code", T_U16, product_code),
	O("onu", "tconts", T_INT, tconts),
	OF("onu", "uni_ports", T_INT, uni_ports, uni_ports_set),
	O("onu", "sw_version0", T_STR, sw_ver_slot[0]),
	O("onu", "sw_version1", T_STR, sw_ver_slot[1]),
	O("onu", "sw_active", T_INT, sw_active),
	O("onu", "sw_committed", T_INT, sw_committed),
	O("onu", "sw_valid0", T_INT, sw_valid[0]),
	O("onu", "sw_valid1", T_INT, sw_valid[1]),
	O("onu", "sw_store", T_BOOL, sw_store),
	O("onu", "sw_dir", T_STR, sw_dir),
	O("onu", "reboot_mode", T_INT, reboot_mode),
	O("onu", "setup_cwmp", T_INT, setup_cwmp),
	O("onu", "uni_dev", T_UNI_LIST, uni_dev),
	O("onu", "veip_proto", T_STR, veip_proto),
	O("onu", "veip_ipv6", T_BOOL, veip_ipv6),
	O("onu", "ip_hosts", T_INT, ip_hosts),
	O("onu", "mcast_downlink", T_STR, mcast_downlink),
	O("onu", "mcast_gate", T_BOOL, mcast_gate),
};

static const struct gpon_mode_name modes[] = {
	{ "gpon", GPON_MODE_GPON },
	{ "gpon_sym", GPON_MODE_GPON_SYM },
};

const char *config_mode_name(int mode)
{
	size_t i;

	for (i = 0; i < sizeof(modes) / sizeof(modes[0]); i++)
		if (modes[i].mode == mode)
			return modes[i].name;

	return "unknown";
}

static void config_defaults(struct gpon_cfg *c)
{
	memset(c, 0, sizeof(*c));
	c->enabled = true;
	c->mode = GPON_MODE_GPON;
	strcpy(c->iface, "wan");
	c->log_level = LOG_INFO;
	c->cfg_done_s = 10;
	strcpy(c->vendor_id, "AIRO");
	strcpy(c->version, "AN7563");
	strcpy(c->equipment_id, "AN7563-ONU");
	c->omcc_version = 0xa0;		/* G.984.4 06/04 + amd 1/2 */
	c->tconts = 31;		/* romfile GPON_ANI TotalTCONTNum */
	c->uni_ports = 1;
	/* an HGU, as the stock build: the IP host config data are created
	 * by the ONU itself (SDK omciInitCapability() :622-660, onuType 2) */
	c->ip_hosts = 2;
	strcpy(c->veip_proto, "dhcp");
	c->sw_valid[0] = 1;
	c->sw_valid[1] = 1;
	strcpy(c->sw_dir, "/tmp");
	c->sw_store = false;
	c->reboot_mode = 0;
	c->setup_cwmp = 0;
	c->mcast_gate = true;
}

static int parse_hex(const char *s, uint8_t *out, size_t n)
{
	if (strlen(s) != n * 2)
		return -1;

	return hex_decode(s, out, n);
}

/*
 * "ABCD1234abcd" (vendor id + 8 hex digits) or 16 hex digits.
 *
 * The four letters are copied as they are, so they have to be letters: the
 * first four bytes go to G_VENDOR_ID as four characters and the rest are
 * eight hex digits in G_VS_SN, and an upper case letter only means anything
 * in the first four.  an7552_mac_set_identity_text() refuses anything else
 * for the same reason, and the two have to agree.
 */
static int parse_sn(const char *s, uint8_t *sn)
{
	if (strlen(s) == 12) {
		size_t i;

		for (i = 0; i < 4; i++)
			if (s[i] < 'A' || s[i] > 'Z')
				return -1;
		memcpy(sn, s, 4);
		return parse_hex(s + 4, sn + 4, 4);
	}

	return parse_hex(s, sn, GPON_SN_LEN);
}

/* 10 characters or 20 hex digits */
static int parse_passwd(const char *s, uint8_t *pw)
{
	size_t l = strlen(s);

	if (l == 2 * GPON_PASSWD_LEN)
		return parse_hex(s, pw, GPON_PASSWD_LEN);
	if (l > GPON_PASSWD_LEN)
		return -1;
	memset(pw, 0, GPON_PASSWD_LEN);
	memcpy(pw, s, l);

	return 0;
}

static int opt_parse(struct gpon_cfg *c, const struct opt *o, const char *v)
{
	uint8_t *p = (uint8_t *)c + o->off;
	char *end;
	long n;
	size_t i;

	switch (o->type) {
	case T_BOOL:
		*(bool *)p = !strcmp(v, "1") || !strcmp(v, "true") ||
			     !strcmp(v, "on") || !strcmp(v, "yes");
		return 0;
	case T_INT:
	case T_U8:
	case T_U16:
	case T_U32:
		errno = 0;
		n = strtol(v, &end, 0);
		if (errno || *end)
			return -EINVAL;
		if (o->type == T_INT)
			*(int *)p = n;
		else if (o->type == T_U8)
			*(uint8_t *)p = n;
		else if (o->type == T_U16)
			*(uint16_t *)p = n;
		else
			*(uint32_t *)p = strtoul(v, NULL, 0);
		break;
	case T_STR:
		if (strlen(v) >= o->size)
			return -EINVAL;
		strcpy((char *)p, v);
		break;
	case T_MODE:
		for (i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
			if (!strcmp(v, modes[i].name)) {
				*(int *)p = modes[i].mode;
				return 0;
			}
		}
		return -EINVAL;
	case T_SN:
		if (parse_sn(v, p))
			return -EINVAL;
		break;
	case T_PASSWD:
		if (parse_passwd(v, p))
			return -EINVAL;
		break;
	case T_UNI_LIST: {
		char tmp[GPON_MAX_UNI * 16 + 1], *tok, *save;

		/* "lan1 lan2", one name per UNI */
		if (strlen(v) >= sizeof(tmp))
			return -EINVAL;
		strcpy(tmp, v);
		memset(c->uni_dev, 0, sizeof(c->uni_dev));
		c->n_uni_dev = 0;
		for (tok = strtok_r(tmp, " ", &save); tok;
		     tok = strtok_r(NULL, " ", &save)) {
			if (c->n_uni_dev >= GPON_MAX_UNI || strlen(tok) >= 16)
				return -EINVAL;
			strcpy(c->uni_dev[c->n_uni_dev++], tok);
		}
		break;
	}
	}

	if (o->flag_off != (size_t)-1)
		*((bool *)c + o->flag_off) = true;

	return 0;
}

/* append @s to the bounded, NUL-terminated @buf, separated by a space */
static void str_list_add(char *buf, size_t len, size_t *off, const char *s)
{
	size_t n = strlen(s);

	if (*off && *off + 1 < len)
		buf[(*off)++] = ' ';
	if (*off >= len)
		return;
	if (n > len - *off - 1)
		n = len - *off - 1;
	memcpy(buf + *off, s, n);
	*off += n;
	buf[*off] = 0;
}

static void opt_format(const struct gpon_cfg *c, const struct opt *o,
		       char *buf, size_t len)
{
	const uint8_t *p = (const uint8_t *)c + o->off;
	size_t i, off;

	switch (o->type) {
	case T_BOOL:
		snprintf(buf, len, "%d", *(const bool *)p);
		break;
	case T_INT:
		snprintf(buf, len, "%d", *(const int *)p);
		break;
	case T_U8:
		snprintf(buf, len, o->off == offsetof(struct gpon_cfg,
						      omcc_version) ?
			 "0x%02x" : "%u", *p);
		break;
	case T_U16:
		snprintf(buf, len, "%u", *(const uint16_t *)p);
		break;
	case T_U32:
		snprintf(buf, len, "%u", *(const uint32_t *)p);
		break;
	case T_STR:
		snprintf(buf, len, "%s", (const char *)p);
		break;
	case T_MODE:
		snprintf(buf, len, "%s", config_mode_name(*(const int *)p));
		break;
	case T_SN:
		snprintf(buf, len, "%c%c%c%c%02x%02x%02x%02x",
			 isprint(p[0]) ? p[0] : '?', isprint(p[1]) ? p[1] : '?',
			 isprint(p[2]) ? p[2] : '?', isprint(p[3]) ? p[3] : '?',
			 p[4], p[5], p[6], p[7]);
		break;
	case T_UNI_LIST:
		off = 0;
		buf[0] = 0;
		for (i = 0; i < (size_t)c->n_uni_dev; i++)
			str_list_add(buf, len, &off, c->uni_dev[i]);
		break;
	case T_PASSWD:
		for (i = 0; i < GPON_PASSWD_LEN && 2 * i + 2 < len; i++)
			sprintf(buf + 2 * i, "%02x", p[i]);
		break;
	}
}

static bool opt_is_set(const struct gpon_cfg *c, const struct opt *o)
{
	return o->flag_off == (size_t)-1 || *((const bool *)c + o->flag_off);
}

/* an option, lists joined with spaces */
static const char *opt_lookup(struct uci_context *ctx, struct uci_section *s,
			      const char *name, char *buf, size_t len)
{
	struct uci_option *o = uci_lookup_option(ctx, s, name);
	struct uci_element *e;
	size_t off = 0;

	if (!o)
		return NULL;
	if (o->type == UCI_TYPE_STRING)
		return o->v.string;

	buf[0] = 0;
	uci_foreach_element(&o->v.list, e)
		str_list_add(buf, len, &off, e->name);

	return buf;
}

/*
 * The box has a single image in flash, but OMCI exposes two software
 * image instances.  When the inactive slot is empty, report the running
 * image there as the backup copy: same version, valid, neither active
 * nor committed.  Nothing is copied on flash (there is only one kernel
 * and rootfs); this is the ME 7 state the OLT sees, and it is derived
 * again on every load, so a real download into that slot still replaces
 * the version.
 */
static void config_clone_image(struct gpon_cfg *c)
{
	size_t active = c->sw_active == 1;
	size_t spare = !active;

	if (c->sw_ver_slot[spare][0] || !c->sw_ver_slot[active][0])
		return;

	snprintf(c->sw_ver_slot[spare], sizeof(c->sw_ver_slot[spare]), "%s",
		 c->sw_ver_slot[active]);
	c->sw_valid[spare] = 1;
	INFO("ONU: image %zu cloned from %zu ('%s')", spare, active,
	     c->sw_ver_slot[spare]);
}

int config_load(struct gpon_cfg *c)
{
	struct uci_context *ctx;
	struct uci_package *pkg = NULL;
	struct uci_section *s;
	char buf[GPON_MAX_UNI * 16 + 1];
	size_t i;

	config_defaults(c);

	ctx = uci_alloc_context();
	if (!ctx)
		return -ENOMEM;
	/* staged (uncommitted) changes of config_mode 1 are seen as well */
	if (uci_load(ctx, GPOND_PKG, &pkg)) {
		WARN("no /etc/config/%s, using defaults", GPOND_PKG);
		uci_free_context(ctx);
		return 0;
	}

	for (i = 0; i < sizeof(opts) / sizeof(opts[0]); i++) {
		const struct opt *o = &opts[i];
		const char *v;

		s = uci_lookup_section(ctx, pkg, o->section);
		if (!s)
			continue;
		v = opt_lookup(ctx, s, o->name, buf, sizeof(buf));
		if (v && opt_parse(c, o, v))
			WARN("%s.%s: invalid value '%s'", o->section, o->name,
			     o->type == T_PASSWD ? "..." : v);
	}

	/*
	 * onu.sw_version is the version of the image that is running, so it
	 * becomes the version of slot sw_active.  onu.sw_version<N> names a
	 * slot directly and wins: that is what an image downloaded over OMCI
	 * is recorded as, and it must not be overridden by the global one.
	 */
	s = uci_lookup_section(ctx, pkg, "onu");
	if (s) {
		size_t slot = c->sw_active == 1;
		const char *v = opt_lookup(ctx, s, "sw_version", buf,
					   sizeof(buf));

		if (v && !c->sw_ver_slot[slot][0]) {
			if (strlen(v) >= sizeof(c->sw_ver_slot[slot]))
				WARN("onu.sw_version: invalid value '%s'", v);
			else
				strcpy(c->sw_ver_slot[slot], v);
		}
	}

	uci_unload(ctx, pkg);
	uci_free_context(ctx);

	config_clone_image(c);

	return 0;
}

/* write software image state of @c to /etc/config/gpon */
int config_write(const struct gpon_cfg *c)
{
	struct uci_package *pkg;
	struct uci_context *ctx;
	const char *cur, *other;
	char buf[16], key[16], akey[16];
	int ret = 0, active = c->sw_active == 1;

	ctx = persist_open(GPOND_PKG, &pkg);
	if (!ctx)
		return -EIO;

	/* onu.sw_version is kept for the running image, the other slot is
	 * named directly; a named key of the running slot would outrank
	 * onu.sw_version when it is edited, so it goes */
	cur = c->sw_ver_slot[active];
	other = c->sw_ver_slot[!active];
	snprintf(key, sizeof(key), "sw_version%d", !active);
	snprintf(akey, sizeof(akey), "sw_version%d", active);

	ret = persist_set(ctx, GPOND_PKG, "onu", NULL, "onu");
	if (cur[0])
		ret = ret ? ret : persist_set(ctx, GPOND_PKG, "onu",
					      "sw_version", cur);
	ret = ret ? ret : persist_set(ctx, GPOND_PKG, "onu", akey, NULL);
	if (other[0])
		ret = ret ? ret : persist_set(ctx, GPOND_PKG, "onu", key, other);
	snprintf(buf, sizeof(buf), "%d", c->sw_active);
	ret = ret ? ret : persist_set(ctx, GPOND_PKG, "onu", "sw_active", buf);
	snprintf(buf, sizeof(buf), "%d", c->sw_committed);
	ret = ret ? ret : persist_set(ctx, GPOND_PKG, "onu", "sw_committed", buf);
	snprintf(buf, sizeof(buf), "%d", c->sw_valid[0]);
	ret = ret ? ret : persist_set(ctx, GPOND_PKG, "onu", "sw_valid0", buf);
	snprintf(buf, sizeof(buf), "%d", c->sw_valid[1]);
	ret = ret ? ret : persist_set(ctx, GPOND_PKG, "onu", "sw_valid1", buf);

	if (ret) {
		persist_close(ctx, pkg, false);
		return ret;
	}

	return persist_close(ctx, pkg, true);
}

void config_dump(const struct gpon_cfg *c, struct blob_buf *b)
{
	const char *sect = NULL;
	char buf[64];
	void *t = NULL;
	size_t i;

	for (i = 0; i < sizeof(opts) / sizeof(opts[0]); i++) {
		const struct opt *o = &opts[i];

		if (!sect || strcmp(sect, o->section)) {
			if (t)
				blobmsg_close_table(b, t);
			sect = o->section;
			t = blobmsg_open_table(b, sect);
		}
		/* never hand out the password or the key */
		if (o->type == T_PASSWD || !opt_is_set(c, o))
			continue;
		opt_format(c, o, buf, sizeof(buf));
		blobmsg_add_string(b, o->name, buf);
	}
	if (t)
		blobmsg_close_table(b, t);
}
