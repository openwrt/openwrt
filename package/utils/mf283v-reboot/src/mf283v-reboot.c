// SPDX-License-Identifier: GPL-2.0-only

#define _GNU_SOURCE

#include <errno.h>
#include <linux/reboot.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

static void usage(const char *name)
{
	fprintf(stderr, "usage: %s recovery|bootloader\n", name);
}

int main(int argc, char **argv)
{
	const char *mode;

	if (argc != 2) {
		usage(argv[0]);
		return 2;
	}

	mode = argv[1];
	if (strcmp(mode, "recovery") && strcmp(mode, "bootloader")) {
		usage(argv[0]);
		return 2;
	}

	if (geteuid()) {
		fprintf(stderr, "%s: root privileges are required\n", argv[0]);
		return 1;
	}

	fprintf(stderr, "Rebooting ZTE MF283V into %s.\n", mode);
	sync();

	if (syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
		    LINUX_REBOOT_CMD_RESTART2, mode) < 0) {
		fprintf(stderr, "%s: reboot(%s) failed: %s\n", argv[0], mode,
			strerror(errno));
		return 1;
	}

	return 0;
}
