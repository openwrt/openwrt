// SPDX-License-Identifier: ISC
/* Copyright (C) 2020 Felix Fietkau <nbd@nbd.name> */
#define _GNU_SOURCE

#include <stdio.h>
#include "mt76-test.h"

struct unl unl;
static uint32_t tm_changed[DIV_ROUND_UP(NUM_MT76_TM_ATTRS, 32)];
static const char *progname;

static int phy_lookup_idx(const char *name)
{
	char buf[128];
	FILE *f;
	int len;

	snprintf(buf, sizeof(buf), "/sys/class/ieee80211/%s/index", name);
	f = fopen(buf, "r");
	if (!f)
		return -1;

	len = fread(buf, 1, sizeof(buf) - 1, f);
	fclose(f);

	if (!len)
		return -1;

	buf[len] = 0;
	return atoi(buf);
}

void usage(void)
{
	static const char *const commands[] = {
		"set <var>=<val> [...]",
		"dump [stats]",
		"eeprom file",
		"eeprom set <addr>=<val> [...]",
		"eeprom changes",
		"eeprom reset",
	};
	int i;

	fprintf(stderr, "Usage:\n");
	for (i = 0; i < ARRAY_SIZE(commands); i++)
		printf("  %s phyX %s\n", progname, commands[i]);

	exit(1);
}

static int mt76_dump_cb(struct nl_msg *msg, void *arg)
{
	struct nlattr *attr;

	attr = unl_find_attr(&unl, msg, NL80211_ATTR_TESTDATA);
	if (!attr) {
		fprintf(stderr, "Testdata attribute not found\n");
		return NL_SKIP;
	}

	msg_field.print(&msg_field, attr);

	return NL_SKIP;
}

static int mt76_dump(int phy, int argc, char **argv)
{
	struct nl_msg *msg;
	void *data;

	msg = unl_genl_msg(&unl, NL80211_CMD_TESTMODE, true);
	nla_put_u32(msg, NL80211_ATTR_WIPHY, phy);

	data = nla_nest_start(msg, NL80211_ATTR_TESTDATA);

	for (; argc > 0; argc--, argv++) {
		if (!strcmp(argv[0], "stats"))
			nla_put_flag(msg, MT76_TM_ATTR_STATS);
	}

	nla_nest_end(msg, data);

	unl_genl_request(&unl, msg, mt76_dump_cb, NULL);

	return 0;
}

static inline void tm_set_changed(uint32_t id)
{
	tm_changed[id / 32] |= (1U << (id % 32));
}

static inline bool tm_is_changed(uint32_t id)
{
	return tm_changed[id / 32] & (1U << (id % 32));
}

static int mt76_set(int phy, int argc, char **argv)
{
	const struct tm_field *fields = msg_field.fields;
	struct nl_msg *msg;
	void *data;
	int i, ret;

	if (argc < 1)
		return 1;

	msg = unl_genl_msg(&unl, NL80211_CMD_TESTMODE, false);
	nla_put_u32(msg, NL80211_ATTR_WIPHY, phy);

	data = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	for (; argc > 0; argc--, argv++) {
		char *name = argv[0];
		char *val = strchr(name, '=');

		if (!val) {
			fprintf(stderr, "Invalid argument: %s\n", name);
			return 1;
		}

		*(val++) = 0;

		for (i = 0; i < msg_field.len; i++) {
			if (!fields[i].parse)
				continue;

			if (!strcmp(fields[i].name, name))
				break;
		}

		if (i == msg_field.len) {
			fprintf(stderr, "Unknown field: %s\n", name);
			return 1;
		}

		if (tm_is_changed(i)) {
			fprintf(stderr, "Duplicate field '%s'\n", name);
			return 1;
		}

		if (!fields[i].parse(&fields[i], i, msg, val))
			return 1;

		tm_set_changed(i);
	}

	nla_nest_end(msg, data);

	ret = unl_genl_request(&unl, msg, NULL, NULL);
	if (ret)
		fprintf(stderr, "nl80211 call failed: %s\n", strerror(-ret));

	return ret;
}

int main(int argc, char **argv)
{
	const char *cmd;
	int phy;
	int ret = 0;

	progname = argv[0];
	if (argc < 3)
		usage();

	if (unl_genl_init(&unl, "nl80211") < 0) {
		fprintf(stderr, "Failed to connect to nl80211\n");
		return 2;
	}

	phy = phy_lookup_idx(argv[1]);
	if (phy < 0) {
		fprintf(stderr, "Could not find phy '%s'\n", argv[1]);
		return 2;
	}

	cmd = argv[2];
	argv += 3;
	argc -= 3;

	if (!strcmp(cmd, "dump"))
		ret = mt76_dump(phy, argc, argv);
	else if (!strcmp(cmd, "set"))
		ret = mt76_set(phy, argc, argv);
	else if (!strcmp(cmd, "eeprom"))
		ret = mt76_eeprom(phy, argc, argv);
	else
		usage();

	unl_free(&unl);

	return ret;
}
