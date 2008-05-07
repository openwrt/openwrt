/*
 * Copyright (C) 2007 Ubiquiti Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <zlib.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "fw.h"

typedef struct part_data {
	char 	partition_name[64];
	int  	partition_index;
	u_int32_t	partition_baseaddr;
	u_int32_t	partition_memaddr;
	u_int32_t	partition_entryaddr;
	u_int32_t  partition_length;

	char	filename[PATH_MAX];
	struct stat stats;
} part_data_t;

#define MAX_SECTIONS	8
#define DEFAULT_OUTPUT_FILE 	"firmware-image.bin"
#define DEFAULT_VERSION		"UNKNOWN"

#define OPTIONS "hv:o:i:"

static int debug = 0;

typedef struct image_info {
	char version[256];
	char outputfile[PATH_MAX];
	u_int32_t	part_count;
	part_data_t parts[MAX_SECTIONS]; 
} image_info_t;

static void write_header(void* mem, const char* version)
{
	header_t* header = mem;
	memset(header, 0, sizeof(header_t));

	memcpy(header->magic, MAGIC_HEADER, MAGIC_LENGTH);
	strncpy(header->version, version, sizeof(header->version));
	header->crc = htonl(crc32(0L, (unsigned char *)header,
				sizeof(header_t) - 2 * sizeof(u_int32_t)));
	header->pad = 0L;
}


static void write_signature(void* mem, u_int32_t sig_offset)
{
	/* write signature */
	signature_t* sign = (signature_t*)(mem + sig_offset);
	memset(sign, 0, sizeof(signature_t));

	memcpy(sign->magic, MAGIC_END, MAGIC_LENGTH);
	sign->crc = htonl(crc32(0L,(unsigned char *)mem, sig_offset));
	sign->pad = 0L;
}

