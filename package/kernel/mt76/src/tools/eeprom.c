// SPDX-License-Identifier: ISC
/* Copyright (C) 2020 Felix Fietkau <nbd@nbd.name> */
#define _GNU_SOURCE
#include <sys/mman.h>
#include <sys/stat.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "mt76-test.h"

static const char *mtd_part;
static uint32_t mtd_offset;

static char *eeprom_file;
static int eeprom_fd = -1;
unsigned char *eeprom_data;

static int mt76_eeprom_dump_cb(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[NUM_MT76_TM_ATTRS];
	struct nlattr *attr;

	attr = unl_find_attr(&unl, msg, NL80211_ATTR_TESTDATA);
	if (!attr)
		return NL_SKIP;

	nla_parse_nested(tb, MT76_TM_ATTR_MAX, attr, msg_field.policy);
	if (!tb[MT76_TM_ATTR_MTD_PART] || !tb[MT76_TM_ATTR_MTD_OFFSET])
		return NL_SKIP;

	mtd_part = strdup(nla_get_string(tb[MT76_TM_ATTR_MTD_PART]));
	mtd_offset = nla_get_u32(tb[MT76_TM_ATTR_MTD_OFFSET]);

	return NL_SKIP;
}

static FILE *mtd_open(const char *mtd)
{
	char line[128], name[64];
	FILE *fp;
	int i;

	fp = fopen("/proc/mtd", "r");
	if (!fp)
		return NULL;

	snprintf(name, sizeof(name), "\"%s\"", mtd);
	while (fgets(line, sizeof(line), fp)) {
		if (!sscanf(line, "mtd%d:", &i) || !strstr(line, name))
			continue;

		snprintf(line, sizeof(line), "/dev/mtd%d", i);
		fclose(fp);
		return fopen(line, "r");
	}
	fclose(fp);

	return NULL;
}


static int
mt76_eeprom_create_file(void)
{
	char buf[1024];
	ssize_t len;
	FILE *f;
	int fd;

	f = mtd_open(mtd_part);
	if (!f) {
		fprintf(stderr, "Failed to open MTD device\n");
		return -1;
	}

	fd = open(eeprom_file, O_RDWR | O_CREAT | O_EXCL, 00644);
	if (fd < 0)
		goto out;

	while ((len = fread(buf, 1, sizeof(buf), f)) > 0) {
		ssize_t w;

retry:
		w = write(fd, buf, len);
		if (w > 0)
			continue;

		if (errno == EINTR)
			goto retry;

		perror("write");
		unlink(eeprom_file);
		close(fd);
		fd = -1;
		goto out;
	}

	lseek(fd, 0, SEEK_SET);

out:
	fclose(f);
	return fd;
}

static bool
mt76_eeprom_file_exists(void)
{
	struct stat st;

	return stat(eeprom_file, &st) == 0;
}

static int
mt76_eeprom_init_file(void)
{
	int fd;

	if (!mt76_eeprom_file_exists())
		return mt76_eeprom_create_file();

	fd = open(eeprom_file, O_RDWR);
	if (fd < 0)
		perror("open");

	return fd;
}

int mt76_eeprom_init(int phy)
{
	struct nl_msg *msg;

	msg = unl_genl_msg(&unl, NL80211_CMD_TESTMODE, true);
	nla_put_u32(msg, NL80211_ATTR_WIPHY, phy);
	unl_genl_request(&unl, msg, mt76_eeprom_dump_cb, NULL);

	if (!mtd_part) {
		fprintf(stderr, "Could not find MTD partition information\n");
		return -1;
	}

	eeprom_file = malloc(sizeof(EEPROM_FILE_PATH_FMT) + strlen(mtd_part));
	sprintf(eeprom_file, EEPROM_FILE_PATH_FMT, mtd_part);

	eeprom_fd = mt76_eeprom_init_file();
	if (eeprom_fd < 0)
		return -1;

	eeprom_data = mmap(NULL, EEPROM_PART_SIZE, PROT_READ | PROT_WRITE,
			   MAP_SHARED, eeprom_fd, mtd_offset);
	if (!eeprom_data) {
		perror("mmap");
		close(eeprom_fd);
		return -1;
	}

	return 0;
}

void mt76_eeprom_close(void)
{
	if (eeprom_fd < 0)
		return;

	msync(eeprom_data, EEPROM_PART_SIZE, MS_SYNC);
	munmap(eeprom_data, EEPROM_PART_SIZE);
	close(eeprom_fd);
	eeprom_fd = -1;
}

static int
mt76_eeprom_set(int argc, char **argv)
{
	for (; argc > 0; argc--, argv++) {
		char *addr_str = argv[0];
		char *val_str = strchr(addr_str, '=');
		unsigned long addr, val;
		char *err;

		if (!val_str) {
			fprintf(stderr, "Invalid argument: %s\n", addr_str);
			return 1;
		}

		*(val_str++) = 0;

		addr = strtoul(addr_str, &err, 0);
		if ((err && *err) || addr >= EEPROM_PART_SIZE) {
			fprintf(stderr, "Invalid address: %s\n", addr_str);
			return 1;
		}

		val = strtoul(val_str, &err, 0);
		if ((err && *err) || val >= 0xff) {
			fprintf(stderr, "Invalid value: %s\n", val_str);
			return 1;
		}

		eeprom_data[addr] = val;
	}

	return 0;
}

static int
mt76_eeprom_changes(void)
{
	unsigned char *buf;
	FILE *f;
	int i;

	f = mtd_open(mtd_part);
	if (!f) {
		fprintf(stderr, "Cannot open MTD device\n");
		return 1;
	}

	buf = malloc(EEPROM_PART_SIZE);
	fseek(f, mtd_offset, SEEK_SET);
	fread(buf, 1, EEPROM_PART_SIZE, f);
	for (i = 0; i < EEPROM_PART_SIZE; i++) {
		if (buf[i] == eeprom_data[i])
			continue;

		printf("[%04x] %02x => %02x\n", i, buf[i], eeprom_data[i]);
	}
	free(buf);

	return 0;
}


int mt76_eeprom(int phy, int argc, char **argv)
{
	const char *cmd;
	int ret = 0;

	if (argc < 1)
		usage();

	if (mt76_eeprom_init(phy))
		return 1;

	cmd = argv[0];
	argv++;
	argc--;

	if (!strcmp(cmd, "file"))
		printf("%s\n", eeprom_file);
	else if (!strcmp(cmd, "set"))
		ret = mt76_eeprom_set(argc, argv);
	else if (!strcmp(cmd, "reset"))
		unlink(eeprom_file);
	else if (!strcmp(cmd, "changes"))
		ret = mt76_eeprom_changes();

	mt76_eeprom_close();

	return ret;
}
