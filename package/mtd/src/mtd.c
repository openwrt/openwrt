/*
 * mtd - simple memory technology device manipulation tool
 *
 * Copyright (C) 2005 Waldemar Brodkorb <wbx@dass-it.de>,
 *	                  Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * The code is based on the linux-mtd examples.
 */

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include "mtd-api.h"
#include "mtd.h"

#define MAX_ARGS 8
#define JFFS2_DEFAULT_DIR	"" /* directory name without /, empty means root dir */

struct trx_header {
	uint32_t magic;		/* "HDR0" */
	uint32_t len;		/* Length of file including header */
	uint32_t crc32;		/* 32-bit CRC from flag_version to end of file */
	uint32_t flag_version;	/* 0:15 flags, 16:31 version */
	uint32_t offsets[3];    /* Offsets of partitions from start of header */
};

static char *buf = NULL;
static char *imagefile = NULL;
static char *jffs2file = NULL, *jffs2dir = JFFS2_DEFAULT_DIR;
static int buflen = 0;
int quiet;
int mtdsize = 0;
int erasesize = 0;

int mtd_open(const char *mtd, bool block)
{
	FILE *fp;
	char dev[PATH_MAX];
	int i;
	int ret;
	int flags = O_RDWR | O_SYNC;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, mtd)) {
				snprintf(dev, sizeof(dev), "/dev/mtd%s/%d", (block ? "block" : ""), i);
				if ((ret=open(dev, flags))<0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%s%d", (block ? "block" : ""), i);
					ret=open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}

	return open(mtd, flags);
}

int mtd_check_open(const char *mtd)
{
	struct mtd_info_user mtdInfo;
	int fd;

	fd = mtd_open(mtd, false);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		return 0;
	}

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info from %s\n", mtd);
		close(fd);
		return 0;
	}
	mtdsize = mtdInfo.size;
	erasesize = mtdInfo.erasesize;

	return fd;
}

int mtd_erase_block(int fd, int offset)
{
	struct erase_info_user mtdEraseInfo;

	mtdEraseInfo.start = offset;
	mtdEraseInfo.length = erasesize;
	ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
	if (ioctl (fd, MEMERASE, &mtdEraseInfo) < 0) {
		fprintf(stderr, "Erasing mtd failed.\n");
		exit(1);
	}
	return 0;
}

int mtd_write_buffer(int fd, const char *buf, int offset, int length)
{
	lseek(fd, offset, SEEK_SET);
	write(fd, buf, length);
	return 0;
}


static int
image_check(int imagefd, const char *mtd)
{
	int ret = 1;
#ifdef target_brcm
	ret = trx_check(imagefd, mtd, buf, &buflen);
#endif
	return ret;
}

static int mtd_check(const char *mtd)
{
	int fd;

	fd = mtd_check_open(mtd);
	if (!fd)
		return 0;

	if (!buf)
		buf = malloc(erasesize);

	close(fd);
	return 1;
}

