// SPDX-License-Identifier: GPL-2.0-only

#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define CHANNELS 14
#define EFUSE_SIZE 512
#define EFS2_SIZE 11534336
#define EFSBAK_SIZE 5636096

static const uint8_t nv4678_tag[] = { '4', '6', '7', '8', 0x6e, 0xff, 0x01 };

struct calibration {
	uint8_t cck_a[CHANNELS];
	uint8_t cck_b[CHANNELS];
	uint8_t ht40_a[CHANNELS];
	uint8_t ht40_b[CHANNELS];
	uint8_t ht20_diff[CHANNELS];
	uint8_t ofdm_diff[CHANNELS];
	uint8_t thermal;
	uint8_t crystal;
};

static void fail(const char *what, const char *path)
{
	if (path)
		fprintf(stderr, "mf283v-wlan-data: %s: %s\n", what, path);
	else
		fprintf(stderr, "mf283v-wlan-data: %s\n", what);
	exit(EXIT_FAILURE);
}

static bool valid_mac(const uint8_t mac[6])
{
	static const uint8_t zero[6];
	static const uint8_t ff[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	return !(mac[0] & 1) && memcmp(mac, zero, 6) && memcmp(mac, ff, 6);
}

static void extract_nv4678(const char *path, off_t expected_size, uint8_t mac[6])
{
	uint8_t buf[65536 + sizeof(nv4678_tag) + 6];
	size_t carry = 0, matches = 0;
	off_t total = 0;
	int fd;

	fd = open(path, O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		fail(strerror(errno), path);

	for (;;) {
		ssize_t got = read(fd, buf + carry, 65536);
		size_t len, i;

		if (got < 0) {
			if (errno == EINTR)
				continue;
			close(fd);
			fail(strerror(errno), path);
		}
		if (!got)
			break;

		total += got;
		len = carry + (size_t)got;
		for (i = 0; i + sizeof(nv4678_tag) + 6 <= len; i++) {
			const uint8_t *candidate;

			if (memcmp(buf + i, nv4678_tag, sizeof(nv4678_tag)))
				continue;
			candidate = buf + i + sizeof(nv4678_tag);
			if (!valid_mac(candidate))
				continue;
			if (matches && memcmp(mac, candidate, 6)) {
				close(fd);
				fail("conflicting valid NV 4678 records", path);
			}
			memcpy(mac, candidate, 6);
			matches++;
		}

		carry = sizeof(nv4678_tag) + 6 - 1;
		if (len < carry)
			carry = len;
		memmove(buf, buf + len - carry, carry);
	}
	close(fd);

	if (total != expected_size)
		fail("unexpected partition size", path);
	if (!matches)
		fail("no valid NV 4678 record", path);
}

static int hex_nibble(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static void read_hex_file(const char *dir, const char *name, uint8_t *out,
			  size_t count, uint8_t maximum)
{
	char path[512], data[CHANNELS * 2 + 2];
	ssize_t got;
	int fd;
	size_t i;

	if (snprintf(path, sizeof(path), "%s/%s", dir, name) >= (int)sizeof(path))
		fail("calibration path is too long", name);
	fd = open(path, O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		fail(strerror(errno), path);
	got = read(fd, data, sizeof(data));
	if (got < 0) {
		close(fd);
		fail(strerror(errno), path);
	}
	close(fd);

	if (got != (ssize_t)(count * 2) &&
	    !(got == (ssize_t)(count * 2 + 1) && data[count * 2] == '\n'))
		fail("invalid calibration file length", path);

	for (i = 0; i < count; i++) {
		int high = hex_nibble(data[i * 2]);
		int low = hex_nibble(data[i * 2 + 1]);

		if (high < 0 || low < 0)
			fail("non-hexadecimal calibration value", path);
		out[i] = (high << 4) | low;
		if (out[i] > maximum)
			fail("calibration value is out of range", path);
	}
}

static void read_calibration(const char *dir, struct calibration *cal)
{
	read_hex_file(dir, "tx_power_cck_a", cal->cck_a, CHANNELS, 0x3f);
	read_hex_file(dir, "tx_power_cck_b", cal->cck_b, CHANNELS, 0x3f);
	read_hex_file(dir, "tx_power_ht40_1s_a", cal->ht40_a, CHANNELS, 0x3f);
	read_hex_file(dir, "tx_power_ht40_1s_b", cal->ht40_b, CHANNELS, 0x3f);
	read_hex_file(dir, "tx_power_diff_ht20", cal->ht20_diff, CHANNELS, 0xff);
	read_hex_file(dir, "tx_power_diff_ofdm", cal->ofdm_diff, CHANNELS, 0xff);
	read_hex_file(dir, "11n_ther", &cal->thermal, 1, 0x3f);
	read_hex_file(dir, "11n_xcap", &cal->crystal, 1, 0x3f);
}

static void write_all(int fd, const void *data, size_t length, const char *path)
{
	const uint8_t *cursor = data;

	while (length) {
		ssize_t written = write(fd, cursor, length);

		if (written < 0) {
			if (errno == EINTR)
				continue;
			fail(strerror(errno), path);
		}
		if (!written)
			fail("short write", path);
		cursor += written;
		length -= (size_t)written;
	}
}

static void write_mac(const char *path, const uint8_t mac[6])
{
	char value[18];
	int fd, len;

	len = snprintf(value, sizeof(value), "%02x:%02x:%02x:%02x:%02x:%02x",
		       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	if (len != 17)
		fail("failed to format WLAN MAC", NULL);
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
	if (fd < 0)
		fail(strerror(errno), path);
	write_all(fd, value, 17, path);
	if (close(fd))
		fail(strerror(errno), path);
}

static uint8_t channel_group_value(const uint8_t values[CHANNELS], int group,
				   bool cck)
{
	static const uint8_t first[] = { 0, 2, 5, 8, 11, 13 };

	if (!cck && group == 5)
		group = 4;
	return values[first[group]];
}

static void write_efuse(const char *path, const uint8_t mac[6],
			const struct calibration *cal)
{
	uint8_t map[EFUSE_SIZE];
	char line[4];
	int fd, i, group;

	memset(map, 0xff, sizeof(map));
	map[0] = 0x29;
	map[1] = 0x81;

	for (group = 0; group < 6; group++)
		map[0x10 + group] = channel_group_value(cal->cck_a, group, true);
	for (group = 0; group < 5; group++)
		map[0x16 + group] = channel_group_value(cal->ht40_a, group, false);
	map[0x1b] = ((cal->ht20_diff[0] & 0x0f) << 4) |
		   (cal->ofdm_diff[0] & 0x0f);

	for (group = 0; group < 6; group++)
		map[0x3a + group] = channel_group_value(cal->cck_b, group, true);
	for (group = 0; group < 5; group++)
		map[0x40 + group] = channel_group_value(cal->ht40_b, group, false);
	map[0x45] = (cal->ht20_diff[0] & 0xf0) |
		   ((cal->ofdm_diff[0] >> 4) & 0x0f);

	map[0xb9] = cal->crystal;
	map[0xba] = cal->thermal;
	memcpy(map + 0x11a, mac, 6);

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
	if (fd < 0)
		fail(strerror(errno), path);
	for (i = 0; i < EFUSE_SIZE; i++) {
		int len = snprintf(line, sizeof(line), "%02x%c", map[i],
				   i % 16 == 15 ? '\n' : ' ');
		if (len != 3) {
			close(fd);
			fail("failed to format synthesized EFUSE map", path);
		}
		write_all(fd, line, 3, path);
	}
	if (close(fd))
		fail(strerror(errno), path);
}

static void usage(const char *name)
{
	fprintf(stderr, "usage: %s EFS2 EFSBAK CALIBRATION_DIR MAC_OUT EFUSE_OUT\n",
		name);
}

int main(int argc, char **argv)
{
	uint8_t primary_mac[6] = { 0 }, backup_mac[6] = { 0 };
	struct calibration cal;

	if (argc != 6) {
		usage(argv[0]);
		return 2;
	}

	extract_nv4678(argv[1], EFS2_SIZE, primary_mac);
	extract_nv4678(argv[2], EFSBAK_SIZE, backup_mac);
	if (memcmp(primary_mac, backup_mac, 6))
		fail("EFS2 and EFSBAK NV 4678 values do not match", NULL);

	read_calibration(argv[3], &cal);
	write_mac(argv[4], primary_mac);
	write_efuse(argv[5], primary_mac, &cal);

	printf("factory WLAN MAC %02x:%02x:%02x:%02x:%02x:%02x verified in EFS2 and EFSBAK\n",
	       primary_mac[0], primary_mac[1], primary_mac[2], primary_mac[3],
	       primary_mac[4], primary_mac[5]);
	printf("factory RTL8192ES calibration validated for 14 channels and two RF paths\n");
	return 0;
}
