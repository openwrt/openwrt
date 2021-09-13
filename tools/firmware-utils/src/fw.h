// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  * Copyright (C) 2007 Ubiquiti Networks, Inc.
 */

#ifndef FW_INCLUDED
#define FW_INCLUDED

#include <stdint.h>
#include <sys/types.h>

#define MAGIC_HEADER	"OPEN"
#define MAGIC_PART	"PART"
#define MAGIC_END	"END."
#define MAGIC_ENDS	"ENDS"

#define MAGIC_LENGTH	4
#define PART_NAME_LENGTH 16

typedef struct header {
	char magic[MAGIC_LENGTH];
	char version[256];
	u_int32_t crc;
	u_int32_t pad;
} __attribute__ ((packed)) header_t;

typedef struct part {
	char magic[MAGIC_LENGTH];
	char name[PART_NAME_LENGTH];
	uint8_t pad[12];
	u_int32_t memaddr;
	u_int32_t index;
	u_int32_t baseaddr;
	u_int32_t entryaddr;
	u_int32_t data_size;
	u_int32_t part_size;
} __attribute__ ((packed)) part_t;

typedef struct part_crc {
	u_int32_t crc;
	u_int32_t pad;
} __attribute__ ((packed)) part_crc_t;

typedef struct signature {
	uint8_t magic[MAGIC_LENGTH];
	u_int32_t crc;
	u_int32_t pad;
} __attribute__ ((packed)) signature_t;

typedef struct signature_rsa {
	uint8_t magic[MAGIC_LENGTH];
//	u_int32_t crc;
	unsigned char rsa_signature[256];
	u_int32_t pad;
} __attribute__ ((packed)) signature_rsa_t;

#define VERSION "1.2"

#define INFO(...) fprintf(stdout, __VA_ARGS__)
#define ERROR(...) fprintf(stderr, "ERROR: "__VA_ARGS__)
#define WARN(...) fprintf(stderr, "WARN: "__VA_ARGS__)
#define DEBUG(...) do {\
        if (debug) \
                fprintf(stdout, "DEBUG: "__VA_ARGS__); \
} while (0);

#endif