static int
mtd_unlock(const char *mtd)
{
	int fd;
	struct erase_info_user mtdLockInfo;

	fd = mtd_check_open(mtd);
	if(fd <= 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if (quiet < 2) 
		fprintf(stderr, "Unlocking %s ...\n", mtd);

	mtdLockInfo.start = 0;
	mtdLockInfo.length = mtdsize;
	if(ioctl(fd, MEMUNLOCK, &mtdLockInfo)) {
		close(fd);
		return 0;
	}
		
	close(fd);
	return 0;
}

static int
mtd_erase(const char *mtd)
{
	int fd;
	struct erase_info_user mtdEraseInfo;

	if (quiet < 2)
		fprintf(stderr, "Erasing %s ...\n", mtd);

	fd = mtd_check_open(mtd);
	if(fd <= 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	mtdEraseInfo.length = erasesize;

	for (mtdEraseInfo.start = 0;
		 mtdEraseInfo.start < mtdsize;
		 mtdEraseInfo.start += erasesize) {
		
		ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
		if(ioctl(fd, MEMERASE, &mtdEraseInfo))
			fprintf(stderr, "Failed to erase block on %s at 0x%x\n", mtd, mtdEraseInfo.start);
	}		

	close(fd);
	return 0;

}

static int
mtd_refresh(const char *mtd)
{
	int fd;

	if (quiet < 2)
		fprintf(stderr, "Refreshing mtd partition %s ... ", mtd);

	fd = mtd_check_open(mtd);
	if(fd <= 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if (ioctl(fd, MTDREFRESH, NULL)) {
		fprintf(stderr, "Failed to refresh the MTD device\n");
		close(fd);
		exit(1);
	}
	close(fd);

	if (quiet < 2)
		fprintf(stderr, "\n");

	return 0;
}

static int
mtd_write(int imagefd, const char *mtd)
{
	int fd, result;
	ssize_t r, w, e;

	fd = mtd_check_open(mtd);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}
		
	if (quiet < 2)
		fprintf(stderr, "Writing from %s to %s ... ", imagefile, mtd);

	r = w = e = 0;
	if (!quiet)
		fprintf(stderr, " [ ]");

	for (;;) {
		/* buffer may contain data already (from trx check) */
		do {
			r = read(imagefd, buf + buflen, erasesize - buflen);
			if (r < 0) {
				if ((errno == EINTR) || (errno == EAGAIN))
					continue;
				else {
					perror("read");
					break;
				}
			}

			if (r == 0)
				break;

			buflen += r;
		} while (buflen < erasesize);

		if (buflen == 0)
			break;

		if (jffs2file) {
			if (memcmp(buf, JFFS2_EOF, sizeof(JFFS2_EOF) - 1) == 0) {
				if (!quiet)
					fprintf(stderr, "\b\b\b   ");
				if (quiet < 2)
					fprintf(stderr, "\nAppending jffs2 data to from %s to %s...", jffs2file, mtd);
				/* got an EOF marker - this is the place to add some jffs2 data */
				mtd_replace_jffs2(mtd, fd, e, jffs2file);
				goto done;
			}
			/* no EOF marker, make sure we figure out the last inode number
			 * before appending some data */
			mtd_parse_jffs2data(buf, jffs2dir);
		}

		/* need to erase the next block before writing data to it */
		while (w + buflen > e) {
			if (!quiet)
				fprintf(stderr, "\b\b\b[e]");

			mtd_erase_block(fd, e);

			/* erase the chunk */
			e += erasesize;
		}
		
		if (!quiet)
			fprintf(stderr, "\b\b\b[w]");
		
		if ((result = write(fd, buf, buflen)) < buflen) {
			if (result < 0) {
				fprintf(stderr, "Error writing image.\n");
				exit(1);
			} else {
				fprintf(stderr, "Insufficient space.\n");
				exit(1);
			}
		}
		w += buflen;

		buflen = 0;
	}
	if (!quiet)
		fprintf(stderr, "\b\b\b\b");

done:
	if (quiet < 2)
		fprintf(stderr, "\n");

	close(fd);
	return 0;
}

static void usage(void)
{
	fprintf(stderr, "Usage: mtd [<options> ...] <command> [<arguments> ...] <device>\n\n"
	"The device is in the format of mtdX (eg: mtd4) or its label.\n"
	"mtd recognizes these commands:\n"
	"        unlock                  unlock the device\n"
	"        refresh                 refresh mtd partition\n"
	"        erase                   erase all data on device\n"
	"        write <imagefile>|-     write <imagefile> (use - for stdin) to device\n"
	"        jffs2write <file>       append <file> to the jffs2 partition on the device\n"
	"Following options are available:\n"
	"        -q                      quiet mode (once: no [w] on writing,\n"
	"                                           twice: no status messages)\n"
	"        -r                      reboot after successful command\n"
	"        -f                      force write without trx checks\n"
	"        -e <device>             erase <device> before executing the command\n"
	"        -d <name>               directory for jffs2write, defaults to \"tmp\"\n"
	"        -j <name>               integrate <file> into jffs2 data when writing an image\n"
	"\n"
	"Example: To write linux.trx to mtd4 labeled as linux and reboot afterwards\n"
	"         mtd -r write linux.trx linux\n\n");
	exit(1);
}

static void do_reboot(void)
{
	fprintf(stderr, "Rebooting ...\n");
	fflush(stderr);

	/* try regular reboot method first */
	system("/sbin/reboot");
	sleep(2);

	/* if we're still alive at this point, force the kernel to reboot */
	syscall(SYS_reboot,LINUX_REBOOT_MAGIC1,LINUX_REBOOT_MAGIC2,LINUX_REBOOT_CMD_RESTART,NULL);
}

int main (int argc, char **argv)
{
	int ch, i, boot, imagefd = 0, force, unlocked;
	char *erase[MAX_ARGS], *device = NULL;
	enum {
		CMD_ERASE,
		CMD_WRITE,
		CMD_UNLOCK,
		CMD_REFRESH,
		CMD_JFFS2WRITE
	} cmd = -1;
	
	erase[0] = NULL;
	boot = 0;
	force = 0;
	buflen = 0;
	quiet = 0;

	while ((ch = getopt(argc, argv, "frqe:d:j:")) != -1)
		switch (ch) {
			case 'f':
				force = 1;
				break;
			case 'r':
				boot = 1;
				break;
			case 'j':
				jffs2file = optarg;
				break;
			case 'q':
				quiet++;
				break;
			case 'e':
				i = 0;
				while ((erase[i] != NULL) && ((i + 1) < MAX_ARGS))
					i++;
					
				erase[i++] = optarg;
				erase[i] = NULL;
				break;
			case 'd':
				jffs2dir = optarg;
				break;
			case '?':
			default:
				usage();
		}
	argc -= optind;
	argv += optind;
	
	if (argc < 2)
		usage();

	if ((strcmp(argv[0], "unlock") == 0) && (argc == 2)) {
		cmd = CMD_UNLOCK;
		device = argv[1];
	} else if ((strcmp(argv[0], "refresh") == 0) && (argc == 2)) {
		cmd = CMD_REFRESH;
		device = argv[1];
	} else if ((strcmp(argv[0], "erase") == 0) && (argc == 2)) {
		cmd = CMD_ERASE;
		device = argv[1];
	} else if ((strcmp(argv[0], "write") == 0) && (argc == 3)) {
		cmd = CMD_WRITE;
		device = argv[2];
	
		if (strcmp(argv[1], "-") == 0) {
			imagefile = "<stdin>";
			imagefd = 0;
		} else {
			imagefile = argv[1];
			if ((imagefd = open(argv[1], O_RDONLY)) < 0) {
				fprintf(stderr, "Couldn't open image file: %s!\n", imagefile);
				exit(1);
			}
		}
	
		if (!mtd_check(device)) {
			fprintf(stderr, "Can't open device for writing!\n");
			exit(1);
		}
		/* check trx file before erasing or writing anything */
		if (!image_check(imagefd, device) && !force) {
			fprintf(stderr, "Image check failed.\n");
			exit(1);
		}
	} else if ((strcmp(argv[0], "jffs2write") == 0) && (argc == 3)) {
		cmd = CMD_JFFS2WRITE;
		device = argv[2];
	
		imagefile = argv[1];
		if (!mtd_check(device)) {
			fprintf(stderr, "Can't open device for writing!\n");
			exit(1);
		}
	} else {
		usage();
	}

	sync();
	
	i = 0;
	unlocked = 0;
	while (erase[i] != NULL) {
		mtd_unlock(erase[i]);
		mtd_erase(erase[i]);
		if (strcmp(erase[i], device) == 0)
			unlocked = 1;
		i++;
	}
	
		
	switch (cmd) {
		case CMD_UNLOCK:
			if (!unlocked)
				mtd_unlock(device);
			break;
		case CMD_ERASE:
			if (!unlocked)
				mtd_unlock(device);
			mtd_erase(device);
			break;
		case CMD_WRITE:
			if (!unlocked)
				mtd_unlock(device);
			mtd_write(imagefd, device);
			break;
		case CMD_JFFS2WRITE:
			if (!unlocked)
				mtd_unlock(device);
			mtd_write_jffs2(device, imagefile, jffs2dir);
			break;
		case CMD_REFRESH:
			mtd_refresh(device);
			break;
	}

	sync();
	
	if (boot)
		do_reboot();

	return 0;
}
