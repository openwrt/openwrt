/*
 * mtd - simple memory technology device manipulation tool
 *
 * Copyright (C) 2005 Waldemar Brodkorb <wbx@dass-it.de>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * mtd utility for the openwrt project
 * it is mainly code from the linux-mtd project, which accepts the same
 * command line arguments as the broadcom utility
 * 
 * $Id$
 *
 */

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <string.h>

#include <linux/mtd/mtd.h>

/* trx header */
#define TRX_MAGIC       0x30524448      /* "HDR0" */
#define TRX_VERSION     1
#define TRX_MAX_LEN     0x3A0000
#define TRX_NO_HEADER   1               /* Do not write TRX header */

struct trx_header {
	uint32_t magic;                 /* "HDR0" */
	uint32_t len;                   /* Length of file including header */
	uint32_t crc32;                 /* 32-bit CRC from flag_version to end of file */
	uint32_t flag_version;  /* 0:15 flags, 16:31 version */
	uint32_t offsets[3];    /* Offsets of partitions from start of header */
};

#define BUFSIZE (10 * 1024)

extern int mtd_open(const char *mtd, int flags);
extern int mtd_erase(const char *mtd);
extern int mtd_write(const char *trxfile, const char *mtd);
extern int mtd_update(const char *trxfile, const char *mtd);

int
mtd_unlock(const char *mtd)
{
	int fd;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdLockInfo;

	fd = mtd_open(mtd, O_RDWR);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info from %s\n", mtd);
		close(fd);
		exit(1);
	}

	mtdLockInfo.start = 0;
	mtdLockInfo.length = mtdInfo.size;
	if(ioctl(fd, MEMUNLOCK, &mtdLockInfo)) {
		fprintf(stderr, "Could not unlock MTD device: %s\n", mtd);
		close(fd);
		exit(1);
	}
		
	close(fd);
	return 0;
}

int
mtd_open(const char *mtd, int flags)
{
	FILE *fp;
	char dev[PATH_MAX];
	int i;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, mtd)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				fclose(fp);
				return open(dev, flags);
			}
		}
		fclose(fp);
	}

	return open(mtd, flags);
}

int
mtd_erase(const char *mtd)
{
	int fd;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdEraseInfo;

	fd = mtd_open(mtd, O_RDWR);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info from %s\n", mtd);
		close(fd);
		exit(1);
	}

	mtdEraseInfo.length = mtdInfo.erasesize;

	for (mtdEraseInfo.start = 0;
		 mtdEraseInfo.start < mtdInfo.size;
		 mtdEraseInfo.start += mtdInfo.erasesize) {
		
		ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
		if(ioctl(fd, MEMERASE, &mtdEraseInfo)) {
			fprintf(stderr, "Could not erase MTD device: %s\n", mtd);
			close(fd);
			exit(1);
		}
	}		

	close(fd);
	return 0;

}

int
mtd_write(const char *trxfile, const char *mtd)
{
	int fd;
	int trxfd;
	int i;
	size_t result,size,written;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdEraseInfo;
	struct stat trxstat;
	unsigned char src[BUFSIZE],dest[BUFSIZE];

	fd = mtd_open(mtd, O_RDWR);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info from %s\n", mtd);
		close(fd);
		exit(1);
	}

	trxfd = open(trxfile,O_RDONLY);	
	if(trxfd < 0) {
		fprintf(stderr, "Could not open trx image: %s\n", trxfile);
		exit(1);
	}

	if (fstat (trxfd,&trxstat) < 0) {
		fprintf(stderr, "Could not get trx image file status: %s\n", trxfile);
		close(trxfd);
		exit(1);
	}
		
	if(mtdInfo.size < trxstat.st_size) {
		fprintf(stderr, "Image too big for partition: %s\n", mtd);
		close(trxfd);
		exit(1);
	}	
	
	mtdEraseInfo.start = 0;
	mtdEraseInfo.length = trxstat.st_size & ~(mtdInfo.erasesize -1);
	if(trxstat.st_size % mtdInfo.erasesize) mtdEraseInfo.length += mtdInfo.erasesize;

	/* erase the chunk */
	if (ioctl (fd,MEMERASE,&mtdEraseInfo) < 0) {
		fprintf(stderr, "Erasing mtd failed: %s\n", mtd);
		exit(1);
	}
	
	size = trxstat.st_size;
	i = BUFSIZE;
	written = 0;

	while (size) {
		if (size < BUFSIZE) i = size;
		read(trxfd,src,i);
		result = write(fd,src,i);
		if (i != result) {
			if (result < 0) {
				fprintf(stderr,"Error while writing image");
				exit(1);
			}
			fprintf(stderr,"Error writing image");
			exit(1);
		}
		written += i;
		size -= i;
	}
	
	return 0;
}

int
mtd_update(const char *trxfile, const char *mtd)
{
	if (mtd_unlock(mtd) != 0) {
		fprintf(stderr, "Could not unlock mtd device: %s\n", mtd);
		exit(1);
	}
	if (mtd_erase("rootfs") != 0) {
		fprintf(stderr, "Could not erase rootfs\n");
		exit(1);
	}
	if (mtd_write(trxfile, mtd) != 0) {
		fprintf(stderr, "Could not update %s with %s\n", mtd, trxfile);
		exit(1);
	}	
	return 0;
}

int main(int argc, char **argv) {
	if(argc == 3 && strcasecmp(argv[1],"unlock")==0) {
		printf("Unlocking %s ...\n",argv[2]);
		return mtd_unlock(argv[2]);
	}
	if(argc == 3 && strcasecmp(argv[1],"erase")==0) {
		printf("Erasing %s ...\n",argv[2]);
		return mtd_erase(argv[2]);
	}
	if(argc == 4 && strcasecmp(argv[1],"write")==0) {
		printf("Writing %s to %s ...\n",argv[2],argv[3]);
		return mtd_write(argv[2],argv[3]);
	}
	if(argc == 4 && strcasecmp(argv[1],"update")==0) {
		printf("Updating %s on %s ...\n",argv[2],argv[3]);
		return mtd_update(argv[2],argv[3]);
	}

	printf("no valid command given\n");
	printf("\nmtd: modify data within a Memory Technology Device.\n");
	printf("Copyright (C) 2005 Waldemar Brodkorb <wbx@dass-it.de>\n");
	printf("Documented by Mike Strates [dumpedcore] <mike@dilaudid.net>\n");
	printf("mtd has ABSOLUTELY NO WARRANTY and is licensed under the GNU GPL.\n");
	printf("\nUsage: mtd [unlock|erase] device\n");
	printf("       mtd [write|update] imagefile device\n");
	printf("\n.. where device is in the format of mtdX (eg: mtd4) or its label.\n\n");
	printf("unlock          enable modification to device\n");
	printf("erase           erase all data on device\n");
	printf("write           write imagefile to device\n");
	printf("update          remove rootfs and update imagefile on device\n");
	printf("\nExample: To write linux.trx to mtd4 labeled as linux\n");
	printf("\n                mtd unlock linux && mtd write linux.trx linux\n\n");
	return -1;
}

