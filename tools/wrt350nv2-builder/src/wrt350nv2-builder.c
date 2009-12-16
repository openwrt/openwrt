/*

	WRT350Nv2-Builder 2.0 (previously called buildimg)
	Copyright (C) 2008-2009 Dirk Teurlings <info@upexia.nl>
	Copyright (C) 2009      Matthias Buecher (http://www.maddes.net/)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A lot of thanks to Kaloz and juhosg from OpenWRT and Lennert Buytenhek from
	marvell for helping me figure this one out. This code is based on bash
	scripts wrote by Peter van Valderen so the real credit should go to him.

	This program reads the provided parameter file and creates an image which can
	be used to flash a Linksys WRT350N v2 from stock firmware.
	The trick is to fill unused space in the bin file with random, so that the
	resulting zip file passes the size check of the stock firmware.

	The parameter file layout for an original Linksys firmware:
		:kernel	0x001A0000	/path/to/uImage
		:rootfs	0	/path/to/root.squashfs
		:u-boot	0	/path/to/u-boot.bin

	args:
		1	wrt350nv2.par		parameter file describing the image layout
		2	wrt350nv2.img		output file for linksys style image

	An u-boot image inside the bin file is not necessary.
	The version is not important.
	The name of the bin file is not important.

	Linksys assumes that no mtd will be used to its maximum, so the last 16 bytes
	of the mtd are abused to define the length of the next mtd content (4 bytes for
	size + 12 pad bytes).

	At the end of "rootfs" additional 16 bytes are abused for some data and an
	highly important eRcOmM identifier, so the last 32 bytes of "rootfs" are abused.

	At the end of "u-boot" 128 bytes are abused for some data, a checksum and an
	highly important sErCoMm identifier.


	This program uses a special GNU scanf modifier to allocate
	sufficient memory for a strings with unknown length.
	See http://www.kernel.org/doc/man-pages/online/pages/man3/scanf.3.html#NOTES


	To extract everything from a Linksys style firmware image see
	https://forum.openwrt.org/viewtopic.php?pid=92928#p92928

*/

// ToDo:
// * Has NODE to be added to bin file *after* creating checksum byte?

// includes
#define _GNU_SOURCE	// for GNU's basename()
#include <assert.h>
#include <errno.h>	// errno
#include <stdarg.h>
#include <stdio.h>	// fopen(), fread(), fclose(), etc.
#include <stdlib.h>	// system(), etc.
#include <string.h>	// basename(), strerror(), strdup(), etc.
#include <unistd.h>	// optopt(), access(), etc.
#include <libgen.h>
#include <sys/wait.h>	// WEXITSTATUS, etc.

// custom includes
#include "md5.h"	// MD5 routines
#include "upgrade.h"	// Linksys definitions from firmware 2.0.19


// version info
#define VERSION "2.0"
char program_info[] = "WRT350Nv2-Builder v%s by Dirk Teurlings <info@upexia.nl> and Matthias Buecher (http://www.maddes.net/)\n";

// verbosity
#define DEBUG 1
#define DEBUG_LVL2 2
int verbosity = 0;

// mtd info
typedef struct {
	char *name;
	int offset;
	int size;
	char *filename;
	long int filesize;
	unsigned char magic[2];
} mtd_info;

mtd_info mtd_kernel = { "kernel", 0, 0, NULL, 0L, { 0, 0 } };
mtd_info mtd_rootfs = { "rootfs", 0, 0, NULL, 0L, { 0, 0 } };
mtd_info mtd_uboot = { "u-boot", 0, 0, NULL, 0L, { 0, 0 } };

#define ROOTFS_END_OFFSET	0x00760000
#define ROOTFS_MIN_OFFSET	0x00700000	// should be filled up to here, to make sure that the zip file is big enough to pass the size check of the stock firmware

