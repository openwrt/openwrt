/*
 * motorola-bin.c
 *
 * Copyright (C) 2005 Mike Baker 
 *                    Openwrt.org
 *
 * $Id$
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <netinet/in.h>

unsigned long *crc32;

void init_crc32()
{
    unsigned long crc;
    unsigned long poly = ntohl(0x2083b8ed);
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

struct motorola {
	unsigned int crc;	// crc32 of the remainder
	unsigned int flags;	// unknown, 10577050
	char *trx;		// standard trx
};

int main(int argc, char **argv)
{
	unsigned int len;
	int fd;
	void *trx;
	struct motorola *firmware;

	if (argc<3) {
		printf("%s <trx> <motorola.bin>\n",argv[0]);
		exit(0);
	}

	// mmap trx file
	if (((fd = open(argv[1], O_RDONLY))  < 0)
	|| ((len = lseek(fd, 0, SEEK_END)) < 0)
	|| ((trx = mmap(0, len, PROT_READ, MAP_SHARED, fd, 0)) == (void *) (-1))
	|| (close(fd) < 0)) {
		perror("open/malloc");
		exit(-1);
	}
	
	// create a firmware image in memory
	// and copy the trx to it
	firmware = malloc(len+8);
	memcpy(&firmware->trx,trx,len);
	munmap(trx,len);

	// setup the motorola headers
	init_crc32();
	firmware->flags = ntohl(0x10577050);
	firmware->crc   = htonl(crc32buf((char *)&firmware->flags,len+4));

	// write the firmware
	if (((fd = open(argv[2], O_CREAT|O_WRONLY,0644)) < 0)
	|| (write(fd,firmware,len+8) != len+8)
	|| (close(fd) < 0)) {
		perror("write");
		exit(-1);
	}

	free(firmware);

	return 0;
}
