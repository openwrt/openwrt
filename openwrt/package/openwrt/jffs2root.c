/*
 * jffs2root.c
 *
 * Copyright (C) 2005 Mike Baker 
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
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/mtd/mtd.h>

#define FILENAME "/dev/mtdblock/1"

struct trx_header {
    unsigned magic;		/* "HDR0" */
    unsigned len;		/* Length of file including header */
    unsigned crc32;		/* 32-bit CRC from flag_version to end of file */
    unsigned flag_version;	/* 0:15 flags, 16:31 version */
    unsigned offsets[3];	/* Offsets of partitions from start of header */
};

unsigned long *crc32;

void init_crc32()
{
    unsigned long crc;
    unsigned long poly = 0xEDB88320L;
    int n, bit;
    if ((crc32 = (unsigned long *) malloc(256 * sizeof(unsigned long))) == (void *)-1) {
	    perror("malloc");
	    exit(1);
    }
    for (n = 0; n < 256; n++) {
	crc = (unsigned long) n;
	for (bit = 0; bit < 8; bit++)
	    crc = (crc & 1) ? (poly ^ (crc >> 1)) : (crc >> 1);
	crc32[n] = crc;
    }
}

unsigned int crc32buf(char *buf, size_t len)
{
    unsigned int crc = 0xFFFFFFFF;
    for (; len; len--, buf++)
	crc = crc32[(crc ^ *buf) & 0xff] ^ (crc >> 8);
    return crc;
}

int main(int argc, char **argv)
{
    int fd;
    struct mtd_info_user mtdInfo;
    unsigned long len;
    struct trx_header *ptr;
	
    if (((fd = open(FILENAME, O_RDWR))  < 0)
    || ((len = lseek(fd, 0, SEEK_END)) < 0)
    || ((ptr = (struct trx_header *) mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == (void *) (-1))
    || (ptr->magic != 0x30524448)) {
	printf("Error reading trx info\n");
	exit(-1);
    }
    close (fd);

    if (((fd = open("/dev/mtd/1", O_RDWR))  < 0)
    || (ioctl(fd, MEMGETINFO, &mtdInfo))) {
      fprintf(stderr, "Could not get MTD device info from %s\n", FILENAME);
      close(fd);
      exit(1);
    }
    close(fd);

    if (argc > 1 && !strcmp(argv[1],"--move")) {
      if (ptr->offsets[2] >= ptr->len) {
        printf("Partition already moved outside trx\n");
      } else {
	init_crc32();
	ptr->offsets[2] +=  (mtdInfo.erasesize - 1);
	ptr->offsets[2] &= ~(mtdInfo.erasesize - 1);
        ptr->len = ptr->offsets[2];
        ptr->crc32 = crc32buf((void *) &(ptr->flag_version), ptr->len - offsetof(struct trx_header, flag_version));
	msync(ptr,sizeof(struct trx_header),MS_SYNC|MS_INVALIDATE);
	printf("Partition moved; please reboot\n");
      }
    } else {
      int x;
      printf(" erase: 0x%08x\n",mtdInfo.erasesize);
      printf("=== trx ===\n");
      printf("mapped: 0x%08x\n", (unsigned)ptr);
      printf(" magic: 0x%08x\n", ptr->magic);
      printf("   len: 0x%08x\n", ptr->len);
      printf("   crc: 0x%08x\n", ptr->crc32);
      for (x = 0; x < 3; x++)
	printf(" offset[%d]: 0x%08x\n", x, ptr->offsets[x]);
    }

    munmap((void *) ptr, len);
    return 0;
}