// rootfs statics via: hexdump -v -e '1/1 "0x%02X, "' -s 0x0075FFE0 -n 16 "wrt350n.bin" ; echo -en "\n"
unsigned char product_id[] = { 0x00, 0x03 };	// seems to be a fixed value
unsigned char protocol_id[] = { 0x00, 0x00 };	// seems to be a fixed value
unsigned char fw_version[] = { 0x20, 0x19 };
unsigned char rootfs_unknown[] = { 0x90, 0xF7 };	// seems to be a fixed value
unsigned char sign[] = { 0x65, 0x52, 0x63, 0x4F, 0x6D, 0x4D, 0x00, 0x00 };	// eRcOmM

// u-boot statics via: hexdump -v -e '1/1 "0x%02X, "' -s 0x007FFF80 -n 128 "wrt350n.bin" ; echo -en "\n"
//unsigned char sn[]   = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };	// (12) seems to be an unused value
//unsigned char pin[]  = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };	// (8) seems to be an unused value
//unsigned char node[] = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// (25) seems to be an unused value
//				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//unsigned char checksum[] = { 0xE9 };	// (1) is calculated, does it belong to node?
unsigned char pid[] = {	0x73, 0x45, 0x72, 0x43, 0x6F, 0x4D, 0x6D, 0x00, 0x01, 0x00, 0x00, 0x59, 0x42, 0x50, 0x00, 0x01,	// (70) seems to be a fixed value, except for fw version
				0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,	// protocol id?
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,	// protocol id?
				0x12, 0x34,	// firmware version, same as in rootfs
				0x00, 0x00, 0x00, 0x04,
				0x73, 0x45, 0x72, 0x43, 0x6F, 0x4D, 0x6D };	// sErCoMm