static int write_part(void* mem, part_data_t* d)
{
	char* addr;
	int fd;
	part_t* p = mem;
	part_crc_t* crc = mem + sizeof(part_t) + d->stats.st_size;

	fd = open(d->filename, O_RDONLY);
	if (fd < 0)
	{
		ERROR("Failed opening file '%s'\n", d->filename);
		return -1;
	}

	if ((addr=(char*)mmap(0, d->stats.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		ERROR("Failed mmaping memory for file '%s'\n", d->filename);
		close(fd);
		return -2;
	}

	memcpy(mem + sizeof(part_t), addr, d->stats.st_size);
	munmap(addr, d->stats.st_size);

	memset(p->name, 0, sizeof(p->name));
	strncpy(p->magic, MAGIC_PART, MAGIC_LENGTH);
	strncpy(p->name, d->partition_name, sizeof(p->name));
	p->index = htonl(d->partition_index);
	p->data_size = htonl(d->stats.st_size);
	p->part_size = htonl(d->partition_length);
	p->baseaddr = htonl(d->partition_baseaddr);
	p->memaddr = htonl(d->partition_memaddr);
	p->entryaddr = htonl(d->partition_entryaddr);

	crc->crc = htonl(crc32(0L, mem, d->stats.st_size + sizeof(part_t)));
	crc->pad = 0L;

	return 0;
}

static void usage(const char* progname)
{
	INFO("Version %s\n"
             "Usage: %s [options]\n"
	     "\t-v <version string>\t - firmware version information, default: %s\n"
	     "\t-o <output file>\t - firmware output file, default: %s\n"
	     "\t-i <input file>\t\t - firmware layout file, default: none\n"
	     "\t-h\t\t\t - this help\n", VERSION,
	     progname, DEFAULT_VERSION, DEFAULT_OUTPUT_FILE);
}

static void print_image_info(const image_info_t* im)
{
	int i = 0;
	INFO("Firmware version: '%s'\n"
	     "Output file: '%s'\n"
	     "Part count: %u\n",
	     im->version, im->outputfile,
	     im->part_count);

	for (i = 0; i < im->part_count; ++i)
	{
		const part_data_t* d = &im->parts[i];
		INFO(" %10s: %8ld bytes (free: %8ld)\n",
		     d->partition_name,
		     d->stats.st_size,
		     d->partition_length - d->stats.st_size);
	}
}



/**
 * Image layout file format:
 *
 * <partition name>\t<partition index>\t<partition size>\t<data file name>
 *
 */
static int parse_image_layout(const char* layoutfile, image_info_t* im)
{
	int fd = 0;
	char line[1028];
	FILE* f;

	im->part_count = 0;

	fd = open(layoutfile, O_RDONLY);
	if (fd < 0) {
		ERROR("Could not open file '%s'\n", layoutfile);
		return -1;
	}

	f = fdopen(fd, "r");
	if (f == NULL) {
		close(fd);
		return -2;
	}

	while (!feof(f))
	{
		char name[32];
		u_int32_t index;
		u_int32_t baseaddr;
		u_int32_t size;
		u_int32_t memaddr;
		u_int32_t entryaddr;
		char file[PATH_MAX];
		u_int32_t c;
		part_data_t* d;

		if (fgets(line, sizeof(line), f) == NULL)
			break;

		// TODO: very inconvenient format, use smarter parsing someday
		if ((c = sscanf(line, "%32[^\t]\t%X\t%X\t%X\t%X\t%X\t%128[^\t\n]", name, &index, &baseaddr, &size, &memaddr, &entryaddr, file)) != 7)
		    	continue;

		DEBUG("%s\t\t0x%02X\t0x%08X\t0x%08X\t0x%08X\t0x%08X\t%s\n", name, index, baseaddr, size, memaddr, entryaddr, file);

		c = im->part_count;
		if (c == MAX_SECTIONS)
			break;

		d = &im->parts[c];
		strncpy(d->partition_name, name, sizeof(d->partition_name));
		d->partition_index = index;
		d->partition_baseaddr = baseaddr;
		d->partition_length = size;
		d->partition_memaddr = memaddr;
		d->partition_entryaddr = entryaddr;
		strncpy(d->filename, file, sizeof(d->filename));

		im->part_count++;
	}

	fclose(f);

	return 0;
}

/**
 * Checks the availability and validity of all image components. 
 * Fills in stats member of the part_data structure.
 */
static int validate_image_layout(image_info_t* im)
{
	int i;

	if (im->part_count == 0 || im->part_count > MAX_SECTIONS)
	{
		ERROR("Invalid part count '%d'\n", im->part_count);
		return -1;
	}

	for (i = 0; i < im->part_count; ++i)
	{
		part_data_t* d = &im->parts[i];
		int len = strlen(d->partition_name);
		if (len == 0 || len > 16)
		{
			ERROR("Invalid partition name '%s' of the part %d\n",
					d->partition_name, i);
			return -1;
		}
		if (stat(d->filename, &d->stats) < 0)
		{
			ERROR("Couldn't stat file '%s' from part '%s'\n",
				       	d->filename, d->partition_name);
			return -2;
		}
		if (d->stats.st_size == 0)
		{
			ERROR("File '%s' from part '%s' is empty!\n",
				       	d->filename, d->partition_name);
			return -3;
		}
		if (d->stats.st_size > d->partition_length) {
			ERROR("File '%s' too big (%d) - max size: 0x%08X (exceeds %lu bytes)\n",
				       	d->filename, i, d->partition_length, 
					d->stats.st_size - d->partition_length);
			return -4;
		}
	}

	return 0;
}

static int build_image(image_info_t* im)
{
	char* mem;
	char* ptr;
	u_int32_t mem_size;
	FILE* f;
	int i;

	// build in-memory buffer
	mem_size = sizeof(header_t) + sizeof(signature_t);
	for (i = 0; i < im->part_count; ++i)
	{
		part_data_t* d = &im->parts[i];
		mem_size += sizeof(part_t) + d->stats.st_size + sizeof(part_crc_t);
	}

	mem = (char*)calloc(mem_size, 1);
	if (mem == NULL)
	{
		ERROR("Cannot allocate memory chunk of size '%u'\n", mem_size);
		return -1;
	}

	// write header
	write_header(mem, im->version);
	ptr = mem + sizeof(header_t);
	// write all parts
	for (i = 0; i < im->part_count; ++i)
	{
		part_data_t* d = &im->parts[i];
		int rc;
		if ((rc = write_part(ptr, d)) != 0)
		{
			ERROR("ERROR: failed writing part %u '%s'\n", i, d->partition_name);
		}
		ptr += sizeof(part_t) + d->stats.st_size + sizeof(part_crc_t);
	}
	// write signature
	write_signature(mem, mem_size - sizeof(signature_t));

	// write in-memory buffer into file
	if ((f = fopen(im->outputfile, "w")) == NULL)
	{
		ERROR("Can not create output file: '%s'\n", im->outputfile);
		return -10;
	}

	if (fwrite(mem, mem_size, 1, f) != 1)
	{
		ERROR("Could not write %d bytes into file: '%s'\n", 
				mem_size, im->outputfile);
		return -11;
	}

	free(mem);
	fclose(f);
	return 0;
}


int main(int argc, char* argv[])
{
	char inputfile[PATH_MAX];
	int o, rc;
	image_info_t im;

	memset(&im, 0, sizeof(im));
	memset(inputfile, 0, sizeof(inputfile));

	strcpy(im.outputfile, DEFAULT_OUTPUT_FILE);
	strcpy(im.version, DEFAULT_VERSION);

	while ((o = getopt(argc, argv, OPTIONS)) != -1)
	{
		switch (o) {
		case 'v':
			if (optarg)
				strncpy(im.version, optarg, sizeof(im.version));
			break;
		case 'o':
			if (optarg)
				strncpy(im.outputfile, optarg, sizeof(im.outputfile));
			break;
		case 'i':
			if (optarg)
				strncpy(inputfile, optarg, sizeof(inputfile));
			break;
		case 'h':
			usage(argv[0]);
			return -1;
		}
	}

	if (strlen(inputfile) == 0)
	{
		ERROR("Input file is not specified, cannot continue\n");
		usage(argv[0]);
		return -2;
	}

	if ((rc = parse_image_layout(inputfile, &im)) != 0)
	{
		ERROR("Failed parsing firmware layout file '%s' - error code: %d\n", 
			inputfile, rc);
		return -3;
	}

	if ((rc = validate_image_layout(&im)) != 0)
	{
		ERROR("Failed validating firmware layout - error code: %d\n", rc);
		return -4;
	}

	print_image_info(&im);

	if ((rc = build_image(&im)) != 0)
	{
		ERROR("Failed building image file '%s' - error code: %d\n", im.outputfile, rc);
		return -5;
	}

	return 0;
}
