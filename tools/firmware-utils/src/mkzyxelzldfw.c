// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2020 Vincent Wiemann <vw@derowe.com>
 *
 * This program is derived from ZyXEL's GPL U-Boot code.
 * Copyright (C) 2011-2011 ZyXEL Communications, Corp.
 */

#define _POSIX_SOURCE
#define _POSIX_C_SOURCE 199309L /* getopt */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include "md5.h"

#define ZYXEL_MAGIC	0xdeadbeaf
#define MAX_MODELS	5
#define CHECKSUM_SIZE	sizeof(uint32_t)
#define MAX_FILES	32
#define MAX_FILENAME	64

#define DATE_SIZE	32
#define REV_SIZE	32

#define error(fmt, ...)                                                        \
	do {                                                                   \
		printf("Error: " fmt, ##__VA_ARGS__);                          \
		exit(1);                                                       \
	} while (0)

enum {
	FILE_TYPE_BM = 1,
	FILE_TYPE_KERNEL,
	FILE_TYPE_CORE,
	FILE_TYPE_DB,
	FILE_TYPE_CONF,
	FILE_TYPE_WTP
};

struct file_type_tbl {
	int type;
	char *str;
};

struct file_type_tbl file_types[] = {
	{ FILE_TYPE_BM, "bm" },	    { FILE_TYPE_KERNEL, "kernel" },
	{ FILE_TYPE_CORE, "core" }, { FILE_TYPE_DB, "db" },
	{ FILE_TYPE_CONF, "conf", }, { FILE_TYPE_WTP, "wtp" }
};
static int FILE_TYPE_COUNT = sizeof(file_types) / sizeof(struct file_type_tbl);

struct fw_header {
	uint32_t checksum; /* MD5-based checksum */
	uint32_t magic; /* 0xdeadbeaf */
	uint16_t version; /* version of the firmware archive */
	uint16_t files_count; /* number of files contained */
	uint16_t models_count; /* number of supported models */
	uint16_t models[MAX_MODELS]; /* supported models' IDs */
	uint32_t total_length; /* total length of the firmware archive */
	uint16_t files_offset; /* offset of the first file */
	uint16_t header_length; /* length of this header */
	uint16_t info_length; /* length of the file information header */
	uint16_t __padding1[3]; /* reserved for future use */
	char capwap_version[32]; /* e.g. "undefined" */
	char model_name[32]; /* e.g. "NWA512X-FAT" */
} __attribute__((packed));

struct fw_header_file {
	uint16_t type; /* file type (e.g. FILE_TYPE_KERNEL) */
	uint16_t flags; /* unknown */
	uint32_t length; /* length of the file */
	uint32_t checksum; /* checksum of the file */
	uint32_t flash_offset; /* write offset from beginning of flash */
	char filename[MAX_FILENAME]; /* original file name */
	char target[128]; /* target "file" name (e.g. "kernel", "zldfs") */
	char revision[REV_SIZE]; /* revision string */
	char date[32]; /* creation date string */
} __attribute__((packed));

struct fw_header_kernel {
	char bm_version[64];
	char kernel_version[64];
	char core_version[64];
	char capwap_version[32];
	char model_name[32];
	uint32_t bm_checksum;
	uint32_t kernel_checksum;
	uint32_t zld_checksum;
	uint32_t core_checksum;
	uint16_t max_models;
	uint16_t models[MAX_MODELS];
	char padding[512 - 64 * 4 - 4 * 4 - 2 - MAX_MODELS * 2 - 4];
	uint32_t baudrate;
} __attribute__((packed));

struct firmware_file {
	struct fw_header_file header;
	size_t offset;
	char filepath[PATH_MAX];
};

struct firmware {
	struct fw_header header;
	struct firmware_file *files;
	size_t files_count;
	struct fw_header_kernel kernel_header;
	char lower_endian;
};

static size_t get_file_size(FILE *fp)
{
	size_t pos = (size_t)ftell(fp);
	size_t file_size;

	fseek(fp, 0, SEEK_END);
	file_size = (size_t)ftell(fp);
	fseek(fp, (long int)pos, SEEK_SET);

	return file_size;
}

static void copy_from_to_file(FILE *fp_src, size_t src_offset, FILE *fp_dst,
		       size_t dst_offset, size_t length)
{
	int buf[512];
	size_t len;

	if (src_offset)
		fseek(fp_src, (long int)src_offset, SEEK_SET);

	if (dst_offset)
		fseek(fp_src, (long int)dst_offset, SEEK_SET);

	while (length) {
		if (length >= sizeof(buf)) {
			len = sizeof(buf);
		} else {
			len = length;
		}

		length -= len;

		if (fread(buf, len, 1, fp_src) != 1)
			error("Failed to read");

		if (fwrite(buf, len, 1, fp_dst) != 1)
			error("Failed to write");
	}
}

static void extract_to_file(FILE *fp_src, char *dst, size_t length,
			    size_t offset)
{
	FILE *fp_dst;

	if (!(fp_dst = fopen(dst, "wb")))
		error("Failed to open %s for writing", dst);

	copy_from_to_file(fp_src, offset, fp_dst, 0, length);

	fclose(fp_dst);
}

static void dump_firmware_header(struct fw_header *header_p)
{
	int i;

	printf("FIRMWARE HEADER\n");
	printf("checksum     =   0x%08x, magic          =   0x%08x\n",
	       header_p->checksum, header_p->magic);
	printf("version      =       0x%04x, files_count    = %12d\n",
	       header_p->version, header_p->files_count);
	printf("models_count = %12d, total_length   = %12d\n",
	       header_p->models_count, header_p->total_length);
	printf("files_offset =       0x%04x, header_length  = %12d\n",
	       header_p->files_offset, header_p->header_length);
	printf("info_length  = %12d, capwap_version = %12s\n",
	       header_p->info_length, header_p->capwap_version);
	printf("model_name   = %s\n", header_p->model_name);
	printf("models       =");
	for (i = 0; i < header_p->models_count && i < MAX_MODELS; i++)
		printf(" 0x%04x", header_p->models[i]);
	printf("\n\n");
}

static int get_file_type_id(char *type)
{
	struct file_type_tbl *ft = file_types;
	int i;

	for (i = 0; i < FILE_TYPE_COUNT; i++, ft++)
		if (!strcmp(type, ft->str))
			return ft->type;

	printf("Supported file types:\n");
	for (i = 0, ft = file_types; i < FILE_TYPE_COUNT; i++, ft++)
		printf("%8s (ID 0x%x)\n", ft->str, ft->type);

	error("Unknown file type \"%s\"\n", type);

	return 0;
}

static char *get_file_type_string(int type)
{
	struct file_type_tbl *ft = file_types;
	int i;

	for (i = 0; i < FILE_TYPE_COUNT; i++, ft++)
		if (ft->type == type)
			return ft->str;

	return NULL;
}

static void dump_file_header(struct fw_header_file *file_header)
{
	printf("\nfilename=%s, type=%s, flags=0x%x target=%s, revision=%s\n",
	       file_header->filename, get_file_type_string(file_header->type),
	       file_header->flags, file_header->target, file_header->revision);
	printf("date=%s, length=%u, checksum=0x%08x, flash_offset=0x%08x\n",
	       file_header->date, file_header->length, file_header->checksum,
	       file_header->flash_offset);
}

static void dump_kernel_header(struct fw_header_kernel *kernel_header)
{
	int i;

	printf("KERNEL HEADER (%lu bytes)\n", sizeof(struct fw_header_kernel));
	printf("bm_version = %s\n", kernel_header->bm_version);
	printf("kernel_version = %s\n", kernel_header->kernel_version);
	printf("core_version = %s\n", kernel_header->core_version);
	printf("capwap_version = %s\n", kernel_header->capwap_version);
	printf("model_name = %s\n", kernel_header->model_name);
	printf("bm_checksum = 0x%08x\n", kernel_header->bm_checksum);
	printf("kernel_checksum = 0x%08x\n", kernel_header->kernel_checksum);
	printf("zld_checksum = 0x%08x\n", kernel_header->zld_checksum);
	printf("core_checksum = 0x%08x\n", kernel_header->core_checksum);
	printf("max_models = %u\n", kernel_header->max_models);
	printf("models =");
	for (i = 0; i < 5; i++)
		printf(" 0x%04x", kernel_header->models[i]);
	printf("\n");
	printf("baudrate = %u\n", kernel_header->baudrate);
	printf("\n");
}

static void translate_fw_header(struct fw_header *header_p)
{
	int i;

	header_p->magic = ntohl(header_p->magic);
	header_p->version = ntohs(header_p->version);
	header_p->files_count = ntohs(header_p->files_count);
	header_p->models_count = ntohs(header_p->models_count);
	for (i = 0; i < MAX_MODELS; i++)
		header_p->models[i] = ntohs(header_p->models[i]);
	header_p->total_length = ntohl(header_p->total_length);
	header_p->files_offset = ntohs(header_p->files_offset);
	header_p->header_length = ntohs(header_p->header_length);
	header_p->info_length = ntohs(header_p->info_length);
}

static void translate_file_header(struct fw_header_file *file_header)
{
	file_header->type = ntohs(file_header->type);
	file_header->flags = ntohs(file_header->flags);
	file_header->length = ntohl(file_header->length);
	file_header->checksum = ntohl(file_header->checksum);
	file_header->flash_offset = ntohl(file_header->flash_offset);
}

static void translate_kernel_header(struct fw_header_kernel *kernel_header)
{
	int i;

	kernel_header->bm_checksum = ntohl(kernel_header->bm_checksum);
	kernel_header->kernel_checksum = ntohl(kernel_header->kernel_checksum);
	/*kernel_header->zld_checksum = ntohl(kernel_header->zld_checksum);*/
	kernel_header->core_checksum = ntohl(kernel_header->core_checksum);

	kernel_header->max_models = ntohs(kernel_header->max_models);
	for (i = 0; i < MAX_MODELS; i++)
		kernel_header->models[i] = ntohs(kernel_header->models[i]);
	kernel_header->baudrate = ntohl(kernel_header->baudrate);
}

static void checksum_add_from_buf(MD5_CTX *ctx, void *buf,
			    size_t length, size_t offset)
{
	char *begin = &((char *)buf)[offset];

	MD5_Update(ctx, begin, length);
}

static void checksum_add_from_file(MD5_CTX *ctx, FILE *fp_src,
			    size_t length, size_t offset)
{
	int buf[512];
	size_t len;

	fseek(fp_src, (long int)offset, SEEK_SET);
	while (length) {
		if (length >= sizeof(buf)) {
			len = sizeof(buf);
		} else {
			len = length;
		}

		length -= len;

		if (fread(buf, len, 1, fp_src) != 1)
			error("Failed to read for checksum calculation");

		checksum_add_from_buf(ctx, buf, len, 0);
	}
}

static uint32_t checksum_finish(MD5_CTX *ctx)
{
	unsigned char md5sum[16];
	uint32_t checksum = 0;
	int i;

	MD5_Final(md5sum, ctx);

	for (i = 0; i < 16; i += 4)
		checksum += ((uint32_t)md5sum[i] << 24 |
			     (uint32_t)md5sum[i + 1] << 16 |
			     (uint32_t)md5sum[i + 2] << 8 |
			     (uint32_t)md5sum[i + 3]);

	if (checksum == 0)
		checksum = 1;

	return checksum;
}

static uint32_t checksum_calculate(FILE *fp, size_t kernel_offset)
{
	struct fw_header_kernel dummy;
	MD5_CTX ctx;
	size_t file_size;

	fseek(fp, 0, SEEK_SET);
	file_size = get_file_size(fp);

	MD5_Init(&ctx);

	checksum_add_from_file(&ctx, fp, kernel_offset - CHECKSUM_SIZE,
			       CHECKSUM_SIZE);

	/* use a zeroed out kernel version header */
	memset(&dummy, 0, sizeof(dummy));
	checksum_add_from_buf(&ctx, &dummy, sizeof(dummy), 0);

	checksum_add_from_file(&ctx, fp,
			       file_size - kernel_offset - sizeof(dummy),
			       kernel_offset + sizeof(dummy));

	return checksum_finish(&ctx);
}

static uint32_t checksum_calculate_file(char *filename)
{
	MD5_CTX ctx;
	FILE *fp;
	size_t file_size;

	if (!(fp = fopen(filename, "rb")))
		error("Failed to open %s for writing\n", filename);

	file_size = get_file_size(fp);

	MD5_Init(&ctx);

	checksum_add_from_file(&ctx, fp, file_size, 0);

	fclose(fp);

	return checksum_finish(&ctx);
}

static void parse_firmware(struct firmware *fw, FILE *fp)
{
	struct firmware_file *file;
	size_t file_size, file_offset, kernel_offset;
	uint32_t checksum = 0;
	int i;

	if (!fw)
		error("Failed to allocate firmware struct\n");

	file_size = get_file_size(fp);

	if (file_size < sizeof(fw->header))
		error("File too small\n");

	if (1 != fread(&fw->header, sizeof(fw->header), 1, fp))
		error("Failed to read firmware header\n");

	if (ntohl(fw->header.magic) == ZYXEL_MAGIC) {
		fw->lower_endian = 1;
		translate_fw_header(&fw->header);
	} else if (fw->header.magic != ZYXEL_MAGIC) {
		error("Unsupported magic. Expected 0x%x, but found 0x%x\n",
		      ZYXEL_MAGIC, fw->header.magic);
	}

	if (fw->header.models_count != MAX_MODELS)
		error("Wrong number of models. Expected %u, but found %u\n",
		      MAX_MODELS, fw->header.models_count);

	dump_firmware_header(&fw->header);

	if (fw->header.total_length != file_size)
		error("File size does not match. Expected %lu, but found %u\n",
		      file_size, fw->header.total_length);

	file_offset = sizeof(fw->header) +
		      fw->header.files_count * sizeof(struct fw_header_file);

	if (file_offset != fw->header.files_offset)
		error("File offset does not match definition in header\n");

	if (file_size < file_offset)
		error("File too small for %u file headers\n",
		      fw->header.files_count);

	if (NULL == (fw->files = malloc(fw->header.files_count *
					sizeof(struct firmware_file))))
		error("Failed to allocate memory for %u file structs\n",
		      fw->header.files_count);

	for (i = 0, file = fw->files; i < fw->header.files_count; i++, file++) {
		if (1 != fread(file, sizeof(struct fw_header_file), 1, fp))
			error("Failed to read file header #%u\n", i + 1);

		if (fw->lower_endian)
			translate_file_header(&file->header);

		if (file_offset + file->header.length > fw->header.total_length)
			error("File offset exceeds size of firmware archive\n");

		if (file->header.type == FILE_TYPE_KERNEL)
			kernel_offset = file_offset;

		file->offset = file_offset;

		file_offset += file->header.length;
	}

	if (!kernel_offset)
		error("Kernel image missing for checksum calculation\n");

	/* as we know the kernel offset, we can calculate the checksum
	 * as it must be excluded from checksum calculation */
	checksum = checksum_calculate(fp, kernel_offset);

	if (fw->lower_endian)
		checksum = ntohl(checksum);

	if (checksum != fw->header.checksum)
		printf("WARN: Checksum mismatch. Calculated 0x%x\n", checksum);

	fseek(fp, (long int)kernel_offset, SEEK_SET);
	if (1 != fread(&fw->kernel_header, sizeof(fw->kernel_header), 1, fp))
		error("Failed to read kernel header\n");

	if (fw->lower_endian)
		translate_kernel_header(&fw->kernel_header);

	dump_kernel_header(&fw->kernel_header);
}

static void extract_firmware(struct firmware *fw, char *filename)
{
	struct firmware_file *file;
	FILE *fp;
	int i;

	if (!(fp = fopen(filename, "rb")))
		error("Failed to open firmware archive for extraction %s\n",
		      filename);

	parse_firmware(fw, fp);

	printf("Extracting files...");

	for (i = 0, file = fw->files; i < fw->header.files_count; i++, file++) {
		dump_file_header(&file->header);
		if (file->header.type == FILE_TYPE_KERNEL) {
			/* strip kernel header */
			extract_to_file(
				fp, file->header.filename,
				file->header.length -
					sizeof(struct fw_header_kernel),
				file->offset + sizeof(struct fw_header_kernel));
		} else {
			extract_to_file(fp, file->header.filename,
					file->header.length, file->offset);
		}

		printf("Calculated file checksum is 0x%08x\n",
		       checksum_calculate_file(file->header.filename));
	}

	free(fw->files);
	fclose(fp);
}

static void init_fw_header(struct firmware *fw)
{
	int i;

	for (i = 0; i < MAX_MODELS; i++)
		fw->header.models[i] = 0xffff;

	fw->kernel_header.baudrate = 115200;
	fw->kernel_header.max_models = MAX_MODELS;
	/* ZyXEL messed up their code and included a 32 bit pointer */
	fw->header.header_length = sizeof(fw->header) + 4;
	fw->header.magic = ZYXEL_MAGIC;
}

static void write_headers(FILE *fp, struct firmware *fw)
{
	struct firmware_file *file;
	unsigned int i;

	fseek(fp, 0, SEEK_SET);

	if (1 != fwrite(&fw->header, sizeof(fw->header), 1, fp))
		error("Failed to write firmware header\n");

	for (i = 0, file = fw->files; i < fw->files_count; i++, file++)
		if (1 !=
		    fwrite(&file->header, sizeof(struct fw_header_file), 1, fp))
			error("Failed to write file header #%u\n", i + 1);
}

static void usage(char *progname)
{
	printf("Usage: %s\n"
	       "  For extraction:\n"
	       "    -e <file>		extract firmware <file>\n\n"
	       "  For creation:\n"
	       "    -v <version>	set hexadecimal firmware <version>\n"
	       "    -b <checksum>	set hexadecimal bootmanager <checksum>\n"
	       "    -c <version>	set capwap <version> string\n"
	       "    -m <model>		set <model> string\n"
	       "    -d <model_id>	set (up to %u) hexadecimal <model_id>\n"
	       "    (multiple input files)\n"
	       "     -i <file>		add input <file>\n"
	       "      -o <offset>	set hexadecimal flash offset for file\n"
	       "      -r <revision>	set revision string for file\n"
	       "      -t <type>		choose file <type>\n"
	       "      -x <target>	set (partition) <target> of file\n"
	       "    <output_filename>\n",
	       progname, MAX_MODELS);
	exit(1);
}

int main(int argc, char *argv[])
{
	struct firmware fw;
	struct firmware_file *file;
	struct firmware_file *core_file = NULL;
	struct stat attr;
	static const char *optstr = "e:v:b:c:m:d:i:o:r:t:x:h";
	const char *capwap_version = "undefined";
	const char *separator = " | ";
	char *filename;
	FILE *fp_src, *fp_dst;
	size_t kernel_offset = 0;
	unsigned int i;
	int opt;

	memset(&fw, 0, sizeof(fw));
	init_fw_header(&fw);

	if (argc < 1)
		usage(argv[0]);

	opt = getopt(argc, argv, optstr);
	if (opt == -1)
		usage(argv[0]);
	while (opt != -1) {
		if (optarg == NULL)
			usage(argv[0]);

		switch (opt) {
		case 'e':
			extract_firmware(&fw, optarg);
			exit(0);
		case 'v':
			fw.header.version = (uint16_t)strtol(optarg, NULL, 16);
			if (!fw.header.version)
				error("Invalid version number\n");
			break;
		case 'b':
			fw.kernel_header.bm_checksum =
				(uint32_t)strtol(optarg, NULL, 16);
			break;
		case 'c':
			strncpy(fw.header.capwap_version, optarg,
				sizeof(fw.header.capwap_version) - 1);
			break;
		case 'm':
			strncpy(fw.header.model_name, optarg,
				sizeof(fw.header.model_name) - 1);
			break;
		case 'd':
			if (fw.header.models_count == MAX_MODELS)
				error("Max. number of supported models is %u\n",
				      MAX_MODELS);

			fw.header.models[fw.header.models_count] =
				(uint16_t)strtol(optarg, NULL, 16);
			fw.header.models_count++;
			break;
		case 'i':
			filename = optarg;

			if (!fw.files &&
			    NULL == (fw.files = malloc(
					     MAX_FILES *
					     sizeof(struct firmware_file))))
				error("Failed to allocate %u file structs\n",
				      MAX_FILES);

			if (fw.files_count == MAX_FILES)
				error("Maximum number of files reached (%u)\n",
				      MAX_FILES);

			if (stat(optarg, &attr))
				error("Stat failed on %s\n", optarg);

			strftime(fw.files[fw.files_count].header.date,
				 DATE_SIZE - 1, "%Y-%m-%d %H:%M:%S",
				 localtime(&attr.st_mtime));

			strncpy(fw.files[fw.files_count].filepath, optarg,
				PATH_MAX - 1);

			filename = strrchr(optarg, '/');
			if (!filename)
				filename = optarg;

			strncpy(fw.files[fw.files_count].header.filename,
				filename, MAX_FILENAME - 1);

			fw.files_count++;
			break;
		case 'o':
			if (!fw.files_count)
				error("Specify offset after filename\n");

			fw.files[fw.files_count - 1].header.flash_offset =
				(uint32_t)strtol(optarg, NULL, 16);
			break;
		case 'r':
			if (!fw.files_count)
				error("Specify file revision after filename\n");

			strncpy(fw.files[fw.files_count - 1].header.revision,
				optarg, REV_SIZE - 1);
			break;
		case 't':
			if (!fw.files_count)
				error("Specify file type after filename!\n");

			fw.files[fw.files_count - 1].header.type =
				get_file_type_id(optarg);
			break;
		case 'x':
			if (!fw.files_count)
				error("Specify file target after filename!\n");
			strncpy(fw.files[fw.files_count - 1].header.target,
				optarg, sizeof(fw.files[0].header.target) - 1);
			break;
		case 'h':
		default:
			usage(argv[0]);
		}

		opt = getopt(argc, argv, optstr);
	}

	if (!fw.header.models_count)
		error("Supported model IDs missing (option -d)\n");

	if (!fw.header.version)
		error("Version number missing (e.g. -v 0x100)\n");

	if (!fw.kernel_header.bm_checksum)
		error("Bootmanager checksum is missing (option -b)\n");

	if (!strlen(fw.header.model_name))
		error("Model name missing (option -m)\n");

	if (!strlen(fw.header.capwap_version))
		strncpy(fw.header.capwap_version, capwap_version,
			sizeof(fw.header.capwap_version) - 1);

	fw.header.models_count = MAX_MODELS;
	fw.header.files_count = fw.files_count;
	memcpy(fw.kernel_header.models, fw.header.models,
	       sizeof(fw.header.models));

	for (i = 0; i < fw.files_count; i++) {
		if (!fw.files[i].header.type)
			error("No file or type specified for file %s\n",
			      fw.files[i].filepath);

		if (!strlen(fw.files[i].header.target))
			error("Target missing for %s (e.g. -x zldfs)\n",
			      fw.files[i].filepath);

		if (!strlen(fw.files[i].header.revision))
			error("Revision missing for %s\n",
			      fw.files[i].filepath);
	}

	filename = argv[optind];
	if (!(fp_dst = fopen(filename, "w+b")))
		error("Failed to open %s for writing\n", filename);

	write_headers(fp_dst, &fw);

	fw.header.info_length = sizeof(struct fw_header_file);
	fw.header.files_offset =
		sizeof(fw.header) + fw.files_count * fw.header.info_length;
	if ((size_t)ftell(fp_dst) != fw.header.files_offset)
		error("Oops. Something went wrong writing the file headers");

	fw.header.total_length = fw.header.files_offset;
	for (i = 0, file = fw.files; i < fw.files_count; i++, file++) {
		file->header.checksum = checksum_calculate_file(file->filepath);

		if (!(fp_src = fopen(file->filepath, "rb")))
			error("Failed to open %s for writing\n", filename);

		file->offset = fw.header.total_length;

		file->header.length = get_file_size(fp_src);

		if (file->header.type == FILE_TYPE_KERNEL)
			if (1 != fwrite(&fw.kernel_header,
					sizeof(fw.kernel_header), 1, fp_dst))
				error("Failed to write kernel header\n");

		copy_from_to_file(fp_src, 0, fp_dst, 0, file->header.length);

		if (file->header.type == FILE_TYPE_KERNEL) {
			file->header.length += sizeof(fw.kernel_header);
			kernel_offset = file->offset;
			fw.kernel_header.kernel_checksum =
				file->header.checksum;
			if (strlen(file->header.revision) + strlen(separator) +
				    strlen(file->header.date) >=
			    sizeof(fw.kernel_header.kernel_version))
				error("Kernel file revision too long\n");

			strcat(fw.kernel_header.kernel_version,
			       file->header.revision);
			strcat(fw.kernel_header.kernel_version, separator);
			strcat(fw.kernel_header.kernel_version,
			       file->header.date);
		} else if (file->header.type == FILE_TYPE_CORE) {
			core_file = file;
		}

		fw.header.total_length += file->header.length;

		translate_file_header(&file->header);

		fclose(fp_src);
	}

	/* update headers with correct lengths and endianness */
	translate_fw_header(&fw.header);

	write_headers(fp_dst, &fw);

	if (!kernel_offset)
		error("Kernel image needed for checksum calculation\n");

	/* update headers with correct checksum */
	fw.header.checksum = htonl(checksum_calculate(fp_dst, kernel_offset));
	fseek(fp_dst, 0, SEEK_SET);
	fwrite(&fw.header.checksum, sizeof(fw.header.checksum), 1, fp_dst);

	fw.kernel_header.zld_checksum = fw.header.checksum;
	strncpy(fw.kernel_header.model_name, fw.header.model_name,
		sizeof(fw.kernel_header.model_name) - 1);
	strncpy(fw.kernel_header.capwap_version, fw.header.capwap_version,
		sizeof(fw.kernel_header.capwap_version) - 1);

	translate_kernel_header(&fw.kernel_header);

	if (core_file) {
		fw.kernel_header.core_checksum = core_file->header.checksum;

		if (strlen(core_file->header.revision) + strlen(separator) +
			    strlen(core_file->header.date) >=
		    sizeof(fw.kernel_header.core_version))
			error("Core file revision string too long\n");

		strcat(fw.kernel_header.core_version,
		       core_file->header.revision);
		strcat(fw.kernel_header.core_version, separator);
		strcat(fw.kernel_header.core_version, core_file->header.date);
	}

	fseek(fp_dst, (long int)kernel_offset, SEEK_SET);
	fwrite(&fw.kernel_header, sizeof(fw.kernel_header), 1, fp_dst);

	fclose(fp_dst);

	exit(0);
}