// img statics via: hexdump -v -e '1/1 "0x%02X, "' -s 0 -n 512 "WRT350N-EU-ETSI-2.00.19.img" ; echo -en "\n"
unsigned char img_hdr[] = {	0x00, 0x01, 0x00, 0x00, 0x59, 0x42, 0x50, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
				0x00, 0x00,
				0x12, 0x34,	// firmware version, same as in rootfs
				0x00, 0x00, 0x00, 0x04, 0x61, 0x44, 0x6D, 0x42, 0x6C, 0x4B, 0x3D, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,	// md5 checksum
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

unsigned char img_eof[] = {	0xFF };


void lprintf(int outputlevel, char *fmt, ...) {
	va_list argp;
	if (outputlevel <= verbosity) {
		va_start(argp, fmt);
		vprintf(fmt, argp);
		va_end(argp);
	}
}


int parse_par_file(FILE *f_par) {
	int exitcode = 0;

	char *buffer;
	size_t buffer_size;
	char *line;

	int lineno;
	int count;

	char string1[256];
	char string2[256];
	int value;

	mtd_info *mtd;
	FILE *f_in;
	int f_exitcode = 0;

	// read all lines
	buffer_size = 1000;
	buffer = NULL;
	lineno = 0;
	while (!feof(f_par)) {
		// read next line into memory
		do {
			// allocate memory for input line
			if (buffer == NULL) {
				buffer = malloc(buffer_size);
			}
			if (buffer == NULL) {
				exitcode = 1;
				printf("parse_par_file: can not allocate %i bytes\n", (int) buffer_size);
				break;
			}

			line = fgets(buffer, buffer_size, f_par);
			if (line == NULL) {
				exitcode = ferror(f_par);
				if (exitcode) {
					printf("parse_par_file: %s\n", strerror(exitcode));
				}
				break;
			}

			// if buffer was not completely filled, then assume that line is complete
			count = strlen(buffer) + 1;
			if (count-- < buffer_size) {
				break;
			}

			// otherwise....

			// reset file position to line start
			value = fseek(f_par, -count, SEEK_CUR);
			if (value == -1) {
				exitcode = errno;
				printf("parse_par_file: %s\n", strerror(exitcode));
				break;
			}

			// double buffer size
			free(buffer);
			buffer = NULL;
			buffer_size *= 2;
			lprintf(DEBUG_LVL2, " extending buffer to %i bytes\n", buffer_size);
		} while (1);
		if (line == NULL || exitcode) {
			break;
		}

		lineno++;	// increase line number

		lprintf(DEBUG_LVL2, " line %i (%i) %s", lineno, count, line);

		value = 0;
		mtd = NULL;

		// split line if starting with a colon
		switch (line[0]) {
			case ':':
				count = sscanf(line, ":%255s %i %255s", string1, &value, string2);
				if (count != 3) {
					printf("line %i does not meet defined format (:<mtdname> <mtdsize> <file>)\n", lineno);
				} else {
					// populate mtd_info if supported mtd names
					if (strcmp(string1, mtd_kernel.name) == 0) {
						mtd = &mtd_kernel;
					} else if (strcmp(string1, mtd_rootfs.name) == 0) {
						mtd = &mtd_rootfs;
					} else if (strcmp(string1, mtd_uboot.name) == 0) {
						mtd = &mtd_uboot;
					}

					if (mtd == NULL) {
						printf("unknown mtd %s in line %i\n", string1, lineno);
					} else if (mtd->filename != NULL) {
						f_exitcode = 1;
						printf("mtd %s in line %i multiple definitions\n", string1, lineno);
					} else {
						mtd->size = value;
						mtd->filename = strdup(string2);

						// Get file size
						f_in = fopen(mtd->filename, "rb");
						if (f_in == NULL) {
							f_exitcode = errno;
							printf("input file %s: %s\n", mtd->filename, strerror(f_exitcode));
						} else {
							value = fread(&mtd->magic, 1, 2, f_in);
							if (value < 2) {
								if (ferror(f_in)) {
									f_exitcode = ferror(f_in);
									printf("input file %s: %s\n", mtd->filename, strerror(f_exitcode));
								} else {
									f_exitcode = 1;
									printf("input file %s: smaller than two bytes, no magic code\n", mtd->filename);
								}
							}

							value = fseek(f_in, 0, SEEK_END);
							if (value == -1) {
								f_exitcode = errno;
								printf("input file %s: %s\n", mtd->filename, strerror(f_exitcode));
							} else {
								mtd->filesize = ftell(f_in);
								if (mtd->filesize == -1) {
									f_exitcode = errno;
									printf("input file %s: %s\n", mtd->filename, strerror(f_exitcode));
								}
							}

							fclose(f_in);
						}

						lprintf(DEBUG, "mtd %s in line %i: size=0x%08X, filesize=0x%08lX, magic=0x%02X%02X, file=%s\n", mtd->name, lineno, mtd->size, mtd->filesize, mtd->magic[0], mtd->magic[1], mtd->filename);
					}
				}
				break;
			case '#':	// integer values
				count = sscanf(line, "#%255s %i", string1, &value);
				if (count != 2) {
					printf("line %i does not meet defined format (:<variable name> <integer>\n", lineno);
				} else {
					if (strcmp(string1, "version") == 0) {
						// changing version
						fw_version[0] = 0x000000FF & ( value >> 8 );
						fw_version[1] = 0x000000FF &   value;
					} else {
						printf("unknown integer variable %s in line %i\n", string1, lineno);
					}

					lprintf(DEBUG, "integer variable %s in line %i: 0x%08X\n", string1, lineno, value);
				}
				break;
			case '$':	// strings
				count = sscanf(line, "$%255s %255s", string1, string2);
				if (count != 2) {
					printf("line %i does not meet defined format (:<mtdname> <mtdsize> <file>)\n", lineno);
				} else {
/*
					if (strcmp(string1, "something") == 0) {
						something = string2;
						string2 = NULL;	// do not free
					} else {
*/
						printf("unknown string variable %s in line %i\n", string1, lineno);
//					}
					lprintf(DEBUG, "string variable %s in line %i: %s\n", string1, lineno, string2);
				}
				break;
			default:
				break;
		}
	}
	free(buffer);

	if (!exitcode) {
		exitcode = f_exitcode;
	}

	return exitcode;
}


int create_bin_file(char *bin_filename) {
	int exitcode = 0;

	unsigned char *buffer;

	int i;
	mtd_info *mtd;
	int addsize;
	int padsize;

	char *rand_filename = "/dev/urandom";
	FILE *f_in;
	int size;

	unsigned long int csum;
	unsigned char checksum;

	FILE *f_out;

	// allocate memory for bin file
	buffer = malloc(KERNEL_CODE_OFFSET + FLASH_SIZE);
	if (buffer == NULL) {
		exitcode = 1;
		printf("create_bin_file: can not allocate %i bytes\n", FLASH_SIZE);
	} else {
		// initialize with zero
		memset(buffer, 0, KERNEL_CODE_OFFSET + FLASH_SIZE);
	}

	// add files
	if (!exitcode) {
		for (i = 1; i <= 3; i++) {
			addsize = 0;
			padsize = 0;

			switch (i) {
				case 1:
					mtd = &mtd_kernel;
					break;
				case 2:
					mtd = &mtd_rootfs;
					addsize = mtd->filesize;
					padsize = ROOTFS_MIN_OFFSET - mtd_kernel.size - mtd->filesize;
					break;
				case 3:
					mtd = &mtd_uboot;
					addsize = mtd->filesize;
					break;
				default:
					mtd = NULL;
					exitcode = 1;
					printf("create_bin_file: unknown mtd %i\n", i);
					break;
			}
			if (mtd == NULL) {
				break;
			}
			if (mtd->filename == NULL) {
				continue;
			}

			lprintf(DEBUG, "adding mtd %s file %s\n", mtd->name, mtd->filename);

			// adding file size
			if (addsize) {
				buffer[KERNEL_CODE_OFFSET + mtd->offset - 16] = 0x000000FFL & ( addsize >> 24 );
				buffer[KERNEL_CODE_OFFSET + mtd->offset - 15] = 0x000000FFL & ( addsize >> 16 );
				buffer[KERNEL_CODE_OFFSET + mtd->offset - 14] = 0x000000FFL & ( addsize >> 8  );
				buffer[KERNEL_CODE_OFFSET + mtd->offset - 13] = 0x000000FFL &   addsize;
			}

			// adding file content
			f_in = fopen(mtd->filename, "rb");
			if (f_in == NULL) {
				exitcode = errno;
				printf("input file %s: %s\n", mtd->filename, strerror(exitcode));
			} else {
				size = fread(&buffer[KERNEL_CODE_OFFSET + mtd->offset], mtd->filesize, 1, f_in);
				if (size < 1) {
					if (ferror(f_in)) {
						exitcode = ferror(f_in);
						printf("input file %s: %s\n", mtd->filename, strerror(exitcode));
					} else {
						exitcode = 1;
						printf("input file %s: smaller than before *doh*\n", mtd->filename);
					}
				}
				fclose(f_in);
			}

			// padding
			if (padsize > 0) {
				printf("mtd %s input file %s is too small (0x%08lX), adding 0x%08X random bytes\n", mtd->name, mtd->filename, mtd->filesize, padsize);

				addsize = padsize & 0x0000FFFF;	// start on next 64KB border
				padsize -= addsize;
				addsize += KERNEL_CODE_OFFSET + mtd->offset + mtd->filesize;	// get offset
				lprintf(DEBUG, " padding offset 0x%08X length 0x%08X\n", addsize, padsize);

				f_in = fopen(rand_filename, "rb");
				if (f_in == NULL) {
					exitcode = errno;
					printf("input file %s: %s\n", rand_filename, strerror(exitcode));
				} else {
					size = fread(&buffer[addsize], padsize, 1, f_in);
					if (size < 1) {
						if (ferror(f_in)) {
							exitcode = ferror(f_in);
							printf("input file %s: %s\n", rand_filename, strerror(exitcode));
						} else {
							exitcode = 1;
							printf("input file %s: smaller than before *doh*\n", rand_filename);
						}
					}
				}
				fclose(f_in);
			}
		}
	}

	// add special contents
	if (!exitcode) {
		lprintf(DEBUG, "adding rootfs special data\n");
		memcpy(&buffer[KERNEL_CODE_OFFSET + PRODUCT_ID_OFFSET], product_id, 2);
		memcpy(&buffer[KERNEL_CODE_OFFSET + PROTOCOL_ID_OFFSET], protocol_id, 2);
		memcpy(&buffer[KERNEL_CODE_OFFSET + FW_VERSION_OFFSET], fw_version, 2);
		memcpy(&buffer[KERNEL_CODE_OFFSET + FW_VERSION_OFFSET + 2], rootfs_unknown, 2);
		memcpy(&buffer[KERNEL_CODE_OFFSET + SIGN_OFFSET], sign, 8);	// eRcOmM

		lprintf(DEBUG, "adding u-boot special data 1/2\n");	// ToDo: or after creating the checksum byte?
//		memcpy(&buffer[KERNEL_CODE_OFFSET + SN_OFF], sn, 12);	// ToDo: find out what's this for?
//		memcpy(&buffer[KERNEL_CODE_OFFSET + PIN_OFF], pin, 8);	// ToDo: find out what's this for?
//		memcpy(&buffer[KERNEL_CODE_OFFSET + NODE_BASE_OFF], node, 25);	// ToDo: find out what's this for?

		lprintf(DEBUG, "adding checksum byte\n");
		csum = 0;
		for (i = 0; i < KERNEL_CODE_OFFSET + FLASH_SIZE; i++) {
			csum += buffer[i];
		}
		lprintf(DEBUG_LVL2, " checksum 0x%016lX (%li)\n", csum, csum);

		buffer[KERNEL_CODE_OFFSET + NODE_BASE_OFF + 25] = ~(csum+108)+1;
		lprintf(DEBUG, " byte 0x%02X\n", buffer[KERNEL_CODE_OFFSET + NODE_BASE_OFF + 25]);

		lprintf(DEBUG, "adding u-boot special data 2/2\n");
		memcpy(&buffer[KERNEL_CODE_OFFSET + BOOT_ADDR_BASE_OFF + PID_OFFSET], pid, 70);	// sErCoMm
		memcpy(&buffer[KERNEL_CODE_OFFSET + BOOT_ADDR_BASE_OFF + PID_OFFSET + 57], fw_version, 2);
	}

	// write bin file
	if (!exitcode) {
		lprintf(DEBUG, "writing file %s\n", bin_filename);
		f_out = fopen(bin_filename, "wb");
		if (f_out == NULL) {
			exitcode = errno;
			printf("output file %s: %s\n", bin_filename, strerror(exitcode));
		} else {
			size = fwrite(buffer, KERNEL_CODE_OFFSET + FLASH_SIZE, 1, f_out);
			if (size < 1) {
				if (ferror(f_out)) {
					exitcode = ferror(f_out);
					printf("output file %s: %s\n", bin_filename, strerror(exitcode));
				} else {
					exitcode = 1;
					printf("output file %s: unspecified write error\n", bin_filename);
				}
			}
			fclose(f_out);
		}
	}

	return exitcode;
}


int create_zip_file(char *zip_filename, char *bin_filename) {
	int exitcode = 0;

	char *buffer;
	size_t buffer_size;
	int count;

	buffer_size = 1000;
	buffer = NULL;
	do {
		// allocate memory for command line
		if (buffer == NULL) {
			buffer = malloc(buffer_size);
		}
		if (buffer == NULL) {
			exitcode = 1;
			printf("create_zip_file: can not allocate %i bytes\n", (int) buffer_size);
			break;
		}

		// if buffer was not completely filled, then line fit in completely
		count = snprintf(buffer, buffer_size, "zip %s %s", zip_filename, bin_filename);
		if (count > -1 && count < buffer_size) {
			break;
		}

		// otherwise try again with more space
		if (count > -1) {	// glibc 2.1
			buffer_size = count + 1;	// precisely what is needed
		} else {	// glibc 2.0
			buffer_size *= 2;	// twice the old size
		}
		free(buffer);
		buffer = NULL;
		lprintf(DEBUG_LVL2, " extending buffer to %i bytes\n", buffer_size);
	} while (1);

	if (!exitcode) {
		// zipping binfile
		lprintf(DEBUG, "%s\n", buffer);
		count = system(buffer);
		if (count < 0 || WEXITSTATUS(count)) {
			exitcode = 1;
			printf("create_zip_file: can not execute %s bytes\n", buffer);
		}
	}

	return exitcode;
}


int create_img_file(FILE *f_out, char *out_filename, char *zip_filename) {
	int exitcode = 0;

	md5_state_t state;
	md5_byte_t digest[16];

	int i;
	int size;

	FILE *f_in;
	unsigned char buffer[1];

	// copy firmware version
	memcpy(&img_hdr[50], fw_version, 2);

	// clear md5 checksum
	memset(&img_hdr[480], 0, 16);

	// prepare md5 checksum calculation
	md5_init(&state);

	// add img header
	lprintf(DEBUG_LVL2, " adding img header\n");
	for (i = 0; i < 512; i++) {
		size = fputc(img_hdr[i], f_out);
		if (size == EOF) {
			exitcode = ferror(f_out);
			printf("output file %s: %s\n", out_filename, strerror(exitcode));
			break;
		}
		md5_append(&state, (const md5_byte_t *)&img_hdr[i], 1);
	}

	// adding zip file
	if (!exitcode) {
		lprintf(DEBUG_LVL2, " adding zip file\n");
		f_in = fopen(zip_filename, "rb");
		if (f_in == NULL) {
			exitcode = errno;
			printf("input file %s: %s\n", zip_filename, strerror(exitcode));
		} else {
			while ((size = fgetc(f_in)) != EOF) {
				buffer[0] = size;

				size = fputc(buffer[0], f_out);
				if (size == EOF) {
					exitcode = ferror(f_out);
					printf("output file %s: %s\n", out_filename, strerror(exitcode));
					break;
				}
				md5_append(&state, (const md5_byte_t *)buffer, 1);
			}
			if (ferror(f_in)) {
				exitcode = ferror(f_in);
				printf("input file %s: %s\n", zip_filename, strerror(exitcode));
			}
		}

	}

	// add end byte
	if (!exitcode) {
		lprintf(DEBUG_LVL2, " adding img eof byte\n");
		size = fputc(img_eof[0], f_out);
		if (size == EOF) {
			exitcode = ferror(f_out);
			printf("output file %s: %s\n", out_filename, strerror(exitcode));
		}
		md5_append(&state, (const md5_byte_t *)img_eof, 1);
	}

	// append salt to md5 checksum
	md5_append(&state, (const md5_byte_t *)"A^gU*<>?RFY@#DR&Z", 17);

	// finish md5 checksum calculation
	md5_finish(&state, digest);

	// write md5 checksum into img header
	if (!exitcode) {
		lprintf(DEBUG_LVL2, " writing md5 checksum into img header of file\n");

		size = fseek(f_out, 480, SEEK_SET);
		if (size == -1) {
			exitcode = errno;
			printf("output file %s: %s\n", out_filename, strerror(exitcode));
		} else {
			size = fwrite(digest, 16, 1, f_out);
			if (size < 1) {
				if (ferror(f_out)) {
					exitcode = ferror(f_out);
					printf("output file %s: %s\n", out_filename, strerror(exitcode));
				} else {
					exitcode = 1;
					printf("output file %s: unspecified write error\n", out_filename);
				}
			}
		}

		fclose(f_in);
	}

	return exitcode;
}


int main(int argc, char *argv[]) {
	int exitcode = 0;

	int help;
	int havezip;
	char option;
	char *par_filename = NULL;
	char *out_filename = NULL;
	char *base_filename = NULL;
	char *bin_filename = NULL;
	char *zip_filename = NULL;

	FILE *f_par;
	FILE *f_out;

	int i;
	mtd_info *mtd;
	int mandatory;
	int noupdate;
	int sizecheck;
	unsigned char magic[2];


// display program header
	printf(program_info, VERSION);


// command line processing
	// options
	help = 0;
	havezip = 0;
	while ((option = getopt(argc, argv, ":hzf:v")) != -1) {
		switch(option) {
			case 'h':
				help = 1;
				break;
			case 'z':
				havezip = 1;
				break;
			case 'f':
				sizecheck = sscanf(optarg, "%i", &i);
				if (sizecheck != 1) {
					printf("firmware version of -f option not a valid integer\n");
					exitcode = 1;
				} else {
					fw_version[0] = 0x000000FF & ( i >> 8 );
					fw_version[1] = 0x000000FF &   i;
				}
				break;
			case 'v':
				verbosity++;
				break;
			case ':':	// option with missing operand
				printf("Option -%c requires an operand\n", optopt);
				exitcode = 1;
				break;
			case '?':
				printf("Unrecognized option: -%c\n", optopt);
				exitcode = 1;
				break;
		}
	}

	// files
	for ( ; optind < argc; optind++) {
		if (par_filename == NULL) {
			par_filename = argv[optind];

			if (access(par_filename, R_OK)) {
				if (havezip) {
					printf("No read access to zip file %s\n", par_filename);
				} else {
					printf("No read access to parameter or zip file %s\n", par_filename);
				}
				exitcode = 1;
			}

			continue;
		}

		if (out_filename == NULL) {
			out_filename = argv[optind];

			if (!access(out_filename, F_OK)) {	// if file already exists then check write access
				if (access(out_filename, W_OK)) {
					printf("No write access to output file %s\n", out_filename);
					exitcode = 1;
				}
			}

			continue;
		}

		printf("Too many files stated\n");
		exitcode = 1;
		break;
	}

	// file name checks
	if (par_filename == NULL) {
		if (havezip) {
			printf("Zip file not stated\n");
		} else {
			printf("Parameter file not stated\n");
		}
		exitcode = 1;
	} else {
		base_filename = basename(par_filename);
		if (base_filename == NULL) {
			if (havezip) {
				printf("Zip file is a directory\n");
			} else {
				printf("Parameter file is a directory\n");
			}
			exitcode = 1;
		}
	}

	if (out_filename == NULL) {
		printf("Output file not stated\n");
		exitcode = 1;
	} else {
		base_filename = basename(out_filename);
		if (base_filename == NULL) {
			printf("Output file is a directory\n");
			exitcode = 1;
		} else {
			base_filename = strdup(base_filename);
			zip_filename = strrchr(base_filename, '.');
			if (zip_filename != NULL) {
				zip_filename[0] = 0;
				zip_filename = NULL;	// clean up
			}
		}
	}

	// react on parameter problems or help request, and exit
	if ((exitcode != 0) || help) {
		if (help) {
			printf("This program creates Linksys style images for the WRT350Nv2 router.\n");
		}
		printf("  Usage:\n\
  %s [-h] [-z] [-f <version>] [-v] <parameter or zip file> <output file>\n\n\
  Options:\n\
  -h            -  Show this help\n\
  -z            -  Have zip file, the img file will be directly created from it\n\
  -f <version>  -  Wanted firmware version to use with -z\n\
                   Default firmware version is 0x2019 = 2.00.19.\n\
                   Note: version from parameter file will supersede this\n\
  -v            -  Increase debug verbosity level\n\n\
  Example:\n\
  %s wrt350nv2.par wrt350nv2.img\n\n", argv[0], argv[0]);
		return exitcode;
	}

	// handle special case when zipfile is stated
	if (havezip) {
		zip_filename = par_filename;
		par_filename = NULL;
	}

	lprintf(DEBUG_LVL2, " verbosity: %i\n", verbosity);
	lprintf(DEBUG_LVL2, " program: %s\n", argv[0]);

	lprintf(DEBUG, "Parameter file: %s\n", par_filename);
	lprintf(DEBUG, "Output file: %s\n", out_filename);
	lprintf(DEBUG_LVL2, " basename: %s (%i)\n", base_filename, strlen(base_filename));


// open files from command line
	// parameter file
	if (par_filename != NULL) {
		f_par = fopen(par_filename, "r");
		if (f_par == NULL) {
			exitcode = errno;
			printf("input file %s: %s\n", par_filename, strerror(exitcode));
		}
	}

	// output file
	f_out = fopen(out_filename, "w");
	if (f_out == NULL) {
		exitcode = errno;
		printf("Output file %s: %s\n", out_filename, strerror(exitcode));
	}

	if (exitcode) {
		return exitcode;
	}


// parameter file processing
	if (!exitcode && par_filename != NULL) {
		lprintf(DEBUG, "parsing parameter file...\n");

		exitcode = parse_par_file(f_par);

		lprintf(DEBUG, "...done parsing file\n");
	}
	if (par_filename != NULL) {
		fclose(f_par);
	}


// check all input data
	if (!exitcode && par_filename != NULL) {
		lprintf(DEBUG, "checking mtd data...\n");

		for (i = 1; i <= 3; i++) {
			mandatory = 0;
			noupdate = 0;
			sizecheck = 0;
			magic[0] = 0;
			magic[1] = 0;

			switch (i) {
				case 1:
					mtd = &mtd_kernel;
					mandatory = 1;
					sizecheck = mtd_kernel.size - 16;
					magic[0] = 0x27;
					magic[1] = 0x05;
					break;
				case 2:
					mtd = &mtd_rootfs;
					mtd->offset = mtd_kernel.size;
					mtd->size = ROOTFS_END_OFFSET - mtd_kernel.size;
					mandatory = 1;
					sizecheck = PRODUCT_ID_OFFSET - mtd_kernel.size;
					magic[0] = 0x68;
					magic[1] = 0x73;
					break;
				case 3:
					mtd = &mtd_uboot;
					mtd->offset = BOOT_ADDR_BASE_OFF;
					noupdate = 1;
					sizecheck = SN_OFF - BOOT_ADDR_BASE_OFF;
					break;
				default:
					mtd = NULL;
					exitcode = 1;
					printf("unknown mtd check %i\n", i);
					break;
			}
			if (mtd == NULL) {
				break;
			}

			lprintf(DEBUG_LVL2, " checking mtd %s\n", mtd->name);

			// general checks
			if (mandatory && mtd->filename == NULL) {
				exitcode = 1;
				printf("mtd %s not specified correctly or at all in parameter file\n", mtd->name);
			}

			// end checks if no file data present
			if (mtd->filename == NULL) {
				continue;
			}

			// not updated by stock firmware
			if (noupdate) {
				printf("mtd %s is specified, but will not be updated as of Linksys firmware 2.0.19\n", mtd->name);
			}

			// general magic number check
			if (magic[0]) {
				if (mtd->magic[0] != magic[0] || mtd->magic[1] != magic[1]) {
					exitcode = 1;
					printf("mtd %s input file %s has wrong magic number (0x%02X%02X)\n", mtd->name, mtd->filename, mtd->magic[0], mtd->magic[1]);
				}
			}

			// mtd specific size check
			if (mtd == &mtd_kernel) {
				if (mtd->filesize < 0x00050000) {
					exitcode = 1;
					printf("mtd %s input file %s too unrealistic small (0x%08lX)\n", mtd->name, mtd->filename, mtd->filesize);
				}
			}

			// general size check
			if (sizecheck) {
				if (sizecheck <= 0) {
					exitcode = 1;
					printf("mtd %s bad file size check (%i) due to input data\n", mtd->name, sizecheck);
				} else {
					if (mtd->filesize > sizecheck) {
						exitcode = 1;
						printf("mtd %s input file %s too big (0x%08lX)\n", mtd->name, mtd->filename, mtd->filesize);
					}
				}
			}
		}
		lprintf(DEBUG, "...done checking mtd data\n");
	}


// bin creation in memory
	if (!exitcode && par_filename != NULL) {
		// create bin name from basename
		bin_filename = malloc(strlen(base_filename)+10);
		sprintf(bin_filename, "%s.bin", base_filename);

		lprintf(DEBUG, "creating bin file %s...\n", bin_filename);

		exitcode = create_bin_file(bin_filename);

		lprintf(DEBUG, "...done creating bin file\n");
	}


// zip file creation
	if (!exitcode && zip_filename == NULL) {
		// create zip name from basename
		zip_filename = malloc(strlen(base_filename)+10);
		sprintf(zip_filename, "%s.zip", base_filename);

		lprintf(DEBUG, "creating zip file %s...\n", zip_filename);

		exitcode = create_zip_file(zip_filename, bin_filename);

		lprintf(DEBUG, "...done creating zip file\n");
	}


// img file creation
	if (!exitcode) {
		lprintf(DEBUG, "creating img file...\n");

		exitcode = create_img_file(f_out, out_filename, zip_filename);

		lprintf(DEBUG, "...done creating img file\n");
	}
	fclose(f_out);


// end program
	return exitcode;
}
